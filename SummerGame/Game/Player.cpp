#include "Player.h"
#include "DxLib.h"
#include "../System/Input.h"
#include "Camera.h"

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

	const char* const kPunchrushAnimName = "Player|Punchrush";

	//攻撃中のフレーム
	constexpr float kPunchAnimFrame = 10;

	constexpr int kAnimBlendFrame = 10;
}

Player::Player() :
	m_pCamera(nullptr),
	m_modelHandle(-1),
	m_jumpPower(0),
	m_isGround(true),
	m_currentAnimCount(0.0f),
	m_lastAnimCount(0.0f),
	m_cureentAnimHandle(-1),
	m_lastAnimHandle(-1),
	m_currentAnimIndex(-1),
	m_animChangeFrame(0),
	m_isInput(false),
	m_isAttack(false),
	m_inputState(Inputdata::None),
	m_forward(VGet(0.0f,0.0f,0.0f)),
	m_hakutoHandle(-1)
{
}

Player::~Player()
{
	MV1DeleteModel(m_modelHandle);
}

void Player::Init()
{
	Character::Init();
	

	m_hp = 100;
	m_jumpPower = 10;
	m_modelHandle = MV1LoadModel("data/Player.mv1");
	m_hakutoHandle = LoadGraph("data/okirohakuto.jpg");
	int animIndex = MV1GetAnimIndex(m_modelHandle, kIdleAnimName);
	m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animIndex,-1,-1);

	m_currentAnimIndex = animIndex;

	m_lastAnimHandle = -1;
	m_lastAnimCount = 0.0f;

	m_animChangeFrame = 0;

}

void Player::Update(const Input& input)
{
	AtackUpdate(input);

	Vector3 forwrd = m_pCamera->GetForward();
	Vector3 right = m_pCamera->GetRight();

	m_inputState = Inputdata::None;

	// 移動処理
	bool isMoving = false;

	//攻撃中じゃないとき受け付ける
	if (!m_isAttack)
	{
		if (input.IsTriggered("Attack"))
		{
			m_inputState = Inputdata::Attack;
			m_isAttack = true;
		}
		else if (input.IsPressed("up"))
		{
			m_inputState = Inputdata::Up;
		}
		else if (input.IsPressed("down"))
		{
			m_inputState = Inputdata::Down;
		}
		else if (input.IsPressed("left"))
		{
			m_inputState = Inputdata::Left;
		}
		else if (input.IsPressed("right"))
		{
			m_inputState = Inputdata::Right;
		}
	}
	else
	{
		m_inputState = Inputdata::Attack;
	}
	

	m_isInput = (m_inputState != Inputdata::None);
	
	//通常移動
	Vector3 moveVec(0.0f, 0.0f, 0.0f);
	if (input.IsPressed("up"))
	{
		moveVec += forwrd;
	}
	if (input.IsPressed("down"))
	{
		moveVec -= forwrd;
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
		//移動方向を保存
		m_forward = moveVec;
		m_pos += moveVec * m_speed;

		m_angle = atan2f(moveVec.x, moveVec.z) + DX_PI_F;
		
		DrawFormatString(100, 10, GetColor(255, 255, 255), "m_angle:%f", m_angle);
	}
	// モデル行列更新
	MATRIX rot = MGetRotY(m_angle);
	MATRIX trans = MGetTranslate(m_pos.ToDxLibVector());
	MV1SetMatrix(m_modelHandle, MMult(rot, trans));

	// アニメーション更新
	AnimUpdate();
}

void Player::Draw()
{
	MV1DrawModel(m_modelHandle);

	DrawBillboard3D(m_pos, 0.0f, 0.0f,500, 0.0f, m_hakutoHandle, false);

	DrawCapsule3D(m_pos.ToDxLibVector(),
		VGet(m_pos.x, m_pos.y + 100.0f, m_pos.z),
		30.0f,
		16,
		GetColor(0, 255, 0),
		GetColor(0, 255, 0),
		false);

	if (m_isAttack&&m_currentAnimCount >= kPunchAnimFrame)
	{
		//前側に表示高さは微調整
		Vector3 attackpos = m_pos + m_forward * 70.0f + VGet(0.0f, 50.0f, 0.0f);

		DrawSphere3D(attackpos.ToDxLibVector(), 50.0f, 6, 0xffffff, 0xffffff, false);
	}
}

