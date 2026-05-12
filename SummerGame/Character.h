#pragma once

//プレイヤーとエネミーの基底クラス
class Character
{
public:
	Character();
	~Character();

	void Init();
	void Update();
	void Draw();
protected:
	//共通のデータや関数を入れる
	float m_speed;
	int m_hp;
	int m_attackPower;
};

