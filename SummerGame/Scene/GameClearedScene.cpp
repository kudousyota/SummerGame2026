#include <algorithm>
#include <cmath>
#include "GameClearedScene.h"
#include "DxLib.h"
#include "../System/Input.h"
#include "TitleScene.h"
#include "SceneController.h"
#include "../Game.h"
#include "../Game/Player.h"
#include "../Game/Camera.h"
#include "../system/Vector3.h"
//#include "../system/SoundManager.h"


namespace 
{
	constexpr int kFadeInterval = 60;

}


void GameClearedScene::FadeInUpdate(Input& input)
{
	//フェードイン中でもエンター("ok")でフェードアウトを開始できるようにする
	if (input.IsTriggered("ok"))
	{
		m_update = &GameClearedScene::FadeOutUpdate;
		m_draw = &GameClearedScene::FadeDraw;
		m_frame = 0;	//フェードアウトの最初
		//SoundManager::PlaySE("Ok");
		return;
	
	}


	if (m_frame-- <= 0)
	{
		m_update = &GameClearedScene::NormalUpdate;
		m_draw = &GameClearedScene::NormalDraw;
		return;
	}
}



void GameClearedScene::NormalUpdate(Input& input)
{

	
	if (input.IsTriggered("ok"))
	{
		m_update = &GameClearedScene::FadeOutUpdate;
		m_draw = &GameClearedScene::FadeDraw;
		m_frame = 0;	//フェードアウトの最初
		//SoundManager::PlaySE("Ok");
		return;
		
	}

	
}

void GameClearedScene::FadeOutUpdate(Input&)
{
	if (m_frame++ >= kFadeInterval)
	{
		//フェードアウト完了
		m_finished = true;
		m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
		return;
	}
}
void GameClearedScene::NormalDraw()
{
 //   MV1DrawModel(m_skyHandle);
	////モデルの位置を反映してから描画
	//MV1SetPosition(m_playerHandle, m_playerPos.ToDxLibVector());
	//MV1DrawModel(m_playerHandle);
	////モデルの位置を反映してから描画
	//MV1SetPosition(m_enemyHandle, m_enemyPos.ToDxLibVector());
	//MV1DrawModel(m_enemyHandle);
	////モデルの位置を反映してから描画
	//MV1SetPosition(m_groundHandle, m_groundPos.ToDxLibVector());
	//MV1DrawModel(m_groundHandle);

	
	const int white = GetColor(255,255,255);
	const int Cyan	= GetColor(0,255,255);
	const int Color = GetColor(224,255,255);
	const int black = GetColor(0,	0,	0);

	DrawStringToHandle(550, 50, "Result", white, m_fontHandle);


	DrawFormatString2ToHandle(540, 300, white, black, m_fontHandle, "Dodge:%d", m_finalScore);
	//点滅頻度
	const int intervar = 650;
	int now = GetNowCount();
	bool visible = (now / intervar) % 2;
	if (visible)
	{
		//操作説明表示
		DrawStringToHandle(470, 580, "Press B to Title", white, m_fontHandle);
	}
}

void GameClearedScene::FadeDraw()
{
	NormalDraw();
	//値の範囲を一旦0.0~1.0fにしておくといろいろと扱いやすい
	auto rate = static_cast<float>(m_frame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * rate);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}


GameClearedScene::GameClearedScene(SceneController& controller) :
	m_draw(0),
	m_fontHandle(-1),
	m_displayScore(0),
	m_finalScore(0),
	m_scoreAnimTime(0),
	m_finished(false),
	Scene(controller),
	m_skyHandle(-1),
	m_playerHandle(-1),
	m_playerPos(VGet(0.0f, 0.0f, 0.0f)),
	m_currentAnimCount(0.0f),
	m_cureentAnimHandle(-1),
	m_currentAnimIndex(-1),
	m_enemyHandle(-1),
	m_enemyPos(VGet(0.0f, 0.0f, 0.0f)),
	m_enemyAngle(0.0f),
	m_effectPos(VGet(0,0,0)),
	m_groundHandle(-1)
{
	m_update = &GameClearedScene::FadeInUpdate;
	m_draw = &GameClearedScene::FadeDraw;
	m_frame = kFadeInterval;
}
GameClearedScene::~GameClearedScene()
{
	//何もしない
}

void GameClearedScene::Init()
{
	//フォントの読み込み
	m_fontHandle	= CreateFontToHandle("x10y12pxDonguriDuel", 60, -1, DX_FONTTYPE_ANTIALIASING_EDGE);

	//m_skyHandle		= MV1LoadModel("data/Sky_Daylight01.mv1");

	//m_playerHandle	= MV1LoadModel("data/Player.mv1");

	//m_enemyHandle	= MV1LoadModel("data/Enemy.mv1");

	//m_groundHandle	= MV1LoadModel("data/Ground.mv1");	

	//SoundManager::PlayBGM("Result",true);

	//m_playerPos		= VGet(0.0f, 0.0f, -10.0f);

	//m_enemyPos		= VGet(0.0f, 0.0f, -400.0f);

	//m_groundPos		= VGet(0.0f, -100.0f, 150.0f);

	//int animNo = MV1GetAnimIndex(m_playerHandle, kShootAnimName);
	//m_currentAnimCount = 0.0f;

	//m_cureentAnimHandle = MV1AttachAnim(m_playerHandle, animNo, -1, -1);
	//m_currentAnimIndex = animNo;
	//
	//m_enemyAngle += DX_PI_F;

	////モデル行列を毎フレーム更新して回転と位置を反映する
	//MATRIX rotMtx = MGetRotY(m_enemyAngle);
	//MATRIX transMtx = MGetTranslate(m_enemyPos.ToDxLibVector());
	//MATRIX world = MMult(rotMtx, transMtx);
	//MV1SetMatrix(m_enemyHandle, world);

	//m_effectManger.Init();

	//m_effectPos = m_enemyPos;

	////カメラをリザルト用に初期化
	////これでメインシーンから引き継いだカメラの位置がリセットされる
	//SetCameraPositionAndTarget_UpVecY(VGet(500.0f, 300.0f, -350.0f), VGet(0.0f, 0.0f, 0.0f));
	//SetupCamera_Perspective(DX_PI_F / 3.0f);
	//SetCameraNearFar(20.0f, 4500.0f);
}

void GameClearedScene::Update(Input& input)
{
	(this->*m_update)(input);

	if (m_finished) return; 

	//0m_effectManger.Update();
}
void GameClearedScene::Draw()
{
	
    (this->*m_draw)();
	//m_effectManger.Draw();
}
