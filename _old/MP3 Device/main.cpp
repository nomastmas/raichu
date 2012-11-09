#include "osHandles.h"              // Includes all OS Files
#include "System/cpu.h"             // Initialize CPU Hardware
#include "drivers/watchdog.h"
#include "drivers/uart0.h"       	// Initialize UART
#include "drivers/uart1.h"       	// Initialize UART
#include "utils/rprintf.h"			// Reduced printf.  STDIO uses lot of FLASH space & Stack space
#include "fat/diskio.h"
#include "../fat/ff.h"            // FAT File System Library
#include "general/userInterface.hpp"	// User interface functions to interact through UART
#include "MP3/mp3.hpp"
#include "utils/timer.hpp"
#include "./drivers/pcm1774.h"
#include "./drivers/i2c.h"

#define IS_SD_CARD_HERE (!(IOPIN0 & (1<<16)))

void sd_card_detect(void *pvParameters)
{
	unsigned char last_sd = 0;
	for (;;)
	{
		if (IS_SD_CARD_HERE && !last_sd)
		{ //if sd card is present for first time
			FATFS SDCard;
			FRESULT res = f_mount(0, &SDCard); // Mount the SD card
			rprintf("--SD Card Mounted %s--\n", (res) ? "Failed" : "Succesfully");

		}
		else if (!IS_SD_CARD_HERE && last_sd)
		{ //if sd card is gone for first time
			FRESULT res = f_mount(0, NULL); // UN-mount the SD card
			rprintf("--SD Card Un-Mounted %s--\n", (res) ? "Failed" : "Succesfully");
		}
		last_sd = IS_SD_CARD_HERE;
		vTaskDelay(100);
	}
}


void FatFsDiskTimer(void* p)
{
	disk_timerproc();
}


/**************************************************************************************
 * TODO 0.  In Eclipse, go to "Tasks" and complete the TODOs to play an mp3 song
 * Hints:
 * 			- SPI & I2C Driver is all setup for you
 * 			- All Drivers and Peripherals are already initialized inside Uart UI task
 * 			- Read "MP3 Decoder" Lab in your Lab Manual
 * ************************************************************************************/

/* INTERRUPT VECTORS:
 * 0:    OS Timer Tick
 * 1:    Not Used
 * 2:    UART0 Interrupt
 * 3:    UART1 Interrupt
 * 4:    I2C0 Interrupt
 * 5-16: Not Used
 */
int main(void)
{
	OSHANDLES SysHandles; // Should contain all OS Handles

	cpuSetupHardware(); // Setup PLL, enable MAM etc.
	watchdogDelayUs(1000 * 1000); // Some startup delay
	uart0Init(38400, 128); // 128 is size of UART0 Rx/Tx FIFO
	uart1Init(38400, 128); // 128 is size of UART1 Rx/Tx FIFO
	// Use polling version of uart0 to do printf/rprintf before starting FreeRTOS
	rprintf_devopen(uart0PutCharPolling);
	cpuPrintMemoryInfo();
	// Open interrupt-driven version of UART0 Rx/Tx
	rprintf_devopen(uart0PutChar);
	watchdogDelayUs(1000 * 1000);

	/** Create timer needed for SD Card I/O */
	Timer diskTimer(FatFsDiskTimer, 10, TimerPeriodic);
	diskTimer.start();
	/** Create any Queues and Mutexes **/
	SysHandles.lock.SPI = xSemaphoreCreateMutex();
	// TODO 2.  Create the "song name" Queue here
	SysHandles.queue.songname = xQueueCreate(1,16); //length, item size
	// Use the WATERMARK command to determine optimal Stack size of each task (set to high, then slowly decrease)
	// Priorities should be set according to response required
		if (pdPASS != xTaskCreate( uartUI, (signed char*)"Uart UI", STACK_BYTES(1024*6), &SysHandles, PRIORITY_HIGH, &SysHandles.task.userInterface )
				||
				pdPASS!= xTaskCreate( mp3Task, (signed char*)"MP3", STACK_BYTES(1024*6), &SysHandles, PRIORITY_HIGH, &SysHandles.task.mp3 )
				||
				pdPASS!= xTaskCreate( sd_card_detect, (signed char*)"sd_card_detect", STACK_BYTES(1024), &SysHandles, PRIORITY_LOW, &SysHandles.task.sd_card_detect )
				||
				pdPASS!= xTaskCreate( mp3_controls, (signed char*)"mp3_controls", STACK_BYTES(1024*6), &SysHandles, PRIORITY_LOW, &SysHandles.task.mp3_controls ))
	{
		rprintf(
				"ERROR:  OUT OF MEMORY: Check OS Stack Size or task stack size.\n");
	}

	// Start FreeRTOS to begin servicing tasks created above, vTaskStartScheduler() will not "return"
	rprintf("\n-- Starting FreeRTOS --\n");
	vTaskStartScheduler();

	// In case OS is exited:
	rprintf_devopen(uart0PutCharPolling);
	rprintf("ERROR: Unexpected OS Exit!\n");

	return 0;
}
