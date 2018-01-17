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
    // Parse a Path into a list of PathElement
    Path(const char *path, OpenFile *currentDirectoryFile, OpenFile *rootDirectoryFile);

    ~Path(); // Delete path

    // Process the list with the help of a specified resolver
    OpenFile* Resolve(char resolver);
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

    OpenFile* CallResolver(char resolver, PathElement *current, Directory *directory, OpenFile *directoryOpenFile);
};


// A set of functions, resolvers, to be call by Path::Resolve to get the OpenFile that corresponds to the current PathElement

OpenFile* PathResolverOpen(PathElement *current, Directory *directory, OpenFile *directoryOpenFile);

OpenFile* PathResolverCreateFile(PathElement *current, Directory *directory, OpenFile *directoryOpenFile, int initialSize);

OpenFile* PathResolverCreateDirectory(PathElement *current, Directory *directory, OpenFile *directoryOpenFile);

OpenFile* PathResolverRemove(PathElement *current, Directory *directory, OpenFile *directoryOpenFile);

#endif // PATH_H
