#pragma once
#include <vector>
#include "Vector3.h"
#include "../Game/Character.h"
class Character;
class AttackData;
class CollisionManager
{
public:
	static CollisionManager& Instance();
	void Register(Character* character);
	void Unregister(Character* character);

	//攻撃用の球判定　//キャラクター事の攻撃当たり判定
	std::vector<Character*> CheckAttackSphere(const AttackData& attackdata, const Vector3& pos);
	//攻撃用のカプセル判定
	std::vector<Character*> CheckAttackCapsule(const AttackData& attackdata, const Vector3& start, const Vector3& end);
	//ステージとの判定
	bool CheckStageWall(Character* caracter,int stagehandle);
	//ステージの地面
	bool CheckStageGround(Character* character, int stagehandle, float& outGroundY);
	//カメラとステージの判定
	bool CheckCameraRay(int stagehandle,const Vector3& start, const Vector3& end, Vector3& hitpos);

private:
	std::vector<Character*> m_pCharacters;

};

