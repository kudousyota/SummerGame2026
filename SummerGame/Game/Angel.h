#pragma once
#include "../System/Vector3.h"
#include "Character.h"
#include <memory> 

class Player;
class Angel :public Character
{
public:
	Angel();
	~Angel()override;
	void Init()override;
	void Update() override;
	void Draw()override;

	void ApplyDamage(int damage) override;
	bool IsDead()const { return m_isDead; }

	float GetCollisionRadius() const override { return 70.0f; }
	float GetCollisionHeight() const override { return 100.0f; }

	void SetPlayer(std::shared_ptr<Player> player) { m_pPlayer = player; };

	CharacterType GetCharacterType() const override;

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

	//ハンドル
	int m_modelHandle;
	//しんだか
	bool m_isDead;

	//アタック
	bool m_isAttackHit;
	//攻撃中か
	bool m_isAttacking;

	//攻撃のクールタイム
	int m_attackCooldown;
	//攻撃するフレーム数
	int m_attackFrame;

	//攻撃する場所
	Vector3 m_attackPos;
	//向き
	Vector3 m_forward;
	//攻撃の向き
	Vector3 m_attackDir;
	//攻撃しているかどうかのフラグ
	bool m_isAttack;

	bool m_dancingAttackHit[8];

	void TransitionTo(AngelState nextState);

	//索敵処理
	bool CanSeePlayer();

	std::shared_ptr<Player> m_pPlayer;
};

