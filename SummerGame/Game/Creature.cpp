#include "Creature.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "Player.h"
#include "../System/Timer.h"
#include "../System/Model.h"

namespace
{
	const char* const kIdleAnimName = "Enemy|Idle";

	const char* const kWalkAnimName = "Enemy|Walk";
	//強攻撃
	const char* const kAttackAnimName = "Enemy|Attack";
	//弱攻撃
	const char* const kPunchAnimName = "Enemy|Punch";

	const char* const kDamageAnimName = "Enemy|Hit";

	constexpr float kAttackRadius = 180.0f;
	constexpr float kPumncRadius = 130.0f;
}

Creature::Creature():
	m_currentState(CreatureState::Idle),
	m_prevState(CreatureState::Idle),
	m_modelDisplayOffsetY(0.0f)
{
}

Creature::~Creature()
{
}

void Creature::Init()
{
	Enemy::Init();

	//本物の体力
	//m_hp = 150;
	m_hp = 500;

	m_attackPower = 20;
	
	m_collisionRadius = 130.0f;
	m_collisionHeight = 530.0f;

	m_currentState = CreatureState::Idle;
	m_prevState = CreatureState::Idle;

	//m_pos = VGet(0.0f, 500.0f, 250.0f);
	m_modelHandle = Model::Instance().CreateCreatureModel();
	m_scale = VGet(3.0f, 3.0f, 3.0f);

	m_animation.Init(m_modelHandle, kIdleAnimName, true, 0.5f);
}

void Creature::Update()
{
	//死んだら
	if (m_isDead)
	{
		OnDead();
		return;
	}
	//当たり判定の更新
	Character::Collision();

	//タイムスケールの取得
	m_timeScale = Timer::Instance().GetEnemyTimeScale();

	//アニメーションの更新
	m_animation.Update(m_timeScale);

	//攻撃表示タイマー
	if (m_attackFrame > 0)
	{
		m_attackFrame--;
	}
	
	//攻撃クールタイム//攻撃のクールタイムもウィッチタイムで遅くする
	UpdateCooldown(m_timeScale);


	//ステート
	switch (m_currentState)
	{
	case CreatureState::Idle:
	{
		//クールタイム中は待つ
		if (m_attackCooldown > 0)
		{
			break;
		}
		//プレイヤーを見つけたら追いかけ始める
		if (CanSeePlayer())
		{
			TransitionTo(CreatureState::Walk);
		}
		
	}
		break;
	case CreatureState::Walk:
	{
		//視野角から消えたら待機にする
		if (!CanSeePlayer())
		{
			TransitionTo(CreatureState::Idle);
			break;
		}
		//プレイヤーのまでのベクトル
		Vector3 dir = m_pPlayer->GetPosition() - m_pos;
		float distsq = dir.SqMagnitude();

		//攻撃範囲に入ったら攻撃
		if (distsq <= kAttackRange * kAttackRange)
		{
			//攻撃
			if (m_attackCooldown <= 0)
			{
				//プレイヤーの方向に向く
				FacePlayer();
				//70％で弱攻撃
				if (GetRand(99) < 70)
				{
					TransitionTo(CreatureState::Punch);
				}
				else
				{
					TransitionTo(CreatureState::Attack);
				}
			}
		}
		else
		{
			//プレイヤーの方向に少しづつ向きを合わせる
			ChasePlayer(0.15f, m_timeScale);
		}
	}
		break;

	case CreatureState::Attack:
		//アニメーションの半分で攻撃判定を出す
		if (!m_isAttack && m_animation.GetAnimRate() >= 0.5f)
		{
			AttackUpdate();
			m_isAttack = true;
		}
		//攻撃終了後は待機状態へ戻る
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(CreatureState::Idle);
		}
		break;

	case CreatureState::Punch:
		//アニメーションの半分で攻撃判定を発生
		if (!m_isAttack && m_animation.GetAnimRate() >= 0.5f)
		{
			AttackUpdate();
			m_isAttack = true;
		}
		//攻撃終了したらIdleに行く
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(CreatureState::Idle);
		}
		break;
	case CreatureState::Damage:
		//ダメージアニメーションが終わったらIdleに戻る
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(CreatureState::Idle);
		}
		break;
	}
	// モデル行列更新
	UpdateModelMatrix();
	
}


