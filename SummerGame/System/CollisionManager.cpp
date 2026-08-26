#include "CollisionManager.h"
#include "../Game/Character/Character.h"
#include <algorithm>
#include <vector>
#include "../DataLoader/AttackData.h"
#include "../System/Timer.h"

namespace
{
	constexpr float kWalkableSlopeAngleDeg = 60.0f;
	//歩ける坂道の角度//ラジアンに変換
	const float kWalkableSlopeCos = cosf(kWalkableSlopeAngleDeg * (DX_PI / 180.0f));

	//ヒットストップフレーム
	constexpr int kHitStopFrame = 8;
	//カメラシェイクの強さ
	constexpr float kShakePower = 15.0f;
	//カメラシェイクのフレーム数
	constexpr int kShakeFrame = 12;
}

// 指定キャラクターと登録済みキャラクターすべてとの衝突を解決する
void CollisionManager::ResolveCollisionsFor(Character* character)
{
    if (character == nullptr) return;

    // 他のキャラクターとのみチェックする
    for (auto other : m_pCharacters)
    {
        if (other == nullptr || other == character) continue;

        // 重複処理を避けるためにポインタの順序で一方のみ処理する
        if (other < character) continue;

        // 衝突していたら解決
        if (CheckCharacterCapsule(character, other))
        {
            ResolveCharacterCollision(character, other);
        }
    }
}

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
std::vector<Character*> CollisionManager::CheckAttackSphere(const AttackData& attackdata, const Vector3& pos)
{
//#ifdef _DEBUG
//DrawSphere3D(pos.ToDxLibVector(), attackdata.GetRadius(), 16, GetColor(0, 255, 0), GetColor(0, 255, 0), false);
//#endif //_DEBUG

	std::vector<Character*> hitCharacters;

	//登録済みの全キャラクターを探す
	for (auto& character : m_pCharacters)
	{
		//攻撃した側と同じタイプには当たらない
		if (character->GetCharacterType() == attackdata.GetAttacker())
		{
			continue;
		}

		// キャラクターのカプセル情報を取得
		float capsuleHeight = character->GetCollisionHeight();

		//カプセルの線分(a -> b)
		Vector3 a = character->GetCollisionPosition();
		Vector3 b = Vector3(a.x, a.y + capsuleHeight, a.z);

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
		float combinedJust = attackdata.GetRadius() + justDodgeRadius;

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
		float combinedNormal = attackdata.GetRadius() + normalRadius;

		if (dist2 <= combinedNormal * combinedNormal)
		{
			//通常の被弾
			character->OnHit(attackdata);
			hitCharacters.push_back(character);

			//ヒットストップ・カメラシェイク
			Timer::Instance().RequestHitStop(kHitStopFrame);
			Timer::Instance().RequestShake(kShakePower, kShakeFrame);
		}
	}
	return hitCharacters;
}

std::vector<Character*> CollisionManager::CheckAttackCapsule(const AttackData& attackdata, const Vector3& start, const Vector3& end)
{

#ifdef _DEBUG
	DrawCapsule3D(start.ToDxLibVector(), end.ToDxLibVector(), attackdata.GetRadius(), 4, GetColor(0, 255, 0), GetColor(0, 255, 0), false);
#endif //_DEBUG
	std::vector<Character*> hitCharacters;

	//登録済みの全キャラクターを探す
	for (auto& character : m_pCharacters)
	{
		//攻撃した側と同じタイプには当たらない
		if (character->GetCharacterType() == attackdata.GetAttacker())
		{
			continue;
		}
		float capsuleRadius = character->GetCollisionRadius();
		float capsuleHeight = character->GetCollisionHeight();

		Vector3 a = character->GetCollisionPosition();

		Vector3 b = a + Vector3(0.0f, capsuleHeight, 0.0f);

		float distance = Segment_Segment_MinLength(start, end, a, b);

		//ジャスト回避の半径を知る
		float justDodgeRadius = character->GetJustDodgeRadius();

		float justRange = attackdata.GetRadius() + justDodgeRadius;


		if (distance <= justRange)
		{
			if (character->IsJustDodgeWindow())
			{
				//ジャスト回避成功
				character->ApplyDamage(0);
				continue;
			}
		}

		//通常の当たり判定
		float hitRange = attackdata.GetRadius() + capsuleRadius;

		if (distance <= hitRange)
		{
			character->OnHit(attackdata);

			//ヒットストップ・カメラシェイク
			Timer::Instance().RequestHitStop(kHitStopFrame);
			Timer::Instance().RequestShake(kShakePower, kShakeFrame);
		}
	}
	return hitCharacters;
}

