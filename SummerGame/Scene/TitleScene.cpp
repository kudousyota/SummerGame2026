#include "TitleScene.h"
#include "DxLib.h"
#include "../Game.h"
//#include "../system/SoundManager.h"
#include "SceneMain.h"
#include "SceneController.h"
namespace
{
	constexpr int fade_interval = 60;
	const char* const kWaveAnimName = "CharacterArmature|Wave";
}


void TitleScene::FadeInUpdate(Input&)
{
	

	if (m_frame-- <= 0)
	{
		m_update = &TitleScene::NormalUpdate;
		m_draw = &TitleScene::NormalDraw;
		m_frame = 0;	//念のためフレームを0にしておく
		return;
	}
}

void TitleScene::NormalUpdate(Input& input)
{
	//アニメ進行
	m_currentAnimCount += 0.5f;
	//アニメの総時間を超えたらループさせる
	float currentTotal = MV1GetAttachAnimTotalTime(m_playerHandle, m_cureentAnimHandle);
	if (m_currentAnimCount >= currentTotal)
	{
		m_currentAnimCount -= currentTotal;
	}
	MV1SetAttachAnimTime(m_playerHandle, m_cureentAnimHandle, m_currentAnimCount);


	if (input.IsTriggered("ok"))
	{
		//SoundManager::PlaySE("Ok");
		m_update = &TitleScene::FadeOutUpdate;
		m_draw = &TitleScene::FadeDraw;
		m_frame = 0;	//フェードアウトの最初
		return;
	}
}

void TitleScene::FadeOutUpdate(Input&)
{
	if (m_frame++ >= fade_interval)
	{
		//フェードアウト完了
		m_finished = true;
		m_controller.ChangeScene(std::make_shared<SceneMain>(m_controller));
		return;
	}
}

void TitleScene::NormalDraw()
{

	//モデルの位置を反映してから描画
	
    const int cx = Game::kScreenWidth / 2;
	const int cy = Game::kScreenHeight / 2;
	//カラー
	const int white = GetColor(255, 255, 255);
	const int Cyan = GetColor(0, 255, 255);
	const int Color = GetColor(224, 255, 255);
	const int black = GetColor(0, 0, 0);

	//DrawStringToHandle(510, 330 , "nigeusagi", black, m_fontHandle);
	//DrawGraph(m_logoPos.x, m_logoPos.y, m_logoHandle, true);
	DrawRotaGraph(660, 150, 0.5, 0.0, m_logoHandle, true);
	//点滅頻度
	const int intervar = 650;
	int now = GetNowCount();
	bool visible = (now / intervar) % 2;
	if (visible)
	{
		//操作説明表示
		DrawStringToHandle(470, 580, "Press A to Game", white, m_fontHandle);
	}

}

void TitleScene::FadeDraw()
{

	NormalDraw();

	//値の範囲をいったん0.0~1.0にしておくといろいろと扱いやすくなります
	auto rate = static_cast<float>(m_frame) / static_cast<float>(fade_interval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate)); //αブレンド
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);	//画面全体に黒フィルムをかける
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);	//ブレンドしない

	
}

TitleScene::TitleScene(SceneController& controller):
	Scene(controller),
	m_fontHandle(-1),
	m_skyHandle(-1),
	m_playerHandle(-1),
	m_cureentAnimHandle(-1),
	m_currentAnimCount(0.0f),
	m_playerPos(VGet(0.0f, 0.0f, 0.0f)),
	m_currentAnimIndex(-1),
	m_logoHandle(-1),
	m_playerAngle(0.0f),
	m_logoPos(VGet(0.0f, 0.0f, 0.0f))
{
	Init();
	m_update = &TitleScene::FadeInUpdate;
	m_draw = &TitleScene::FadeDraw;
	m_frame = fade_interval;
    m_finished = false;
}

void TitleScene::Init()
{
	//SetBackgroundColor(100, 150, 255);

	
	m_fontHandle = CreateFontToHandle("Constantia", 40, -1, DX_FONTTYPE_ANTIALIASING_EDGE);

	
	m_logoHandle = LoadGraph("data/kudonetta.png");

	m_logoPos = VGet(130.0f, -270.0f, 0.0f);

	//SoundManager::Load();

	//SoundManager::PlayBGM("Title", true);

	int animNo = MV1GetAnimIndex(m_playerHandle, kWaveAnimName);
	m_currentAnimCount = 0.0f;

	m_cureentAnimHandle = MV1AttachAnim(m_playerHandle, animNo, -1, -1);
	m_currentAnimIndex = animNo;

	//カメラを初期化
	//カメラをタイトル用に初期化(SceneMainと同じ初期位置に戻す)
	//これでメインシーンから引き継いだカメラの位置がリセットされる
	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 100.0f, -700.0f), VGet(0.0f, 100.0f, 0.0f));
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