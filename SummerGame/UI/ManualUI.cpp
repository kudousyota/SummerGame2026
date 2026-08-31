#include "ManualUI.h"
#include <DxLib.h>
#include "../Game.h"

ManualUI::ManualUI():
	m_manualHandle(-1)
{
}

ManualUI::~ManualUI()
{
}

void ManualUI::Init()
{
	m_manualHandle = LoadGraph("Data/Manual.png");
}

void ManualUI::Update(Input& input)
{
}

void ManualUI::Draw()
{
    //基準解像度1280x720に対するスケーリング
	const float sx = static_cast<float>(Game::kScreenWidth) / 1280.0f;
	const float sy = static_cast<float>(Game::kScreenHeight) / 720.0f;
	const int x = static_cast<int>(1100 * sx);
	const int y = static_cast<int>(600 * sy);
	const float scale = 0.7f * sx;
	DrawRotaGraph(x, y, scale, 0, m_manualHandle, true);
}
