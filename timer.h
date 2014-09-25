#ifndef _TIMER_H_
#define _TIMER_H_

#include <windows.h>

class Timer{
public:
	Timer();
	~Timer();

	bool Initialize();
	void Frame();

	float GetLastFrameTime();

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
};

#endif