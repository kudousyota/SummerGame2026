#include "Player.h"
#include "DxLib.h"
#include "../System/Input.h"
#include "Camera.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "../System/Timer.h"

namespace
{
	//プレイヤーからカメラに向かうベクトル
	const Vector3 kPlayerToTarget = VGet(0.0f, 290.0f, 0.0f);
	//重力加速度
	const float kGravity				 = 0.5f;
	//アニメーションの名前
	const char* const kIdleAnimName		 = "Player|Idle";
	
	const char* const kWalkAnimName		 = "Player|Walk";

	const char* const kPunchAnimName	 = "Player|Punch";

	const char* const kPunchRushAnimName = "Player|Punchrush";

	const char* const kJumpAnimName		 = "Player|Jump";

	const char* const kDodgeAnimName	 = "Player|Dodge";

	//攻撃中のフレーム
	constexpr float kPunchAnimFrame		 = 10.0f;

	constexpr int kAnimBlendFrame		 = 10;

	constexpr float kAttackStartFrame	 = 10.0f;

	constexpr float kAttackEndFrame		 = 15.0f;

	//攻撃の一回目のダメージを与えるフレーム
	constexpr int kAttackDamageFrame1 = 5;
	//攻撃の二回目のダメージを与えるフレーム
	constexpr int kAttackDamageFrame2 = 10;
	//攻撃の三回目のダメージを与えるフレーム
	constexpr int kAttackDamageFrame3 = 17;
	//攻撃の四回目のダメージを与えるフレーム
	constexpr int kAttackDamageFrame4 = 25;

	//ジャスト回避の受付フレーム
	constexpr int kDodgeFrame = 30;
	//無敵時間
	constexpr int kInvincibleFrame = 30;

	//ジャスト回避の範囲
	constexpr float kJustDodgeRadius = 100.0f;
}

Player::Player() :
	m_pCamera(nullptr),
	m_jumpPower(0),
	m_forward(VGet(0.0f, 0.0f, 1.0f)),
	m_hakutoHandle(-1),
	m_isAttackHit(false),
	m_isNextAttack(false),
	m_attackPos(VGet(0.0f,0.0f,0.0f)),
	m_currentState(PlayerState::Idle),
	m_prevState(PlayerState::Idle),
	m_isDead(false),
	m_isHit(false),
	m_invincibleTime(0),
	m_dodgeFrame(0),
	m_rushHit{false, false, false, false},
	m_isWitchTime(false),
	m_uiHandle(-1)
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
	CollisionManager::Instance().Register(this);
	//初期状態
	m_currentState = PlayerState::Idle;
	m_prevState = PlayerState::Idle;

	m_pos = VGet(0.0f, 0.0f, 0.0f);

	m_hp = 100;
	m_jumpPower = 10;
	m_invincibleTime = 0;
	m_modelHandle = MV1LoadModel("data/Player.mv1");
	m_hakutoHandle = LoadGraph("data/kudonetta.png");
	m_uiHandle = LoadGraph("data/ui_maho.png");
	m_animation.Init(m_modelHandle,kIdleAnimName,true,0.5f);
	
	m_isGround = true;
	
}

void Player::Update()
{
	//HPがゼロになったら
	if (m_isDead)
	{
		return;
	}

	Vector3 forward = m_pCamera->GetForward();
	Vector3 right = m_pCamera->GetRight();

	auto& input = Input::Instance();

	// アニメーション更新
	m_animation.Update();
	if (m_invincibleTime > 0)
	{
		m_invincibleTime--;
	}

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

		if (input.IsTriggered("Dodge"))
		{
			TransitionTo(PlayerState::Dodge);
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
		//ジャスト回避の受付
		m_dodgeFrame++;

		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(PlayerState::Idle);
		}
		break;
	}

	//前の場所
	Vector3 oldPos = m_pos;
	

	//通常移動
	if (m_currentState == PlayerState::Idle || m_currentState == PlayerState::Walk || m_currentState == PlayerState::Jump || m_currentState == PlayerState::Dodge)
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
	//回避判定
	DodgeUpdate();
	
}

void Player::Draw()
{
	//HPがゼロになったら
	if (m_isDead)
	{
		return;
	}

	MV1DrawModel(m_modelHandle);

	DrawCapsule3D(m_pos.ToDxLibVector(),VGet(m_pos.x, m_pos.y + 100.0f, m_pos.z),30.0f,16,GetColor(0, 255, 0),GetColor(0, 255, 0),false);

	DrawBillboard3D(VGet(100.0f, 300.0f, 30.0f), 0.0f, 1.0f, 450.0f, 0.0f,m_hakutoHandle, true);

	DrawBillboard3D(VGet(-500.0f, 300.0f, 30.0f), 0.0f, 1.0f, 450.0f, 0.0f, m_uiHandle, true);

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
	
	// プレイヤーの状態によって、表示する半径と色を変える
	float drawRadius = 100.0f; // デフォルトの半径
	unsigned int drawColor = GetColor(255, 0, 0); // 通常は赤

	// 今ジャスト回避の受付時間か？
	if (m_currentState == PlayerState::Dodge && m_dodgeFrame <= kDodgeFrame)
	{
		//受付中だけ、判定の半径を広げ、色を青にする
		drawRadius = kJustDodgeRadius; // コンストで定義した 100.0f
		drawColor = GetColor(0, 100, 255); // 青
	}

	//実際に描画する
	DrawSphere3D(m_pos.ToDxLibVector(), drawRadius, 16, drawColor, drawColor, false);
	//HP
	DrawFormatString(
		50,
		70,
		GetColor(255, 255, 255),
		"PlayerHP:%d",
		m_hp
	);

	

}

