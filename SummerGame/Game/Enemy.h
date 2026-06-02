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

	//攻撃する場所
	Vector3 m_attackPos;
	//向き
	Vector3 m_forward;

	std::shared_ptr<Player> m_pPlayer;
};

