#ifndef _CELLTimestamp_hpp_
#define _CELLTimestamp_hpp_

//#include <windows.h>
#include<chrono>
using namespace std::chrono;

class CELLTimestamp
{
public:
	CELLTimestamp()
	{
		update();
	}
	void update()
	{
		_begin = high_resolution_clock::now();
	}
	//��ȡ��ǰ��
	double getElapsedSec()
	{
		return getElapsedTimeMicroSec() * 0.000001;
	}
	//��ȡ��ǰ����
	double getElapsedTimeMilliSec()
	{
		return getElapsedTimeMicroSec() * 0.001;
	}
	//��ȡ��ǰ΢��
	long long getElapsedTimeMicroSec()
	{
		return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
	}
protected:
	time_point<high_resolution_clock> _begin;
};

#endif // !_CELLTimestamp_hpp_