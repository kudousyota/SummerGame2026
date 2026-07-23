#include "EffectManager.h"
#include "EffekseerForDXLib.h"
#include "Effect.h"

EffectManager& EffectManager::Instns()
{
    static EffectManager effect;
    return effect;
}

void EffectManager::Init()
{
	m_resourceHandles[static_cast<int>(EffectType::Hit)] = LoadEffekseerEffect("Data/eff/HIt3.efk");
}

void EffectManager::Update()
{
    for (auto it = m_effects.begin(); it != m_effects.end();)
    {
        (*it)->Update();

        if (!(*it)->IsPlaying())
        {
            it = m_effects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void EffectManager::Draw()
{
	DrawEffekseer3D();
}

void EffectManager::PlayEffect(EffectType type, const Vector3& pos)
{
	auto effect = std::make_unique<Effect>();

    effect->Play(GetResourceHandle(type),pos);

    m_effects.push_back(std::move(effect));

}

int EffectManager::GetResourceHandle(EffectType type) const
{
	return m_resourceHandles[static_cast<int>(type)];
}
