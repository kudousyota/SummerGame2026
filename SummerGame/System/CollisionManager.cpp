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
	// 距離比較用に半径の二乗を計算
	float radiusSquared = radius * radius;
	// 登録済みの全キャラクターを走査
	for (auto* character : m_pCharacters)
	{
		// 自分自身には当たらない
		if (character == attacker) continue;
		// 攻撃中心から対象までのベクトル
		Vector3 toTarget = character->GetPosition() - pos;
		// 攻撃範囲内ならダメージを与える
		if (toTarget.SqMagnitude() <= radiusSquared)
		{
			character->ApplyDamage(damage);
		}
	}
}
