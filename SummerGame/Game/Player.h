#pragma once
#include "../System/Vector3.h"
#include "Character.h"
#include <memory> 

class Camera;
class Input;
class Player : public Character
{
public:
	Player();
	~Player() override;
	void Init()override; 
	void Update(const Input& input) override;
	void Draw() override;

	//カメラの注視点を取得する関数
	Vector3 GetCameraTarget()const;
	float GetAngle() const { return m_angle; }

	void SetCamera(std::shared_ptr<Camera> camera) { m_pCamera = camera; }

private:
	enum class Inputdata
	{
		None,
		Up,
		Down,
		Left,
		Right,
		Jump,
		Attack
	};

	Inputdata m_inputState;

	int m_modelHandle;
	int m_jumpPower;
	bool m_isGround;

	
	//入力されているか
	bool m_isInput;

	//アニメーション系
	float m_currentAnimCount;
	float m_lastAnimCount;
	//アニメーションのハンドル
	int m_cureentAnimHandle;
	int m_lastAnimHandle;
	//現在セットしているアニメーションのインデックス
	int m_currentAnimIndex;
	//アニメーション切り替えのフレーム数
	int m_animChangeFrame;
	//アニメーション関数
	void AnimUpdate();
	//攻撃処理
	void AtackUpdate(const Input& input);
	//アタック
	bool m_isAttack;
	//向き
	Vector3 m_forward;

	//カメラ
	std::shared_ptr<Camera>m_pCamera;

};

