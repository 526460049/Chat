#pragma comment(lib,"ws2_32.lib")
#include<winsock2.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<iostream>
#include <Ws2tcpip.h>
#define MAX_NUM  50
#define PORT 5000
#define MSG_MAX_SIZE 1000


using namespace std;

int main()
{
	int sendbyte, i, j, m, people = 0;
	char MesAll[MAX_NUM][MSG_MAX_SIZE + 25] ;
	int first[MAX_NUM] = { 0 };
	char name[MAX_NUM][25] = {0};
	char buf[MAX_NUM][MSG_MAX_SIZE] ;
	time_t   timenow;
	WSADATA wsadata;
	int err = WSAStartup(WINSOCK_VERSION, &wsadata);
	if (err != 0)
	{
		return -1;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		printf("socket() failed:%d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	sockaddr_in localaddress;
	localaddress.sin_family = AF_INET;
	localaddress.sin_port = htons(PORT);
	localaddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	err = bind(sock, (sockaddr*)&localaddress, sizeof(localaddress));
	if (err == INVALID_SOCKET)
	{
		closesocket(sock);
		WSACleanup();
		return -1;
	}

	err = listen(sock, 6);
	if (err == INVALID_SOCKET)
	{
		closesocket(sock);
		WSACleanup();
		return -1;
	}
	SOCKET client[MAX_NUM];
	for (int i = 0; i < MAX_NUM; i++)
	{
		client[i] = INVALID_SOCKET;
	}

	fd_set rset, allset;
	FD_ZERO(&allset);
	FD_SET(sock, &allset);
	while (1)
	{
		rset = allset;
		int ret = select(0, &rset, NULL, NULL, NULL);
		if (ret == SOCKET_ERROR)
		{
			break;
		}


		if (ret == 0) {
			continue;
		}

		if (FD_ISSET(sock, &rset))
		{
			sockaddr_in clientaddr;
			int len = sizeof(clientaddr);
			SOCKET sockconn = accept(sock, (sockaddr*)&clientaddr, &len);
			if (sockconn == INVALID_SOCKET)
			{
				break;
			}
			for (i = 0; i < MAX_NUM; i++)
			{
				if (client[i] == INVALID_SOCKET)
				{
					client[i] = sockconn;
					break;
				}
			}
			if (i < MAX_NUM)
			{
				FD_SET(sockconn, &allset);
			}

			else {
				cout<<"too many client"<<endl;
				closesocket(sockconn);
			}

		}
		for (i = 0; i < MAX_NUM; i++)
		{
			if ((client[i] != INVALID_SOCKET) && FD_ISSET(client[i], &rset))
			{
				memset(buf[i], 0, MSG_MAX_SIZE);
				int recbyte = recv(client[i], buf[i], MSG_MAX_SIZE, 0);//收到数据
				if (recbyte == SOCKET_ERROR)
				{
					first[i] = 0;//若退出，则将客户信息置为初始值
					people = 0;
					for (m = 0; m < MAX_NUM; m++)
						if (first[m] == 1)
							people++;
					printf("%s exit the chat room.目前在线人数：%d.\n", name[i], people);
					strcpy_s(MesAll[i], "From server: ");
					strcat_s(MesAll[i], name[i]);
					strcat_s(MesAll[i], " 下线.");
					FD_CLR(client[i], &allset);
					closesocket(client[i]);
					client[i] = INVALID_SOCKET;
					//告诉其他客户有人下线
					for (j = 0; j < MAX_NUM; j++)
					{
						if ((client[j] != INVALID_SOCKET))
						{
							sendbyte = send(client[j], MesAll[i], 1024, 0);
							if (sendbyte == SOCKET_ERROR)
							{
								printf("send() failed:%d\n", WSAGetLastError());
							}

						}
					}
					continue;
				}
				//如果有新人进入，则给客户端发送提示消息
				if (!first[i])
				{
					strcpy_s(MesAll[i], "From server: ");
					strcat_s(MesAll[i], buf[i]);
					strcat_s(MesAll[i], " 上线.");
				}
				else
				{
					strcpy_s(MesAll[i], name[i]);
					strcat_s(MesAll[i], ": ");
					strcat_s(MesAll[i], buf[i]);
				}
				for (j = 0; j < MAX_NUM; j++)
				{
					if ((client[j] != INVALID_SOCKET))
					{
						sendbyte = send(client[j], MesAll[i], 1000, 0);
						if (sendbyte == SOCKET_ERROR)
						{
							printf("send() failed:%d\n", WSAGetLastError());
						}

					}
				}
				time(&timenow);
				//服务器端显示新进客户的信息
				if (!first[i])
				{
					people = 0;
					for (m = 0; m < MAX_NUM; m++)
						if (first[m] == 1)
							people++;
					strcpy_s(name[i], buf[i]);
					printf("目前在线人数：%d\n", people + 1);
					first[i] = 1;
				}
				else
					cout << name[i] << buf[i] << endl;

			}
		}

	}
	closesocket(sock);
	WSACleanup();
	return 0;
}