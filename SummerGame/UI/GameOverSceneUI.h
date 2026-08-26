#pragma once
#include "UIBase.h"

class GameOverSceneUI : public UIBase
{
public:
    GameOverSceneUI();
    ~GameOverSceneUI();
    void Init() override;
    void Update(Input& input) override;
    void Draw() override;

    enum class MenuSelect
    {
        Retle,
        Title
    };
    MenuSelect m_menuSelect = MenuSelect::Retle;
    MenuSelect GetMenuSelect()const { return m_menuSelect; }
    //okが押されたかどうか
    bool IsDecided()const { return m_isDecided; }
    //決定フラグをリセット
    void ResetDecided() { m_isDecided = false; }
private:
    int m_logoHandle;

    int m_logoposX;
    int m_logoposY;
    bool m_isUp;
    bool m_isDecided;
};

