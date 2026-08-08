#pragma once
class Score
{
public:
	Score();
	~Score();

	void Init();
	//敵を倒したときのスコアを加算する
	void AddEnemyScore(int score);
	void SetClearTime(float time);

	//ウィッチタイム発動回数を加算する
	void AddWitchTimeCount();
	//ノーダメージフラグを設定する
	void OnNoDamage();

	//計算してリザルトシーンで呼ぶ
	void Calculate();

	//Getter
	int GetEnemyScore()const { return m_enemyScore; }
	int GetTimeScore()const { return m_timeScore; }
	int GetWitchTimeScore()const { return m_witchTimeScore; }
	int GetNoDamageScore()const { return m_noDamageScore; }
	int GetTotalScore()const { return m_totalScore; }
	float GetClearTime()const { return m_clearTime; }

private:

	int m_enemyScore;
	int m_timeScore;
	int m_witchTimeScore;
	int m_noDamageScore;
	int m_totalScore;
	int m_witchTimeCount;
	float m_clearTime;
	bool m_isNoDamage;
};
