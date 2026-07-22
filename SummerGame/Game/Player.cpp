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
	const float kGravity				 = 0.6f;
	//アニメーションの名前
	const char* const kIdleAnimName		 = "Player|Idle";
	
	const char* const kWalkAnimName		 = "Player|Walk";

	const char* const kPunchAnimName	 = "Player|Punch";

	const char* const kPunchRushAnimName = "Player|Punchrush";

	const char* const kJumpAnimName		 = "Player|Jump";

	const char* const kDodgeAnimName	 = "Player|Dodge";

	const char* const kKickAnimName		 = "Player|Kick";

	const char* const kSkyAnimName		 = "Player|Sky";

	const char* const kSkyKickAnimName	 = "Player|kakatootosi_TEST";

	const char* const kRunAnimName		 = "Player|Run";

	const char* const kHitAnimName 		 = "Player|Hit";

	//攻撃中のフレーム
	constexpr float kPunchAnimFrame		 = 10.0f;

	constexpr int   kAnimBlendFrame		 = 10;
	//最初の攻撃の判定フレーム
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
	//ラッシュ攻撃回数
	constexpr int kRushAttackCount = 4;
	//ダメージを出すフレーム
	constexpr int kAttackDamageFrame[kRushAttackCount] =
	{
		kAttackDamageFrame1,
		kAttackDamageFrame2,
		kAttackDamageFrame3,
		kAttackDamageFrame4
	};

	//キックのダメージを与えれるフレーム
	constexpr float kKickStartFrame = 15.0f;

	constexpr float kKickEndFrame = 25.0f;

	//キックのダメージ
	constexpr int kKickPower = 50;

	//プレイヤーの前方のどこに攻撃を出すか
	constexpr float kAttackOffset = 70.0f;
	
	//ジャスト回避の受付フレーム
	constexpr int kDodgeFrame = 30;
	//無敵時間
	constexpr int kInvincibleFrame = 30;

	//ジャスト回避の範囲
	constexpr float kJustDodgeRadius = 100.0f;

	//攻撃した時に進む距離
	constexpr float kAttackMove = 35.0f;
	constexpr float kRushMove = 4.0f;
	constexpr float kKickMove = 25.0f;

	constexpr float kSkyAttackMove = 10.0f;
	constexpr float kSkyRushMove = 3.0f;
	constexpr float kSkyKickMove = 15.0f;
	//落ちるスピード
	constexpr float kSkyKickFallSpeed = -8.0f;
	constexpr float kSkyKickRadius = 140.0f;
	//攻撃の時に徐々に回転
	constexpr float kRotateSpeed = 0.03f;
}

Player::Player() :
	m_pCamera(nullptr),
	m_jumpPower(0),
	m_forward(VGet(0.0f, 0.0f, 1.0f)),
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
	m_attackForward(VGet(0.0f,0.0f,0.0f)),
	m_moveVelocity(VGet(0.0f, 0.0f, 0.0f)),
	m_attackVelocity(VGet(0.0f, 0.0f, 0.0f)),
	m_modelDisplayOffsetY(0.0f),
	m_lastGroundPos(VGet(0.0f, 0.0f, 0.0f)),
	m_maxHp(0)
{
	
}

Player::~Player()
{
	CollisionManager::Instance().Unregister(this);

	MV1DeleteModel(m_modelHandle);	
}

CharacterType Player::GetCharacterType() const
{
	return CharacterType::Player;
}

void Player::Init()
{
	Character::Init();
	CollisionManager::Instance().Register(this);
	//初期状態
	m_currentState = PlayerState::Idle;
	m_prevState = PlayerState::Idle;

	m_pos = VGet(0.0f, 500.0f, 0.0f);
	//移動速度
	m_speed = 13.0f;
	//ステータス
	m_maxHp = 300;
	m_hp = 300;
	m_jumpPower = 15;
	//攻撃力
	m_attackPower = 20;
	m_invincibleTime = 0;
	m_modelHandle = MV1LoadModel("Data/Player.mv1");

	m_animation.Init(m_modelHandle,kIdleAnimName,true,0.5f);
	
	m_isGround = true;
	
}

