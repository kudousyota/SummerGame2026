#include "ManualUI.h"
#include <DxLib.h>

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

void ManualUI::Update()
{
}

void ManualUI::Draw()
{
	DrawRotaGraph(1100, 600, 0.7f, 0, m_manualHandle, true);
}
