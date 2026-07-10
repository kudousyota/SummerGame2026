#pragma once
#include "Vector3.h"
//敵が使う投擲物や球などの基底クラス
class Projectile
{
public:
	//引数は変更しないからconst参照で受け取る
	Projectile(const Vector3& pos,const Vector3& forward,float speed,int attackPower);
	virtual~Projectile() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool IsDead()const { return m_isDead; }
protected:

	Vector3 m_pos;
	Vector3 m_forward;

	float m_speed = 0.0f;
	int m_attackPower = 0;
	bool m_isDead = false;

	
};

