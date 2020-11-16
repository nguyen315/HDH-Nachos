#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 255
int main()
{
 	char c;
	c = ReadChar();
	PrintChar(c);
	return 0;		
}
