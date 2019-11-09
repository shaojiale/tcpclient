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
int main()
{
	//启动cmd输入线程
	std::thread t1(cmdthread);
	t1.detach();

	Login login;
	strcpy(login.userName, "xiaoming");
	strcpy(login.passWord, "xiaoming");

	const int cCount = 100;
	TcpClient* clients[cCount];
	int a = sizeof(TcpClient);
	for (int n = 0; n < cCount; n++)
	{
		if (!g_bRun)
		{
			return 0;
		}
		clients[n] = new TcpClient();
		clients[n]->InitSocket();
	}
	for (int n = 0; n < cCount; n++)
	{
		if (!g_bRun)
		{
			return 0;
		}
		clients[n]->Connect((char*)"192.168.0.106", 1245);
	}
	while (true)
	{
		for (int n = 0; n < cCount; n++)
		{
			if (!g_bRun || -1 == clients[n]->SendData(&login))
			{
				return 0;
			}
			/*clients[n]->OnRun();*/
		}
	}
	for (int n = 0; n < cCount; n++)
	{
		clients[n]->Close();
	}
	return 0;
}