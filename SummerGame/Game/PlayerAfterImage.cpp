#include "PlayerAfterImage.h"
#include <DxLib.h>
#include <algorithm>

namespace
{
    //残像の存在できるフレーム
    constexpr int kLifeFrame = 30;
}

PlayerAfterImage::PlayerAfterImage():
    m_playerModelHandle(-1)
{

}

PlayerAfterImage::~PlayerAfterImage()
{
    for (auto& afterImage : m_afterImages)
    {
        if (afterImage.animHandle != -1)
        {
            MV1DetachAnim(
                afterImage.modelHandle,
                afterImage.animHandle);

            afterImage.animHandle = -1;
        }

        if (afterImage.modelHandle != -1)
        {
            MV1DeleteModel(afterImage.modelHandle);
            afterImage.modelHandle = -1;
        }
    }

    m_afterImages.clear();
    //m_playerModelHandleは削除しない
}

void PlayerAfterImage::Init(int playerModelHandle)
{
    m_playerModelHandle = playerModelHandle;
}

void PlayerAfterImage::Create(const Vector3& pos, float angle, const std::string& animName, float animTime)
{
    if (m_playerModelHandle == -1)
    {
        return;
    }

    AfterImageData data;

    data.pos = pos;
    data.angle = angle;

    data.animName = animName;
    data.animTime = animTime;

    data.modelHandle = -1;
    data.animHandle = -1;
    data.life = 0.0f;

    //残像専用のモデルを作る
    data.modelHandle = MV1DuplicateModel(m_playerModelHandle);

    if (data.modelHandle == -1)
    {
        return;
    }

    //アニメーション番号を取得
    int animNo =
        MV1GetAnimIndex(data.modelHandle,data.animName.c_str());

    if (animNo == -1)
    {
        MV1DeleteModel(data.modelHandle);
        data.modelHandle = -1;
        return;
    }

    //アニメーションをアタッチ
    data.animHandle = MV1AttachAnim(data.modelHandle,animNo,-1,-1);

    if (data.animHandle == -1)
    {
        MV1DeleteModel(data.modelHandle);
        data.modelHandle = -1;
        return;
    }

    //保存していたフレームへ移動
    MV1SetAttachAnimTime(data.modelHandle,data.animHandle,data.animTime);

    //アニメーションを完全表示
    MV1SetAttachAnimBlendRate(data.modelHandle,data.animHandle,1.0f);

    //vectorに追加
    m_afterImages.push_back(data);
}
void PlayerAfterImage::Update()
{
    for (auto& afterImage : m_afterImages)
    {
        afterImage.life++;
    }

    for (auto it = m_afterImages.begin();
        it != m_afterImages.end();)
    {
        if (it->life < kLifeFrame)
        {
            ++it;
            continue;
        }

        //アニメーションをデタッチ
        if (it->animHandle != -1)
        {
            MV1DetachAnim(
                it->modelHandle,
                it->animHandle);

            it->animHandle = -1;
        }

        //残像専用モデルを削除
        if (it->modelHandle != -1)
        {
            MV1DeleteModel(it->modelHandle);
            it->modelHandle = -1;
        }

        //vectorから削除
        it = m_afterImages.erase(it);
    }
}

void PlayerAfterImage::Draw()
{
    for (const auto& afterImage : m_afterImages)
    {
        if (afterImage.modelHandle == -1)
        {
            continue;
        }

        MATRIX rot = MGetRotY(afterImage.angle);

        MATRIX trans =
            MGetTranslate(
                afterImage.pos.ToDxLibVector());

        MV1SetMatrix(
            afterImage.modelHandle,
            MMult(rot, trans));

        MV1DrawModel(afterImage.modelHandle);
    }
}

bool PlayerAfterImage::IsDead() const
{
    return m_afterImages.empty();
}
