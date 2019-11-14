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
	int _lastPos;
public:
	TcpClient()
	{
		_sock = INVALID_SOCKET;
		_lastPos = 0;
	}
	virtual ~TcpClient()
	{
		Close();
	}
	//��ʼ��socket
	void InitSocket()
	{
		//����WinSocket2.x����\
		//���汾��(���ֽ�)Ϊ2�����汾��Ϊ2������ 0x0202
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(sockVersion, &wsaData);
		//����һ��socket
		if (INVALID_SOCKET != _sock)
		{
			printf("�ر�<Socket:%d>�����ӡ�����\n", (int)_sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			printf("���󣬽���Socketʧ�ܡ�����\n");
		}
		else
		{
			//printf("����Socket�ɹ���\n");
		}
	}
	//���ӷ�����
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
			printf("����ʧ��\n");
		}
		else
		{
			//printf("�ɹ����ӵ���������\n");
		}
		return ret;
	}
	//�ر�socket
	void Close()
	{
		if (INVALID_SOCKET != _sock)
		{
			closesocket(_sock);
			WSACleanup();
			_sock = INVALID_SOCKET;
		}
	}
	//��������С��Ԫ��С
#define RECV_BUFF_SIZE 10240
	//���ܻ�����
	char _szRecv[RECV_BUFF_SIZE] = {};
	//�ڶ������� ��Ϣ������
	char _szMsgBuf[RECV_BUFF_SIZE * 10] = {};
	//�������� ����ճ�����
	int RecvData(SOCKET _sock)
	{
		//���ܻ���
		int nLen = (int)recv(_sock, _szRecv, RECV_BUFF_SIZE, 0);
		if (nLen <= 0)
		{
				printf("�������Ѿ��˳����������\n");
				return -1;//��������ѭ��
		}
		//����ȡ�������ݿ�������Ϣ������
		memcpy(_szMsgBuf+ _lastPos, _szRecv, nLen);
		_lastPos += nLen;
		while (_lastPos >= sizeof(Dataheader))
		{
			//��ʱ���֪����Ϣ�ĳ���
			Dataheader* header = (Dataheader*)_szMsgBuf;
			if (_lastPos >= header->dataLength)
			{
				//ʣ��δ������Ϣ���峤��
				int nSize = _lastPos - header->dataLength;
				//������Ϣ
				OnNetMsg(header);
				//��δ������Ϣǰ��
				memcpy(_szMsgBuf, _szMsgBuf + header->dataLength, nSize);
				//�µ�δ������Ϣβ��
				_lastPos = nSize;
			}
			else
			{
				//û��������Ϣ��
				break;
			}
		}
		return 0;
	}
	//��Ӧ������Ϣ
	virtual void OnNetMsg(Dataheader* header)
	{
		//������������
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			//LoginResult* ret = (LoginResult*)header;
			//printf("�յ��������Ϣ��CMD_LOGIN_RESULT��CMD_LOGIN ���ݳ��ȣ�%d\n", ret->dataLength);
			break;
		}
		case CMD_LOGOUT_RESULT:
		{
			//LogoutResult* ret = (LogoutResult*)header;
			//printf("�յ��������Ϣ��CMD_LOGOUT_RESULT��CMD_LOGIN ���ݳ��ȣ�%d\n", ret->dataLength);
			break;
		}
		case CMD_NEW_USER_JOIN:
		{
			//NewUserJoin* ret = (NewUserJoin*)header;
			//printf("�յ��������Ϣ��CMD_NEW_USER_JOIN��CMD_LOGIN ���ݳ��ȣ�%d\n", ret->dataLength);
			break;
		}
		case CMD_ERROR:
		{
			printf("�յ��������Ϣ:CMD_ERROR,���ݳ��ȣ�%d\n", header->dataLength);
			break;
		}
		default:
			printf("�յ�δ֪�������Ϣ�����ݳ��ȣ�%d\n", header->dataLength);
			break;
		} 
	}
	//����������Ϣ
	//���õ�������ģ��
	//��·����IO
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdRead;
			FD_ZERO(&fdRead);
			FD_SET(_sock, &fdRead);
			timeval timeout = { 0,0 };
			int ret = select(_sock + 1, &fdRead, NULL, NULL, &timeout);
			if (ret < 0)
			{
				printf("select�����Ѿ��˳����������\n");
				return false;
			}
			if (FD_ISSET(_sock, &fdRead))
			{
				FD_CLR(_sock, &fdRead);
				if (-1 == RecvData(_sock))
				{
					printf("����������Ѿ��˳����������\n");
					return false;
				}
			}
			return true;
		}
		return false;
	}
	//socket��Ч
	bool isRun()
	{ 
		return _sock != INVALID_SOCKET;
	}

	//��������
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
