#include "WitchTimeHand.h"
#include "Character/Player.h"
#include "../System/Model.h"
#include "../System/CollisionManager.h"
#include "../DataLoader/AttackData.h" 



namespace
{
	const char* const kPunchAnimName = "Hand|Punch";
	constexpr float kAnimSpeed = 0.5f;
	constexpr int kDamage = 100;
	constexpr float kAttackRadius = 150.0f;
	constexpr float kAttackFrame = 10.0f;
}

WitchTimeHand::WitchTimeHand():
	m_modelHandle(-1),
	m_pos(Vector3(0.0f,0.0f,0.0f)),
	m_angle(0.0f),
	m_isActive(false),
	m_hasAttacked(false)
{
}

WitchTimeHand::~WitchTimeHand()
{
	MV1DeleteModel(m_modelHandle);
}
void WitchTimeHand::Init()
{
	m_modelHandle = Model::Instance().CreatWitchTimeHandModel();
	m_animation.Init(m_modelHandle, kPunchAnimName, false, 0.5f);
	//モデルのスケールを設定する
	MV1SetScale(m_modelHandle,Vector3(2.0f,2.0f,2.0f));

}
void WitchTimeHand::Update()
{
	//アクティブでない場合は更新しない
	if (!m_isActive)
	{
		return;
	}
	m_animation.Update(kAnimSpeed);
	//プレイヤーの位置を取得して手の位置を設定する
	auto player = m_pPlayer.lock();
	//プレイヤーが存在しない場合は更新しない
	if (!player) return;

	//角度をプレイヤーに合わせる
	m_angle = player->GetAngle();

	m_pos = player->GetPosition() + player->GetForward() *120.0f;
	
	if(!m_hasAttacked &&m_animation.GetCurrentAnimTime() >= kAttackFrame)
	{
		AttackUpdate();

		m_hasAttacked = true;
	}
	if (m_animation.GetAnimEndFlag())
	{
		Disappear();
	}

	//モデルの回転を設定する
	MV1SetRotationXYZ(m_modelHandle, Vector3(0.0f, m_angle, 0.0f).ToDxLibVector());

	MV1SetPosition(m_modelHandle, m_pos.ToDxLibVector());
}
void WitchTimeHand::Draw()
{
	if (!m_isActive)
	{
		return;
	}
	//モデルの描画
	MV1DrawModel(m_modelHandle);
}

void WitchTimeHand::Appear()
{
	m_isActive = true;
	m_hasAttacked = false;
	m_animation.ChangeAnim(kPunchAnimName, false, kAnimSpeed);
}

void WitchTimeHand::Disappear()
{
	m_isActive = false;
}

void WitchTimeHand::AttackUpdate()
{
	//攻撃判定を作成して当たり判定をチェックする
	AttackData attack(CharacterType::Player, AttackType::WitchHand, kDamage, kAttackRadius);
	CollisionManager::Instance().CheckAttackSphere(attack,m_pos);
}
