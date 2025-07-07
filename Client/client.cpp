#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <process.h>

#pragma comment(lib,"ws2_32")

unsigned RecvThread(void* Args)
{
	SOCKET ServerSocket = *(SOCKET*)Args;

	while (true)
	{
		char RecvBuffer[1024] = {};
		recv(ServerSocket, RecvBuffer, 1024, 0);
		std::cout << "Server : " << RecvBuffer << std::endl;

	}

	return 0;
}

unsigned SendThread(void* Args)
{
	SOCKET ServerSocket = *(SOCKET*)Args;

	while (true)
	{
		char Buffer[1024] = {};
		std::cin >> Buffer; // cin ¿Ã Blocking ∞…æÓ¡‹
		send(ServerSocket, Buffer, 1024, 0);
	}


	return 0;
}

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServerSocketAddr;
	ServerSocketAddr.sin_family = PF_INET;
	ServerSocketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerSocketAddr.sin_port = htons(32000);
	connect(ServerSocket, (SOCKADDR*)&ServerSocketAddr, sizeof(ServerSocketAddr));

	HANDLE ThreadHandles[2];
	ThreadHandles[0] = (HANDLE) _beginthreadex(0, 0, RecvThread, (void*)&ServerSocket, 0, 0);
	ThreadHandles[1] = (HANDLE) _beginthreadex(0, 0, SendThread, (void*)&ServerSocket, 0, 0);

	WaitForMultipleObjects(2, ThreadHandles, TRUE, INFINITE);

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}