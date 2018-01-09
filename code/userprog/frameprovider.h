#include "copyright.h"
#include "utility.h"
#include "bitmap.h"


FrameProvider(int NumPhysPages, int PageSize, char* mainMemory);
~FrameProvider();
int GetEmptyFrame();
void ReleaseFrame();
int NumAvailFrame();
