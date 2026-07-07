#pragma once
#include <vector>
#include <string>
class DataManager
{
public:
	static DataManager& GetInstance()
	{
		static DataManager instance;
		return instance;
	}
	// データの読み込み
	void LoadData();

	//敵のスポーンデータを返す
	const std::vector<std::vector<std::string>>& GetSpawnData() const{return m_spawnData;}

private:
	// コンストラクタを非公開にしてシングルトンを実現
	DataManager() = default;
	~DataManager() = default;
	DataManager(const DataManager&) = delete;
	DataManager& operator=(const DataManager&) = delete;

	//ロードする関数
	//敵の出現データをロードする
	void LoadSpawnData();

	//敵の出現データを格納する二次元配列
	std::vector<std::vector<std::string>> m_spawnData;
};

