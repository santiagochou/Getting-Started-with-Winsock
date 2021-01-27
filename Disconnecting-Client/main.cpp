#ifndef WIN32_LEAN_AND_MEAN //reason in https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application
#define WIN32_LEAN_AND_MEAN
#endif

#include<Windows.h>
#include<WinSock2.h>
#include<ws2tcpip.h> //for winsock 2 tcp/ip protocol
#include<stdio.h>
#include<iphlpapi.h> // for ip helper api.
#pragma comment(lib, "Ws2_32.lib")

/*
* https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code
*/
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

int __cdecl main(int argc, char* argv[]) {

	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

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
	// the call to getaddrinfo until one succeeds.

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
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
		break;
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

	const char* sendbuf = "this is a message from client";
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

	printf("Bytes Sents: %ld, Message: %s\n", iResult, sendbuf);


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
			printf("Bytes received: %d, Message: %.29s\n", iResult, recvbuf);
		}
		else if (iResult == 0) {
			printf("Connection closed\n");
		}
		else {
			printf("recv failed: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);

	/*
	* https://docs.microsoft.com/en-us/windows/win32/winsock/disconnecting-the-client
	*/
	//cleanup
	closesocket(connectSocket);
	WSACleanup();

	return 0;

	/*
	* The whole code, please refer to:
	* https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code
	*/
}