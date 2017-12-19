#include "syscall.h"

void thread_loop(void *arg){
	PutChar(GetChar());
	UserThreadExit();
}

int main()
{
	// int deux = 2;
	UserThreadCreate(thread_loop, 0);
	//UserThreadCreate(thread_loop, (void*)(&deux));
	// long long int i=0;
	// while(i != 100000)
	// {
	// 	++i;
	// }
	//UserThreadExit();
	//Halt();
	return 0;
}
