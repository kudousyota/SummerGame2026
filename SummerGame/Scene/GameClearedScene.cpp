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
//#include "../system/SoundManager.h"


namespace 
{
	constexpr int kFadeInterval = 60;
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
	m_playerPos(Vector3(0.0f, 0.0f, 0.0f)),
	m_currentAnimCount(0.0f),
	m_cureentAnimHandle(-1),
	m_currentAnimIndex(-1),
	m_enemyHandle(-1),
	m_enemyPos(Vector3(0.0f, 0.0f, 0.0f)),
	m_enemyAngle(0.0f),
	m_effectPos(Vector3(0,0,0)),
	m_groundHandle(-1)
{
	m_update = &GameClearedScene::FadeInUpdate;
	m_draw = &GameClearedScene::FadeDraw;
	m_frame = kFadeInterval;
}
GameClearedScene::~GameClearedScene()
{
    // フォントやモデルハンドルが残っていれば解放する
	if (m_fontHandle != -1)
	{
		DeleteFontToHandle(m_fontHandle);
		m_fontHandle = -1;
	}

	if (m_skyHandle != -1)
	{
		MV1DeleteModel(m_skyHandle);
		m_skyHandle = -1;
	}

	if (m_playerHandle != -1)
	{
		MV1DeleteModel(m_playerHandle);
		m_playerHandle = -1;
	}

	if (m_enemyHandle != -1)
	{
		MV1DeleteModel(m_enemyHandle);
		m_enemyHandle = -1;
	}

	if (m_groundHandle != -1)
	{
		MV1DeleteModel(m_groundHandle);
		m_groundHandle = -1;
	}
}

void GameClearedScene::Init()
{
	//残っているエフェクトの停止
	EffectManager::Instns().StopAll();

	////フォントの読み込み
	m_fontHandle = CreateFontToHandle("Constantia", 60, -1, DX_FONTTYPE_ANTIALIASING_EDGE);

	//最終スコアを取得
	m_finalScore = Score::Instance().GetTotalScore();
	
	//UI
	m_pUiManager = std::make_unique<UIManager>();
	//リザルトUI
	auto gameClearedUI = std::make_unique<GameClearedUI>();
	//hpUI->SetPlayerHP(m_pPlayer);
	m_pUiManager->Add(std::move(gameClearedUI));

	m_pUiManager->Init();

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
	//スコアアップ演出
	if (m_displayScore < m_finalScore)
	{
		m_scoreAnimTime++;
		//徐々に近づける
		int diff = m_finalScore - m_displayScore;
		int step = diff / 10;
		if (step < 1)
		{
			step = 1;
		}

		m_displayScore += step;

		//行き過ぎないように補正
		if (m_displayScore > m_finalScore)
		{
			m_displayScore = m_finalScore;
		}
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

	//DrawStringToHandle(550, 50, "Result", white, m_fontHandle);
	m_pUiManager->Draw();
	//点滅頻度
	const int intervar = 650;
	int now = GetNowCount();
	bool visible = (now / intervar) % 2;
	if (visible)
	{
		//操作説明表示
		DrawStringToHandle(470, 580, "Press A to Title", white, m_fontHandle);
	}
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