void Player::Update()
{

	auto& input = Input::Instance();
	//HPがゼロになったら
	if (m_isDead)
	{
#ifdef _DEBUG
		if (input.IsTriggered("pause"))
		{
			m_isDead = false;

		}
#endif // _DEBUG


		return;
	}

	//プレイヤーがいた場所を保存
	if(m_isGround)
	{
		m_lastGroundPos = m_pos;
	}

	//地面より下に行ったらリスポーンさせる
	if(m_pos.y < -250.0f)
	{
		m_lastGroundPos.y += 10.0f;
		m_pos = m_lastGroundPos;
		m_hp -= 10;
	}

	Vector3 forward = m_pCamera->GetForward();
	Vector3 right = m_pCamera->GetRight();

	

	float scale = Timer::Instance().GetTimeScale();

	//アニメーション更新
	m_animation.Update(scale);
	if (m_invincibleTime > 0)
	{
		m_invincibleTime--;
	}

	m_lockOnManager.Update();

	//GetGraphSize(m_hpGaugeHandle, &m_hpX, &m_hpY);


	switch (m_currentState)
	{
	case PlayerState::Idle:

		if (input.IsTriggered("Attack"))
		{
			TransitionTo(PlayerState::Attack);
		}
		//攻撃方向を入力方向に変える
		TurnToInputDirection(right, forward);
		if (input.IsPressed("up")	||
			input.IsPressed("down") ||
			input.IsPressed("left") ||
			input.IsPressed("right"))
		{
			TransitionTo(PlayerState::Run);
		}

		if (input.IsTriggered("Jump")&& m_isGround)
		{
			TransitionTo(PlayerState::Jump);

			//上にジャンプパワーを入れる
			m_velocity.y = static_cast<float>(m_jumpPower);

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

	case PlayerState::Run:

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
			m_velocity.y = static_cast<float>(m_jumpPower);

			//ジャンプしたら空中ってことにする
			m_isGround = false;

			//地面判定から1ピクセルだけ浮かす
			//m_pos.y += 1.0f;
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
		//回避できる
		if (input.IsTriggered("Dodge"))
		{
			TransitionTo(PlayerState::Dodge);
		}
		//攻撃アニメ終了
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

		//コンボ受け付け
		if (input.IsTriggered("Attack"))
		{
			m_isNextAttack = true;
		}
		if (input.IsTriggered("Dodge"))
		{
			TransitionTo(PlayerState::Dodge);
		}
		if (m_animation.GetAnimEndFlag())
		{
			if (m_isNextAttack)
			{
				TransitionTo(PlayerState::Kick);
				m_isNextAttack = false;
			}
			else
			{
				TransitionTo(PlayerState::Idle);
			}
		}
		break;
	

	case PlayerState::Kick:

		//コンボ受け付け
		if (input.IsTriggered("Attack"))
		{
			m_isNextAttack = true;
		
		}
		if (input.IsTriggered("Dodge"))
		{
			TransitionTo(PlayerState::Dodge);
		}
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(PlayerState::Idle);
			m_attackPower = 20;
		}
		break;

	case PlayerState::Jump:

		if (!m_isGround)
		{
			TransitionTo(PlayerState::Sky);
		}
		//着地したら
		if (m_isGround)
		{
			//移動入力があるか
			bool isMove =
				input.IsPressed("up") ||
				input.IsPressed("down") ||
				input.IsPressed("left") ||
				input.IsPressed("right");

			// 移動優先
			if (isMove)
			{
				TransitionTo(PlayerState::Run);
			}
			else
			{
				TransitionTo(PlayerState::Idle);
			}
		}
		break;

	case PlayerState::Damage:
		
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(PlayerState::Idle);
		}
		break;
	case PlayerState::Sky:
		//着地したとき
		if (m_isGround)
		{
			//移動入力があるか
			bool isMove =
				input.IsPressed("up") ||
				input.IsPressed("down") ||
				input.IsPressed("left") ||
				input.IsPressed("right");

			// 移動優先
			if (isMove)
			{
				TransitionTo(PlayerState::Run);
			}
			else
			{
				TransitionTo(PlayerState::Idle);
			}
			break;
		}

		if (input.IsTriggered("Attack"))
		{
			TransitionTo(PlayerState::SkyAttack);
		}
		if (input.IsTriggered("Dodge"))
		{
			TransitionTo(PlayerState::SkyDodge);
		}
		break;
	case PlayerState::SkyAttack:
		//コンボ受け付け
		if (input.IsTriggered("Attack"))
		{
			m_isNextAttack = true;
		}
		if (m_animation.GetAnimEndFlag())
		{
			if (m_isNextAttack)
			{
				TransitionTo(PlayerState::SkyRush);
				m_isNextAttack = false;
			}
			else
			{
				TransitionTo(PlayerState::Sky);
			}
		}
		if (input.IsTriggered("Dodge"))
		{
			TransitionTo(PlayerState::SkyDodge);
		}
		break;

	case PlayerState::SkyRush:
		//コンボ受け付け
		if (input.IsTriggered("Attack"))
		{
			m_isNextAttack = true;
			
		}

		if (m_animation.GetAnimEndFlag())
		{
			if (m_isNextAttack)
			{
				TransitionTo(PlayerState::SkyKick);
				m_isNextAttack = false;
			}
			else
			{
				TransitionTo(PlayerState::Sky);
			}
		}
		if (input.IsTriggered("Dodge"))
		{
			TransitionTo(PlayerState::Dodge);
		}
		break;

	case PlayerState::SkyKick:

		//コンボ受け付け
		if (input.IsTriggered("Attack"))
		{
			m_isNextAttack = true;
		}
		if (input.IsTriggered("Dodge"))
		{
			TransitionTo(PlayerState::Dodge);
		}
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(PlayerState::Idle);
			m_attackPower = 20;
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
	case PlayerState::SkyDodge:

		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(PlayerState::Sky);
		}
		break;
	}


	//通常移動
	if (m_currentState == PlayerState::Idle || m_currentState == PlayerState::Run || m_currentState == PlayerState::Jump /*|| m_currentState == PlayerState::Dodge*/ || m_currentState == PlayerState::Sky)
	{

		float stickX = input.GetStickLX();
		float stickY = input.GetStickLY();
		//カメラ基準の移動ベクトル
		Vector3 moveVec = right * stickX + forward * stickY;
		//水平成分のみにする
		moveVec.y = 0.0f;

		float length = moveVec.Magnitude();

		if (length > 0.0001f)
		{
			Vector3 moveDir = moveVec.Normalize();

			if (length > 1.0f)
			{
				length = 1.0f;
			}
			
			//移動
			m_moveVelocity.x = moveDir.x * m_speed * length;
			m_moveVelocity.z = moveDir.z * m_speed * length;

			float rotSpeed = 0.2f;

			float targetAngle = atan2f(moveDir.x, moveDir.z) + DX_PI_F;

			float diff = targetAngle - m_angle;
			//角度の差を-π～πの範囲に収める
			while (diff > DX_PI_F)
			{
				diff -= DX_TWO_PI_F;
			}
			//角度の差を-π～πの範囲に収める
			while (diff < -DX_PI_F)
			{
				diff += DX_TWO_PI_F;
			}

			m_angle += diff * 0.2f;

			//角度から向きを生成
			m_forward.x = sinf(m_angle - DX_PI_F);
			m_forward.y = 0.0f;
			m_forward.z = cosf(m_angle - DX_PI_F);
		}
		else
		{
			m_moveVelocity.x = 0.0f;
			m_moveVelocity.z = 0.0f;
		}
	}

	//攻撃の時も向きをかえるようにする
	if (m_currentState == PlayerState::Attack ||
		m_currentState == PlayerState::Rush ||
		m_currentState == PlayerState::Kick ||
		m_currentState == PlayerState::SkyAttack ||
		m_currentState == PlayerState::SkyRush ||
		m_currentState == PlayerState::SkyKick)
	{
		TurnToInputDirection(right, forward);
	}

	//攻撃位置は垂直成分を除いた前方で決める
	m_attackForward = m_forward;
	m_attackForward.y = 0.0f;
	if (m_attackForward.SqMagnitude() > 0.0001f)
	{
		m_attackForward = m_attackForward.Normalize();
	}
	else
	{
		m_attackForward = VGet(0.0f, 0.0f, 1.0f); //フォールバック
	}

	m_attackVelocity *= 0.8f;

	m_pos += m_moveVelocity + m_attackVelocity;

	Character::Collision();
	//ここで攻撃位置を更新
	m_attackPos = m_pos + m_attackForward * kAttackOffset + VGet(0.0f, GetCollisionHeight() * 0.5f, 0.0f);

	//攻撃判定
	AttackUpdate();

	//モデル行列更新
	MATRIX rot = MGetRotY(m_angle);
	Vector3 drawPos = m_pos;
	MATRIX trans = MGetTranslate(drawPos.ToDxLibVector());
	MV1SetMatrix(m_modelHandle, MMult(rot, trans));

	m_isWitchTime = Timer::Instance().IsEnemySlow();

}

