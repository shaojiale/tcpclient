#include "tcpclient.hpp"


void cmdthread(TcpClient* clientSocket)
{
	char input[100] = {};
	while (true)
	{
		scanf("%s", input);
		if (0 == strcmp(input, "exit"))
		{
			clientSocket->Close();
			printf("�ͻ������˳���\n");			
			break;
		}
		else if(0 == strcmp(input, "login"))
		{
			Login login;
			strcpy(login.userName, "xiaoming");
			strcpy(login.passWord, "123456");
			//�������������Ϣ
			clientSocket->SendData(&login);
		}
		else if (0 == strcmp(input, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "xiaoming");
			//�������������Ϣ
			clientSocket->SendData(&logout);
		}
		else
		{
			printf("�������������\n");
		}
	}
}
int main()
{
	TcpClient clientSocket;
	clientSocket.InitSocket();
	clientSocket.Connect((char*)"192.168.0.106",3000);
	//����cmd�����߳�
	std::thread t1(cmdthread,&clientSocket);
	t1.detach();
	while (clientSocket.isRun())
	{
		clientSocket.OnRun();
	}
	return 0;
}