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
  int stackNum;
  fa(int _f, int _arg, int sn)
  {
     f = _f;
     arg = _arg;
     stackNum = sn;
  }
};

static void StartUserThread(int f)
{
  int sp;
  machine->WriteRegister(4, ((fa*) f)->arg);
  machine->WriteRegister(PCReg, ((fa*) f)->f);
  machine->WriteRegister(NextPCReg, ((fa*) f)->f+4);
  sp = currentThread->space->stackBeginning - currentThread->space->getSingleStackSize() * ((fa*) f)->stackNum;
  machine->WriteRegister(StackReg, sp);

  machine->Run();
}

int do_UserThreadCreate(int f, int arg)
{
    int stackNum;

    currentThread->space->spaceSem->P();
    stackNum = currentThread->space->bitMap->Find();
    currentThread->space->spaceSem->V();

    if (stackNum == -1){
      return -1;
    }

    fa *f1 = new fa(f, arg, stackNum);

    newthread = new Thread("usethr");
    newthread->stackNum = stackNum;
    joint[newthread->id]->P();
    newthread->Fork(StartUserThread,(int) f1 );

    return newthread->id;
}


void do_UserThreadExit()
{
  joint[currentThread->id]->V();
  currentThread->space->bitMap->Clear(currentThread->stackNum);
  currentThread->Finish ();
}

void do_UserThreadJoin(int id)
{
  joint[id]->P();
}
