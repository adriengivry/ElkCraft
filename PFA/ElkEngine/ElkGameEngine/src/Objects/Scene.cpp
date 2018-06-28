#include "stdafx.h"

#include "ElkGameEngine/Objects/Scene.h"

using namespace ElkGameEngine::Objects;

Scene::Scene()
{
	SetName("Scene");
}

ElkGameEngine::Objects::Scene::~Scene()
{
}

GameObject * ElkGameEngine::Objects::Scene::GetGameObject(uint64_t p_instanceID)
{
	if (m_gameObjects.find(p_instanceID) != m_gameObjects.end())
		return m_gameObjects[p_instanceID].get();

	return nullptr;
}

bool ElkGameEngine::Objects::Scene::DestroyGameObject(uint64_t p_instanceID)
{
	if (GetGameObject(p_instanceID))
	{
		m_gameObjects.erase(p_instanceID);
		return true;
	}
	
	return false;
}

GameObject& ElkGameEngine::Objects::Scene::CreateGameObject(const std::string & p_name)
{
	std::shared_ptr<GameObject> newGameObject = std::make_shared<GameObject>(p_name);
	m_gameObjects.insert(std::pair<uint64_t, std::shared_ptr<GameObject>>(newGameObject->GetInstanceID(), newGameObject));
	return *newGameObject;
}
