#include "Player.h"
#include "DxLib.h"
#include "../System/Input.h"

namespace
{
	//プレイヤーからカメラに向かうベクトル
	const Vector3 kPlayerToTarget = VGet(0.0f, 290.0f, 0.0f);
	//重力加速度
	const float kGravity = 0.5f;
	//アニメーションの名前
	const char* const kIdleAnimName = "mixamo.com";
}

Player::Player():
	m_modelHandle(-1),
	m_jumpPower(0),
	m_isGround(true),
	m_currentAnimCount(0.0f),
	m_lastAnimCount(0.0f),
	m_cureentAnimHandle(-1),
	m_lastAnimHandle(-1)
{
}

Player::~Player()
{
	MV1DeleteModel(m_modelHandle);
}

void Player::Init()
{
	Character::Init();

	m_modelHandle = MV1LoadModel("data/Player.mv1");
	m_hp = 100;
	m_jumpPower = 10;
	int animIndex = MV1GetAnimIndex(m_modelHandle, kIdleAnimName);

	m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animIndex);
}

void Player::Update(const Input& input)
{
	// アニメーション更新
	AnimUpdate();

	// 移動
	if (input.IsPressed("up"))
	{
		m_pos.z += m_speed;
	}
	if (input.IsPressed("down"))
	{
		m_pos.z -= m_speed;
	}
	if (input.IsPressed("left"))
	{
		m_pos.x -= m_speed;
	}
	if (input.IsPressed("right"))
	{
		m_pos.x += m_speed;
	}
	// ジャンプ
	if (input.IsTriggered("Jump") && m_isGround)
	{
		// 上向き速度を与える
		m_gravity = m_jumpPower;

		m_isGround = false;
	}
	// ジャンプ中
	if (!m_isGround)
	{
		// 座標更新
		m_pos.y += m_gravity;

		// 重力を加える
		m_gravity -= kGravity;
	}
	//これ以上下に行かないようにする
	if (m_pos.y <= 0.0f)
	{
		m_pos.y = 0.0f;

		m_gravity = 0.0f;

		m_isGround = true;
	}
	


	// モデル行列更新
	MATRIX rot = MGetRotY(m_angle);
	MATRIX trans = MGetTranslate(m_pos.ToDxLibVector());
	MV1SetMatrix(m_modelHandle, MMult(rot, trans));
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
}
