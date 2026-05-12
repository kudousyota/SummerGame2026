#pragma once
#include <memory>
#include "../system/Vector3.h"
class Player;
class Camera
{
public:
	Camera();
	~Camera();
	void Init();
	void End();
	void Update();
	void Draw();
	void SetPlayer(std::shared_ptr<Player> player) { m_pPlayer = player; }

	Vector3 GetForward() const;
	Vector3 GetRight() const;



private:
	//プレイヤーへの参照を保持
	std::shared_ptr<Player> m_pPlayer;

	//カメラの位置
	Vector3 m_cameraTarget;
	Vector3 m_cameraPos;

	//スカイドームのハンドル
	int m_skyDomeHandle;

	//カメラの角度
	//カメラの水平回転角度
	float m_cameraAngleX;
};

