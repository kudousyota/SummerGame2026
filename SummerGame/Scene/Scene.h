#pragma once
class SceneController; //シーンコントローラーのプロトタイプ宣言
class Input; //入力のプロトタイプ宣言

///<summary>
///シーン基底クラス
///</summary>
class Scene
{
protected:
	SceneController& m_controller;
public:
	Scene(SceneController& controller);

	virtual void Init() =0;
	///<summary>
	///シーンの情報の更新
	///</summary>
	///<param name="input">入力情報</param>
	virtual void Update(Input& input) = 0;

	///<summary>
	///シーンの描画(シーンの持ち物も描画する)
	///</summary>
	virtual void Draw() = 0;
};

