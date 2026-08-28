#pragma once
#include <optional>
#include "../Math/Vector3.h"
#include <string>
#include "../System/Animation.h"


//ゲームシーン以外でで演出などをするキャラクター用
class CharacterViewer
{
public:
	CharacterViewer();
	~CharacterViewer();

	void Init(const int modelhandle, const std::string& animname,const Vector3&pos,const Vector3& scale);
	void Update();
	void Draw();

	void ChangeAnimation(const std::string& name,bool loop = true);
	void SetPosition(const Vector3& pos) { m_pos = pos; }
	void SetRotation(float angley) { m_angleY = angley; }
	void SetScale(const Vector3& scale) { m_scale = scale; }

	//直線的な動きをする
	void StartLinearMove(const Vector3& startPos,const Vector3& endPos,float speed,float angle,bool faceDirection = false,std::optional<Vector3>faceTarget = std::nullopt);
	bool IsMoveFinished()const { return m_isMoveFinished; }
	void UpdateLinearMove();
private:
	int m_modelHandle;
	Vector3 m_pos;
	float m_animSpeed;
	float m_angleY;
	Animation m_animation;
	Vector3 m_scale;

	//直線的な移動用
	bool m_isMoving;
	bool m_isMoveFinished;
	Vector3 m_moveStartPos;
	Vector3 m_moveEndPos;
	float m_moveSpeed;
};

