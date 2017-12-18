#include "syscall.h"

void thread_loop(int arg){
	for(int i=0; i<5; i++){
		PutInt(arg);
	}
}

int main()
{

	UserThreadCreate(thread_loop, 1);
	UserThreadCreate(thread_loop, 2);
	Thread::StartUserThread(1);
	Thread::StartUserThread(2);

	//Halt();
	return 0;
}
