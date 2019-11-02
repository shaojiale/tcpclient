#include "stdlib.h"
#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

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

	//向服务器发起请求,将创建的套接字与IP地址、端口 3000 绑定：
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
		char recvBuf[100] = { 0 };
		std::cin >> input;

		if (0 == strcmp(input, "XXX"))
		{
		}
		else if (0 == strcmp(input, "exit"))
		{
			std::cout << "客户端已退出" << std::endl;
			break;
		}
		else
		{
			send(sock, input, sizeof(input) + 1, 0);//发送msgbuf中的内容。如果长度确定则不应该每次都重新计算
			recv(sock, recvBuf, 100, 0);
			std::cout << recvBuf << std::endl;
		}
	}
	

	closesocket(sock);
	WSACleanup();
	
	return 1;
}