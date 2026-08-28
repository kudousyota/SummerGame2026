#pragma once
#include "UIBase.h"
#include "../Math/Vector3.h"

class Input;
class TitleUI :public UIBase
{
public:
	TitleUI();
	~TitleUI();
	void Init()override;
	void Update(Input& input)override;
	void Draw()override;

	enum class MenuSelect
	{
		Start,
		Exit
	};
	MenuSelect m_menuSelect = MenuSelect::Start;
	MenuSelect GetMenuSelect()const { return m_menuSelect; }
	//okが押されたかどうか
	bool IsDecided()const { return m_isDecided; }
	//決定フラグをリセット
	void ResetDecided() { m_isDecided = false; }
private:
	int	m_fontHandle;
	int	m_logoHandle;
	Vector3	m_logoPos;
	bool m_isDecided;
	
};

