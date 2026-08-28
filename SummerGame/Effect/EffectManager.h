#pragma once
#include "../Math/Vector3.h"
#include <vector>
#include <array>
#include <memory>

enum class EffectType
{
	Hit,
	Just,
	Breath,
	Floating,
	Gameover,
	Hazard,
	Wind,
	Maho,
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
    //再生中のエフェクト停止とロード済みリソースの解放
    void Terminate();
	//指定した種類と座標でエフェクトを再生する関数を追加
	int PlayEffect(EffectType type, const Vector3& pos);
	//個別でエフェクトを停止する
	void StopEffect(int handle);
	//エフェクトを停止する
	void StopAll();

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

