#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment( lib, "ws2_32.lib")

using namespace std;

int main()
{

	WSADATA wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in ListenSockAddr;
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));
	ListenSockAddr.sin_family = PF_INET;
	ListenSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ListenSockAddr.sin_port = htons(30303);

	bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	listen(ListenSocket, 5);

	sockaddr_in ClientSockAddr;
	memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
	int ClientSockLength = sizeof(ClientSockAddr);

	SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*) &ClientSockAddr, &ClientSockLength);

	int RecvBytes = 0;
	int SentBytes = 0;
	char Buffer[1024] = { 0, };
	do {
		RecvBytes = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
		cout << "Recive Message : " << Buffer << endl;

		send(ClientSocket, Buffer, sizeof(Buffer), 0);
	} while (RecvBytes > 0);


	closesocket(ClientSocket);
	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}