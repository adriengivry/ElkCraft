#pragma once

#include <ElkGameEngine/ElkGameEngine.h>
#include <ElkTools/Utils/BlocksInfo.h>

#include "ElkCraft/Gameplay/Inventory.h"
#include "ElkCraft/Gameplay/HealthSystem.h"
#include "ElkCraft/Gameplay/HungerSystem.h"
#include "ElkCraft/Gameplay/PlayerController.h"
#include "ElkCraft/Terrain/World.h"
#include "ElkCraft/Utils/ItemsInfo.h"

namespace ElkCraft::Gameplay
{
	class ItemInteraction : public ElkGameEngine::Objects::Components::Behaviours::ABehaviour
	{
	public:
		ItemInteraction();
		~ItemInteraction() = default;

		void Update() override;
		void HandleInputs();

		void UseItem();

	private:
		ElkGameEngine::Managers::InputManager& m_inputManager;
		ElkTools::Managers::AudioManager& m_audioManager;

		std::shared_ptr<ElkCraft::Gameplay::Inventory> m_inventory;
		std::shared_ptr<ElkCraft::Gameplay::HealthSystem> m_healthSystem;
		std::shared_ptr<ElkCraft::Gameplay::HungerSystem> m_hungerSystem;
	};
}