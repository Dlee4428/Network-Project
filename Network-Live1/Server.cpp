#undef UNICODE

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"

int main() {
	WSADATA wsaData;
	int iResult;
	int iSendResult;

	// Exit
	const char* exit = "exit";

	// Socket Declaration
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	// Buffers
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		system("pause");
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		system("pause");
		return 1;
	}
	printf("getaddrininfo successful 1\n");

	ListenSocket = socket(
		result->ai_family, 
		result->ai_socktype, 
		result->ai_protocol
	);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		system("pause");
		return 1;
	}
	printf("getaddrininfo successful 2\n");

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		system("pause");
		return 1;
	}
	freeaddrinfo(result);
	printf("socket binding successfully \n");

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		system("pause");
		return 1;
	}
	printf("Waiting for connection request... \n");

	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		system("pause");
		return 1;
	}
	closesocket(ListenSocket);

	// Receive data
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d \n", iResult);
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			std::cout << "Message Received: " << recvbuf << std::endl;
			if (iSendResult == SOCKET_ERROR) {
				printf("Send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				system("pause");
				return 1;
			}
		}
		else if (iResult == 0) {
			printf("Connection closing... \n");
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			system("pause");
			return 1;
		}
	} while (iResult > 0);

	// Shutdown socket
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("Shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		system("pause");
		return 1;
	}

	// Clean up state
	closesocket(ClientSocket);
	WSACleanup();
	printf("It's over \n");
	system("pause");
	return 0;
}