#include "DataManager.h"
#include <fstream>
#include <sstream>
#include <cassert>

void DataManager::LoadData()
{
	LoadSpawnData();
}

void DataManager::LoadSpawnData()
{
	m_spawnData.clear();

	std::ifstream file("Data/Csv/Spawn.csv");
	assert(file.is_open() && "Spwn.csvがない");

	std::string line;
	while(std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
		{
			//コメント行や空行はスキップ
			continue;
		}

		std::istringstream iss(line);
		std::string token;
		std::vector<std::string> tokens;
		//カンマで分割してトークンを取得
		while (std::getline(iss,token,','))
		{
			tokens.push_back(token);
		}
		//分割したトークンのベクターを生データとして保存
		m_spawnData.push_back(tokens);
	}

}