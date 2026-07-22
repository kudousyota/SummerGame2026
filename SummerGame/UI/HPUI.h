#pragma once
#include "UIBase.h"
#include <memory>
class Player;
class HPUI : public UIBase
{
public:
	HPUI();
	~HPUI();
	void Init()override;
	void Update()override;
	void Draw()override;
	//プレイヤーを受け取ってメンバ変数に保存
	void SetPlayerHP(const std::shared_ptr<Player>& player) { m_pPlayer = player; }
private:
	//ハンドル
	int m_hpGaugeHandle;
	int m_hpGaugeBackHandle;
	int m_hplidHandle;
	
	//現在のHP
	int m_hp;
	//HPの最大値
	int m_maxHp;
	int m_hpWidth;
	//プレイヤー
	std::weak_ptr<Player> m_pPlayer;
};

