#include "UIManager.h"
#include "DxLib.h"

void UIManager::Add(std::unique_ptr<UIBase> ui)
{
	//UI‚ð“o˜^‚·‚é
	//unique_ptr‚ÅŠ—LŒ ‚ð‚à‚ç‚¢ŒÄ‚Ño‚µ‘¤‚Ístd::move(ui)‚Å“n‚·
	m_pUiList.push_back(std::move(ui));
}

void UIManager::Init()
{
	//“o˜^‚³‚ê‚½UI‚ÌInit‚ð‚Ü‚Æ‚ß‚ÄŒÄ‚Ô
	for (auto& ui : m_pUiList)
	{
		ui->Init();
	}

}
void UIManager::Update()
{
	//“o˜^‚³‚ê‚½UI‚ÌUpdate‚ð‚Ü‚Æ‚ß‚ÄŒÄ‚Ô
	for (auto& ui : m_pUiList)
	{
		ui->Update();
	}
}
void UIManager::Draw()
{
	//“o˜^‚³‚ê‚½UI‚ÌDraw‚ð‚Ü‚Æ‚ß‚ÄŒÄ‚Ô
	for (auto& ui : m_pUiList)
	{
		ui->Draw();
	}
}
