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
	//モデルの解放
	if (m_modelHandle != -1)
	{
		MV1DeleteModel(m_modelHandle);
	}
}

void CharacterViewer::Init(const int modelhandle, const std::string& animname, const Vector3& pos,const Vector3& scale)
{
	//表示するモデルを設定
	m_modelHandle = modelhandle;
	//初期アニメーションを設定
	m_animation.Init(m_modelHandle, animname, true, 0.5f);
	//場所とサイズ
	m_pos = pos;
	m_scale = scale;
}

void CharacterViewer::Update()
{
	//指定した位置までの直線的な動き
	UpdateLinearMove();
	//アニメーションを更新
	m_animation.Update(1.0f);
	//モデルの位置とサイズを更新
	MV1SetPosition(m_modelHandle, m_pos);
	MV1SetScale(m_modelHandle, m_scale);
}

void CharacterViewer::Draw()
{ 
	//モデルの位置を設定
	MV1SetPosition(m_modelHandle, m_pos);
	//常にスケールを設定して描画時のサイズ変化を防ぐ
	MV1SetScale(m_modelHandle, m_scale);
	//Y軸を中心にモデルの向きを設定する
	MV1SetRotationXYZ(m_modelHandle, Vector3(0.0f, m_angleY, 0.0f).ToDxLibVector());
	//モデルを描画
	MV1DrawModel(m_modelHandle);
}
void CharacterViewer::ChangeAnimation(const std::string& name, bool loop)
{
	//再生するアニメーションを変更
	m_animation.ChangeAnim(name, loop);
}

void CharacterViewer::StartLinearMove(const Vector3& startPos, const Vector3& endPos, float speed, float angle ,bool faceDirection,std::optional<Vector3>faceTarget)
{
	//直線移動の開始位置と終了位置を設定する
	m_moveStartPos = startPos;
	m_moveEndPos = endPos;
	//移動速度を設定する
	m_moveSpeed = speed;
	//移動開始位置を設定する
	m_pos = startPos;
	//初期の向きを設定する
	m_angleY = angle;
	//移動対象の方向向く
	m_isMoving = true;
	m_isMoveFinished = false;
	//移動方向を計算して向きを変える
	if (faceTarget.has_value())
	{
		Vector3 dir = (faceTarget.value() - startPos).Normalize();
		m_angleY = atan2f(dir.x, dir.z) + DX_PI_F;
	}
	//移動方向を向く
	else if (faceDirection)
	{
		Vector3 dir = (endPos - startPos).Normalize();
		m_angleY = atan2f(dir.x, dir.z) + DX_PI_F;
	}
}

void CharacterViewer::UpdateLinearMove()
{
	//移動中でない、または移動が完了している場合は何もしない
	if(!m_isMoving || m_isMoveFinished) return;
	//開始位置から終了位置までの計算
	Vector3 diff = m_moveEndPos - m_moveStartPos;
	//開始位置から終了位置までの距離の二乗を取得
	//距離そのものを計算するよりも高速に判定できる
	float totalLen = diff.SqMagnitude();
	//移動方向を正規化して取得
	Vector3 dir = diff.Normalize();
	//移動方向に沿ってキャラクターを移動
	m_pos = m_pos + dir * m_moveSpeed;
	//終了位置まで到達したか
	if((m_pos - m_moveStartPos).SqMagnitude() >= totalLen)
	{
		//終了位置に合わせる
		m_pos = m_moveEndPos;
		//直線移動を終了
		m_isMoveFinished = true;
		m_isMoving = false;
	}
}

