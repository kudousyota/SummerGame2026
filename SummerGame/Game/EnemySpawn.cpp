#include "EnemySpawn.h"
#include "EnemyManager.h"
#include "Angel.h"
#include "Creature.h"

EnemySpawner::EnemySpawner()
{
}

void EnemySpawner::SetupCreateData()
{
	//とりあえずコード内で生成データを組み立てる
	//ここでは、敵の名前と出現フレームを設定する例を示します
	//いずれCSVで読み込みして管理したい
	m_createInfoList.push_back({ "Angel", 100.0f, false });
	m_createInfoList.push_back({ "Creature", 120.0f, false });
	m_createInfoList.push_back({ "Creature", 300.0f, false });
}

void EnemySpawner::Update(EnemyManager& manager, float currentFrame)
{
	for (auto& info : m_createInfoList)
	{
		//すでに生成済みの場合はスキップ
		if (info.isCreated)
		{
			continue;
		}
		//出現フレームに達していない場合はスキップ
		if (currentFrame < info.appearFrame)
		{
			continue;
		}
		if (info.enemyName == "Angel")
		{
			manager.AddAngel(std::make_unique<Angel>());
		}
		else if(info.enemyName == "Creature")
		{
			manager.AddCreature(std::make_unique<Creature>());
		}
		info.isCreated = true;
	}
}
