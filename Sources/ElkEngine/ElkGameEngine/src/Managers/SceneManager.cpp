#include "stdafx.h"

#include "ElkGameEngine/Managers/SceneManager.h"

using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects;

SceneManager::SceneManager()
{
	SceneManager::Setup();
}

void SceneManager::Update() {}
void SceneManager::Close() {}

void SceneManager::Setup()
{
	m_currentScene = nullptr;
}

void SceneManager::SetCurrentScene(const std::string& p_sceneName)
{
	for (const auto& scene : m_scenes)
		if (scene.first == p_sceneName)
			m_currentScene = scene.second;
}

Scene& SceneManager::CreateScene(const std::string& p_sceneName, const bool p_setCurrent)
{
	std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
	newScene->SetName(p_sceneName);

	m_scenes.insert_or_assign(p_sceneName, newScene);
	if (p_setCurrent)
		m_currentScene = newScene;

	return *newScene;
}

Scene& SceneManager::GetCurrentScene() const
{
	return *m_currentScene;
}

void ElkGameEngine::Managers::SceneManager::RemoveScene(const std::string & p_sceneName)
{
	m_scenes.erase(p_sceneName);
}

bool SceneManager::HasCurrentScene() const
{
	if (m_currentScene)
		return true;
	else
		return false;
}