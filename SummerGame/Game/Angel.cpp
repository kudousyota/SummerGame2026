#include "Angel.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "Player.h"
#include "../System/Timer.h"
#include "../System/Model.h"

namespace
{
	const char* const kShoutAnimName = "Angel|Shout";

	const char* const kRunAnimName = "Angel|Run";

	const char* const kDancingAttackAnimName = "Angel|DancingAttack";

	const char* const kRotateAnimName = "Angel|Rotate";

	const char* const kDamageAnimName = "Angel|Hit";

	constexpr int kDanicgAttackRadius = 180.0f;

	//ラッシュ攻撃回数
	constexpr int kDancingAttackCount = 8;

	//ダメージを出すフレーム
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

}

Angel::Angel():
	m_currentState(AngelState::Idle),
	m_prevState(AngelState::Idle),
	m_dancingAttackHit{false,false, false, false, false, false, false, false }
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

	m_hp = 300;
	m_attackPower = 20;

	m_modelHandle = Model::Instance().CreateAngelModel();
	m_animation.Init(m_modelHandle, kShoutAnimName, true, 0.5f);
}

void Angel::Update()
{
	if (m_isDead)
	{
		return;
	}

	Character::Collision();
	//敵用のタイムスケールを取得
	float scale = Timer::Instance().GetEnemyTimeScale();
	//アニメーションのフレーム
	float animTime = m_animation.GetCurrentAnimTime();

	//攻撃クールタイム
	UpdateCooldown(scale);

	m_animation.Update(scale);

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
			TransitionTo(AngelState::Idle);
			break;
		}

		Vector3 dir = m_pPlayer->GetPosition() - m_pos;
		float distSq = dir.SqMagnitude();

		if (distSq <= kAttackRange * kAttackRange)
		{
			if (m_attackCooldown <= 0)
			{
				FacePlayer();
				TransitionTo(AngelState::DancingAttack);
			}
		}
		else
		{
			//追跡
			ChasePlayer(0.15f, scale);
		}
		
	}
		break;
	case AngelState::DancingAttack:
		{
		
			for (int i = 0; i < kDancingAttackCount; i++)
			{
				if (!m_dancingAttackHit[i] && animTime >= kAttackDamageFrame[i])
				{
					CollisionManager::Instance().CheckAttackSphere(this, m_pos, kDanicgAttackRadius, m_attackPower);

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
		m_attackCooldown = 90;

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
	}
}

void Angel::OnDamaged()
{
	TransitionTo(AngelState::Damage);
}

