#include "stdlib.h"
#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll

enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};
//��Ϣͷ
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
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

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
	while (connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		printf("����ʧ��\n");
		Sleep(1000);
	}
	printf("�ɹ����ӵ���������\n");
	while (true)
	{
		char input[100] = {};
		scanf("%s", input);
		if (0 == strcmp(input, "exit"))
		{
			printf("�ͻ������˳���\n");
			break;
		}
		else if(0 == strcmp(input, "login"))
		{
			Login login = { "xiaoming" ,"123456"};
			Dataheader hd = { CMD_LOGIN,sizeof(login) };
			//�������������Ϣ
			send(sock, (const char*)&hd, sizeof(hd), 0);
			send(sock, (const char*)&login, sizeof(login), 0);
			//���շ�����������Ϣ
			Dataheader rthd = { };
			LoginResult loginret = { };
			recv(sock, (char*)&rthd, sizeof(rthd), 0);
			recv(sock, (char*)&loginret, sizeof(loginret), 0);
			printf("LoginResult��%d\n", loginret.result);
		}
		else if (0 == strcmp(input, "logout"))
		{
			Logout logout = {"xiaoming"};
			Dataheader hd = { CMD_LOGOUT,sizeof(logout) };
			//�������������Ϣ
			send(sock, (const char*)&hd, sizeof(hd), 0);
			send(sock, (const char*)&logout, sizeof(logout), 0);
			//���շ�����������Ϣ
			Dataheader rthd = { };
			LogoutResult logoutret = { };
			recv(sock, (char*)&rthd, sizeof(rthd), 0);
			recv(sock, (char*)&logoutret, sizeof(logoutret), 0);
			printf("LoginResult��%d\n", logoutret.result);
		}
		else
		{
			printf("�������������\n");
		}
	}
	closesocket(sock);
	WSACleanup();
	
	return 1;
}