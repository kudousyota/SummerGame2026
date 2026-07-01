#pragma once
#include "../System/Vector3.h"
class Stage
{
public:
	Stage();
	~Stage();
	void Init();
	void Draw();

	int GetModelHandle()const { return m_modelHandle; }
private:
	int m_modelHandle;

	Vector3 m_stagePos;
};

