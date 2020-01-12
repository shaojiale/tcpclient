#include "tcpclient.hpp"
#include "CELLTimestamp.hpp"
#include <atomic>
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
			printf("�ͻ������˳���\n");			
			break;
		}
		else
		{
			printf("�������������\n");
		}
	}
}

//�ͻ�������
const int cCount = 12;
//�߳�����
const int tCount = 6;
//�ͻ�������
TcpClient* clients[cCount];
std::atomic_int sendCount = 0;
std::atomic_int readyCount = 0;

void sendThread(int id)
{
	printf("thread: %d start\n",id);
	int begin = (cCount / tCount) * (id - 1);
	int end = (cCount / tCount) * id;

	for (int n = begin; n < end; n++)
	{
		clients[n] = new TcpClient();
	}
	for (int n = begin; n < end; n++)
	{
		clients[n]->Connect((char*)"192.168.0.106", 1245);
	}
	printf("thread: %d connect = %d\n",id, end);

	readyCount++;
	while (readyCount < tCount)
	{
		std::chrono::milliseconds t(3000);
		std::this_thread::sleep_for(t);
	}

	Login login[1];
	for (int n = 0; n < 1; n++)
	{
		strcpy(login[n].userName, "xiaoming");
		strcpy(login[n].passWord, "xiaoming");
	}
	const int nLen = sizeof(login);
	CELLTimestamp timer;
	while (g_bRun)
	{
		for (int n = begin; n < end; n++)
		{
			if (timer.getElapsedSec() >= 1.0)
			{
				if (SOCKET_ERROR != clients[n]->SendData(login, nLen))
				{
					sendCount++;
				}
				timer.update();
			}
			clients[n]->OnRun();
		}
	}
	for (int n = begin; n < end; n++)
	{
		clients[n]->Close();
		delete clients[n];
	}
	printf("thread: %d end\n", id);
}
int main()
{
	//����UI�߳�
	std::thread t1(cmdthread);
	t1.detach();

	//���������߳�
	for (int n = 0; n < tCount; n++)
	{
		std::thread t1(sendThread,n+1);
		t1.detach();
	}

	//����ʱ�������
	CELLTimestamp timer;

	while (g_bRun)
	{
		auto t1 = timer.getElapsedSec();
		if (t1 >= 1.0)
		{
			printf("Thread��%d Clients��%d Time: %lf send: %d \n", \
				tCount, cCount, t1, (int)(sendCount / t1));
			sendCount = 0;
			timer.update();
			Sleep(1);
		}
	}

	printf("���˳���\n");
	return 0;
}