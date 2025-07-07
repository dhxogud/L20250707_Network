#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define FD_SETSIZE      100 // ������

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32")

int main()
{
	
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ListenSocketAddr;
	memset(&ListenSocketAddr, 0, sizeof(ListenSocketAddr));
	ListenSocketAddr.sin_family = PF_INET;
	ListenSocketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ListenSocketAddr.sin_port = htons(32000);
	bind(ListenSocket, (SOCKADDR*)&ListenSocketAddr, sizeof(ListenSocketAddr));

	listen(ListenSocket, 5);

	fd_set ReadSockets;

	FD_ZERO(&ReadSockets);
	FD_SET(ListenSocket, &ReadSockets);
	while (true)
	{
		// select �Լ� ����
		// fd_set : File Descripter(���� ������) �� ���� ����ü, ��Ī�� ������ �ü������ ���°�����, �����쿡�� ���� ���� ����ü������ ���� �����ص�
		// FD_ZERO : fd_set ���� ����ü�� �ʱ�ȭ
		// FD_SET : fd_set ���� ����ü �ȿ� File Descripter ����ü�� ����ֱ� (Add �߰�)
		// *exceptfds : ���� ��Ȳ(���� �ƴ�), �ٸ� ����� ���������� �����ϱ� ���� ������ ������ �ʿ����
		// timeval timeout : CPU�� OS���� �۾� ó�� �ϷῩ�θ� ����� �ð� ����, 
			// �׷� �� float ���� �ɰ����� �� ����ü ���³İ�? ������ Cpu�� float ����ó���� ���ؼ�..

		fd_set RecvSocketCopys;
		// ������ ������ �ְ� �������� ��
		RecvSocketCopys = ReadSockets;

		struct timeval Timeout;
		Timeout.tv_sec = 0;
		Timeout.tv_usec = 1000;

		//polling : CPU�� OS���� ó���� ������ ����� ����(Ȥ�� task ������� �����ص�) �� ����� ����
		int ChangeSocketCount = select(0, &RecvSocketCopys, NULL, NULL, &Timeout);

		char RecvBuffer[1024] = {};

		if (ChangeSocketCount <= 0) // ��, Ŭ���̾�Ʈ ��Ĺ�� ������ ���� �޶����� ������ �ǹ�
		{
			//�ٸ� ���� �۾�
			//std::cout << "���� �۾��ؾ���" << std::endl;

			continue;
		}
		//��Ʈ��ũ �۾�
		else
		{
			for (int i = 0; i < (int)ReadSockets.fd_count; ++i)
			{
				// FD_ISSET : ������ �������� �޶��� �� Ȯ��
				if (FD_ISSET(ReadSockets.fd_array[i], &RecvSocketCopys))
				{
					// �� ó�� ReadSockets �� �� ������ ������ ListenSocket �ϳ��̱� ������ �� ó���� ����� ChangeSocketCount = 1 �̶��,
					// ������ ListenSocket�̴�.
					if (ReadSockets.fd_array[i] == ListenSocket)
					{
						SOCKADDR_IN ClientSocketAddr;
						int ClientSocketAddrSize = sizeof(ClientSocketAddr);
						SOCKET ClientSocket = accept(ListenSocket, 
							(SOCKADDR*)&ClientSocketAddr, 
							&ClientSocketAddrSize);
						FD_SET(ClientSocket, &ReadSockets);
						std::cout << "Connect" << std::endl;
					}
					else
					{
						char RecvBuffer[1024] = {};
						int RecvBytes = recv(ReadSockets.fd_array[i], RecvBuffer, 1024, 0);
						if (RecvBytes <= 0)
						{
							closesocket(ReadSockets.fd_array[i]);

							FD_CLR(ReadSockets.fd_array[i], &ReadSockets);

							std::cout << "Disconnect" << std::endl;
						}
						else
						{
							for (int j = 0; j < (int)ReadSockets.fd_count; ++j)
							{
								if (ReadSockets.fd_array[j] != ListenSocket)
								{
									send(ReadSockets.fd_array[j], RecvBuffer, 1024, 0);
								}
							}
						}
					}
				}
			}
		}
	}
	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}