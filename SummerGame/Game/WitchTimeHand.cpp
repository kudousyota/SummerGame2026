#include "WitchTimeHand.h"
#include "Character/Player.h"
#include "../System/Model.h"
#include "../System/Vector3.h"
#include "../System/Animation.h"

namespace
{
	const char* const kPunchAnimName = "Hand|Punch";
}

WitchTimeHand::WitchTimeHand():
	m_modelHandle(-1)
{
}

WitchTimeHand::~WitchTimeHand()
{
}
void WitchTimeHand::Init()
{
	m_modelHandle = Model::Instance().CreatWitchTimeHandModel();
	m_animation.Init(m_modelHandle, kPunchAnimName, true, 0.5f);
	//モデルのスケールを設定する
	MV1SetScale(m_modelHandle,Vector3(2.0f,2.0f,2.0f));
}
void WitchTimeHand::Update()
{
	m_animation.Update(0.5f);
	//プレイヤーの位置を取得して手の位置を設定する
	auto player = m_pPlayer.lock();
	if (player)
	{
		Vector3 playerPos = player->GetPosition();
		//手の位置をプレイヤーの位置に設定する
		SetPosition(playerPos);
	}
}
void WitchTimeHand::Draw()
{
	//モデルの描画
	MV1DrawModel(m_modelHandle);
}