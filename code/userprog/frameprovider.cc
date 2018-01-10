#include "frameprovider.h"
#include <stdlib.h>
#include <time.h>

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

int FrameProvider::GetEmptyFrameRandom()
{
  int nbClear = NumClear ();
  int i, j=0;
	int nombreAleatoire = 0;
  int nbFrames = framesMap->GetSize();
	srand(time(NULL)); // initialisation de rand
	nombreAleatoire = rand()%(nbClear) +1;
  for(i=0; i<nbFrames || j == nombreAleatoire; i++){
    if (!Test (i)) j++;
  }
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
