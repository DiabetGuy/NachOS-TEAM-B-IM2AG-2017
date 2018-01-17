// filesys.cc
//	Routines to manage the overall operation of the file system.
//	Implements routines to map from textual file names to files.
//
//	Each file in the file system has:
//	   A file header, stored in a sector on disk
//		(the size of the file header data structure is arranged
//		to be precisely the size of 1 disk sector)
//	   A number of data blocks
//	   An entry in the file system directory
//
// 	The file system consists of several data structures:
//	   A bitmap of free disk sectors (cf. bitmap.h)
//	   A directory of file names and file headers
//
//      Both the bitmap and the directory are represented as normal
//	files.  Their file headers are located in specific sectors
//	(sector 0 and sector 1), so that the file system can find them
//	on bootup.
//
//	The file system assumes that the bitmap and directory files are
//	kept "open" continuously while Nachos is running.
//
//	For those operations (such as Create, Remove) that modify the
//	directory and/or bitmap, if the operation succeeds, the changes
//	are written immediately back to disk (the two files are kept
//	open during all this time).  If the operation fails, and we have
//	modified part of the directory and/or bitmap, we simply discard
//	the changed version, without writing it back to disk.
//
// 	Our implementation at this point has the following restrictions:
//
//	   there is no synchronization for concurrent accesses
//	   files have a fixed size, set when the file is created
//	   files cannot be bigger than about 3KB in size
//	   there is no hierarchical directory structure, and only a limited
//	     number of files can be added to the system
//	   there is no attempt to make the system robust to failures
//	    (if Nachos exits in the middle of an operation that modifies
//	    the file system, it may corrupt the disk)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "disk.h"
#include "bitmap.h"
#include "directory.h"
#include "filehdr.h"
#include "filesys.h"
#include "path.h"


//----------------------------------------------------------------------
// FileSystem::FileSystem
// 	Initialize the file system.  If format = TRUE, the disk has
//	nothing on it, and we need to initialize the disk to contain
//	an empty directory, and a bitmap of free sectors (with almost but
//	not all of the sectors marked as free).
//
//	If format = FALSE, we just have to open the files
//	representing the bitmap and the directory.
//
//	"format" -- should we initialize the disk?
//----------------------------------------------------------------------

FileSystem::FileSystem(bool format)
{
    DEBUG('f', "Initializing the file system.\n");
    if (format) {
        BitMap *freeMap = new BitMap(NumSectors);
        Directory *directory = new Directory(NumDirEntries);
      	FileHeader *mapHdr = new FileHeader;
      	FileHeader *dirHdr = new FileHeader;
        //FileHeader *dirHdr = dirHdrTmp;

        DEBUG('f', "Formatting the file system.\n");

    // First, allocate space for FileHeaders for the directory and bitmap
    // (make sure no one else grabs these!)
      	freeMap->Mark(FreeMapSector);
      	freeMap->Mark(DirectorySector);

    // Second, allocate space for the data blocks containing the contents
    // of the directory and bitmap files.  There better be enough space!

      	ASSERT(mapHdr->Allocate(freeMap, FreeMapFileSize));
      	ASSERT(dirHdr->Allocate(freeMap, DirectoryFileSize));

    // Flush the bitmap and directory FileHeaders back to disk
    // We need to do this before we can "Open" the file, since open
    // reads the file header off of disk (and currently the disk has garbage
    // on it!).

        DEBUG('f', "Writing headers back to disk.\n");
      	mapHdr->WriteBack(FreeMapSector);
      	dirHdr->WriteBack(DirectorySector);

    // OK to open the bitmap and directory files now
    // The file system operations assume these two files are left open
    // while Nachos is running.

        freeMapFile = new OpenFile(FreeMapSector);
        rootDirectoryFile = new OpenFile(DirectorySector);
        directory->AddDirectory(".", DirectorySector);
        directory->AddDirectory("..", DirectorySector);
        currentDirectoryFile = rootDirectoryFile; //At first the root directory is the current one

    // Once we have the files "open", we can write the initial version
    // of each file back to disk.  The directory at this point is completely
    // empty; but the bitmap has been changed to reflect the fact that
    // sectors on the disk have been allocated for the file headers and
    // to hold the file data for the directory and bitmap.

        DEBUG('f', "Writing bitmap and directory back to disk.\n");
      	freeMap->WriteBack(freeMapFile);	 // flush changes to disk
      	directory->WriteBack(rootDirectoryFile);

      	if (DebugIsEnabled('f')) {
      	    freeMap->Print();
      	    directory->Print();
      	}

        delete freeMap;
        delete directory;
        delete mapHdr;
        delete dirHdr;

    } else {
    // if we are not formatting the disk, just open the files representing
    // the bitmap and directory; these are left open while Nachos is running
        freeMapFile = new OpenFile(FreeMapSector);
        rootDirectoryFile = new OpenFile(DirectorySector);
        currentDirectoryFile = rootDirectoryFile;
    }

    indexCache = 0;
    for(int i = 0; i < NumDirEntries; i++) {
        openFileCache[i] = NULL;
    }
}

