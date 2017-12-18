#include "syscall.h"

char* scan()
{
	char *res = 0;
	GetString(res, 10);
	return res;
}

void print(char* str)
{
	PutString(str);
	PutChar('\n');
}

int main()
{
	char* str;
	str = scan();
	print(str);

	str = scan();
	print(str);

	str = scan();
	print(str);

	Halt();
	return 0;
}
