#include "WitchTimeNeedleUI.h"
#include "../Game/Player.h"

WitchTimeNeedleUI::WitchTimeNeedleUI():
	m_nidelHandle(-1),
	m_angle(0.0f),
	m_witchEffectFrame(0),
	m_isPrevWitchTime(false)
{
}

WitchTimeNeedleUI::~WitchTimeNeedleUI()
{
	DeleteGraph(m_nidelHandle);
}

void WitchTimeNeedleUI::Init()
{
	m_nidelHandle = LoadGraph("data/ui_niidle_flower.png");
}

void WitchTimeNeedleUI::Update()
{

	auto player = m_pPlayer.lock();
	//プレイヤーがウィッチタイムかどうかを知る
	bool currentwitch = player->GetWitchTime();
	//ウィッチタイムに入ったときに演出をする
	//今の状態&&前の状態
	if (currentwitch && !m_isPrevWitchTime)
	{
		//表示する時間
		m_witchEffectFrame = 80;
		//回転角度
		m_angle = 0.0f;
	}

	//次のフレームで判定するために保存
	m_isPrevWitchTime = currentwitch;
	//演出中だったら回転させる
	if (m_witchEffectFrame > 0)
	{
		//演出フレームをここで減らす
		m_witchEffectFrame--;

		//回転角度を増やす
		m_angle += DX_PI_F / 40.0f;

		//0から360度まで回転させる
		//360度以上にならないように
		if (m_angle > DX_TWO_PI_F)
		{
			m_angle -= DX_TWO_PI_F;
		}

	}
}

void WitchTimeNeedleUI::Draw()
{
	auto player = m_pPlayer.lock();
	//ウィッチタイムだったら
	if (player->GetWitchTime())
	{
		//DrawBox(0, 0, 1280, 720, GetColor(255, 0, 255), true);
		if (m_witchEffectFrame > 0)
		{
			//画面の真ん中に回転する画像を描画する
			DrawRectRotaGraph2(1280 / 2, 720 / 2, 0, 0, 256, 512, 128, 256, 1.5f, m_angle, m_nidelHandle, true, false);
		}

	}
}
