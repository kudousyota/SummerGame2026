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
	const Vector3 kCameraPos = Vector3(0.0f, 100.0f, -700.0f); //タイトルのカメラ位置
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
	for (size_t i = 0; i < m_alienMoveInfos.size(); i++)
	{
		size_t index = AlienStart + i;

		if (m_titleCharacter[index].IsMoveFinished())
		{
			StartAlienMove(i);
		}
	}

	m_titleUI.Update(input);

	if (m_titleUI.IsDecided())
	{
		m_titleUI.ResetDecided();
		m_update = &TitleScene::FadeOutUpdate;
		m_draw = &TitleScene::FadeDraw;
		m_frame = 0;
	}
}

void TitleScene::FadeOutUpdate(Input&)
{
	if (m_frame++ >= kFadeInterval)
	{
		//フェードアウト完了
		m_finished = true;

		SoundManager::Instance().StopBGM();

		if (m_titleUI.GetMenuSelect() == TitleUI::MenuSelect::Exit)
		{
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

	m_titleUI.Draw();
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
	m_skyHandle(-1),
	m_logoPos(Vector3(0.0f, 0.0f, 0.0f))
{

	m_update = &TitleScene::FadeInUpdate;
	m_draw = &TitleScene::FadeDraw;
	m_frame = kFadeInterval;
    m_finished = false;
}

void TitleScene::Init()
{	
	//BGMの再生
	SoundManager::Instance().PlayBGM("Title",true);

	m_titleUI.Init();

	m_SkyDome.Init();
	m_SkyDome.SetPos(Vector3(0.0f, 0.0f, -5500.0f));
	//ゆっくり回転させる
	m_SkyDome.SetRotSpeed(0.002f);
	//サイズ
	m_SkyDome.SetScale(3.0f);
	//場所を確定させる
	m_SkyDome.Update(Vector3(0.0f, 0.0f, -5500.0f));

	//タイトルでCharacterを描画
	std::vector<CharacterInfo> infos =
	{
		{Model::Instance().CreatPlayerModel(), "Player|Title", Vector3(0.0f, 0.0f, -500.0f),Vector3(1.0f,1.0f,1.0f)},
		{Model::Instance().CreatAlienModel(),"Alien|Move",Vector3(0.0f,0.0f,0.0f),Vector3(1.0f,1.0f,1.0f)},
		{Model::Instance().CreatAlienModel(),"Alien|UP",Vector3(0.0f,0.0f,0.0f),Vector3(1.0f,1.0f,1.0f)},
		{Model::Instance().CreatAlienModel(),"Alien|Move",Vector3(0.0f,0.0f,0.0f),Vector3(1.0f,1.0f,1.0f)},
	};
	m_titleCharacter.resize(infos.size());
	for (size_t i = 0; i < infos.size(); i++)
	{
		m_titleCharacter[i].Init(infos[i].modelHandle, infos[i].animName, infos[i].pos,infos[i].scale);
	}

	//エイリアンごとの往復移動を設定する
	//増やしたいときはここに追加するだけ
	m_alienMoveInfos =
	{
		//1体目
		{Vector3(800.0f,0.0f,-250.0f),Vector3(-800.0f,0.0f,-250.0f),5.0f,0.0f,false},
		//2体目
		{Vector3(300.0f,-600.0f,-300.0f),Vector3(300.0f,300.0f,-300.0f),3.5f,DX_PI_F,true},
		//3体目
		{Vector3(800.0f,-200.0f,-300.0f),Vector3(-300.0f,300.0f,-300.0f),2.5f,0.0f,true},
	};
	//初期移動
	for (size_t i = 0; i < m_alienMoveInfos.size(); i++)
	{
		StartAlienMove(i);
	}
	
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

void TitleScene::StartAlienMove(size_t index)
{
	const auto& moveInfo = m_alienMoveInfos[index];
	m_titleCharacter[AlienStart + index].StartLinearMove(
		moveInfo.start,
		moveInfo.end,
		moveInfo.speed,
		moveInfo.angle,
		!moveInfo.faceCamera,//faceCameraでないときだけ移動方向を向く
		moveInfo.faceCamera ? std::optional<Vector3>(kCameraPos) : std::nullopt
	);
}