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
	m_isInput(true)
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
	//カメラ基準のベクトル
	Vector3 forwrd = m_pCamera->GetForward();
	Vector3 right = m_pCamera->GetRight();

	//入力方向を取得
	Inputdata now = Inputdata::None;
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
	//角度変更
	if (moveVec.SqMagnitude() > 0.0001f)
	{
		moveVec = moveVec.Normalize();
		Vector3 nexPos = m_pos + moveVec * m_speed;

		m_angle = atan2f(moveVec.x, moveVec.z) + DX_PI_F;
		m_isInput = true;
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
	//アニメーションの切り替え
	const char* nextAnimName;

	//移動しているときは歩きモーション
	if(m_pos.x != 0.0f || m_pos.z != 0.0f)
	{
		nextAnimName = kWalkAnimName;
	}
	else
	{
		nextAnimName = kIdleAnimName;
	}

	//再生したいアニメーションのハンドルを取得
	int animNo = MV1GetAnimIndex(m_modelHandle, nextAnimName);

	//アニメーションの切り替え処理
	if (animNo != m_currentAnimIndex)
	{
		if(m_lastAnimHandle != -1)
		{
			MV1DetachAnim(m_modelHandle,m_lastAnimHandle);
		}
		// 現在のアニメを前のアニメとして保存
	   // ブレンド用にこの後もしばらく再生し続ける
		m_lastAnimHandle = m_cureentAnimHandle;
		m_lastAnimCount = m_currentAnimCount;

		// 新しいアニメーションをアタッチ
		m_cureentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
		m_currentAnimIndex = animNo;

		// 新しいアニメーションは最初から再生
		m_currentAnimCount = 0.0f;

		// ブレンド開始フレームをリセット
		m_animChangeFrame = 0;
	}

}
