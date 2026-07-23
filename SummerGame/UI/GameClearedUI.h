#pragma once
#include "UIBase.h"
#include <memory>
class Player;
class GameClearedUI : public UIBase
{
public:
	GameClearedUI();
	~GameClearedUI();
	void Init()override;
	void Update()override;
	void Draw()override;
	//プレイヤーを受け取ってメンバ変数に保存
	void SetPlayerHP(const std::shared_ptr<Player>& player) { m_pPlayer = player; }
private:
	//ハンドル
	int m_ResultUIHandle;
	int m_TimerUIHandle;
	int m_ScoreUIHandle;

	//プレイヤー
	std::weak_ptr<Player> m_pPlayer;
};

