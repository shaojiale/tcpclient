#include "stdlib.h"
#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};
//消息头
struct Dataheader {
	CMD cmd;
	int dataLength;
};

struct Login {
	char userName[32];
	char passWord[32];
};

struct LoginResult
{
	int result;
};

struct Logout {
	char userName[32];
};

struct LogoutResult
{
	int result;
};
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
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

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
	while (connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		printf("连接失败\n");
		Sleep(1000);
	}
	printf("成功连接到服务器！\n");
	while (true)
	{
		char input[100] = {};
		scanf("%s", input);
		if (0 == strcmp(input, "exit"))
		{
			printf("客户端已退出！\n");
			break;
		}
		else if(0 == strcmp(input, "login"))
		{
			Login login = { "xiaoming" ,"123456"};
			Dataheader hd = { CMD_LOGIN,sizeof(login) };
			//向服务器发送消息
			send(sock, (const char*)&hd, sizeof(hd), 0);
			send(sock, (const char*)&login, sizeof(login), 0);
			//接收服务器返回消息
			Dataheader rthd = { };
			LoginResult loginret = { };
			recv(sock, (char*)&rthd, sizeof(rthd), 0);
			recv(sock, (char*)&loginret, sizeof(loginret), 0);
			printf("LoginResult：%d\n", loginret.result);
		}
		else if (0 == strcmp(input, "logout"))
		{
			Logout logout = {"xiaoming"};
			Dataheader hd = { CMD_LOGOUT,sizeof(logout) };
			//向服务器发送消息
			send(sock, (const char*)&hd, sizeof(hd), 0);
			send(sock, (const char*)&logout, sizeof(logout), 0);
			//接收服务器返回消息
			Dataheader rthd = { };
			LogoutResult logoutret = { };
			recv(sock, (char*)&rthd, sizeof(rthd), 0);
			recv(sock, (char*)&logoutret, sizeof(logoutret), 0);
			printf("LoginResult：%d\n", logoutret.result);
		}
		else
		{
			printf("请重新输入命令！\n");
		}
	}
	closesocket(sock);
	WSACleanup();
	
	return 1;
}