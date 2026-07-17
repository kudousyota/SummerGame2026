#pragma once
#include "../System/Vector3.h"
#include "../System/Animation.h"
#include <memory>
#include "../System/CharacterType.h"
#include "../System/AttackData.h"

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

	//純粋仮想関数
	virtual CharacterType GetCharacterType()const = 0;

	//当たり判定
	virtual void Collision();

	virtual void ApplyDamage(int damage);


	virtual Vector3 GetPosition() const { return m_pos; }

	//カプセルの半径(XZ方向)
	virtual float GetCollisionRadius() const { return m_collisionRadius; }
	//カプセルの高さ(Y方向の長さ)
	virtual float GetCollisionHeight() const { return m_collisionHeight; };

	//ジャスト回避用の半径を取得するための関数(通常の半径と同じにしとく)
	virtual float GetJustDodgeRadius() const { return GetCollisionRadius(); }
	//今ジャスト回避の受付中(Dodge状態で受付フレーム内)かどうか
	virtual bool IsJustDodgeWindow() const { return false; }

	virtual Vector3 GetCollisionPosition() const;

	virtual void SetPosition(const Vector3& pos) { m_pos = pos; }
	
	void SetStage(std::shared_ptr<Stage> stage);
	
	virtual void OnHit(const AttackData& attackdata);

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
	//カプセルの半径(XZ方向)
	float m_collisionRadius;
	//カプセルの高さ(Y方向の長さ)
	float m_collisionHeight;
	std::shared_ptr<Stage> m_pStage;
	
	
};

