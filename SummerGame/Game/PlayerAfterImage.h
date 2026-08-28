#pragma once
#include "../Math/Vector3.h"
#include <string>
#include <vector>
//ウィッチタイムを発動した際の残像を描画する
class PlayerAfterImage
{
public:
	PlayerAfterImage();
	~PlayerAfterImage();

	//プレイヤーのモデルを元に初期化
	void Init(int playerModelHandle);

	void Update();

	void Draw();

	//プレイヤーの現在の状態を保存

	//分身を生成
	void Create(const Vector3& pos, float angle, const std::string& animName, float animTime);
	//消滅したか
	bool IsDead()const;
private:
	struct AfterImageData
	{
		Vector3 pos;
		float angle;

		std::string animName;
		float animTime;

		int modelHandle;
		int animHandle;
		int life;
	};

	//残像を管理
	std::vector<AfterImageData> m_afterImages;

	//元となるプレイヤーモデル
	int m_playerModelHandle;
};

