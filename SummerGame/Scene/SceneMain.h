#pragma once
#include "../Game/Player.h"
#include "../Game/Camera.h"
#include <memory>

class Input;
class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update(Input& input);
	void Draw();
private:
	void DrawGrid();

private:
	int m_frameCount;

	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Camera> m_pCamera;
};

