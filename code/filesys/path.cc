// path.cc
//	Path parser

#include "copyright.h"
#include "openfile.h"
#include "directory.h"
#include "filesys.h"
#include "path.h"


//----------------------------------------------------------------------
// Path::Path
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
// Path::Open
//  Open the file that corresponds to the path
//----------------------------------------------------------------------
OpenFile*
Path::Open()
{
    OpenFile *openFile = initialDirectoryFile;
    DirectoryEntry *currentDirectoryEntry;
    Directory *directory = new Directory(NumDirEntries);
    directory->FetchFrom(openFile);

    for (PathElement *current = head; current != NULL; current = current->next) {
        //if directory entry found
        currentDirectoryEntry = directory->FindDirectoryEntry(current->name);
        if (currentDirectoryEntry->sector != -1) {
            openFile = new OpenFile(currentDirectoryEntry->sector);
            if (currentDirectoryEntry->isDir) {
                openFile->SetAsDir();
                directory->FetchFrom(openFile);
            } else { //current is the file and should be the last PathElement
                if (current->next != NULL) { //current is not the last PathElement so Path is wrong
                    delete directory;
                    delete openFile;
                    return NULL;
                }
            }
        } else { //if directory entry not found
            delete directory;
            delete openFile;
            return NULL;
        }
    }
    delete directory;

    return openFile;
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
