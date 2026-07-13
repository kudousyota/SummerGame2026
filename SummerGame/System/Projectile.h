#pragma once
#include "Vector3.h"
//敵が使う投擲物や球などの基底クラス
class Projectile
{
public:
	//引数は変更しないからconst参照で受け取る
	Projectile(const Vector3& pos,const Vector3& forward,float speed,int attackPower);
	virtual~Projectile() = default;

	void SetPos(const Vector3& pos) { m_pos = pos; }
	void SetForward(const Vector3& forwrd) { m_forward = forwrd; }


	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool IsDead()const { return m_isDead; }
protected:
	//位置
	Vector3 m_pos;
	//前の位置
	Vector3 m_prevPos;
	//前方
	Vector3 m_forward;
	//スピード
	float m_speed;
	//攻撃力
	int m_attackPower;
	//生存フレーム
	int m_lifeFrame;
	//まだあるか
	bool m_isDead;

	
};

