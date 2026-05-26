#include "Animation.h"
#include "DxLib.h"
#include "Timer.h"
namespace
{
	//アニメションを切り替えるフレーム数
	constexpr int kAnimChangeFrame = 10;
}

Animation::Animation():
	m_modelHandle(-1),
	m_currentAnimHandle(-1),
	m_prevAnimHandle(-1),
	m_currentAnimCount(0.0f),
	m_prevAnimCount(0.0f),
	m_animChangeFrame(0),
	m_isRoop(false),
	m_prevRoop(false),
	m_isEndAnim(false),
	m_animTimeScale(1.0f),
	m_prevAnimTimeScale(0.0f)
{
}

Animation::~Animation()
{
	//モデルをメモリから解放する
	MV1DeleteModel(m_modelHandle);
}

void Animation::Init(int modelHandle, std::string name, bool isRoop, float timeScale)
{
	SetAnim(isRoop);
	//モデルのハンドルを保存する
	m_modelHandle = modelHandle;

	//animation名からアニメーションのインデックスを取得
	int animNo = MV1GetAnimIndex(modelHandle, name.c_str());

	//m_animHandleに入れることで,アニメーションを変えるときや、ブレンドするときにm_animHandleを使う
	//アニメーションをアタッチする
	m_currentAnimHandle = MV1AttachAnim(m_modelHandle, animNo, -1, -1);
	//アニメションのフレーム数0を0にする
	m_currentAnimCount = 0.0f;
	//前のアニメションのハンドルを-1にする
	m_prevAnimHandle = -1;
	//前のアニメションのフレーム数を0にする
	m_animTimeScale = timeScale;
}

void Animation::Update()
{

}

void Animation::AnimBlend()
{

}

void Animation::SetAnim(bool isRoop)
{
	m_prevRoop = m_isRoop;
	//ループするかどうかの保存
	m_isRoop = isRoop;
	//アニメーションの終了フラグを初期化
	m_isEndAnim = false;
}

void Animation::ChangeAnim(std::string name, bool isRoop, float timescale)
{

}

float Animation::GetAnimRate()
{

}