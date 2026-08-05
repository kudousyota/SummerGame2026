#include "Breath.h"
#include "../System/CollisionManager.h"
#include "../Effect/EffectManager.h"

namespace
{
	constexpr float kBreathRange = 190.0f;
}

Breath::Breath(const Vector3& pos, const Vector3& forward, float speed, const AttackData& attack):
	//親に渡す
	Projectile(pos,forward,speed,attack),
	m_effectHandle(-1)
{
}

void Breath::Update()
{
	//前のフレームを保存
	Vector3 endpos = m_pos + m_forward * kBreathRange;
	//カプセルの判定
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
	//エフェクト再生
	EffectManager::Instns().PlayEffect(EffectType::Breath, m_pos + m_forward * kBreathRange * 0.5f);

	//DrawCapsule3D(m_pos, endpos, m_attackData.GetRadius(), 4, 0xffffff, 0xffffff, false);
}

void Breath::Kill()
{
	m_isDead = true;
}
