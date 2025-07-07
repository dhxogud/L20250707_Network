#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define FD_SETSIZE      100 // 재정의

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
		// select 함수 리뷰
		// fd_set : File Descripter(파일 설명자) 의 집합 구조체, 명칭은 리눅스 운영체제에서 따온거지만, 윈도우에서 저걸 소켓 구조체용으로 따로 구현해둠
		// FD_ZERO : fd_set 집합 구조체의 초기화
		// FD_SET : fd_set 집합 구조체 안에 File Descripter 구조체를 집어넣기 (Add 추가)
		// *exceptfds : 예외 상황(에러 아님), 다른 방식의 프로토콜을 감지하기 위해 쓰지만 지금은 필요없음
		// timeval timeout : CPU가 OS에게 작업 처리 완료여부를 물어보는 시간 간격, 
			// 그럼 걍 float 쓰면 될것이지 왜 구조체 형태냐고? 예전에 Cpu가 float 연산처리를 못해서..

		fd_set RecvSocketCopys;
		// 원본은 가지고 있고 복제본을 줌
		RecvSocketCopys = ReadSockets;

		struct timeval Timeout;
		Timeout.tv_sec = 0;
		Timeout.tv_usec = 1000;

		//polling : CPU가 OS에게 처리할 소켓의 변경된 개수(혹은 task 개수라고 생각해도) 를 물어보는 동작
		int ChangeSocketCount = select(0, &RecvSocketCopys, NULL, NULL, &Timeout);

		char RecvBuffer[1024] = {};

		if (ChangeSocketCount <= 0) // 즉, 클라이언트 소캣의 개수가 전과 달라지지 않음을 의미
		{
			//다른 서버 작업
			//std::cout << "서버 작업해야지" << std::endl;

			continue;
		}
		//네트워크 작업
		else
		{
			for (int i = 0; i < (int)ReadSockets.fd_count; ++i)
			{
				// FD_ISSET : 원본과 복제본의 달라진 점 확인
				if (FD_ISSET(ReadSockets.fd_array[i], &RecvSocketCopys))
				{
					// 맨 처음 ReadSockets 에 들어간 소켓은 서버의 ListenSocket 하나이기 때문에 맨 처음에 변경된 ChangeSocketCount = 1 이라면,
					// 무조건 ListenSocket이다.
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