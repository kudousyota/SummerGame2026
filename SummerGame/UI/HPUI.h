#pragma once
#include "UIManager.h"
class HPUI : public UIManager
{
public:
	HPUI();
	~HPUI();
	void Init();
	void Update();
	void Draw();

private:
	int m_uiHandle;

	int m_hpGaugeHandle;

	int m_hpX;
	int m_hpY;

	int m_hpGaugeBackHandle;
};

