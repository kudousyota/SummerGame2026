#include "Camera.h"
#include "DxLib.h"
#include "../System/CollisionManager.h"
#include "../Game/Stage.h"

namespace
{
	//注視点からカメラに向かうベクトル
	const Vector3 kTargetToCamera = VGet(0.0f, 200.0f, 800.0f);

	//注視点からカメラまでの距離
	constexpr float kCameraDistance = 300.0f;
	//カメラの高さ
	constexpr float kCameraHeight = -250.0f;

	//補間(一時的に1.0fにして追従を即時にすることで動作確認しやすくする)
	constexpr float kCameraFollow = 0.15f;

	constexpr float kStickDeadZone = 0.001f;
	constexpr float kCameraRotSpeedX = 0.8f;
	constexpr float kCameraRotSpeedY = 0.6f;
}

Camera::Camera() :
	m_cameraAngleX(0.0f),
	m_cameraAngleY(0.0f),
	m_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	m_cameraTarget(VGet(0.0f, 0.0f, 0.0f)),
	m_cameraTargetY(0.0f),
	m_skyDomeHandle(-1)
{
}

Camera::~Camera()
{
}

void Camera::Init(const Vector3& initialTarget, float initialAngle)
{
	
	m_skyDomeHandle = MV1LoadModel("Data/Sky_Evening.mv1");

	m_cameraAngleX = initialAngle;
	m_cameraAngleY = 0.0f;
	m_cameraTargetY = initialTarget.y;

	Vector3 offset;
	offset.x = sinf(m_cameraAngleX) * cosf(m_cameraAngleY) * kCameraDistance;
	offset.y = sinf(m_cameraAngleY) * kCameraDistance;
	offset.z = cosf(m_cameraAngleX) * cosf(m_cameraAngleY) * kCameraDistance;

	m_cameraTarget = initialTarget;
	m_cameraPos = initialTarget - offset;

	SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToDxLibVector(), m_cameraTarget.ToDxLibVector());
	MV1SetScale(m_skyDomeHandle, VGet(1.5f, 1.5f, 1.5f));
}

