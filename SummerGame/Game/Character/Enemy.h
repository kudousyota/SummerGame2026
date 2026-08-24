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

    virtual float GetAttackRange()const  { return m_attackRange; }

    virtual AttackType GetAttackType() const = 0;
    virtual float GetAttackRadius()const = 0;
    virtual int GetScore()const = 0;
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
    //近づいたら気づくように
    float m_closeRange;
    float m_fov;
    float m_attackRange;

    //サーチ関連
    float m_baseSightRange;		//通常時の索敵距離(戻す用)
    float m_searchSightMultiplier;	//キョロキョロ中の索敵距離倍率
    float m_lookTimer;				//キョロキョロ経過時間
    float m_lookMaxTime;			//キョロキョロを続ける時間
    float m_lookSwingSpeed;		//首振りの速さ
    float m_lookSwingAngle;		//首振りの振れ幅(ラジアン)

    //タイムスケールの取得
    float m_timeScale;

    //スコア
    int m_score;

 

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
    //死亡時の処理(継承先で死亡アニメーションへの遷移などを行う)
    virtual void OnDead() { m_isDead = true; }
    //デバッグ
    void DrawDebugCollision() const;
    //指定位置まで移動
    virtual void MoveTo(const Vector3& target, float rotatespeed, float scale);
   

    std::shared_ptr<Player> m_pPlayer;
};

