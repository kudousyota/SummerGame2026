#pragma once
#include <memory>
#include "Character/Player.h"

//ウィッチタイムの時に出現する手のクラス
class WitchTimeHand : public Player
{
public:
	WitchTimeHand();
	~WitchTimeHand();
	void Init();
	void Update();
	void Draw();

	//プレイヤーを知るための関数
	void SetPlayer(std::shared_ptr<Player> player) { m_pPlayer = player; }

private:
	//ハンドル
	int m_modelHandle;
	//プレイヤーを知る
	std::weak_ptr<Player> m_pPlayer;
};

