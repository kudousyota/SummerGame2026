#include "Timer.h"
#include <algorithm>

//シングルトンのTimerを取得
Timer& Timer::Instance()
{
	static Timer timer;
	return timer;
}
void Timer::Update()
{
	if (m_hitStopFrame > 0)
	{
		m_hitStopFrame--;
	}

	//シェイク
	if (m_shakeFrame > 0)
	{
		m_shakeFrame--;
	}
	//ゲームの全体の時間
	if (m_frameCount > 0)
	{
		m_frameCount--;

		if (m_frameCount <= 0)
		{
			m_timeScale = 1.0f;
		}
	}

	if (m_frameCountEnemy > 0)
	{
		m_frameCountEnemy--;

		if (m_frameCountEnemy <= 0)
		{
			m_enemyTimeScale = 1.0f;
		}
	}

}
void Timer::SetTimeScaleForFrames(float timescale, int frames)
{
	//時間のスケールを設定する
	m_timeScale = timescale;
	//フレームカウントを設定する
	m_frameCount = frames;
}
void Timer::SetEnemyTimeScaleForFrames(float scale, int frames)
{
	m_enemyTimeScale = scale;//敵の時間のスケールを設定する
	m_frameCountEnemy = frames;//敵のフレームカウントを設定する
}

bool Timer::IsEnemySlow() const
{
	return m_frameCountEnemy > 0;
}

void Timer::RequestHitStop(int frame)
{
	//既に停止されてたら上書きしない
	m_hitStopFrame = (std::max)(m_hitStopFrame, frame);
}

void Timer::RequestShake(float power, int frame)
{
	//より強くて長いリクエストのみ上書きする(弱い連続ヒットで先発のシェイクが消えないように)
	if (power >= m_shakePower)
	{
		m_shakePower = power;
		m_shakeFrame = frame;
		m_shakeFrameMax = frame;
	}
}

Vector3 Timer::GetShakeOffset() const
{
	if (m_shakeFrame <= 0)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	//残りフレームの割合で強さを減衰させる
	float ratio = static_cast<float>(m_shakeFrame) / static_cast<float>(m_shakeFrameMax);
	float power = m_shakePower * ratio;

	float x = (GetRand(1000) / 1000.0f - 0.5f) * 2.0f * power;
	float y = (GetRand(1000) / 1000.0f - 0.5f) * 2.0f * power;

	return Vector3(x,y,0.0f);
}
