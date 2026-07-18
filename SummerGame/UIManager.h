#pragma once
class UIManager
{
public:
	UIManager();
	~UIManager();
	void Init();
	void Update();
	void Draw();
	//プレイヤーのHPから値を渡してもらう
	void SetPleyerHpGauge(int hp,int maxhp);

private:
	int m_uiHandle;

	int m_hpGaugeHandle;

	int m_hpX;
	int m_hpY;

	int m_hpGaugeBackHandle;
};

