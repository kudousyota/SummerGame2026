#include "TitleUI.h"
#include <DxLib.h>
#include "../System/Input.h"
#include "../System/SoundManager.h"

namespace
{
	constexpr int kUiPosX = 590;
	constexpr int kStartY = 560;
	constexpr int kExitPosY = 610;
	//点滅の間隔
	constexpr int kBlinkInterval = 650;

}

TitleUI::TitleUI():
	m_fontHandle(-1),
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
	m_fontHandle = CreateFontToHandle("Constantia", 40, -1, DX_FONTTYPE_ANTIALIASING_EDGE);

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

	DrawRotaGraph(660, 150, 0.5, 0.0, m_logoHandle, true);

	const int intervar = 650;
	int now = GetNowCount();
	bool visible = (now / intervar) % 2;

	const int startColor = (m_menuSelect == MenuSelect::Start) ? GetColor(255, 255, 0) : white;
	const int exitColor = (m_menuSelect == MenuSelect::Exit) ? GetColor(255, 255, 0) : white;

	//選択中の文字だけ点滅させる例
	if (m_menuSelect != MenuSelect::Start || visible)
	{
		DrawStringToHandle(kUiPosX, kStartY, "Start", startColor, m_fontHandle);
	}
	if (m_menuSelect != MenuSelect::Exit || visible)
	{
		DrawStringToHandle(kUiPosX, kExitPosY, "Exit", exitColor, m_fontHandle);
	}
}
