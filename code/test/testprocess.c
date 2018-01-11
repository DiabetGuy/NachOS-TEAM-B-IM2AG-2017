#include "syscall.h"

int main()
{
  ForkExec("../build/userpages1");
  ForkExec("../build/userpages0");

  Exit(0);


}
