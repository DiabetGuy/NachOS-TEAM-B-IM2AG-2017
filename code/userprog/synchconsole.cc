
#include "copyright.h"
#include "system.h"
#include "synchconsole.h"

static Semaphore *readAvail;
static Semaphore *writeDone;

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

char buffer[MAX_STRING_SIZE];

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch)
{
	console->PutChar(ch);
	writeDone->P();
}

char SynchConsole::SynchGetChar()
{
	readAvail->P();
	return console->GetChar();
}

void SynchConsole::SynchPutString(const char s[])
{
	for(int i = 0; s[i] != '\0' && i < MAX_STRING_SIZE; i++)
	{
		this->SynchPutChar(s[i]);
	}
}

void SynchConsole::SynchGetString(char *s, int n)
{
	int i;
	for (i = 0; i < n; i++) {
		s[i] = this->SynchGetChar();
		if(s[i] == 0xa)
			break;
	}
	s[i]= '\0';
}

void SynchConsole::SynchPutInt(int n)
{
	char str[15];
    snprintf(str,10,"%d",n);
	SynchPutString(str);
}

void SynchConsole::SynchGetInt(int *n)
{
	char c;
  char str[15];
  int i = 0;

	c=SynchGetChar();
	while((c >= '0') && (c <= '9') && (i < 15) && (c != 0xa) && (c != EOF))
	{
		str[i] = c;
		c = SynchGetChar();
		i++;
	}
	str[i] = '\0';
	sscanf(str, "%d", n);

}
