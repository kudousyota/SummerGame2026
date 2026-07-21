#pragma once
#include "Vector3.h"
class Character;
class LockOnManager
{
public:
	LockOnManager();
	void OnAttackHit(Character* hitcharacter);
	void Update();
	void ClearIfTarget(Character* character);
	bool IsLockOn() const;
	const Vector3* GetLockOnPos();
	
private:
	Character* m_pTarget = nullptr;
	//Œ©‚Ä‚¢‚éêŠ
	Vector3 m_cachadPos;
	int m_timerFrame;
};

