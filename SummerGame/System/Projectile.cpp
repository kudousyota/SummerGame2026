#include "Projectile.h"

Projectile::Projectile(const Vector3& pos, const Vector3& forward, float speed, int attackPower):
	m_pos(pos.x, pos.y, pos.z),
	m_prevPos(0.0f,0.0f,0.0f),
	m_forward(forward.x, forward.y, forward.z),
	m_speed(speed),
	m_attackPower(attackPower),
	m_lifeFrame(0),
	m_isDead(false)
{
}
