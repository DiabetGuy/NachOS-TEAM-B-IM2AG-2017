// path.h
//	Path parser

#include "copyright.h"

#ifndef PATH_H
#define PATH_H


// The following class corresponds to a file or directory that the path need to access

class PathElement {
  public:
    PathElement() { // constructor
      nameSize = 0;
    }

    char name[FileNameMaxLen + 1];	// Text name for file, with +1 for
					// the trailing '\0'
    int nameSize;
    PathElement *next; //next file in the path
};


// The following class corresponds to a path

class Path {
  public:
    ~Path(); //Delete path

    bool Initialize(const char *path, OpenFile *currentDirectoryFile, OpenFile *rootDirectoryFile); // Parse a Path into a list of PathElement
    
    OpenFile* Open(); // Open the file that corresponds to the path

  private:
    OpenFile *initialDirectoryFile;
    PathElement *head;
    PathElement *tail;

    // Set the initial directory from which the path starts
    void SetInitialDirectory(const char *path, OpenFile *currentDirectoryFile, OpenFile *rootDirectoryFile);
};

#endif // PATH_H
