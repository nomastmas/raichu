/*
 * wifi.hpp
 *
 *  Created on: May 4, 2012
 *      Author: Bailey Wu
 */

#ifndef WIFI_HPP_
#define WIFI_HPP_
/* Initilization */
bool wifiBegin();
/* WiFi Module Enters Command Mode */
bool wifiEnterCommandMode();
/* WiFi Module Exits Command Mode */
bool wifiExitCommandMode();
/* Set Network Access Point Passphrase  */
bool wifiSetWlanPhrase();
/* WiFi Module Joins Network Access Point */
bool wifiJoinWirelessNetwork();
/* WiFi Module Opens TCP Connection */
bool wifiOpenTCPConnection();
/* WiFi Module Closes TCP Connection */
bool wifiCloseTCPConnection();
/* WiFi Send PlayList to Server Side */
bool wifiSentPlayListToServer();
/* WiFi Retrieve Device ID from Module */
void wifiGetDeviceIDFromModule();
/* WiFi Send Device ID Server Side */
bool wifiSendDeviceIDToServer();
/* WiFi receive confirmation from Server Side */
bool wifiReceiveFromServer();
/* WiFi Send data */
void wifiSendString(const char* s);
/* WiFi Receive data */
void wifiReceiveExpectedData(char* s, int maxLen);
/* WiFi Parse PlayList */
void parsePlayListFromServer(char * s);
/* WiFi check connection status */
bool wifiConnectionStatus();

#endif /* WIFI_HPP_ */
