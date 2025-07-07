#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32")

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ListenSocketAddr;
	ListenSocketAddr.sin_family = PF_INET;
	ListenSocketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ListenSocketAddr.sin_port = htons(32000);
	connect(ListenSocket, (SOCKADDR*)&ListenSocketAddr, sizeof(ListenSocketAddr));

	while (true)
	{
		char Buffer[1024] = {};
		std::cin >> Buffer;
		send(ListenSocket, Buffer, 1024, 0);

		char RecvBuffer[1024] = {};
		recv(ListenSocket, RecvBuffer, 1024, 0);
		std::cout << "Server : " << RecvBuffer << std::endl;
	}

	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}