#include "syscall.h"
int tok=0;
void thread_loop(void *arg){
	PutString("Zouk");
	tok=1;
	UserThreadExit();//Quitte
}

int main()
{
	// int deux = 2;
	UserThreadCreate(thread_loop, 0);
	UserThreadCreate(thread_loop, 0);
	//UserThreadCreate(thread_loop, (void*)(&deux));
	// long long int i=0;
	while(1)
	{

	}
	//UserThreadExit();
	//Halt();
	return 0;
}
