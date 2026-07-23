#include "Effect.h"
#include "EffekseerForDXLib.h"
#include "../Game.h"

void Effect::Play(int resourceHandle, const Vector3& pos)
{
	/*m_pos = pos;

	m_playingHandle = PlayEffekseer3DEffect(resourceHandle);

	SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.x, m_pos.y, m_pos.z);*/

	m_pos = pos;
	m_playingHandle = PlayEffekseer3DEffect(resourceHandle);
	SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.x, m_pos.y, m_pos.z);
	SetScalePlayingEffekseer3DEffect(m_playingHandle, 10.0f, 10.0f, 10.0f); //デバッグ用に極端に拡大
}

void Effect::Stop()
{
	//エフェクトの停止
	StopEffekseer3DEffect(m_playingHandle);
}

void Effect::Update()
{
	//エフェクトの位置を設定する
	SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.x, m_pos.y, m_pos.z);
}

bool Effect::IsPlaying() const
{
	//エフェクトが再生中かどうか
	return IsEffekseer3DEffectPlaying(m_playingHandle);
}
