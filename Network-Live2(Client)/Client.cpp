#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define EXIT "exit"
#define DEFAULT_PORT "27016"
#define DEFAULT_HOST "localhost"

int main() {
	WSADATA wsaData;

	SOCKET ConnectSocket = INVALID_SOCKET;

	int iResult;
	bool isExit = false;

	char sendbuf[] = "hello world";
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	// Init Library
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error code: %d\n", iResult);
		WSACleanup();
		system("pause");
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(DEFAULT_HOST, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		system("pause");
		return 1;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("error at socket(): %d\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
		iResult = connect(ConnectSocket, ptr->ai_addr, (ptr)->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// Proceed Do While loop for send() recv()
	do {
		// SEND
		std::cout << "Send messages to server: ";
		std::string temp = sendbuf;
		std::getline(std::cin, temp);
		iResult = send(ConnectSocket, temp.c_str(), strlen(temp.c_str()) + 1, 0);

		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			WSACleanup();
			system("pause");
			return 1;
		}

		printf("bytes sent: %d\n", iResult);

		// RECV
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("bytes received: %d\n", iResult);
		}
		else if(iResult == 0){
			printf("Connection closed\n");
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
		}

		// If msg "exit" input, process shutdown stage to close socket and clean up
		if (temp == EXIT) {
			iResult = shutdown(ConnectSocket, SD_SEND);
			closesocket(ConnectSocket);
			WSACleanup();
			system("pause");
			return 0;
		}

	} while (iResult > 0);

	closesocket(ConnectSocket);
	WSACleanup();

	system("pause");
	return 0;
}