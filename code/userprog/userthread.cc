/*
The code for creating and destructiing threads for users
go there
*/
#include "userthread.h"
#include "system.h"

Thread * newthread;

// static void StartUserThread(int f)
// {
// }
int do_UserThreadCreate(int f, int arg)
{
  bool ok = true;
  if(ok)
  {
    newthread = new Thread("usethr");
    newthread->Fork((VoidFunctionPtr)f,(int)arg );
    machine->WriteRegister(4, arg);
    // StartUserThread(f);
    return 0;
  }
  else
  {
      return -1;
  }
}
