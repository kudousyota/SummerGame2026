#include "Enemy.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"

Enemy::Enemy():
	m_modelHandle(-1)
{
}

Enemy::~Enemy()
{
	CollisionManager::Instance().Unregister(this);

	MV1DeleteModel(m_modelHandle);
}

void Enemy::Init()
{
	Character::Init();

	m_hp = 100;

	m_pos = VGet(0.0f, 0.0f, 0.0f);
	m_modelHandle = MV1LoadModel("Data/Enemy.mv1");

	CollisionManager::Instance().Register(this);
}

void Enemy::Update(const Input& input)
{
	

	// モデル行列更新
	MATRIX rot = MGetRotY(m_angle);
	MATRIX trans = MGetTranslate(m_pos.ToDxLibVector());
	MV1SetMatrix(m_modelHandle, MMult(rot, trans));
}


void Enemy::Draw()
{
	MV1DrawModel(m_modelHandle);

	DrawCapsule3D(m_pos.ToDxLibVector(),
		VGet(m_pos.x, m_pos.y + 100.0f, m_pos.z),
		70.0f,
		16,
		GetColor(255, 0, 0),
		GetColor(255, 0, 0),
		false
	);

	DrawFormatString(
		50,
		50,
		GetColor(255, 255, 255),
		"EnemyHP:%d",
		m_hp
	);

}

void Enemy::ApplyDamage(int damage)
{
	m_hp -= damage;

	if (m_hp < 0)
	{
		m_hp = 0;
	}

	printfDx("Enemy HP = %d\n",m_hp);
}
