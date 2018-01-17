#include "syscall.h"

int main()
{
  PutChar('B');
  ForkExec("./putcharsimple");
  ForkExec("./putcharsimple");

  //Exit(0);
  while(1);


}
