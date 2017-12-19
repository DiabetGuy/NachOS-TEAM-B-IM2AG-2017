#include "syscall.h"

void thread_loop(void *arg){
	int i = 0;
	for(i=0; i<5; i++){
		PutInt((int)arg);
	}
}

int main()
{
	int un = 1;
	// int deux = 2;
	UserThreadCreate(thread_loop, (void*)(&un));
	//UserThreadCreate(thread_loop, (void*)(&deux));

	PutInt(3);

	//Halt();
	return 0;
}
