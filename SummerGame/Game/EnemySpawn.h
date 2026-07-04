#pragma once
#include <vector>
#include <string>
#include "../System/Vector3.h"

class EnemyManager;
class EnemySpawner
{
public:
	//
	enum class SpawnTriggerType
	{
		Frame,	//フレーム数で出現
		PlayerNear,	//プレイヤーが近づいたときに出現
	};

	//出現する敵の情報をまとめた構造体
	struct EnemyCreateInfo
	{
		std::string enemyName;//出現させる敵の名前(Angel/Creatureなど)
		SpawnTriggerType triggerType = SpawnTriggerType::Frame;	//出現条件の種類


		float appearFrame = 0.0f;//Frame用
		Vector3 triggerPos;//PlayerNear用(判定基準になる座標)
		float triggerRadius = 0.0f;//PlayerNear用(この半径に入ったら)

		std::vector<Vector3> appearPosList;//実際に敵が出る座標(複数可)
		bool isCreated = false;

	};

	EnemySpawner();

	//とりあえずコード内で生成データを組み立てる
	void SetupCreateData();

	void Update(EnemyManager& manager, float currentFrame, const Vector3& playerPos);

private:
	std::vector<EnemyCreateInfo> m_createInfoList;
	

};