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
	int id1 = UserThreadCreate(thread_loop, 0);
	int id2 = UserThreadCreate(thread_loop, 0);
	int id3 = UserThreadCreate(thread_loop, 0);

	//UserThreadCreate(thread_loop, (void*)(&deux));
	// long long int i=0;
	/*PutChar('\n');
	PutInt(id1);
	PutChar('\n');
	PutInt(id2);
	PutChar('\n');
	PutInt(id3);*/
	UserThreadJoin(id1);
	UserThreadJoin(id2);
	UserThreadJoin(id3);
	//UserThreadExit();
	//Halt();
	return 0;
}
