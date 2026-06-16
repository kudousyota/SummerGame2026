#pragma once
#include "../System/Vector3.h"
#include "Character.h"
#include <memory> 

class Player;
class Input;
class Enemy: public Character
{
public:
	Enemy();
	~Enemy()override;
	void Init()override;
	void Update() override;
	void Draw()override;

	//ダメージ
	void ApplyDamage(int damage)override;

	float GetCollisionRadius() const override { return 70.0f; }
	float GetCollisionHeight() const override { return 100.0f; }

	bool IsDead()const { return m_isDead; }

	void SetPlayer(std::shared_ptr<Player> player);

private:

	enum class EnemyState
	{
		Idle,
		Walk,
		Attack,
		Rush,
		Avoid,
		Damage,
		Jump,
		Dodge
	};

	//現在の状態
	EnemyState m_currentState;

	//前回の状態
	EnemyState m_prevState;

	//攻撃処理
	void AttackUpdate();

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

	void TransitionTo(EnemyState nextState);

	std::shared_ptr<Player> m_pPlayer;
};

