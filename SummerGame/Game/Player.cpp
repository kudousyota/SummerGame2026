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

	constexpr int kAnimBlendFrame = 10;
}

Player::Player():
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
	m_isInput(false)
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
	int animIndex = MV1GetAnimIndex(m_modelHandle, kIdleAnimName);
	m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animIndex,-1,-1);

	m_currentAnimIndex = animIndex;

	m_lastAnimHandle = -1;
	m_lastAnimCount = 0.0f;

	m_animChangeFrame = 0;
}

void Player::Update(const Input& input)
{



	Vector3 forwrd = m_pCamera->GetForward();
	Vector3 right = m_pCamera->GetRight();

	Inputdata now = Inputdata::None;

	// 移動処理
	bool isMoving = false;

	if (input.IsPressed("up"))
	{
		now = Inputdata::Up;
	}
	else if (input.IsPressed("down"))
	{
		now = Inputdata::Down;
	}
	else if (input.IsPressed("left"))
	{
		now = Inputdata::Left;
	}
	else if (input.IsPressed("right"))
	{
		now = Inputdata::Right;
	}

	m_isInput = (now != Inputdata::None);

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
	if (input.IsPressed("left"))
	{
		moveVec -= right;
	}
	if (input.IsPressed("right"))
	{
		moveVec += right;
	}

	if (moveVec.SqMagnitude() > 0.0001f)
	{
		moveVec = moveVec.Normalize();
		m_pos += moveVec * m_speed;

		m_angle = atan2f(moveVec.x, moveVec.z) + DX_PI_F;
		
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
	const char* nextAnimName = m_isInput ? kWalkAnimName:kIdleAnimName;

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

		// 新
		MV1SetAttachAnimBlendRate(
			m_modelHandle,
			m_cureentAnimHandle,
			rate);

		// 旧
		MV1SetAttachAnimBlendRate(
			m_modelHandle,
			m_lastAnimHandle,
			1.0f - rate);

		// 完了
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
