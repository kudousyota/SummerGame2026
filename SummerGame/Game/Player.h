#pragma once
#include "../System/Vector3.h"
#include "Character.h"

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
};

