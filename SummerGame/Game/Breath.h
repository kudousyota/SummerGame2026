#pragma once
#include "../System/Projectile.h"
#include "../System/AttackData.h"
class Breath: public Projectile
{
public:
	Breath(const Vector3& pos, const Vector3& forward, float speed, const AttackData& attack);

	
	void Update()override;
	void Draw()override;

	void Kill();
private:

	
};

