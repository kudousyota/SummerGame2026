#include "GameOverScene.h"
#include "SceneController.h"
#include "SceneMain.h"
#include "TitleScene.h"
#include "../Game.h"
#include "../System/SoundManager.h"
#include "../Effect/EffectManager.h"
#include "../System/Model.h"
#include "../Common/FontManager.h"
#include "../UI/GameOverSceneUI.h"
#include "../UI/UIManager.h"
namespace
{
	constexpr int kFadeInterval = 60;

}

// No-op patch to trigger file update
GameOverScene::GameOverScene(SceneController& controller):
	Scene(controller),
	m_finished(false),
	m_fontHandle(-1),
	m_effectPos({0.0f,0.0f,0.0f})
{
	m_update = &GameOverScene::FadeInUpdate;
	m_draw = &GameOverScene::FadeDraw;
	m_frame = kFadeInterval;
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Init()
{
	EffectManager::Instns().StopAll();

	EffectManager::Instns().Init();
	//タイトルと同様にキャラクターを表示する
	std::vector<CharacterInfo> infos =
	{
		{ Model::Instance().CreatPlayerModel(), "Player|GameOverDead", Vector3(0.0f, 0.0f, -500.0f),Vector3(1.0f,1.0f,1.0f)},
	};
	m_GameOverCharacter.resize(infos.size());
	for (size_t i = 0; i < infos.size(); i++)
	{
		m_GameOverCharacter[i].Init(infos[i].modelHandle, infos[i].animName, infos[i].pos, infos[i].scale);
	}

    //エフェクトの再生位置はプレイヤーと合わせておく
    if (!infos.empty())
    {
        m_effectPos = infos[0].pos;
    }

	//背景エフェクトは1回だけ再生
	EffectManager::Instns().PlayEffect(EffectType::Gameover, m_effectPos);

    //UIを作成して初期化
	m_pUiManager = std::make_unique<UIManager>();
	//リザルトUI
	auto gameOverUI = std::make_unique<GameOverSceneUI>();
	m_pUiManager->Add(std::move(gameOverUI));

	m_pUiManager->Init();

	SetCameraPositionAndTarget_UpVecY(Vector3(0.0f, 100.0f, -700.0f), Vector3(0.0f, 100.0f, 0.0f));
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	SetCameraNearFar(20.0f, 4500.0f);
}

void GameOverScene::Update(Input& input)
{
	(this->*m_update)(input);

	if (m_finished) return;
}

void GameOverScene::Draw()
{
	(this->*m_draw)();
}

void GameOverScene::FadeInUpdate(Input& input)
{
	EffectManager::Instns().Update();
	//フェードイン中でもエンター("ok")でフェードアウトを開始できるようにする
	if (input.IsTriggered("ok"))
	{
		m_update = &GameOverScene::FadeOutUpdate;
		m_draw = &GameOverScene::FadeDraw;
		m_frame = 0;	//フェードアウトの最初
        SoundManager::Instance().PlaySE("Ok");
		return;

	}


	if (m_frame-- <= 0)
	{
		m_update = &GameOverScene::NormalUpdate;
		m_draw = &GameOverScene::NormalDraw;
		return;
	}
}

void GameOverScene::NormalUpdate(Input& input)
{
	EffectManager::Instns().Update();

	if (input.IsTriggered("ok"))
	{
		m_update = &GameOverScene::FadeOutUpdate;
		m_draw = &GameOverScene::FadeDraw;
		m_frame = 0;	//フェードアウトの最初
        SoundManager::Instance().PlaySE("Ok");
		return;

	}
	//EffectManager::Instns().PlayEffect(EffectType::Gameover, m_effectPos);
	//描画するキャラクターの更新
	for (auto& charcter : m_GameOverCharacter)
	{
		charcter.Update();
	}

	m_pUiManager->Update(input);
}

void GameOverScene::FadeOutUpdate(Input&)
{
	if (m_frame++ >= kFadeInterval)
	{
		//フェードアウト完了
		m_finished = true;
		EffectManager::Instns().StopAll();
		m_controller.ChangeScene(std::make_shared<SceneMain>(m_controller));
		return;
	}
}
void GameOverScene::NormalDraw()
{

	EffectManager::Instns().Draw();

	//モデルの描画
	for (auto& charcter : m_GameOverCharacter)
	{
		charcter.Draw();
	}
	
	m_pUiManager->Draw();
}

void GameOverScene::FadeDraw()
{
	NormalDraw();
	//値の範囲を一旦0.0~1.0fにしておくといろいろと扱いやすい
	auto rate = static_cast<float>(m_frame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}