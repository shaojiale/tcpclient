#ifndef _TCPSERVER_HPP
#define _TCPSERVER_HPP
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib ")
#endif  //end define _win32

#include <stdio.h>
#include "MessageHeader.hpp"
class TcpClient
{
	SOCKET _sock;
public:
	TcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~TcpClient()
	{
		Close();
	}
	//初始化socket
	void InitSocket()
	{
		//启动WinSocket2.x环境\
		//主版本号(低字节)为2，副版本号为2，返回 0x0202
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(sockVersion, &wsaData);
		//建立一个socket
		if (INVALID_SOCKET != _sock)
		{
			printf("关闭<Socket:%d>旧连接。。。\n", (int)_sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			printf("错误，建立Socket失败。。。\n");
		}
		else
		{
			printf("建立Socket成功！\n");
		}
	}
	//连接服务器
	int Connect(char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		_sin.sin_port = htons(port);
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			printf("连接失败\n");
		}
		else
		{
			printf("成功连接到服务器！\n");
		}
		return ret;
	}
	//关闭socket
	void Close()
	{
		if (INVALID_SOCKET != _sock)
		{
			closesocket(_sock);
			WSACleanup();
			_sock = INVALID_SOCKET;
		}
	}
	//接受数据 处理粘包拆包
	int RecvData(SOCKET _sock)
	{
		//接受缓存
		char szRecv[4096] = {};
		int cnt = (int)recv(_sock, szRecv, sizeof(Dataheader), 0);
		Dataheader* header = (Dataheader*)szRecv;
		if (cnt <= 0)
		{
			printf("服务器已经退出，任务结束\n");
			return -1;//跳出接收循环
		}
		recv(_sock, szRecv + sizeof(header), header->dataLength - sizeof(header), 0);
		OnNetMsg(header);
		return 0;
	}
	//响应网络消息
	virtual void OnNetMsg(Dataheader* header)
	{
		//正常处理数据
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult* ret = (LoginResult*)header;
			printf("收到服务端消息：CMD_LOGIN_RESULT，CMD_LOGIN 数据长度：%d\n", ret->dataLength);
			break;
		}
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult* ret = (LogoutResult*)header;
			printf("收到服务端消息：CMD_LOGOUT_RESULT，CMD_LOGIN 数据长度：%d\n", ret->dataLength);
			break;
		}
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* ret = (NewUserJoin*)header;
			printf("收到服务端消息：CMD_NEW_USER_JOIN，CMD_LOGIN 数据长度：%d\n", ret->dataLength);
			break;
		}
		default:
			break;
		} 
	}
	//处理网络消息
	//运用到非阻塞模型
	//多路复用IO
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdRead;
			FD_ZERO(&fdRead);
			FD_SET(_sock, &fdRead);
			timeval timeout = { 1,0 };
			int ret = select(_sock + 1, &fdRead, NULL, NULL, &timeout);
			if (ret < 0)
			{
				printf("select任务已经退出，任务结束\n");
				return false;
			}
			if (FD_ISSET(_sock, &fdRead))
			{
				FD_CLR(_sock, &fdRead);
				if (-1 == RecvData(_sock))
				{
					printf("服务端任务已经退出，任务结束\n");
					return false;
				}
			}
			return true;
		}
		return false;
	}
	//socket有效
	bool isRun()
	{ 
		return _sock != INVALID_SOCKET;
	}

	//发送数据
	int SendData(Dataheader* header)
	{
		if (isRun() && header)
		{
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}
private:

};



#endif // !_TCPCLIENT_HPP
