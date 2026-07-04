#include "Angel.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "Player.h"
#include "../System/Timer.h"

namespace
{
	const char* const kShoutAnimName = "Angel|Shout";

	const char* const kRunAnimName = "Angel|Run";

	const char* const kDancingAttackAnimName = "Angel|DancingAttack";

	const char* const kRotateAnimName = "Angel|Rotate";

	constexpr int kDanicgAttackRadius = 180.0f;

	//ラッシュ攻撃回数
	constexpr int kDancingAttackCount = 8;

	//ダメージを出すフレーム
	constexpr int kAttackDamageFrame[kDancingAttackCount] =
	{
		20,
		40,
		60,
		80,
		100,
		120,
		140,
		160
	};

	//攻撃する長さ
	constexpr float kAttackRange = 150.0f;

	//見える距離
	constexpr float kSightRange = 500.0f;
	//視野角
	constexpr float kFov = 90.0f;
}

Angel::Angel():
	m_modelHandle(-1),
	m_isDead(false),
	m_isAttackHit(false),
	m_attackCooldown(0),
	m_forward(VGet(0.0f, 0.0f, 1.0f)),
	m_isAttacking(false),
	m_attackFrame(0),
	m_currentState(AngelState::Idle),
	m_prevState(AngelState::Idle),
	m_isAttack(false),
	m_attackDir(VGet(0.0f, 0.0f, 0.0f)),
	m_dancingAttackHit{false,false, false, false, false, false, false, false }
{
}

Angel::~Angel()
{
	CollisionManager::Instance().Unregister(this);

	MV1DeleteModel(m_modelHandle);
}

void Angel::Init()
{
	Character::Init();

	m_currentState = AngelState::Shout;
	m_prevState = AngelState::Shout;

	//m_pos = VGet(300.0f, 300.0f, 250.0f);

	m_hp = 300;
	m_attackPower = 20;

	m_modelHandle = MV1LoadModel("Data/Angel.mv1");
	m_animation.Init(m_modelHandle, kShoutAnimName, true, 0.5f);

	CollisionManager::Instance().Register(this);
}

void Angel::Update()
{
	if (m_isDead)
	{
		return;
	}

	Character::Collision();
	//敵用のタイムスケールを取得
	float scale = Timer::Instance().GetEnemyTimeScale();
	//アニメーションのフレーム
	float animTime = m_animation.GetCurrentAnimTime();

	//攻撃クールタイム
	if (m_attackCooldown > 0)
	{
		//攻撃のクールタイムもウィッチタイムで遅くする
		m_attackCooldown -= static_cast<int>(scale);
	}

	m_animation.Update(scale);

	//float animpos = MV1GetFramePosition(m_modelHandle,50);

	switch (m_currentState)
	{
	case AngelState::Shout:

		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(AngelState::Run);
		}

		break;

	case AngelState::Idle:

		if (CanSeePlayer())
		{
			TransitionTo(AngelState::Run);
		}
		break;
	case AngelState::Run:
	{
		//視野角から消えたら待機にする
		if (!CanSeePlayer())
		{
			TransitionTo(AngelState::Idle);
			break;
		}

		Vector3 dir = m_pPlayer->GetPosition() - m_pos;
		float distSq = dir.SqMagnitude();

		if (distSq <= kAttackRange * kAttackRange)
		{
			if (m_attackCooldown <= 0)
			{
				m_forward = (m_pPlayer->GetPosition() - m_pos).Normalize();
				m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;
				TransitionTo(AngelState::DancingAttack);
			}
		}
		else
		{
			//追跡
			float rotSpeed = 0.15f;
			Vector3 targetDir = dir.Normalize();
			//現在の向きから目標方向に少しずつ近づく
			m_forward = m_forward + (targetDir - m_forward) * rotSpeed;
			//正規化
			m_forward = m_forward.Normalize();

			m_angle = atan2f(m_forward.x, m_forward.z) + DX_PI_F;
			//速度を与えるウィッチタイムで遅くなるように
			m_pos += m_forward * m_speed * scale;
		}
		
	}
		break;
	case AngelState::DancingAttack:
		{
		
			for (int i = 0; i < kDancingAttackCount; i++)
			{
				if (!m_dancingAttackHit[i] && animTime >= kAttackDamageFrame[i])
				{
					CollisionManager::Instance().CheckAttackSphere(this, m_pos, kDanicgAttackRadius, m_attackPower);

					m_dancingAttackHit[i] = true;
				
				}
			}
			//アニメーションが終わったら追跡に戻る
			if (m_animation.GetAnimEndFlag())
			{
				TransitionTo(AngelState::Run);
			}
		
		}
		break;
	}
	// モデル行列更新
	MATRIX rot = MGetRotY(m_angle);
	Vector3 drawPos = m_pos;
	MATRIX trans = MGetTranslate(drawPos.ToDxLibVector());
	MV1SetMatrix(m_modelHandle, MMult(rot, trans));
}

