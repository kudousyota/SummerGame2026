#include "Model.h"
#include <DxLib.h>
void Model::PreloadAll()
{
	m_creatureBaseHandle = MV1LoadModel("Data/Enemy.mv1");
	m_angelBaseHandle = MV1LoadModel("Data/Angel.mv1");
}

void Model::ReleseAll()
{
	MV1DeleteModel(m_angelBaseHandle);
	MV1DeleteModel(m_creatureBaseHandle);
}
