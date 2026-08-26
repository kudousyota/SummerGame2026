#include "Alien.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "Player.h"
#include "../System/Timer.h"
#include "../System/Model.h"
#include "../Game/Breath.h"
#include "../System/ProjectileManager.h"
#include "../Effect/EffectManager.h"
#include "../Effect/Effect.h"
#include "EffekseerForDXLib.h"
#include "../System/SoundManager.h"

namespace
{

	const char* const kIdleAnimName = "Alien|Idle";
	const char* const kMoveAnimName = "Alien|Move";
	const char* const kStandUpAnimName = "Alien|StandUp";
	const char* const kUPAnimName = "Alien|UP";
	const char* const kDeathAnimName = "Alien|Death";
	const char* const kDownAnimName = "Alien|Down";
	const char* const kAttackAnimName = "Alien|Attack";
	const char* const kLookAnimName = "Alien|Look";
	const char* const kDeadAnimName = "Alien|Dead";

	//ブレス攻撃するために口元付近のリグを取る
	const char* const kAttackRig = "mixamorig1:Head";
	constexpr float kBreathRadius = 80.0f;

	//このフレーム数でブレスを発射する
	constexpr float kBreathFrame = 45.0f;

	//スコア
	constexpr int kScore = 300;
	
}

Alien::Alien():
	m_currentState(AlienState::Idle),
	m_prevState(AlienState::Idle),
	m_modelDisplayOffsetY(0.0f),
	m_headBone(-1),
	m_pBreath(nullptr),
	m_breathEffectHandle(-1),
	m_floatingEffectHandle(-1),
	m_floatingEffectPos({0.0f,0.0f,0.0f}),
	m_lastSeePos({0.0f,0.0f,0.0f})
{
}

Alien::~Alien()
{
	if (m_floatingEffectHandle != -1)
	{
		EffectManager::Instns().StopEffect(m_floatingEffectHandle);
		m_floatingEffectHandle = -1;
	}

	if (m_breathEffectHandle != -1)
	{
		EffectManager::Instns().StopEffect(m_breathEffectHandle);
		m_breathEffectHandle = -1;
	}

	if (m_pBreath)
	{
		m_pBreath->Kill();
		m_pBreath = nullptr;
	}
}

void Alien::Init()
{
	//基底クラスの初期化
	Enemy::Init();
	//HP
	m_hp = 400;
	//攻撃力
	m_attackPower = 10;
	//現在のステート
	m_currentState = AlienState::Idle;
	m_prevState = AlienState::Idle;
	//重力
	m_gravity = 0.0f;
	//モデルのスケール
	m_scale = Vector3(1.0f, 1.0f, 1.0f);

	//当たり判定
	m_collisionRadius = 50.0f;
	m_collisionHeight = 160.0f;

	//視界
	m_sightRange = 400.0f;
	m_fov = 120.0f;
	//サーチ
	m_baseSightRange = m_sightRange;
	m_searchSightMultiplier = 1.5f;	//キョロキョロ中は1.5倍見える
	m_lookMaxTime = 180.0f;			//60fps換算で3秒
	m_lookSwingSpeed = 0.05f;
	m_lookSwingAngle = 60.0f * DX_PI_F / 180.0f; //左右60度ずつ振る
	m_lookTimer = 0.0f;

	m_modelHandle = Model::Instance().CreatAlienModel();
	m_animation.Init(m_modelHandle, kIdleAnimName, true, 0.5f);

	//モデルを読み込んでボーンを見つけてくる
	m_headBone = MV1SearchFrame(m_modelHandle, kAttackRig);
	//浮遊エフェクトの再生
	m_floatingEffectHandle = EffectManager::Instns().PlayEffect(EffectType::Floating, m_pos);

	//スコア
	m_score = kScore;
}