void Creature::Draw()
{

	//HPがゼロになったら
	if (m_isDead)
	{
		return;
	}

	MV1DrawModel(m_modelHandle);
#ifdef _DEBUG
	//デバッグ描画
	DrawDebugCollision();

	//攻撃判定表示
	/*if (m_attackFrame > 0)
	{
		DrawSphere3D(m_attackPos.ToDxLibVector(),50.0f,16,GetColor(0, 255, 0),GetColor(0, 255, 0),false);
	}*/

	//HP表示
	DrawFormatString(300,50,GetColor(255, 255, 255),"CreatureHP:%d",m_hp);

	//視界のデバッグ
	DrawDebugSight();
#endif

}

void Creature::OnDead()
{
	m_isDead = true;
}


void Creature::AttackUpdate()
{

	AttackType type;
	float radius;
	if (m_currentState == CreatureState::Punch)
	{
		type = AttackType::Punch;
		radius = kPumncRadius;
	}
	else
	{
		type = AttackType::Attack;
		radius = kAttackRadius;
	}

	//攻撃データを作成
	AttackData attack(CharacterType::Enemy,m_currentState == CreatureState::Punch ? AttackType::Punch : AttackType::Attack,m_attackPower, kAttackRadius);
	//前側に表示高さは微調整
	m_attackPos = m_pos + m_attackDir * 70.0f + VGet(0.0f, 20.0f, 0.0f);
	//攻撃判定を出す
	CollisionManager::Instance().CheckAttackSphere(attack,m_attackPos);
	m_isAttacking = true;
	m_attackFrame = 30;
}


Vector3 Creature::GetCollisionPosition() const
{
	return m_pos + VGet(0.0f, m_modelDisplayOffsetY, 0.0f);
}

void Creature::TransitionTo(CreatureState nextState)
{
	//同じステートなら何もしない
	if (m_currentState == nextState)
	{
		return;
	}
	//ステートの更新
	m_prevState = m_currentState;
	m_currentState = nextState;

	switch (m_currentState)
	{
	case CreatureState::Idle:
		//待機アニメーション
		m_animation.ChangeAnim(kIdleAnimName, true, 0.5f);
		break;
	case CreatureState::Walk:
		//歩きアニメーション
		m_animation.ChangeAnim(kWalkAnimName, true, 0.5f);
		break;
	case CreatureState::Attack:
		//強攻撃アニメーション
		m_animation.ChangeAnim(kAttackAnimName, false, 0.5f);
		//攻撃ステートになったら更新する
		m_isAttack = false;
		//クールタイム設定
		m_attackCooldown = 90;
		//攻撃方向を保存
		m_attackDir = m_forward;
		break;
	case CreatureState::Punch:
		//弱攻撃のアニメーション
		m_animation.ChangeAnim(kPunchAnimName, false, 0.3f);
		//攻撃判定をリセット
		m_isAttack = false;
		//クールタイム設定
		m_attackCooldown = 90;
		//攻撃方向を保存
		m_attackDir = m_forward;

		break;
	case CreatureState::Damage:
		m_animation.ChangeAnim(kDamageAnimName, false, 0.5);
		break;
	}
	
}

void Creature::OnHit(const AttackData& attackdata)
{
	ApplyDamage(attackdata.GetDamage());

	//攻撃されたらプレイヤーの方を向く
	FacePlayer();
}

void Creature::OnDamaged()
{
	TransitionTo(CreatureState::Damage);
}

AttackType Creature::GetAttackType() const
{
	return AttackType::Breath;
}

float Creature::GetAttackRadius() const
{
	return 80.0f;
}

