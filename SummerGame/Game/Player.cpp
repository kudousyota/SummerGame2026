#include "Player.h"
#include "DxLib.h"
#include "../System/Input.h"
#include "Camera.h"
#include "../System/CollisionManager.h"
#include "Stage.h"

namespace
{
	//プレイヤーからカメラに向かうベクトル
	const Vector3 kPlayerToTarget = VGet(0.0f, 290.0f, 0.0f);
	//重力加速度

	const float kGravity = 0.5f;
	//アニメーションの名前
	const char* const kIdleAnimName = "Player|Idle";
	
	const char* const kWalkAnimName = "Player|Walk";

	const char* const kPunchAnimName = "Player|Punch";

	const char* const kPunchRushAnimName = "Player|Punchrush";

	const char* const kJumpAnimName = "Player|Jump";

	const char* const kDodgeAnimName = "Player|Dodge";

	//攻撃中のフレーム
	constexpr float kPunchAnimFrame = 10;

	constexpr int kAnimBlendFrame = 10;

	constexpr float kAttackStartFrame = 10.0f;

	constexpr float kAttackEndFrame = 15.0f;
}

Player::Player() :
	m_pCamera(nullptr),
	m_jumpPower(0),
	m_forward(VGet(0.0f, 0.0f, 1.0f)),
	m_hakutoHandle(-1),
	m_isAttackHit(false),
	m_isNextAttack(false),
	m_attackPos(VGet(0.0f,0.0f,0.0f))
{
}

Player::~Player()
{
	CollisionManager::Instance().Unregister(this);

	MV1DeleteModel(m_modelHandle);	
}

void Player::Init()
{
	Character::Init();
	//初期状態
	m_currentState = PlayerState::Idle;
	m_prevState = PlayerState::Idle;

	m_pos = VGet(0.0f, 50.0f, 0.0f);

	m_hp = 100;
	m_jumpPower = 10;
	m_modelHandle = MV1LoadModel("data/Player.mv1");
	m_hakutoHandle = LoadGraph("data/kudonetta.png");
	m_animation.Init(m_modelHandle,kIdleAnimName,true,0.5f);
	CollisionManager::Instance().Register(this);
	m_isGround = true;
	
}

void Player::Update()
{

	Vector3 forward = m_pCamera->GetForward();
	Vector3 right = m_pCamera->GetRight();

	auto& input = Input::Instance();

	// アニメーション更新
	m_animation.Update();

	switch (m_currentState)
	{
	case PlayerState::Idle:

		if (input.IsTriggered("Attack"))
		{
			TransitionTo(PlayerState::Attack);
		}
		if (input.IsPressed("up")	||
			input.IsPressed("down") ||
			input.IsPressed("left") ||
			input.IsPressed("right"))
		{
			TransitionTo(PlayerState::Walk);
		}

		if (input.IsTriggered("Jump")&& m_isGround)
		{
			TransitionTo(PlayerState::Jump);

			//上にジャンプパワーを入れる
			m_velocity.y = m_jumpPower;

			//ジャンプしたら空中ってことにする
			m_isGround = false;

			//地面判定から1ピクセルだけ浮かす
			m_pos.y += 1.0f;

		}

		if (input.IsTriggered("Dodge"))
		{
			TransitionTo(PlayerState::Dodge);
		}

		break;

	case PlayerState::Walk:

		if (input.IsTriggered("Attack"))
		{
			TransitionTo(PlayerState::Attack);
		}

		if (!input.IsPressed("up") &&
			!input.IsPressed("down") &&
			!input.IsPressed("left") &&
			!input.IsPressed("right"))
		{
			TransitionTo(PlayerState::Idle);
		}

		if (input.IsTriggered("Jump"))
		{
			TransitionTo(PlayerState::Jump);

			//上にジャンプパワーを入れる
			m_velocity.y = m_jumpPower;

			//ジャンプしたら空中ってことにする
			m_isGround = false;

			//地面判定から1ピクセルだけ浮かす
			m_pos.y += 1.0f;
		}

		break;

	case PlayerState::Attack:

		//コンボ受け付け
		if (input.IsTriggered("Attack"))
		{
			m_isNextAttack = true;
		}
		// 攻撃アニメ終了
		if (m_animation.GetAnimEndFlag())
		{
			if (m_isNextAttack)
			{
				TransitionTo(PlayerState::Rush);
				m_isNextAttack = false;
			}
			else
			{
				TransitionTo(PlayerState::Idle);
			}
			
		}

		break;
	
	case PlayerState::Rush:

		//float rate = m_animation.GetAnimRate();

		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(PlayerState::Idle);

		}
		break;
	
	case PlayerState::Jump:
		
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(PlayerState::Idle);
		}
		break;

	case PlayerState::Dodge:

		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(PlayerState::Idle);
		}
		break;
	}

	//前の場所
	Vector3 oldPos = m_pos;
	

	//通常移動
	if (m_currentState == PlayerState::Idle || m_currentState == PlayerState::Walk || m_currentState == PlayerState::Jump)
	{
		Vector3 moveVec(0, 0, 0);

		if (input.IsPressed("up"))
		{
			moveVec += forward;
		}
		if (input.IsPressed("down"))
		{
			moveVec -= forward;
		}
		
		if (input.IsPressed("right"))
		{
			moveVec += right;
		}

		if (input.IsPressed("left"))
		{
			moveVec -= right;
		}

		if (moveVec.SqMagnitude() > 0.0001f)
		{
			moveVec = moveVec.Normalize();

			m_forward = moveVec;
			m_pos += moveVec * m_speed;

			m_angle = atan2f(moveVec.x, moveVec.z) + DX_PI_F;
		}
	}

	Character::Collision();

	//前側に表示高さは微調整
	m_attackPos = m_pos + m_forward * 70.0f + VGet(0.0f, 50.0f, 0.0f);

	// モデル行列更新
	MATRIX rot = MGetRotY(m_angle);
	MATRIX trans = MGetTranslate(m_pos.ToDxLibVector());
	MV1SetMatrix(m_modelHandle, MMult(rot, trans));

	//攻撃判定
	AttackUpdate();
	
}

