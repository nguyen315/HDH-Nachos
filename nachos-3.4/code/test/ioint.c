#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main()
{
    int a, b;
    a = ReadInt();
    PrintInt(a);
    b = ReadInt();
    PrintInt(b);
    return 0;
}