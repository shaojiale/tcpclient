#ifndef _TCPCLIENT_HPP
#define _TCPCLIENT_HPP
#include <stdio.h>
#include <WinSock2.h>
#include <thread>
#include "MessageHeader.hpp"
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll
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
	//��ʼ��socket
	int InitSocket()
	{
		//����WinSocket2.x����\
		//���汾��(���ֽ�)Ϊ2�����汾��Ϊ2������ 0x0202
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0)
			return 0;
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
			printf("����Socket�ɹ���\n");
		}
	}
	//���ӷ�����
	int Connect(char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}
		//���������������,���������׽����������IP��ַ���˿� 3000 �󶨣�
		sockaddr_in sockAddr = {};
		sockAddr.sin_family = AF_INET;

		//32λIP��ַ,s_addr ��һ������(unsigned long)����IP��ַ��һ���ַ�����
		//������Ҫ inet_addr() ��������ת��
		sockAddr.sin_addr.s_addr = inet_addr(ip);

		//16λ�Ķ˿ں�,�˿ں���Ҫ�� htons() ����ת��,0~1023 �Ķ˿�һ����ϵͳ������ض��ķ������
		//������ 1024~65536 ֮�����˿ںš�
		sockAddr.sin_port = htons(port);

		//ǿ������ת��,��������Ҫ�� bind() �������׽������ض���IP��ַ�Ͷ˿ڰ�������
		//ֻ��������������IP��ַ�Ͷ˿ڵ����ݲ��ܽ����׽��ִ���
		if (SOCKET_ERROR == connect(_sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)))
		{
			printf("����ʧ��\n");
		}
		else
		{
			printf("�ɹ����ӵ���������\n");
		}
		return 0;
	}
	//�ر�socket
	void Close()
	{
		if (INVALID_SOCKET != _sock)
		{
			closesocket(_sock);
			WSACleanup();
		}
		_sock = INVALID_SOCKET;
	}
	//�������� ����ճ�����
	int RecvData(SOCKET _sock)
	{
		//���ܻ���
		char szRecv[4096] = {};
		int cnt = (int)recv(_sock, szRecv, sizeof(Dataheader), 0);
		Dataheader* header = (Dataheader*)szRecv;
		if (cnt <= 0)
		{
			printf("�������Ѿ��˳����������\n");
			return -1;//��������ѭ��
		}
		recv(_sock, szRecv + sizeof(header), header->dataLength - sizeof(header), 0);
		OnNetMsg(header);
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
			LoginResult* ret = (LoginResult*)header;
			printf("�յ��������Ϣ��CMD_LOGIN_RESULT��CMD_LOGIN ���ݳ��ȣ�%d\n", ret->dataLength);
			break;
		}
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult* ret = (LogoutResult*)header;
			printf("�յ��������Ϣ��CMD_LOGOUT_RESULT��CMD_LOGIN ���ݳ��ȣ�%d\n", ret->dataLength);
			break;
		}
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* ret = (NewUserJoin*)header;
			printf("�յ��������Ϣ��CMD_NEW_USER_JOIN��CMD_LOGIN ���ݳ��ȣ�%d\n", ret->dataLength);
			break;
		}
		default:
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
			timeval timeout = { 1,0 };
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