void Player::Draw()
{
	//HPがゼロになったら
	if (m_isDead)
	{
		return;
	}

	MV1DrawModel(m_modelHandle);
	
#ifdef _DEBUG

	Vector3 debugPos = GetCollisionPosition();

	VECTOR start = VGet(
		debugPos.x,
		debugPos.y + GetCollisionRadius(),
		debugPos.z);

	VECTOR end = VGet(
		debugPos.x,
		debugPos.y + GetCollisionHeight() - GetCollisionRadius(),
		debugPos.z);

	DrawCapsule3D(start,end,GetCollisionRadius(),16,0xffffff,0xffffff,false);


	//プレイヤーの状態によって、表示する半径と色を変える
	float drawRadius = 100.0f; //デフォルトの半径
	unsigned int drawColor = GetColor(255, 0, 0); //通常は赤

	//今ジャスト回避の受付時間か？
	if (m_currentState == PlayerState::Dodge && m_dodgeFrame <= kDodgeFrame)
	{
		//受付中だけ,色を青にする
		drawRadius = kJustDodgeRadius;//ジャスト回避の受付半径
		drawColor = GetColor(0, 100, 255);
	}

	//プレイヤーのジャスト回避判定を描画
	DrawSphere3D(VGet(m_pos.x, m_pos.y + GetCollisionHeight() * 0.5f, m_pos.z), drawRadius, 16, drawColor, drawColor, false);
	
	//当たり判定の描画
	if (m_currentState == PlayerState::Attack ||
		m_currentState == PlayerState::Rush ||
		m_currentState == PlayerState::Kick ||
		m_currentState == PlayerState::SkyAttack ||
		m_currentState == PlayerState::SkyRush ||
		m_currentState == PlayerState::SkyKick)
	{
		AttackData data = CreateAttackData();

		DrawSphere3D(
			m_attackPos.ToDxLibVector(),
			data.GetRadius(),
			16,
			GetColor(0, 255, 0),
			GetColor(0, 255, 0),
			false);
	}


	//printfDx("CollisionHeight = %f\n", GetCollisionHeight());
	//HP
	DrawFormatString(300, 70, GetColor(255, 255, 255), "PlayerHP:%d", m_hp);

	DrawFormatString(300,100, GetColor(255, 255, 255),"PlayerPos = (%f, %f, %f)\n", m_pos.x, m_pos.y, m_pos.z);
	
#endif  //DEBUG

}

