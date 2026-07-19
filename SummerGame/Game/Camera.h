#pragma once
#include <memory>
#include "../system/Vector3.h"
class Stage;
class Camera
{
public:
	Camera();
	~Camera();
	void Init(const Vector3& initialTarget, float initialAngle);
	void End();
	void Update(const Vector3& targetpos, const Vector3* lockonpos = nullptr);
	void Draw();
	void SetStage(std::shared_ptr<Stage> stage) { m_pStage = stage; }
	Vector3 GetForward() const;
	Vector3 GetRight() const;



private:
	//プレイヤーへの参照を保持
	std::shared_ptr<Stage> m_pStage;

	//カメラの位置
	Vector3 m_cameraTarget;
	Vector3 m_cameraPos;

	//スカイドームのハンドル
	int m_skyDomeHandle;

	//カメラの角度
	//カメラの水平回転角度
	float m_cameraAngleX;
	//カメラの垂直回転角度
	float m_cameraAngleY;

	float m_cameraTargetY;
};

