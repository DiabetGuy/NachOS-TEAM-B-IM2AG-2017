#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H

#include "copyright.h"
#include "utility.h"
#include "bitmap.h"


class FrameProvider
{
  public:
	FrameProvider(int numPhysPages, int pageSize, char* mainMemory);
	~FrameProvider();
	int GetEmptyFrame();
	int GetEmptyFrameRandom();
	void ReleaseFrame(int frameNb);
	int NumAvailFrame();
  private:
    BitMap * framemap;
    int physicalPageSize;
    char *memory;
};



#endif // FRAMEPROVIDER_H
