#include "Enemy.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "Player.h"

Enemy::Enemy():
	m_modelHandle(-1),
	m_isDead(false),
	m_isAttackHit(false),
	m_attackCooldown(0),
	m_forward(VGet(0.0f, 0.0f, 1.0f)),
	m_isAttacking(false),
	m_attackFrame(0)
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

	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;

	m_hp = 150;
	m_attackPower = 20;

	m_attackCooldown = 0;

	m_pos = VGet(0.0f, 0.0f, 250.0f);
	m_modelHandle = MV1LoadModel("Data/Enemy.mv1");


	CollisionManager::Instance().Register(this);
}

void Enemy::Update()
{

	m_isAttacking = false;

	Character::Collision();


	
	//攻撃表示タイマー
	if (m_attackFrame > 0)
	{
		m_attackFrame--;
	}
	//攻撃クールタイム
	if (m_attackCooldown > 0)
	{
		m_attackCooldown--;
	}

	//攻撃発生
	if (m_attackCooldown <= 0)
	{
		AttackUpdate();
		m_attackCooldown = 60;
	}


	//プレイヤーの方向を向く
	Vector3 dir = m_pPlayer->GetPosition() - m_pos;

	if (dir.SqMagnitude() > 0.001f)
	{
		m_forward = dir.Normalize();

		m_angle = atan2f(m_forward.x,m_forward.z) + DX_PI_F;
	}

	// モデル行列更新
	MATRIX rot = MGetRotY(m_angle);
	MATRIX trans = MGetTranslate(m_pos.ToDxLibVector());
	MV1SetMatrix(m_modelHandle, MMult(rot, trans));
	
}


void Enemy::Draw()
{

	//HPがゼロになったら
	if (m_isDead)
	{
		return;
	}

	MV1DrawModel(m_modelHandle);

	DrawCapsule3D(m_pos.ToDxLibVector(),
		VGet(m_pos.x, m_pos.y + 100.0f, m_pos.z),
		70.0f,
		16,
		GetColor(255, 0, 0),
		GetColor(255, 0, 0),
		false
	);

	//攻撃の時に判定を表示
	if (m_attackFrame > 0)
	{
		DrawSphere3D(
			m_attackPos.ToDxLibVector(),
			50.0f,
			16,
			GetColor(0, 255, 0),
			GetColor(0, 255, 0),
			false
		);
	}

	

	DrawFormatString(
		50,
		50,
		GetColor(255, 255, 255),
		"EnemyHP:%d",
		m_hp
	);

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
		//死んだ
		m_isDead = true;

		//当たり判定を消す
		CollisionManager::Instance().Unregister(this);

		printfDx("EnemyDead!\n");
	}

	printfDx("Enemy HP = %d\n",m_hp);
}

void Enemy::SetPlayer(std::shared_ptr<Player> player)
{
	m_pPlayer = player;
}


void Enemy::AttackUpdate()
{
	//前側に表示高さは微調整
	m_attackPos = m_pos + m_forward * 70.0f + VGet(0.0f, 20.0f, 0.0f);

	CollisionManager::Instance().CheckAttackSphere(
		this,
		m_attackPos,
		50.0f,
		m_attackPower
	);
	
	m_attackFrame = 30;

}
