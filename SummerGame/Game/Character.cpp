#include "Character.h"
#include "Stage.h"
#include "../System/CollisionManager.h"

Character::Character():
	m_speed(0.0f),
	m_hp(0),
	m_attackPower(0),
	m_gravity(0.0f),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_angle(0),
	m_modelHandle(-1),
	m_velocity(VGet(0.0f,0.0f,0.0f)),
	m_isGround(false)
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
	if (!m_isGround)
	{
		m_velocity.y -= m_gravity;
	}
	else
	{
		//地面にいる間
		m_velocity.y = 0.0f;
	}
	//座標に落下分の移動量を足す
	m_pos.y += m_velocity.y;

	//地面の判定
	float groundY = 0.0f;
	int stageHandle = m_pStage->GetModelHandle();


	// CollisionManagerに地面判定用の関数を呼び出す
	if (m_velocity.y <= 0.0f &&
		CollisionManager::Instance().CheckStageGround(this, stageHandle, &groundY))
	{
		m_pos.y = groundY;
		m_isGround = true;
		m_velocity.y = 0.0f;
	}
	else
	{
		m_isGround = false;
	}

	// 座標を押し出すように
	CollisionManager::Instance().CheckStageCollision(this, stageHandle);

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
