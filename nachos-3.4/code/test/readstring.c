#include "syscall.h"
#include "copyright.h"

int main() 
{
	char* buffer;
	ReadString(buffer, 255);
	PrintString(buffer);
	return 0;
}