//----------------------------------------------------------------------
// FileSystem::Create
// 	Create a file in the Nachos file system (similar to UNIX create).
//	Since we can't increase the size of files dynamically, we have
//	to give Create the initial size of the file.
//
//	The steps to create a file are:
//	  Make sure the file doesn't already exist
//        Allocate a sector for the file header
// 	  Allocate space on disk for the data blocks f
//	"initialSize" -- size of file to be createdor the file
//	  Add the name to the directory
//	  Store the new file header on disk
//	  Flush the changes to the bitmap and the directory back to disk
//
//	Return TRUE if everything goes ok, otherwise, return FALSE.
//
// 	Create fails if:
//   		file is already in directory
//	 	no free space for file header
//	 	no free entry for file in directory
//	 	no free space for data blocks for the file
//
// 	Note that this implementation assumes there is no concurrent access
//	to the file system!
//
//	"path" -- the path of the file to be created
//	"initialSize" -- size of file to be created
//----------------------------------------------------------------------

bool
FileSystem::Create(const char *path, int initialSize)
{
    Path *_path = new Path(path, currentDirectoryFile, rootDirectoryFile);
    return _path->Create(initialSize) != NULL;
}


//----------------------------------------------------------------------
// FileSystem::CreateDirectory
// 	Create a directory in the Nachos file system
//
//	"path" -- the path of the directory to be created
//----------------------------------------------------------------------

bool
FileSystem::CreateDirectory(const char *path)
{
    Path *_path = new Path(path, currentDirectoryFile, rootDirectoryFile);
    return _path->Create(-1) != NULL;
}

//----------------------------------------------------------------------
// FileSystem::Open
// 	Open a file from a path for reading and writing.
//	To open a file:
//	  Find the location of the file's header, using the directory
//	  Bring the header into memory
//
//	"path" -- the path of the file to be opened
//----------------------------------------------------------------------

OpenFile *
FileSystem::Open(const char *path)
{
    Path *_path = new Path(path, currentDirectoryFile, rootDirectoryFile);
    return _path->Open();
}

//----------------------------------------------------------------------
// FileSystem::Remove
// 	Delete a file from the file system.  This requires:
//	    Remove it from the directory
//	    Delete the space for its header
//	    Delete the space for its data blocks
//	    Write changes to directory, bitmap back to disk
//
//	Return TRUE if the file was deleted, FALSE if the file wasn't
//	in the file system.
//
//	"path" -- the path of the file to be removed
//----------------------------------------------------------------------

bool
FileSystem::Remove(const char *path)
{
  Path *_path = new Path(path, currentDirectoryFile, rootDirectoryFile);
  return _path->Remove() != NULL;
}


//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the file system directory.
//----------------------------------------------------------------------

void
FileSystem::List()
{
    Directory *directory = new Directory(NumDirEntries);

    directory->FetchFrom(currentDirectoryFile);
    directory->List();
    delete directory;
}

//----------------------------------------------------------------------
// FileSystem::ListPath
// 	List all the files in the specified path.
//
//	"path" -- the path of the directory to list
//----------------------------------------------------------------------
void
FileSystem::ListPath(const char* path)
{
    Directory *directory = new Directory(NumDirEntries);
    OpenFile *openFile = Open(path);
    if (openFile != NULL) {
      directory->FetchFrom(openFile);
      directory->List();
    }
    else{
      printf("The directory does not exist");
    }
    delete openFile;
    delete directory;
}

