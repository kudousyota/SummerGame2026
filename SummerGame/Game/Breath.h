#pragma once
#include "../System/Projectile.h"

class Breath: public Projectile
{
public:
	Breath(const Vector3& pos, const Vector3& forward, float speed, int attackPower);

	
	void Update()override;
	void Draw()override;

	void Kill();
private:

	
};

