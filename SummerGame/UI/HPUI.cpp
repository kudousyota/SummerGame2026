#include "HPUI.h"
#include "DxLib.h"

namespace
{
	constexpr int kGaugeWidth = 200;
}

HPUI::HPUI():
	m_hpGaugeHandle(-1),
	m_hpGaugeBackHandle(-1),
	m_hpX(0),
	m_hpY(0)
{

}

HPUI::~HPUI()
{
}

void HPUI::Init()
{
	m_hpGaugeHandle = LoadGraph("Data/gauge_green.png");
	m_hpGaugeBackHandle = LoadGraph("Data/HPberfrem.png");
}

void HPUI::Update()
{
	//HPƒQ[ƒW‚Ì•`‰æ
	//HP‚ÌŠ„‡‚É‰‚¶‚ÄƒQ[ƒW‚Ì•‚ğŒvZ //Å‘åHP
	m_hpWidth = kGaugeWidth * m_hp / 100;
}

void HPUI::Draw()
{
	DrawExtendGraph(80, 70, 80 + m_hpWidth, 70 + m_hpY, m_hpGaugeHandle, true);
}