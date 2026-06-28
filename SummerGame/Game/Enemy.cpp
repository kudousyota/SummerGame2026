#include "Enemy.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "Player.h"
#include "../System/Timer.h"

namespace
{
	const char* const kIdleAnimName = "Enemy|Idle";

	const char* const kWalkAnimName = "Enemy|Walk";
	//強攻撃
	const char* const kAttackAnimName = "Enemy|Attack";
	//弱攻撃
	const char* const kPunchAnimName = "Enemy|Punch";
	//攻撃する長さ
	constexpr float kAttackRange = 150.0f;

	//見える距離
	constexpr float kSightRange = 500.0f;
	//視野角
	constexpr float kFov = 90.0f;
}

Enemy::Enemy():
	m_modelHandle(-1),
	m_isDead(false),
	m_isAttackHit(false),
	m_attackCooldown(0),
	m_forward(VGet(0.0f, 0.0f, 1.0f)),
	m_isAttacking(false),
	m_attackFrame(0),
	m_currentState(EnemyState::Idle),
	m_prevState(EnemyState::Idle),
	m_isAttack(false),
	m_attackDir(VGet(0.0f,0.0f,0.0f))
{
}

Enemy::~Enemy()
{
	CollisionManager::Instance().Unregister(this);

	MV1DeleteModel(m_modelHandle);
}

void Enemy::Init()
{
	Character::Init();

	m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;

	//本物の体力
	//m_hp = 150;
	m_hp = 500;

	m_attackPower = 20;
	

	m_currentState = EnemyState::Idle;
	m_prevState = EnemyState::Idle;

	m_attackCooldown = 0;

	m_pos = VGet(0.0f, 0.0f, 250.0f);
	m_modelHandle = MV1LoadModel("Data/Enemy.mv1");
	m_animation.Init(m_modelHandle, kIdleAnimName, true, 0.5f);

	CollisionManager::Instance().Register(this);
}

void Enemy::Update()
{
	//死んだら
	if (m_hp == 0)
	{
		return;
	}
	//当たり判定の更新
	Character::Collision();

	//タイムスケールの取得
	float scale = Timer::Instance().GetEnemyTimeScale();
	//アニメーションの更新
	m_animation.Update(scale);
	
	Timer::Instance().Update();

	//攻撃表示タイマー
	if (m_attackFrame > 0)
	{
		m_attackFrame--;
	}
	//攻撃クールタイム
	if (m_attackCooldown > 0)
	{
		//攻撃のクールタイムもウィッチタイムで遅くする
		m_attackCooldown -= scale;
	}

	//ステート
	switch (m_currentState)
	{
	case EnemyState::Idle:
	{
		//クールタイム中は待つ
		if (m_attackCooldown > 0)
		{
			break;
		}
		//プレイヤーを見つけたら追いかけ始める
		if (CanSeePlayer())
		{
			TransitionTo(EnemyState::Walk);
		}
		
	}
		break;
	case EnemyState::Walk:
	{
		//視野角から消えたら待機にする
		if (!CanSeePlayer())
		{
			TransitionTo(EnemyState::Idle);
			break;
		}
		//プレイヤーのまでのベクトル
		Vector3 dir = m_pPlayer->GetPosition() - m_pos;
		float distsq = dir.SqMagnitude();

		//攻撃範囲に入ったら攻撃
		if (distsq <= kAttackRange * kAttackRange)
		{
			//攻撃
			if (m_attackCooldown <= 0)
			{
				//プレイヤーの方向に向く
				m_forward = (m_pPlayer->GetPosition() - m_pos).Normalize();
				m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;
				//70％で弱攻撃
				if (GetRand(99) < 70)
				{
					TransitionTo(EnemyState::Punch);
				}
				else
				{
					TransitionTo(EnemyState::Attack);
				}
			}
		}
		else
		{
			//プレイヤーの方向に少しづつ向きを合わせる
			float rotSpeed = 0.15f;
			Vector3 targetDir = dir.Normalize();
			//現在の向きから目標方向に少しずつ近づく
			m_forward = m_forward + (targetDir - m_forward) * rotSpeed;
			//正規化
			m_forward = m_forward.Normalize();
			//モデルの向きを更新
			m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;
			//速度を与えるウィッチタイムで遅くなるように
			m_pos += m_forward * m_speed * scale;
		}
	}
		break;

	case EnemyState::Attack:
		//アニメーションの半分で攻撃判定を出す
		if (!m_isAttack && m_animation.GetAnimRate() >= 0.5f)
		{
			AttackUpdate();
			m_isAttack = true;
			//攻撃する方向を保存
			m_attackDir = (m_pPlayer->GetPosition() - m_pos).Normalize();
		}
		//攻撃終了後は待機状態へ戻る
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(EnemyState::Idle);
		}
		break;

	case EnemyState::Punch:
		//アニメーションの半分で攻撃判定を発生
		if (!m_isAttack && m_animation.GetAnimRate() >= 0.5f)
		{
			AttackUpdate();
			m_isAttack = true;
		}
		//攻撃終了したらIdleに行く
		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(EnemyState::Idle);
		}
		break;
	}
	// モデル行列更新
	MATRIX rot = MGetRotY(m_angle);
	MATRIX trans = MGetTranslate(m_pos.ToDxLibVector());
	MV1SetMatrix(m_modelHandle, MMult(rot, trans));
	
}


