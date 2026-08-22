#include <algorithm>
#include <cmath>
#include "GameClearedScene.h"
#include "DxLib.h"
#include "../System/Input.h"
#include "TitleScene.h"
#include "SceneController.h"
#include "../Game.h"
#include "../Game/Character/Player.h"
#include "../Game/Camera.h"
#include "../system/Vector3.h"
#include "../UI/UIManager.h"
#include "../UI/GameClearedUI.h"
#include "../System/Score.h"
#include "../Effect/EffectManager.h"
#include "../System/Model.h"
#include "../Character/CharacterViewer.h"
#include "../System/SoundManager.h"


namespace 
{
	constexpr int kFadeInterval = 60;
}

GameClearedScene::GameClearedScene(SceneController& controller) :
	m_draw(0),
	m_fontHandle(-1),
	m_finished(false),
	Scene(controller),
	m_pos(Vector3(0.0f,0.0f,0.0f)),
	m_effectPos(Vector3(0.0f,0.0f,0.0f)),
	m_modelHandle(-1)
{
	m_update = &GameClearedScene::FadeInUpdate;
	m_draw = &GameClearedScene::FadeDraw;
	m_frame = kFadeInterval;
}
GameClearedScene::~GameClearedScene()
{
}

void GameClearedScene::Init()
{
	//残っているエフェクトの停止
	EffectManager::Instns().StopAll();
	//モデル
	m_modelHandle = Model::Instance().CreatClearedModel();

	m_pos = Vector3(0.0f,0.0f,300.0f);
	MV1SetPosition(m_modelHandle, m_pos);
	SoundManager::Instance().PlayBGM("Cleare",true);

	//UI
	m_pUiManager = std::make_unique<UIManager>();
	//リザルトUI
	auto gameClearedUI = std::make_unique<GameClearedUI>();
	//hpUI->SetPlayerHP(m_pPlayer);
	m_pUiManager->Add(std::move(gameClearedUI));

	m_pUiManager->Init();

    //カメラを初期化
	////カメラをタイトル用に初期位置に戻す（注視点とカメラ位置が同じだと正しく描画されない）
	////ステージ全体が見えるようにカメラを少し上・手前に配置する
	//SetCameraPositionAndTarget_UpVecY(Vector3(0.0f, 200.0f, -800.0f), Vector3(0.0f, 100.0f, 0.0f));
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

void GameClearedScene::FadeInUpdate(Input& input)
{
	//フェードイン中でもエンター("ok")でフェードアウトを開始できるようにする
	if (input.IsTriggered("ok"))
	{
		m_update = &GameClearedScene::FadeOutUpdate;
		m_draw = &GameClearedScene::FadeDraw;
		m_frame = 0;//フェードアウトの最初
        SoundManager::Instance().PlaySE("Ok");
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
        SoundManager::Instance().PlaySE("Ok");
		return;

	}
	m_pUiManager->Update();
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

	const int white = GetColor(255, 255, 255);
	const int Cyan = GetColor(0, 255, 255);
	const int Color = GetColor(224, 255, 255);
	const int black = GetColor(0, 0, 0);
	MV1DrawModel(m_modelHandle);
	//DrawStringToHandle(550, 50, "Result", white, m_fontHandle);
	m_pUiManager->Draw();

}

void GameClearedScene::FadeDraw()
{
	NormalDraw();
	//値の範囲を一旦0.0~1.0fにしておくといろいろと扱いやすい
	auto rate = static_cast<float>(m_frame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}