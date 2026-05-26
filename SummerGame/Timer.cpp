#include "Timer.h"

//シングルトンのTimerを取得
Timer& Timer::Instance()
{
	static Timer timer;
	return timer;
}
Timer::Timer()
{

}
Timer::~Timer()
{

}
