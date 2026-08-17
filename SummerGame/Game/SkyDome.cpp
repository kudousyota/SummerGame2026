#include "SkyDome.h"
#include "../System/Model.h"


namespace
{
	constexpr float kscale = 2.5f;
}

SkyDome::SkyDome():
	m_skyDomeHandle(-1),
	m_pos(0.0f,0.0f,0.0f)
{
}

SkyDome::~SkyDome()
{
	MV1DeleteModel(m_skyDomeHandle);
}

void SkyDome::Init()
{
	m_skyDomeHandle = Model::Instance().CreatSkyDomeModel();

	//èÍèä
	MV1SetPosition(m_skyDomeHandle, m_pos);
	//ÉTÉCÉY
	MV1SetScale(m_skyDomeHandle, Vector3(kscale, kscale, kscale));
}

void SkyDome::Update(const Vector3& cameraPos)
{
	m_pos = cameraPos;
	//èÍèä
	MV1SetPosition(m_skyDomeHandle, m_pos);
}

void SkyDome::Draw()
{
	//ï`âÊ
	MV1DrawModel(m_skyDomeHandle);
}

void SkyDome::SetPos()
{
	m_pos = Vector3(0.0f, 0.0f, -3000.0f);
}
