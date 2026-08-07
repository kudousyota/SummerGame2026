#pragma once
class Score
{
public:
	void Init();
	//敵を倒したときのスコアを加算する
	void AddEnemyScore(int score);
	void SetClearTime(float time);

	//計算してリザルトシーンで呼ぶ
	void Calculate();

	//Getter
	int GetEnemyScore()const { return m_enemyScore; }
	int GetTimeScore()const { return m_enemyScore; }
	int GetTotalScore()const { return m_enemyScore; }
	float GetClearTime()const { return m_enemyScore; }

private:

	int m_enemyScore;
	int m_timeScore;
	int m_totalScore;

	float m_clearTime;
};

