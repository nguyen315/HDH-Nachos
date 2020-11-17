#include "syscall.h"
#include "copyright.h"

int main() 
{
	char buffer[255];
	int a;
	ReadString(buffer, 255);
	PrintString(buffer);
	return 0;
}
