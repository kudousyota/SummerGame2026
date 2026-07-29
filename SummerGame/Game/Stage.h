#pragma once
#include "../System/Vector3.h"
class Stage
{
public:
	Stage();
	~Stage();
	void Init();
	void Draw();

	int GetModelHandle()const { return m_colliderHandle; }
private:
	int m_modelHandle;
	int m_colliderHandle;

	Vector3 m_stagePos;
};