void Alien::Update()
{
	
	//死んだとき
	if (m_isDead)
	{
		//死亡アニメーションが終わったら消える
		if (m_floatingEffectHandle != -1)
		{
			EffectManager::Instns().StopEffect(m_floatingEffectHandle);
			m_floatingEffectHandle = -1;
		}
		return;
	}
	//当たり判定
	Character::Collision();
	//タイムスケールの取得
	m_timeScale = Timer::Instance().GetEnemyTimeScale();
	//アニメーションの更新
	m_animation.Update(m_timeScale);

	if (m_currentState == AlienState::Dead)
	{
		if (m_animation.GetAnimEndFlag())
		{
			//アニメーションが終わったら完全に死亡扱い
			m_isDead = true;
		}
		return;
	}

	//攻撃表示タイマー
	if (m_attackFrame > 0)
	{
		m_attackFrame--;
	}

	//攻撃クールタイム//攻撃のクールタイムもウィッチタイムで遅くする
	UpdateCooldown(m_timeScale);
	//浮遊エフェクトの位置をエイリアンの位置に合わせる
	m_floatingEffectPos = m_pos;
	if (m_floatingEffectHandle != -1)
	{
		//浮遊エフェクトの位置を更新
		SetPosPlayingEffekseer3DEffect(m_floatingEffectHandle, m_floatingEffectPos.x, m_floatingEffectPos.y, m_floatingEffectPos.z);
	}

	//ステート
	switch (m_currentState)
	{
	case AlienState::Idle:
		//プレイヤーを見つけたら追いかける
		if (CanSeePlayer())
		{
			TransitionTo(AlienState::Move);
			//プレイヤーが最後にいた場所を記憶
			m_lastSeePos = m_pPlayer->GetPosition();
		}
		//クールタイム
		if (m_attackCooldown > 0)
		{
			break;
		}
		
		break;
	case AlienState::Move:
	{
		//視界から外れたら
		if (!CanSeePlayer())
		{
			//プレイヤーが最後にいた場所を記憶
			m_lastSeePos = m_pPlayer->GetPosition();
			TransitionTo(AlienState::Search);
			break;
		}
		//プレイヤーまでのベクトル
		Vector3 dir = m_pPlayer->GetPosition() - m_pos;
		dir.y = 0.0f;
		float distsq = dir.SqMagnitude();

		//攻撃範囲に入ったら攻撃
		if (distsq <= kAttackRange * kAttackRange)
		{
			//攻撃
			if (m_attackCooldown <= 0)
			{
				//プレイヤーの方向に向く
				FacePlayer();
				TransitionTo(AlienState::Attack);
			}
		}
		else
		{
			//プレイヤーの方向に少しづつ向きを合わせる
			ChasePlayer(0.15f, m_timeScale);
		}
	}
		break;
	case AlienState::Attack:
		//攻撃処理
		AttackUpdate();

		//攻撃終了後は待機状態へ戻る
		if (m_animation.GetAnimEndFlag())
		{
			if (m_pBreath)
			{
				m_pBreath->Kill();
				m_pBreath = nullptr;
			}

			if (m_breathEffectHandle != -1)
			{
				EffectManager::Instns().StopEffect(m_breathEffectHandle);
				m_breathEffectHandle = -1;
			}
			TransitionTo(AlienState::Idle);
		}
		break;
	case AlienState::Down:
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(AlienState::StandUp);
		}
		break;

	case AlienState::StandUp:
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(AlienState::Up);
		}
		break;

	case AlienState::Up:
		//上昇
		m_pos.y += m_velocity.y;
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(AlienState::Idle);
			//アニメーションが終わると止まる
			m_velocity.y = 0.0f;
		}
		
		break;
	case AlienState::Hit:

		//攻撃中に被弾したら攻撃を消す
		if (m_pBreath)
		{
			m_pBreath->Kill();
			m_pBreath = nullptr;
		}

		if (m_breathEffectHandle != -1)
		{
			EffectManager::Instns().StopEffect(m_breathEffectHandle);
			m_breathEffectHandle = -1;
		}
		
		//ダメージアニメーションが終わったらIdleに戻る
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(AlienState::Idle);
		}
		break;
	case AlienState::Search:
	{
		//プレイヤーを見つけたら追いかける
		if (CanSeePlayer())
		{
			TransitionTo(AlienState::Move);
			break;
		}

		Vector3 dir = m_lastSeePos - m_pos;
		dir.y = 0.0f;

		if (dir.SqMagnitude() < 30.0f * 30.0f)
		{
			TransitionTo(AlienState::LookAround);
		}
		else
		{
			MoveTo(m_lastSeePos, 0.15f, m_timeScale);
		}
	}
		break;

	case AlienState::LookAround:
		//プレイヤーを見つけたら
		if (CanSeePlayer())
		{
			TransitionTo(AlienState::Move);
			break;
		}

		m_lookTimer += m_timeScale;
		{
		//最後にプレイヤーがいた方向を索敵する
		float baseAngle = atan2f(m_lastSeePos.x - m_pos.x, m_lastSeePos.z - m_pos.z);
		float swing = sinf(m_lookTimer * m_lookSwingSpeed) * m_lookSwingAngle;
		float angle = baseAngle + swing;

		m_forward = Vector3(sinf(angle), 0.0f, cosf(angle));
		m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;
		}
		//一定時間見つからなければ諦める
		if (m_lookTimer >= m_lookMaxTime)
		{
			TransitionTo(AlienState::Idle);
		}
		break;
	}
	//モデル更新行列
	UpdateModelMatrix();
}

