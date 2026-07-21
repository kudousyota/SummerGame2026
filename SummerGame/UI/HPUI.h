#pragma once
#include "UIManager.h"
class HPUI : public UIManager
{
public:
	HPUI();
	~HPUI();
	void Init()override;
	void Update()override;
	void Draw()override;

private:
	//ƒnƒ“ƒhƒ‹
	int m_uiHandle;
	int m_hpGaugeHandle;
	int m_hpGaugeBackHandle;
	int m_hpX;
	int m_hpY;

	int m_hp;

	int m_hpWidth;

	
};

