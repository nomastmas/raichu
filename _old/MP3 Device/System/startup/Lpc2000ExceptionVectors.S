/*******************************************************************************
*                                                                              *
* FILE:        LPC2000ExceptionVectors.S                                       *
*                                                                              *
* DESCRIPTION: Exception vector definitions for the LPC2000 microcontroller.   *
*              family.                                                         *
*              Exception vectors will be remapped to RAM if                    *
*              LPC2000_EXCEPTION_VECTORS_IN_RAM is set.                        *
*              All but the reset and IRQ are declared weak so they can be      *
*              overwritten by higher level code.                               *
*                                                                              *
* AUTHOR:      Jean-Sebastien Stoezel                                          *
*              js.stoezel@gmail.com                                            *
*                                                                              *
* Heavily influenced by Rowley's Crosswork                                     *
* Visit www.rowley.co.uk - They are good guys                                  *
*                                                                              *
* Used with Rowley's blessing                                                  *
*                                                                              *
*******************************************************************************/


/* FROM UM10139 */
#define SCB_MEMMAP							0xE01FC040
#define LPC2000_RAM_START					0x40000000
#define LPC2000_EXCEPTION_VECTORS_IN_RAM	1
/*******************************************************************************
*                                                                              *
* EXTERN INTERFACES                                                            *
*                                                                              *
*******************************************************************************/

   .extern	Arm7StartupStart
   
/*******************************************************************************
*                                                                              *
* PUBLIC INTERFACES                                                            *
*                                                                              *
*******************************************************************************/

   .WEAK  _Lpc2000ExceptionVectorsUndefinedHandler
   .WEAK  _Lpc2000ExceptionVectorsSoftwareIntHandler
   .WEAK  _Lpc2000ExceptionVectorsAbortPrefetchHandler
   .WEAK  _Lpc2000ExceptionVectorsAbortDataHandler
   .WEAK  _Lpc2000ExceptionVectorsFIQHandler
   
/*******************************************************************************
*                                                                              *
* _Lpc2000ExceptionVectors                                                     *
*                                                                              *
*******************************************************************************/
   .SECTION    .lpc2000ExceptionVectors, "ax"
   .CODE       32
   .ALIGN      0

_Lpc2000ExceptionVectors:
   LDR PC, [PC, #Lpc2000ExceptionVectorsResetHandlerAddress    - . -    8]          /* Reset */
   LDR PC, [PC, #Lpc2000ExceptionVectorsUndefinedHandlerAddress    - . -    8]      /* Undefined instruction */
   LDR PC, [PC, #Lpc2000ExceptionVectorsSoftwareIntHandlerAddress      - . -    8]  /* Software interrupt handler */
   LDR PC, [PC, #Lpc2000ExceptionVectorsAbortPrefetchHandlerAddress   - . -    8]   /* Abort prefetch */
   LDR PC, [PC, #Lpc2000ExceptionVectorsAbortDataHandlerAddress   - . -    8]       /* Abort data */
   NOP                                                                              /* Boot loader checksum */
   LDR PC, [PC, #-0xFF0]                                                            /* IRQ handler */
   LDR PC, [PC, #Lpc2000ExceptionVectorsFIQHandlerAddress      - . -    8]          /* FIQ handler */
Lpc2000ExceptionVectorsResetHandlerAddress:
   .WORD _Lpc2000ExceptionVectorsResetHandler
Lpc2000ExceptionVectorsUndefinedHandlerAddress:
   .WORD _Lpc2000ExceptionVectorsUndefinedHandler
Lpc2000ExceptionVectorsSoftwareIntHandlerAddress:
   .WORD _Lpc2000ExceptionVectorsSoftwareIntHandler
Lpc2000ExceptionVectorsAbortPrefetchHandlerAddress:
   .WORD _Lpc2000ExceptionVectorsAbortPrefetchHandler
Lpc2000ExceptionVectorsAbortDataHandlerAddress:
   .WORD _Lpc2000ExceptionVectorsAbortDataHandler
Lpc2000ExceptionVectorsFIQHandlerAddress:
   .WORD _Lpc2000ExceptionVectorsFIQHandler


/*******************************************************************************
*                                                                              *
* _Lpc2000ExceptionVectorsResetHandler                                         *
*                                                                              *
*******************************************************************************/
   .SECTION    .init, "ax"
   .CODE       32
   .ALIGN       0

#if LPC2000_EXCEPTION_VECTORS_IN_RAM
_Lpc2000ExceptionVectorsResetHandler:
   /* The exception vectors are copied to SRAM */
   MOV R8, #LPC2000_RAM_START
   LDR R9, =_Lpc2000ExceptionVectors
   LDMIA R9!, { R0 - R7 }
   STMIA R8!, { R0 - R7 }
   LDMIA R9!, { R0 - R6 }
   STMIA R8!, { R0 - R6 }

   /* Memory Mapping control register */
   LDR R0, =SCB_MEMMAP
   MOV R1, #2 /* User RAM Mode. Interrupt vectors are re-mapped to Static RAM. */
   STR R1, [R0]
   
#else

   /* User Flash Mode. Interrupt vectors are not re-mapped and reside in Flash. */
   /* Do we need to force this? */
   
#endif
   B Arm7StartupStart

/*******************************************************************************
*                                                                              *
* _Lpc2000ExceptionVectorsUndefinedHandler                                     *
*                                                                              *
*******************************************************************************/

_Lpc2000ExceptionVectorsUndefinedHandler:
  B Exception_UndefinedAddress
  
/*******************************************************************************
*                                                                              *
* _Lpc2000ExceptionVectorsSoftwareIntHandler                                   *
* This is needed for FreeRTOS
*                                                                              *
*******************************************************************************/
  
_Lpc2000ExceptionVectorsSoftwareIntHandler:
  B vPortYieldProcessor //Previously: _Lpc2000ExceptionVectorsSoftwareIntHandler


/*******************************************************************************
*                                                                              *
* _Lpc2000ExceptionVectorsAbortPrefetchHandler                                 *
*                                                                              *
*******************************************************************************/

_Lpc2000ExceptionVectorsAbortPrefetchHandler:
   B Exception_PrefetchAbort


/*******************************************************************************
*                                                                              *
* _Lpc2000ExceptionVectorsAbortDataHandler                                     *
*                                                                              *
*******************************************************************************/
_Lpc2000ExceptionVectorsAbortDataHandler:
   B Exception_DataAbort

/*******************************************************************************
*                                                                              *
* _Lpc2000ExceptionVectorsFIQHandler                                           *
*                                                                              *
*******************************************************************************/
_Lpc2000ExceptionVectorsFIQHandler:
  B Exception_FIQHandler
