#include "GameOverSceneUI.h"
#include "DxLib.h"
#include "../Common/FontManager.h"
#include "../Game.h"
#include "../System/Input.h"
#include "../System/SoundManager.h"

namespace
{
	constexpr int kUpperY = 260;
	constexpr int kLowerY = 320;
	//動く速さ
	constexpr int kSpeed = 2;

	constexpr int kVlinkIntercal = 650;

	//振れ幅
	constexpr int kQuakeAmplitude = 20;
	//揺らす力
	constexpr int kQuakePower = 15;
}

GameOverSceneUI::GameOverSceneUI() :
	m_logoHandle(-1),
	m_logoposX(0),
	m_logoposY(0),
	m_menuSelect(MenuSelect::Retle),
	m_isDecided(false),
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

	//上下でカーソル移動
	if (input.IsTriggered("left") || input.IsTriggered("right"))
	{
		m_menuSelect = (m_menuSelect == MenuSelect::Retle) ? MenuSelect::Title : MenuSelect::Retle;
		SoundManager::Instance().PlaySE("Cursor");
	}

	if (input.IsTriggered("ok"))
	{
		SoundManager::Instance().PlaySE("Ok");
		m_isDecided = true;
	}
}

void GameOverSceneUI::Draw()
{
	
	
	if (m_menuSelect == MenuSelect::Retle)
	{
		//操作説明表示
		FontManager::Instance().DrawBottomRightAndQuakeText(850, Game::kScreenHeight / 2, "Retry", 0xfff000, 88, 0xff0000,true, kQuakeAmplitude,kQuakePower);
	}
	else
	{
		FontManager::Instance().DrawBottomRightAndQuakeText(850, Game::kScreenHeight / 2, "Retry", 0xffffff, 88, 0xff0000);
	}
	if (m_menuSelect == MenuSelect::Title )
	{
		//操作説明表示
		FontManager::Instance().DrawBottomRightAndQuakeText(1360, Game::kScreenHeight / 2, "Title", 0xfff000, 88, 0xff0000,true, kQuakeAmplitude,kQuakePower);
	}
	else
	{
		FontManager::Instance().DrawBottomRightAndQuakeText(1360, Game::kScreenHeight / 2, "Title", 0xffffff, 88, 0xff0000);
	}

	////大きめのGameOver表示
	//const int goColor = GetColor(255, 80, 80);
	//FontManager::Instance().DrawCenteredExtendText(Game::kScreenWidth / 2, 140, "Game Over", goColor, 80, black, 1.0f);
	//ロゴの描画
	DrawRotaGraph(Game::kScreenWidth / 2, m_logoposY, 0.8f, 0, m_logoHandle, true);

}
