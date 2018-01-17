#include "syscall.h"

int main()
{
  PutChar('B');
  ForkExec("./putcharsimple");

  //ForkExec("../build/userpages1");

  //Exit(0);
  while(1);


}
