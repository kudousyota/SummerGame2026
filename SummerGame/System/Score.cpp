#include "Score.h"
#include <algorithm>

namespace
{
	//スコアリセット
	constexpr int kScoreReset = 0;
}

Score& Score::Instance()
{
	static Score score;
	return score;
}

void Score::Init()
{
	//スコアを初期化
	m_enemyScore = kScoreReset;
	m_timeScore = kScoreReset;
	m_witchTimeScore = kScoreReset;
	m_noDamageScore = kScoreReset;
	m_totalScore = kScoreReset;

	m_witchTimeCount = kScoreReset;

	m_isNoDamage = true;

	m_clearTime = kScoreReset;
}

void Score::AddEnemyScore(int score)
{
	m_enemyScore += score;
}

void Score::SetClearTime(float time)
{
	m_clearTime = time;
}

void Score::AddWitchTimeCount()
{
	m_witchTimeCount++;
}

void Score::OnNoDamage()
{
	m_isNoDamage = false;
}

void Score::Calculate()
{
	//3分以内ならボーナス加算
	m_timeScore = std::max(0, 18000 - static_cast<int>(m_clearTime * 100.0f));

	//ウィッチタイム発動回数に応じてボーナス加算
	m_witchTimeScore = m_witchTimeCount * 1000;

	//ノーダメージならボーナス加算
	if (m_isNoDamage)
	{
		m_noDamageScore = 5000;
	}
	else
	{
		m_noDamageScore = 0;
	}
	//合計スコアを計算
	m_totalScore = m_enemyScore + m_timeScore + m_witchTimeScore + m_noDamageScore;
}
