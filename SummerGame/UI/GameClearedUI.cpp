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
	const int kIntervar = 650;
	constexpr int kfontBigsize = 48;
}
GameClearedUI::GameClearedUI():
	m_ResultUIHandle(-1),
	m_TimerUIHandle(-1),
	m_ScoreUIHandle(-1),
	m_timerX(0.0f),
	m_scoreX(0.0f),
	m_now(0)
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
    //ラープでUIを目標位置へ滑らかに移動させる
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
	//白より
	//const int papul = GetColor(220, 200, 240);
	//紫より
	const int papul = GetColor(180, 100, 220);
	//青より
	//const int papul = GetColor(190, 180, 240);

	const int fontSize = 24;

	//スコアやタイムなどのテキスト描画を行う
    //各項目ごとのスコアを表示
	//変数名の重複を避けるため再代入で対応
	std::string text;
	//タイムを描画
	text = "Time:" + std::to_string(Score::Instance().GetTimeScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 190, 240, text, white, fontSize, papul);

	//ウィッチタイム
	text = "WitchTime:" + std::to_string(Score::Instance().GetWitchTimeScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 170, 280, text, white, fontSize, papul);

	//キル数を描画
	text = "Kile:" + std::to_string(Score::Instance().GetEnemyScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 110, 440, text, white, fontSize, papul);
	
	//ノーダメージ
	text = "NoDamage:" + std::to_string(Score::Instance().GetWitchTimeScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 95, 480, text, white, fontSize, papul);
	
	//合計
	text = "Totale:" + std::to_string(Score::Instance().GetTotalScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 80, 520, text, white, fontSize, papul);

	//操作説明
	text = "Press A to Title";
	m_now = GetNowCount();
	m_isVisible = (m_now / kIntervar) % 2;
	if (m_isVisible)
	{
		FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 60, 620, text, white, kfontBigsize, papul);
	}
	
}
