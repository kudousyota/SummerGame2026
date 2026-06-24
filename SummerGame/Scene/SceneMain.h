#pragma once
#include "../Game/Player.h"
#include "../Game/Camera.h"
#include <memory>
#include "../Game/Enemy.h"
#include <vector>
#include "../Game/Stage.h"
#include "../Game/Angel.h"

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
	int m_witchEffectFrame;

	bool m_isPrevWitchTime;

	float m_angle;

	int m_nidelHandle;

	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<Stage> m_pStage;
	std::vector<std::shared_ptr<Enemy>> m_pEnemy;
	std::vector<std::shared_ptr<Angel>> m_pAngel;
};

