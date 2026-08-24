#include "CharacterViewer.h"
#include "DxLib.h"

CharacterViewer::CharacterViewer():
	m_modelHandle(-1),
	m_pos({ 0.0f,0.0f,0.0f }),
	m_angleY(0.0f),
	m_animSpeed(0.0f),
	m_isMoving(false),
	m_isMoveFinished(false),
	m_moveStartPos({ 0.0f,0.0f,0.0f }),
	m_moveEndPos({ 0.0f,0.0f,0.0f }),
	m_moveSpeed(0.0f),
	m_scale({0.0f,0.0f,0.0f})
{
}

CharacterViewer::~CharacterViewer()
{
	if (m_modelHandle != -1)
	{
		MV1DeleteModel(m_modelHandle);
	}
}

void CharacterViewer::Init(const int modelhandle, const std::string& animname, const Vector3& pos,const Vector3& scale)
{
	m_modelHandle = modelhandle;
	m_animation.Init(m_modelHandle, animname, true, 0.5f);
	m_pos = pos;
	m_scale = scale;
}

void CharacterViewer::Update()
{
	UpdateLinearMove();
	m_animation.Update(1.0f);

	MV1SetPosition(m_modelHandle, m_pos);
	MV1SetScale(m_modelHandle, m_scale);
}

void CharacterViewer::Draw()
{ 
	MV1SetPosition(m_modelHandle, m_pos);
	//常にスケールを設定して描画時のサイズ変化を防ぐ
	MV1SetScale(m_modelHandle, m_scale);
	MV1SetRotationXYZ(m_modelHandle, Vector3(0.0f, m_angleY, 0.0f).ToDxLibVector());
	MV1DrawModel(m_modelHandle);
}
void CharacterViewer::ChangeAnimation(const std::string& name, bool loop)
{
	m_animation.ChangeAnim(name, loop);
}

void CharacterViewer::StartLinearMove(const Vector3& startPos, const Vector3& endPos, float speed, float angle ,bool faceDirection,std::optional<Vector3>faceTarget)
{
	m_moveStartPos = startPos;
	m_moveEndPos = endPos;
	m_moveSpeed = speed;
	m_pos = startPos;
	m_angleY = angle;
	m_isMoving = true;
	m_isMoveFinished = false;
	//移動方向を計算して向きを変える
	if (faceTarget.has_value())
	{
		Vector3 dir = (faceTarget.value() - startPos).Normalize();
		m_angleY = atan2f(dir.x, dir.z) + DX_PI_F;
	}
	else if (faceDirection)
	{
		Vector3 dir = (endPos - startPos).Normalize();
		m_angleY = atan2f(dir.x, dir.z) + DX_PI_F;
	}
}

void CharacterViewer::UpdateLinearMove()
{
	if(!m_isMoving || m_isMoveFinished) return;

	Vector3 diff = m_moveEndPos - m_moveStartPos;
	//大きさを返す
	float totalLen = diff.SqMagnitude();
	Vector3 dir = diff.Normalize();
	m_pos = m_pos + dir * m_moveSpeed;
	if((m_pos - m_moveStartPos).SqMagnitude() >= totalLen)
	{
		m_pos = m_moveEndPos;
		m_isMoveFinished = true;
		m_isMoving = false;
	}
}

