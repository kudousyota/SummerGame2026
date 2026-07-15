#pragma once
#include "CharacterType.h"
//値を表すだけなので処理はない
enum class AttackType
{
	Punch,
	Rush,
	Kick,
	SkyKick,
	Attack,
	Breath
};

class AttackData
{
public:
	AttackData(CharacterType attacker, AttackType type, int damage);

	//攻撃したのは誰なのか
	void SetAttacker(CharacterType attacker) { m_attacker = attacker; }
	CharacterType GetAttacker()const { return m_attacker; }

	//攻撃のタイプ
	void SetAttackType(AttackType type) { m_attackType = type; }
	AttackType GetAttackType()const { return m_attackType; }

	//ダメージ系
	void SetDamage(int damage) { m_damage = damage; }
	int GetDamage()const { return m_damage; }

private:
	CharacterType m_attacker = CharacterType::Player;
	AttackType m_attackType = AttackType::Punch;
	int m_damage = 0;
	
};

