#pragma once
#include "../System/Vector3.h"

//プレイヤーとエネミーの基底クラス
class Input;
class Character
{
public:
	Character();
	virtual ~Character();

	virtual void Init();
	virtual void Update(const Input& input) = 0;
	virtual void Draw() = 0;
protected:
	//共通のデータや関数を入れる
	float m_speed;
	int m_hp;
	int m_attackPower;
	float m_gravity;
	int m_angle;
	Vector3 m_pos;
};

