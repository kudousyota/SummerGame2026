#pragma once
#include "UIBase.h"
#include <memory>
class Player;
class WitchTimeNeedleUI: public UIBase
{
public:
	WitchTimeNeedleUI();
	~WitchTimeNeedleUI();
	void Init()override;
	void Update()override;
	void Draw()override;

	//プレイヤーを受け取ってメンバ変数に保存
	void SetPlayerWitchTime(const std::shared_ptr<Player>& player) { m_pPlayer = player; }
private:
	//ハンドル
	int m_nidelHandle;

	//角度
	float m_angle;
	//フレームカウント
	int m_witchEffectFrame;
	//ウィッチタイムかどうか
	bool m_isPrevWitchTime;

	//プレイヤー
	std::weak_ptr<Player> m_pPlayer;
};

