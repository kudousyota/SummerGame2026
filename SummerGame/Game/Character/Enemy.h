#pragma once
#include "Character.h"
class Player;
class Enemy : public Character
{
public:
    Enemy();
    ~Enemy()override;
    void Init()override;
    void Update()override = 0;
    void Draw()override = 0;

    virtual void ApplyDamage(int damage);
    CharacterType GetCharacterType() const override;

    void SetPlayer(std::shared_ptr<Player> player) { m_pPlayer = player; }
    bool IsDead() const override{ return m_isDead; }

    float GetCollisionRadius() const override { return m_collisionRadius; }
    float GetCollisionHeight() const override { return m_collisionHeight; }

    virtual AttackType GetAttackType() const = 0;
    virtual float GetAttackRadius()const = 0;
    AttackData CreateAttackData()const;
protected:
    //共通の値
    static constexpr float kAttackRange = 150.0f;

    int m_modelHandle;
    bool m_isDead;
    int m_attackCooldown;
    Vector3 m_forward;
    bool m_isAttacking;
    int m_attackFrame;
    bool m_isAttack;
    Vector3 m_attackDir;
    Vector3 m_scale;
    //見える距離
    float m_sightRange;
    float m_fov;

    //タイムスケールの取得
    float m_timeScale;

    //モデルの行列//ここはほかのとこでプレイヤーとまとめる可能性あり
    void UpdateModelMatrix();
    //プレイヤーが視野内にいるか
    bool CanSeePlayer();
    //索敵範囲のデバッグ描画
    void DrawDebugSight()const;
    //プレイヤーの方を向く(敵ごとに調整可能)
    virtual  void FacePlayer();
    //プレイヤーの方に移動(敵ごとに調整可能)
    virtual void ChasePlayer(float rotatespeed, float scale);
    //攻撃のクールタイム
    void UpdateCooldown(float scale);
    //ダメージ受けたときの処理
    virtual void OnDamaged() {}
    //デバッグ
    void DrawDebugCollision() const;
    //指定位置まで移動
    virtual void MoveTo(const Vector3& target, float rotatespeed, float scale);

    std::shared_ptr<Player> m_pPlayer;
};

