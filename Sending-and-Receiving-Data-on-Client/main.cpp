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
	* https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client
	*/
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

#define DEFAULT_PORT "27015"
	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET connectSocket = INVALID_SOCKET;

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (connectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	/*
	* https://docs.microsoft.com/en-us/windows/win32/winsock/connecting-to-a-socket
	*/
	//Connect to server.
	iResult = connect(connectSocket, ptr->ai_addr, ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}

	/*
	* Should really try the next address returnded by getaddrinfo
	* if the connect call failed
	* But for this simple example we just free the resources
	* returned by getaddrinfo and print an error message.
	*/

	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	/*
	* https://docs.microsoft.com/en-us/windows/win32/winsock/sending-and-receiving-data-on-the-client
	*/

#define DEFAULT_BUFLEN 512
	int recvbuflen = DEFAULT_BUFLEN;

	const char* sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];

	// int iResult;

	// Send an initial buffer
	iResult = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sents: %ld\n", iResult);

	/*
	* shutdown the connection for sending since no more data will be sent
	* the client can still use the connectSocket for receiving data
	*/
	iResult = shutdown(connectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	// Receive data until the server closes teh connection
	do {
		iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0){
			printf("Connection closed\n");
		}
		else {
			printf("recv failed: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);


	return 0;
}