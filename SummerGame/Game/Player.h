#pragma once
#include "../System/Vector3.h"
#include "Character.h"
#include <memory> 

class Camera;
class Input;
class Stage; 
class Player : public Character
{
public:
	Player();
	~Player() override;
	void Init()override; 
	void Update() override;
	void Draw() override;
	//ダメージを受ける関数
	void ApplyDamage(int damage) override;

	//カメラの注視点を取得する関数
	Vector3 GetCameraTarget()const;
	float GetAngle() const { return m_angle; }
	Vector3 GetPosition()const { return m_pos; }

	//ジャスト回避しているかどうか
	bool GetWitchTime() { return m_isWitchTime; }

	void SetCamera(std::shared_ptr<Camera> camera) { m_pCamera = camera; }

	float GetJustDodgeRadius() const override;
	bool IsJustDodgeWindow() const override;


private:
	
	enum class PlayerState
	{
		Idle,
		Walk,
		Attack,
		Rush,
		Kick,
		Damage,
		Jump,
		Dodge,
		Sky,
		WitchTime
	};

	// 現在の状態
	PlayerState m_currentState;

	// 前回の状態
	PlayerState m_prevState;

	int m_jumpPower;

	int m_hakutoHandle;

	int m_uiHandle;

	
	//攻撃処理
	void AttackUpdate();
	//回避処理
	void DodgeUpdate();

	
	//アタック
	bool m_isAttackHit;
	//次に行けるか
	bool m_isNextAttack;

	//攻撃する場所
	Vector3 m_attackPos;
	//向き
	Vector3 m_forward;
	//被弾したかどうか
	bool m_isHit;
	//ラッシュの攻撃が当たったかどうかを管理する配列
	bool m_rushHit[4];

	//死んだ
	bool m_isDead;
	//回避開始からの経過フレーム
	int m_dodgeFrame;
	//無敵時間
	int m_invincibleTime;
	//ウィッチタイムかどうか
	bool m_isWitchTime;
	
	void TransitionTo(PlayerState nextState);
	//ウィッチタイム
	void WitchTime();

	//カメラ
	std::shared_ptr<Camera>m_pCamera;

	//プレイヤーの当たり判定はカプセルで行う
	float GetCollisionRadius() const override { return 30.0f; }
	//プレイヤーの当たり判定はカプセルで行う
	float GetCollisionHeight() const override { return 100.0f; }

};

