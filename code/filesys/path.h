// path.h
//	Path parser

#include "copyright.h"
#include "bool.h"

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
    ~Path(); // Delete path

    // Parse a Path into a list of PathElement
    bool Initialize(const char *path, OpenFile *currentDirectoryFile, OpenFile *rootDirectoryFile);

    // Process the list with the help of a specified resolver
    OpenFile* Resolve(void (PathResolver::*resolver)(PathElement *current, Directory *directory));
    // Must be call like that : Resolve(&PathResolver::ResolverName);
    // Resolve calls resolver like this : (this->*resolver)(current, directory);

    OpenFile* Open(); // Open the file or directory that the path refers to

    OpenFile* Create(int size); // Create the file or directory that the path refers to

    OpenFile* Remove(); // Remove the file or directory that the path refers to

  private:
    OpenFile *initialDirectoryFile;
    PathElement *head;
    PathElement *tail;
    int initialSize; // To hold initialSize if CreateFile will be called

    // Set the initial directory from which the path starts
    void SetInitialDirectory(const char *path, OpenFile *currentDirectoryFile, OpenFile *rootDirectoryFile);
};


// A set of functions, resolvers, to be call by Path::Resolve if a file cannot be opened
// Must be something like : OpenFile* ResolverName(PathElement *current, Directory *directory, OpenFile *directoryOpenFile);

class PathResolver {
  private:
    OpenFile* Open(PathElement *current, Directory *directory, OpenFile *directoryOpenFile);

    OpenFile* CreateFile(PathElement *current, Directory *directory, OpenFile *directoryOpenFile);

    OpenFile* CreateDirectory(PathElement *current, Directory *directory, OpenFile *directoryOpenFile);

    OpenFile* Remove(PathElement *current, Directory *directory, OpenFile *directoryOpenFile);
};

#endif // PATH_H
