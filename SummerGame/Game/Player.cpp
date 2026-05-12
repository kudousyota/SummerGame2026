#include "Player.h"
#include "DxLib.h"

Player::Player():
	m_modelHandle(-1)
{
}

Player::~Player()
{

}

void Player::Init()
{
	m_modelHandle = MV1LoadModel("data/Player.mv1");
}

void Player::Update(const Input& input)
{
}

void Player::Draw()
{
	MV1DrawModel(m_modelHandle);
}
