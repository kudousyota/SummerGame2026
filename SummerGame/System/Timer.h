#pragma once
class Timer
{
public:
	static Timer& Instance();
	
	void SetTimeScale(float scale) { m_timeScale = scale; }
	float GetTimeScale() const { return m_timeScale; }

	//敵専用のタイムスケール
	float GetEnemyTimeScale()const { return m_enemyTimeScale; }

	void Update();

	void SetTimeScaleForFrames(float timescale, int frames);
	
	void SetEnemyTimeScaleForFrames(float scale, int frames);

	bool IsEnemySlow() const;

private:
	//コンストラクタとデストラクタをプライベートにして、シングルトンパターンを実装
	Timer() = default;
	virtual ~Timer() = default;
	//コピーコンストラクタと代入演算子を削除して、シングルトンのインスタンスが複製されないようにする
	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;
	//時間の管理
	float m_timeScale = 1.0f;
	//時間を元に戻すためのやつ
	int m_frameCount = 0;
	
	float m_enemyTimeScale = 1.0f;
	int m_frameCountEnemy = 0;

	
	
};

