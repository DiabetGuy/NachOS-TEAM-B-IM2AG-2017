#include "syscall.h"
#define TAILLE 99
int tok=0;
void thread_loop(void *arg){
	PutString("Zouk");
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
		if(ids[i]!= -1){
			UserThreadJoin(ids[i]);
	  }
		else{
			PutString("Pas cree\n");
		}
  }
	return 0;
}
