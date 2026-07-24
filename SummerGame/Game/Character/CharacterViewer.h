#pragma once
#include "Vector3.h"
#include <string>
#include "../System/Animation.h"

//タイトルやリザルトで描画するキャラクター用
class CharacterViewer
{
public:
	CharacterViewer();
	~CharacterViewer();

	void Init(const int modelhandle, const std::string& animname);
	void Update();
	void Draw();

	void ChangeAnimation(const std::string& name,bool loop = true);
	void SetPosition(const Vector3& pos);
	void SetRotation(float angley);

private:
	int m_modelHandle;
	Vector3 m_pos;
	float m_animSpeed;
	Animation m_animation;
};

