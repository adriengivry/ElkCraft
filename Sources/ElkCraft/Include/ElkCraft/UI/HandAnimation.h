#pragma once

#include <ElkGameEngine/ElkGameEngine.h>

namespace ElkCraft::UI
{
	class HandAnimation : public ElkGameEngine::Objects::Components::Behaviours::ABehaviour
	{
	public:
		HandAnimation();
		~HandAnimation() = default;

		void Update() override;
		void GoToNextDestination(std::vector<glm::vec3>& p_destinationList);
		float GetCurrentSpeed();

	private:
		ElkGameEngine::Managers::InputManager& m_inputManager;

		glm::vec3 m_startPosition;
		glm::vec3 m_positionOffset;

		std::vector<glm::vec3>* m_currentAnimation;
		std::vector<glm::vec3>::iterator m_currentDestination;

		std::vector<glm::vec3> m_idleDestinations;
		std::vector<glm::vec3> m_actionDestinations;
	};
}