void Alien::Draw()
{
	//HPがゼロになったら
	if (m_isDead)
	{
		return;
	}

	MV1DrawModel(m_modelHandle);
#ifdef _DEBUG
	//デバッグ描画
	DrawDebugCollision();

	//HP表示
	DrawFormatString(300, 50, GetColor(255, 255, 255), "AlienHP:%d", m_hp);

	//視界のデバッグ
	DrawDebugSight();
#endif
}

void Alien::OnHit(const AttackData& attackdata)
{

	ApplyDamage(attackdata.GetDamage());

	//死亡していたら、これ以上ステートを変更しない
	if (m_isDead || m_currentState == AlienState::Dead)
	{
		return;
	}

	//攻撃されたらプレイヤーの方を向く
	FacePlayer();

	if (attackdata.GetAttackType() == AttackType::SkyKick)
	{
		TransitionTo(AlienState::Down);
		return;
	}
	else
	{
		TransitionTo(AlienState::Hit);
	}
}

void Alien::ChasePlayer(float rotateSpeed, float scale)
{
	Vector3 dir = m_pPlayer->GetPosition() - m_pos;
	//エイリアンは高さ気にしない
	dir.y = 0.0f;
	Vector3 targetDir = dir.Normalize();

	m_forward += (targetDir - m_forward) * rotateSpeed;
	m_forward = m_forward.Normalize();

	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;

	m_pos += m_forward * m_speed * scale;
}

void Alien::KickDown()
{
	TransitionTo(AlienState::Down);
}

void Alien::AttackUpdate()
{
	//現在のアニメーションを取得
	float animFrame = m_animation.GetCurrentAnimTime();
	
	//頭のボーンの位置
	VECTOR headPos = MV1GetFramePosition(m_modelHandle, m_headBone);

	Vector3 pos = Vector3(headPos);
	//ボーンの行列から前の方向を取得
	MATRIX headMat = MV1GetFrameLocalWorldMatrix(m_modelHandle, m_headBone);
	//後ろ向きに伸びたのでーにして前側にした
	VECTOR forward =
	{
		-headMat.m[2][0],
		-headMat.m[2][1],
		-headMat.m[2][2]
	};
	//正規化
	forward = VNorm(forward);
	//指定したフレーム数でブレスを生成
	if (!m_isAttack && animFrame >= kBreathFrame)
	{
		m_pBreath = static_cast<Breath*>(ProjectileManager::Instance().Add(std::make_unique<Breath>(pos, forward, 10.0f, CreateAttackData())));

		m_breathEffectHandle = EffectManager::Instns().PlayEffect(EffectType::Breath,Vector3(headPos));
		SoundManager::Instance().PlaySE("Breath");

		//発射位置と向きを正規化
		m_pBreath->SetPos(Vector3(pos));
		m_pBreath->SetForward(Vector3(forward));
		//いっぱい生成されるのを防ぐ
		m_isAttack = true;
	}
	//ブレスがある間は頭のボーンから生成される
	if (m_pBreath)
	{
		m_pBreath->SetPos(Vector3(headPos));
		m_pBreath->SetForward(Vector3(forward));
		
	}
	SetPosPlayingEffekseer3DEffect(m_breathEffectHandle, headPos.x, headPos.y, headPos.z);

	//頭ボーンの向きを使う
	float effectYaw = atan2f(forward.x, forward.z) + DX_PI_F;

	//上下の傾きも計算する
	float horizontalLen = sqrtf(forward.x * forward.x + forward.z * forward.z);
	float effectPitch = atan2f(-forward.y, horizontalLen);

	SetRotationPlayingEffekseer3DEffect(m_breathEffectHandle, effectPitch, effectYaw, 0.0f);
}


