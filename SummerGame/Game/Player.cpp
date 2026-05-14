#include "Player.h"
#include "DxLib.h"
#include "../System/Input.h"

namespace
{
	//プレイヤーからカメラに向かうベクトル
	const Vector3 kPlayerToTarget = VGet(0.0f, 290.0f, 0.0f);
}

Player::Player():
	m_modelHandle(-1),
	m_jumpPower(0)
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
	m_jumpPower = 6;
}

void Player::Update(const Input& input)
{
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
	if (input.IsPressed("Jump"))
	{
		m_pos.y += m_jumpPower;
	}
	m_pos.y -= m_gravity;

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
