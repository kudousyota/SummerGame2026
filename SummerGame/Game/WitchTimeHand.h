#pragma once
#include <memory>
#include "../System/Vector3.h"
#include "../System/Animation.h"

class Player;
//ウィッチタイムの時に出現する手のクラス
class WitchTimeHand
{
public:
	WitchTimeHand();
	~WitchTimeHand();
	void Init();
	void Update();
	void Draw();
	//出現する関数と消える関数
	void Appear();
	void Disappear();

	//プレイヤーを知るための関数
	void SetPlayer(const std::shared_ptr<Player>& player) { m_pPlayer = player; }
	//位置を設定する関数
	void SetPosition(const Vector3& pos) { m_pos = pos; }
	//アクティブかどうかを外部から確認する関数
	bool IsActive() const { return m_isActive; }

private:
	//ハンドル
	int m_modelHandle;
	//位置
	Vector3 m_pos;
	//向き
	float m_angle;
	//アクティブかどうか
	bool m_isActive;
	//攻撃済みかどうかのフラグ
	bool m_hasAttacked;

	void AttackUpdate();

	//プレイヤーを知る
	std::weak_ptr<Player> m_pPlayer;
	//アニメーション
	Animation m_animation;
};

