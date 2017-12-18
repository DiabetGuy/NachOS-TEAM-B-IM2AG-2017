#include "syscall.h"

void print(char* str)
{

	PutString(str);

	PutString("\n");
}

int main()
{
	print("bonjour");
	print("comment vas tu");
	print("comment ?");

	//Halt();
	return 2;
}
