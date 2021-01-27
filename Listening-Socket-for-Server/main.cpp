#ifndef WIN32_LEAN_AND_MEAN //reason in https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application
#define WIN32_LEAN_AND_MEAN
#endif

#include<Windows.h>
#include<WinSock2.h>
#include<ws2tcpip.h> //for winsock 2 tcp/ip protocol
#include<stdio.h>
#include<iphlpapi.h> // for ip helper api.
#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[]) {

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

	/*
	* https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-server
	*/

#define DEFAULT_PORT "27015"

	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	// Resolve the local address and port to be used by the server.7
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET listenSocket = INVALID_SOCKET;

	// Create a SOCKET for the server to listen for client connections
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	/*
	* https://docs.microsoft.com/en-us/windows/win32/winsock/binding-a-socket
	*/
	// Setup the TCP listening socket
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}



	return 0;
}