#pragma once
#include <vector>
#include <string>

class EnemyManager;

class EnemySpawner
{
public:
	struct EnemyCreateInfo
	{
		std::string enemyName;
		float appearFrame;
		bool isCreated = false;
	};

	EnemySpawner();

	//とりあえずコード内で生成データを組み立てる
	void SetupCreateData();

	void Update(EnemyManager& manager, float currentFrame);

private:
	std::vector<EnemyCreateInfo> m_createInfoList;
};