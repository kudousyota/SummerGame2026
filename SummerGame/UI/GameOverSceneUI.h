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
private:
    int m_logoHandle;

    int m_logoposX;
    int m_logoposY;
    bool m_isUp;
};

