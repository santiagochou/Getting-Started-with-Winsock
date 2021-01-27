#ifndef WIN32_LEAN_AND_MEAN //reason in https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application
#define WIN32_LEAN_AND_MEAN
#endif

#include<Windows.h>
#include<WinSock2.h>
#include<ws2tcpip.h> //for winsock 2 tcp/ip protocol
#include<stdio.h>
#include<iphlpapi.h> // for ip helper api.
#pragma comment(lib, "Ws2_32.lib")

int main() {

	WSADATA wsaData; //https://docs.microsoft.com/en-us/windows/win32/winsock/initializing-winsock
	int iResult; //Learning the naming method at windows programming.

	// Initialize Winsock
	/*
	* The MAKEWORD(2,2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system,
	* and sets the passed version as the highest version of Windows Sockets support that the caller can use.
	*/
	iResult = WSAStartup(MAKEWORD(2, 3), &wsaData); // The WSAStartup function is called to initiate use of WS2_32.dll
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}



	return 0;
}