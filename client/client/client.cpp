#pragma comment(lib,"ws2_32.lib")
#include<winsock2.h>
#include<stdio.h>
#include<string.h>
#include<windows.h>
#include<PROCESS.H>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h> 
#include <cstdio>
#include <iostream>
#include <ws2tcpip.h> 
#define PORT 5000

using namespace std;
int first = 1;
//监视输入的线程函数
void  getInput(void* param)
{
	SOCKET sock = (SOCKET)param;
	while (1)
	{
		char buf[1024];
		gets_s(buf);
		int sendbyte = send(sock, buf, strlen(buf), 0);
		if (sendbyte == SOCKET_ERROR)
		{
			printf("send() failed:%d\n", WSAGetLastError());
		}
	}
}
int main()
{
	char flag, flag1;
	int loginmax = 3;
	int connectmax = 5;
	int i = 1;
	int flag2;
	int flag3 = 0;
	time_t timenow;
	WSADATA wsadata1;
	char name[15] ;
	char password[30];
	flag1 = 'y';
	//输口令
	do {
		cout<<"Please input the password:"<<endl;
		gets_s(password);
		if (strcmp(password, "123") == 0)
			break;
		else
		{
			loginmax -= 1;
			if (loginmax)
				cout<<"You have %d times to change the password!"<<loginmax<<endl;
			else
				printf("The system will be closed!\n");
		}
	} while (loginmax);
	if (loginmax)
	{
		cout<<"Please input your name:"<<endl;
		gets_s(name);
		do {
			cout<<"Do you want to establish a connection with the server?(y/n):"<<endl;
			cin >> flag;
			//建立连接
			if (flag == 'y')
			{
				connectmax--;
				i = 0;
				int err = WSAStartup(WINSOCK_VERSION, &wsadata1);
				if (err != 0)
				{
					return -1;
				}
				SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
				if (sock == INVALID_SOCKET)
				{
					WSACleanup();
					return -1;
				}
				sockaddr_in serveraddress;
				serveraddress.sin_family = AF_INET;
				serveraddress.sin_port = htons(PORT);
				string ip = "127.0.0.1";
				inet_pton(AF_INET, ip.data(), (void*)&serveraddress.sin_addr.s_addr);
//				serveraddress.sin_addr.S_un.S_addr = inet_pton("127.0.0.1");
				do {
					if (flag3)
					{
						connectmax--;
						if (connectmax)
							cout<<"connecting..."<<endl;
						else
						{
							return -1;
						}
					}
					flag3 = 1;
					err = connect(sock, (sockaddr*)&serveraddress, sizeof(serveraddress));
					if (err == INVALID_SOCKET)
					{
						getchar();
						cout<<"If you want to try again,please press 'y':"<<endl;
						cin >> flag1;
						if (flag1 == 'y')
							continue;
						else
						{
							closesocket(sock);
							WSACleanup();
							return -1;
						}
					}
					else
						break;
				} while (flag1 == 'y');
				//连接成功
				send(sock, name, strlen(name), 0);
				flag2 = recv(sock, name, 100, 0);
				if (flag2 == SOCKET_ERROR)
				{
					return -1;
				}
				system("cls");
				cout<<"Welcome to the chatroom!\n"<<endl;
				_beginthread(getInput, 0, (void*)sock);
				Sleep(1000);
				char rbuf[1024];
				while (1)
				{
					memset(rbuf, 0, 1024);
					int recbyte = recv(sock, rbuf, 1000, 0);
					if (recbyte == SOCKET_ERROR)
					{
						closesocket(sock);
						WSACleanup();
						return -1;
					}
					timenow = time(NULL);
					if (first)
					{
						first = 0;
						continue;
					}
					printf("%s\n",rbuf);
					Sleep(1000);
				}
				closesocket(sock);
				WSACleanup();
				return 0;
			}
			else if (flag == 'n')
				return 0;
			else
			{
				getchar();
			}
		} while (i);
	}
}
