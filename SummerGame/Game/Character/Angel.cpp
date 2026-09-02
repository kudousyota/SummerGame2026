#include "Angel.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "Player.h"
#include "../System/Timer.h"
#include "../System/Model.h"
#include "../Effect/EffectManager.h"

namespace
{
	//==================================================
	// アニメーション
	//==================================================

	//登場時の叫び
	const char* const kShoutAnimName = "Angel|Shout";

	//走り
	const char* const kRunAnimName = "Angel|Run";

	//連続攻撃
	const char* const kDancingAttackAnimName = "Angel|DancingAttack";

	//プレイヤーを見失ったとき
	const char* const kRotateAnimName = "Angel|Rotate";

	//ダメージ
	const char* const kDamageAnimName = "Angel|Hit";

	//死亡
	const char* const kDeadAnimName = "Angel|Dead";


	//==================================================
	// リグ
	//==================================================

	//目元のリグ
	const char* const kEyegRig = "mixamorig:RightEye";


	//==================================================
	// ステータス
	//==================================================

	//最大HP
	constexpr int kMaxHP = 500;

	//攻撃力
	constexpr int kAttackPower = 20;

	//スコア
	constexpr int kScore = 500;

	//サイズ
	constexpr float kScale = 1.0f;

	//==================================================
	// 移動・索敵
	//==================================================

	//プレイヤーを追跡するときの移動速度
	constexpr float kChaseSpeed = 0.15f;

	//最後にプレイヤーを確認した場所まで移動するときの速度
	constexpr float kSearchMoveSpeed = 0.15f;

	//最後に確認した場所に到着したと判定する距離
	constexpr float kSearchArriveDistance = 30.0f;


	//==================================================
	// 当たり判定
	//==================================================

	//エンジェル本体の当たり判定半径
	constexpr float kCollisionRadius = 50.0f;

	//エンジェル本体の当たり判定高さ
	constexpr float kCollisionHeight = 160.0f;

	//攻撃判定の半径
	constexpr float kAttackRadius = 180.0f;


	//==================================================
	// 攻撃
	//==================================================

	//ラッシュ攻撃の攻撃回数
	constexpr int kDancingAttackCount = 8;

	//各攻撃の攻撃判定を発生させるアニメーションフレーム
	constexpr int kAttackDamageFrame[kDancingAttackCount] =
	{
		20,
		40,
		60,
		80,
		100,
		120,
		140,
		160
	};

	//攻撃後のクールタイム
	constexpr float kAttackCooldown = 90.0f;

	constexpr float kAttackRange = 150.0f;
	//==================================================
	// 攻撃予兆
	//==================================================

	//攻撃予兆を表示してから攻撃するまでの時間
	constexpr float kHazardFrame = 15.0f;

	//攻撃予兆エフェクトのY座標オフセット
	constexpr float kHazardOffsetY = 180.0f;


	//==================================================
	// デバッグ
	//==================================================

	//攻撃判定デバッグ表示の表示時間
	constexpr float kAttackDebugDisplayFrame = 5.0f;

}

Angel::Angel():
	m_currentState(AngelState::Idle),
	m_prevState(AngelState::Idle),
	m_dancingAttackHit{false,false, false, false, false, false, false, false },
	m_lastSeePos({0.0f,0.0f,0.0f})
{
}

Angel::~Angel()
{
}

void Angel::Init()
{
	Enemy::Init();

	m_currentState = AngelState::Shout;
	m_prevState = AngelState::Shout;

	//ステータス
	m_hp = kMaxHP;
	m_attackPower = kAttackPower;

	//視界
	//m_sightRange = 400.0f;
	//m_fov = 120.0f;
	
	//サイズ
	m_scale = Vector3(kScale, kScale, kScale);
	//当たり判定
	m_collisionRadius = kCollisionRadius;
	m_collisionHeight = kCollisionHeight;

	m_modelHandle = Model::Instance().CreateAngelModel();

	m_score = kScore;

	m_animation.Init(m_modelHandle, kShoutAnimName, true, 0.5f);

}

