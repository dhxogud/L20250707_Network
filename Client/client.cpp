#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment( lib, "ws2_32.lib")

using namespace std;


int main()
{
	WSADATA wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = PF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerSockAddr.sin_port = htons(30303);
	
	connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	int RecvBytes = 0;
	int SentBytes = 0;
	char Message[1024] = { "Hello Server?" };
	char Buffer[1024] = { 0, };
	do {
		send(ServerSocket, Message, sizeof(Message), 0);
		recv(ServerSocket, Buffer, sizeof(Buffer), 0);
		cout << Buffer << endl;

	} while (true);


	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}