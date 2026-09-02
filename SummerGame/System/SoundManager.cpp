#include "SoundManager.h"
#include <DxLib.h>

SoundManager& SoundManager::Instance()
{
	static SoundManager instance;
	return instance;
}

SoundManager::SoundManager() :
	m_currentBgm(-1)
{
}

SoundManager::~SoundManager()
{
	//unordered_mapに入っているハンドルを全部解放
	for (const auto& bgm : m_bgmList)
	{
		DeleteSoundMem(bgm.second);
	}

	for (const auto& se : m_seList)
	{
		DeleteSoundMem(se.second);
	}
}

void SoundManager::Update()
{
	//必要に応じて、BGMやSEの状態を更新する処理をここに書く
}

void SoundManager::Load()
{
	//bgm
	m_bgmList["Title"] = LoadSoundMem("Data/Sound/Title.ogg");
	m_bgmList["Game"] = LoadSoundMem("Data/Sound/Game.ogg");
	m_bgmList["Cleare"] = LoadSoundMem("Data/Sound/Cleare.ogg");
	m_bgmList["Gameover"] = LoadSoundMem("Data/Sound/Gameover.ogg");

	//se
	m_seList["Ok"] = LoadSoundMem("Data/Sound/Ok2.mp3");
	m_seList["Cursor"] = LoadSoundMem("Data/Sound/Cursor.mp3");
	m_seList["Dodge"] = LoadSoundMem("Data/Sound/Dodge.mp3");
	m_seList["Just"] = LoadSoundMem("Data/Sound/Just.mp3");
	m_seList["WitchTimeStart"] = LoadSoundMem("Data/Sound/WitchTimeStart.mp3");
	m_seList["WitchTimeEnd"] = LoadSoundMem("Data/Sound/WitchTimeEnd.mp3");
	m_seList["Hit"] = LoadSoundMem("Data/Sound/Hit.mp3");
	m_seList["StrongHit"] = LoadSoundMem("Data/Sound/StrongHit.mp3");
	m_seList["NoHit"] = LoadSoundMem("Data/Sound/NoHit.mp3");
	m_seList["Dead"] = LoadSoundMem("Data/Sound/Dead.mp3");
	m_seList["Breath"] = LoadSoundMem("Data/Sound/Breath.mp3");
	m_seList["Shout"] = LoadSoundMem("Data/Sound/Shout.mp3");
	m_seList["CountDown"] = LoadSoundMem("Data/Sound/CountDown.mp3");
	m_seList["Landing"] = LoadSoundMem("Data/Sound/Landing.mp3");
}

void SoundManager::PlayBGM(const std::string& key, bool loop)
{
	//BGM再生処理
	//まず、鍵となる文字列からBGMのハンドルを取得する
	auto it = m_bgmList.find(key);
	//見つからなかった場合は何もしない
	if (it == m_bgmList.end())
	{
		return;
	}
	//現在再生中のBGMがあれば停止する
	if (m_currentBgm != -1)
	{
		StopSoundMem(m_currentBgm);
	}
	//新しいBGMを再生する
	m_currentBgm = it->second;
	if (m_currentBgm >= 0)
	{
		//音量調整
		ChangeVolumeSoundMem(100, m_currentBgm);
		//ループ再生の設定
		PlaySoundMem(m_currentBgm, loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK);
	}
}

void SoundManager::PlaySE(const std::string& key)
{
	//SE再生処理
	//鍵となる文字列からSEのハンドルを取得する
	auto it = m_seList.find(key);

	if (it != m_seList.end())
	{
		ChangeVolumeSoundMem(170, it->second);
		//SEはループさせないので、DX_PLAYTYPE_BACKを指定して再生する
		PlaySoundMem(it->second, DX_PLAYTYPE_BACK);
	}
}

void SoundManager::StopBGM()
{
	//現在再生中のBGMがあれば停止する
	if (m_currentBgm != -1)
	{
		//BGMを停止する
		StopSoundMem(m_currentBgm);
		//現在再生中のBGMのハンドルをリセットする
		m_currentBgm = -1;
	}
}