#include "Alien.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "Player.h"
#include "../System/Timer.h"
#include "../System/Model.h"
#include "../Game/Breath.h"
#include "../System/ProjectileManager.h"


namespace
{
	const char* const kIdleAnimName = "Alien|Idle";
	const char* const kMoveAnimName = "Alien|Move";
	const char* const kStandUpAnimName = "Alien|StandUp";
	const char* const kUPAnimName = "Alien|UP";
	const char* const kDeathAnimName = "Alien|Death";
	const char* const kDownAnimName = "Alien|Down";
	const char* const kAttackAnimName = "Alien|Attack";

	//ブレス攻撃するために口元付近のリグを取る
	const char* const kAttackRig = "mixamorig1:Head";
	constexpr float kBreathRadius = 80.0f;

	//このフレーム数でブレスを発射する
	constexpr float kBreathFrame = 20.0f;

}

Alien::Alien():
	m_currentState(AlienState::Idle),
	m_prevState(AlienState::Idle),
	m_modelDisplayOffsetY(0.0f),
	m_headBone(-1),
	m_pBreath(nullptr)
{
}

Alien::~Alien()
{
}

void Alien::Init()
{
	//基底クラスの初期化
	Enemy::Init();
	//HP
	m_hp = 500;
	//攻撃力
	m_attackPower = 10;
	//現在のステート
	m_currentState = AlienState::Idle;
	m_prevState = AlienState::Idle;
	//重力
	m_gravity = 0.0f;
	//モデルのスケール
	m_scale = VGet(1.0f, 1.0f, 1.0f);

	m_collisionRadius = 50.0f;
	m_collisionHeight = 160.0f;

	m_modelHandle = Model::Instance().CreatAlienModel();
	m_animation.Init(m_modelHandle, kIdleAnimName, true, 0.5f);

	//モデルを読み込んでボーンを見つけてくる
	m_headBone = MV1SearchFrame(m_modelHandle, kAttackRig);

}

void Alien::Update()
{
	//死んだとき
	if (m_isDead)
	{
		return;
	}
	//当たり判定
	Character::Collision();
	//タイムスケールの取得
	m_timeScale = Timer::Instance().GetEnemyTimeScale();
	//アニメーションの更新
	m_animation.Update(m_timeScale);

	

	//攻撃表示タイマー
	if (m_attackFrame > 0)
	{
		m_attackFrame--;
	}

	//攻撃クールタイム//攻撃のクールタイムもウィッチタイムで遅くする
	UpdateCooldown(m_timeScale);

	//ステート
	switch (m_currentState)
	{
	case AlienState::Idle:
		//クールタイム
		if (m_attackCooldown > 0)
		{
			break;
		}
		//プレイヤーを見つけたら追いかける
		if (CanSeePlayer())
		{
			TransitionTo(AlienState::Move);
		}
		break;
	case AlienState::Move:
	{
		//視界から外れたら
		if (!CanSeePlayer())
		{
			TransitionTo(AlienState::Idle);
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

		AttackUpdate();

		//攻撃終了後は待機状態へ戻る
		if (m_animation.GetAnimEndFlag())
		{
			if(m_pBreath)
			{
				m_pBreath->Kill();
				m_pBreath = nullptr;
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
		}
		
		//ダメージアニメーションが終わったらIdleに戻る
		if (m_animation.GetAnimEndFlag())
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

	////攻撃判定表示6
	//if (m_attackFrame > 0)
	//{
	//	DrawSphere3D(m_attackPos.ToDxLibVector(), 50.0f, 16, GetColor(0, 255, 0), GetColor(0, 255, 0), false);
	//}

	//HP表示
	DrawFormatString(300, 50, GetColor(255, 255, 255), "AlienHP:%d", m_hp);

	//DrawLine3D(pos, pos + VScale(forward, 100.0f),GetColor(255, 0, 0));

	

	//視界のデバッグ
	DrawDebugSight();
#endif
}

void Alien::OnHit(const AttackData& attackdata)
{
	ApplyDamage(attackdata.GetDamage());

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

}

Vector3 Alien::GetCollisionPosition() const
{
	return m_pos + VGet(0.0f, m_modelDisplayOffsetY, 0.0f);
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
		break;
	case AlienState::Move:
		m_animation.ChangeAnim(kMoveAnimName, true, 0.5f);
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
	
	}
}

AttackType Alien::GetAttackType() const
{
	return AttackType::Breath;
}

float Alien::GetAttackRadius() const
{
	return kBreathRadius;
}