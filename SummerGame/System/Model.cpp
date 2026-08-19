#include "Model.h"
#include <DxLib.h>
//ベースモデルを一括でロードする
//起動時に一度だけ呼び出し、以後は Create*Model で複製を作成して利用する
void Model::PreloadAll()
{
	m_creatureBaseHandle = MV1LoadModel("Data/Enemy.mv1");
	m_angelBaseHandle = MV1LoadModel("Data/Angel.mv1");
	m_alienBaseHandle = MV1LoadModel("Data/Alien.mv1");
	m_playerBaseHandle = MV1LoadModel("Data/Player.mv1");
	m_witchTimeHandBaseHandle = MV1LoadModel("Data/WitchTimeHand.mv1");
	m_skyDomeBaseHandle = MV1LoadModel("Data/Sky_Night01.mv1");
}

//ロードしたベースモデルを解放する
//アプリ終了時に呼び出す
void Model::ReleseAll()
{
	if (m_angelBaseHandle != -1) MV1DeleteModel(m_angelBaseHandle);
	if (m_creatureBaseHandle != -1) MV1DeleteModel(m_creatureBaseHandle);
	if (m_alienBaseHandle != -1) MV1DeleteModel(m_alienBaseHandle);
	if (m_playerBaseHandle != -1) MV1DeleteModel(m_playerBaseHandle);
	if (m_witchTimeHandBaseHandle != -1) MV1DeleteModel(m_witchTimeHandBaseHandle);
	if (m_skyDomeBaseHandle != -1)MV1DeleteModel(m_skyDomeBaseHandle);
}
