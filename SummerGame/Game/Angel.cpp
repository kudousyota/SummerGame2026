#include "Angel.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "Stage.h"
#include "Player.h"
#include "../System/Timer.h"

namespace
{
	const char* const kRunAnimName = "Angel|Run";
}

Angel::Angel():
	m_modelHandle(1)
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

	m_currentState = AngelState::Run;
	m_prevState = AngelState::Run;

	m_modelHandle = MV1LoadModel("Data/Angel.mv1");
}

void Angel::Update()
{
	switch (m_currentState)
	{
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
		m_pos += m_forward * m_speed /** scale*/;
	}

}

void Angel::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void Angel::TransitionTo(AngelState nextState)
{
	switch (m_currentState)
	{
	case AngelState::Run:
		m_animation.ChangeAnim(kRunAnimName, true, 0.5f);
		break;
	}
}


