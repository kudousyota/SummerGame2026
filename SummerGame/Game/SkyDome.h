#pragma once
#include "../System/Vector3.h"
#include "Camera.h"
class Camera;
class SkyDome
{
public:
	SkyDome();
	~SkyDome();

	void Init();
	//ˆÊ’u‚¾‚¯Žó‚¯Žæ‚é
	void Update(const Vector3& cameraPos);
	void Draw();

	void SetPos(const Vector3& pos);
	void SetScale(float scale);
	//‰ñ“]‘¬“x‚ð‚¢‚¶‚ê‚é‚æ‚¤‚É‚·‚é
	void SetRotSpeed(float speed);
public:
	int m_skyDomeHandle;
	Vector3 m_pos;
	float m_scale;
	float m_angleY;
	float m_rotSpeed;
};