void Player::ApplyDamage(int damage)
{
	if (m_isDead)
	{
		return;
	}

	//無敵時間中なら、通常の被弾もジャスト回避もすべて無視する
	if (m_invincibleTime > 0)
	{
		return;
	}

	//CollisionManager側でジャスト回避の大きな判定半径100に当たった時に、
	//damage = 0 でこの関数が呼ばれる
	if (damage == 0 && m_currentState == PlayerState::Dodge && m_dodgeFrame <= kDodgeFrame)
	{
#ifdef DEBUG

		//ジャスト回避成功時の処理
		printfDx("Just Dodge (Witch Time!)\n");

#endif // DEBUG

		
		//直ぐに無敵を付与連続ヒット防止にもなる
		m_invincibleTime = kInvincibleFrame;

		//ここでウィッチタイム状態に入る処理
		m_isWitchTime = true;
		//例1: プレイヤーのステートをウィッチタイム中に変える場合
		//TransitionTo(PlayerState::WitchTime); 
		//敵のアニメーションを遅くする
		Timer::Instance().SetEnemyTimeScaleForFrames(0.2f, 300);

		return; // ダメージを受けずに処理を抜ける
	}

	//通常の被ダメ処理上のジャスト回避に引っかからなかった場合のみここに来る
	m_hp -= damage;
	TransitionTo(PlayerState::Damage);

	//無敵時間を設定
	m_invincibleTime = 120;

	
	if (m_hp <= 0)
	{
		m_hp = 0;
		m_isDead = true;
		CollisionManager::Instance().Unregister(this);
		//printfDx("PlayerDead!\n");
	}

	//printfDx("Player HP = %d\n", m_hp);
}

Vector3 Player::GetCameraTarget() const
{
	return m_pos + kPlayerToTarget;
}

bool Player::IsJustDodgeWindow() const
{
	return (m_currentState == PlayerState::Dodge && m_dodgeFrame <= kDodgeFrame);
}

