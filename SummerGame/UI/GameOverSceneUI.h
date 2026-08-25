#pragma once
#include "UIBase.h"

class GameOverSceneUI : public UIBase
{
public:
    GameOverSceneUI();
    ~GameOverSceneUI();
    void Init() override;
    void Update() override;
    void Draw() override;
private:
    int m_logoHandle;
};