//----------------------------------------------------------------------
// FileSystem::Print
// 	Print everything about the file system:
//	  the contents of the bitmap
//	  the contents of the directory
//	  for each file in the directory,
//	      the contents of the file header
//	      the data in the file
//----------------------------------------------------------------------

void
FileSystem::Print()
{
    FileHeader *bitHdr = new FileHeader;
    FileHeader *dirHdr = new FileHeader;
    BitMap *freeMap = new BitMap(NumSectors);
    Directory *directory = new Directory(NumDirEntries);

    printf("Bit map file header:\n");
    bitHdr->FetchFrom(FreeMapSector);
    bitHdr->Print();

    printf("Directory file header:\n");
    dirHdr->FetchFrom(DirectorySector);
    dirHdr->Print();

    freeMap->FetchFrom(freeMapFile);
    freeMap->Print();

    directory->FetchFrom(rootDirectoryFile);
    directory->Print();

    delete bitHdr;
    delete dirHdr;
    delete freeMap;
    delete directory;
}


//----------------------------------------------------------------------
// FileSystem::ChangeDirectory
// 	Change the current directory
//
//	"path" -- the path of the directory to move to
//----------------------------------------------------------------------

void
FileSystem::ChangeDirectory(const char* path) {
    OpenFile *openFile = Open(path);

    if (openFile->IsDirectory()) {
        currentDirectoryFile = openFile;
    }

    delete openFile;
}

//----------------------------------------------------------------------
// FileSystem::CacheOpenFile
// 	Cache an OpenFile
//
//	"openFile" -- OpenFile to store in the cache
//----------------------------------------------------------------------

void
FileSystem::CacheOpenFile(OpenFile *openFile)
{
    openFileCache[indexCache] = openFile;
    indexCache = (indexCache+1) % NumDirEntries;
}

//--------------------FileSystem::--------------------------------------------------
// FileSystem::GetOpenFileFromCache
// 	Return an OpenFile from the cache or NULL
//
//	"sector" -- the sector of the OpenFile to retrieve from the cache
//----------------------------------------------------------------------

OpenFile*
FileSystem::GetOpenFileFromCache(int sector)
{
    for(int i = 0; i < NumDirEntries; i++) {
        if (openFileCache[i]) {
          if (openFileCache[i]->GetSector() == sector) {
              return openFileCache[i];
          }
        }
    }
    return NULL;
}

//----------------------------------------------------------------------
// FileSystem::IsOpenFileInCache
//  Check if an OpenFile is in the cache
//
//	"sector" -- the sector of the OpenFile to retrieve from the cache
//----------------------------------------------------------------------

bool
FileSystem::IsOpenFileInCache(int sector)
{
  for(int i = 0; i < NumDirEntries; i++) {
      if (openFileCache[i]) {
        if (openFileCache[i]->GetSector() == sector) {
            return TRUE;
        }
      }
  }
  return FALSE;
}


//----------------------------------------------------------------------
// FileSystem::OpenFromDirectory
// 	Open a file that is right inside a given directory
//	To open a file:
//	  Find the location of the file's header, using the directory
//	  Bring the header into memory
//
//	"name" -- the text name of the file to be opened
//  "directory" -- the directory from which the file should be open from
//----------------------------------------------------------------------

OpenFile*
FileSystem::OpenFromDirectory(const char *name, Directory *directory)
{
    OpenFile *openFile = NULL;
    DirectoryEntry *directoryEntry = directory->FindDirectoryEntry(name);

    if (directoryEntry->sector >= 0) { // name was found in directory
        openFile = GetOpenFileFromCache(directoryEntry->sector);
        
        if (openFile == NULL) {
         openFile = new OpenFile(directoryEntry->sector);
        }
        if (directoryEntry->isDir) openFile->SetAsDir();
        CacheOpenFile(openFile);
    }

    return openFile;
}


