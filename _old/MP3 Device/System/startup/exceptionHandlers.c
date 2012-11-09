/**
 * These exception vectors are mapped from Lpc2000ExceptionVectors.S
 */
#include "utils/rprintf.h"
#include "drivers/uart0.h"


#define getProgramCounterValue(value, sub)								\
			register unsigned int programCounter asm ("lr");			\
			unsigned int value = programCounter - (unsigned int) sub

void printExceptionInfo(char* str, unsigned int value)
{
	rprintf_devopen(uart0PutCharPolling);
	rprintf("%s!  Program Counter Address: 0x%08X\n", str, value);
	rprintf("Open your project's *.lst file to locate PC Value to debug\n");
	while(1);
}

void Exception_UndefinedAddress()
{
	getProgramCounterValue(PCValue, 0x04);
	printExceptionInfo("Undefined Address Exception", PCValue);
}

void Exception_PrefetchAbort()
{
	getProgramCounterValue(PCValue, 0x04);
	printExceptionInfo("Undefined Instruction Exception", PCValue);
}

void Exception_DataAbort()
{
	getProgramCounterValue(PCValue, 0x08);
	printExceptionInfo("Undefined Data Access Exception", PCValue);
}

void Exception_FIQHandler()
{
	getProgramCounterValue(PCValue, 0x04);
	printExceptionInfo("FIQ Interrupt Exception", PCValue);
}


