#include "Enemy.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Player.h"
#include "../System/Timer.h"
#include "../Effect/EffectManager.h"
#include "../System/SoundManager.h"

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
	m_score(0)
{
}

Enemy::~Enemy()
{
	CollisionManager::Instance().Unregister(this);
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

	m_sightRange = 500.0f;
	m_closeRange = 150.0f;
	m_fov = 90.0f;

	CollisionManager::Instance().Register(this);
}

void Enemy::ApplyDamage(int damage)
{
	if (m_isDead)
	{
		return;
	}

	m_hp -= damage;

	EffectManager::Instns().PlayEffect(EffectType::Hit, GetHitEffect());
	SoundManager::Instance().PlaySE("Hit");

	if (m_hp <= 0)
	{
		m_hp = 0;
		OnDead();
		CollisionManager::Instance().Unregister(this);
		return;
	}
	OnDamaged();
}

CharacterType Enemy::GetCharacterType() const
{
	return CharacterType::Enemy;
}

AttackData Enemy::CreateAttackData() const
{
	return AttackData(CharacterType::Enemy,GetAttackType(),m_attackPower,GetAttackRadius());
}

void Enemy::UpdateModelMatrix()
{
	MATRIX scr = MGetScale(m_scale);
	MATRIX rot = MGetRotY(m_angle);
	Vector3 drawPos = m_pos;
	MATRIX trans = MGetTranslate(drawPos.ToDxLibVector());
	MV1SetMatrix(m_modelHandle, MMult(MMult(scr, rot),trans));
}

bool Enemy::CanSeePlayer()
{
	//プレイヤーまでのベクトル
	Vector3 dir = m_pPlayer->GetPosition() - m_pos;
	dir.y = 0.0f;
	float distSq = dir.SqMagnitude();

	// 近すぎたら向きに関係なく気づく
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
	float halfFov = m_fov * 0.5f * DX_PI_F / 180.0f;
	//前方の角度
	float baseAngle = atan2f(m_forward.x, m_forward.z);
	//分割数
	int segments = 16;
	//線形の始点
	VECTOR prevPoint = Vector3(
		m_pos.x + m_sightRange * sinf(baseAngle - halfFov),
		m_pos.y + 160.0f,
		m_pos.z + m_sightRange * cosf(baseAngle - halfFov)
	);
	//扇形の円弧を線分で描画
	for (int i = 1; i <= segments; i++)
	{
		float angle = baseAngle - halfFov + (halfFov * 2.0f) * (float)i / segments;
		VECTOR point = Vector3(
			m_pos.x + m_sightRange * sinf(angle),
			m_pos.y + 160.0f,
			m_pos.z + m_sightRange * cosf(angle)
		);
		DrawLine3D(prevPoint, point, color);
		prevPoint = point;
	}

	//扇の両辺(敵から視野端への線)
	VECTOR center = Vector3(m_pos.x, m_pos.y + 160.0f, m_pos.z);
	VECTOR leftEdge = Vector3(
		m_pos.x + m_sightRange * sinf(baseAngle - halfFov),
		m_pos.y + 160.0f,
		m_pos.z + m_sightRange * cosf(baseAngle - halfFov)
	);
	VECTOR rightEdge = Vector3(
		m_pos.x + m_sightRange * sinf(baseAngle + halfFov),
		m_pos.y + 160.0f,
		m_pos.z + m_sightRange * cosf(baseAngle + halfFov)
	);
	//扇形の境界線を描画
	DrawLine3D(center, leftEdge, color);
	DrawLine3D(center, rightEdge, color);

	//近接円を描画
	VECTOR prevClosePoint = Vector3(
		m_pos.x + m_closeRange * sinf(0.0f),
		m_pos.y + 160.0f,
		m_pos.z + m_closeRange * cosf(0.0f)
	);
	for (int i = 1; i <= segments; i++)
	{
		float angle = (DX_TWO_PI_F) * (float)i / segments;
		VECTOR point = Vector3(
			m_pos.x + m_closeRange * sinf(angle),
			m_pos.y + 160.0f,
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
	
	Vector3 dir = m_pPlayer->GetPosition() - m_pos;
	dir.y = 0.0f;

	if (dir.SqMagnitude() < 0.00001f)
	{
		return;
	}

	m_forward = (m_pPlayer->GetPosition() - m_pos).Normalize();
	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;
}

void Enemy::ChasePlayer(float rotatespeed, float scale)
{
	MoveTo(m_pPlayer->GetPosition(), rotatespeed, scale);
}

void Enemy::UpdateCooldown(float scale)
{
	if (m_attackCooldown > 0)
	{
		m_attackCooldown -= static_cast<int>(scale);
	}
}

void Enemy::DrawDebugCollision() const
{
	Vector3 debugPos = GetCollisionPosition();

	VECTOR start = Vector3(debugPos.x,debugPos.y + GetCollisionRadius(),debugPos.z);

	VECTOR end = Vector3(debugPos.x,debugPos.y + GetCollisionHeight() - GetCollisionRadius(),debugPos.z);

	DrawCapsule3D(start,end,GetCollisionRadius(),16,0xffffff,0xffffff,false);
}

void Enemy::MoveTo(const Vector3& target, float rotatespeed, float scale)
{
	Vector3 dir = target - m_pos;
	dir.y = 0.0f;

	if (dir.SqMagnitude() < 0.001f)
	{
		return;
	}

	Vector3 targetDir = dir.Normalize();

	m_forward += (targetDir - m_forward) * rotatespeed;
	m_forward = m_forward.Normalize();

	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;

	m_pos += m_forward * m_speed * scale;

}
