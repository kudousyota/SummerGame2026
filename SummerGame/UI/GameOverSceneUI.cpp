#include "GameOverSceneUI.h"
#include "DxLib.h"
#include "../Common/FontManager.h"
#include "../Game.h"

GameOverSceneUI::GameOverSceneUI() :
	m_logoHandle(-1)
{
}

GameOverSceneUI::~GameOverSceneUI()
{
	DeleteGraph(m_logoHandle);
}

void GameOverSceneUI::Init()
{
	m_logoHandle = LoadGraph("Data/Continue.png");
}

void GameOverSceneUI::Update()
{
}

void GameOverSceneUI::Draw()
{
	
	const int white = GetColor(255, 255, 255);
	const int Cyan = GetColor(0, 255, 255);
	const int Color = GetColor(224, 255, 255);
	const int black = GetColor(0, 0, 0);

	//ì_ñ≈ïpìx
	const int intervar = 650;
	int now = GetNowCount();
	bool visible = (now / intervar) % 2;
	if (visible)
	{
		//ëÄçÏê‡ñæï\é¶
		FontManager::Instance().DrawCenteredText(Game::kScreenWidth / 2, 580, "Press A to Retry", white, 24, black);
	}

	////ëÂÇ´ÇﬂÇÃGameOverï\é¶
	//const int goColor = GetColor(255, 80, 80);
	//FontManager::Instance().DrawCenteredExtendText(Game::kScreenWidth / 2, 140, "Game Over", goColor, 80, black, 1.0f);
	//ÉçÉSÇÃï`âÊ
	DrawRotaGraph(620, 360, 0.02f, 0, m_logoHandle, true);

}
