#include "SceneMain.h"
#include "DxLib.h"
#include "../System/Input.h"
#include "../System/Timer.h"
#include "../DataLoader/DataManager.h"
#include "../System/Model.h"
#include "../System/ProjectileManager.h"
#include "GameClearedScene.h"
#include "SceneController.h"
#include "GameOverScene.h"
#include "../UI/UIManager.h"
#include "../UI/HPUI.h"
#include "../UI/WitchTimeNeedleUI.h"
#include "../Game.h"
#include "../Effect/EffectManager.h"
namespace
{
	constexpr float kRotateSpeed = DX_PI_F / 180.0f;
	constexpr int kFadeInterval = 60;
}


SceneMain::SceneMain(SceneController& controller):
Scene(controller),
m_frameCount(0)

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
	m_pCamera->SetStage(m_pStage);
	m_pCamera->Init(m_pPlayer->GetCameraTarget(),m_pPlayer->GetAngle());

	m_pPlayer->SetCamera(m_pCamera);
	m_pPlayer->SetStage(m_pStage);
	

	//敵の管理に依存するプレイヤーとステージをセット
	m_enemyManager.SetPlayer(m_pPlayer);
	m_enemyManager.SetStage(m_pStage);

	m_enemySpawner.SetupCreateData();
	//UI
	m_pUiManager = std::make_unique<UIManager>();
	//プレイヤーのHPUI
	auto hpUI = std::make_unique<HPUI>();
	hpUI->SetPlayerHP(m_pPlayer);
	m_pUiManager->Add(std::move(hpUI));
	//ウィッチタイムの針
	auto witchTimeUI = std::make_unique<WitchTimeNeedleUI>();
	witchTimeUI->SetPlayerWitchTime(m_pPlayer);
	m_pUiManager->Add(std::move(witchTimeUI));

	m_pUiManager->Init();

	EffectManager::Instns().Init();

	SetUseAlphaChannelGraphCreateFlag(true);

	
	
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
#ifdef _DEBUG
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
#endif // _DEBUG

	
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
	m_pCamera->Update(m_pPlayer->GetCameraTarget(), m_pPlayer->GetLockOnManager().GetLockOnPos());


	m_frameCount++;
	//敵の更新を任せる
	m_enemyManager.Update();

	m_pUiManager->Update();

	EffectManager::Instns().Update();
	//ボスを倒したら
	if (m_enemyManager.IsCreatureDead())
	{
		m_update = &SceneMain::FadeOutUpdate;
		m_draw = &SceneMain::FadeDraw;
		m_frame = 0;
		return;
	}
	//プレイヤーが死んだら
	if (m_pPlayer->IsDead())
	{
		m_update = &SceneMain::GameOverFadeOutUpdate;
		m_draw = &SceneMain::FadeDraw;
		m_frame = 0;
		return;
	}

	//敵のスポーンを管理するクラスに更新を任せる
	m_enemySpawner.Update(m_enemyManager, static_cast<float>(m_frameCount), m_pPlayer->GetPosition());

	ProjectileManager::Instance().Update();
}

void SceneMain::FadeOutUpdate(Input&)
{
	//ボスを倒すとゲームクリアシーンに行く
	if (m_frame++ >= kFadeInterval)
	{
		//フェードアウト完了
		m_finished = true;
		m_controller.ChangeScene(std::make_shared<GameClearedScene>(m_controller));
		return;
	}
}

void SceneMain::GameOverFadeOutUpdate(Input&)
{
	//プレイヤーが死ぬとゲームオーバーシーンに行く
	if (m_frame++ >= kFadeInterval)
	{
		//フェードアウト完了
		m_finished = true;
		m_controller.ChangeScene(std::make_shared<GameOverScene>(m_controller));
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

	m_pUiManager->Draw();

	EffectManager::Instns().Draw();

	ProjectileManager::Instance().Draw();



	DrawGrid();
	//DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	//DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", m_frameCount);
}

void SceneMain::FadeDraw()
{
	NormalDraw();

	auto rate = static_cast<float>(m_frame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * rate);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