bool CollisionManager::CheckStageWall(Character* character, int stagehandle)
{
	//プレイヤーのカプセル情報を取得
	float radius = character->GetCollisionRadius();
	float height = character->GetCollisionHeight();

	//足元基準の現在地
	Vector3 pos = character->GetPosition();
	//このフレームで一回でもポリゴンに当たったか確認
	bool hitAny = false;

	//当たっていたら
	//4回判定する
	const int kMaxIteration = 4;
	for (int iter = 0; iter < kMaxIteration; iter++)
	{
		VECTOR start = Vector3(pos.x, pos.y + radius, pos.z);
		VECTOR end = VAdd(start, Vector3(0.0f, height - radius * 2.0f, 0.0f));

		auto hit = MV1CollCheck_Capsule(stagehandle, -1, start, end, radius);
		//何もヒットしなければ,これ以上押し出す必要はない
		if (hit.HitNum == 0)
		{
			MV1CollResultPolyDimTerminate(hit);
			break;
		}
		//ポリゴンに当たったか記録
		//hitAny = true;
		//ここでは壁ポリゴンに当たったかを一旦保留にする

		//一番めり込んだポリゴンを探すための変数
		float maxDepth = -FLT_MAX;
		//一番深いポリゴンの法線を保持
		Vector3 bestNormal;
		//当たったポリゴンをチェック
		for (int i = 0; i < hit.HitNum; i++)
		{
			Vector3 fullNormal(hit.Dim[i].Normal.x, hit.Dim[i].Normal.y, hit.Dim[i].Normal.z);
			fullNormal = fullNormal.Normalize();

			//歩ける床と坂とみなして壁判定から除外
			//歩行可能な坂道は壁として扱わない
			if (fullNormal.y >= kWalkableSlopeCos)
			{
				//坂と床はスキップ
				continue;
			}
			//法線のY成分を無視してXZ平面に投射
			Vector3 rawNormal(hit.Dim[i].Normal.x, 0.0f, hit.Dim[i].Normal.z);

			//ほぼ水平なポリゴン床は壁判定から除外
			float lenSq = rawNormal.SqMagnitude();
			if (lenSq < 0.0001f)
			{
				continue;
			}
			//歩行可能な坂道は壁として扱わない
			Vector3 normal(hit.Dim[i].Normal.x,0.0f,hit.Dim[i].Normal.z);

			normal = normal.Normalize();
			//ポリゴンの頂点の一つを平面上の点として使う
			Vector3 planepoint(hit.Dim[i].Position[0].x,
				hit.Dim[i].Position[0].y,
				hit.Dim[i].Position[0].z);

			//カプセル軸
			Vector3 axispoint(pos.x, planepoint.y, pos.z);
			//平面点からカプセル軸への距離を法線方向に射影
			float signedDist = normal.Dot(axispoint - planepoint);

			//半径よりも平面に近ければめり込んでいる
			float depth = radius - signedDist;

			if (depth > maxDepth)
			{
				maxDepth = depth;
				bestNormal = normal;
			}

		}
		//実際に壁として押し出しが発生した場合のみ hitAny を立てる
		//実際にめり込んでいた場合のみ、その分だけ壁の法線方向に押し出す
		if (maxDepth > 0.0f)
		{
			pos += bestNormal * maxDepth;
			hitAny = true;
		}
		//ヒット結果のメモリを解放
		MV1CollResultPolyDimTerminate(hit);
	}
	//押し出し後の最終的な座標をキャラクターに反映
	character->SetPosition(pos);
	//壁に一度でも当たったかを返す
	return hitAny;
}

