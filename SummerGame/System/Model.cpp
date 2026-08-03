#include "Model.h"
#include <DxLib.h>
void Model::PreloadAll()
{
	m_creatureBaseHandle = MV1LoadModel("Data/Enemy.mv1");
	m_angelBaseHandle = MV1LoadModel("Data/Angel.mv1");
	m_alienBaseHandle = MV1LoadModel("Data/Alien.mv1");
	m_playerBaseHandle = MV1LoadModel("Data/Player.mv1");
	m_witchTimeHandBaseHandle = MV1LoadModel("Data/WitchTimeHand.mv1");
}

void Model::ReleseAll()
{
    if (m_angelBaseHandle != -1) MV1DeleteModel(m_angelBaseHandle);
    if (m_creatureBaseHandle != -1) MV1DeleteModel(m_creatureBaseHandle);
    if (m_alienBaseHandle != -1) MV1DeleteModel(m_alienBaseHandle);
    if (m_playerBaseHandle != -1) MV1DeleteModel(m_playerBaseHandle);
    if (m_witchTimeHandBaseHandle != -1) MV1DeleteModel(m_witchTimeHandBaseHandle);
}