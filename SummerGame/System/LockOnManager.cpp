#include "LockOnManager.h"
#include "../Game/Character/Enemy.h"
namespace
{
	static constexpr int kLockOnDurationFrame = 120;
}
LockOnManager::LockOnManager():
	m_timerFrame(0)
{
}

void LockOnManager::OnAttackHit(Character* hitcharacter)
{
	m_pTarget = hitcharacter;
	//リセット
	m_timerFrame = kLockOnDurationFrame;
}
void LockOnManager::Update()
{
	//ターゲットする奴がいなかったらリターン
	if (!m_pTarget)
	{
		return;
	}
	//ターゲットしてる奴が死んだらぬるぽにする
	if (m_pTarget->IsDead())
	{
		m_pTarget = nullptr;
		return;
	}

	m_timerFrame--;
	if (m_timerFrame <= 0)
	{
		//攻撃しなかったら解除
		m_pTarget = nullptr;
	}
}

void LockOnManager::ClearIfTarget(Character* character)
{
	//消す
	if (m_pTarget == character)
	{
		m_pTarget = nullptr;
	}
}

bool LockOnManager::IsLockOn() const
{
	return m_pTarget != nullptr;
}

const Vector3* LockOnManager::GetLockOnPos()
{
	if (!m_pTarget)
	{
		return nullptr;
	}
    //ターゲットの当たり判定位置を基準に、少し上を狙う
    //Creatureなど大きめの敵の頭部付近をロックオンしたいので
    //当たり判定位置に高さのオフセットを加える
    m_cachadPos = m_pTarget->GetCollisionPosition();
    m_cachadPos.y += m_pTarget->GetCollisionHeight() * 0.7f;
	return &m_cachadPos;
}
