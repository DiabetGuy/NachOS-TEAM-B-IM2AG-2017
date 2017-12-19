#include "syscall.h"

void thread_loop(void *arg){
	PutChar('C');
	UserThreadExit();
}

int main()
{
	// int deux = 2;
	UserThreadCreate(thread_loop, 0);
	UserThreadCreate(thread_loop, 0);
	UserThreadCreate(thread_loop, 0);
	//UserThreadCreate(thread_loop, (void*)(&deux));
	// long long int i=0;
	// while(1)
	// {
	// 	++i;
	// }
	//UserThreadExit();
	//Halt();
	return 0;
}
