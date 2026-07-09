#pragma once
#include "../Game/Character.h"
class Enemy : public Character
{
public:
	Enemy();
	~Enemy()override;
	void Init()override;
	void Update()override = 0;
	void Draw()override = 0;

	
};

