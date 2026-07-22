#include "HPUI.h"
#include "DxLib.h"
#include "../Game/Player.h"

namespace
{
	constexpr int kGaugeWidth = 200;
	constexpr int kGaugeHeight = 20;
	constexpr int kHpAngle = 0.0f;
	//”wŒi
	constexpr int kHpBackX = 170;
	constexpr int kHpBackY = 70;
	constexpr float kHpBackScale = 0.5f;
	
	//ƒQ[ƒW
	constexpr int kHpGaugeX = 80;
	constexpr int kHpGaugeY = 65;


	//ŠW
	constexpr int kHpLidLeft = 80;
	constexpr int kHoLidRight = 280;
	constexpr int kHpLidY = 75;
	constexpr float kHpLidScale = 0.8f;

}

HPUI::HPUI():
	m_hpGaugeHandle(-1),
	m_hpGaugeBackHandle(-1),
	m_hplidHandle(-1),
	m_hp(0),
	m_maxHp(0),
	m_hpWidth(0)
{

}

HPUI::~HPUI()
{
	DeleteGraph(m_hpGaugeHandle);
	DeleteGraph(m_hpGaugeBackHandle);
	DeleteGraph(m_hplidHandle);
}

void HPUI::Init()
{
	m_hpGaugeHandle = LoadGraph("Data/gauge_green.png");
	m_hpGaugeBackHandle = LoadGraph("Data/HPberfrem.png");
	m_hplidHandle = LoadGraph("Data/HPlid.png");
}

void HPUI::Update()
{
	auto player = m_pPlayer.lock();
	if (!player)
	{
		//ƒvƒŒƒCƒ„[‚ª‘¶İ‚µ‚È‚¢ê‡
		return;
	}
	m_hp = player->GetHP();
	m_maxHp = player->GetMaxHP();

	if (m_maxHp > 0)
	{
		//HPƒQ[ƒW‚Ì•`‰æ
		//HP‚ÌŠ„‡‚É‰‚¶‚ÄƒQ[ƒW‚Ì•‚ğŒvZ //Å‘åHP
		m_hpWidth = kGaugeWidth * m_hp / m_maxHp;
	}
	else
	{
		m_hpWidth = 0;
	}
}

void HPUI::Draw()
{
	//æ‚ÉHP‚Ì˜g‚ğ•`‰æ
	DrawRotaGraph(kHpBackX, kHpBackY, kHpBackScale, kHpAngle, m_hpGaugeBackHandle, true);
	//Œ»İ‚ÌHP‚ğ•`‰æ
	DrawExtendGraph(kHpGaugeX, kHpGaugeY, kHpGaugeX + m_hpWidth, kHpGaugeY + kGaugeHeight, m_hpGaugeHandle, true);
	//¶ŠW
	DrawRotaGraph(kHpLidLeft, kHpLidY, kHpLidScale, kHpAngle, m_hplidHandle, true);
	//‰EŠW
	DrawRotaGraph(kHoLidRight, kHpLidY, kHpLidScale, kHpAngle, m_hplidHandle, true);
}