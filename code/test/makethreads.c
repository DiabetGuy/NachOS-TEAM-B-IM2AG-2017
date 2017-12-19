#include "syscall.h"
int bite=0;
void thread_loop(void *arg){
	PutString("Zouk");
	bite=1;
	UserThreadExit();//Quitte
}

int main()
{
	// int deux = 2;
	UserThreadCreate(thread_loop, 0);
	//UserThreadCreate(thread_loop, 0);
	//UserThreadCreate(thread_loop, (void*)(&deux));
	// long long int i=0;
	while(bite==0)
	{
		
	}
	//UserThreadExit();
	//Halt();
	return 0;
}
