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

	CharacterType GetCharacterType() const override;

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
		SkyAttack,
		SkyRush,
		SkyKick,
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
	//void DodgeUpdate();

	
	//アタック
	bool m_isAttackHit;
	//次に行けるか
	bool m_isNextAttack;

	//攻撃する場所
	Vector3 m_attackPos;
	//向き
	Vector3 m_forward;
	//攻撃する向き
	Vector3 m_attackForward;
	//被弾したかどうか
	bool m_isHit;
	//ラッシュの攻撃が当たったかどうかを管理する配列
	bool m_rushHit[4];
	//通常移動
	Vector3 m_moveVelocity;
	//攻撃移動
	Vector3 m_attackVelocity;

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

	//攻撃で進む処理
	void MoveAttack(float distance);
	//攻撃時に方向を変えるための関数
	void TurnToInputDirection(const Vector3& right, const Vector3& forward);

	//カメラ
	std::shared_ptr<Camera>m_pCamera;

	//プレイヤーの当たり判定はカプセルで行う
	float GetCollisionRadius() const override { return 30.0f; }
	//プレイヤーの当たり判定はカプセルで行う
	float GetCollisionHeight() const override { return 100.0f; }

	//描画に使うモデルの垂直オフセット
	float m_modelDisplayOffsetY;
};