void Angel::Draw()
{
	if (m_isDead)
	{
		return;
	}

	MV1DrawModel(m_modelHandle);
	
#ifdef _DEBUG

	DrawFormatString(300, 120, GetColor(255, 255, 255), "AngelHP:%d", m_hp);

	//当たり判定のデバッグ描画
	Vector3 debugPos = GetCollisionPosition();

	VECTOR start = VGet(
		debugPos.x,
		debugPos.y + GetCollisionRadius(),
		debugPos.z);

	VECTOR end = VGet(
		debugPos.x,
		debugPos.y + GetCollisionHeight() - GetCollisionRadius(),
		debugPos.z);

	DrawCapsule3D(start, end, GetCollisionRadius(), 16, 0xffffff, 0xffffff, false);
	if (m_currentState == AngelState::DancingAttack)
	{
		DrawSphere3D(m_pos.ToDxLibVector(), kDanicgAttackRadius, 6, 0x00ffff, 0x00ffff, false);
	}

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
		m_pos.y + 160.0f,
		m_pos.z + kSightRange * cosf(baseAngle - halfFov)
	);
	//扇形の円弧を線分で描画
	for (int i = 1; i <= segments; i++)
	{
		float angle = baseAngle - halfFov + (halfFov * 2.0f) * (float)i / segments;
		VECTOR point = VGet(
			m_pos.x + kSightRange * sinf(angle),
			m_pos.y + 160.0f,
			m_pos.z + kSightRange * cosf(angle)
		);
		DrawLine3D(prevPoint, point, color);
		prevPoint = point;
	}

	//扇の両辺(敵から視野端への線)
	VECTOR center = VGet(m_pos.x, m_pos.y + 160.0f, m_pos.z);
	VECTOR leftEdge = VGet(
		m_pos.x + kSightRange * sinf(baseAngle - halfFov),
		m_pos.y + 160.0f,
		m_pos.z + kSightRange * cosf(baseAngle - halfFov)
	);
	VECTOR rightEdge = VGet(
		m_pos.x + kSightRange * sinf(baseAngle + halfFov),
		m_pos.y + 160.0f,
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
			DrawLine3D(center, VGet(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y + 160.0f, m_pPlayer->GetPosition().z), GetColor(255, 0, 0));
		}
	}
#endif
}

void Angel::ApplyDamage(int damage)
{
	if (m_isDead)
	{
		return;
	}

	m_hp -= damage;

	if(m_hp <= 0)
	{
		m_hp = 0;
		m_isDead = true;
		CollisionManager::Instance().Unregister(this);
	}
}

void Angel::TransitionTo(AngelState nextState)
{
	if (m_currentState == nextState)
	{
		return;
	}

	m_prevState = m_currentState;
	m_currentState = nextState;

	switch (m_currentState)
	{
	case AngelState::Shout:
		m_animation.ChangeAnim(kShoutAnimName, false, 0.5);
		break;
	case AngelState::Idle:
		m_animation.ChangeAnim(kShoutAnimName, true, 0.5f);
		break;
	case AngelState::Run:
		m_animation.ChangeAnim(kRunAnimName, true, 0.5f);
		break;
	case AngelState::DancingAttack:
		m_attackCooldown = 90;

		//ラッシュの攻撃は複数回当たる可能性があるので、当たったかどうかを管理する配列をリセットする
		for (int i = 0; i < kDancingAttackCount; i++)
		{
			m_dancingAttackHit[i] = false;
		}

		m_animation.ChangeAnim(kDancingAttackAnimName, false, 0.5);
		break;
	}
}

bool Angel::CanSeePlayer()
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


CharacterType Angel::GetCharacterType() const
{
	return CharacterType::Ememy;
}

