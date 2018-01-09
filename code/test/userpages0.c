#include "syscall.h"

#define TAILLE 3

void thread_loop(void *arg){
	int i = 0;
	//char *str = "ABCDE";
	PutString("a");
	i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;
	i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;
	i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;
	PutString("b");
	i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;
	i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;
	i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;i++;
	PutString("c");

  UserThreadExit();
}

int main()
{
  int i;
  int ids[TAILLE];
  for(i=0;i<TAILLE;i++){
    ids[i] = UserThreadCreate(thread_loop, 0);
  }
  for(i=0;i<TAILLE;i++){
  	UserThreadJoin(ids[i]);
  }

	return 0;
}
