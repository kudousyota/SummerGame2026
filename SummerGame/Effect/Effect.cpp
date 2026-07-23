#include "Effect.h"
#include "EffekseerForDXLib.h"
#include "../Game.h"

void Effect::Play(int resourceHandle, const Vector3& pos)
{
	m_playingHandle = resourceHandle;

	m_pos = pos;

	SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.x, m_pos.y, m_pos.z);
}

void Effect::Stop()
{
	StopEffekseer3DEffect(m_playingHandle);
}

void Effect::Update()
{
	SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.x, m_pos.y, m_pos.z);
}

bool Effect::IsPlaying() const
{
	return IsEffekseer3DEffectPlaying(m_playingHandle);
}
