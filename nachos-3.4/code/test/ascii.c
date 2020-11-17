#include "syscall.h"

int
main()
{
    int c;
    PrintString("All printable ascii characters:\n");
    for (c = 32; c < 127; c++) {
        PrintChar(c);
        PrintChar(':');
        PrintInt(c);
        PrintChar('\n');
    }
    Halt();
}

