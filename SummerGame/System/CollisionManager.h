#pragma once
#include <vector>
#include "Vector3.h"
class Character;
class CollisionManager
{
public:
	static CollisionManager& Instance();
	void Register(Character* character);
	void Unregister(Character* character);

	// 攻撃用の単純な判定
	void CheckAttackSphere(Character* attacker, const Vector3& pos, float radius, int damage);
	//ステージとの判定
	bool CheckStageCollision(Character* caracter,int stageHandle);
	//ステージの地面
	bool CheckStageGround(Character* character, int stageHandle, float& outGroundY);
private:
	std::vector<Character*> m_pCharacters;

};

