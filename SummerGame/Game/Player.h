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
	
	enum class PlayerState
	{
		Idle,
		Walk,
		Attack,
		Avoid,
		Damage
	};

	// 現在の状態
	PlayerState m_currentState;

	// 前回の状態
	PlayerState m_prevState;

	int m_jumpPower;
	bool m_isGround;

	int m_hakutoHandle;

	
	//攻撃処理
	void AttackUpdate();
	//アタック
	bool m_isAttackHit;
	//次に行けるか
	bool m_isNextAttack;

	//攻撃する場所
	Vector3 m_attackPos;
	//向き
	Vector3 m_forward;

	void TransitionTo(PlayerState nextState);
	void EnterState();
	void ExitState();

	//カメラ
	std::shared_ptr<Camera>m_pCamera;
	float GetCollisionRadius() const override { return 30.0f; }
	float GetCollisionHeight() const override { return 100.0f; }

};