Vector3 Player::GetCameraTarget() const
{
	return m_pos + kPlayerToTarget;
}

void Player::AnimUpdate()
{
	// アニメ進行
	m_currentAnimCount += 0.5f;
	float total = MV1GetAttachAnimTotalTime(m_modelHandle, m_cureentAnimHandle);
	if (m_currentAnimCount >= total)
	{
		m_currentAnimCount -= total;

		m_isAttack = false;
	}
	MV1SetAttachAnimTime(m_modelHandle, m_cureentAnimHandle, m_currentAnimCount);

	if (m_lastAnimHandle != -1)
	{
		m_lastAnimCount += 0.5f;
		float lastTotal = MV1GetAttachAnimTotalTime(m_modelHandle, m_lastAnimHandle);
		if (m_lastAnimCount >= lastTotal)
		{
			m_lastAnimCount -= lastTotal;
		}
		MV1SetAttachAnimTime(m_modelHandle, m_lastAnimHandle, m_lastAnimCount);
	}
	//アニメーションの切り替え
	const char* nextAnimName = kIdleAnimName;

	switch (m_inputState)
	{
	case Inputdata::Up:
	case Inputdata::Down:
	case Inputdata::Left:
	case Inputdata::Right:
		nextAnimName = kWalkAnimName;
		break;
	case Inputdata::Attack:
		nextAnimName = kPunchAnimName;
		break;
	case Inputdata::Punchrush:
		nextAnimName = kPunchrushAnimName;
		break;
	case Inputdata::None:
	default:
		nextAnimName = kIdleAnimName;
		break;

	}

	//再生したいアニメーションのハンドルを取得
	int animNo = MV1GetAnimIndex(m_modelHandle, nextAnimName);
	
	//アニメーションの切り替え処理
	if (animNo != m_currentAnimIndex)
	{
		if(m_lastAnimHandle != -1)
		{
			MV1DetachAnim(m_modelHandle,m_lastAnimHandle);

			m_lastAnimHandle = -1;
		}
		m_lastAnimHandle = m_cureentAnimHandle;
		m_lastAnimCount = m_currentAnimCount;

		// 新しいアニメーションをアタッチ
		m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
		
		m_currentAnimIndex = animNo;
		// 新しいアニメーションは最初から再生
		m_currentAnimCount = 0.0f;
		MV1SetAttachAnimBlendRate(m_modelHandle, m_cureentAnimHandle, 0.0f);
		// ブレンド開始フレームをリセット
		m_animChangeFrame = 0;
	}
	//ブレンド
	if (m_lastAnimHandle != -1)
	{
		m_animChangeFrame++;

		float rate =
			static_cast<float>(m_animChangeFrame) /
			kAnimBlendFrame;

		if (rate > 1.0f)
		{
			rate = 1.0f;
		}

		//新しいアニメーション
		MV1SetAttachAnimBlendRate(
			m_modelHandle,
			m_cureentAnimHandle,
			rate);

		//前のアニメーション
		MV1SetAttachAnimBlendRate(
			m_modelHandle,
			m_lastAnimHandle,
			1.0f - rate);

		//いらないやつ
		if (rate >= 1.0f)
		{
			MV1DetachAnim(
				m_modelHandle,
				m_lastAnimHandle);

			m_lastAnimHandle = -1;
		}
	}
	else
	{
		MV1SetAttachAnimBlendRate(
			m_modelHandle,
			m_cureentAnimHandle,
			1.0f);
	}
}

void Player::AtackUpdate(const Input& input)
{
	if (m_isAttack&&m_inputState == Inputdata::Attack)
	{

		if (m_currentAnimCount <= kPunchAnimFrame)
		{
			m_isAttack = true;
		}

	}
}
