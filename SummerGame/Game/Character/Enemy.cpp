#include "Enemy.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Player.h"
#include "../System/Timer.h"
#include "../Effect/EffectManager.h"
#include "../System/SoundManager.h"

namespace
{
	//==================================================
	// 初期設定
	//==================================================

	// 初期状態で向いている方向
	constexpr float kDefaultForwardX = 0.0f;
	constexpr float kDefaultForwardY = 0.0f;
	constexpr float kDefaultForwardZ = 1.0f;

	// 初期の索敵範囲
	constexpr float kDefaultSightRange = 500.0f;

	// 敵に近づいた場合、向きに関係なく発見する距離
	constexpr float kDefaultCloseRange = 180.0f;

	// 攻撃を開始する距離
	constexpr float kDefaultAttackRange = 300.0f;

	// 視野角
	constexpr float kDefaultFov = 90.0f;


	//==================================================
	// 索敵
	//==================================================

	// 視野角を計算するための角度変換
	constexpr float kHalfAngle = 0.5f;

	// 視野デバッグの分割数
	constexpr int kSightSegments = 16;

	// 索敵デバッグを表示する高さ
	constexpr float kSightDebugOffsetY = 160.0f;


	//==================================================
	// 移動
	//==================================================

	// 移動方向がほぼゼロの場合の判定値
	constexpr float kMoveThresholdSq = 0.001f;

	// プレイヤーの方向がほぼゼロの場合の判定値
	constexpr float kFaceThresholdSq = 0.00001f;


	//==================================================
	// 攻撃
	//==================================================

	// クールタイムの初期値
	constexpr int kInitialAttackCooldown = 0;


	//==================================================
	// デバッグ
	//==================================================

	// カプセル判定の分割数
	constexpr int kCollisionSegments = 16;
}

Enemy::Enemy():
	m_modelHandle(-1),
	m_isDead(false),
	m_attackCooldown(0),
	m_forward(Vector3(0.0f, 0.0f, 1.0f)),
	m_isAttacking(false),
	m_attackFrame(0),
	m_isAttack(false),
	m_attackDir(Vector3(0.0f, 0.0f, 0.0f)),
	m_scale(0.0f,0.0f,0.0f),
	m_timeScale(0.0f),
	m_sightRange(0.0f),
	m_closeRange(0.0f),
	m_fov(0.0f),
	m_attackRange(0.0f),
	m_score(0),
	m_hazardPos(Vector3(0.0f,0.0f,0.0f)),
	m_attackWarnigFrame(0.0f)
{
}

Enemy::~Enemy()
{
	//当たり判定管理から自信を登録解除
	CollisionManager::Instance().Unregister(this);
	//モデルが有効な場合は解放
    if (m_modelHandle != -1)
    {
        MV1DeleteModel(m_modelHandle);
    }
}

void Enemy::Init()
{
	Character::Init();

	//最初は正面を向く
	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;

	//索敵範囲を設定
	m_sightRange = kDefaultSightRange;

	//近距離では敵の向きに関係なくプレイヤーを発見する
	m_closeRange = kDefaultCloseRange;

	//攻撃を開始する距離を設定
	m_attackRange = kDefaultAttackRange;

	//視野角を設定
	m_fov = kDefaultFov;

	//当たり判定管理に自身を登録
	CollisionManager::Instance().Register(this);
}

void Enemy::ApplyDamage(int damage)
{
	//すでに死亡している場合はダメージを受けない
	if (m_isDead)
	{
		return;
	}
	//HPを減らす
	m_hp -= damage;
	//エフェクトと効果音再生
	EffectManager::Instns().PlayEffect(EffectType::Hit, GetHitEffect());
	SoundManager::Instance().PlaySE("Hit");
	//HPが０以下になった場合は死亡処理
	if (m_hp <= 0)
	{
		m_hp = 0;
		OnDead();
		//死亡したら敵は当たり判定の管理から外す
		CollisionManager::Instance().Unregister(this);
		return;
	}
	//生存している場合はダメージ状態に行く
	OnDamaged();
}

