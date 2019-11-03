#include "stdlib.h"
#include <stdio.h>
#include <ctime>
#include <WinSock2.h>
#include <iostream>
#include <thread>
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
//��Ϣͷ
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
	//���ܻ���
	char szRecv[4096] = {};
	int cnt = recv(clientSocket, szRecv, sizeof(Dataheader), 0);
	Dataheader* header = (Dataheader*)szRecv;
	if (cnt <= 0)
	{
		//if ((cnt < 0) && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		//{
		//	continue;//������������
		//}
		printf("�ͷ������Ѿ��˳����������\n", (int)clientSocket);
		return -1;//��������ѭ��
	}
	//������������
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(clientSocket, szRecv + sizeof(header), header->dataLength - sizeof(header), 0);
		LoginResult *ret = (LoginResult*)szRecv;
		printf("�յ��������Ϣ��CMD_LOGIN_RESULT��CMD_LOGIN ���ݳ��ȣ�%d\n", ret->dataLength);
		break;
	}
	case CMD_LOGOUT_RESULT:
	{
		recv(clientSocket, szRecv + sizeof(header), header->dataLength - sizeof(header), 0);
		LoginResult* ret = (LoginResult*)szRecv;
		printf("�յ��������Ϣ��CMD_LOGOUT_RESULT��CMD_LOGIN ���ݳ��ȣ�%d\n", ret->dataLength);
		break;
	}
	case CMD_NEW_USER_JOIN:
	{
		recv(clientSocket, szRecv + sizeof(header), header->dataLength - sizeof(header), 0);
		NewUserJoin* ret = (NewUserJoin*)szRecv;
		printf("�յ��������Ϣ��CMD_NEW_USER_JOIN��CMD_LOGIN ���ݳ��ȣ�%d\n", ret->dataLength);
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
			printf("�ͻ������˳���\n");			
			break;
		}
		else if(0 == strcmp(input, "login"))
		{
			Login login;
			strcpy(login.userName, "xiaoming");
			strcpy(login.passWord, "123456");
			//�������������Ϣ
			send(clientSocket, (const char*)&login, sizeof(login), 0);
		}
		else if (0 == strcmp(input, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "xiaoming");
			//�������������Ϣ
			send(clientSocket, (const char*)&logout, sizeof(logout), 0);
		}
		else
		{
			printf("�������������\n");
		}
	}
	
}
int main()
{
	//��ʼ��DLL
	//���汾��(���ֽ�)Ϊ2�����汾��Ϊ2������ 0x0202
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	//WSAStartup() ����ִ�гɹ��󣬻Ὣ�� ws2_32.dll �йص���Ϣд�� WSAData �ṹ�����
	//(ֻ���עǰ������Ա����)
	if (WSAStartup(sockVersion, &wsaData) != 0)
		return 0;

	//�����׽��֣�socket() �������������׽��֣�ȷ���׽��ֵĸ�������
	//IPv4 ��ַ���������ӵ����ݴ��䷽ʽ��TCP ����Э��
	SOCKET clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//���������������,���������׽����������IP��ַ���˿� 3000 �󶨣�
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;

	//32λIP��ַ,s_addr ��һ������(unsigned long)����IP��ַ��һ���ַ�����
	//������Ҫ inet_addr() ��������ת��
	sockAddr.sin_addr.s_addr = inet_addr("192.168.0.106");

	//16λ�Ķ˿ں�,�˿ں���Ҫ�� htons() ����ת��,0~1023 �Ķ˿�һ����ϵͳ������ض��ķ������
	//������ 1024~65536 ֮�����˿ںš�
	sockAddr.sin_port = htons(3000);

	//ǿ������ת��,��������Ҫ�� bind() �������׽������ض���IP��ַ�Ͷ˿ڰ�������
	//ֻ��������������IP��ַ�Ͷ˿ڵ����ݲ��ܽ����׽��ִ���
	while (connect(clientSocket, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		printf("����ʧ��\n");
		Sleep(1000);
	}
	printf("�ɹ����ӵ���������\n");
	//����cmd�����߳�
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
			printf("select�����Ѿ��˳����������\n");
			break;
		}
		if (FD_ISSET(clientSocket, &fdRead))
		{
			FD_CLR(clientSocket, &fdRead);
			if (-1 == Processor(clientSocket))
			{
				printf("����������Ѿ��˳����������\n");
				break;
			}
		}
		//timelast = timethis;
		//timethis = clock();
		//if ((double)(timethis - timelast) / CLOCKS_PER_SEC >= 1)
		//{
		//	/*printf("time: %f Do something else!!!!!!!!��\n", (double)(timethis - timelast) / CLOCKS_PER_SEC);*/
		//}
		
	}
	closesocket(clientSocket);
	WSACleanup();
	
	return 1;
}