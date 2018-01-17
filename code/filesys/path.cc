// path.cc
//	Path parser

#include "copyright.h"
#include "openfile.h"
#include "directory.h"
#include "filesys.h"
#include "path.h"


//----------------------------------------------------------------------
// Path::Initialize
// 	Parse a Path into a list of PathElement
//
//	"path" -- the path of the file to be opened
//  "currentDirectoryFile" -- the directory the file sytem is current in
//  "rootDirectoryFile" -- the root directory of the file sytem
//----------------------------------------------------------------------

bool
Path::Initialize(const char *path, OpenFile *currentDirectoryFile, OpenFile *rootDirectoryFile)
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
                if (tail->nameSize > FileNameMaxLen) return FALSE; //file name too long
        }
    }
    tail->name[tail->nameSize] = '\0';
    tail->next = NULL;

    return TRUE;
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
// Path::Open
//  Open the file or directory that the path refers to
//----------------------------------------------------------------------
OpenFile*
Path::Open()
{
    return Resolve(&PathResolver::Open);
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
        return Resolve(&PathResolver::CreateFile);
    } else {
        initialSize = -1;
        return Resolve(&PathResolver::CreateDirectory);
    }
}


//----------------------------------------------------------------------
// Path::Resolve
//  Process the list with the help of a specified resolver
//
//  "resolver" -- a resolver to be called every time the file cannot be opened
//----------------------------------------------------------------------
OpenFile*
Path::Resolve(OpenFile* (PathResolver::*resolver)(PathElement *current, Directory *directory, openFile *directoryOpenFile))
{
    OpenFile *openFile = initialDirectoryFile;
    DirectoryEntry *currentDirectoryEntry;
    Directory *directory = new Directory(NumDirEntries);
    directory->FetchFrom(openFile);

    for (PathElement *current = head; current != NULL; current = current->next) {
        //try to open the file else to resolve it
        openFile = FileSystemUtils::OpenFromDirectory(current->name, directory)
                   || (this->*resolver)(current, directory, openFile); //open failed so try to resolve

        if (openFile) {
            if (openFile->IsDirectory()) {
                directory->FetchFrom();
            } else {
                if (current->next != NULL) { //current is not the last PathElement so Path is wrong
                    delete current;
                    delete directory;
                    delete openFile;
                    return NULL;
                } //else the for loop will end and return openFile
            }
        } else {
            delete current;
            delete directory;
            delete openFile;
            return NULL;
        }
    }
    delete current;
    delete directory;

    return openFile;
}



//----------------------------------------------------------------------
// PathResolver::Open
//  Always return NULL as there is nothing to do if the file does not exist
//
//  "current" -- the current PathElement being processed by Resolve
//  "directory" -- the current Directory, Resolve is in
//  "directoryOpenFile" -- the OpenFile that corresponds to the directory
//----------------------------------------------------------------------
OpenFile*
PathResolver::Open(PathElement *current, Directory *directory, OpenFile *directoryOpenFile)
{
    return NULL;
}


//----------------------------------------------------------------------
// PathResolver::CreateFile
//  Create a directory if current is not last else a file
//
//  "current" -- the current PathElement being processed by Resolve
//  "directory" -- the current Directory, Resolve is in
//  "directoryOpenFile" -- the OpenFile that corresponds to the directory
//----------------------------------------------------------------------
OpenFile*
PathResolver::CreateFile(PathElement *current, Directory *directory, OpenFile *directoryOpenFile)
{
    return FileSystemUtils::CreateFromDirectory(current->name, (current->next != NULL) ? -1 : initialSize, directory, directoryOpenFile);
}


//----------------------------------------------------------------------
// PathResolver::CreateDirectory
//  Create a directory
//
//  "current" -- the current PathElement being processed by Resolve
//  "directory" -- the current Directory, Resolve is in
//  "directoryOpenFile" -- the OpenFile that corresponds to the directory
//----------------------------------------------------------------------
OpenFile*
PathResolver::CreateDirectory(PathElement *current, Directory *directory, OpenFile *directoryOpenFile, directoryOpenFile)
{
    return FileSystemUtils::CreateFromDirectory(current->name, -1, directory, directoryOpenFile);
}
