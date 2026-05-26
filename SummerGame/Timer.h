#pragma once
class Timer
{
	static Timer& Instance();
	Timer();
	~Timer();
	float GetTimeScale() { return m_timeScale; }
private:
	float m_timeScale;


	
};

