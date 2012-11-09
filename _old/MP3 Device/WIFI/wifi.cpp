/*
 * wifi.cpp
 *
 *  Created on: May 4, 2012
 *      Author: Bailey Wu
 */
#include "wifi.hpp"
#include "../drivers/uart1.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../utils/rprintf.h"
#include "../MP3/mp3.hpp"
#include "utils/vector.hpp"
#include "../osHandles.h"
#include "../fat/ff.h"
#include "../drivers/sta013.h"
#include "../drivers/ssp_spi.h"
#include "../drivers/sta013.h"
#include "../drivers/pcm1774.h"

const char commandOpCode [] = "$$$"; // command mode opCode
const char exitOpCode [] = "exit"; // exit commad mode opCode
const char closeConnectionopCode [] = "close";  //close TCP connection opCode
const char wifiSSID[]="join NETGEAR"; //<usage: join [options]>
const char serverIPAddress[] ="open 130.65.157.219 33333";//<usage: open [options][options]>
//const char serverIPAddress[] ="open 130.65.178.141 2222";
const char setWlanPhrase[] = "set wlan phrase 123abc456def"; //<usage: set wlan phrase [options]>
const char enterOpCode [] = "\r"; //enter
const char songNameDelimeter[]="#"; // delimeter for each song
const char deviceIDBuffer[] = "DeviceId=raichu_mp3_1";
const char wifiStatusOpCode[] ="show io";
//char deviceIDBuffer[128] = {0}; // buffer to hold device id
const char MP3List[]= "MP3List:";

