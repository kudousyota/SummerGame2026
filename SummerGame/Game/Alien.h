#pragma once
#include "Enemy.h"
class Breath;
class Player;
class Alien : public Enemy
{
public:
	Alien();
	~Alien()override;
	void Init()override;
	void Update()override;
	void Draw()override;

	void OnHit(const AttackData& attackdata)override;

private:

	enum class AlienState
	{
		Idle,
		Move,
		Attack,
		Down,
		StandUp,
		Up,
		Hit,
	};

	//現在の状態
	AlienState m_currentState;

	//前回の状態
	AlienState m_prevState;

	//描画に使うモデルの垂直オフセット
	float m_modelDisplayOffsetY;

	//攻撃する場所
	Vector3 m_attackPos;

	//リグ
	int m_headBone;
	Breath* m_pBreath;

private:
	//攻撃処理
	void AttackUpdate();
	//当たり判定の位置を取得する関数
	Vector3 GetCollisionPosition() const override;
	void ChasePlayer(float rotateSpeed, float scale)override;
	void KickDown();

	void TransitionTo(AlienState nextState);



	
	//ダメージを受けたらDamageステートへ
	//void OnDamaged() override;

};