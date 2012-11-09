/******************************************************************************
*                                                                             *
* FILE:        Arm7Startup.S                                                  *
*                                                                             *
* DESCRIPTION: Startup code for setting up the C/C++ environments for the    *
*              ARM7.                                                          *
*                                                                             *
* AUTHOR:      Jean-Sebastien Stoezel                                         *
*              js.stoezel@gmail.com                                           *
*                                                                             *
* Heavily influenced by Rowley's Crosswork                                    *
* Visit www.rowley.co.uk - They are good guys                                 *
*                                                                             *
* Used with Rowley's blessing                                                 *
*                                                                             *
******************************************************************************/

#include "ARM7.h"

#define MAIN_FUNCTION  main

/******************************************************************************
*                                                                             *
* EXTERN INTERFACES                                                           *
*                                                                             *
******************************************************************************/
 
   .extern MAIN_FUNCTION

/******************************************************************************
*                                                                             *
* PUBLIC INTERFACES                                                          *
*                                                                             *
******************************************************************************/

   .global Arm7StartupStart
   

/******************************************************************************
*                                                                             *
******************************************************************************/
   .SECTION    .init, "ax"
   .CODE       32
   .ALIGN      4
   
/******************************************************************************
* Arm7StartupStart                                                            *
******************************************************************************/

Arm7StartupStart:
  /* Save the current state of the processor */
  MRS R0, CPSR
  
  /* Clear all the bits related to the processor mode. We will set them one
     after the other for each mode.
  */
  BIC R0, R0, #ARM7_CPSR_MODE_MASK

  /* Setup the stacks for the different modes */ 
  
  /* Undefined mode */
  ORR R1, R0, #ARM7_CPSR_MODE_UND
  MSR cpsr_cxsf, R1
  LDR SP, =__stack_und_end__
  
  /* Abort mode */
  ORR R1, R0, #ARM7_CPSR_MODE_ABT
  MSR cpsr_cxsf, R1
  LDR SP, =__stack_abt_end__

  /* IRQ mode */
  ORR R1, R0, #ARM7_CPSR_MODE_IRQ
  MSR cpsr_cxsf, R1
  LDR SP, =__stack_irq_end__

  /* FIQ mode */
  ORR R1, R0, #ARM7_CPSR_MODE_FIQ
  MSR cpsr_cxsf, R1
  LDR SP, =__stack_fiq_end__

  /* Supervisor mode */
  ORR R1, R0, #ARM7_CPSR_MODE_SVC
  MSR cpsr_cxsf, R1
  LDR SP, =__stack_svc_end__
  
  /*
     Start application in supervisor mode.
     WARNING: stack is NOT set for user and system modes
  */
  
  /* RAM initialization */

  /* Copy the initialized memory sections into RAM */
  LDR R0, =__data_load_start__
  LDR R1, =__data_start__
  LDR R2, =__data_end__
  BL Arm7StartupMemcpy

  LDR R0, =__text_load_start__
  LDR R1, =__text_start__
  LDR R2, =__text_end__
  BL Arm7StartupMemcpy

  /* Constructors */
  LDR R0, =__ctors_load_start__
  LDR R1, =__ctors_start__
  LDR R2, =__ctors_end__
  BL Arm7StartupMemcpy

  /* Destructors */
  LDR R0, =__dtors_load_start__
  LDR R1, =__dtors_start__
  LDR R2, =__dtors_end__
  BL Arm7StartupMemcpy

  LDR R0, =__rodata_load_start__
  LDR R1, =__rodata_start__
  LDR R2, =__rodata_end__
  BL Arm7StartupMemcpy
  
  /* Zero the BSS section */
  LDR R0, =__bss_start__
  LDR R1, =__bss_end__
  MOV R2, #0
  BL Arm7StartupMemset

  /* Initialise the heap */
  LDR R0, = __heap_start__
  LDR R1, = __heap_end__
  SUB R1, R1, R0
  CMP R1, #8
  MOVGE R2, #0
  STRGE R2, [ R0 ], #+4
  STRGE R1, [ R0 ]

  /* Call the constructors */
  LDR R0, =__ctors_start__
  LDR R1, =__ctors_end__
  
/******************************************************************************
* Arm7StartupStartCtorLoop                                                    *
******************************************************************************/

Arm7StartupStartCtorLoop:
  CMP R0, R1
  BEQ Arm7StartupStartCtorEnd
  LDR R2, [ R0 ], #+4
  STMFD SP!, { R0 - R1 }
  MOV LR, PC
  BX R2
  LDMFD SP!, { R0 - R1 }
  b Arm7StartupStartCtorLoop
  
/******************************************************************************
* Arm7StartupStartCtorEnd                                                     *
******************************************************************************/

Arm7StartupStartCtorEnd:
  /* Setup the initial call frame */
  MOV LR, #4
  MOV R12, SP
  STMFD SP!, { R11 - R12, LR - PC }
  SUB R11, R12, #0x00000004

/******************************************************************************
* Arm7StartupCallMain                                                         *
******************************************************************************/

Arm7StartupCallMain:
  /* Jump to the application entry point */
  MOV R0, #0
  MOV R1, #0
  LDR R2, =MAIN_FUNCTION
  MOV LR, PC
  BX R2

/******************************************************************************
* Arm7StartupReturnedFromMain                                                 *
******************************************************************************/

/* Returned from the application, loop forever. */
Arm7StartupReturnedFromMain:
  b Arm7StartupReturnedFromMain

/******************************************************************************
* Arm7StartupMemcpy                                                           *
******************************************************************************/

Arm7StartupMemcpy:
  CMP R0, R1
  MOVEQ PC, LR  
  SUBS R2, R2, R1
  MOVEQ PC, LR  
1:
  LDRB R3, [ R0 ], #+1
  STRB R3, [ R1 ], #+1
  SUBS R2, R2, #1
  BNE 1b
  MOV PC, LR

/******************************************************************************
* Arm7StartupMemset                                                           *
******************************************************************************/

Arm7StartupMemset:
  CMP R0, R1
  MOVEQ PC, LR
  STRB R2, [ R0 ], #1
  b Arm7StartupMemset
