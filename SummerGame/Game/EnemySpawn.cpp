#include "EnemySpawn.h"
#include "EnemyManager.h"
#include "Angel.h"
#include "Creature.h"
#include "Alien.h"
#include "../DataLoader/DataManager.h"

namespace
{
	constexpr float kSpawnRadius = 300.0f;	//敵の出現座標の半径
	constexpr float kSpawnY = 300.0f;	//敵の出現座標のY座標
}

EnemySpawner::EnemySpawner()
{
}

void EnemySpawner::SetupCreateData()
{
	auto& spawnData = DataManager::GetInstance().GetSpawnData();
	//敵の出現データをCSVから読み込んでEnemyCreateInfoに変換してm_createInfoListに格納する
	for (const auto& data : spawnData)
	{
		EnemyCreateInfo spawn;
		spawn.enemyName = data[0];
		spawn.triggerType = SpawnTriggerType::PlayerNear;
		spawn.triggerPos = Vector3(std::stof(data[2]),std::stof(data[3]),std::stof(data[4]));
		spawn.triggerRadius = std::stof(data[5]);
		spawn.appearPosList = {Vector3(std::stof(data[6]), std::stof(data[7]), std::stof(data[8]))};
		m_createInfoList.push_back(spawn);
	}

	
}

void EnemySpawner::Update(EnemyManager& manager, float currentFrame, const Vector3& playerPos)
{
	for (auto& info : m_createInfoList)
	{
		//すでに生成済みの場合はスキップ
		if (info.isCreated)
		{
			continue;
		}
		bool shouldSpawn = false;
		//出現フレームに達していない場合はスキップ
		switch (info.triggerType)
		{
		case SpawnTriggerType::Frame:
			{
				shouldSpawn = (currentFrame >= info.appearFrame);
				break;
			}
		case SpawnTriggerType::PlayerNear:
			{
				Vector3 diff = playerPos - info.triggerPos;
				float distSq = diff.SqMagnitude();
				shouldSpawn = (distSq <= info.triggerRadius * info.triggerRadius);
			}
		break;
		}

		if (!shouldSpawn)
		{
			continue;
		}
		//座標の数だけ敵を生成
		for (const auto& pos : info.appearPosList)
		{
			if (info.enemyName == "Angel")
			{
				manager.AddAngel(std::make_unique<Angel>(), pos);
			}
			else if (info.enemyName == "Creature")
			{
				manager.AddCreature(std::make_unique<Creature>(), pos);
			}
			else if (info.enemyName == "Alien")
			{
				manager.AddAlien(std::make_unique<Alien>(), pos);
			}
		}

		info.isCreated = true;
	}
}
