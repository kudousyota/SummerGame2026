#include "EnemyManager.h"
#include "Angel.h"
#include "Creature.h"
#include "Alien.h"
#include <algorithm>
EnemyManager::EnemyManager()
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Update()
{
	//敵の更新処理
	for(auto& angel : m_pAngels)
	{
		//angelの更新処理
		angel->Update();
	}

	for(auto& creature : m_pCreatures)
	{
		//creatureの更新処理
		creature->Update();
	}

	for (auto& alien : m_pAlien)
	{
		alien->Update();
	}
	//死んだ敵の削除処理
	m_pAngels.erase(std::remove_if(m_pAngels.begin(), m_pAngels.end(),
		[](const std::unique_ptr<Angel>& angel) {return angel->IsDead(); }),
		m_pAngels.end());

	m_pCreatures.erase(std::remove_if(m_pCreatures.begin(), m_pCreatures.end(),
		[](const std::unique_ptr<Creature>& creature) {return creature->IsDead(); }),
		m_pCreatures.end());

	m_pAlien.erase(std::remove_if(m_pAlien.begin(), m_pAlien.end(),
		[](const std::unique_ptr < Alien > & alien) {return alien->IsDead(); }),
		m_pAlien.end());
}

void EnemyManager::Draw()
{
	for(auto& angel : m_pAngels)
	{
		//angelの描画処理
		angel->Draw();
	}
	for (auto& creature : m_pCreatures)
	{
		//angelの描画処理
		creature->Draw();
	}
	for (auto& alien : m_pAlien)
	{
		alien->Draw();
	}
}

void EnemyManager::AddAngel(std::unique_ptr<Angel> angel,const Vector3& pos)
{
	//先に依存しているプレイヤーやステージを渡して初期化する
	angel->SetPlayer(m_pPlayer);
	angel->SetStage(m_pStage);
	angel->Init();
	angel->SetPosition(pos);
	//std::moveで所有権を移動させる
	m_pAngels.push_back(std::move(angel));
}

void EnemyManager::AddCreature(std::unique_ptr<Creature> creature, const Vector3& pos)
{
	creature->SetPlayer(m_pPlayer);
	creature->SetStage(m_pStage);
	creature->Init();
	creature->SetPosition(pos);

	m_pCreatures.push_back(std::move(creature));
}

void EnemyManager::AddAlien(std::unique_ptr<Alien> alien, const Vector3& pos)
{
	alien->SetPlayer(m_pPlayer);
	alien->SetStage(m_pStage);
	alien->Init();
	alien->SetPosition(pos);

	m_pAlien.push_back(std::move(alien));
}