#include "TitleUI.h"
#include <DxLib.h>
#include "../System/Input.h"
#include "../System/SoundManager.h"
#include "../Game.h"
#include "../Common/FontManager.h"

namespace
{
    //基準解像度の横幅(旧): 1280
	constexpr int kUiPosX = 600;
	constexpr int kStartY = 780;
	constexpr int kExitPosY = 830;
	//点滅の間隔
	constexpr int kBlinkInterval = 350;

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

	//横方向のみ画面幅に合わせて調整、縦位置は固定
	const float sx = static_cast<float>(Game::kScreenWidth) / 1280.0f;
	const int logoX = static_cast<int>(660 * sx);
	DrawRotaGraph(logoX, 200, 0.8f, 0.0, m_logoHandle, true);

    const int intervar = kBlinkInterval;
	int now = GetNowCount();
	bool visible = (now / intervar) % 2;

	const int startColor = (m_menuSelect == MenuSelect::Start) ? GetColor(255, 255, 0) : white;
	const int exitColor = (m_menuSelect == MenuSelect::Exit) ? GetColor(255, 255, 0) : white;

	//選択中の文字だけ点滅させる例
    const int uiX = static_cast<int>(kUiPosX * sx);
    if (m_menuSelect != MenuSelect::Start || visible)
	{
		FontManager::Instance().DrawLeftText(uiX, kStartY, "Start", startColor, m_menuFontSize, GetColor(0,0,0));
	}
	if (m_menuSelect != MenuSelect::Exit || visible)
	{
		FontManager::Instance().DrawLeftText(uiX, kExitPosY, "Exit", exitColor, m_menuFontSize, GetColor(0,0,0));
	}
}
