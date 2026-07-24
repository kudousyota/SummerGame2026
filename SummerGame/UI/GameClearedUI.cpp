#include "GameClearedUI.h"
#include "DxLib.h"
#include "../Game/Character/Player.h"


GameClearedUI::GameClearedUI():
	m_ResultUIHandle(-1),
	m_TimerUIHandle(-1),
	m_ScoreUIHandle(-1)
{
}

GameClearedUI::~GameClearedUI()
{
	DeleteGraph(m_ResultUIHandle);
	DeleteGraph(m_TimerUIHandle);
	DeleteGraph(m_ScoreUIHandle);
}

void GameClearedUI::Init()
{
	m_ResultUIHandle = LoadGraph("Data/ResultUI.png");
	m_TimerUIHandle = LoadGraph("Data/TimerUI.png");
	m_ScoreUIHandle = LoadGraph("Data/ScoreUI.png");
}

void GameClearedUI::Update()
{
}

void GameClearedUI::Draw()
{

	DrawRotaGraph(500, 100, 0.5f, 0, m_TimerUIHandle, true);
	DrawRotaGraph(580, 300, 0.5f, 0, m_ScoreUIHandle, true);
	DrawRotaGraph(620, 360, 1.0f, 0, m_ResultUIHandle, true);

}
