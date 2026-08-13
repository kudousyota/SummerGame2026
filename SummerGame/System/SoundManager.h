#pragma once
#include <unordered_map>
#include <string>

class SoundManager
{
public:
	//シングルトン
	static SoundManager& Instance();
	

	void Update();
	//ロード
	void Load();
	void PlayBGM(const std::string& key, bool loop);
	void PlaySE(const std::string& key);
	void StopBGM();
private:
	SoundManager();
	~SoundManager();
	//コピー、代入禁止
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator =(const SoundManager&) = delete;

	//BGMとSEのハンドルを管理するマップ
	std::unordered_map<std::string, int> m_bgmList;
	std::unordered_map<std::string, int> m_seList;

	//現在再生中のBGMのハンドル
	int m_currentBgm;
};

