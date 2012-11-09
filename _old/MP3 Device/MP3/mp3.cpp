#include "mp3.hpp"
#include "../osHandles.h"
#include "../fat/ff.h"
#include "../drivers/sta013.h"
#include "../utils/rprintf.h"
#include "../drivers/ssp_spi.h"
#include "string.h"
#include "utils/vector.hpp"
#include "../drivers/sta013.h"
#include "../drivers/pcm1774.h"
#include "../WIFI/wifi.hpp"
#include "./drivers/i2c.h"
#include "./fat/diskio.h"
#include "./fat/ff.h"
#include "./drivers/sta013.h"
#include "./drivers/ssp_spi.h"
#include "./drivers/pcm1774.h"

int lastFileIndex; // song count
char songArray[30][24]; // array to hold song list
char *nowPlaying; // current song

enum {Play,Forward,Stop,Back,PauseResume,VolUp,VolDown,Default};

const char * lookup_table[]	= {"Play","Forward","Stop","Back",
		"PauseResume","VolUp","VolDown","Default"};
/*
 * LookUp table for case switch incoming commands
 */
int lookup(char stringCommand[])
{
	const int avaliableString = sizeof lookup_table / sizeof * lookup_table;
	for(int i=0;i!=avaliableString;i++)
		if(strcmp(stringCommand,lookup_table[i])==0)
			return i;
	return Default;
}
typedef struct SongNameType
{
	char name[16];
	/// Operator for SongNameType = (char*)
	void operator=(char* s)
	{(*this) = 0;strncpy(&name[0], s, sizeof(name) - 2);}
	/// Operator for SongNameType a = SongNameType b
	SongNameType& operator=(const SongNameType& copy)
	{if (this != &copy)(*this) = (char*) &copy.name[0];return *this;}
	/// Operator for SongNameType a = 0;
	void operator=(int)
	{memset(&name[0], 0, sizeof(name));}
} SongNameType;


/*
 * Populate song database
 */
void populateSongs(void *pvParameters)
{
	OSHANDLES *osHandles = (OSHANDLES*) pvParameters;
	char fullName[24];
	int counter = 0;
	int i = 0;
	// Error check if SPI Lock doesn't exist.
	if (0 == osHandles->lock.SPI)
	{
		rprintf("You did not create an SPI Mutex\n");
		while (1);
	}
	initialize_SSPSPI();
	initialize_I2C0(100 * 1000);
	initialize_SdCardSignals();
	initialize_sta013();
	initialize_pcm1774();

	FATFS SDCard; // Takes ~550 Bytes
	if (FR_OK != f_mount(0, &SDCard))
	{ // Mount the Card on the File System
		rprintf("WARNING: SD CARD Could not be mounted!\n");
	}
	DIR Dir;
	FILINFO Finfo;
	FRESULT returnCode = FR_OK;

	unsigned int fileBytesTotal, numFiles, numDirs;
	fileBytesTotal = numFiles = numDirs = 0;
	#if _USE_LFN
	char Lfname[512];
	#endif

	char dirPath[] = "0:";
	if (FR_OK != (returnCode = f_opendir(&Dir, dirPath)))
	{
		rprintf("Invalid directory: |%s|\n", dirPath);
	}
	rprintf("Directory listing of: %s\n\n", dirPath);
	for (;;)
	{
		#if _USE_LFN
		Finfo.lfname = Lfname;
		Finfo.lfsize = sizeof(Lfname);
		#endif

		char returnCode = f_readdir(&Dir, &Finfo);
		if ((FR_OK != returnCode) || !Finfo.fname[0])
			break;
		if (Finfo.fattrib & AM_DIR)
		{
			numDirs++;
		}
		else
		{
			numFiles++;
			fileBytesTotal += Finfo.fsize;
		}
		if ((strstr(Finfo.fname, ".MP3")))
		{
			strcpy(fullName, Finfo.fname);
			char *temp = fullName;
			while (temp[i] != NULL)
			{
				//rprintf("Copying File[%d]: %c\n", i, file[i]);
				songArray[counter][i] = temp[i];
				i++;
				//rprintf("Received songArray[%d][%d]: %c\n", counter, i, songArray[counter][i]);
			}
		}
		songArray[counter][i + 1] = NULL;
		i = 0;
		//rprintf("Just populated song: %s\n", songArray[counter]);

		lastFileIndex = counter;
		counter++;

		//rprintf("lastFileIndex = %d\n", lastFileIndex);
		//don't overflow the song array!
		if (counter + 1 > 30)
			break;
	}
	for (i = 0; i <= lastFileIndex; i++)
	{
		rprintf("songArray[%d] = %s\n", i, songArray[i]);
	}
	rprintf("lastFileIndex = %d\n", lastFileIndex);

	//rprintf("Exiting popSongs.\n");

	 xSemaphoreGive(osHandles->lock.SPI);
}
/********** Get Current Song Position ***********/
int getSong(char c, char *fileName, int current)
{
	//rprintf("Received the following character: %c.\n", c);
	vTaskDelay(100);
	/******** Next Song ********/
	if (c == 'S')
	{
		if (current + 1 > lastFileIndex){current = 0;}
		else{current++;}
		strcpy(fileName, songArray[current]);
	}
	/******** Previous Song ********/
	else if (c == 'P')
	{
		if (current - 1 < 0){current = lastFileIndex;}
		else{current--;}
		strcpy(fileName, songArray[current]);
	}
	/******** Current Song ********/
	else if (c == 'C')
	{
		strcpy(fileName, songArray[current]);
	}
	else if (c != NULL)
	{
		fileName = "ERROR!";
	}
	vTaskDelay(50);
	//rprintf("Sent Along: songArray[%d] = %s =? %s!\n", currentSong, songArray[currentSong], fileName);
	return current;
}

