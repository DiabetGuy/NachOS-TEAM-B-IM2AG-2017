#include "frameprovider.h"
#include <stdlib.h>
#include <time.h>

FrameProvider::FrameProvider(int numPhysPages, int pageSize, char* mainMemory)
{
  framemap=new BitMap(numPhysPages);
  physicalPageSize=pageSize;
  memory=mainMemory;
}

FrameProvider::~FrameProvider()
{
  delete framemap;
}

int FrameProvider::GetEmptyFrame()
{
  int frameNb = framemap->Find();
  bzero(memory + frameNb * physicalPageSize, physicalPageSize);
  return frameNb;
}

int FrameProvider::GetEmptyFrameRandom()
{
  int nbClear = framemap->NumClear ();
  int i, j=0;
	int frameNb = 0;
  int nbFrames = framemap->GetSize();
	srand(time(NULL)); // initialisation de rand
	frameNb = rand()%(nbClear) +1;
  for(i=0; i<nbFrames || j == frameNb; i++){
    if (!framemap->Test (i)) j++;
  }
  bzero(memory + i * physicalPageSize, physicalPageSize);
  framemap->Mark(i);
  return i;
}

void FrameProvider::ReleaseFrame(int frameNb)
{
  framemap->Clear(frameNb);
}

int FrameProvider::NumAvailFrame()
{
    return framemap->NumClear();
}
