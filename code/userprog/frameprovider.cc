#include "frameprovider.h"

FrameProvider::FrameProvider(int NumPhysPages, int PageSize, char* mainMemory)
{
  framemap=new BitMap(NumPhysPages);
  physicalPageSize=PageSize;
  memory=mainMemory;
}

FrameProvider::~FrameProvider()
{
  delete framemap;
}

int FrameProvider::GetEmptyFrame()
{
  int frameNb = framesMap->Find();
  bzero(memory + frameNb * physPageSize, physPageSize);
  return frameNb;
}

void FrameProvider::ReleaseFrame()
{
  framesMap->Clear(frameNb);
}

int FrameProvider::NumAvailFrame()
{
    return framesMap->NumClear();
}