typedef struct SongNameType
{
	char name[24];
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
 * Initilization Process
 * Check for command mode status
 *
 */

bool wifiBegin()
{
	if(wifiEnterCommandMode())
	{
		rprintf("Entered Command Mode!\n");
		return true;
	}
	else
	{
		rprintf("Fail to Entered Command Mode!\n");
		if(wifiExitCommandMode())
		{
			rprintf("Exit Command Mode!\n");
			wifiEnterCommandMode();
		}
		else
			rprintf("Fail to Exit Command Mode!\n");
		return true;
	}
}
/*
 * Enter Command mode
 */
bool wifiEnterCommandMode()
{
	char buffer[128] ={ 0 };
	wifiSendString(commandOpCode);
	wifiReceiveExpectedData(buffer, sizeof(buffer));
	rprintf("%s\n", buffer);
	if (strstr(buffer, "CMD"))
		return true;
	else
		return false;
}
/*
 * Exit Command mode
 */
bool wifiExitCommandMode()
{
	bool exitCommandMode = false;
	while(!exitCommandMode)
	{
		char buffer[128] ={ 0 };
		wifiSendString(exitOpCode);
		wifiSendString(enterOpCode);
		wifiReceiveExpectedData(buffer,sizeof(buffer));
		rprintf("%s\n", buffer);
		if (strstr(buffer, "EXIT"))
		{
			exitCommandMode = true;
			return true;
		}
	}
	return false;
}
/*
 * Set SSID Wlan Phrase
 */
bool wifiSetWlanPhrase()
{
	bool setWlanPhraseSuccessful = false;
	while(!setWlanPhraseSuccessful)
	{
		char buffer[128] ={ 0 };
		wifiSendString(setWlanPhrase);
		wifiSendString(enterOpCode);
		wifiReceiveExpectedData(buffer,sizeof(buffer));
		rprintf("%s\n", buffer);
		if (strstr(buffer, "AOK"))
		{
			setWlanPhraseSuccessful = true;
			return true;
		}
	}
	return false;
}
/*
 * Join SSID network
 */
bool wifiJoinWirelessNetwork()
{
	bool joinSuccessful = false;
	while(!joinSuccessful)
	{
		char buffer[256] ={ 0 };
		wifiSendString(wifiSSID);
		wifiSendString(enterOpCode);
		wifiReceiveExpectedData(buffer,sizeof(buffer));
		rprintf("%s\n", buffer);
		if (strstr(buffer, "Associated!"))
		{
			rprintf("Joined Wireless Network!\n");
			joinSuccessful = true;
			return true;
		}
	}
	return false;
}
/*
 *Establish TCP connection
 */
bool wifiOpenTCPConnection()
{
	bool connectionEstablished = false;
	while(!connectionEstablished)
	{
		char buffer[256] ={ 0 };
		wifiSendString(serverIPAddress);
		wifiSendString(enterOpCode);
		wifiReceiveExpectedData(buffer,sizeof(buffer));
		rprintf("%s\n", buffer);
		if(strstr(buffer,"*OPEN*"))
		{
			rprintf("Connection opened!\n");
			connectionEstablished=true;
			return true;
		}
	}
	return false;
}
/*
 * Close TCP Connection
 */
bool wifiCloseTCPConnection()
{
	vTaskDelay(500); // wait for 1/2 sec before closing connection
	bool connectionClosed = false;
	while(!connectionClosed)
	{
		char buffer[256] = {0};
		wifiBegin(); // enter command code before sending close connection op code
		wifiSendString(closeConnectionopCode);
		wifiSendString(enterOpCode);
		wifiReceiveExpectedData(buffer,sizeof(buffer));
		rprintf("%s\n", buffer);
		if (strstr(buffer, "*CLOS*"))//compare the result
		{
			rprintf("Connection Closed!\n");
			connectionClosed = true; // confim the connection if closed
			return true;
		}
	}
	return false;
}
bool wifiSentPlayListToServer()
{
		vTaskDelay(100); // wait for SD card to be mounted
		VECTOR<SongNameType> songs;
		SongNameType songsList;
		DIR Dir;
		FILINFO Finfo;
		FRESULT returnCode = FR_OK;

		unsigned int fileBytesTotal, numFiles, numDirs;
		fileBytesTotal = numFiles = numDirs = 0;
		#if _USE_LFN
		char Lfname[256];
			#endif
		char dirPath[] = "0:";
		if (FR_OK != (returnCode = f_opendir(&Dir, dirPath)))
		{
			rprintf("--Error:Invalid directory: |%s|\n", dirPath);
		}
		rprintf("--Sent PlayList to Server--\n");
		wifiSendString(MP3List); //sent notification of MP3List:
		for (;;)
		{
			#if _USE_LFN
			Finfo.lfname = Lfname;
			Finfo.lfsize = sizeof(Lfname);
			#endif
			char returnCode = f_readdir(&Dir, &Finfo);
			if ((FR_OK != returnCode) || !Finfo.fname[0])
				break;
			/*** Find matching .mp3 extention, then copy it into the vector ***/
			if ((strstr(Finfo.fname, ".MP3")))
			//char * got_ext = strrchr(Finfo.fname,'.');
			//if ((got_ext != NULL) && (0 == strncmp(got_ext, ".MP3", 4)) )
			{
				songsList = (char*) Finfo.fname;
				songs.push_back(songsList);
				//rprintf("%s \n", songsList.name);
				/*Sent song list to the server with delimeter*/
				wifiSendString(songsList.name);
				wifiSendString(songNameDelimeter);
			}
		}

		return true;
}
/*
void wifiGetDeviceIDFromModule()
{
	wifiSendString(deviceIDOpCode);
	wifiSendString(enterOpCode);
	wifiReceiveExpectedData(deviceIDBuffer, sizeof(deviceIDBuffer));
	rprintf("%s\n", deviceIDBuffer);
	vTaskDelay(100);
}
*/
/*
 * Send Device ID to server
 */
bool wifiSendDeviceIDToServer()
{
	bool sentDeviceIDSuccesful = false;
	wifiSendString(deviceIDBuffer);
	while(!sentDeviceIDSuccesful)
	{
		char buffer[256] ={ 0 };
		wifiReceiveExpectedData(buffer,sizeof(buffer));
		if (strstr(buffer, "ok"))
		{
			rprintf("Sent Device ID Succesfully!\n");
			sentDeviceIDSuccesful = true;
			return true;
		}
	}
	return false;
	vTaskDelay(100);
}
/*
 * Server acknowledgement
 */
bool wifiReceiveFromServer()
{
	bool serverAck = false;
	while (!serverAck)
	{
		char buffer[256] ={ 0 };
		wifiReceiveExpectedData(buffer,sizeof(buffer));
		if (strstr(buffer, "ok"))
		{
			rprintf("Server Acknowledged!\n");
			serverAck = true;
			return true;
		}
	}
	return false;
}
/*
 * Connection status confirmation
 */
bool wifiConnectionStatus()
{
	bool wifiConnectionStatus = false;
	while(!wifiConnectionStatus)
	{
		char buffer[256] = {0};
		wifiSendString(wifiStatusOpCode);
		wifiReceiveExpectedData(buffer, sizeof(buffer));
		if(strstr(buffer, "*OPEN*"))
		{
			rprintf("Connection Status Opened!\n");
			wifiConnectionStatus = true;
			return true;
		}
	}
	return false;
}
/*
 * Parse PlayList data
 */
void parsePlayListFromServer(char *s)
{
	char * pch;
	VECTOR<SongNameType> songsFromServer;
	SongNameType songsListFromServer;
	pch = strtok(s, "#");
	while(pch!=NULL)
	{
		songsListFromServer = pch;
		songsFromServer.push_back(songsListFromServer);
		rprintf("%s\n",pch);
		pch = strtok(NULL,"#");
	}
}
/*
 * Wifi send data
 */
void wifiSendString(const char* s)
{
	while (*s != 0)
	{
		uart1PutChar(*s++, portMAX_DELAY);
	}
}
/*
 * Wifi receive data
 */
void wifiReceiveExpectedData(char* s, int maxLen)
{
	int charCount = 0;
	while (uart1GetChar(s++, 1000))
	{
		if(charCount > maxLen) {
			break;
		}
	}
}


