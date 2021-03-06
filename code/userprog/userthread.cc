/*
The code for creating and destructiing threads for users
go there
*/
#include "userthread.h"
#include "system.h"


extern void StartProcess(char*);

class thread_args
{
public:
  int f;
  int arg;
  int stackNum;
  thread_args(int _f, int _arg, int sn)
  {
     f = _f;
     arg = _arg;
     stackNum = sn;
  }
};

static void StartUserThread(int f)
{
  int sp;
  machine->WriteRegister(4, ((thread_args*) f)->arg);
  machine->WriteRegister(PCReg, ((thread_args*) f)->f);
  machine->WriteRegister(NextPCReg, ((thread_args*) f)->f+4);
  sp = currentThread->space->stackBeginning - currentThread->space->getSingleStackSize() * ((thread_args*) f)->stackNum;
  machine->WriteRegister(StackReg, sp);

  machine->Run();
}

// static void MyStartProcess()
// {
// 	currentThread->space->InitRegisters();
//   currentThread->space->RestoreState();
//
//   machine->Run();
//   ASSERT(FALSE); //ne passe jamais ici
// }

int do_UserThreadCreate(int f, int arg)
{
    int stackNum;

    currentThread->space->spaceSem->P();
    stackNum = currentThread->space->bitMap->Find();
    currentThread->space->spaceSem->V();

    if (stackNum == -1){
      return -1;
    }

    thread_args *f1 = new thread_args(f, arg, stackNum);

    Thread *newthread = new Thread("usethr");
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

int do_ForkExec(char *s)
{
    // OpenFile *executable = fileSystem->Open(s);
    //
    // if (executable == NULL) {
    //     printf("ForkExec %s failed...\n", s);
    //     return -1;
    // }
    char * myexec = new char [MAX_STRING_SIZE];
    strcpy(myexec, s);
    printf("ForkExec %s \n", myexec);
		Thread * threadlauncher = new Thread("ForkedProcess");
    testSem->P();
    processNb++;
    testSem->V();
    //threadlauncher->space = new AddrSpace (executable);

    //thread_args *f1 = new thread_args(0, 0, 0);

    //AddrSpace *space = new AddrSpace(executable);

    //delete executable;
    threadlauncher->Fork ((VoidFunctionPtr)StartProcess, (int)myexec);
    return 0;
}