void Camera::End()
{
}
//targetpos追従する基準点
void Camera::Update(const Vector3& targetpos, const Vector3* lockonpos)
{

	if (lockonpos != nullptr)
	{
		Vector3 toTarget = *lockonpos - targetpos;
		toTarget.y = 0.0f;

		if (toTarget.SqMagnitude() > 0.0001f)
		{
			toTarget = toTarget.Normalize();
			m_cameraAngleX = atan2f(toTarget.x, toTarget.z);
		}
		//水平距離
		float horizontalDist = sqrtf((lockonpos->x - targetpos.x) * (lockonpos->x - targetpos.x) +
			(lockonpos->z - targetpos.z) * (lockonpos->z - targetpos.z));
		if (horizontalDist > 0.0001f)
		{
			m_cameraAngleY = atan2f(lockonpos->y - targetpos.y, horizontalDist) * 0.3f;
		}
		//ピッチの制限は通常時と同じものを適用しておく
		const float kMaxPitch = DX_PI_F * 0.4f;
		const float kMinPitch = -DX_PI_F * 0.2f;
		if (m_cameraAngleY > kMaxPitch) m_cameraAngleY = kMaxPitch;
		if (m_cameraAngleY < kMinPitch) m_cameraAngleY = kMinPitch;
	}
	else
	{


		int xBuf = 0, yBuf = 0;

		//右スティック入力取得
		GetJoypadAnalogInputRight(&xBuf, &yBuf, DX_INPUT_PAD1);
#ifdef _DEBUG


		DrawFormatString(256, 128, 0xffffff, "Right Stick X : %d", xBuf);
		DrawFormatString(256, 144, 0xffffff, "Right Stick Y : %d", yBuf);
		DrawFormatString(256, 160, 0xffffff, "CameraAngle : %.3f", m_cameraAngleX);
#endif //_DEBUG

		//正規化
		float stickX = static_cast<float>(xBuf) / 32767.0f;
		float stickY = static_cast<float>(yBuf) / 32767.0f;

		//デッドゾーン
		if (fabsf(stickX) < kStickDeadZone) stickX = 0.0f;
		if (fabsf(stickY) < kStickDeadZone) stickY = 0.0f;

		//カメラ左右回転
		if (stickX != 0.0f)
		{
			m_cameraAngleX += stickX * kCameraRotSpeedX;
		}
		if (stickY != 0.0f)
		{
			m_cameraAngleY -= stickY * kCameraRotSpeedY;
		}
	}
		const float kMaxPitch = DX_PI_F * 0.4f;
		const float kMinPitch = -DX_PI_F * 0.2f;
		//上下移動の制限
		if (m_cameraAngleY > kMaxPitch)
		{
			m_cameraAngleY = kMaxPitch;
		}

		if (m_cameraAngleY < kMinPitch)
		{
			m_cameraAngleY = kMinPitch;
		}

		//角度を-π～πに収める
		if (m_cameraAngleX > DX_PI_F)  m_cameraAngleX -= DX_TWO_PI_F;
		if (m_cameraAngleX < -DX_PI_F) m_cameraAngleX += DX_TWO_PI_F;

		//プレイヤー追従先
		Vector3 cameraTarget = targetpos;
		//注視点を下げる
		cameraTarget.y += -180.0f;

		//カメラの位置
		Vector3 offset;
		offset.x = sinf(m_cameraAngleX) * cosf(m_cameraAngleY) * kCameraDistance;
		offset.y = sinf(m_cameraAngleY) * kCameraDistance;
		offset.z = cosf(m_cameraAngleX) * cosf(m_cameraAngleY) * kCameraDistance;

		//プレイヤーの後ろから見るようにするなら「-offset」
		Vector3 idealCameraPos = cameraTarget - offset;


		Vector3 hitpos;

		//ステージとのレイ判定を行い、カメラがステージにめり込まないようにする
		if (CollisionManager::Instance().CheckCameraRay(
			m_pStage->GetModelHandle(),
			m_cameraTarget,
			m_cameraPos,
			hitpos))
		{
			Vector3 dir = (m_cameraTarget - hitpos).Normalize();
			m_cameraPos = hitpos + dir * 10.0f;
		}

		//追従
		m_cameraPos = m_cameraPos + (idealCameraPos - m_cameraPos) * kCameraFollow;
		m_cameraTarget = m_cameraTarget + (cameraTarget - m_cameraTarget) * kCameraFollow;

		//カメラ反映
		SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToDxLibVector(), m_cameraTarget.ToDxLibVector());

		//光
		Vector3 camPos = m_cameraPos.ToDxLibVector();
		Vector3 target = m_cameraTarget.ToDxLibVector();

		//カメラから注視点の方向
		Vector3 LightDir = VNorm(VSub(target, camPos));

		SetLightDirection(LightDir);

		MV1SetPosition(m_skyDomeHandle, m_cameraPos.ToDxLibVector());


	

	
}

void Camera::Draw()
{
	MV1DrawModel(m_skyDomeHandle);
}

Vector3 Camera::GetForward() const
{
	Vector3 forward = m_cameraTarget - m_cameraPos;
	forward.y = 0.0f;

	if (forward.SqMagnitude() > 0.0001f)
	{
		forward = forward.Normalize();
	}
	return forward;
}

Vector3 Camera::GetRight() const
{
	Vector3 forward = GetForward();
	Vector3 up = VGet(0.0f, 1.0f, 0.0f);
	Vector3 right = forward.Cross(up);

	if (right.SqMagnitude() > 0.0001f)
	{
		right = right.Normalize();
	}
	//間違っていたから反転
	right *= -1;

	return right;
}