bool CollisionManager::CheckStageGround(Character* character, int stagehandle, float& outGroundY, Vector3& outGroundNormal)
{
	//キャラクターの現在の位置を取得
	Vector3 pos = character->GetPosition();
	//キャラクターのカプセルの半径を取得
	float radius = character->GetCollisionRadius();
	//レイを伸ばして坂道や段差でも設置判定をできるようにする
	const float kUpMargin = 5.0f;
	const float kDownMargin = 8.0f;

	//レイ(下向きの線分)の開始点と終了点を決める
	//キャラクターの基準位置は足元
	//カプセル半径を考慮した位置から下向きにレイを飛ばす
	VECTOR rayStart = Vector3(pos.x, pos.y + radius + kUpMargin, pos.z);
	VECTOR rayEnd = Vector3(pos.x, pos.y - radius - kDownMargin, pos.z);

	//ステージとのレイ判定をする
	MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(stagehandle, -1, rayStart, rayEnd);
	//レイが地面に当たったら
	if (hitPoly.HitFlag == 1)
	{
		//当たった場所を返す
		outGroundY = hitPoly.HitPosition.y;
		//地面の法線を正規化して返す
		outGroundNormal = Vector3(hitPoly.Normal.x, hitPoly.Normal.y, hitPoly.Normal.z).Normalize();
		return true;
	}
	//地面が見つからなかったら
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

void CollisionManager::ResolveCharacterCollision(Character* characterA, Character* characterB)
{
	//Aのカプセル軸
	Vector3 aStart = characterA->GetCollisionPosition();
	Vector3 aEnd = aStart + Vector3(0.0f, characterA->GetCollisionHeight(), 0.0f);
	float radiusA = characterA->GetCollisionRadius();

	//Bのカプセル軸
	Vector3 bStart = characterB->GetCollisionPosition();
	Vector3 bEnd = bStart + Vector3(0.0f, characterB->GetCollisionHeight(), 0.0f);
	float radiusB = characterB->GetCollisionRadius();

	//2本の軸の最近接点を求める
	Vector3 closestA, closestB;
	ClosestPointSegmentSegment(aStart, aEnd, bStart, bEnd, closestA, closestB);

	Vector3 direction = closestA - closestB;

	//Y方向は無視してXZ平面のみで押し出す
	direction.y = 0.0f;

	float distanceSq = direction.SqMagnitude();

	//ほぼ同じ位置なら押し出し方向は決めれない
	if (distanceSq < 0.000001f)
	{
		//真上/真下に重なっている場合の保険として、任意方向にずらす
		direction = Vector3(1.0f, 0.0f, 0.0f);
		distanceSq = 1.0f;
	}

	float distance = sqrtf(distanceSq);
	direction /= distance;

	float combinedRadius = radiusA + radiusB;
	float penetration = combinedRadius - distance;

	if (penetration <= 0.0f)
	{
		return;
	}

	float push = penetration * 0.5f;

	Vector3 posA = characterA->GetPosition();
	Vector3 posB = characterB->GetPosition();

	posA += direction * push;
	posB -= direction * push;

	characterA->SetPosition(posA);
	characterB->SetPosition(posB);
}
//2本の線分
void CollisionManager::ClosestPointSegmentSegment(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, Vector3& outC1, Vector3& outC2)
{
	Vector3 d1 = q1 - p1; //カプセルAの軸ベクトル
	Vector3 d2 = q2 - p2; //カプセルBの軸ベクトル
	Vector3 r = p1 - p2;

	float a = d1.Dot(d1); //線分1の長さの2乗
	float e = d2.Dot(d2); //線分2の長さの2乗
	float f = d2.Dot(r);

	float s, t;

	//両方が点(長さ0)の場合
	if (a <= 0.000001f && e <= 0.000001f)
	{
		outC1 = p1;
		outC2 = p2;
		return;
	}

	if (a <= 0.000001f)
	{
		//線分1が点の場合
		s = 0.0f;
		t = f / e;
		t = std::clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = d1.Dot(r);
		if (e <= 0.000001f)
		{
			//線分2が点の場合
			t = 0.0f;
			s = std::clamp(-c / a, 0.0f, 1.0f);
		}
		else
		{
			float b = d1.Dot(d2);
			float denom = a * e - b * b;

			if (denom != 0.0f)
			{
				s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}
			else
			{
				s = 0.0f; //平行な場合
			}

			t = (b * s + f) / e;

			if (t < 0.0f)
			{
				t = 0.0f;
				s = std::clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f)
			{
				t = 1.0f;
				s = std::clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	outC1 = p1 + d1 * s;
	outC2 = p2 + d2 * t;
}

bool CollisionManager::CheckCharacterCapsule(Character* characterA, Character* characterB)
{
	//AのカプセルAの軸を作る
	Vector3 aStart = characterA->GetCollisionPosition();
	Vector3 aEnd = aStart + Vector3(0.0f, characterA->GetCollisionHeight(), 0.0f);
	float radiusA = characterA->GetCollisionRadius();

	//Bのカプセルの軸を作る
	Vector3 bStart = characterB->GetCollisionPosition();
	Vector3 bEnd = bStart + Vector3(0.0f, characterB->GetCollisionHeight(), 0.0f);
	float radiusB = characterB->GetCollisionRadius();

	//ここで純粋な幾何計算関数に投げる
	return CheckCapsule(aStart, aEnd, radiusA, bStart, bEnd, radiusB);
}

bool CollisionManager::CheckCapsule(const Vector3& aStart, const Vector3& aEnd, float radiusA,const Vector3& bStart, const Vector3& bEnd, float radiusB)
{
	//2本の線分同士の最短距離を求める
	float distance = Segment_Segment_MinLength(aStart, aEnd, bStart, bEnd);

	//両カプセルの半径を合計
	float combinedRadiu = radiusA + radiusB;

	//距離が半径の合計以下なら当たっている
	bool isHit = distance <= combinedRadiu;

	return isHit;
}

