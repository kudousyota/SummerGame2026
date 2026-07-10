#include "Breath.h"
#include "System/CollisionManager.h"

Breath::Breath(const Vector3& pos, const Vector3& forward, float speed, int attackPower):
	//親に渡す
	Projectile(pos,forward,speed,attackPower)
{
}

void Breath::Update()
{
	//前のフレームを保存
	m_prevPos = m_pos;
	//前方移動
	m_pos += m_forward * m_speed;

	CollisionManager::Instance().CheckAttackCapsule(CharacterType::Enemy, m_prevPos, m_pos, 25.0f, m_attackPower);

	//寿命
	m_lifeFrame++;

	if (m_lifeFrame > 150)
	{
		m_isDead = true;
	}
}

void Breath::Draw()
{
	//ここでエフェクト出してもいい
	//デバッグ表示とかしたい
}
