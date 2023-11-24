#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main(void) {			
	const short BUFF_SIZE = 1024;	

	WSADATA wsData;
	//инициализация скетных интерфейсов Win32Api
	int error_status = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (error_status != 0) {
		cout << "Error WinSock version initializaion " << WSAGetLastError();
		return 1;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		cout << "Invalid socket" << WSAGetLastError();
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	//Привязка сокета к паре IP-адрес/Порт
	in_addr ipToNum;
	// преобразование IP-адреса в текстовом формате в двоичный формат
	error_status = inet_pton(AF_INET, "127.0.0.1", &ipToNum);
	if (error_status <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;

	}

	sockaddr_in servSockInfo;
	ZeroMemory(&servSockInfo, sizeof(servSockInfo));
	servSockInfo.sin_family = AF_INET;
	servSockInfo.sin_addr = ipToNum;
	servSockInfo.sin_port = htons(1234);

	error_status = bind(serverSocket, (sockaddr*)&servSockInfo, sizeof(servSockInfo));
	if (error_status != 0) {
		cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	error_status = listen(serverSocket, SOMAXCONN);
	if (error_status != 0) {
		cout << "Error listen() " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));
	int clientInfo_size = sizeof(clientInfo);
	SOCKET ClientConn = accept(serverSocket, (sockaddr*)&clientInfo, &clientInfo_size);
	if (ClientConn == INVALID_SOCKET) {
		cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	}

	vector<char> servBuffer(BUFF_SIZE), clBuffer(BUFF_SIZE);
	short packet_size = 0;

	while (true) {
		packet_size = recv(ClientConn, servBuffer.data(), servBuffer.size(), 0);
		cout << "Client's message: " << servBuffer.data() << endl;

		cout << "Your (host) message: ";
		fgets(clBuffer.data(), clBuffer.size(), stdin);

		if (clBuffer[0] == 's' && clBuffer[1] == 't' && clBuffer[2] == 'o' && clBuffer[3] == 'p') {
			shutdown(ClientConn, SD_BOTH);
			closesocket(serverSocket);
			closesocket(ClientConn);
			WSACleanup();
			return 0;
		}

		packet_size = send(ClientConn, clBuffer.data(), clBuffer.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Can't send message to Client. Error # " << WSAGetLastError() << endl;
			closesocket(serverSocket);
			closesocket(ClientConn);
			WSACleanup();
			return 1;
		}

	}
	closesocket(serverSocket);
	closesocket(ClientConn);
	WSACleanup();
	return 0;
}