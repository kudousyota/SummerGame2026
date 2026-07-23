#pragma once
#include "../System/Vector3.h"
#include <vector>
#include <array>
#include <memory>

enum class EffectType
{
	Hit,
	Just,
	Max
};
class Effect;
class EffectManager
{
public:
	void Init();
	void Update();
	void Draw();

	void PlayEffect(EffectType type, const Vector3& pos);

	int GetResourceHandle(EffectType type) const;

private:
	std::array<int, static_cast<int>(EffectType::Max)> m_resourceHandles;

	std::vector<std::unique_ptr<Effect>> m_effects;
};

