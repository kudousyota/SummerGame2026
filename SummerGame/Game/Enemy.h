#pragma once
#include "../System/Vector3.h"
#include "Character.h"
#include <memory> 

class Input;
class Enemy: public Character
{
public:
	Enemy();
	~Enemy()override;
	void Init()override;
	void Update(const Input& input)override;
	void Draw()override;

	//ダメージ
	void ApplyDamage(int damage)override;

	float GetCollisionRadius() const override { return 70.0f; }
	float GetCollisionHeight() const override { return 100.0f; }

private:
	//ハンドル
	int m_modelHandle;

};

