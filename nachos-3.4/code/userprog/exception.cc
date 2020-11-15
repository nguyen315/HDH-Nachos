// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32
#define MY_INT_MAX 2147483647
#define MY_INT_MIN -2147483648

// Input: - User space address (int)
//  - Limit of buffer(int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char *User2System(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; //need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    //printf("\n Filename u2s:");
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        //printf("%c",kernelBuf[i]);
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

// Input: - User space address (int)
//  - Limit of buffer (int)
//  - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

// Increase Program Counter
void IncreasePC()
{
    // printf("\n IncreasePC() call ...");
    int counter = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, counter);
    counter = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, counter);
    machine->WriteRegister(NextPCReg, counter + 4);
}

// function handle SyscallException

void SyscallExceptionHandler_ReadInt()
{
    // Input: K co
    // Output: Tra ve so nguyen doc duoc tu man hinh console.
    // Chuc nang: Doc so nguyen tu man hinh console. [-2147483648, 2147483647]
    char *buffer;
    int MAX_BUFFER = 255;
    buffer = new char[MAX_BUFFER + 1];
    int numbytes = gSynchConsole->Read(buffer, MAX_BUFFER); // doc buffer toi da MAX_BUFFER ki tu, tra ve so ki tu doc dc
    long long number = 0;                                   

    /* Qua trinh chuyen doi tu buffer sang so nguyen int */
    bool isNegative = false;
    int startIndex = 0;
    if (buffer[0] == '-')
    {
        if (numbytes == 1)
        {
            DEBUG('d', "\nInvalid input. Return 0");
            machine->WriteRegister(2, 0);
            delete buffer;
            return;
        }
        else
        {
            isNegative = true;
            startIndex = 1;
        }
    }
    // DEBUG('d', "GOT HERE %s", buffer);

    // Kiem tra tinh hop le cua so nguyen buffer
    for (int i = startIndex; i < numbytes; i++)
    {
        if (buffer[i] < '0' || buffer[i] > '9')
        {
            DEBUG('d', "\n Invalid input. Return 0");
            machine->WriteRegister(2, 0);
            delete buffer;
            return;
        }
    }
    // DEBUG('d', "GOT HERE %s", buffer);

    // La so nguyen hop le, tien hanh chuyen chuoi ve so nguyen
    for (int i = startIndex; i < numbytes; i++)
    {
        if (isNegative && ((number * 10 + (int)(buffer[i] - '0')) > -MY_INT_MIN))
        {
            DEBUG('d', "\n Overflow INT_MIN. Return INT_MIN -2,147,483,648");
            machine->WriteRegister(2, MY_INT_MIN);
            delete buffer;
            return;
        }
        if (!isNegative && (number * 10 + (int)(buffer[i] - '0')) > MY_INT_MAX)
        {
            DEBUG('d', "\n Overflow INT_MAX. Return INT_MAX 2,147,483,647");
            machine->WriteRegister(2, MY_INT_MAX);
            delete buffer;
            return;
        }
        number = number * 10 + (buffer[i] - '0');
    }

    if (isNegative)
        number = -number;
    
    // DEBUG('d', "GOT HERE ! %d", number);

    machine->WriteRegister(2, number);
    delete buffer;
    return;
}

void SyscallExceptionHandler_PrintInt()
{
    int n = machine->ReadRegister(4);
    // DEBUG('d', "LOL%d\n", n);
    const int maxlen = 12;
    char num_string[maxlen];
    int nums[maxlen], i = 0, j = 0;

    if (n == MY_INT_MIN)
    {
        num_string[j++] = '-';
        nums[i++] = 8; // so cuoi cua -2147483648
        n /= 10;
        n = -n;
    }
    else if (n < 0)
    {
        num_string[j++] = '-';
        n = -n;
    }

    do
    {
        nums[i++] = n % 10;
        n /= 10;
    } while (n);

    for (int k = i - 1; k >= 0; k--)
    {
        num_string[j++] = '0' + nums[k];
    }

    gSynchConsole->Write(num_string, j);
    machine->WriteRegister(2, 0);
    return;
}
void SyscallExceptionHandler_ReadChar()
{
}
void SyscallExceptionHandler_PrintChar()
{
}
void SyscallExceptionHandler_ReadString()
{
}
void SyscallExceptionHandler_PrintString()
{
}
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which)
    {
    case NoException:
        return;
    case PageFaultException:
        DEBUG('a', "\n PageFaultException.");
        printf("\n\n PageFaultException.");
        interrupt->Halt();
        break;
    case ReadOnlyException:
        DEBUG('a', "\n ReadOnlyException.");
        printf("\n\n ReadOnlyException.");
        interrupt->Halt();
        break;
    case BusErrorException:
        interrupt->Halt();
        break;
    case AddressErrorException:
        interrupt->Halt();
        break;
    case OverflowException:
        interrupt->Halt();
        break;
    case IllegalInstrException:
        interrupt->Halt();
        break;
    case NumExceptionTypes:
        interrupt->Halt();
        break;
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
        {
            DEBUG('a', "\n Shutdown, initiated by user program.");
            // printf("\n\n Shutdown, initiated by user program.");
            interrupt->Halt();
            break;
        }

        case SC_Exit:
        {
            DEBUG('a', "\n System call Exit.");
            // printf("\n\n System call Exit.");
            interrupt->Halt();
            break;
        }

        case SC_Create:
        {
            int virtAddr;
            char *filename;
            DEBUG('a', "\n SC_Create call ...");
            DEBUG('a', "\n Reading virtual address of filename");
            // Lấy tham số tên tập tin từ thanh ghi r4
            virtAddr = machine->ReadRegister(4);
            DEBUG('a', "\n Reading filename.");
            // MaxFileLength là = 32
            filename = User2System(virtAddr, 32 + 1);
            if (filename == NULL)
            {
                printf("\n Not enough memory in system");
                DEBUG('a', "\n Not enough memory in system");
                machine->WriteRegister(2, -1); // trả về lỗi cho chương
                // trình người dùng
                delete filename;
                return;
            }
            DEBUG('a', "\n Finish reading filename.");
            //DEBUG('a',"\n File name : '"<<filename<<"'");
            // Create file with size = 0
            // Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
            // việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
            // hành Linux, chúng ta không quản ly trực tiếp các block trên
            // đĩa cứng cấp phát cho file, việc quản ly các block của file
            // trên ổ đĩa là một đồ án khác
            if (!fileSystem->Create(filename, 0))
            {
                printf("\n Error create file '%s'", filename);
                machine->WriteRegister(2, -1);
                delete filename;
                return;
            }
            machine->WriteRegister(2, 0); // trả về cho chương trình
            // người dùng thành công
            delete filename;
            break;
        }

        case SC_ReadInt:
        {
            SyscallExceptionHandler_ReadInt();
            break;
        }

        case SC_PrintInt:
        {
            SyscallExceptionHandler_PrintInt();
            break;
        }

        case SC_ReadChar:
        {
            SyscallExceptionHandler_ReadChar();
            break;
        }

        case SC_PrintChar:
        {
            SyscallExceptionHandler_PrintChar();
            break;
        }

        case SC_ReadString:
        {
            SyscallExceptionHandler_ReadString();
            break;
        }
        case SC_PrintString:
        {
            SyscallExceptionHandler_PrintString();
            break;
        }

        default:
        {
            printf("\n Unexpected user mode exception (%d %d)", which, type);
            interrupt->Halt();
        }
        }

        IncreasePC();
        break;
    }
}
