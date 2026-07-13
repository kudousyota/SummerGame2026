#include "Enemy.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Player.h"

Enemy::Enemy():
	m_modelHandle(-1),
	m_isDead(false),
	m_attackCooldown(0),
	m_forward(VGet(0.0f, 0.0f, 1.0f)),
	m_isAttacking(false),
	m_attackFrame(0),
	m_isAttack(false),
	m_attackDir(VGet(0.0f, 0.0f, 0.0f)),
	m_scale(0.0f,0.0f,0.0f)
{
}

Enemy::~Enemy()
{
	CollisionManager::Instance().Unregister(this);

	MV1DeleteModel(m_modelHandle);
}

void Enemy::Init()
{
	Character::Init();

	//最初は正面を向く
	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;

	CollisionManager::Instance().Register(this);
}

void Enemy::ApplyDamage(int damage)
{
	if (m_isDead)
	{
		return;
	}

	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_isDead = true;
		CollisionManager::Instance().Unregister(this);
		return;
	}
	OnDamaged();
}

CharacterType Enemy::GetCharacterType() const
{
	return CharacterType::Enemy;
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
	float distSq = dir.SqMagnitude();

	//視認距離の外ならfalse
	if (distSq > kSightRange * kSightRange)
	{
		return false;
	}

	//視野角内かを内積で判定
	float dot = m_forward.Dot(dir.Normalize());
	float halfFovCos = cosf(kFov * 0.5f * DX_PI_F / 180.0f);

	return dot >= halfFovCos;
}

void Enemy::DrawDebugSight() const
{
	//索敵範囲デバッグ描画
	int color = GetColor(255, 255, 0);//黄色

	//視野角の半分(ラジアン)
	float halfFov = kFov * 0.5f * DX_PI_F / 180.0f;
	//前方の角度
	float baseAngle = atan2f(m_forward.x, m_forward.z);
	//分割数
	int segments = 16;
	//線形の始点
	VECTOR prevPoint = VGet(
		m_pos.x + kSightRange * sinf(baseAngle - halfFov),
		m_pos.y + 160.0f,
		m_pos.z + kSightRange * cosf(baseAngle - halfFov)
	);
	//扇形の円弧を線分で描画
	for (int i = 1; i <= segments; i++)
	{
		float angle = baseAngle - halfFov + (halfFov * 2.0f) * (float)i / segments;
		VECTOR point = VGet(
			m_pos.x + kSightRange * sinf(angle),
			m_pos.y + 160.0f,
			m_pos.z + kSightRange * cosf(angle)
		);
		DrawLine3D(prevPoint, point, color);
		prevPoint = point;
	}

	//扇の両辺(敵から視野端への線)
	VECTOR center = VGet(m_pos.x, m_pos.y + 160.0f, m_pos.z);
	VECTOR leftEdge = VGet(
		m_pos.x + kSightRange * sinf(baseAngle - halfFov),
		m_pos.y + 160.0f,
		m_pos.z + kSightRange * cosf(baseAngle - halfFov)
	);
	VECTOR rightEdge = VGet(
		m_pos.x + kSightRange * sinf(baseAngle + halfFov),
		m_pos.y + 160.0f,
		m_pos.z + kSightRange * cosf(baseAngle + halfFov)
	);
	//扇形の境界線を描画
	DrawLine3D(center, leftEdge, color);
	DrawLine3D(center, rightEdge, color);

	//プレイヤーが視野内にいるとき色を変える
	Vector3 dir = (m_pPlayer->GetPosition() - m_pos);
	float dist = dir.SqMagnitude();
	if (dist <= kSightRange * kSightRange)
	{
		//内積の計算
		float dot = m_forward.Dot(dir.Normalize());
		//視野角の境界になる内積の値
		float halfFovCos = cosf(kFov * 0.5f * DX_PI_F / 180.0f);
		//視野の中ならプレイヤーまで赤い線を引く
		if (dot >= halfFovCos)
		{
			// 発見中は赤で上書き
			DrawLine3D(center, VGet(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y + 160.0f, m_pPlayer->GetPosition().z), GetColor(255, 0, 0));
		}
	}
}

void Enemy::FacePlayer()
{
	m_forward = (m_pPlayer->GetPosition() - m_pos).Normalize();
	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;
}

void Enemy::ChasePlayer(float rotateSpeed, float scale)
{
	Vector3 dir = m_pPlayer->GetPosition() - m_pos;
	Vector3 targetDir = dir.Normalize();

	m_forward += (targetDir - m_forward) * rotateSpeed;
	m_forward = m_forward.Normalize();

	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;

	m_pos += m_forward * m_speed * scale;
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

	VECTOR start = VGet(debugPos.x,debugPos.y + GetCollisionRadius(),debugPos.z);

	VECTOR end = VGet(debugPos.x,debugPos.y + GetCollisionHeight() - GetCollisionRadius(),debugPos.z);

	DrawCapsule3D(start,end,GetCollisionRadius(),16,0xffffff,0xffffff,false);
}