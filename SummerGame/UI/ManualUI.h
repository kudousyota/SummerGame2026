#pragma once
#include "UIBase.h"
class ManualUI : public UIBase
{
public:
	ManualUI();
	~ManualUI();
	void Init()override;
	void Update(Input& input)override;
	void Draw()override;
private:
	//ƒnƒ“ƒhƒ‹
	int m_manualHandle;
};

