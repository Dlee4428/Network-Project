#include <iostream>
#include <string>
#include <vector>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

void main(int argc, char* argv[]) {
	
	////////////////////////////////////////////////////////////
	// INITIALIZE WINSOCK
	////////////////////////////////////////////////////////////

	// Structure to store the WinSock version. This is filled in
	// on the call to WSAStartup()
	WSAData data;

	// To start WinSock, the required version must be passed to
	// WSAStartup(). This server is going to use WinSock version
	// 2 so I create a word that will store 2 and 2 in hex i.e.
	// 0x0202
	// Define WinSock version
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0) {
		// Not ok! Get out quickly
		std::cout << "Can't start WinSock!" << wsOk;
		return;
	}

	////////////////////////////////////////////////////////////
	// CONNECT TO THE SERVER
	////////////////////////////////////////////////////////////

	// Socket creation, note that the socket type is datagram
	SOCKET sckt = socket(AF_INET, SOCK_DGRAM, 0);

	// Create a hint structure for the server
	sockaddr_in server;
	server.sin_family = AF_INET; // AF_INET = IPv4 addresses
	server.sin_port = htons(54000);  // Little to big endian conversion
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); // Convert from string to byte array

	char recvbuf[1024];
	int serverLength = sizeof(server);

	// Input message
	std::vector<std::string> serverMsg;
	std::string line, currentWord;
	std::cout << "Send message to server here: ";
	while (std::getline(std::cin, line)) {
		std::cout << "Send message to server here: ";
		if (line == "exit") {
			break;
		}

		serverMsg.push_back(line);

		if (line == "send") {
			// Write out to that socket
			serverMsg.pop_back(); // To remove last element "send"

			for (int i = 0; i < serverMsg.size(); i++) {
				int sendOk = sendto(sckt, serverMsg[i].c_str(), serverMsg[i].size() + 1, 0, (sockaddr*)&server, serverLength);
				if (sendOk == SOCKET_ERROR) {
					std::cout << "Error! " << WSAGetLastError() << std::endl;
				}
				else {
					while (true) {
						int recvOk = recvfrom(sckt, recvbuf, sizeof(recvbuf), 0, (sockaddr*)&server, &serverLength);
						if (recvOk != SOCKET_ERROR) {
							break;
						}
					}
				}
			}
		}
	}

	// Server-Client Specific Code
	// Close Socket
	closesocket(sckt);
	// Close down Winsock
	WSACleanup();
}