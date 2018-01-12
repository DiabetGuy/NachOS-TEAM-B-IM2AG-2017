// exception.cc
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"



void copyStringFromMachine(int from, char *to, unsigned size){
  unsigned int i = 0;
  int resultatMemoire = 0;

  while((i < size) && (machine->ReadMem(from+i, 1, &resultatMemoire))){
    to[i] = resultatMemoire;
    i++;
  }
}

void copyStringToMachine(int to, char *from, unsigned size){
  unsigned int i = 0;
  while((i < size) && (machine->WriteMem(to+i, 1,(int)from[i])))
    i++;
}

//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
    int pc = machine->ReadRegister (PCReg);
    machine->WriteRegister (PrevPCReg, pc);
    pc = machine->ReadRegister (NextPCReg);
    machine->WriteRegister (PCReg, pc);
    pc += 4;
    machine->WriteRegister (NextPCReg, pc);
}


//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions
//      are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister (2);

    if (which == SyscallException) {
      switch (type) {
        case SC_Halt: {
          DEBUG('a', "Shutdown, initiated by user program.\n");
          interrupt->Halt();
          break;
        }
        case SC_Exit: {
          int returnParameter = machine->ReadRegister(4);
          if(returnParameter == 0)
          {
            machine->lock->P();
            machine->processNb--;
            machine->lock->V();
            if(machine->processNb == 0)
            {

                interrupt->Halt();
            }
            else
            {
                currentThread->Finish();
            }
          }
          DEBUG('a', "Shutdown, initiated by user program.\n");

          break;
        }
        case SC_PutChar: {
          DEBUG ('p', "PutChar.\n");
          synchconsole->SynchPutChar((char) machine->ReadRegister(4));
          break;
        }
        case SC_PutString: {
          DEBUG ('p', "PutString.\n");
          char str[MAX_STRING_SIZE];
          copyStringFromMachine(machine->ReadRegister(4), str, MAX_STRING_SIZE);
          synchconsole->SynchPutString(str);
          break;
        }
        case SC_GetChar: {
          DEBUG ('p', "GetChar.\n");
          char charGot = synchconsole->SynchGetChar();
          machine->WriteRegister(2, (int) charGot);
          break;
        }
        case SC_GetString: {
          DEBUG ('p', "GetString.\n");
          char charGot[MAX_STRING_SIZE];
          synchconsole->SynchGetString(charGot, MAX_STRING_SIZE);
          copyStringToMachine(machine->ReadRegister(4), charGot, machine->ReadRegister(5));
          break;
        }
        case SC_PutInt: {
          DEBUG ('p', "PutInt.\n");
          int n = machine->ReadRegister(4);
          synchconsole->SynchPutInt(n);
          break;
        }
        case SC_GetInt: {
          DEBUG ('p', "GetInt.\n");
          int i = synchconsole->SynchGetInt();
          machine->WriteRegister(2, i);
          break;
        }
        case SC_UserThreadCreate: {
          DEBUG ('p', "UserThreadCreate.\n");
          int id = do_UserThreadCreate(machine->ReadRegister(4), machine->ReadRegister(5));
          machine->WriteRegister(2,id);
          break;
        }
        case SC_UserThreadExit: {
          DEBUG ('p', "GetUserThreadExit.\n");
          do_UserThreadExit();
          break;
        }
        case SC_UserThreadJoin: {
          DEBUG ('p', "GetUserThreadJoin.\n");
          do_UserThreadJoin(machine->ReadRegister(4));
          break;
        }
        case SC_ForkExec: {
          DEBUG ('p', "ForkExec.\n");
          int arg = machine->ReadRegister(4);
          char strarg[MAX_STRING_SIZE];
          copyStringFromMachine(arg, strarg, MAX_STRING_SIZE);
          do_ForkExec(strarg);
          break;
        }
        default: {
          printf("Unexpected user mode exception %d %d %d\n", which, type, machine->ReadRegister (2));
          ASSERT(FALSE);
        }
      }
    }

    // LB: Do not forget to increment the pc before returning!
    UpdatePC ();
    // End of addition
}
