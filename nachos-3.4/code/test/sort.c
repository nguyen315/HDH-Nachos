/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

#include "syscall.h"

int a[100];	/* size of physical memory; with code, we'll run out of space!*/


int main()
{
    int N, i, j, tmp;
    PrintChar('\n');
    PrintString("SORT ARRAY INTEGER");
    PrintString("Input N = ");
    N = ReadInt();
    if (N < 1 || N > 100) {
        PrintString("Invalid Number. 1 <= N <= 100 \n");    
        Halt();
    }
    for (i = 0; i < N; i++) {
        a[i] = 0;
        PrintString("a[");
        PrintInt(i);
        PrintString("] = ");
        a[i] = ReadInt();
    }
    for (i = 0; i < N-1; i++) {
        for (j = N-1; j > i; j--) {
            if (a[j-1] > a[j]) {
                tmp = a[j];
                a[j] = a[j-1];
                a[j-1] = tmp;
            }
        }
    }
    PrintString("Sorted array:");
    for (i = 0; i < N; i++) {   
        PrintChar(' ');
        PrintInt(a[i]);
    }
    PrintChar('\n');
    Halt();
}
