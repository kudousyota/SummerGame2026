#pragma once
#include "UIBase.h"
class Player;
class WitchTimeNeedleUI: public UIBase
{
public:
	WitchTimeNeedleUI();
	~WitchTimeNeedleUI();
	void Init()override;
	void Update()override;
	void Draw()override;
private:
	//ƒnƒ“ƒhƒ‹
	int m_nidelHandle;
};

