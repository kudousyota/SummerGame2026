#include "CollisionManager.h"
#include "../Game/Character.h"
#include <algorithm>
#include <vector>

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
	// 登録済みキャラクターの中から対象を探す
	auto iterator = std::find(m_pCharacters.begin(), m_pCharacters.end(), character);
	// 見つかったら登録リストから削除
	if (iterator != m_pCharacters.end())
	{
		m_pCharacters.erase(iterator);
	}
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
			// 線分上の最近接点パラメータ t
			t = ap.Dot(ab) / abLen2;
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;
		}

		Vector3 closest = a + ab * t;
		Vector3 diff = closest - pos;
		float dist2 = diff.SqMagnitude();

		// 球の中心とカプセル中心線の最短距離(攻撃半径 + カプセル半径)
		float combined = radius + capsuleRadius;
		// 最短距離が両者の半径の和以下ならヒット
		if (dist2 <= combined * combined)
		{
			// ダメージを与える
			character->ApplyDamage(damage);
		}
	}
}

bool CollisionManager::CheckStageCollision(Character* character, int stageHandle)
{
	//プレイヤーのカプセル情報を取得
	float radius = character->GetCollisionRadius();
	float height = character->GetCollisionHeight();


	Vector3 pos = character->GetPosition();
	//カプセルの足元
	VECTOR start = VGet(pos.x, pos.y + radius, pos.z);
	//カプセルの頭のほう
	VECTOR end = VAdd(start, VGet(pos.x,pos.y + height - radius,pos.z));
	
	// ステージモデルとのカプセル衝突判定
	auto hit = MV1CollCheck_Capsule(
		stageHandle, // ステージモデル
		-1,                 // 全フレーム対象
		start,
		end,
		radius
	);

	// 当たっていたら
	if (hit.HitNum > 0)
	{
		Vector3 pos = character->GetPosition();
		for(int i = 0;i < hit.HitNum;i++)
		{
			// 衝突したポリゴンの情報を取得
			auto& normal = hit.Dim[i].Normal;

			//法線方向に押し出す
			pos += Vector3(normal.x, normal.y, normal.z) * 3.0f;
		}

		character->SetPosition(pos);
		
		//MV1CollCheck_Lineが確保した衝突ポリゴン情報を解放
		MV1CollResultPolyDimTerminate(hit);

		return true;
	}

	MV1CollResultPolyDimTerminate(hit);
	return false;
}

bool CollisionManager::CheckStageGround(Character* character, int stageHandle, float& outGroundY)
{
	Vector3 pos = character->GetPosition();

	float radius = character->GetCollisionRadius();

	// レイ(下向きの線分)の開始点と終了点を決める
	// キャラクターの足元の少し上から、少し下まで
	VECTOR rayStart = VGet(pos.x, pos.y + 5.0f, pos.z);
	VECTOR rayEnd = VGet(pos.x, pos.y - radius - 5.0f, pos.z);

	// DxLibの線分とモデルの当たり判定
	MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(stageHandle, -1, rayStart, rayEnd);

	if (hitPoly.HitFlag == 1)
	{
		if (outGroundY)
		{

			outGroundY = hitPoly.HitPosition.y;
		}
		return true; // 接地
	}

	return false; // 空中


}
