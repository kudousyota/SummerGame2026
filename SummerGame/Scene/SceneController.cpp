#include "SceneController.h"
#include "Scene.h"
#include "SceneMain.h"

void SceneController::ResetScene(std::shared_ptr<Scene> scene)
{
	// シーンを一つだけにしたいので、いったんシーンをすべてクリアします
	m_scenes.clear();
	m_scenes.push_back(scene);
}

void SceneController::ChangeScene(std::shared_ptr<Scene> scene)
{
	// もし、リストが空の場合、普通に代入でChangeSceneしようとすると、当然emptyの箱に対してChangeしようとするので、クラッシュします。
	// このため、最初にemptyかどうかをチェックします
	if (m_scenes.empty())	// 最初は要素がないためpush_backで要素数を増やす
	{
		// 空っぽの場合には指定の要素をpushします。少なくとも一つは積まれている状態にする
		m_scenes.push_back(scene);
	}
	else
	{
		m_scenes.back() = scene;	// この行の時点で元のシーンは自動的に削除されています
		
	}
	m_scenes.back()->Init();
}

void SceneController::PushScene(std::shared_ptr<Scene> scene)
{
	// 新しいシーンを末尾に積みます
	m_scenes.push_back(scene);
}

void SceneController::PopScene()
{
	// 末尾のシーンを削除します
	m_scenes.pop_back();
}

void SceneController::Update(Input& input)
{
	// 末尾の要素に対してのみUpdateする
	m_scenes.back()->Update(input);
}

void SceneController::Draw()
{
	// 末尾の要素に対してのみDrawする
	for (auto& scene : m_scenes)
	{
		scene->Draw();
	}
}