void Enemy::Draw()
{

	//HPがゼロになったら
	if (m_isDead)
	{
		return;
	}

	MV1DrawModel(m_modelHandle);
#ifdef _DEBUG
	DrawCapsule3D(m_pos.ToDxLibVector(),VGet(m_pos.x, m_pos.y + 100.0f, m_pos.z),70.0f,16,GetColor(255, 0, 0),GetColor(255, 0, 0),false);


	//攻撃の時に判定を表示
	if (m_attackFrame > 0)
	{
		DrawSphere3D(m_attackPos.ToDxLibVector(),50.0f,16,GetColor(0, 255, 0),GetColor(0, 255, 0),false);
	}

	DrawFormatString(50,50,GetColor(255, 255, 255),"EnemyHP:%d",m_hp);


	//索敵範囲デバッグ描画
	int color = GetColor(255, 255, 0);//黄色

	//視野角の半分(ラジアン)
	float halfFov = kFov * 0.5f * DX_PI_F / 180.0f;
	//前方の角度
	float baseAngle = atan2f(m_forward.x, m_forward.z);
	//分割数
	int segments = 16;
	//線形の始点
	VECTOR prevPoint = VGet(
		m_pos.x + kSightRange * sinf(baseAngle - halfFov),
		m_pos.y + 50.0f,
		m_pos.z + kSightRange * cosf(baseAngle - halfFov)
	);
	//扇形の円弧を線分で描画
	for (int i = 1; i <= segments; i++)
	{
		float angle = baseAngle - halfFov + (halfFov * 2.0f) * (float)i / segments;
		VECTOR point = VGet(
			m_pos.x + kSightRange * sinf(angle),
			m_pos.y + 50.0f,
			m_pos.z + kSightRange * cosf(angle)
		);
		DrawLine3D(prevPoint, point, color);
		prevPoint = point;
	}

	//扇の両辺(敵から視野端への線)
	VECTOR center = VGet(m_pos.x, m_pos.y + 50.0f, m_pos.z);
	VECTOR leftEdge = VGet(
		m_pos.x + kSightRange * sinf(baseAngle - halfFov),
		m_pos.y + 50.0f,
		m_pos.z + kSightRange * cosf(baseAngle - halfFov)
	);
	VECTOR rightEdge = VGet(
		m_pos.x + kSightRange * sinf(baseAngle + halfFov),
		m_pos.y + 50.0f,
		m_pos.z + kSightRange * cosf(baseAngle + halfFov)
	);
	//扇形の境界線を描画
	DrawLine3D(center, leftEdge, color);
	DrawLine3D(center, rightEdge, color);

	//プレイヤーが視野内にいるとき色を変える
	//敵からプレイヤーへのベクトル
	Vector3 dir = (m_pPlayer->GetPosition() - m_pos);
	float dist = dir.SqMagnitude();
	if (dist <= kSightRange * kSightRange)
	{
		//内積の計算
		float dot = m_forward.Dot(dir.Normalize());
		//視野角の境界になる内積の値
		float halfFovCos = cosf(kFov * 0.5f * DX_PI_F / 180.0f);
		//視野の中ならプレイヤーまで赤い線を引く
		if (dot >= halfFovCos)
		{
			// 発見中は赤で上書き
			DrawLine3D(center, VGet(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y + 50.0f, m_pPlayer->GetPosition().z), GetColor(255, 0, 0));
		}
	}
#endif

}

void Enemy::ApplyDamage(int damage)
{
	if (m_isDead)
	{
		return;
	}

	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		//死んだ
		m_isDead = true;

		//当たり判定を消す
		CollisionManager::Instance().Unregister(this);

		printfDx("EnemyDead!\n");
	}

	printfDx("Enemy HP = %d\n",m_hp);
}

void Enemy::AttackUpdate()
{
	//前側に表示高さは微調整
	m_attackPos = m_pos + m_attackDir * 70.0f + VGet(0.0f, 20.0f, 0.0f);
	//攻撃判定を出す
	CollisionManager::Instance().CheckAttackSphere(this,m_attackPos,50.0f,m_attackPower);
	m_isAttacking = true;
	m_attackFrame = 30;
}

bool Enemy::CanSeePlayer()
{
	//プレイヤーのまでのベクトル
	Vector3 dir = m_pPlayer->GetPosition() - m_pos;
	float distSq = dir.SqMagnitude();

	//視認距離の外ならfalse
	if (distSq > kSightRange * kSightRange)
	{
		return false;
	}

	//視野角内かを内積で判定
	float dot = m_forward.Dot(dir.Normalize());
	float halfFovCos = cosf(kFov * 0.5f * DX_PI_F / 180.0f);

	return dot >= halfFovCos;
}

void Enemy::TransitionTo(EnemyState nextState)
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
	case EnemyState::Idle:
		//待機アニメーション
		m_animation.ChangeAnim(kIdleAnimName, true, 0.5f);
		break;
	case EnemyState::Walk:
		//歩きアニメーション
		m_animation.ChangeAnim(kWalkAnimName, true, 0.5f);
		break;
	case EnemyState::Attack:
		//強攻撃アニメーション
		m_animation.ChangeAnim(kAttackAnimName, false, 0.5f);
		//攻撃ステートになったら更新する
		m_isAttack = false;
		//クールタイム設定
		m_attackCooldown = 90;
		//攻撃方向を保存
		m_attackDir = m_forward;
		break;
	case EnemyState::Punch:
		//弱攻撃のアニメーション
		m_animation.ChangeAnim(kPunchAnimName, false, 0.3f);
		//攻撃判定をリセット
		m_isAttack = false;
		//クールタイム設定
		m_attackCooldown = 90;
		//攻撃方向を保存
		m_attackDir = m_forward;

		break;
	}
}
