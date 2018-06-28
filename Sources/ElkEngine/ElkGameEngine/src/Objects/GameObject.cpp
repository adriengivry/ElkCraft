#include "stdafx.h"

#include "ElkGameEngine/Objects/GameObject.h"
#include "ElkGameEngine/Objects/Components/Transform.h"

using namespace ElkGameEngine;

ElkGameEngine::Objects::GameObject::GameObject(const std::string & p_name)
{
	transform = AddComponent<Objects::Components::Transform>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));

	SetName(p_name);
}

std::vector<std::shared_ptr<Objects::Components::AComponent>>& Objects::GameObject::GetComponents()
{
	return m_components;
}

void ElkGameEngine::Objects::GameObject::UpdateComponents()
{
	for (auto component : m_components)
		component->Update();
}
