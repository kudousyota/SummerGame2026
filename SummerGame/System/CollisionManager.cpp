#include "CollisionManager.h"
#include "../Game/Character.h"
#include <algorithm>

// シングルトンのCollisionManagerを取得
CollisionManager& CollisionManager::Instance()
{
	static CollisionManager collisionManager;
	return collisionManager;
}
// 当たり判定対象のキャラクターを登録
void CollisionManager::Register(Character* character)
{
	m_pCharacters.push_back(character);
}
// 登録済みのキャラクターを削除
void CollisionManager::Unregister(Character* character)
{
	auto it = std::find(m_pCharacters.begin(), m_pCharacters.end(), character);
	if (it != m_pCharacters.end()) m_pCharacters.erase(it);
}
// 球状の攻撃判定を行う
void CollisionManager::CheckAttackSphere(Character* attacker, const Vector3& pos, float radius, int damage)
{
	// 登録済みの全キャラクターを走査
	for (auto* character : m_pCharacters)
	{
		// 自分自身には当たらない
		if (character == attacker) continue;

		// キャラクターのカプセル情報を取得
		float capsuleRadius = character->GetCollisionRadius();
		float capsuleHeight = character->GetCollisionHeight();

		// カプセルの線分 (a -> b)
		Vector3 a = character->GetPosition();
		Vector3 b = VGet(a.x, a.y + capsuleHeight, a.z);

		Vector3 ab = b - a;
		Vector3 ap = pos - a;

		float abLen2 = ab.SqMagnitude();
		float t = 0.0f;
		if (abLen2 > 0.000001f)
		{
			// 線分上の最近接点パラメータ t（0..1 にクランプ）
			t = ap.Dot(ab) / abLen2;
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;
		}

		Vector3 closest = a + ab * t;
		Vector3 diff = closest - pos;
		float dist2 = diff.SqMagnitude();

		// 衝突条件：球の中心とカプセル中心線の最短距離 <= (攻撃半径 + カプセル半径)
		float combined = radius + capsuleRadius;
		if (dist2 <= combined * combined)
		{
			character->ApplyDamage(damage);
		}
	}
}
