#include "CharacterViewer.h"
#include "DxLib.h"

namespace
{
	const char* const kIdleAnimName = "Player|Idle";
}

CharacterViewer::CharacterViewer():
	m_modelHandle(-1),
	m_pos({0.0f,0.0f,0.0f}),
	m_animSpeed(0.0f)
{
}

CharacterViewer::~CharacterViewer()
{
	if (m_modelHandle != -1)
	{
		MV1DeleteModel(m_modelHandle);
	}
}

void CharacterViewer::Init(const int modelhandle, const std::string& animname)
{
	m_modelHandle = modelhandle;

	m_animation.Init(m_modelHandle, animname, true, 0.5f);
}

void CharacterViewer::Update()
{
	m_animation.Update(1.0f);
}

void CharacterViewer::Draw()
{ 
	MV1SetPosition(m_modelHandle, m_pos);
	MV1DrawModel(m_modelHandle);

	//DrawString(0, 0, "Draw", GetColor(255, 255, 255));
}
void CharacterViewer::ChangeAnimation(const std::string& name, bool loop)
{
	m_animation.ChangeAnim(name, loop);
}

