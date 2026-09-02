#include "TitleUI.h"
#include <DxLib.h>
#include "../System/Input.h"
#include "../System/SoundManager.h"
#include "../Game.h"
#include "../Common/FontManager.h"

namespace
{
	constexpr int kUiPosX = 600;
	constexpr int kStartY = 780;
	constexpr int kExitPosY = 860;
	//点滅の間隔
	constexpr int kBlinkInterval = 350;
	//振れ幅
	constexpr int kQuakeAmplitude = 20;
	//揺らす力
	constexpr int kQuakePower = 15;
}

TitleUI::TitleUI():
	m_menuFontSize(40),
	m_logoHandle(-1),
	m_logoPos(Vector3(0.0f,0.0f,0.0f)),
	m_menuSelect(MenuSelect::Start),
	m_isDecided(false)
{
}

TitleUI::~TitleUI()
{
    
}

void TitleUI::Init()
{
    //基準サイズに合わせてメニュー文字の大きさを決定 (FontManager の用意したサイズに合わせる)
	const float sx = static_cast<float>(Game::kScreenWidth) / 1280.0f;
	int targetSize = static_cast<int>(40.0f * sx);
	if (targetSize < 8) targetSize = 8;
	//FontManager が用意しているサイズ一覧
	const int availableSizes[] = {8,16,24,32,40,48,56,64,72,80,88};
	int best = availableSizes[0];
	int bestDiff = abs(availableSizes[0] - targetSize);
	for (int s : availableSizes)
	{
		int d = abs(s - targetSize);
		if (d < bestDiff)
		{
			best = s;
			bestDiff = d;
		}
	}
	m_menuFontSize = best;

	m_logoHandle = LoadGraph("data/kudonetta.png");
}

void TitleUI::Update(Input& input)
{
	//上下でカーソル移動
	if (input.IsTriggered("up") || input.IsTriggered("down"))
	{
		m_menuSelect = (m_menuSelect == MenuSelect::Start) ? MenuSelect::Exit : MenuSelect::Start;
		SoundManager::Instance().PlaySE("Cursor");
	}

	if (input.IsTriggered("ok"))
	{
		SoundManager::Instance().PlaySE("Ok");
		m_isDecided = true;
	}
}

void TitleUI::Draw()
{
	const int white = GetColor(255, 255, 255);

	const int startColor = (m_menuSelect == MenuSelect::Start) ? GetColor(255, 255, 0) : white;
	const int exitColor = (m_menuSelect == MenuSelect::Exit) ? GetColor(255, 255, 0) : white;
	//ロゴの描画
	DrawRotaGraph(Game::kScreenWidth / 2, 200, 0.8f, 0.0, m_logoHandle, true);

	//選ばれている方を揺らす	
	if (m_menuSelect == MenuSelect::Start)
	{
		FontManager::Instance().DrawBottomRightAndQuakeText(1060, kStartY, "Start", 0xfff000, 88, 0xff0000, true, kQuakeAmplitude, kQuakePower);
	}
	//選ばれていない方は揺らさない
	else
	{
		FontManager::Instance().DrawBottomRightAndQuakeText(1060, kStartY, "Start", 0xffffff, 88, 0xff0000, false);
	}
	if (m_menuSelect == MenuSelect::Exit)
	{
		FontManager::Instance().DrawBottomRightAndQuakeText(1050, kExitPosY, "Exit", 0xfff000, 88, 0xff0000, true, kQuakeAmplitude, kQuakePower);
	}
	//選ばれていない方は揺らさない
	else
	{
		FontManager::Instance().DrawBottomRightAndQuakeText(1050, kExitPosY, "Exit", 0xffffff, 88, 0xff0000, false);
	}
}
