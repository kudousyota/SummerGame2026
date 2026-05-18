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

private:
	//ハンドル
	int m_modelHandle;

};

