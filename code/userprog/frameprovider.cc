#include "frameprovider.h"
#include "system.h"
#include <stdlib.h>
#include <time.h>

FrameProvider::FrameProvider(int numPhysPages, int pageSize, char* mainMemory)
{

  framemap = new BitMap(numPhysPages);
  physicalPageSize = pageSize;
  memory = mainMemory;
}

FrameProvider::~FrameProvider()
{
  delete framemap;
}

int FrameProvider::GetEmptyFrame()
{
  testSem->P();
  int frameNb = framemap->Find();
  bzero(memory + frameNb * physicalPageSize, physicalPageSize);
  testSem->V();
  return frameNb;
}

int FrameProvider::GetEmptyFrameRandom()
{
  srand(time(NULL));
  int nbFrames = framemap->GetSize();
  int res = rand()%(nbFrames-1);
  while(framemap->Test(res))
  {
		res=rand()%(nbFrames-1);
	}
  framemap->Mark(res);
  return res;
}

void FrameProvider::ReleaseFrame(int frameNb)
{
    framemap->Clear(frameNb);
}

int FrameProvider::NumAvailFrame()
{
    return framemap->NumClear();
}
