#include "EnemySpawn.h"
#include "EnemyManager.h"
#include "Angel.h"
#include "Creature.h"

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
	//とりあえずコード内で生成データを組み立てる
	//ここでは、敵の名前と出現フレームを設定する例を示す
	//いずれCSVで読み込みして管理したい
	//時間経過で2体同時に出す例
	EnemyCreateInfo info1;
	info1.enemyName = "Creature";
	info1.triggerType = SpawnTriggerType::Frame;
	info1.appearFrame = 60.0f;
	info1.appearPosList =
	{
		Vector3(100.0f, kSpawnY, 200.0f),
		Vector3(-100.0f, kSpawnY, 200.0f)
	};
	m_createInfoList.push_back(info1);

	//プレイヤーが近づいたら出す
	EnemyCreateInfo info2;
	info2.enemyName = "Angel";
	info2.triggerType = SpawnTriggerType::PlayerNear;

	info2.triggerPos = Vector3(0.0f, 0.0f, 500.0f);
	info2.triggerRadius = kSpawnRadius;
	
	info2.appearPosList = { Vector3(0.0f, kSpawnY, 600.0f) };
	m_createInfoList.push_back(info2);

	EnemyCreateInfo info3;
	info3.enemyName = "Creature";
	info3.triggerType = SpawnTriggerType::PlayerNear;

	//プレイヤーが近づいたら出す
	info3.triggerPos = Vector3(-1010.0f, 0.0f, -5041.0f);
	info3.triggerRadius = kSpawnRadius;
	//出現座標
	info3.appearPosList = { Vector3(-1010.0f, kSpawnY, -5041.0f) };
	m_createInfoList.push_back(info3);
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
		}

		info.isCreated = true;
	}
}
