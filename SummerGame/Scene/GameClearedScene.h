#pragma once
#include "Scene.h"
#include "../system/Vector3.h"
//#include "../system/EffectManager.h"

class GameClearedScene :public Scene
{
public:
	GameClearedScene(SceneController& controller);
	~GameClearedScene();
	void Init();
	void Update(Input& input)override;
	void Draw()override;

	bool IsFinished() const { return m_finished; }
private:
	//経過フレーム
	int m_frame;
	//フォント
	int m_fontHandle;
	//スコア表示の足されていく演出用
	int m_displayScore;
	int m_finalScore;
	//アニメーションを用
	float m_scoreAnimTime;
	//シーン終了フラグ
	bool m_finished;
	//スカイドームのモデルハンドル
	int m_skyHandle;
	//リザルト用のプレイヤー
	int m_playerHandle;
	//リザルトプレイヤーの位置
	Vector3 m_playerPos;
	//アニメーション
	float m_currentAnimCount;
	//アニメーションのハンドル
	int m_cureentAnimHandle;
	//現在セットしているアニメーションのインデックス
	int m_currentAnimIndex;
	//敵のハンドル
	int m_enemyHandle;
	//敵の位置
	Vector3 m_enemyPos;
	//敵の角度
	float m_enemyAngle;
	//地面のハンドル
	int m_groundHandle;
	//地面の位置
	Vector3 m_groundPos;
	//エフェクトの位置
	Vector3 m_effectPos;


	//メンバ関数ポインタを使用して内部状態を書き換える
	//update関数とdraw関数を、現在のTitleSceneの状態に合わせて
	//切り替えていく

	//フェードインupdate
	void FadeInUpdate(Input& input);
	//通常update
	void NormalUpdate(Input&);
	//フェードアウトupdate
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void(GameClearedScene::*)(Input&);
	//Updateメンバ関数を代入
	UpdateFunc_t m_update;

    void NormalDraw();	//通常Draw
	void FadeDraw();	//フェードDraw
	using DrawFunc_t = void(GameClearedScene::*)();
	DrawFunc_t m_draw;	//Drawメンバ関数を代入できるメンバ関数ポインタ

	//EffectManager m_effectManger;
};

