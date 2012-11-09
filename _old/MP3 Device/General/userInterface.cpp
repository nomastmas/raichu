#include "userInterface.hpp"

#include <stdlib.h>                 // atoi()
#include <stdio.h>                  // printf() or iprintf()
#include <string.h>                 // strtok()


#include "../osHandles.h"
#include "../utils/rprintf.h"     // rprintf
#include "../fat/ff.h"            // FAT File System Library
#include "../fat/diskio.h"        // Disk IO Initialize SPI Mutex

#include "../drivers/uart0.h"     // uart0GetChar()
#include "../drivers/uart1.h"     // uart1GetChar()
#include "../WIFI/wifi.hpp"
#include "../MP3/mp3.hpp"

#include "../drivers/sta013.h"    // STA013 Mp3 Decoder
#include "../drivers/pcm1774.h"   // PCM 1774 DAC
#include "../drivers/ssp_spi.h"
#include "../drivers/i2c.h"


#define MATCH(a,b) (0 == strcmp(a,b))
void getUartLine(char* uartInput);
void getUart1Line(char* uartInput);
void printLine()
{
	rprintf("------------------------------------------------------------------------------------\n");
}

void uartUI(void *pvParameters)
{
	OSHANDLES *osHandles = (OSHANDLES*)pvParameters;

	// Suspend the task until connection is open
	vTaskSuspend(osHandles->task.mp3_controls);

	char uartInput[128];

	// Error check if SPI Lock doesn't exist.
	if(0 == osHandles->lock.SPI)
	{
		rprintf("You did not create an SPI Mutex\n");
		while(1);
	}

#if 1
	initialize_SSPSPI();
	initialize_I2C0(100*1000);

	diskio_initializeSPIMutex(&(osHandles->lock.SPI));
	initialize_SdCardSignals();
	initialize_sta013();
	initialize_pcm1774();

	FATFS SDCard;        // Takes ~550 Bytes
	if(FR_OK != f_mount(0, &SDCard)) { // Mount the Card on the File System
		rprintf("WARNING: SD CARD Could not be mounted!\n");
	}
	/*
	 * Web-Based MP3 Player Starts Here
	 */

	if (wifiBegin())
	{
		//wifiGetDeviceIDFromModule();
		if(wifiJoinWirelessNetwork()
			&&wifiOpenTCPConnection()
			&&wifiSendDeviceIDToServer()
			&&wifiSentPlayListToServer()
			&&wifiReceiveFromServer()
		    &&wifiCloseTCPConnection()
		    &&wifiConnectionStatus())
		{
			//rprintf("--Parse PlayList From Server--\n");
			for (;;)
			{
				//After TCP connection accepted from website,
				//We set our UartUI task priority to lowest and resume MP3 Task
				vTaskResume(osHandles->task.mp3_controls);
				vTaskPrioritySet(osHandles->task.userInterface, PRIORITY_LOW);

				//vTaskDelay(100);
				//char buffer[256] ={ 0 };
				//wifiReceiveExpectedData(buffer, sizeof(buffer));
				//parsePlayListFromServer(buffer);
				//xQueueSend(osHandles->queue.songname, &buffer[0], 1000);

			}
		}
	}


#endif

	for (;;)
	{
		rprintf("LPC: ");
		memset(uartInput, 0, sizeof(uartInput));
		getUartLine(uartInput);
		if (strlen(uartInput) <= 0){
			continue;
		}

		char *command = strtok(uartInput, " ");

		if (MATCH(command, "echo")){
			char *echoBack = strtok(NULL, "");
			rprintf("Echo: %s\n", echoBack);
		}
		else if(MATCH("ls", command))
		{
			DIR Dir;
			FILINFO Finfo;
			FATFS *fs;
			FRESULT returnCode = FR_OK;

			unsigned int fileBytesTotal, numFiles, numDirs;
			fileBytesTotal = numFiles = numDirs = 0;
			#if _USE_LFN
			char Lfname[512];
			#endif

			char dirPath[] = "0:";
			if (FR_OK != (returnCode = f_opendir(&Dir, dirPath))) {
				rprintf("Invalid directory: |%s|\n", dirPath);
				continue;
			}

			rprintf("Directory listing of: %s\n\n", dirPath);
			for (;;) {
				#if _USE_LFN
				Finfo.lfname = Lfname;
				Finfo.lfsize = sizeof(Lfname);
				#endif

				char returnCode = f_readdir(&Dir, &Finfo);
				if ( (FR_OK != returnCode) || !Finfo.fname[0])
					break;
				if (Finfo.fattrib & AM_DIR){
					numDirs++;
				}
				else{
					numFiles++;
					fileBytesTotal += Finfo.fsize;
				}
				iprintf("%c%c%c%c%c %u/%2u/%2u %2u:%2u %10lu %13s",
						(Finfo.fattrib & AM_DIR) ? 'D' : '-',
						(Finfo.fattrib & AM_RDO) ? 'R' : '-',
						(Finfo.fattrib & AM_HID) ? 'H' : '-',
						(Finfo.fattrib & AM_SYS) ? 'S' : '-',
						(Finfo.fattrib & AM_ARC) ? 'A' : '-',
						(Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
						(Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63, Finfo.fsize, &(Finfo.fname[0]));
				#if _USE_LFN
				iprintf(" -- %s", Lfname);
				#endif
				iprintf("\n");
			}
			iprintf("\n%4u File(s), %10u bytes total\n%4u Dir(s)", numFiles, fileBytesTotal, numDirs);

			if (f_getfree(uartInput, (DWORD*) &fileBytesTotal, &fs) == FR_OK)
			{
				iprintf(", %10uK bytes free\n", fileBytesTotal * fs->csize / 2);
			}
		}
		/**************************************************************************************
		 * TODO 3.  Add "play" command and queue the songname over to "song name" Queue
		 * 			This should be your initial test command, later on, you need to simply
		 * 			queue all songs on the SD Card without this command
		 * ***********************************************************************************/
		else if(MATCH(command, "play"))
		{
			char *songname = strtok(0,"");
			if(xQueueSend(osHandles->queue.songname, &songname[0], 1000))
			{
				rprintf("Song to be play: |%s|\n", songname);
			}
		}
		/*
		 * Op code to initialize the wifi module into CMD mode
		 */
		else if (MATCH(command, "wifi_cmd"))
		{
			wifiEnterCommandMode();
		}
		else if (MATCH(command, "wifi_exitcmd"))
		{
			wifiExitCommandMode();
		}
		/*
		 * Command to interface with the wifi module, must initialize CMD first
		 * Recommended sequence: wifi reboot, wifi_cmd, wifi [command]
		 */
		else if(MATCH(command, "wifi"))
		{
			char * wifiCommand = strtok(0, "");
			char bufForWifi[256] ={ 0 };
			char enter[] = "\r";
			vTaskDelay(1000);
			wifiSendString(wifiCommand);
			wifiSendString(enter);
			wifiReceiveExpectedData(bufForWifi, sizeof(bufForWifi));
			rprintf("----------------------------\n");
			rprintf("%s\n", bufForWifi);
			rprintf("----------------------------\n");
		}
		else if(MATCH(command, "wifi_open"))
		{
			wifiSentPlayListToServer();
		}
		else if(MATCH(command, "read")) {
			char *filename = strtok(0, "");
			rprintf("File to read: |%s|\n", filename);

			FIL file;
			if(FR_OK == f_open(&file, filename, (FA_READ | FA_OPEN_EXISTING)))
			{
				rprintf("File successfully opened\n");
				char buff[513] = {0};

				const unsigned int bytesToRead = 512;
				unsigned int bytesRead = 0;
				rprintf("File Contents:\n");
				printLine();
				do
				{
					if(FR_OK != f_read(&file, buff, bytesToRead, &bytesRead))
					{
						rprintf("--Failed to read file after having it opened--\n");
						break;
					}
					rprintf("    %s", buff);
				}while(bytesRead == bytesToRead);
				printLine();

				f_close(&file);
				rprintf("File is closed\n");
			}
			else
			{
				rprintf("Failed to open the file\n");
			}
		}
		else if(MATCH(command, "write")) {
			char *filename = strtok(0, "");
			rprintf("File to write: |%s|\n", filename);

			FIL file;
			if(FR_OK == f_open(&file, filename, (FA_WRITE | FA_CREATE_ALWAYS)))
			{
				rprintf("File successfully opened\n");
				char buff[] = "File written using FATFS From Chen\n";

				unsigned int bytesWritten = 0;
				if(FR_OK != f_write(&file, buff, sizeof(buff), &bytesWritten))
				{
					rprintf("Failed to write file\n");
				}

				f_close(&file);
				rprintf("File is closed\n");
			}
			else
			{
				rprintf("Failed to open the file\n");
			}
		}
#if configGENERATE_RUN_TIME_STATS
		else if(MATCH(command, "CPU")) {
			vTaskGetRunTimeStats(uartInput, 0);
			rprintf("%s", uartInput);
		}
		else if(MATCH(command, "CPUr")) {
			vTaskGetRunTimeStats(uartInput, 1);
			rprintf("%s", uartInput);
		}
		else if(MATCH(command, "CPUn")) {
			int delayTimeMs = atoi(strtok(NULL, ""));
			if(delayTimeMs > 0) {
				rprintf("Delaying for %ims.  CPU Usage will be reported afterwards...\n", delayTimeMs);
				vTaskGetRunTimeStats(uartInput, 1);
				vTaskDelay(OS_MS(delayTimeMs));
				vTaskGetRunTimeStats(uartInput, 1);
				rprintf("%s", uartInput);
			}
			else {
				rprintf("You must define a delay time in milliseconds as parameter.\n");
			}
		}
#endif
#if configUSE_TRACE_FACILITY != 0
		else if(MATCH(command, "info")) {
			char buff[512];
			vTaskList(buff);
			rprintf("%s", buff);
		}
#endif
		else if (MATCH("HELP", command)) {
			rprintf("\n");
			printLine();
			rprintf("Command list:\n");
			printLine();
			rprintf("echo <anything>    - Echoes back anything sent.\n");
			rprintf("ls                 - Get directory listing.\n");
			rprintf("read <filename>    - Read a file.\n");
			rprintf("write <filename>   - Wrie a file.\n");
				#if configGENERATE_RUN_TIME_STATS
			rprintf("CPU                - Show tasks and CPU Allocation.\n");
			rprintf("CPUr               - Show tasks and CPU Allocation & reset counters\n");
			rprintf("CPUn <# ms>        - Reset counters, delay, then report CPU usage & reset counters\n");
				#endif
				#if configUSE_TRACE_FACILITY != 0
			rprintf("info               - Show tasks and their Stack-high watermark.\n");
				#endif
			printLine();
		}
		else {
			rprintf("Command not recognized.\n");
		}
	}
}

void getUartLine(char* uartInput)
{
	char data;
	unsigned int uartInputPtr = 0;

	while (1)
	{
		while(!uart0GetChar(&data, portMAX_DELAY));

		if (data == '\n'){
			uartInput[uartInputPtr] = 0;
			uart0PutChar('\n', 100);
			break;
		}
		else if (data == '\r'){
			// Ignore it
		}
		else if (data == '\b')
		{
			if (uartInputPtr > 0){
				uartInputPtr--;
				rprintf("\b \b");
			}
			else {					// Nothing to backspace
				uart0PutChar(7, 0);	// ASCII Char 7 is for bell
			}
		}
		/*
		// If you want +/- keys to be directly considered as commands:
		else if(uartInputPtr == 0 && (data == '+' || data == '-')) {
			uartInput[0] = data;
			uartInput[1] = '\0';
			break;
		}
		*/
		else{
			uart0PutChar(data, 100);
			uartInput[uartInputPtr++] = data;
		}
	}
}

void getUart1Line(char* uartInput)
{
	char data;
	unsigned int uartInputPtr = 0;

	while (1)
	{
		while(!uart1GetChar(&data, portMAX_DELAY));

		if (data == '\n'){
			uartInput[uartInputPtr] = 0;
			uart1PutChar('\n', 100);
			break;
		}
		else if (data == '\r'){
			// Ignore it
		}
		else if (data == '\b')
		{
			if (uartInputPtr > 0){
				uartInputPtr--;
				rprintf("\b \b");
			}
			else {					// Nothing to backspace
				uart1PutChar(7, 0);	// ASCII Char 7 is for bell
			}
		}
		/*
		// If you want +/- keys to be directly considered as commands:
		else if(uartInputPtr == 0 && (data == '+' || data == '-')) {
			uartInput[0] = data;
			uartInput[1] = '\0';
			break;
		}
		*/
		else{
			uart1PutChar(data, 100);
			uartInput[uartInputPtr++] = data;
		}
	}
}

