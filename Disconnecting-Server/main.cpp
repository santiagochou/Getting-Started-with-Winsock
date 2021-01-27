#undef UNICODE

#ifndef WIN32_LEAN_AND_MEAN //reason in https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application
#define WIN32_LEAN_AND_MEAN
#endif

#include<Windows.h>
#include<WinSock2.h>
#include<ws2tcpip.h> //for winsock 2 tcp/ip protocol
#include<stdio.h>
#include<iphlpapi.h> // for ip helper api.
#pragma comment(lib, "Ws2_32.lib")

int __cdecl main(int argc, char* argv[]) {

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

	/*
	* https://docs.microsoft.com/en-us/windows/win32/winsock/listening-on-a-socket
	*/
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	/*
	* https://docs.microsoft.com/en-us/windows/win32/winsock/accepting-a-connection
	*/

	SOCKET clientSocket;

	clientSocket = INVALID_SOCKET;

	// Accept a client socket
	clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	/*
	* https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code
	*/
	// No longer need server socket
	closesocket(listenSocket);

	/*
	* https://docs.microsoft.com/en-us/windows/win32/winsock/receiving-and-sending-data-on-the-server
	*/
#define DEFAULT_BUFLEN 512
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	const char* sendbuf = "this is a message from server";

	// Receive until the peer shuts down the connection
	do {
		iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d, Message: %.29s\n", iResult, recvbuf);

			//Echo the buffer back to the sender
			iSendResult = send(clientSocket, sendbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d, Message: %s\n", iSendResult, sendbuf);
		}
		else if (iResult == 0) {
			printf("Connection closing...\n");
		}
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);

	/*
	* https://docs.microsoft.com/en-us/windows/win32/winsock/disconnecting-the-server
	*/
	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}