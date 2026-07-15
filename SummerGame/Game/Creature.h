#pragma once
#include "Enemy.h"

class Creature : public Enemy
{
public:
	Creature();
	~Creature()override;
	void Init()override;
	void Update() override;
	void Draw()override;

	void OnHit(const AttackData& attackdata);
	void OnDead();

private:
	enum class CreatureState
	{
		Idle,
		Walk,
		Attack,
		Punch,
		Damage,
	};
	//現在の状態
	CreatureState m_currentState;
	//前回の状態
	CreatureState m_prevState;

	//攻撃処理
	void AttackUpdate();
	//当たり判定の位置を取得する関数
	Vector3 GetCollisionPosition() const override;
	//描画に使うモデルの垂直オフセット
	float m_modelDisplayOffsetY;
	//攻撃する場所
	Vector3 m_attackPos;



	void TransitionTo(CreatureState nextState);

	//ダメージを受けたらDamageステートへ
	void OnDamaged() override;
};
