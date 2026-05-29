#include "Character.h"

Character::Character():
	m_speed(0.0f),
	m_hp(0),
	m_attackPower(0),
	m_gravity(0.0f),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_angle(0)
{
}

Character::~Character()
{
}

void Character::Init()
{
	m_speed = 4.0f;
	m_hp = 100;
	m_attackPower = 10;
	m_gravity = 0.5f;
	m_pos = VGet(0.0f, 0.0f, 0.0f);
	m_angle = 0.0f;

	
}

void Character::Collision()
{
	//当たり判定

}

void Character::ApplyDamage(int damage)
{
	m_hp -= damage;

	if (m_hp < 0)
	{
		m_hp = 0;
	}
}

float Character::GetCollisionRadius() const
{
	return 30.0f;
}

float Character::GetCollisionHeight() const
{
	return 100.0f;
}

void Character::SetStage(std::shared_ptr<Stage> stage)
{
	m_pStage = stage;
}
