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

private:

	enum class AngelState
	{
		Idle,
		Run,
		DancingAttack,
		Rotate,
		Shout,
		Damage,
	};

	//現在の状態
	AngelState m_currentState;
	//前回の状態
	AngelState m_prevState;

	bool m_dancingAttackHit[8];

	void TransitionTo(AngelState nextState);

	void OnDamaged();

	Vector3 GetHitEffect()const override;
};

