#include "GameClearedUI.h"
#include "DxLib.h"
#include "../Game/Character/Player.h"
#include "../System/Score.h"
#include "../Common/FontManager.h"

// GameClearedUI
// ゲームクリア時に表示する結果ウィンドウを管理する実装ファイル。
// - 各種UI画像を読み込み、画面外から滑らかに入ってくるアニメーションを行う
// - スコアやタイムなどのテキスト描画を行う


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
    // ラープで UI を目標位置へ滑らかに移動させる
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
	//変数名の重複を避けるため再代入で対応
	std::string text;
	//キル数を描画
	text = "Kile:" + std::to_string(Score::Instance().GetEnemyScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 60, 280, text, white, fontSize, black);
	//タイムを描画
	text = "Time:" + std::to_string(Score::Instance().GetTimeScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 60, 310, text, white, fontSize, black);
	//ウィッチタイム
	text = "WitchTime:" + std::to_string(Score::Instance().GetWitchTimeScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 60, 340, text, white, fontSize, black);
	//ノーダメージ
	text = "NoDamage:" + std::to_string(Score::Instance().GetWitchTimeScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 60, 370, text, white, fontSize, black);
	//合計
	text = "Totale:" + std::to_string(Score::Instance().GetTotalScore());
	FontManager::Instance().DrawLeftText(static_cast<int>(m_scoreX) - 60, 400, text, white, fontSize, black);

	/*DrawFormatString(static_cast<int>(m_scoreX) - 60, 280, white, "敵撃破: %d", Score::Instance().GetEnemyScore());
	DrawFormatString(static_cast<int>(m_scoreX) - 60, 310, white, "タイム: %d", Score::Instance().GetTimeScore());
	DrawFormatString(static_cast<int>(m_scoreX) - 60, 340, white, "ウィッチタイム: %d", Score::Instance().GetWitchTimeScore());
	DrawFormatString(static_cast<int>(m_scoreX) - 60, 370, white, "ノーダメージ: %d", Score::Instance().GetNoDamageScore());
	DrawFormatString(620, 400, white, "合計: %d", Score::Instance().GetTotalScore());*/

}
