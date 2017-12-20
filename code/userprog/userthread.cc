/*
The code for creating and destructiing threads for users
go there
*/
#include "userthread.h"
#include "system.h"

Thread * newthread;

class fa
{
public:
  int f;
  int arg;
  fa(int _f, int _arg)
  {
     f = _f;
     arg = _arg;
  }
};

static void StartUserThread(int f)
{
  machine->WriteRegister(4, ((fa*) f)->arg);
  machine->WriteRegister(PCReg, ((fa*) f)->f);
  machine->WriteRegister(NextPCReg, ((fa*) f)->f+4);

  machine->Run();
}

int do_UserThreadCreate(int f, int arg)
{
  bool ok = true;
  fa *f1 = new fa(f, arg);

  if(ok)
  {
    newthread = new Thread("usethr");
    newthread->Fork(StartUserThread,(int) f1 );

    // StartUserThread(f);

    return newthread->id;
  }
  else
  {
      return -1;
  }
}


void do_UserThreadExit()
{
  joint[currentThread->id]->V();
  currentThread->Finish ();
}

void do_UserThreadJoin(int id)
{
  joint[id]->P();
}
