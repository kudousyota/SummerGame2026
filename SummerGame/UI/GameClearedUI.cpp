#include "GameClearedUI.h"
#include "DxLib.h"
#include "../Game/Character/Player.h"
#include "../System/Score.h"
#include "../Common/FontManager.h"


namespace
{
	const float kTimerGoalX = 500.0f;
	const float kScoreGoalX = 580.0f;
	const float kResultGoalX = 620.0f;
}
GameClearedUI::GameClearedUI():
	m_ResultUIHandle(-1),
	m_TimerUIHandle(-1),
	m_ScoreUIHandle(-1),
	m_timerX(0.0f),
	m_scoreX(0.0f)
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

	m_timerX = -300;
	m_scoreX = -300; 
}

void GameClearedUI::Update()
{
	//ラープ
	m_timerX += (kTimerGoalX - m_timerX) * 0.1f;
	m_scoreX += (kScoreGoalX - m_scoreX) * 0.1f;
	//ほぼ到着したらぴったり合わせる
	if (fabsf(kTimerGoalX - m_timerX) < 1.0f)
	{
		m_timerX = kTimerGoalX;
	}
	if (fabsf(kScoreGoalX - m_scoreX) < 1.0f)
	{
		m_scoreX = kScoreGoalX;
	}

}

void GameClearedUI::Draw()
{
	DrawRotaGraph(static_cast<int>(m_timerX), 100, 0.5f, 0, m_TimerUIHandle, true);
	DrawRotaGraph(static_cast<int>(m_scoreX), 300, 0.5f, 0, m_ScoreUIHandle, true);
	DrawRotaGraph(620, 360, 1.0f, 0, m_ResultUIHandle, true);

	const int white = GetColor(255, 255, 255);
	const int black = GetColor(0, 0, 0);
	const int fontSize = 24;

	//各項目ごとのスコアを表示
	std::string text = "Kill:" + std::to_string(Score::Instance().GetEnemyScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 60, 280, text, white, fontSize, black);


	/*DrawFormatString(static_cast<int>(m_scoreX) - 60, 280, white, "敵撃破: %d", Score::Instance().GetEnemyScore());
	DrawFormatString(static_cast<int>(m_scoreX) - 60, 310, white, "タイム: %d", Score::Instance().GetTimeScore());
	DrawFormatString(static_cast<int>(m_scoreX) - 60, 340, white, "ウィッチタイム: %d", Score::Instance().GetWitchTimeScore());
	DrawFormatString(static_cast<int>(m_scoreX) - 60, 370, white, "ノーダメージ: %d", Score::Instance().GetNoDamageScore());
	DrawFormatString(620, 400, white, "合計: %d", Score::Instance().GetTotalScore());*/

}
