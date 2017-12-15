#include "syscall.h"

char scan()
{
	return GetChar();
}

void print(char str)
{
	PutChar(str);
	PutChar('\n');
}

int main()
{
	char str;
	str = scan();
	print(str);

	//Halt();
	return 0;
}
