#include "ProjectileManager.h"
#include "../System/Projectile.h"
#include <algorithm>

ProjectileManager& ProjectileManager::Instance()
{
	static ProjectileManager manager;
	return manager;
}

void ProjectileManager::Add(std::unique_ptr<Projectile> projectile)
{
	m_pProjectiles.push_back(std::move(projectile));
}

void ProjectileManager::Update()
{
	for (auto& projectile : m_pProjectiles)
	{
		projectile->Update();
	}

	m_pProjectiles.erase(
		std::remove_if(
			m_pProjectiles.begin(),
			m_pProjectiles.end(),
			[](const std::unique_ptr<Projectile>& projectile)
			{
				return projectile->IsDead();
			}),
		m_pProjectiles.end()
	);
}

void ProjectileManager::Draw()
{
	for (auto& projectile : m_pProjectiles)
	{
		projectile->Draw();
	}

}