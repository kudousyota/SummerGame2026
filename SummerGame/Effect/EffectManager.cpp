#include "EffectManager.h"
#include "EffekseerForDXLib.h"
#include "Effect.h"

//シングルトンインスタンスを取得
EffectManager& EffectManager::Instns()
{
    static EffectManager effect;
    return effect;
}

void EffectManager::Init()
{
    //被ダメエフェクトのリソース読み込みハンドルを所持する
	m_resourceHandles[static_cast<int>(EffectType::Hit)] = LoadEffekseerEffect("Data/eff/Effect/Hit.efk");
}

void EffectManager::Update()
{
    //DXLibのカメラ・ライト情報をEffekseer側に同期
    Effekseer_Sync3DSetting();
    //再生中のエフェクトを更新し、再生終了したものはリストから削除//
    //it = イテレータ
    for (auto it = m_effects.begin(); it != m_effects.end();)
    {
        (*it)->Update();

        if (!(*it)->IsPlaying())
        {
            //再生が終了していたらリストから削除
            it = m_effects.erase(it);
        }
        else
        {
            //再生中なら次の要素へ
            ++it;
        }
    }
    //Effekseerの3D更新処理
    UpdateEffekseer3D();
}

void EffectManager::Draw()
{
    //エフェクシアの3Dエフェクトを描画
	DrawEffekseer3D();
}
//指定した種類と座標でエフェクトを再生する
void EffectManager::PlayEffect(EffectType type, const Vector3& pos)
{
    //指定したエフェクトのインスタンスを取得
	auto effect = std::make_unique<Effect>();
    //対応するリソースハンドルを使って指定座標で再生する
    effect->Play(GetResourceHandle(type),pos);
    //管理リストに追加して更新・削除の対象にする
    m_effects.push_back(std::move(effect));

}
//エフェクトの種類からリソースハンドルを取得
int EffectManager::GetResourceHandle(EffectType type) const
{
	return m_resourceHandles[static_cast<int>(type)];
}
