#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H

#include "copyright.h"
#include "utility.h"
#include "bitmap.h"


class FrameProvider
{
  public:

  private:
    BitMap * framemap;
    int physicalPageSize;
    char *memory;
};

FrameProvider(int NumPhysPages, int PageSize, char* mainMemory);
~FrameProvider();
int GetEmptyFrame();
void ReleaseFrame();
int NumAvailFrame();

#endif // FRAMEPROVIDER_H
