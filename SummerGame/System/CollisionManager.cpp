#include "CollisionManager.h"
#include "../Game/Character.h"
#include <algorithm>
#include <vector>

//シングルトンのCollisionManagerを取得
CollisionManager& CollisionManager::Instance()
{
	static CollisionManager collisionManager;
	return collisionManager;
}
//当たり判定対象のキャラクターを登録
void CollisionManager::Register(Character* character)
{
	m_pCharacters.push_back(character);
}
//登録済みのキャラクターを削除
void CollisionManager::Unregister(Character* character)
{
	//登録済みキャラクターの中から対象を探す
	auto iterator = std::find(m_pCharacters.begin(), m_pCharacters.end(), character);
	//見つかったら登録リストから削除
	if (iterator != m_pCharacters.end())
	{
		m_pCharacters.erase(iterator);
	}
}
//球状の攻撃判定を行う
void CollisionManager::CheckAttackSphere(Character* attacker, const Vector3& pos, float radius, int damage)
{
	//登録済みの全キャラクターを探す
	for (auto& character : m_pCharacters)
	{
		//自分自身には当たらない
		if (character == attacker)
		{
			continue;
		}
		//同じタイプには当たらない
		if (character->GetCharacterType() == attacker->GetCharacterType())
		{
			continue;
		}

		// キャラクターのカプセル情報を取得
		float capsuleRadius = character->GetCollisionRadius();
		float capsuleHeight = character->GetCollisionHeight();

		//カプセルの線分(a -> b)
		Vector3 a = character->GetCollisionPosition();
		Vector3 b = VGet(a.x, a.y + capsuleHeight, a.z);

		Vector3 ab = b - a;
		Vector3 ap = pos - a;

		float abLen2 = ab.SqMagnitude();
		float t = 0.0f;
		if (abLen2 > 0.000001f)
		{
			//線分上の最近接点パラメータt
			t = ap.Dot(ab) / abLen2;
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;
		}

		Vector3 closest = a + ab * t;
		Vector3 diff = closest - pos;
		float dist2 = diff.SqMagnitude();
		//ジャスト回避
		//受付中なら100、普段は30
		float justDodgeRadius = character->GetJustDodgeRadius(); 
		float combinedJust = radius + justDodgeRadius;

		if (dist2 <= combinedJust * combinedJust)
		{
			//もし相手がジャスト回避受付中なら、ジャスト回避成功
			if (character->IsJustDodgeWindow())
			{
				//ダメージ0で呼び出すして中でジャスト回避処理が走る
				character->ApplyDamage(0);
				//通常ダメージは与えない
				continue; 
			}
		}

		//ジャスト回避が不発、または受付時間外だった場合、通常の判定でチェック
		float normalRadius = character->GetCollisionRadius(); //30.0f
		float combinedNormal = radius + normalRadius;

		if (dist2 <= combinedNormal * combinedNormal)
		{
			//通常の被弾
			character->ApplyDamage(damage);
		}
	}
}

bool CollisionManager::CheckStageWall(Character* character, int stagehandle)
{
	//プレイヤーのカプセル情報を取得
	float radius = character->GetCollisionRadius();
	float height = character->GetCollisionHeight();


	Vector3 pos = character->GetPosition();

	bool hitAny = false;

	//当たっていたら
	const int kMaxIteration = 4;
	for (int iter = 0; iter < kMaxIteration; iter++)
	{
		VECTOR start = VGet(pos.x, pos.y + radius, pos.z);
		VECTOR end = VAdd(start, VGet(0.0f, height - radius * 2.0f, 0.0f));

		auto hit = MV1CollCheck_Capsule(stagehandle, -1, start, end, radius);
		if (hit.HitNum == 0)
		{
			MV1CollResultPolyDimTerminate(hit);
			break;
		}

		hitAny = true;

		for (int i = 0; i < hit.HitNum; i++)
		{
			auto& normal = hit.Dim[i].Normal;
			
			Vector3 push(normal.x, 0.0f, normal.z);

			if (push.SqMagnitude() > 0.0001f)
			{
				push = push.Normalize();
				
				pos += push * 15.0f;
			}
		}

		MV1CollResultPolyDimTerminate(hit);
	}
	character->SetPosition(pos);

	return hitAny;
}

bool CollisionManager::CheckStageGround(Character* character, int stagehandle, float& outGroundY)
{
	Vector3 pos = character->GetPosition();
	//キャラクターのカプセルの半径を取得
	float radius = character->GetCollisionRadius();
	
	
	const float kUpMargin = 5.0f;
	const float kDownMargin = 10.0f;

	//レイ(下向きの線分)の開始点と終了点を決める
	//キャラクターの足元の少し上から、少し下まで
	VECTOR rayStart = VGet(pos.x, pos.y + kUpMargin, pos.z);
	VECTOR rayEnd = VGet(pos.x, pos.y - kDownMargin, pos.z);

	//DxLibの線分とモデルの当たり判定
	MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(stagehandle, -1, rayStart, rayEnd);

	if (hitPoly.HitFlag == 1)
	{
		outGroundY = hitPoly.HitPosition.y;
		return true;
	}
	return false;


}

bool CollisionManager::CheckCameraRay(const int stagehandle ,const Vector3& start, const Vector3& end, Vector3& hitpos)
{
	MV1_COLL_RESULT_POLY result = MV1CollCheck_Line(stagehandle,-1,start,end);
	if (result.HitFlag)
	{
		hitpos.x = result.HitPosition.x;
		hitpos.y = result.HitPosition.y;
		hitpos.z = result.HitPosition.z;
		return true;
	}
	return false;
}


