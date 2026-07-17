#include "Breath.h"
#include "../System/CollisionManager.h"

namespace
{
	constexpr float kBreathRange = 190.0f;
}

Breath::Breath(const Vector3& pos, const Vector3& forward, float speed, const AttackData& attack):
	//親に渡す
	Projectile(pos,forward,speed,attack)
{
}

void Breath::Update()
{
	//前のフレームを保存
	Vector3 endpos = m_pos + m_forward * kBreathRange;

	CollisionManager::Instance().CheckAttackCapsule(m_attackData, m_pos, endpos);

	//寿命
	m_lifeFrame++;
}

void Breath::Draw()
{
	//ここでエフェクト出してもいい
	//デバッグ表示とかしたい

	//前のフレームを保存
	Vector3 endpos = m_pos + m_forward * kBreathRange;

	DrawCapsule3D(m_pos, endpos, m_attackData.GetRadius(), 4, 0xffffff, 0xffffff, false);
}

void Breath::Kill()
{
	m_isDead = true;
}
