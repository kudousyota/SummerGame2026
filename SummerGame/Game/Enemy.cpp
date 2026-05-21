#include "Enemy.h"
#include "DxLib.h"

Enemy::Enemy():
	m_modelHandle(-1)
{
}

Enemy::~Enemy()
{
	MV1DeleteModel(m_modelHandle);
}

void Enemy::Init()
{
	m_pos = VGet(0.0f, 0.0f, 0.0f);
	m_modelHandle = MV1LoadModel("Data/Enemy.mv1");
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
}
