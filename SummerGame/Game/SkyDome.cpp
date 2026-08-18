#include "SkyDome.h"
#include "../System/Model.h"


namespace
{
	constexpr float kscale = 2.5f;
}

SkyDome::SkyDome():
	m_skyDomeHandle(-1),
	m_pos(0.0f,0.0f,0.0f),
	m_scale(0.0f),
	m_angleY(0.0f),
	m_rotSpeed(0.0f)
{
}

SkyDome::~SkyDome()
{
	MV1DeleteModel(m_skyDomeHandle);
}

void SkyDome::Init()
{
	m_skyDomeHandle = Model::Instance().CreatSkyDomeModel();
	m_scale = kscale;
	//èÍèä
	MV1SetPosition(m_skyDomeHandle, m_pos);
	//ÉTÉCÉY
	MV1SetScale(m_skyDomeHandle, Vector3(m_scale, m_scale, m_scale));
}

void SkyDome::Update(const Vector3& cameraPos)
{
	m_pos = cameraPos;
	//èÍèä
	MV1SetPosition(m_skyDomeHandle, m_pos);
	//âÒì]
	m_angleY += m_rotSpeed;
	MV1SetRotationXYZ(m_skyDomeHandle, Vector3(0.0f, m_angleY, 0.0f).ToDxLibVector());
}

void SkyDome::Draw()
{
	//ï`âÊ
	MV1DrawModel(m_skyDomeHandle);
}

void SkyDome::SetPos(const Vector3& pos)
{
	m_pos = pos;
}

void SkyDome::SetScale(float scale)
{
	m_scale = scale;
	if(m_skyDomeHandle != -1)
	{
		MV1SetScale(m_skyDomeHandle, Vector3(m_scale, m_scale, m_scale));
	}
}

void SkyDome::SetRotSpeed(float speed)
{
	m_rotSpeed = speed;
}
