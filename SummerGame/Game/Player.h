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

private:

	int m_modelHandle;
	int m_jumpPower;
	bool m_isGround;

	//アニメーション系
	float m_currentAnimCount;
	float m_lastAnimCount;
	//アニメーションのハンドル
	int m_cureentAnimHandle;
	int m_lastAnimHandle;
	//アニメーション関数
	void AnimUpdate();

	//カメラ
	std::shared_ptr<Camera> m_pCamera;

};

