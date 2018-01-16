#include "syscall.h"

int main()
{
  //PutChar('A');
  ForkExec("../build/userpages0");
  ForkExec("../build/userpages1");

  Exit(0);


}
