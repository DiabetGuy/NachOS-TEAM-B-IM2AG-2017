// directoryhdr.h

#include "copyright.h"

#ifndef DIRECTORYHDR_H
#define DIRECTORYHDR_H

#include "filehdr.h"

// DirectoryHeader is a FileHeader that is constructed as a directory

class DirectoryHeader : public FileHeader {
  public:
    DirectoryHeader() { // constructor
      isDir = TRUE;
    };

  private:
    bool isDir;
};

#endif // DIRECTORYHDR_H
