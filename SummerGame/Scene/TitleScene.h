#pragma once
#include "../System/Input.h"
#include "../System/Vector3.h"
#include "Scene.h"
#include "CharacterViewer.h"

class TitleScene: public Scene
{
public:
	TitleScene(SceneController& controller);
	void Init();
	void Update(Input& input);
	void Draw();
	bool IsFinished() const { return m_finished; }
private:
	

	//メンバ関数ポインタを使用して内部状態を書き換える
	//update関数とdraw関数を、現在のTitleSceneの状態に合わせて
	//切り替えていく
	void FadeInUpdate(Input&);	//フェードインUpdate
	void NormalUpdate(Input&);	//通常Update
	void FadeOutUpdate(Input&);	//フェードアウトUpdate
	using UpdateFunc_t = void(TitleScene::*)(Input&);
	UpdateFunc_t m_update;	//Updateメンバ関数を代入できるメンバ関数ポインタ


	void NormalDraw();	//通常Draw
	void FadeDraw();	//フェードDraw
	using DrawFunc_t = void(TitleScene::*)();
	DrawFunc_t m_draw;	//Drawメンバ関数を代入できるメンバ関数ポインタ
	
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

	//アニメーション再生時間を小数で扱うためfloatにする
	float m_currentAnimCount;
	//現在セットしているアニメーションのインデックス
	int m_currentAnimIndex;
	//プレイヤーの位置
	Vector3 m_playerPos;
	//プレイヤーの角度
	float m_playerAngle;
	//タイトルの地面ハンドル
	int m_groundHandle;
	//タイトルの地面の位置
	Vector3 m_groundPos;
	//ロゴハンドル
	int m_logoHandle;
	//ロゴの位置
	Vector3 m_logoPos;

	CharacterViewer m_titlePlayer;
};