Vector3 Alien::GetCollisionPosition() const
{
	return m_pos + Vector3(0.0f, m_modelDisplayOffsetY, 0.0f);
}

void Alien::TransitionTo(AlienState nextState)
{
	//同じステートなら何もしない
	if (m_currentState == nextState)
	{
		return;
	}
	//ステートの更新
	m_prevState = m_currentState;
	m_currentState = nextState;
	switch (m_currentState)
	{
	case AlienState::Idle:
		m_animation.ChangeAnim(kIdleAnimName, true, 0.5f);
		//広げた視界を戻す
		m_sightRange = m_baseSightRange;
		break;
	case AlienState::Move:
		m_animation.ChangeAnim(kMoveAnimName, true, 0.5f);
		m_sightRange = m_baseSightRange;
		break;
	case AlienState::Attack:
		m_animation.ChangeAnim(kAttackAnimName, false, 0.5);
		m_isAttack = false;
		m_attackCooldown = 90;
		m_attackDir = m_forward;
	
		break;
	case AlienState::Down:
		m_animation.ChangeAnim(kDownAnimName, false, 0.5f);
		//スカイキックを食らったら落ちる
		m_gravity = 0.5f;
		m_velocity.y = 0.0f;
		break;
	case AlienState::StandUp:
		m_animation.ChangeAnim(kStandUpAnimName, false, 0.5f);
		//立ち上がったら戻す
		m_gravity = 0.0f;
		m_velocity.y = 0.0f;
		break;
	case AlienState::Up:
		m_animation.ChangeAnim(kUPAnimName, false, 0.5f);
		m_velocity.y = 0.3f;
		break;
	case AlienState::Hit:
		//ヒットアニメーションがないので今は適当にほかのモーションを渡す
		m_animation.ChangeAnim(kDownAnimName, false, 0.8f);
		break;
	case AlienState::Search:
		//着くまではMoveのモーション
		m_animation.ChangeAnim(kMoveAnimName, true, 0.5f);
		break;
	case AlienState::LookAround:
		//到着後はLookのモーション
		m_animation.ChangeAnim(kLookAnimName, true, 0.5f);
		m_lookTimer = 0.0f;
		m_sightRange = m_baseSightRange * m_searchSightMultiplier;
		break;
	case AlienState::Dead:
		m_animation.ChangeAnim(kDeadAnimName, false, 0.5f);
		break;
	}
}

Vector3 Alien::GetHitEffect() const
{
	return m_pos + Vector3(0.0f,GetCollisionHeight(),0.0f);
}

AttackType Alien::GetAttackType() const
{
	return AttackType::Breath;
}

float Alien::GetAttackRadius() const
{
	return kBreathRadius;
}

void Alien::OnDead()
{
	//死亡時に攻撃中だった場合、ブレスの弾とエフェクトを後片付けする
	if (m_pBreath)
	{
		m_pBreath->Kill();
		m_pBreath = nullptr;
	}

	if (m_breathEffectHandle != -1)
	{
		EffectManager::Instns().StopEffect(m_breathEffectHandle);
		m_breathEffectHandle = -1;
	}
	//浮遊エフェクトもここで止める
	if (m_floatingEffectHandle != -1)
	{
		EffectManager::Instns().StopEffect(m_floatingEffectHandle);
		m_floatingEffectHandle = -1;
	}
	TransitionTo(AlienState::Dead);
}
