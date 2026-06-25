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

	const char* const kDancingAttackAnimName = "Anglel|DancingAttack";

	const char* const kRotateAnimName = "Angel|Rotate";
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
	m_attackDir(VGet(0.0f, 0.0f, 0.0f))
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

	m_pos = VGet(300.0f, 0.0f, 250.0f);

	m_modelHandle = MV1LoadModel("Data/Angel.mv1");
	m_animation.Init(m_modelHandle, kRunAnimName, true, 0.5f);

	CollisionManager::Instance().Register(this);
}

void Angel::Update()
{

	Character::Collision();

	float scale = Timer::Instance().GetEnemyTimeScale();

	m_animation.Update(scale);

	Timer::Instance().Update();

	switch (m_currentState)
	{
	case AngelState::Shout:

		if (m_animation.GetAnimEndFlag())
		{
			TransitionTo(AngelState::Run);
		}

		break;
	case AngelState::Run:
		Vector3 dir = m_pPlayer->GetPosition() - m_pos;
		float distSq = dir.SqMagnitude();
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

		break;
	}
	MATRIX rot = MGetRotY(m_angle);
	MATRIX trans = MGetTranslate(m_pos.ToDxLibVector());
	MV1SetMatrix(m_modelHandle, MMult(rot, trans));
}

void Angel::Draw()
{
	MV1DrawModel(m_modelHandle);
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
	case AngelState::Run:
		m_animation.ChangeAnim(kRunAnimName, true, 0.5f);
		break;
	}
}