//今ジャスト回避を受け付ける状態にいるかを返す
float Player::GetJustDodgeRadius() const
{
	if (IsJustDodgeWindow())
	{
		return kJustDodgeRadius;
	}
	return 30.0f; //通常時の半径(GetCollisionRadius()の戻り値など)
}

void Player::AttackUpdate()
{
	if (m_currentState != PlayerState::Attack && m_currentState != PlayerState::Rush && m_currentState != PlayerState::Kick && m_currentState != PlayerState::SkyAttack && m_currentState != PlayerState::SkyRush && m_currentState != PlayerState::SkyKick)
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
			TryAttackHit();
			m_isAttackHit = true;
		}
	}
	if (m_currentState == PlayerState::Rush || m_currentState ==PlayerState::SkyRush)
	{
		for (int i = 0; i < kRushAttackCount; i++)
		{
			if (!m_rushHit[i] && animTime >= kAttackDamageFrame[i])
			{
				TryAttackHit();
				m_rushHit[i] = true;
			}
		}
	}
	if (m_currentState == PlayerState::Kick)
	{

		if (!m_isAttackHit && animTime >= kKickStartFrame && animTime <= kKickEndFrame)
		{
			//攻撃判定を出す
			TryAttackHit();
			m_isAttackHit = true;
		}
	}
	
	//攻撃判定を出すフレームの範囲内にいるか
	if (m_currentState == PlayerState::SkyAttack)
	{
		if (!m_isAttackHit)
		{
			//攻撃判定を出す
			TryAttackHit();
			m_isAttackHit = true;
		}
	}
	
	if (m_currentState == PlayerState::SkyKick)
	{

		if (!m_isAttackHit && animTime >= kKickStartFrame && animTime <= kKickEndFrame)
		{
			//攻撃判定を出す
			TryAttackHit();
			m_isAttackHit = true;
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
		//重力を戻す
		m_gravity = kGravity;

		m_attackPower = 20;

		m_animation.ChangeAnim(kIdleAnimName,true,0.5f);
		break;

	/*case PlayerState::Walk:
		m_animation.ChangeAnim(kWalkAnimName,true,0.4f);
		break;*/

	case PlayerState::Run:
		m_animation.ChangeAnim(kRunAnimName, true, 0.4f);
		break;
	case PlayerState::Attack:

		m_isAttackHit = false;
		m_isNextAttack = false;
		//ここでmoveVelocityをリセット
		m_moveVelocity = (VGet(0.0f, 0.0f, 0.0f));
		//攻撃で進む
		MoveAttack(kAttackMove);

		m_animation.ChangeAnim(kPunchAnimName,false,0.5f);
		break;
	case PlayerState::Rush:

		//ラッシュの攻撃は複数回当たる可能性があるので、当たったかどうかを管理する配列をリセットする
		for (int i = 0; i < 4; i++)
		{
			m_rushHit[i] = false;
		}
		m_moveVelocity = (VGet(0.0f, 0.0f, 0.0f));
		MoveAttack(kRushMove);
		m_animation.ChangeAnim(kPunchRushAnimName, false, 0.9f);
		break;

	case PlayerState::Kick:
		m_isAttackHit = false;
		m_attackPower += kKickPower;
		m_moveVelocity = (VGet(0.0f, 0.0f, 0.0f));
		MoveAttack(kKickMove);
		m_animation.ChangeAnim(kKickAnimName, false, 0.5f);
		break;
	case PlayerState::Damage:

		m_moveVelocity = (VGet(0.0f, 0.0f, 0.0f));

		m_animation.ChangeAnim(kHitAnimName, false, 0.5f);
		break;

	case PlayerState::Sky:
		m_gravity = kGravity;
		
		m_animation.ChangeAnim(kSkyAnimName, true, 0.5f);
		break;
	case PlayerState::SkyAttack:
		
		//空中攻撃をしたときにY方向のパワー無くして重力も消す
		m_velocity.y = 0.0f;
		m_gravity = 0.0f;
		m_moveVelocity = (VGet(0.0f, 0.0f, 0.0f));
		MoveAttack(kSkyAttackMove);
		m_isAttackHit = false;
		m_isNextAttack = false;
		

		m_animation.ChangeAnim(kPunchAnimName, false, 0.5f);
		break;
	case PlayerState::SkyRush:
		//同様の処理
		m_velocity.y = 0.0f;
		m_gravity = 0.0f;

		//ラッシュの攻撃は複数回当たる可能性があるので、当たったかどうかを管理する配列をリセットする
		for (int i = 0; i < 4; i++)
		{
			m_rushHit[i] = false;
		}
		m_moveVelocity = (VGet(0.0f, 0.0f, 0.0f));
		MoveAttack(kSkyRushMove);
		m_animation.ChangeAnim(kPunchRushAnimName, false, 0.9f);
		break;

	case PlayerState::SkyKick:
		//ここも同じ
		m_velocity.y = kSkyKickFallSpeed;
		m_gravity = 0.0f;
		m_isAttackHit = false;
		m_attackPower += kKickPower;
		m_moveVelocity = (VGet(0.0f, 0.0f, 0.0f));
		MoveAttack(kSkyKickMove);

		m_animation.ChangeAnim(kSkyKickAnimName, false, 0.8f);
		break;
	case PlayerState::Jump:
		m_gravity = kGravity;
		m_animation.ChangeAnim(kJumpAnimName, false, 0.38f);
		break;

	case PlayerState::Dodge:
		//フレームをリセット
		m_dodgeFrame = 0;
		//慣性のために移動速度をリセット
		m_moveVelocity = (VGet(0.0f, 0.0f, 0.0f));
		m_animation.ChangeAnim(kDodgeAnimName, false, 1.35f);
		break;
	case PlayerState::SkyDodge:
		//フレームをリセット
		m_dodgeFrame = 0;
		//慣性のために移動速度をリセット
		m_moveVelocity = (VGet(0.0f, 0.0f, 0.0f));
		m_animation.ChangeAnim(kDodgeAnimName, false, 0.85f);
		break;
	default:
		break;
	}
}

