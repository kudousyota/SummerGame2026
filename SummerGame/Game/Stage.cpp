#include "Stage.h"
#include "DxLib.h"

Stage::Stage():
	m_modelHandle(-1)
{
}

Stage::~Stage()
{
	MV1DeleteModel(m_modelHandle);
}

void Stage::Init()
{
	m_modelHandle = MV1LoadModel("Data/Ground.mv1");
}

void Stage::Draw()
{
	MV1DrawModel(m_modelHandle);
}
