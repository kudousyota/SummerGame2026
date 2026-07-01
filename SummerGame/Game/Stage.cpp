#include "Stage.h"
#include "DxLib.h"
#include <cassert>

Stage::Stage():
	m_modelHandle(-1),
	m_stagePos(VGet(0.0f, 0.0f, 0.0f))
{
}

Stage::~Stage()
{
	MV1DeleteModel(m_modelHandle);
}

void Stage::Init()
{
	m_modelHandle = MV1LoadModel("Data/Stage.mv1");
	assert(m_modelHandle != -1);

	MV1SetPosition(m_modelHandle, m_stagePos.ToDxLibVector());
}

void Stage::Draw()
{
	MV1DrawModel(m_modelHandle);
}
