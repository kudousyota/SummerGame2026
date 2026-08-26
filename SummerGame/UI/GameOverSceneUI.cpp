#include "GameOverSceneUI.h"
#include "DxLib.h"
#include "../Common/FontManager.h"
#include "../Game.h"

namespace
{
	constexpr int kUpperY = 260;
	constexpr int kLowerY = 320;
	//動く速さ
	constexpr int kSpeed = 2;
}

GameOverSceneUI::GameOverSceneUI() :
	m_logoHandle(-1),
	m_logoposX(0),
	m_logoposY(0),
	m_isUp(false)
{
}

GameOverSceneUI::~GameOverSceneUI()
{
	DeleteGraph(m_logoHandle);
}

void GameOverSceneUI::Init()
{
	m_logoHandle = LoadGraph("Data/Continue.png");

	m_logoposY = kUpperY;
	//m_isUp = false;
}

void GameOverSceneUI::Update(Input& input)
{
	if (m_isUp)
	{
		m_logoposY += kSpeed;
	}
	else
	{
		m_logoposY -= kSpeed;
	}
	//上限にいったら下に行くようにする
	if (m_logoposY <= kUpperY)
	{
		m_logoposY = kUpperY;
		m_isUp = true;
	}
	//下限にいったら下に行くようにする
	else if (m_logoposY >= kLowerY)
	{
		m_logoposY = kLowerY;
		m_isUp = false;
	}
}

void GameOverSceneUI::Draw()
{
	const int white = GetColor(255, 255, 255);
	const int Cyan = GetColor(0, 255, 255);
	const int Color = GetColor(224, 255, 255);
	const int black = GetColor(0, 0, 0);

	//点滅頻度
	const int intervar = 650;
	int now = GetNowCount();
	bool visible = (now / intervar) % 2;
	if (visible)
	{
		//操作説明表示
		FontManager::Instance().DrawCenteredText(Game::kScreenWidth / 2, 580, "Press A to Retry", white, 32, black);
	}

	////大きめのGameOver表示
	//const int goColor = GetColor(255, 80, 80);
	//FontManager::Instance().DrawCenteredExtendText(Game::kScreenWidth / 2, 140, "Game Over", goColor, 80, black, 1.0f);
	//ロゴの描画
	DrawRotaGraph(620, m_logoposY, 0.8f, 0, m_logoHandle, true);

}
