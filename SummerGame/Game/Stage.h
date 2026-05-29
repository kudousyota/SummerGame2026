#pragma once
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
};

