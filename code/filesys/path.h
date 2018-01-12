// path.h
//	Path parser

#include "copyright.h"

#ifndef PATH_H
#define PATH_H


// The following class corresponds to a file or directory that the path need to access

class PathElement {
private:
    char name[FileNameMaxLen + 1];	// Text name for file, with +1 for
					// the trailing '\0'
    int nameSize = 0;
    PathElement *next; //next file in the path
};


// The following class corresponds to a path

class Path {
  public:
    Path(const char *path, Directory *); // Parse a Path into a list of PathElement

    ~Path(); //Delete path

    OpenFile Open(); // Open the file that corresponds to the path

  private:
    OpenFile *initialDirectoryFile;
    PathElement *head;
    PathElement *tail;

    // Set the initial directory from which the path starts
    SetInitialDirectory(const char *name, OpenFile *currentDirectoryFile, OpenFile *rootDirectoryFile);
};

#endif // PATH_H
