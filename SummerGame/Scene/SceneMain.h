#pragma once
#include "../Game/Player.h"
#include <memory>

class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update();
	void Draw();
private:
	void DrawGrid();

private:
	int m_frameCount;

	std::shared_ptr<Player> m_pPlayer;
};

