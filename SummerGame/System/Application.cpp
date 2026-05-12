#include "Application.h"
#include "DxLib.h"
#include <cassert>
#include "../System/Input.h"
//#include "../Scene/SceneController.h"
//#include "../Scene/TitleScene.h"
#include "../Scene/SceneMain.h"
#include "../Game.h"
#include "EffekseerForDXLib.h"


Application::Application()
{

}

Application::~Application()
{
}

Application& Application::GetInstance()
{
	//staticなのでメモリの場所が一つで確定
	static Application instance;
	return instance;

}

bool Application::Init()
{

	//ウィンドウモード設定
	ChangeWindowMode(true);
	//タイトル変更
	SetMainWindowText("nigeusagi");
	//画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);
	//フォントの読み込み
	AddFontResourceEx("data/x10y12pxDonguriDuel.ttf", FR_PRIVATE, NULL);

	if (DxLib_Init() == -1)
	{
		return false;
	}


	//DirectX11をしようするための初期化
	//Effecseerを使用するには必ず設定する
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	//Effekseer使用の初期化
	//引数には画面に表示する最大パーティクルを設定する
	if (Effekseer_Init(8000) == -1)
	{
		return -1;			//エラーが起きたら直ちに終了
	}

	//フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ
	SetChangeScreenModeGraphicsSystemResetFlag(false);




	//描画対象をバックバッファに変更
	SetDrawScreen(DX_SCREEN_BACK);
	//カリングの設定
	SetUseBackCulling(true);


	return true;
}

void Application::Run()
{
	Input input;
	//SceneController controller;
	//controller.ChangeScene(std::make_shared<TitleScene>(controller));

	//一旦SceneMainを表示する
	SceneMain sceneMain;
	sceneMain.Init();

	while (ProcessMessage() != -1 && !m_IsRequstendExit)
	{

		//このフレームの開始時間を取得
		LONGLONG start = GetNowHiPerformanceCount();

		ClearDrawScreen();
		input.Update();
		sceneMain.Update(input);
		sceneMain.Draw();

		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
		ScreenFlip();

		//フレームレート６０に固定
		while (GetNowHiPerformanceCount() - start < 16667)
		{

		}
	}
}

void Application::Terminate()
{
	//フォントの解放
	RemoveFontResourceEx("data/x10y12pxDonguriDuel.ttf", FR_PRIVATE, NULL);
	//Effekseer使用の終了処理
	Effkseer_End();
	DxLib_End();
}

void Application::RequestExit()
{
	m_IsRequstendExit = true;
}
