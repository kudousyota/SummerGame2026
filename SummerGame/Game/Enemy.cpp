#include "Enemy.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "Player.h"
#include "../System/Timer.h"

namespace
{
	const char* const kIdleAnimName = "Enemy|Idle";

	const char* const kWalkAnimName = "Enemy|Walk";
	//強攻撃
	const char* const kAttackAnimName = "Enemy|Attack";
	//弱攻撃
	const char* const kPunchAnimName = "Enemy|Punch";

	constexpr float kAttackRange = 150.0f;

	//見える距離
	constexpr float kSightRange = 500.0f;
	//視野角
	constexpr float kFov = 90.0f;
}

Enemy::Enemy():
	m_modelHandle(-1),
	m_isDead(false),
	m_isAttackHit(false),
	m_attackCooldown(0),
	m_forward(VGet(0.0f, 0.0f, 1.0f)),
	m_isAttacking(false),
	m_attackFrame(0),
	m_currentState(EnemyState::Idle),
	m_prevState(EnemyState::Idle),
	m_isAttack(false),
	m_attackDir(VGet(0.0f,0.0f,0.0f))
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

	//本物の体力
	//m_hp = 150;
	m_hp = 500;

	m_attackPower = 20;
	

	m_currentState = EnemyState::Idle;
	m_prevState = EnemyState::Idle;

	m_attackCooldown = 0;

	m_pos = VGet(0.0f, 0.0f, 250.0f);
	m_modelHandle = MV1LoadModel("Data/Enemy.mv1");
	m_animation.Init(m_modelHandle, kIdleAnimName, true, 0.5f);

	CollisionManager::Instance().Register(this);
}

void Enemy::Update()
{
	//死んだら
	if (m_hp == 0)
	{
		return;
	}

	Character::Collision();

	//タイムスケールの取得
	float scale = Timer::Instance().GetEnemyTimeScale();
	//アニメーションの更新
	m_animation.Update(scale);
	
	Timer::Instance().Update();

	//攻撃表示タイマー
	if (m_attackFrame > 0)
	{
		m_attackFrame--;
	}
	//攻撃クールタイム
	if (m_attackCooldown > 0)
	{
		//攻撃のクールタイムもウィッチタイムで遅くする
		m_attackCooldown -= scale;
	}

	//ステート
	switch (m_currentState)
	{
	case EnemyState::Idle:
	{
		if (m_attackCooldown <= 0)
		{
			TransitionTo(EnemyState::Walk);
		}
		
	}
		break;
	case EnemyState::Walk:
	{
		Vector3 dir = m_pPlayer->GetPosition() - m_pos;
		float distSq = dir.SqMagnitude();
		//攻撃にする
		if (distSq <= kAttackRange * kAttackRange)
		{
			if (m_attackCooldown <= 0)
			{
				m_forward = (m_pPlayer->GetPosition() - m_pos).Normalize();
				m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;

				TransitionTo(EnemyState::Attack);
			}
		}
		else
		{
			//追跡
			float rotSpeed = 0.15f;
			Vector3 targetDir = dir.Normalize();
			//現在の向きから目標方向に少しずつ近づく
			m_forward = m_forward + (targetDir - m_forward) * rotSpeed;
			//正規化
			m_forward = m_forward.Normalize();

			m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;
			//速度を与えるウィッチタイムで遅くなるように
			m_pos += m_forward * m_speed * scale;
		}
	}
		break;

	case EnemyState::Attack:
		if (!m_isAttack &&
			m_animation.GetAnimRate() >= 0.5f)
		{
			AttackUpdate();
			m_isAttack = true;

			m_attackDir = (m_pPlayer->GetPosition() - m_pos).Normalize();
		}

		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(EnemyState::Idle);
		}
		break;

	case EnemyState::Punch:
		if()
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

	DrawCapsule3D(m_pos.ToDxLibVector(),VGet(m_pos.x, m_pos.y + 100.0f, m_pos.z),70.0f,16,GetColor(255, 0, 0),GetColor(255, 0, 0),false);


	//攻撃の時に判定を表示
	if (m_attackFrame > 0)
	{
		DrawSphere3D(m_attackPos.ToDxLibVector(),50.0f,16,GetColor(0, 255, 0),GetColor(0, 255, 0),false);
	}

	DrawFormatString(50,50,GetColor(255, 255, 255),"EnemyHP:%d",m_hp);

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

void Enemy::AttackUpdate()
{
	//前側に表示高さは微調整
	m_attackPos = m_pos + m_attackDir * 70.0f + VGet(0.0f, 20.0f, 0.0f);

	CollisionManager::Instance().CheckAttackSphere(this,m_attackPos,50.0f,m_attackPower);
	m_isAttacking = true;
	m_attackFrame = 30;


}

void Enemy::TransitionTo(EnemyState nextState)
{
	if (m_currentState == nextState)
	{
		return;
	}

	m_prevState = m_currentState;
	m_currentState = nextState;

	switch (m_currentState)
	{
	case EnemyState::Idle:
		m_animation.ChangeAnim(kIdleAnimName, true, 0.5f);
		break;
	case EnemyState::Walk:
		m_animation.ChangeAnim(kWalkAnimName, true, 0.5f);
		break;
	case EnemyState::Attack:

		m_animation.ChangeAnim(kAttackAnimName, false, 0.5f);
		//攻撃ステートになったら更新する
		m_isAttack = false;
		//クールタイム設定
		m_attackCooldown = 90;
		
		m_attackDir = m_forward;
		break;
	case EnemyState::Punch:

		m_animation.ChangeAnim(kPunchAnimName, false, 0.5f);

		break;
	}
}
