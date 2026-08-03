#pragma once
#include "CharacterType.h"
//’l‚ğ•\‚·‚¾‚¯‚È‚Ì‚Åˆ—‚Í‚È‚¢
enum class AttackType
{
	Punch,
	Rush,
	Kick,
	SkyKick,
	Attack,
	Breath,
	WitchHand,
};

class AttackData
{
public:
	//’N‚©//UŒ‚ƒ^ƒCƒv//UŒ‚—Í//”¼Œa
	AttackData(CharacterType attacker, AttackType type, int damage,float radius);

	//UŒ‚‚µ‚½‚Ì‚Í’N‚È‚Ì‚©
	void SetAttacker(CharacterType attacker) { m_attacker = attacker; }
	CharacterType GetAttacker()const { return m_attacker; }

	//UŒ‚‚Ìƒ^ƒCƒv
	void SetAttackType(AttackType type) { m_attackType = type; }
	AttackType GetAttackType()const { return m_attackType; }

	//ƒ_ƒ[ƒWŒn
	void SetDamage(int damage) { m_damage = damage; }
	int GetDamage()const { return m_damage; }
	//UŒ‚‚Ì”¼Œa
	float GetRadius() const { return m_radius; }

private:
	CharacterType m_attacker = CharacterType::Player;
	AttackType m_attackType = AttackType::Punch;
	int m_damage = 0;
	float m_radius = 0;
	
};

