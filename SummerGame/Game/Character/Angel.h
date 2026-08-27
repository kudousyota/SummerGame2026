#pragma once
#include "Enemy.h"

class Player;
class Angel :public Enemy
{
public:
	Angel();
	~Angel()override;
	void Init()override;
	void Update() override;
	void Draw()override;
	//被弾処理
	void OnHit(const AttackData& attackdata);

	AttackType GetAttackType() const override;
	float GetAttackRadius() const override;
	int GetScore() const override { return m_score; }

private:

	enum class AngelState
	{
		Idle,
		Run,
		AttackWarnig,
		DancingAttack,
		Rotate,
		Shout,
		Damage,
		Look,
		Dead
	};

	//現在の状態
	AngelState m_currentState;
	//前回の状態
	AngelState m_prevState;

	Vector3 m_lastSeePos;


	bool m_dancingAttackHit[8];

	void TransitionTo(AngelState nextState);

	void OnDamaged();
	void OnDead()override;

	Vector3 GetHitEffect()const override;
};

