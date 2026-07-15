#include "AttackData.h"

AttackData::AttackData(CharacterType attacker, AttackType type, int damage) :
	m_attacker(attacker),
	m_attackType(type),
	m_damage(damage)
{
}
