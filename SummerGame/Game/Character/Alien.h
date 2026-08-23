#pragma once
#include "Enemy.h"

class Breath;
class Player;
class Effect;
class Alien : public Enemy
{
public:
	Alien();
	~Alien()override;
	void Init()override;
	void Update()override;
	void Draw()override;

	void OnHit(const AttackData& attackdata)override;

	AttackType GetAttackType() const override;
	float GetAttackRadius() const override;
	int GetScore() const override { return m_score; }

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
		Look,
		Search,//記憶位置まで移動
		LookAround,//到着後にキョロキョロする
		Dead
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
	//ブレス
	Breath* m_pBreath;
	// ブレスエフェクトの再生ハンドル
	int m_breathEffectHandle;
	//浮遊エフェクトの再生ハンドル
	int m_floatingEffectHandle;
	//浮遊エフェクトの位置
	Vector3 m_floatingEffectPos;
	Vector3 m_lastSeePos;


private:
	//攻撃処理
	void AttackUpdate();
	//当たり判定の位置を取得する関数
	Vector3 GetCollisionPosition() const override;
	void ChasePlayer(float rotateSpeed, float scale)override;
	void KickDown();

	void TransitionTo(AlienState nextState);

	Vector3 GetHitEffect()const override;
	void OnDead()override;


	//ダメージを受けたらDamageステートへ
	//void OnDamaged() override;

};