//----------------------------------------------------------------------
// FileSystem::CreateFromDirectory
// 	Create a file or directory that is right inside a given directory
//
//	"name" -- the text name of the file to be created
//	"initialSize" -- size of file to be created; if > 0 then it is a file not a directory
//  "directory" -- the directory from which the file should be open from
//  "directoryOpenFile" -- the OpenFile that corresponds to directory
//----------------------------------------------------------------------

OpenFile*
FileSystem::CreateFromDirectory(const char *name, int initialSize, Directory *directory, OpenFile *directoryOpenFile)
{
    BitMap *freeMap;
    FileHeader *hdr;
    int sector;
    bool isDir = (initialSize == -1);

    DEBUG('f', "Creating file %s, size %d\n", name, initialSize);

    if (directory->Find(name) != -1)
      return NULL;			// file is already in directory
    else {
        freeMap = new BitMap(NumSectors);
        freeMapFile = new OpenFile(FreeMapSector);
        freeMap->FetchFrom(freeMapFile);
        sector = freeMap->Find();	// find a sector to hold the file header
        if (sector == -1)
            return NULL;		// no free block for file header
        else if (!((isDir) ? directory->AddDirectory(name, sector) : directory->Add(name, sector)))
            return NULL;	// no space in directory
        else {
            hdr = new FileHeader;
            if (!hdr->Allocate(freeMap, (isDir) ? DirectoryFileSize : initialSize))
                return NULL;	// no space on disk for data
            else {
                // everthing worked, flush all changes back to disk
              	hdr->WriteBack(sector);
              	directory->WriteBack(directoryOpenFile);
              	freeMap->WriteBack(freeMapFile);
                OpenFile *openFile = new OpenFile(sector);

                if (isDir) {
                    Directory *newDirectory = new Directory(NumDirEntries);
                    newDirectory->AddDirectory(".", sector);
                    newDirectory->AddDirectory("..", directoryOpenFile->GetSector());
                    newDirectory->WriteBack(openFile);

                    delete newDirectory;
                }

                delete freeMap;
                delete hdr;
                return openFile;
            }
        }
    }
}


//----------------------------------------------------------------------
// FileSystem::RemoveFromDirectory
// 	Remove a file or directory that is right inside a given directory
//
//	"name" -- the text name of the file to be removed
//	"initialSize" -- size of file to be created; if > 0 then it is a file not a directory
//  "directory" -- the directory from which the file should be open from
//  "directoryOpenFile" -- the OpenFile that corresponds to directory
//----------------------------------------------------------------------

OpenFile*
FileSystem::RemoveFromDirectory(const char* name, OpenFile *openFile, Directory *directory, OpenFile *directoryOpenFile)
{
    BitMap *freeMap;
    FileHeader *fileHdr;
    int sector = openFile->GetSector();

    fileHdr = new FileHeader;
    fileHdr->FetchFrom(sector);

    freeMap = new BitMap(NumSectors);
    freeMap->FetchFrom(freeMapFile);

    fileHdr->Deallocate(freeMap);  		// remove data blocks
    freeMap->Clear(sector);			// remove header block
    directory->Remove(name);

    freeMap->WriteBack(freeMapFile);		// flush to disk
    directory->WriteBack(directoryOpenFile);        // flush to disk
    delete fileHdr;
    delete freeMap;

    return openFile;
}


// FileSystem::RemoveSafelyFromDirectory
// 	Remove a file or directory that is right inside a given directory
//
//	"name" -- the text name of the file to be removed
//	"initialSize" -- size of file to be created; if > 0 then it is a file not a directory
//  "directory" -- the directory from which the file should be open from
//  "directoryOpenFile" -- the OpenFile that corresponds to directory
//----------------------------------------------------------------------

OpenFile*
FileSystem::RemoveSafelyFromDirectory(const char* name, OpenFile *openFile, Directory *directory, OpenFile *directoryOpenFile)
{
    if (openFile->IsDirectory()) {
        if (directory->IsEmpty()){
          return RemoveFromDirectory(name, openFile, directory, directoryOpenFile);
        } else {
          return NULL;
        }
    } else {
        return RemoveFromDirectory(name, openFile, directory, directoryOpenFile);
    }
}
