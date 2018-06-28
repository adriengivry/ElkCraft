#include "stdafx.h"

#include "ElkCraft/UI/HandAnimation.h"

ElkCraft::UI::HandAnimation::HandAnimation()
	: m_inputManager(ElkTools::Utils::ManagerLocator::Get<ElkGameEngine::Managers::InputManager>())
{
	m_startPosition = glm::vec3(45.f, -37.f, -70.f);

	m_positionOffset = glm::vec3(0.f);

	m_idleDestinations.emplace_back(0.f, 5.f, 0.f);
	m_idleDestinations.emplace_back(0.f, 0.f, 0.f);

	m_actionDestinations.emplace_back(0.f, 0.f, 0.f);
	m_actionDestinations.emplace_back(-5.f, 7.f, 0.f);
	m_actionDestinations.emplace_back(-2.f, 4.f, -8.f);
	m_actionDestinations.emplace_back(0.f, 0.f, -10.f);

	m_currentDestination = m_idleDestinations.begin();
}

void ElkCraft::UI::HandAnimation::Update()
{
	m_owner->transform->SetPosition(m_startPosition + m_positionOffset);

	float time = GetCurrentSpeed() * ElkTools::Utils::Time::GetDeltaTime();
	if (time > 1.f)
		time = 1.f;

	m_positionOffset.x = ElkTools::Utils::Math::Lerp(m_positionOffset.x, m_currentDestination->x, time);
	m_positionOffset.y = ElkTools::Utils::Math::Lerp(m_positionOffset.y, m_currentDestination->y, time);
	m_positionOffset.z = ElkTools::Utils::Math::Lerp(m_positionOffset.z, m_currentDestination->z, time);

	if (m_inputManager.IsMouseButtonDown(ElkGameEngine::Managers::InputManager::MouseButton::MOUSE_BUTTON_LEFT))
	{
		if (m_currentAnimation != &m_actionDestinations)
		{
			m_currentAnimation = &m_actionDestinations;
			m_currentDestination = m_actionDestinations.begin();
		}
	}
	else
	{
		if (m_currentAnimation != &m_idleDestinations)
		{
			m_currentAnimation = &m_idleDestinations;
			m_currentDestination = m_idleDestinations.begin();
		}
	}

	if (glm::distance(m_positionOffset, *m_currentDestination) <= 1.f)
	{
		GoToNextDestination(*m_currentAnimation);
	}
}

void ElkCraft::UI::HandAnimation::GoToNextDestination(std::vector<glm::vec3>& p_destinationList)
{
	++m_currentDestination;
	if (m_currentDestination == p_destinationList.end())
		m_currentDestination = p_destinationList.begin();
}

float ElkCraft::UI::HandAnimation::GetCurrentSpeed()
{
	if (m_currentAnimation == &m_idleDestinations)
		return 1.f;
	else
		return 28.f;
}
