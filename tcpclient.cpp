#include "stdlib.h"
#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll

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

	//���������������,���������׽�����IP��ַ���˿� 3000 �󶨣�
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
		char recvBuf[100] = { 0 };
		std::cin >> input;

		if (0 == strcmp(input, "XXX"))
		{
		}
		else if (0 == strcmp(input, "exit"))
		{
			std::cout << "�ͻ������˳�" << std::endl;
			break;
		}
		else
		{
			send(sock, input, sizeof(input) + 1, 0);//����msgbuf�е����ݡ��������ȷ����Ӧ��ÿ�ζ����¼���
			recv(sock, recvBuf, 100, 0);
			std::cout << recvBuf << std::endl;
		}
	}
	

	closesocket(sock);
	WSACleanup();
	
	return 1;
}