void mp3Task(void* p)
{
	for (;;)
	{
		OSHANDLES *osHandles = (OSHANDLES*) p;
		char songname[16];
		/* --- Receive a song name ---*/
		if (xQueueReceive(osHandles->queue.songname, &songname[0], portMAX_DELAY ))
		{
			rprintf("Currently playing this song: %s\n", songname);
			FIL fileHandle;
			char buffer[4096];
			/* --- Open the file --- */
			if (FR_OK == f_open(&fileHandle, songname, FA_OPEN_EXISTING | FA_READ))
			{
				//rprintf("File has opened, about to play the file \n");/*debug*/
				unsigned int bytesRead = sizeof(buffer);
				//check if there is an item was sent to the queue, if there is, play that item
				while (0 == uxQueueMessagesWaiting(osHandles->queue.songname))
				//while (bytesRead == sizeof(buffer))
				{
					if (FR_OK == f_read(&fileHandle, buffer, sizeof(buffer),&bytesRead))
					{
						if (xSemaphoreTake(osHandles->lock.SPI, 1000))
						{
							SELECT_MP3_CS();
							unsigned int bytesStreamed = 0;
							while (bytesStreamed < bytesRead)
							{
								if (STA013_NEEDS_DATA())
								{
									rxTxByteSSPSPI(buffer[bytesStreamed++]);
								}
								else{vTaskDelay(1);}
							}
							DESELECT_MP3_CS();
							xSemaphoreGive(osHandles->lock.SPI);
						}
						else
						{
							rprintf("--Error: SPI Semaphore is taken\n");
						}
					}
				}
				//rprintf("--Song Ended-\n");
				f_close(&fileHandle);
			}
			else
			{
				rprintf("--Failed to open file-\n");
			}
		}
		//rprintf("--Next Song-\n");
		//xQueueSend(osHandles->queue.songname, &songs.front().name, 1000);
		//rprintf("The Next Song is: %s\n", &songs.front().name);
	}
}

/********** Control Task ***********/
void mp3_controls(void *p)
{
	rprintf("MP3 Control Task Executed \n");
	OSHANDLES *osHandles = (OSHANDLES*) p;
	int volume = 50;
	int pauseResume = 0;

	int currentSong;
	char songName[24];
	int i =0;

	populateSongs(osHandles);
	currentSong = 0;
	pcm1774_OutputVolume(volume, volume);

	for (;;)
	{
		char buff[128]={0};
		wifiReceiveExpectedData(buff, sizeof(buff));
		rprintf("%s\n",buff);
		vTaskDelay(50);
		//if(lookup(buff)<7)
		//{
			switch (lookup(buff))
			{
				//Play
			case Play:
				sta013StartDecoder();
				currentSong = getSong('C', songName, currentSong);
				xQueueSend(osHandles->queue.songname, &songName[0], 100);
				//xQueueSend(osHandles->queue.songname, &songs[0].name, 500);
				break;
				//Foward
			case Forward:
				sta013StopDecoder();

				currentSong = getSong('S', songName, currentSong);
				while (songArray[currentSong][i] != NULL)
				{
					songName[i] = songArray[currentSong][i];
					i++;
				}
				songName[i] = 0;
				i = 0;
				xQueueSend(osHandles->queue.songname, &songName, 100);
				//xQueueSend(osHandles->queue.songname, &songs.rotateRight().name, 500);
				//initialize_sta013(); // performs reset
				sta013StartDecoder();
				break;
				//Stop
			case Stop:
				sta013StopDecoder();
				pauseResume = 1;
				break;
				//Back
			case Back:
				sta013StopDecoder();

				currentSong = getSong('P', songName, currentSong);
				while (songArray[currentSong][i] != NULL)
				{
					songName[i] = songArray[currentSong][i];
					i++;
				}
				songName[i] = 0;
				rprintf("Previous song is: %s.\n", songName);
				i = 0;
				xQueueSend(osHandles->queue.songname, &songName, 100);

				//xQueueSend(osHandles->queue.songname, &songs.rotateLeft().name, 500);
				//initialize_sta013(); // performs reset
				sta013StartDecoder();
				break;
				//Pause | Resume
			case PauseResume:
				rprintf("Pause\n");
				if (pauseResume == 0)
				{
					sta013PauseDecoder();
					pauseResume = 1;
				}
				else
				{
					rprintf("Resume\n");
					sta013ResumeDecoder();
					sta013StartDecoder();
					pauseResume = 0;
				}
				break;
				//Incase Volume
			case VolUp:
				if (volume == 100)
				{
					rprintf("Maximum Volume Reached \n");
				}
				else
				{
					volume+=5;
					rprintf("Volume Level %d%%\n", volume);
					pcm1774_OutputVolume(volume, volume);
				}
				break;
				//Lower Volume
			case VolDown:
				if (volume == 0)
				{
					rprintf("Muted \n");
				}
				else
				{
					volume-=5;
					rprintf("Volume Level %d%%\n", volume);
					pcm1774_OutputVolume(volume, volume);
				}
				break;
			case Default:
					//rprintf("-- Waiting --\n");
				break;
			}
	/*	}
		else
		{
			char buffer[256] ={ 0 };
			wifiReceiveExpectedData(buffer, sizeof(buffer));
			char * pch;
			VECTOR<SongNameType> songsFromServer;
			SongNameType songsListFromServer;
			pch = strtok(buffer, "#");
			while (pch != NULL)
			{
				songsListFromServer = pch;
				songsFromServer.push_back(songsListFromServer);
				rprintf("%s\n", pch);
				pch = strtok(NULL, "#");
			}
			//xQueueSend(osHandles->queue.songname, &songsFromServer[0].name, 500);
		} */
	}
}