void Angel::Update()
{
	if (m_isDead)
	{
		return;
	}

	Character::Collision();

	//タイムスケールの取得
	m_timeScale = Timer::Instance().GetEnemyTimeScale();

	//攻撃クールタイム
	UpdateCooldown(m_timeScale);

	m_animation.Update(m_timeScale);
	if (m_currentState == AngelState::Dead)
	{
		if (m_animation.GetAnimEndFlag())
		{
			//アニメーションが終わったら完全に死亡扱い
			m_isDead = true;
		}
		return;
	}
	//アニメーションのフレーム
	float animTime = m_animation.GetCurrentAnimTime();

	//float animpos = MV1GetFramePosition(m_modelHandle,50);

	switch (m_currentState)
	{
	case AngelState::Shout:

		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(AngelState::Run);
		}

		break;

	case AngelState::Idle:

		if (CanSeePlayer())
		{
			TransitionTo(AngelState::Run);
		}
		break;
	case AngelState::Run:
	{
		
		//視野角から消えたら待機にする
		if (!CanSeePlayer())
		{
			//プレイヤーが最後にいた場所を記憶
			m_lastSeePos = m_pPlayer->GetPosition();
			TransitionTo(AngelState::Look);
			break;
		}

		Vector3 dir = m_pPlayer->GetPosition() - m_pos;
		float distSq = dir.SqMagnitude();

		if (distSq <= kAttackRange * kAttackRange)
		{
			if (m_attackCooldown <= 0)
			{
				//現在の位置にエフェクトを出す
				m_hazardPos = m_pos;
				//ちょっと高めにする
				m_hazardPos.y += kHazardOffsetY;

				FacePlayer();
				EffectManager::Instns().PlayEffect(EffectType::Hazard, m_hazardPos);
				//30f待つ
				m_attackWarnigFrame = kHazardFrame;

				TransitionTo(AngelState::AttackWarnig);

			}
		}
		else
		{
			//追跡
			ChasePlayer(kChaseSpeed, m_timeScale);
		}
		
	}
		break;

	case AngelState::AttackWarnig:

		m_attackWarnigFrame -= m_timeScale;

		if (m_attackWarnigFrame <= 0.0f)
		{
			TransitionTo(AngelState::DancingAttack);
		}
		break;
	case AngelState::DancingAttack:
		{
		
		for (int i = 0; i < kDancingAttackCount; i++)
		{
			if (!m_dancingAttackHit[i] && animTime >= kAttackDamageFrame[i])
			{
				CollisionManager::Instance().CheckAttackSphere(CreateAttackData(), m_pos);
				EffectManager::Instns().PlayEffect(EffectType::Wind, m_pos);
				m_dancingAttackHit[i] = true;
			}
		}
		//アニメーションが終わったら追跡に戻る
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(AngelState::Run);
		}
		
		}
		break;
	case AngelState::Damage:
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(AngelState::Run);
		}
		break;
	case AngelState::Look:

		//プレイヤーを見つけたら追いかける
		if (CanSeePlayer())
		{
			TransitionTo(AngelState::Run);
			break;
		}

		Vector3 dir = m_lastSeePos - m_pos;
		dir.y = 0.0f;

		if (dir.SqMagnitude() < kSearchArriveDistance * kSearchArriveDistance)
		{
			TransitionTo(AngelState::Idle);
		}
		else
		{
			MoveTo(m_lastSeePos, kSearchMoveSpeed, m_timeScale);
		}

	}
	// モデル行列更新
	UpdateModelMatrix();
}

void Angel::Draw()
{
	if (m_isDead)
	{
		return;
	}

	MV1DrawModel(m_modelHandle);
	
#ifdef _DEBUG
	DrawDebugCollision();
	DrawFormatString(300, 120, GetColor(255, 255, 255), "AngelHP:%d", m_hp);

	float animTime = m_animation.GetCurrentAnimTime();

	bool drawAttack = false;

	for (int i = 0; i < kDancingAttackCount; i++)
	{
		if (animTime >= kAttackDamageFrame[i] &&animTime < kAttackDamageFrame[i] + kAttackDebugDisplayFrame)//表示するフレーム
		{
			drawAttack = true;
			break;
		}
	}
	//敵の攻撃タイミングでデバッグ表示
	if (m_currentState ==AngelState::DancingAttack && drawAttack)
	{
		DrawSphere3D(m_pos.ToDxLibVector(),CreateAttackData().GetRadius(),16,0xffffff,0xffffff,false);
	}
	//索敵デバッグ表示
	DrawDebugSight();
#endif
}


void Angel::TransitionTo(AngelState nextState)
{
	if (m_currentState == nextState)
	{
		return;
	}

	m_prevState = m_currentState;
	m_currentState = nextState;

	switch (m_currentState)
	{
	case AngelState::Shout:
		m_animation.ChangeAnim(kShoutAnimName, false, 0.5);
		break;
	case AngelState::Idle:
		m_animation.ChangeAnim(kShoutAnimName, true, 0.5f);
		break;
	case AngelState::Run:
		m_animation.ChangeAnim(kRunAnimName, true, 0.5f);
		break;
	case AngelState::DancingAttack:
		m_attackCooldown = kAttackCooldown;

		//ラッシュの攻撃は複数回当たる可能性があるので、当たったかどうかを管理する配列をリセットする
		for (int i = 0; i < kDancingAttackCount; i++)
		{
			m_dancingAttackHit[i] = false;
		}

		m_animation.ChangeAnim(kDancingAttackAnimName, false, 0.5);
		break;
	case AngelState::Damage:
		m_animation.ChangeAnim(kDamageAnimName, false, 0.5f);
		break;

	case AngelState::Look:
		m_animation.ChangeAnim(kRotateAnimName, true, 0.5f);
		break;
	case AngelState::Dead:
		m_animation.ChangeAnim(kDeadAnimName,false,0.5f);
		break;
	}
}

void Angel::OnDamaged()
{
	TransitionTo(AngelState::Damage);
}

void Angel::OnDead()
{
	TransitionTo(AngelState::Dead);
}


void Angel::OnHit(const AttackData& attackdata)
{

	ApplyDamage(attackdata.GetDamage());

	//攻撃されたらプレイヤーの方を向く
	FacePlayer();
}

AttackType Angel::GetAttackType() const
{
	return AttackType::Punch;
}

float Angel::GetAttackRadius() const
{
	return kAttackRadius;
}

Vector3 Angel::GetHitEffect() const
{
	return m_pos + Vector3(0.0f,GetCollisionHeight()/2,0.0f);
}
