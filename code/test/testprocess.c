#include "syscall.h"

int main()
{
  //PutChar('A');
  ForkExec("./putcharsimple");
  ForkExec("./putcharsimple");

  Exit(0);



}
