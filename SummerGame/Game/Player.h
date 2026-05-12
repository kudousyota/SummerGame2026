#pragma once
#include "../System/Vector3.h"

class Input;
class Player
{
public:
	Player();
	~Player();
	void Init();
	void Update(const Input& input);
	void Draw();
private:
	int m_modelHandle;
};

