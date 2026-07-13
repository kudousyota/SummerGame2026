#pragma once
#include "../Game/Player.h"
#include "../Game/Camera.h"
#include <memory>
#include "../Game/Stage.h"
#include "../Game/EnemyManager.h"
#include "../Game/EnemySpawn.h"
#include "Scene.h" //Sceneの基底クラスをインクルード
#include "../Game/Creature.h"

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


	//メンバ関数ポインタを使用して内部状態を書き換える
	//update関数とdraw関数を、現在のTitleSceneの状態に合わせて
	//切り替えていく
	void FadeInUpdate(Input&);	//フェードインUpdate
	void NormalUpdate(Input&);	//通常Update
	void FadeOutUpdate(Input&);	//フェードアウトUpdate
	using UpdateFunc_t = void(SceneMain::*)(Input&);
	UpdateFunc_t m_update;	//Updateメンバ関数を代入できるメンバ関数ポインタ


	void NormalDraw();	//通常Draw
	void FadeDraw();	//フェードDraw
	using DrawFunc_t = void(SceneMain::*)();
	DrawFunc_t m_draw;	//Drawメンバ関数を代入できるメンバ関数ポインタ


private:

	//タイトルシーンが終了したかどうか
	bool m_finished;

	//フォントハンドル
	int m_fontHandle;
	//経過フレーム
	int m_frame;
	//スカイドームのモデルハンドル
	int m_skyHandle;
	//プレイヤーのモデルハンドル
	int m_playerHandle;
	//アニメーションハンドル
	int m_cureentAnimHandle;

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