AttackType Player::GetAttackType()const
{
	//攻撃タイプを保存
	switch (m_currentState)
	{
	case PlayerState::Attack:
	case PlayerState::SkyAttack:
		return AttackType::Punch;

	case PlayerState::Rush:
	case PlayerState::SkyRush:
		return AttackType::Rush;

	case PlayerState::Kick:
		return AttackType::Kick;

	case PlayerState::SkyKick:
		return AttackType::SkyKick;

	default:
		return AttackType::Punch;
	}
}

AttackData Player::CreateAttackData()
{
	//攻撃ごとに半径をかえれるようにする
	float radius = 0.0f;
	switch (GetAttackType())
	{
	case AttackType::Punch:
		radius = 80.0f;
		break;
	case AttackType::Rush:
		radius = 60.0f;
		break;
	case AttackType::Kick:
		radius = 90.0f;
		break;
	case AttackType::SkyKick:
		radius = 140.0f;
		break;

	}

	return AttackData(CharacterType::Player,GetAttackType(),m_attackPower,radius);
}

void Player::MoveAttack(float distance)
{
	Vector3 dir = m_forward;
	dir.y = 0.0f;

	if (dir.SqMagnitude() > 0.0001f)
	{
		dir = dir.Normalize();
	}

	m_attackVelocity = dir * distance;
}

void Player::TurnToInputDirection(const Vector3& right, const Vector3& forward)
{
	float stickX = Input::Instance().GetStickLX();
	float stickY = Input::Instance().GetStickLY();

	Vector3 moveVec = right * stickX + forward * stickY;
	moveVec.y = 0.0f;

	if (moveVec.SqMagnitude() <= 0.0001f)
	{
		return;
	}

	Vector3 moveDir = moveVec.Normalize();

	//目標角度
	float targetAngle = atan2f(moveDir.x, moveDir.z) + DX_PI_F;

	//現在との差
	float diff = targetAngle - m_angle;

	//最短角度に補正
	while (diff > DX_PI_F)
	{
		diff -= DX_TWO_PI_F;
	}
	while (diff < -DX_PI_F)
	{
		diff += DX_TWO_PI_F;
	}


	m_angle += diff * kRotateSpeed;

	//向きを更新
	m_forward.x = sinf(m_angle - DX_PI_F);
	m_forward.y = 0.0f;
	m_forward.z = cosf(m_angle - DX_PI_F);
}

Vector3 Player::GetCollisionPosition() const
{
	return m_pos + VGet(0.0f, m_modelDisplayOffsetY, 0.0f);
}

void Player::TryAttackHit()
{
	auto hitList = CollisionManager::Instance().CheckAttackSphere(CreateAttackData(), m_attackPos);

	if (!hitList.empty())
	{
		// 一番最初に当たったキャラクターをロックオン対象にする
		m_lockOnManager.OnAttackHit(hitList.front());
	}
}
