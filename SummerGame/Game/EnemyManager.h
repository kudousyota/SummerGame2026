#pragma once
#include <vector>
#include <memory>
#include "../System/Vector3.h"

class Angel;
class Creature;
class Alien;
class Player;
class Stage;
class EnemyManager
{
public:
	//敵を管理するクラス
	EnemyManager();
	~EnemyManager();
	void Update();
	void Draw();

	//依存しているクラスのポインタを渡す
	void SetPlayer(std::shared_ptr<Player> player) { m_pPlayer = player; }
	void SetStage(std::shared_ptr<Stage> stage) { m_pStage = stage; }

	//ボスが死んだかどうか
	bool IsCreatureDead() const;

	//リセット
	void ResetCreatureDead();

	//敵の読み込み
	//敵を追加する関数//別々にすることによって,AngelとCreatureを一体だけ出したりなどができる
	void AddAngel(std::unique_ptr<Angel> angel,const Vector3& pos);
	void AddCreature(std::unique_ptr<Creature> creature,const Vector3& pos);
	void AddAlien(std::unique_ptr<Alien> alien, const Vector3& pos);
private:

	bool m_isCreatureDead = false;

	//敵のリスト
	std::vector<std::unique_ptr<Angel>> m_pAngels;
	std::vector<std::unique_ptr<Creature>> m_pCreatures;
	std::vector<std::unique_ptr<Alien>> m_pAlien;

	//依存しているクラスのポインタ
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Stage> m_pStage;
};

