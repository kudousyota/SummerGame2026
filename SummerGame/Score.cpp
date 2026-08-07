#include "Score.h"
#include <algorithm>

void Score::AddEnemyScore(int score)
{
}

void Score::SetClearTime(float time)
{
}

void Score::Calculate()
{
	//3分以内ならボーナス加算
	m_timeScore = std::max(0, 6000 - static_cast<int>(m_clearTime * 100));

	m_totalScore = m_enemyScore + m_timeScore;
}
