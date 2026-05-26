#include "Timer.h"

//シングルトンのTimerを取得
Timer& Timer::Instance()
{
	static Timer timer;
	return timer;
}
void Timer::SetTimeScaleForFrames(float timescale, int frames)
{
	//時間のスケールを設定する
	m_timeScale = timescale;
	//フレームカウントを設定する
	m_frameCount = frames;
}
//void Timer::SetEnemyTimeScaleForFrames(float scale, int frames)
//{
//	enemyTimeScale = scale;//敵の時間のスケールを設定する
//	m_frameCountEnemy = frames;//敵のフレームカウントを設定する
//}