#include "SceneMain.h"
#include "DxLib.h"
#include "../System/Input.h"

namespace
{
	constexpr float kRotateSpeed = DX_PI_F / 180.0f;
}

SceneMain::SceneMain():
m_frameCount(0),
m_nidelHandle(-1),
m_angle(0.0f)
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init()
{
	// Zバッファの設定
	SetUseZBuffer3D(true);	// Zバッファを使います
	SetWriteZBuffer3D(true);	// 描画する物体はZバッファにも距離を書き込む

	// カメラの設定
	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 300.0f, -700.0f), VGet(0.0f, 0.0f, 0.0f));
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	SetCameraNearFar(200.0f, 1500.0f);

	// 先にPlayer生成
	m_pPlayer = std::make_shared<Player>();
	m_pPlayer->Init();

	// 次にCamera生成
	m_pCamera = std::make_shared<Camera>();
	m_pCamera->SetPlayer(m_pPlayer);
	m_pCamera->Init();

	m_pPlayer->SetCamera(m_pCamera);

	auto enemy = std::make_shared<Enemy>();
	enemy->Init();
	enemy->SetPlayer(m_pPlayer);
	m_pEnemy.push_back(enemy);

	m_pStage = std::make_shared<Stage>();
	m_pStage->Init();
	m_pPlayer->SetStage(m_pStage);

	for (auto& enemy : m_pEnemy)
	{
		enemy->SetStage(m_pStage);
	}
	SetUseAlphaChannelGraphCreateFlag(TRUE);
	//m_nidelHandle = LoadGraph("data/ui_niidle.png");
	
	m_nidelHandle = LoadGraph("data/ui_niidle_flower.png");
	//m_nidelHandle = LoadGraph("data/ui_niidle_test.png");
	
}

void SceneMain::Update(Input& input)
{
	Input::Instance().Update();

	

	m_frameCount++;
	m_pPlayer->Update();
	m_pCamera->Update();

	if (!m_pEnemy.empty())
	{
		for (auto& enemy : m_pEnemy)
		{
			if (enemy)
			{
				enemy->Update();
			}
		}
	}
	m_angle += kRotateSpeed;
	//回転角度の最大値(360度 = 2πラジアン)
	const float kMaxAngle = DX_TWO_PI_F;
	//まだ1週してなければ回転
	if (m_angle < kMaxAngle)
	{
		
		m_angle -= DX_PI_F / 180.0f;
	}

	//360度超えないようにする
	if (m_angle > kMaxAngle)
	{
		m_angle = kMaxAngle;
	}

	if (m_pPlayer->GetWitchTime())
	{
		// 360度→0度まで逆回転
		if (m_angle > 0.0f)
		{
			m_angle -= DX_PI_F / 30.0f;

			if (m_angle < 0.0f)
			{
				m_angle = 0.0f;
			}
		}
	}
	else
	{
		m_angle = DX_TWO_PI_F;
	}
}

void SceneMain::Draw()
{

	//ウィッチタイムだったら
	if (m_pPlayer->GetWitchTime())
	{
		DrawBox(0,0,1280,720,GetColor(255,0, 255),true);
		//0.5f0.5fで真ん中
		DrawBillboard3D(VGet(0.0f, 300.0f, 30.0f), 0.5f, 0.5f, 320.0f,/*0.0f*/ m_angle, m_nidelHandle, true);

	}
	//SetUseAlphaChannelGraphCreateFlag(TRUE);

	m_pPlayer->Draw();
	m_pStage->Draw();

	

	if (!m_pEnemy.empty())
	{
		for (auto& enemy : m_pEnemy)
		{
			if (enemy)
			{
				enemy->Draw();
			}
		}
	}

	DrawGrid();
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", m_frameCount);
}

void SceneMain::DrawGrid()
{
	// 直線の始点と終点
	VECTOR startPos;
	VECTOR endPos;

	for (int z = -300; z <= 300; z += 100)
	{
		startPos = VGet(-300.0f, 0.0f, static_cast<float>(z));
		endPos = VGet(300.0f, 0.0f, static_cast<float>(z));
		DrawLine3D(startPos, endPos, 0xff0000);
	}
	for (int x = -300; x <= 300; x += 100)
	{
		startPos = VGet(static_cast<float>(x), 0.0f, -300.0f);
		endPos = VGet(static_cast<float>(x), 0.0f, 300.0f);
		DrawLine3D(startPos, endPos, 0x0000ff);
	}
}
