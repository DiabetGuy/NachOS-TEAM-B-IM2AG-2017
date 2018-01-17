// path.cc
//	Path parser

#include "copyright.h"
#include "openfile.h"
#include "directory.h"
#include "filesys.h"
#include "path.h"
#include "system.h"


//----------------------------------------------------------------------
// Path::Path
// 	Parse a Path into a list of PathElement
//
//	"path" -- the path of the file to be opened
//  "currentDirectoryFile" -- the directory the file sytem is current in
//  "rootDirectoryFile" -- the root directory of the file sytem
//----------------------------------------------------------------------

Path::Path(const char *path, OpenFile *currentDirectoryFile, OpenFile *rootDirectoryFile)
{
    SetInitialDirectory(path, currentDirectoryFile, rootDirectoryFile);

    head = tail = new PathElement;

    for (int i = 0; path[i] != '\0'; i++) {
        switch (path[i]) {
            case '/':
                tail->name[tail->nameSize] = '\0';
                tail->next = new PathElement;
                tail = tail->next;
                break;
            default:
                tail->name[tail->nameSize] = path[i]; //add caracter by caracter
                tail->nameSize++;
                //if (tail->nameSize > FileNameMaxLen) //file name too long
        }
    }
    tail->name[tail->nameSize] = '\0';
    tail->next = NULL;
}


// Path::~Path
// 	Delete path, de-allocating any in-memory data structures.
//----------------------------------------------------------------------

Path::~Path()
{
    for (PathElement *current = head; current != NULL;) {
        PathElement *tmp = current;
        current = current->next;
        delete tmp;
    }
    //delete current; //TODO

    delete head;
    delete tail;
    delete initialDirectoryFile;
}


//----------------------------------------------------------------------
// Path::SetInitialDirectory
// 	Set the initial directory from which the path starts
//
//	"path" -- the path of the file to be opened
//  "currentDirectoryFile" -- in which directory the file sytem is currently in
//  "rootDirectoryFile" -- the root directory of the file sytem
//----------------------------------------------------------------------

void
Path::SetInitialDirectory(const char *path, OpenFile *currentDirectoryFile, OpenFile *rootDirectoryFile)
{
    if (path[0] == '/') {
        initialDirectoryFile = rootDirectoryFile; //from root directory
        path++;
    } else {
        initialDirectoryFile = currentDirectoryFile; //from current directory
    }
}

//----------------------------------------------------------------------
// Path::CallResolver
// 	A switch to call our resolver
//
//  "resolver" -- the resolver code name
//	"path" -- the path of the file to be opened
//  "currentDirectoryFile" -- in which directory the file sytem is currently in
//  "rootDirectoryFile" -- the root directory of the file sytem
//----------------------------------------------------------------------

OpenFile*
Path::CallResolver(char resolver, PathElement *current, Directory *directory, OpenFile *directoryOpenFile)
{
  switch(resolver) {
    case 'o':
      return PathResolverOpen(current, directory, directoryOpenFile);
      break;
    case 'f':
      return PathResolverCreateFile(current, directory, directoryOpenFile, initialSize);
      break;
    case 'd':
      return PathResolverCreateDirectory(current, directory, directoryOpenFile);
      break;
    case 'r':
      return PathResolverRemove(current, directory, directoryOpenFile);
      break;
    default:
      return NULL;
  }
}


//----------------------------------------------------------------------
// Path::Open
//  Open the file or directory that the path refers to
//----------------------------------------------------------------------
OpenFile*
Path::Open()
{
    return Resolve('o');
}


//----------------------------------------------------------------------
// Path::Create
//  Create the file or directory that the path refers to
//
//  "size" -- size of file to be created; if > 0 then it is a file not a directory
//----------------------------------------------------------------------
OpenFile*
Path::Create(int size)
{
    if (size > 0) {
        initialSize = size;
        return Resolve('f');
    } else {
        initialSize = -1;
        return Resolve('d');
    }
}


