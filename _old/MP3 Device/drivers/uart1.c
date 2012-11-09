//
//  $Id: uart1.c 234 2008-10-28 23:56:14Z jcw $
//  $Revision: 234 $
//  $Author: jcw $
//  $Date: 2008-10-28 19:56:14 -0400 (Tue, 28 Oct 2008) $
//  $HeadURL: http://tinymicros.com/svn_public/arm/lpc2148_demo/trunk/uart/uart1.c $
//


#include <stdlib.h>
#include "../System/lpc214x.h"
#include "uart1.h"

//
//  Constants to determine the ISR source
#define serSOURCE_THRE					  ((unsigned portCHAR) 0x02)
#define serSOURCE_RX_TIMEOUT			((unsigned portCHAR) 0x0c)
#define serSOURCE_ERROR					  ((unsigned portCHAR) 0x06)
#define serSOURCE_RX					    ((unsigned portCHAR) 0x04)
#define serINTERRUPT_SOURCE_MASK	((unsigned portCHAR) 0x0f)

//  Queues used to hold received characters, and characters waiting to be transmitted
static xQueueHandle xRX1Queue;
static xQueueHandle xTX1Queue;
//  Communication flag between the interrupt service routine and serial API
static volatile portLONG lTHREEmpty1;

static void uart1ISR_Handler(void)
{
	signed char cChar;
	portBASE_TYPE higherPriorityTaskWoken = pdFALSE;

	long statusReg = UART1_IIR;
	switch (statusReg & serINTERRUPT_SOURCE_MASK)
	{
		//  Not handling this, but clear the interrupt
		case serSOURCE_ERROR:
		{
			cChar = UART1_LSR;
		}
			break;

			//  The THRE is empty.  If there is another character in the Tx queue, send it now,
			//  otherwise, no more characters, so indicate THRE is available
		case serSOURCE_THRE:
		{

			// Depending on if FIFO is enabled, we can send up to 16 chars because
			// THRE triggers when FIFO is empty.
			signed char bytesToSend = (statusReg & 0xC0) ? 16 : 1;
			while (bytesToSend-- && xQueueReceiveFromISR(xTX1Queue, &cChar,
					&higherPriorityTaskWoken))
			{
				U1THR = cChar;
			};
			// If we did not fill all the buffer, we have capacity to write to U1THR directly.
			if (bytesToSend > 0)
			{
				lTHREEmpty1 = 1;
			}
		}
			break;

			//  A character was received.  Place it in the queue of received characters
		case serSOURCE_RX_TIMEOUT:
		case serSOURCE_RX:
		{
			while ((UART1_LSR & 0x01))
			{ // read as long as there is unread data
				cChar = UART1_RBR;
				xQueueSendFromISR (xRX1Queue, &cChar, &higherPriorityTaskWoken);
			}
		}
			break;

		default:
			break;
	}

	VICVectAddr = (unsigned portLONG) 0;

	if (higherPriorityTaskWoken) portYIELD_FROM_ISR ();
}

void uart1ISR(void) __attribute__ ((naked));
void uart1ISR(void)
{
	portSAVE_CONTEXT ();
	runTimeStatISREntry();
	uart1ISR_Handler();
	runTimeStatISRExit();
	portRESTORE_CONTEXT ();
}

char uart1Init(unsigned long ulWantedBaud, unsigned long uxQueueLength)
{
	unsigned long ulDivisor;
	unsigned long ulWantedClock;

	if (0 == ulWantedBaud) ulWantedBaud = 38400;
	if (0 == uxQueueLength) uxQueueLength = 64;

	//  Create the queues used to hold Rx and Tx characters
	xRX1Queue = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE) sizeof(signed portCHAR));
	xTX1Queue = xQueueCreate(uxQueueLength + 1, (unsigned portBASE_TYPE) sizeof(signed portCHAR));

	lTHREEmpty1 = 1;

	if ((xRX1Queue == 0) || (xTX1Queue == 0)) return 0;

	portENTER_CRITICAL ();
	{
		PINSEL0 = (PINSEL0 & 0xfff0ffff) | 0x00050000; // PinSelect Uart1

		PCONP |= 16; // Enable power to UART1, For UART1, Bit 4: 10000

		ulWantedClock = ulWantedBaud * 16;
		ulDivisor = configCPU_CLOCK_HZ / ulWantedClock;

		// Set DLAB bit to access divisors
		UART1_LCR |= UART_LCR_DLAB;
		UART1_DLL = (unsigned char) (ulDivisor & (unsigned long) 0xff);
		ulDivisor >>= 8;
		UART1_DLM = (unsigned char) (ulDivisor & (unsigned long) 0xff);

		//  Turn on the FIFO's and clear the buffers
		UART1_FCR = UART_FCR_EN | UART_FCR_CLR;
		UART1_FCR |= (1 << 6); // 0=1char trigger, 1=4char, 2=8char, 3=14char trigger

		//  Setup transmission format
		UART1_LCR = UART_LCR_NOPAR | UART_LCR_1STOP | UART_LCR_8BITS;

		//  Setup the VIC for the UART
		VICIntSelect &= ~VIC_IntSelect_UART1;
		VICVectAddr3 = (long) uart1ISR;
		VICVectCntl3 = VIC_VectCntl_ENABLE | VIC_Channel_UART1;
		VICIntEnable |= VIC_IntEnable_UART1;

		//  Enable UART1 interrupts
		UART1_IER |= UART_IER_EI;
	}
	portEXIT_CRITICAL ();

	return 1;
}

unsigned long uart1GetChar(char *pcRxedChar, portTickType xBlockTime)
{
	return xQueueReceive (xRX1Queue, pcRxedChar, xBlockTime);
}

unsigned long uart1PutCharPolling(char cOutChar, unsigned long c)
{
	while (!(U1LSR & (1 << 5)))
		;
	UART1_THR = cOutChar;
	return 1;
}

unsigned long uart1PutChar(char cOutChar, portTickType xBlockTime)
{
	signed portBASE_TYPE xReturn = 0;

	portENTER_CRITICAL ();
	{
		//  Is there space to write directly to the UART?
		if (lTHREEmpty1)
		{
			lTHREEmpty1 = 0;
			UART1_THR = cOutChar;
			xReturn = pdPASS;
		}
		else
		{
			//  We cannot write directly to the UART, so queue the character.  Block for a maximum of
			//  xBlockTime if there is no space in the queue.
			xReturn = xQueueSend (xTX1Queue, &cOutChar, xBlockTime);

			//  Depending on queue sizing and task prioritisation:  While we were blocked waiting to post
			//  interrupts were not disabled.  It is possible that the serial ISR has emptied the Tx queue,
			//  in which case we need to start the Tx off again.
			if (lTHREEmpty1 && (xReturn == pdPASS))
			{
				xQueueReceive (xTX1Queue, &cOutChar, 0);
				lTHREEmpty1 = 0;
				UART1_THR = cOutChar;
			}
		}
	}
	portEXIT_CRITICAL ();

	return xReturn;
}
