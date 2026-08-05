#include "Effect.h"
#include "EffekseerForDXLib.h"
#include "../Game.h"

void Effect::Play(int resourceHandle, const Vector3& pos)
{
	/*m_pos = pos;

	m_playingHandle = PlayEffekseer3DEffect(resourceHandle);

	SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.x, m_pos.y, m_pos.z);*/
	m_resourceHandle = resourceHandle;
	m_pos = pos;
	m_playingHandle = PlayEffekseer3DEffect(resourceHandle);
	SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.x, m_pos.y, m_pos.z);
	//デバッグ用に極端に拡大
	SetScalePlayingEffekseer3DEffect(m_playingHandle, 20.0f, 20.0f, 20.0f);
}

void Effect::Stop()
{
	//エフェクトの停止
	if (m_playingHandle != -1)
	{
		StopEffekseer3DEffect(m_playingHandle);
		m_playingHandle = -1;
	}
}

void Effect::Update()
{
	if (m_playingHandle == -1)
	{
		return;
	}

	if (!IsEffekseer3DEffectPlaying(m_playingHandle))
	{
		m_playingHandle = -1;
	}
}

bool Effect::IsPlaying() const
{
	//エフェクトが再生中かどうか
	return IsEffekseer3DEffectPlaying(m_playingHandle);
}