//----------------------------------------------------------------------
// Path::Remove
//  Remove the file or directory that the path refers to
//----------------------------------------------------------------------
OpenFile*
Path::Remove()
{
    return Resolve('r');
}


//----------------------------------------------------------------------
// Path::Resolve
//  Process the list with the help of a specified resolver
//
//  "resolver" -- a resolver to be called every time the file cannot be opened
//----------------------------------------------------------------------
OpenFile*
Path::Resolve(char resolver)
{
    OpenFile *openFile = initialDirectoryFile;
    Directory *directory = new Directory(NumDirEntries);
    directory->FetchFrom(openFile);

    for (PathElement *current = head; current != NULL; current = current->next) {
        openFile = CallResolver(resolver, current, directory, openFile);

        if (openFile) {
            if (openFile->IsDirectory()) {
                directory->FetchFrom(openFile);
            } else {
                if (current->next != NULL) { //current is not the last PathElement so Path is wrong
                    delete directory;
                    delete openFile;
                    return NULL;
                } //else the for loop will end and return openFile
            }
        } else {
            delete directory;
            delete openFile;
            return NULL;
        }
    }
    delete directory;

    return openFile;
}



//----------------------------------------------------------------------
// PathResolverOpen
//  Always return NULL as there is nothing to do if the file does not exist
//
//  "current" -- the current PathElement being processed by Resolve
//  "directory" -- the current Directory, Resolve is in
//  "directoryOpenFile" -- the OpenFile that corresponds to the directory
//----------------------------------------------------------------------
OpenFile*
PathResolverOpen(PathElement *current, Directory *directory, OpenFile *directoryOpenFile)
{
    return fileSystem->OpenFromDirectory(current->name, directory);
}


//----------------------------------------------------------------------
// PathResolverCreateFile
//  Create a directory if current is not last else a file
//
//  "current" -- the current PathElement being processed by Resolve
//  "directory" -- the current Directory, Resolve is in
//  "directoryOpenFile" -- the OpenFile that corresponds to the directory
//----------------------------------------------------------------------
OpenFile*
PathResolverCreateFile(PathElement *current, Directory *directory, OpenFile *directoryOpenFile, int initialSize)
{
    OpenFile *openFile = fileSystem->OpenFromDirectory(current->name, directory);
    if (openFile) return openFile;

    return fileSystem->CreateFromDirectory(current->name, (current->next != NULL) ? -1 : initialSize, directory, directoryOpenFile);
}


//----------------------------------------------------------------------
// PathResolverCreateDirectory
//  Create a directory
//
//  "current" -- the current PathElement being processed by Resolve
//  "directory" -- the current Directory, Resolve is in
//  "directoryOpenFile" -- the OpenFile that corresponds to the directory
//----------------------------------------------------------------------
OpenFile*
PathResolverCreateDirectory(PathElement *current, Directory *directory, OpenFile *directoryOpenFile)
{
    OpenFile *openFile = fileSystem->OpenFromDirectory(current->name, directory);
    if (openFile) return openFile;

    return fileSystem->CreateFromDirectory(current->name, -1, directory, directoryOpenFile);
}

//----------------------------------------------------------------------
// PathResolverRemove
//  Remove a file or directory
//
//  "current" -- the current PathElement being processed by Resolve
//  "directory" -- the current Directory, Resolve is in
//  "directoryOpenFile" -- the OpenFile that corresponds to the directory
//----------------------------------------------------------------------
OpenFile*
PathResolverRemove(PathElement *current, Directory *directory, OpenFile *directoryOpenFile)
{
    OpenFile *openFile = fileSystem->OpenFromDirectory(current->name, directory);

    if (current->next != NULL) {
        return openFile;
    } else {
        return fileSystem->RemoveSafelyFromDirectory(current->name, openFile, directory, directoryOpenFile);
    }
}
