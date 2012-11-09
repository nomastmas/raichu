#include "cpu.h"
#include "../System/sysConfig.h"
#include "../System/lpc214x.h"
#include "../utils/rprintf.h"

void cpuSetupHardware (void)
{
  //
  //  Configure pin functions.  All pins are set to GPIO, including the Debug
  //  port (P1.26) and the Trace port (P1.25..P1.16).
  //
  PINSEL0 = 0;
  PINSEL1 = 0;
  PINSEL2 = 0;
  IODIR0 = 0;
  IODIR1 = 0;

#if USE_PLL

	#if (PLL_DIV == 1)
	#define PLL_DIV_VALUE 0x00
			PLLCFG = ((PLL_MUL - 1) | (0x00<<5));
	#elif (PLL_DIV == 2)
			PLLCFG = ((PLL_MUL - 1) | (0x01<<5));
	#elif (PLL_DIV == 4)
			PLLCFG = ((PLL_MUL - 1) | (0x10<<5));
	#elif (PLL_DIV == 8)
			PLLCFG = ((PLL_MUL - 1) | (0x11<<5));
	#endif

	PLLCON = 1;

	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	while((PLLSTAT & (1 << 10)) == 0);
	PLLCON = 3;			// Connect PLL clock to System Clock
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
#else
	PLLCON = 0;
#endif

  //  Setup and turn on the MAM.  Three cycle access is used due to the fast
  //  PLL used.  It is possible faster overall performance could be obtained by
  //  tuning the MAM and PLL settings.
  MAMTIM = MAM_TIMING;
  MAMCR = MAM_SETTING;

  // Set the Peripheral Clock Divider
#if(PBSD == 4)
	VPBDIV = 0;
#else
	VPBDIV = PBSD;
#endif

  //  Make sure all interrupts disabled
  VICIntEnClr = (0x007ffffd);
}

void cpuPrintMemoryInfo(void)
{
	/* Extern variables are defined by the linker.  */
	extern unsigned int* __SRAM_segment_start__;  /* Start of SRAM Start */
	extern unsigned int* __non_init_end__;        /* End of last global memory space */
	extern unsigned int* __stack_user_start__;    /* Start of fisrt Stack */
	extern unsigned int* __stack_und_end__;       /* End of last Stack */
	extern unsigned int* __heap_start__;          /* Start of HEAP */

	/**
	 * The addresses of these symbols represent 32-bit memory region addresses
	 * So to calculate in bytes, we multiply by 4
	 */
	const unsigned int globalMemUse = 4 * (&__non_init_end__ - &__SRAM_segment_start__);
	const unsigned int systemStack  = 4 * (&__stack_und_end__ - &__stack_user_start__);
	const unsigned int totalUsedMem = 4 * (&__heap_start__ - &__SRAM_segment_start__);
	const unsigned int freeMem = 32*1024 - totalUsedMem;

	rprintf("\n");
	rprintf("---------------------------------------\n");
	rprintf("-- Memory Information: \n");
	rprintf("--      Total Stack: % 5u\n", systemStack);
	rprintf("-- Approx. Used RAM: % 5u\n", globalMemUse);
	rprintf("-- Approx. Free RAM: % 5u\n", freeMem);
	rprintf("\n");
	rprintf("Sram Start : %x\n", &__SRAM_segment_start__);
	rprintf("Stack Start: %x\n", &__stack_user_start__);
	rprintf("Heap Start : %x\n", &__heap_start__);
	rprintf("---------------------------------------\n");
}

