#include "syscall.h"

int
main()
{
    int c;
    PrintString("All printable ascii characters:\n");
    for (c = 33; c < 127; c++) {
        PrintChar(c);
        PrintChar(':');
        PrintInt(c);
        PrintChar('\n');
    }
    return 0;
}

