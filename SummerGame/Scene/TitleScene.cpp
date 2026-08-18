#include "TitleScene.h"
#include "DxLib.h"
#include "../Game.h"
//#include "../system/SoundManager.h"
#include "SceneMain.h"
#include "SceneController.h"
#include "Model.h"
#include "../System/Application.h"
#include "../System/SoundManager.h"
namespace
{
	constexpr int kFadeInterval = 60;
	constexpr int kUiPos = 590;
}


void TitleScene::FadeInUpdate(Input&)
{
	if (m_frame -- <= 0)
	{
		m_update = &TitleScene::NormalUpdate;
		m_draw	 = &TitleScene::NormalDraw;
		m_frame  = 0;//念のためフレームを0にしておく
		return;
	}
}

void TitleScene::NormalUpdate(Input& input)
{
	//回転
	m_SkyDome.Update(Vector3(0.0f, 0.0f, -5500.0f));

	for (auto& character : m_titleCharacter)
	{
		character.Update();
	}

	//エイリアンが端まで到達したら再スタート
	if (m_titleCharacter[kAlien].IsMoveFinished())
	{
		m_titleCharacter[kAlien].StartLinearMove(
			Vector3(-800.0f, 0.0f, -500.0f),
			Vector3(800.0f, 0.0f, -500.0f),
			5.0f
		);
	}

	//上下でカーソル移動
	if (input.IsTriggered("up") || input.IsTriggered("down"))
	{
		m_menuSelect = (m_menuSelect == MenuSelect::Start) ? MenuSelect::Exit : MenuSelect::Start;
	}

	if (input.IsTriggered("ok"))
	{
		//SoundManager::PlaySE("Ok");
		m_update = &TitleScene::FadeOutUpdate;
		m_draw = &TitleScene::FadeDraw;
		m_frame = 0;//フェードアウトの最初
		return;
	}
}

void TitleScene::FadeOutUpdate(Input&)
{
	if (m_frame++ >= kFadeInterval)
	{
		//フェードアウト完了
		m_finished = true;

		if (m_menuSelect == MenuSelect::Exit)
		{
			//ゲーム終了をApplicationに要求する
			Application::GetInstance().RequestExit();
		}
		else
		{
			m_controller.ChangeScene(std::make_shared<SceneMain>(m_controller));
		}
		return;
	}
}

void TitleScene::NormalDraw()
{
	m_SkyDome.Draw();
	for (auto& character : m_titleCharacter)
	{
		character.Draw();
	}

	const int white = GetColor(255, 255, 255);

	DrawRotaGraph(660, 150, 0.5, 0.0, m_logoHandle, true);

	const int intervar = 650;
	int now = GetNowCount();
	bool visible = (now / intervar) % 2;
	
	const int startColor = (m_menuSelect == MenuSelect::Start) ? GetColor(255, 255, 0) : white;
	const int exitColor = (m_menuSelect == MenuSelect::Exit) ? GetColor(255, 255, 0) : white;

	DrawStringToHandle(kUiPos, 560, "Start", startColor, m_fontHandle);
	DrawStringToHandle(kUiPos, 610, "Exit", exitColor, m_fontHandle);
}

void TitleScene::FadeDraw()
{
	NormalDraw();

	//値の範囲をいったん0.0~1.0にしておくといろいろと扱いやすくなります
	auto rate = static_cast<float>(m_frame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));//αブレンド
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);//画面全体に黒フィルムをかける
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);//ブレンドしない
}

TitleScene::TitleScene(SceneController& controller):
	Scene(controller),
	m_fontHandle(-1),
	m_skyHandle(-1),
	m_logoHandle(-1),
	m_logoPos(Vector3(0.0f, 0.0f, 0.0f))
{
	Init();
	m_update = &TitleScene::FadeInUpdate;
	m_draw = &TitleScene::FadeDraw;
	m_frame = kFadeInterval;
    m_finished = false;
}

void TitleScene::Init()
{	
	m_fontHandle = CreateFontToHandle("Constantia", 40, -1, DX_FONTTYPE_ANTIALIASING_EDGE);

	m_logoHandle = LoadGraph("data/kudonetta.png");

	m_logoPos = Vector3(130.0f, -270.0f, 0.0f);

	//BGMの再生
	SoundManager::Instance().PlayBGM("Title",true);

	m_SkyDome.Init();
	m_SkyDome.SetPos(Vector3(0.0f, 0.0f, -5500.0f));
	//ゆっくり回転させる
	m_SkyDome.SetRotSpeed(0.002f);

	m_SkyDome.SetScale(3.0f);

	//タイトルでCharacterを描画
	std::vector<CharacterInfo> infos =
	{
		{Model::Instance().CreatPlayerModel(), "Player|Title", Vector3(0.0f, 0.0f, -500.0f)},
		{Model::Instance().CreatAlienModel(),"Alien|Move",Vector3(300.0f,0.0f,1000.0f)},
		{Model::Instance().CreatAlienModel(),"Alien|Move",Vector3(300.0f,0.0f,-600.0f)},
	};
	m_titleCharacter.resize(infos.size());
	for (size_t i = 0; i < infos.size(); i++)
	{
		m_titleCharacter[i].Init(infos[i].modelHandle, infos[i].animName, infos[i].pos);
	}

	//1個目のキャラクターに移動量を設定する
	m_titleCharacter[kAlien].StartLinearMove(
		Vector3(800.0f, 0.0f, -500.f),//最初の場所
		Vector3(-800.0f, 0.0f, -500.0f),//目指す場所
		5.0f);//速度
	
	//カメラを初期化
	//カメラをタイトル用に初期化(SceneMainと同じ初期位置に戻す)
	//これでメインシーンから引き継いだカメラの位置がリセットされる
	SetCameraPositionAndTarget_UpVecY(Vector3(0.0f, 100.0f, -700.0f), Vector3(0.0f, 100.0f, 0.0f));
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	SetCameraNearFar(20.0f, 4500.0f);
}

void TitleScene::Update(Input& input)
{
	(this->*m_update)(input);
}

void TitleScene::Draw()
{
	//DrawString(0, 0, "TitleScene", GetColor(255, 255, 255));
	(this->*m_draw)();
}