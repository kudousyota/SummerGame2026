#pragma once
#include <string>
//アニメーションさせるやつ
class Animation
{
public:
	Animation();
	~Animation();
	//handleとnameを受け取ってアニメーションの初期化を行う//nameはアニメーションの名前
	void Init(int modelHandle, std::string name, bool isRoop, float timeScale = 1.0f);
	//ここにtimeScaleを引数にして渡すことでプレイヤーでもエネミーでも使うと一緒に使える
	void Update();
	//アニメーションのブレンドを行う
	void AnimBlend();
	//アニメーションの再生を設定する//m_isEndを初期化
	void SetAnim(bool isRoop);
	//アニメーションの切り替え
	void ChangeAnim(std::string name, bool isRoop = true, float timeScale = 1.0f);
	//アニメーションが終わったかどうかのフラグを返す
	bool GetAnimEndFlag() { return m_isEndAnim; }
	//アニメションの進行率を返す//0から１の値を返す
	float GetAnimRate();

	float GetCurrentAnimTime() const{return m_currentAnimCount;}

private:
	//モデルのハンドル
	int m_modelHandle;
	//現在のアニメションハンドル
	int m_currentAnimHandle;
	//前のアニメションのハンドル
	int m_prevAnimHandle;

	//現在のアニメションのフレーム数
	float m_currentAnimCount;
	//前のアニメションのフレーム数
	float m_prevAnimCount;
	//アニメションを切り替えるフレーム数
	float m_animChangeFrame;
	//アニメションをループさせるかどうかのフラグ
	bool m_isRoop;
	//前のアニメションがループしているかどうかのフラグ
	bool m_prevRoop;
	//アニメーションがループしているかどうかのフラグ
	bool m_isEndAnim;
	//アニメーションの再生速度を管理するための変数
	float m_animTimeScale;
	//前のアニメーションの再生速度を管理するための変数
	float m_prevAnimTimeScale;
};

