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
			printf("客户端已退出！\n");			
			break;
		}
		else if(0 == strcmp(input, "login"))
		{
			Login login;
			strcpy(login.userName, "xiaoming");
			strcpy(login.passWord, "123456");
			//向服务器发送消息
			clientSocket->SendData(&login);
		}
		else if (0 == strcmp(input, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "xiaoming");
			//向服务器发送消息
			clientSocket->SendData(&logout);
		}
		else
		{
			printf("请重新输入命令！\n");
		}
	}
}
int main()
{
	TcpClient clientSocket;
	clientSocket.InitSocket();
	clientSocket.Connect((char*)"192.168.0.106",3000);
	//启动cmd输入线程
	std::thread t1(cmdthread,&clientSocket);
	t1.detach();
	while (clientSocket.isRun())
	{
		clientSocket.OnRun();
	}
	return 0;
}