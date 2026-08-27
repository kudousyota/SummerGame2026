#pragma once
#include "Scene.h"
#include "../System/Input.h"
#include "../System/Vector3.h"
#include "CharacterViewer.h"
#include <vector>
#include <memory>

class UIManager;
class EffectManager;
class GameOverScene :public Scene
{
public:
	GameOverScene(SceneController& controller);
	~GameOverScene();
	void Init();
	void Update(Input& input)override;
	void Draw()override;
private:

	enum TitleCharacter
	{
		Player,
		Creature,
	};

	struct CharacterInfo
	{
		int modelHandle;
		std::string animName;
		Vector3 pos;
		Vector3 scale;
	};

	//経過フレーム
	int m_frame;
	//フォント
	int m_fontHandle;
	//シーン終了フラグ
	bool m_finished;

	//エフェクトの場所
	Vector3 m_effectPos;
	//エフェクトのハンドル
	int m_effectHandle;

	//メンバ関数ポインタを使用して内部状態を書き換える
	//update関数とdraw関数を、現在のTitleSceneの状態に合わせて
	//切り替えていく

	//フェードインupdate
	void FadeInUpdate(Input& input);
	//通常update
	void NormalUpdate(Input&);
	//フェードアウトupdate
	void FadeOutUpdate(Input&);
	using UpdateFunc_t = void(GameOverScene::*)(Input&);
	//Updateメンバ関数を代入
	UpdateFunc_t m_update;

	void NormalDraw();//通常Draw
	void FadeDraw();//フェードDraw
	using DrawFunc_t = void(GameOverScene::*)();
	DrawFunc_t m_draw;//Drawメンバ関数を代入できるメンバ関数ポインタ

	std::vector<CharacterViewer>m_GameOverCharacter;

	std::unique_ptr<UIManager> m_pUiManager;
};