void Player::Draw()
{
	MV1DrawModel(m_modelHandle);

	DrawCapsule3D(m_pos.ToDxLibVector(),VGet(m_pos.x, m_pos.y + 100.0f, m_pos.z),30.0f,16,GetColor(0, 255, 0),GetColor(0, 255, 0),false);

	DrawBillboard3D(VGet(100.0f, 50.0f, 30.0f), 0.0f, 1.0f, 450.0f, 0.0f,m_hakutoHandle, true);

	float animTime = m_animation.GetCurrentAnimTime();

	//攻撃判定
	if (m_currentState == PlayerState::Attack &&
		animTime >= kAttackStartFrame &&
		animTime <= kAttackEndFrame)
	{
		DrawSphere3D(m_attackPos.ToDxLibVector(),50.0f,6,0xffffff,0xffffff,false);
	}
	if (m_currentState == PlayerState::Rush)
	{
		DrawSphere3D(m_attackPos.ToDxLibVector(), 50.0f, 6, 0x00ffff, 0x00ffff, false);
	}

}

Vector3 Player::GetCameraTarget() const
{
	return m_pos + kPlayerToTarget;
}
void Player::AttackUpdate()
{
	if (m_currentState != PlayerState::Attack && m_currentState != PlayerState::Rush)
	{
		return;
	}

	float animTime = m_animation.GetCurrentAnimTime();

	if (animTime >= kAttackStartFrame &&
		animTime <= kAttackEndFrame)
	{
		if (!m_isAttackHit)
		{
			CollisionManager::Instance().CheckAttackSphere(this,m_attackPos,50.0f,m_attackPower);

			m_isAttackHit = true;
		}
		if (m_currentState == PlayerState::Rush)
		{
			CollisionManager::Instance().CheckAttackSphere(this, m_attackPos, 50.0f, m_attackPower);
		}
		
	}
	
	

}
void Player::TransitionTo(PlayerState nextState)
{
	if (m_currentState == nextState)
	{
		return;
	}

	m_prevState = m_currentState;
	m_currentState = nextState;

	switch (m_currentState)
	{
	case PlayerState::Idle:

		m_animation.ChangeAnim(kIdleAnimName,true,0.5f);
		break;

	case PlayerState::Walk:

		m_animation.ChangeAnim(kWalkAnimName,true,0.5f);
		break;

	case PlayerState::Attack:

		m_isAttackHit = false;
		m_isNextAttack = false;

		m_animation.ChangeAnim(kPunchAnimName,false,0.5f);
		break;
	case PlayerState::Rush:

		m_animation.ChangeAnim(kPunchRushAnimName, false, 0.9f);
		break;

	case PlayerState::Jump:
		m_animation.ChangeAnim(kJumpAnimName, false, 0.38f);
		break;

	case PlayerState::Dodge:
		m_animation.ChangeAnim(kDodgeAnimName, false, 0.8f);
		break;

	default:
		break;
	}
}