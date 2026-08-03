#include "Character.h"
#include "Stage.h"
#include "../System/CollisionManager.h"

Character::Character():
	m_speed(0.0f),
	m_hp(0),
	m_attackPower(0),
	m_gravity(0.0f),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_angle(0),
	m_modelHandle(-1),
	m_velocity(VGet(0.0f,0.0f,0.0f)),
	m_isGround(false),
	m_collisionRadius(0.0f),
	m_collisionHeight(0.0f)
{
}

Character::~Character()
{
}

void Character::Init()
{
	m_speed = 4.0f;
	m_hp = 100;
	m_attackPower = 10;
    //重力を少し強めにして落下のもっさり感を軽減
    m_gravity = 1.2f;
	m_pos = {0.0f, 0.0f, 0.0f};
	m_angle = 0.0f;
	m_collisionRadius = 30.0f;
	m_collisionHeight = 100.0f;
}

void Character::Collision()
{

    //落下速度の上限を設ける
    //値を大きくしてターミナル速度を上げ、ふわふわ感を軽減
    const float kMaxFallSpeed = -20.0f;

	//当たり判定
    if (!m_isGround)
    {
        // 空中にいる間: 落下時は重力を強めに適用して沈み込みを防ぐ
        if (m_velocity.y < 0.0f)
        {
            m_velocity.y -= m_gravity * 1.5f; //落下中は重力増幅
        }
        else
        {
            m_velocity.y -= m_gravity;
        }

        if (m_velocity.y < kMaxFallSpeed)
        {
            m_velocity.y = kMaxFallSpeed;
        }
    }
	
    //座標に落下分の移動量を足す
    m_pos.y += m_velocity.y;

	int stageHandle = m_pStage->GetModelHandle();
	//地面の判定
	float groundY = 0.0f;
	Vector3 outGroundNormal = Vector3(0.0f,0.0f,0.0f);
	

	// CollisionManagerに地面判定用の関数を呼び出す
    if (m_velocity.y <= 0.0f &&
        CollisionManager::Instance().CheckStageGround(this, stageHandle, groundY, outGroundNormal))
    {
        //足元の座標を地面の高さに合わせる
        //地面との差が小さい場合は即座にスナップして浮遊感を消す
        const float snapThreshold = 2.0f;
        if (m_pos.y - groundY <= snapThreshold)
        {
            m_pos.y = groundY;
            m_isGround = true;
            m_velocity.y = 0.0f;
        }
        else
        {
            //高速で落下している場合はそのまま埋まらないよう通常処理
            m_pos.y = groundY;
            m_isGround = true;
            m_velocity.y = 0.0f;
        }
    }
	else
	{
		m_isGround = false;
	}

	//座標を押し出すように
	CollisionManager::Instance().CheckStageWall(this, stageHandle);

}

void Character::ApplyDamage(int damage)
{
	m_hp -= damage;

	if (m_hp < 0)
	{
		m_hp = 0;
	}
}

Vector3 Character::GetCollisionPosition() const
{
	//現在地を返す
	return m_pos;
}

void Character::SetStage(std::shared_ptr<Stage> stage)
{
	//ステージをもらう
	m_pStage = stage;
}

void Character::OnHit(const AttackData& attackdata)
{
	//攻撃データ
	ApplyDamage(attackdata.GetDamage());
}
