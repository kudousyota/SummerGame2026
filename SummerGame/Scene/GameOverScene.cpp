#include "GameOverScene.h"
#include "SceneController.h"
#include "SceneMain.h"
#include "TitleScene.h"
#include "../Game.h"
namespace
{
	constexpr int kFadeInterval = 60;

}

GameOverScene::GameOverScene(SceneController& controller):
	Scene(controller),
	m_finished(false),
	m_fontHandle(-1)
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
	//フォントの読み込み
	m_fontHandle = CreateFontToHandle("x10y12pxDonguriDuel", 60, -1, DX_FONTTYPE_ANTIALIASING_EDGE);
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
	//フェードイン中でもエンター("ok")でフェードアウトを開始できるようにする
	if (input.IsTriggered("ok"))
	{
		m_update = &GameOverScene::FadeOutUpdate;
		m_draw = &GameOverScene::FadeDraw;
		m_frame = 0;	//フェードアウトの最初
		//SoundManager::PlaySE("Ok");
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


	if (input.IsTriggered("ok"))
	{
		m_update = &GameOverScene::FadeOutUpdate;
		m_draw = &GameOverScene::FadeDraw;
		m_frame = 0;	//フェードアウトの最初
		//SoundManager::PlaySE("Ok");
		return;

	}


}

void GameOverScene::FadeOutUpdate(Input&)
{
	if (m_frame++ >= kFadeInterval)
	{
		//フェードアウト完了
		m_finished = true;
		m_controller.ChangeScene(std::make_shared<SceneMain>(m_controller));
		return;
	}
}
void GameOverScene::NormalDraw()
{

	const int white = GetColor(255, 255, 255);
	const int Cyan = GetColor(0, 255, 255);
	const int Color = GetColor(224, 255, 255);
	const int black = GetColor(0, 0, 0);

	DrawStringToHandle(550, 50, "GameOver", white, m_fontHandle);

	//点滅頻度
	const int intervar = 650;
	int now = GetNowCount();
	bool visible = (now / intervar) % 2;
	if (visible)
	{
		//操作説明表示
		DrawStringToHandle(470, 580, "Press A to Retry", white, m_fontHandle);
	}
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