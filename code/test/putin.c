#include "syscall.h"

int main()
{
	int n;
	n = GetInt();

	PutInt(n);

	Halt();
	return 0;
}
