#pragma once
#include "../System/Vector3.h"
#include "../System/Animation.h"
#include <memory>

//プレイヤーとエネミーの基底クラス
class Input;
class Stage;
class Character
{
public:
	Character();
	virtual ~Character();

	virtual void Init();
	virtual void Update() = 0;
	virtual void Draw() = 0;
	//当たり判定
	virtual void Collision();

	virtual void ApplyDamage(int damage);

	virtual Vector3 GetPosition() const { return m_pos; }

	// カプセルの半径（XZ方向）
	virtual float GetCollisionRadius() const;
	// カプセルの高さ（Y方向の長さ）
	virtual float GetCollisionHeight() const;

	virtual void SetPosition(const Vector3& pos) { m_pos = pos; }

	void SetStage(std::shared_ptr<Stage> stage);

protected:
	//共通のデータや関数を入れる
	int m_modelHandle;
	Animation m_animation;
	float m_speed;
	int m_hp;
	int m_attackPower;
	float m_gravity;
	float m_angle;
	Vector3 m_pos;
	Vector3 m_velocity;
	bool m_isGround;
	std::shared_ptr<Stage> m_pStage;
private:
	
	
};

