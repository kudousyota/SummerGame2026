#pragma once
#include "../Game/Player.h"
#include "../Game/Camera.h"
#include <memory>
#include "../Game/Stage.h"
#include "../Game/EnemyManager.h"
#include "../Game/EnemySpawn.h"
#include "Scene.h" //Sceneの基底クラスをインクルード

class Input;
class Timer;

//SceneMainをSceneの派生クラスにする
class SceneMain : public Scene
{
public:
	SceneMain(SceneController& controller);
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

	EnemyManager m_enemyManager;
	EnemySpawner m_enemySpawner;
};
