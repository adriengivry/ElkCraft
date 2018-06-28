#include "stdafx.h"

#include "ElkGameEngine/Objects/Components/Behaviours/AButton.h"

using namespace ElkGameEngine::Managers;

ElkGameEngine::Objects::Components::Behaviours::AButton::AButton()
	: m_inputManager(ElkTools::Utils::ManagerLocator::Get<InputManager>())
{
	SetName("AButton");
}

void ElkGameEngine::Objects::Components::Behaviours::AButton::Update()
{
	glm::vec2 mousePosition = m_inputManager.GetCursorPosition();
	glm::vec2 halfExtends = m_extends * 0.5f;

	m_mouseIsOver = (mousePosition.x >= m_screenPosition.x - halfExtends.x
		&& mousePosition.x <= m_screenPosition.x + halfExtends.x
		&& mousePosition.y >= m_screenPosition.y - halfExtends.y
		&& mousePosition.y <= m_screenPosition.y + halfExtends.y);

	if (MouseIsOver())
	{
		OnMouseOver();
		if (m_inputManager.IsMouseEventOccured(InputManager::MouseButton::MOUSE_BUTTON_LEFT, InputManager::MouseButtonState::MOUSE_DOWN))
		{
			OnClick();
		}
	}
	else
	{
		OnMouseNotOver();
	}
}

void ElkGameEngine::Objects::Components::Behaviours::AButton::SetPosition(float p_x, float p_y)
{
	m_screenPosition.x = p_x;
	m_screenPosition.y = p_y;
}

void ElkGameEngine::Objects::Components::Behaviours::AButton::SetExtends(float p_x, float p_y)
{
	m_extends.x = p_x;
	m_extends.y = p_y;
}

bool ElkGameEngine::Objects::Components::Behaviours::AButton::MouseIsOver()
{
	return m_mouseIsOver;
}
