#include "Stage.h"
#include "DxLib.h"
#include <cassert>

Stage::Stage():
	m_modelHandle(-1),
	m_colliderHandle(-1),
	m_stagePos(Vector3(0.0f, 0.0f, 0.0f))
{
}

Stage::~Stage()
{
    if (m_modelHandle != -1)
    {
        MV1DeleteModel(m_modelHandle);
    }
	if (m_colliderHandle != -1)
	{
		MV1DeleteModel(m_colliderHandle);
	}
}

void Stage::Init()
{
	m_modelHandle = MV1LoadModel("Data/Stage.mv1");
	assert(m_modelHandle != -1);
	m_colliderHandle = MV1LoadModel("Data/GameCollider.mv1");
	assert(m_colliderHandle != -1);

	MV1SetPosition(m_modelHandle, m_stagePos.ToDxLibVector());
	MV1SetPosition(m_colliderHandle, m_stagePos.ToDxLibVector());
}

void Stage::Draw()
{
	MV1DrawModel(m_modelHandle);
#ifdef _DEBUG
	//í≤êÆóp
	//MV1DrawModel(m_colliderHandle);
#endif // _DEBUG

	
}
