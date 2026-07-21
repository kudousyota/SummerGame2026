#include "HPUI.h"
#include "DxLib.h"
#include "../Game/Player.h"

namespace
{
	constexpr int kGaugeWidth = 200;
	constexpr int kGaugeHeight = 20;
}

HPUI::HPUI():
	m_hpGaugeHandle(-1),
	m_hpGaugeBackHandle(-1),
	m_hp(0),
	m_maxHp(0),
	m_hpWidth(0)
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
	auto player = m_pPlayer.lock();
	if (!player)
	{
		//プレイヤーが存在しない場合
		return;
	}
	m_hp = player->GetHP();
	m_maxHp = player->GetMaxHP();

	if (m_maxHp > 0)
	{
		//HPゲージの描画
		//HPの割合に応じてゲージの幅を計算 //最大HP
		m_hpWidth = kGaugeWidth * m_hp / m_maxHp;
	}
	else
	{
		m_hpWidth = 0;
	}
}

void HPUI::Draw()
{
	//先にHPの枠を描画
	DrawGraph(80, 70, m_hpGaugeBackHandle, true);
	//現在のHPを描画
	DrawExtendGraph(80, 70, 80 + m_hpWidth, 70 + kGaugeHeight, m_hpGaugeHandle, true);
}