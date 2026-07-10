#include "Projectile.h"

Projectile::Projectile(const Vector3& pos, const Vector3& forward, float speed, int attackPower):
	m_pos(0.0f,0.0f,0.0f),
	m_prevPos(0.0f,0.0f,0.0f),
	m_forward(0.0f,0.0f,0.0f),
	m_speed(0.0f),
	m_attackPower(0),
	m_lifeFrame(0),
	m_isDead(false)
{
}
