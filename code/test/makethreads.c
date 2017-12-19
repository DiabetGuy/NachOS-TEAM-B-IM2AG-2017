#include "syscall.h"

void thread_loop(void *arg){
	PutChar('P');
	PutChar('D');
}

int main()
{
	// int deux = 2;
	UserThreadCreate(thread_loop, 0);
	//UserThreadCreate(thread_loop, (void*)(&deux));
	PutInt(3);

	//Halt();
	return 0;
}
