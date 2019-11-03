#include "stdlib.h"
#include <stdio.h>
#include <ctime>
#include <WinSock2.h>
#include <iostream>
#include <thread>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
//消息头
struct Dataheader {
	CMD cmd;
	int dataLength;
};

struct Login :public Dataheader {
	Login()
	{
		cmd = CMD_LOGIN;
		dataLength = sizeof(Login);
	}
	char userName[32];
	char passWord[32];
};

struct LoginResult :public Dataheader {
	LoginResult()
	{
		cmd = CMD_LOGIN_RESULT;
		dataLength = sizeof(LoginResult);
		result = 0;
	}
	int result;
};

struct Logout :public Dataheader {
	Logout()
	{
		cmd = CMD_LOGOUT;
		dataLength = sizeof(Logout);
	}
	char userName[32];
};

struct LogoutResult :public Dataheader {
	LogoutResult()
	{
		cmd = CMD_LOGOUT_RESULT;
		dataLength = sizeof(LogoutResult);
		result = 0;
	}
	int result;
};
struct NewUserJoin :public Dataheader {
	NewUserJoin()
	{
		cmd = CMD_NEW_USER_JOIN;
		dataLength = sizeof(NewUserJoin);
		clientSocket = 0;
	}
	int clientSocket;
};
int Processor(SOCKET clientSocket)
{
	//接受缓存
	char szRecv[4096] = {};
	int cnt = recv(clientSocket, szRecv, sizeof(Dataheader), 0);
	Dataheader* header = (Dataheader*)szRecv;
	if (cnt <= 0)
	{
		//if ((cnt < 0) && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		//{
		//	continue;//继续接收数据
		//}
		printf("和服务器已经退出，任务结束\n", (int)clientSocket);
		return -1;//跳出接收循环
	}
	//正常处理数据
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(clientSocket, szRecv + sizeof(header), header->dataLength - sizeof(header), 0);
		LoginResult *ret = (LoginResult*)szRecv;
		printf("收到服务端消息：CMD_LOGIN_RESULT，CMD_LOGIN 数据长度：%d\n", ret->dataLength);
		break;
	}
	case CMD_LOGOUT_RESULT:
	{
		recv(clientSocket, szRecv + sizeof(header), header->dataLength - sizeof(header), 0);
		LoginResult* ret = (LoginResult*)szRecv;
		printf("收到服务端消息：CMD_LOGOUT_RESULT，CMD_LOGIN 数据长度：%d\n", ret->dataLength);
		break;
	}
	case CMD_NEW_USER_JOIN:
	{
		recv(clientSocket, szRecv + sizeof(header), header->dataLength - sizeof(header), 0);
		NewUserJoin* ret = (NewUserJoin*)szRecv;
		printf("收到服务端消息：CMD_NEW_USER_JOIN，CMD_LOGIN 数据长度：%d\n", ret->dataLength);
		break;
	}
	default:
		break;
	}
}
double timethis;
double timelast = timethis;
bool g_bRun = true;
void cmdthread(SOCKET clientSocket)
{
	char input[100] = {};
	while (true)
	{
		scanf("%s", input);
		if (0 == strcmp(input, "exit"))
		{
			g_bRun = false;
			printf("客户端已退出！\n");			
			break;
		}
		else if(0 == strcmp(input, "login"))
		{
			Login login;
			strcpy(login.userName, "xiaoming");
			strcpy(login.passWord, "123456");
			//向服务器发送消息
			send(clientSocket, (const char*)&login, sizeof(login), 0);
		}
		else if (0 == strcmp(input, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "xiaoming");
			//向服务器发送消息
			send(clientSocket, (const char*)&logout, sizeof(logout), 0);
		}
		else
		{
			printf("请重新输入命令！\n");
		}
	}
	
}
int main()
{
	//初始化DLL
	//主版本号(低字节)为2，副版本号为2，返回 0x0202
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	//WSAStartup() 函数执行成功后，会将与 ws2_32.dll 有关的信息写入 WSAData 结构体变量
	//(只需关注前两个成员即可)
	if (WSAStartup(sockVersion, &wsaData) != 0)
		return 0;

	//创建套接字，socket() 函数用来创建套接字，确定套接字的各种属性
	//IPv4 地址、面向连接的数据传输方式、TCP 传输协议
	SOCKET clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//向服务器发起请求,将创建的套接字与服务器IP地址、端口 3000 绑定：
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;

	//32位IP地址,s_addr 是一个整数(unsigned long)，而IP地址是一个字符串，
	//所以需要 inet_addr() 函数进行转换
	sockAddr.sin_addr.s_addr = inet_addr("192.168.0.106");

	//16位的端口号,端口号需要用 htons() 函数转换,0~1023 的端口一般由系统分配给特定的服务程序，
	//尽量在 1024~65536 之间分配端口号。
	sockAddr.sin_port = htons(3000);

	//强制类型转换,服务器端要用 bind() 函数将套接字与特定的IP地址和端口绑定起来，
	//只有这样，流经该IP地址和端口的数据才能交给套接字处理
	while (connect(clientSocket, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		printf("连接失败\n");
		Sleep(1000);
	}
	printf("成功连接到服务器！\n");
	//启动cmd输入线程
	std::thread t1(cmdthread,clientSocket);
	t1.detach();
	while (g_bRun)
	{
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(clientSocket, &fdRead);
		timeval timeout = { 1,0 };
		int ret = select(clientSocket + 1, &fdRead, NULL, NULL, &timeout);
		if (ret < 0)
		{
			printf("select任务已经退出，任务结束\n");
			break;
		}
		if (FD_ISSET(clientSocket, &fdRead))
		{
			FD_CLR(clientSocket, &fdRead);
			if (-1 == Processor(clientSocket))
			{
				printf("服务端任务已经退出，任务结束\n");
				break;
			}
		}
		//timelast = timethis;
		//timethis = clock();
		//if ((double)(timethis - timelast) / CLOCKS_PER_SEC >= 1)
		//{
		//	/*printf("time: %f Do something else!!!!!!!!！\n", (double)(timethis - timelast) / CLOCKS_PER_SEC);*/
		//}
		
	}
	closesocket(clientSocket);
	WSACleanup();
	
	return 1;
}