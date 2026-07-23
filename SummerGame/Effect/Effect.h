#pragma once
#include "../System/Vector3.h"
class Effect
{
public:
	void Play(int resourceHandle,const Vector3& pos);
	void Stop();
	void Update();

	bool IsPlaying()const;

	//void SetPos(const Vector3& pos) { m_pos = pos; }
private:
	//エフェクトの再生位置
	Vector3 m_pos;
	//再生中のエフェクトハンドル
	int m_playingHandle;
};

