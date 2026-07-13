#include "Breath.h"
#include "../System/CollisionManager.h"

namespace
{
	constexpr float kBreathRange = 80.0f;
}

Breath::Breath(const Vector3& pos, const Vector3& forward, float speed, int attackPower):
	//親に渡す
	Projectile(pos,forward,speed,attackPower)
{
}

void Breath::Update()
{
	//前のフレームを保存
	Vector3 endpos = m_pos + m_forward * kBreathRange;

	CollisionManager::Instance().CheckAttackCapsule(CharacterType::Enemy, m_pos, endpos, 25.0f, m_attackPower);

	//寿命
	m_lifeFrame++;
}

void Breath::Draw()
{
	//ここでエフェクト出してもいい
	//デバッグ表示とかしたい

	//前のフレームを保存
	Vector3 endpos = m_pos + m_forward * kBreathRange;

	DrawCapsule3D(m_pos, endpos, 25.0f, 4, 0xffffff, 0xffffff, false);
}

void Breath::Kill()
{
	m_isDead = true;
}
