#include "SceneMain.h"
#include "DxLib.h"
#include "../System/Input.h"
#include "../System/Timer.h"
#include "../DataLoader/DataManager.h"
#include "../System/Model.h"
#include "../System/ProjectileManager.h"
#include "GameClearedScene.h"
#include "SceneController.h"
namespace
{
	constexpr float kRotateSpeed = DX_PI_F / 180.0f;
	constexpr int kFadeInterval = 60;
}

void SceneMain::FadeInUpdate(Input&)
{

	if (m_frame-- <= 0)
	{
		m_update = &SceneMain::NormalUpdate;
		m_draw = &SceneMain::NormalDraw;
		m_frame = 0;	//念のためフレームを0にしておく
		return;
	}
}

void SceneMain::NormalUpdate(Input& input)
{
	
	Input::Instance().Update();

	Timer::Instance().Update();

	m_pPlayer->Update();
	m_pCamera->Update();


	m_frameCount++;
	//敵の更新を任せる
	m_enemyManager.Update();

	if (m_enemyManager.IsCreatureDead())
	{
		m_update = &SceneMain::FadeOutUpdate;
		m_draw = &SceneMain::FadeDraw;
		m_frame = 0;
		return;
	}
	//敵のスポーンを管理するクラスに更新を任せる
	m_enemySpawner.Update(m_enemyManager, static_cast<float>(m_frameCount), m_pPlayer->GetPosition());

	ProjectileManager::Instance().Update();
	//プレイヤーがウィッチタイムかどうかを知る
	bool currentwitch = m_pPlayer->GetWitchTime();
	//ウィッチタイムに入ったときに演出をする
	//今の状態&&前の状態
	if (currentwitch && !m_isPrevWitchTime)
	{
		//表示する時間
		m_witchEffectFrame = 80;
		//回転角度
		m_angle = 0.0f;
	}

	//次のフレームで判定するために保存
	m_isPrevWitchTime = currentwitch;
	//演出中だったら回転させる
	if (m_witchEffectFrame > 0)
	{
		//演出フレームをここで減らす
		m_witchEffectFrame--;

		//回転角度を増やす
		m_angle += DX_PI_F / 40.0f;

		//0から360度まで回転させる
		//360度以上にならないように
		if (m_angle > DX_TWO_PI_F)
		{
			m_angle -= DX_TWO_PI_F;
		}

	}
	

}

void SceneMain::FadeOutUpdate(Input&)
{
	if (m_frame++ >= kFadeInterval)
	{
		//フェードアウト完了
		m_finished = true;
		m_controller.ChangeScene(std::make_shared<GameClearedScene>(m_controller));
		return;
	}
}

void SceneMain::NormalDraw()
{
	m_pCamera->Draw();
	m_pStage->Draw();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);

	//ウィッチタイムだったら
	if (m_pPlayer->GetWitchTime())
	{
		DrawBox(0, 0, 1280, 720, GetColor(125, 0, 185), true);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	m_pPlayer->Draw();

	m_enemyManager.Draw();

	ProjectileManager::Instance().Draw();

	//ウィッチタイムだったら
	if (m_pPlayer->GetWitchTime())
	{
		//DrawBox(0, 0, 1280, 720, GetColor(255, 0, 255), true);
		if (m_witchEffectFrame > 0)
		{
			//画面の真ん中に回転する画像を描画する
			DrawRectRotaGraph2(1280 / 2, 720 / 2, 0, 0, 256, 512, 128, 256, 1.5f, m_angle, m_nidelHandle, true, false);
		}

	}

	DrawGrid();
	//DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	//DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", m_frameCount);
}

void SceneMain::FadeDraw()
{

}

SceneMain::SceneMain(SceneController& controller):
Scene(controller),
m_frameCount(0),
m_nidelHandle(-1),
m_angle(0.0f),
m_witchEffectFrame(0),
m_isPrevWitchTime(false)
{
	Init();
	m_update = &SceneMain::FadeInUpdate;
	m_draw = &SceneMain::FadeDraw;
	m_frame = kFadeInterval;
	m_finished = false;
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
	SetCameraNearFar(20.0f, 5000.0f);

	DataManager::GetInstance().LoadData();

	//先にPlayer生成
	m_pPlayer = std::make_shared<Player>();
	m_pPlayer->Init();

	m_pStage = std::make_shared<Stage>();
	m_pStage->Init();

	//次にCamera生成
	m_pCamera = std::make_shared<Camera>();
	m_pCamera->SetPlayer(m_pPlayer);
	m_pCamera->SetStage(m_pStage);
	m_pCamera->Init();

	m_pPlayer->SetCamera(m_pCamera);
	m_pPlayer->SetStage(m_pStage);
	

	

	//敵の管理に依存するプレイヤーとステージをセット
	m_enemyManager.SetPlayer(m_pPlayer);
	m_enemyManager.SetStage(m_pStage);

	m_enemySpawner.SetupCreateData();



	SetUseAlphaChannelGraphCreateFlag(true);

	Model::Instance().PreloadAll();
	//m_nidelHandle = LoadGraph("data/ui_niidle.png");
	
	m_nidelHandle = LoadGraph("data/ui_niidle_flower.png");
	//m_nidelHandle = LoadGraph("data/ui_niidle_test.png");
	
}

void SceneMain::Update(Input& input)
{
	(this->*m_update)(input);
}

void SceneMain::Draw()
{
	
	(this->*m_draw)();
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