CharacterType Enemy::GetCharacterType() const
{
	return CharacterType::Enemy;
}

AttackData Enemy::CreateAttackData() const
{
	//自身の攻撃情報を作成
	return AttackData(CharacterType::Enemy,GetAttackType(),m_attackPower,GetAttackRadius());
}

void Enemy::UpdateModelMatrix()
{
	//サイズの行列を作成
	MATRIX scr = MGetScale(m_scale);
	//Y軸回転行列を作成
	MATRIX rot = MGetRotY(m_angle);
	//モデルの描画位置を設定
	Vector3 drawPos = m_pos;
	//移動行列を作成
	MATRIX trans = MGetTranslate(drawPos.ToDxLibVector());
	//スケール、回転、移動を合成してモデルに適用
	MV1SetMatrix(m_modelHandle, MMult(MMult(scr, rot),trans));
}

bool Enemy::CanSeePlayer()
{
	//プレイヤーまでのベクトル
	Vector3 dir = m_pPlayer->GetPosition() - m_pos;
	//水平移動方向のみで索敵する
	dir.y = 0.0f;
	//プレイヤーまでの距離の2乗
	float distSq = dir.SqMagnitude();

	//近すぎたら向きに関係なく気づく
	if (distSq <= m_closeRange * m_closeRange)
	{
		return true;
	}

	//視認距離の外ならfalse
	if (distSq > m_sightRange * m_sightRange)
	{
		return false;
	}

	//視野角内かを内積で判定
	float dot = m_forward.Dot(dir.Normalize());
	//視野角の半分をコサインに変換
	float halfFovCos = cosf(m_fov * 0.5f * DX_PI_F / 180.0f);

	return dot >= halfFovCos;
}

void Enemy::DrawDebugSight() const
{
	//索敵範囲を黄色で描画
	int color = GetColor(255, 255, 0);
	//近接判定範囲を緑で描画
	int closeColor = GetColor(0, 255, 0);

	//視野角の半分(ラジアン)
	float halfFov = m_fov * kHalfAngle * DX_PI_F / 180.0f;
	//前方の角度
	float baseAngle = atan2f(m_forward.x, m_forward.z);
	//分割数
	int segments = kSightSegments;
	//線形の始点
	VECTOR prevPoint = Vector3(
		m_pos.x + m_sightRange * sinf(baseAngle - halfFov),
		m_pos.y + kSightDebugOffsetY,
		m_pos.z + m_sightRange * cosf(baseAngle - halfFov)
	);
	//扇形の円弧を線分で描画
	for (int i = 1; i <= segments; i++)
	{
		float angle = baseAngle - halfFov + (halfFov * 2.0f) * (float)i / segments;
		VECTOR point = Vector3(
			m_pos.x + m_sightRange * sinf(angle),
			m_pos.y + kSightDebugOffsetY,
			m_pos.z + m_sightRange * cosf(angle)
		);
		DrawLine3D(prevPoint, point, color);
		prevPoint = point;
	}

	//扇の両辺(敵から視野端への線)
	VECTOR center = Vector3(m_pos.x, m_pos.y + kSightDebugOffsetY, m_pos.z);
	VECTOR leftEdge = Vector3(
		m_pos.x + m_sightRange * sinf(baseAngle - halfFov),
		m_pos.y + kSightDebugOffsetY,
		m_pos.z + m_sightRange * cosf(baseAngle - halfFov)
	);
	VECTOR rightEdge = Vector3(
		m_pos.x + m_sightRange * sinf(baseAngle + halfFov),
		m_pos.y + kSightDebugOffsetY,
		m_pos.z + m_sightRange * cosf(baseAngle + halfFov)
	);
	//扇形の境界線を描画
	DrawLine3D(center, leftEdge, color);
	DrawLine3D(center, rightEdge, color);

	//近接円を描画
	VECTOR prevClosePoint = Vector3(
		m_pos.x + m_closeRange * sinf(0.0f),
		m_pos.y + kSightDebugOffsetY,
		m_pos.z + m_closeRange * cosf(0.0f)
	);
	for (int i = 1; i <= segments; i++)
	{
		float angle = (DX_TWO_PI_F) * (float)i / segments;
		VECTOR point = Vector3(
			m_pos.x + m_closeRange * sinf(angle),
			m_pos.y + kSightDebugOffsetY,
			m_pos.z + m_closeRange * cosf(angle)
		);
		DrawLine3D(prevClosePoint, point, closeColor);
		prevClosePoint = point;
	}

	//プレイヤーが視野内か近接円内にいるとき色を変える
	Vector3 dir = (m_pPlayer->GetPosition() - m_pos);
	float dist = dir.SqMagnitude();

	bool isClose = dist <= m_closeRange * m_closeRange;
	bool isInFov = false;

	if (dist <= m_sightRange * m_sightRange)
	{
		float dot = m_forward.Dot(dir.Normalize());
		float halfFovCos = cosf(m_fov * 0.5f * DX_PI_F / 180.0f);
		isInFov = (dot >= halfFovCos);
	}

	if (isClose || isInFov)
	{
		// 発見中は赤で上書き
		DrawLine3D(center, Vector3(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y + 160.0f, m_pPlayer->GetPosition().z), GetColor(255, 0, 0));
	}
}

