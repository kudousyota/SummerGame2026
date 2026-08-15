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
public:
	int m_skyDomeHandle;
	Vector3 m_pos;
};

