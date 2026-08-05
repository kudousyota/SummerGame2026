#pragma once
#include "../System/Vector3.h"
#include <vector>
#include <array>
#include <memory>

enum class EffectType
{
	Hit,
	Just,
	Breath,
	Max
};
class Effect;
class EffectManager
{
public:
	
	std::array<int, static_cast<int>(EffectType::Max)> m_resourceHandles;

	static EffectManager& Instns();
	void Init();
	void Update();
	void Draw();
	//指定した種類と座標でエフェクトを再生する関数を追加
	int PlayEffect(EffectType type, const Vector3& pos);

	//エフェクトを停止する関数を追加
	//void StopEffect(EffectType type);

	int GetResourceHandle(EffectType type) const;

private:
	//コンストラクタとデストラクタをプライベートにして、シングルトンパターンを実装
	EffectManager() = default;
	virtual ~EffectManager() = default;
	//コピーコンストラクタと代入演算子を削除して、シングルトンのインスタンスが複製されないようにする
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
	
	std::vector<std::unique_ptr<Effect>> m_effects;
};

