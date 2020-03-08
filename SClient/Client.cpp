#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "MySerialization.h"


#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "27035"

int main()
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	char recvbuf[sizeof(MySerialization)];
	int iResult;
	int zombieCount = 0;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		system("pause");
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		system("pause");
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %d\n", WSAGetLastError());
			WSACleanup();
			system("pause");
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
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
		system("pause");
		return 1;
	}



	MySerialization msg;

	char* tmp = reinterpret_cast<char*>(&msg);

	//Receive until the peer closes the connection
	do {
		// Send an initial buffer
		iResult = send(ConnectSocket, tmp, sizeof(MySerialization), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			system("pause");
			return 1;
		}
		printf("Bytes Sent: %d\n", iResult);
		iResult = recv(ConnectSocket, recvbuf, sizeof(MySerialization), 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);

			MySerialization*response = reinterpret_cast<MySerialization*>(recvbuf);
			msg = MySerialization(*response);

			std::cout << "\nName: " << msg.plantNAME.c_str() << std::endl;
			std::cout << "HP: " << std::to_string(msg.plantHP) << std::endl;
			std::cout << "DAMAGE: " << std::to_string(msg.plantDAMAGE) << std::endl;

			std::cout << "\nName: " << msg.zombieNAME.c_str() << std::endl;
			std::cout << "HP: " << std::to_string(msg.zombieHP) << std::endl;
			std::cout << "DAMAGE: " << std::to_string(msg.zombieDAMAGE) << "\n" << std::endl;
			std::cout << "\nNumber of Zombies sent: " << " : [" << zombieCount << "]\n" << std::endl;
			zombieCount++;
		} 
		else if (iResult == 0) {
			printf("Connection closed\n");
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
		}
		Sleep(3000); // The interval is 3 sec.

		// If PLANT HP is less than or equal to 0 proceed to shutdown
		if (msg.plantHP <= 0) {
			printf("\nPlant Lose, Winner is Zombie!\n");
			// shutdown the connection since no more data will be sent
			iResult = shutdown(ConnectSocket, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				printf("shutdown failed with error: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				system("pause");
				return 1;
			}
		}
	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	system("pause");
	return 0;
}