void Player::ApplyDamage(int damage)
{
	if (m_isDead)
	{
		return;
	}

	// 無敵時間中なら、通常の被弾もジャスト回避もすべて無視する
	if (m_invincibleTime > 0)
	{
		return;
	}

	// CollisionManager側でジャスト回避の大きな判定半径100に当たった時に、
	// damage = 0 でこの関数が呼ばれる
	if (damage == 0 && m_currentState == PlayerState::Dodge && m_dodgeFrame <= kDodgeFrame)
	{
		// ジャスト回避成功時の処理
		printfDx("Just Dodge (Witch Time!)\n");

		// 直ぐに無敵を付与連続ヒット防止にもなる
		m_invincibleTime = kInvincibleFrame;

		// ここでウィッチタイム状態に入る処理
		m_isWitchTime = true;
		// 例1: プレイヤーのステートをウィッチタイム中に変える場合
		// TransitionTo(PlayerState::WitchTime); 

		// 例2: 専用のフラグや、ゲーム全体をスローにする関数を呼ぶ場合
		// TriggerWitchTime(); 

		return; // ダメージを受けずに処理を抜ける
	}

	// 通常の被ダメ処理上のジャスト回避に引っかからなかった場合のみここに来る
	m_hp -= damage;

	// 無敵時間を設定
	m_invincibleTime = 120;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_isDead = true;
		CollisionManager::Instance().Unregister(this);
		printfDx("PlayerDead!\n");
	}

	printfDx("Player HP = %d\n", m_hp);
}

Vector3 Player::GetCameraTarget() const
{
	return m_pos + kPlayerToTarget;
}

bool Player::IsJustDodgeWindow() const
{
	return (m_currentState == PlayerState::Dodge && m_dodgeFrame <= kDodgeFrame);
}

// 今ジャスト回避を受け付ける状態にいるかを返す
float Player::GetJustDodgeRadius() const
{
	if (IsJustDodgeWindow())
	{
		return kJustDodgeRadius; // 受付中なら広げる（100.0f）
	}
	return 30.0f; // 通常時の半径（GetCollisionRadius()の戻り値など）
}

void Player::AttackUpdate()
{
	if (m_currentState != PlayerState::Attack && m_currentState != PlayerState::Rush)
	{
		return;
	}

	float animTime = m_animation.GetCurrentAnimTime();

	//アニメーションのフレーム数を取得して、攻撃判定を出すタイミングを決める
	float rate = m_animation.GetAnimRate();

	//攻撃判定を出すフレームの範囲内にいるか
	if (animTime >= kAttackStartFrame &&
		animTime <= kAttackEndFrame)
	{
		if (!m_isAttackHit)
		{
			//攻撃判定を出す
			CollisionManager::Instance().CheckAttackSphere(this,m_attackPos,50.0f,m_attackPower);

			m_isAttackHit = true;
		}
	}
	if (m_currentState == PlayerState::Rush)
	{
		//ラッシュの攻撃は一回だけ当たるようにする
		if (!m_rushHit[0] && animTime >= kAttackDamageFrame1)
		{
			//攻撃判定を出す
			CollisionManager::Instance().CheckAttackSphere(this, m_attackPos, 50.0f, m_attackPower);
			//一回当たったら当たり判定を消す
			m_rushHit[0] = true;
		}
		//ラッシュの攻撃の二回目の攻撃判定
		if(!m_rushHit[1] && animTime >= kAttackDamageFrame2)
		{
			//攻撃判定を出す
			CollisionManager::Instance().CheckAttackSphere(this, m_attackPos, 50.0f, m_attackPower);
			//一回当たったら当たり判定を消す
			m_rushHit[1] = true;
		}
		//ラッシュの攻撃の三回目の攻撃判定
		if(!m_rushHit[2] && animTime >= kAttackDamageFrame3)
		{
			//攻撃判定を出す
			CollisionManager::Instance().CheckAttackSphere(this, m_attackPos, 50.0f, m_attackPower);
			//一回当たったら当たり判定を消す
			m_rushHit[2] = true;
		}
		//ラッシュの攻撃の四回目の攻撃判定
		if(!m_rushHit[3] && animTime >= kAttackDamageFrame4)
		{
			//攻撃判定を出す
			CollisionManager::Instance().CheckAttackSphere(this, m_attackPos, 50.0f, m_attackPower);
			//一回当たったら当たり判定を消す
			m_rushHit[3] = true;
		}
		
	}
	
}

void Player::DodgeUpdate()
{
	
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

		m_animation.ChangeAnim(kWalkAnimName,true,0.4f);
		break;

	case PlayerState::Attack:

		m_isAttackHit = false;
		m_isNextAttack = false;

		m_animation.ChangeAnim(kPunchAnimName,false,0.5f);
		break;
	case PlayerState::Rush:

		//ラッシュの攻撃は複数回当たる可能性があるので、当たったかどうかを管理する配列をリセットする
		for (int i = 0; i < 4; i++)
		{
			m_rushHit[i] = false;
		}

		m_animation.ChangeAnim(kPunchRushAnimName, false, 0.9f);
		break;

	case PlayerState::Jump:
		m_animation.ChangeAnim(kJumpAnimName, false, 0.38f);
		break;

	case PlayerState::Dodge:
		//フレームをリセット
		m_dodgeFrame = 0;
		m_animation.ChangeAnim(kDodgeAnimName, false, 0.85f);
		break;

	default:
		break;
	}
}

void Player::WitchTime()
{
	//ウィッチタイムに入ったら
	if (m_isWitchTime)
	{

	}
}
