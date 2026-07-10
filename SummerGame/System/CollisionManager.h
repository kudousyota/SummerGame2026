#pragma once
#include <vector>
#include "Vector3.h"
#include "../Game/Character.h"
class Character;
class CollisionManager
{
public:
	static CollisionManager& Instance();
	void Register(Character* character);
	void Unregister(Character* character);

	//攻撃用の球判定
	void CheckAttackSphere(CharacterType attackerType, const Vector3& pos, float radius, int damage);
	//攻撃用のカプセル判定
	void CheckAttackCapsule(CharacterType attackerType, const Vector3& start, const Vector3& end, float radius, int damage);
	//ステージとの判定
	bool CheckStageWall(Character* caracter,int stagehandle);
	//ステージの地面
	bool CheckStageGround(Character* character, int stagehandle, float& outGroundY);
	//カメラとステージの判定
	bool CheckCameraRay(int stagehandle,const Vector3& start, const Vector3& end, Vector3& hitpos);

private:
	std::vector<Character*> m_pCharacters;

};

