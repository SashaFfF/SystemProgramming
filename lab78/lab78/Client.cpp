#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main(void) {
	const char IP_SERV[] = "";			// Enter local Server IP address
	const int PORT_NUM = 0;				// Enter Open working server port
	const short BUFF_SIZE = 1024;


	WSADATA wsData;//структура, в которую автоматически в момент создания загружаются данные о версии сокетов
	int error_status = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (error_status != 0) {
		cout << "Error WinSock version initializaion " << WSAGetLastError();
		return 1;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		cout << "Invalid socket" << WSAGetLastError();
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	//Привязка сокета к паре IP-адрес/Порт
	in_addr ipToNum;
	error_status = inet_pton(AF_INET, "127.0.0.1", &ipToNum);
	if (error_status <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;

	}

	sockaddr_in clientSockInfo;
	ZeroMemory(&clientSockInfo, sizeof(clientSockInfo));
	clientSockInfo.sin_family = AF_INET;
	clientSockInfo.sin_addr = ipToNum;
	clientSockInfo.sin_port = htons(1234);

	error_status = connect(clientSocket, (sockaddr*)&clientSockInfo, sizeof(clientSockInfo));
	if (error_status != 0) {
		cout << "Connection to server error # " << WSAGetLastError() << endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}


	vector<char> servBuffer(BUFF_SIZE), clBuffer(BUFF_SIZE);
	short packet_size = 0;
	while (true) {

		cout << "Your (Client) message to Server: ";
		fgets(clBuffer.data(), clBuffer.size(), stdin);

		// Check whether client like to stop chatting 
		if (clBuffer[0] == 's' && clBuffer[1] == 't' && clBuffer[2] == 'o' && clBuffer[3] == 'p') {
			shutdown(clientSocket, SD_BOTH);
			closesocket(clientSocket);
			WSACleanup();
			return 0;
		}

		packet_size = send(clientSocket, clBuffer.data(), clBuffer.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

		packet_size = recv(clientSocket, servBuffer.data(), servBuffer.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Can't receive message from Server. Error # " << WSAGetLastError() << endl;
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
		else
			cout << "Server message: " << servBuffer.data() << endl;

	}

	closesocket(clientSocket);
	WSACleanup();



	return 0;
}