void Enemy::FacePlayer()
{
	//プレイヤーまでの方向を取得
	Vector3 dir = m_pPlayer->GetPosition() - m_pos;
	//水平方向のみ計算
	dir.y = 0.0f;
	//プレイヤーとの距離がほぼ0なら向きを変更しない
	if (dir.SqMagnitude() < kFaceThresholdSq)
	{
		return;
	}
	//プレイヤー方向を正規化して正面方向として設定
	m_forward = (m_pPlayer->GetPosition() - m_pos).Normalize();
	//正面方向からY軸の回転角を計算
	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;
}

void Enemy::ChasePlayer(float rotatespeed, float scale)
{
	//プレイヤーの位置を目標にして移動
	MoveTo(m_pPlayer->GetPosition(), rotatespeed, scale);
}

void Enemy::UpdateCooldown(float scale)
{
	//クールタイム中のみ時間を減らす
	if (m_attackCooldown > 0)
	{
		m_attackCooldown -= static_cast<int>(scale);
	}
}

void Enemy::DrawDebugCollision() const
{
	//当たり判定の中心位置を取得
	Vector3 debugPos = GetCollisionPosition();
	//カプセルの上端・下端を計算
	VECTOR start = Vector3(debugPos.x,debugPos.y + GetCollisionRadius(),debugPos.z);

	VECTOR end = Vector3(debugPos.x,debugPos.y + GetCollisionHeight() - GetCollisionRadius(),debugPos.z);
	//カプセル型の当たり判定をデバッグ表示
	DrawCapsule3D(start,end,GetCollisionRadius(),16,0xffffff,0xffffff,false);
}

void Enemy::MoveTo(const Vector3& target, float rotatespeed, float scale)
{
	//目標地点までの方向を計算
	Vector3 dir = target - m_pos;
	//水平方向のみで移動方向を計算
	dir.y = 0.0f;
	//目標地点に十分近い場合は移動しない
	if (dir.SqMagnitude() < kMoveThresholdSq)
	{
		return;
	}
	//目標方向を正規化
	Vector3 targetDir = dir.Normalize();
	//現在の向きを目標方向へ徐々に回転
	m_forward += (targetDir - m_forward) * rotatespeed;
	//回転後の方向を正規化
	m_forward = m_forward.Normalize();
	//正面方向からY軸の回転角を計算
	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;
	//敵を前方へ移動
	m_pos += m_forward * m_speed * scale;

}
