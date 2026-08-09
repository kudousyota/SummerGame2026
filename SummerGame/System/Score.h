#pragma once
class Score
{
public:
	static Score& Instance();

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
	Score() = default;
	virtual~Score() = default;
	//コピーコンストラクタと代入演算子を削除して、シングルトンのインスタンスが複製されないようにする
	Score(const Score&) = delete;
	Score& operator=(const Score&) = delete;

	int m_enemyScore = 0;
	int m_timeScore = 0;
	int m_witchTimeScore = 0;
	int m_noDamageScore = 0;
	int m_totalScore = 0;
	int m_witchTimeCount = 0;
	float m_clearTime = 0.0f;
	bool m_isNoDamage = true;
};
