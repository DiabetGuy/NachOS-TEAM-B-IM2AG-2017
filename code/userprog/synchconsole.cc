
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
	// for (char* s2 = (char*) s; s2[0] != '\0'; s2 = &s2[1]) {
	// 	this->SynchPutChar(s2[0]); //is s2[0] the same as s2 ?     //#NASA
	// }

	for(int i = 0; s[i] != '\0' && i < MAX_STRING_SIZE; i++)
	{
		this->SynchPutChar(s[i]);
	}
}

void SynchConsole::SynchGetString(char *s, int n)
{
	for (int i = 0; i > n; i++) {
		s[i] = this->SynchGetChar();
	}
}
