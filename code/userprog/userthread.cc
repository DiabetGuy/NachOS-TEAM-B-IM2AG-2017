/*
The code for creating and destructiing threads for users
go there
*/
#include "userthread.h"
#include "system.h"

Thread * newthread;

int do_UserThreadCreate(int f, int arg)
{
  bool ok = true;
  if(ok)
  {
    newThread = new Thread("usethr");
    newThread->Fork((VoidFunctionPtr)f,(int)arg );
    machine->WriteRegister(4, arg);
    return 0;
  }
  else
  {
      return -1;
  }
}

static void Thread::StartUserThread(int f)
{
  newthread.Yield();
}
