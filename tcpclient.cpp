#include "tcpclient.hpp"

#include <thread>

bool g_bRun = true;
void cmdthread()
{
	char input[100] = {};
	while (true)
	{
		scanf("%s", input);
		if (0 == strcmp(input, "exit"))
		{
			g_bRun = false;
			printf("客户端已退出！\n");			
			break;
		}
		else
		{
			printf("请重新输入命令！\n");
		}
	}
}

//客户端数量
const int cCount = 4000;
//线程数量
const int tCount = 4;
//客户端数量
TcpClient* clients[cCount];
void sendThread(int id)
{
	Login login;
	strcpy(login.userName, "xiaoming");
	strcpy(login.passWord, "xiaoming");
	int begin = (cCount / tCount) * (id - 1);
	int end = (cCount / tCount) * id;
	int a = sizeof(TcpClient);
	for (int n = begin; n < end; n++)
	{
		if (!g_bRun)
		{
			return;
		}
		clients[n] = new TcpClient();
		clients[n]->InitSocket();
	}
	for (int n = begin; n < end; n++)
	{
		if (!g_bRun)
		{
			return;
		}
		clients[n]->Connect((char*)"192.168.0.106", 1245);
		printf("connect = %d\n", n);
	}
	while (true)
	{
		for (int n = begin; n < end; n++)
		{
			if (!g_bRun || -1 == clients[n]->SendData(&login))
			{
				return;
			}
			clients[n]->OnRun();
		}
	}
	for (int n = begin; n < end; n++)
	{
		clients[n]->Close();
	}
}
int main()
{
	std::thread t1(cmdthread);
	t1.detach();

	//启动发送线程
	for (int n = 0; n < tCount; n++)
	{
		std::thread t1(sendThread,n+1);
		t1.detach();
	}
	while (g_bRun)
		Sleep(1000);

	return 0;
}