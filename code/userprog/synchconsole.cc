
#include "copyright.h"
#include "system.h"
#include "synchconsole.h"

static Semaphore *readAvail;
static Semaphore *writeDone;

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

static Semaphore *putted;
static Semaphore *getted;

char buffer[MAX_STRING_SIZE];

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
	putted = new Semaphore("putting...", 0);
	getted = new Semaphore("getting...", 0);
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch)
{
	putted->P();
	console->PutChar(ch);
	writeDone->P();
	putted->V();
}

char SynchConsole::SynchGetChar()
{
	getted->P();
	char c;
	readAvail->P();
	c = console->GetChar();
	getted->V();
	return c;

}

void SynchConsole::SynchPutString(const char s[])
{
	for(int i = 0; s[i] != '\0' && i < MAX_STRING_SIZE && s[i] != EOF; i++)
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

int SynchConsole::SynchGetInt()
{
	char c;
	int n = 0 ;
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
	sscanf(str, "%d", &n);
	return n;
}

void SynchConsole::UserThreadExit()
{
	currentThread->Finish();
}
