#pragma once

#include <ElkGameEngine/ElkGameEngine.h>
#include <ElkTools/Utils/BlocksInfo.h>

#include "ElkCraft/Gameplay/Inventory.h"
#include "ElkCraft/Gameplay/PlayerController.h"
#include "ElkCraft/Terrain/World.h"
#include "ElkCraft/Utils/ItemsInfo.h"
#include "ElkCraft/Utils/Recipe.h"
#include "ElkCraft/Gameplay/HealthSystem.h"

namespace ElkCraft::Gameplay
{
	class HungerSystem : public ElkGameEngine::Objects::Components::Behaviours::ABehaviour
	{
	public:
		HungerSystem();
		~HungerSystem() = default;

		void Update() override;

		uint8_t GetHunger();

		void RemoveHunger(uint8_t p_amount);
		void AddHunger(uint8_t p_amount);
		void SetHunger(const uint8_t& p_energy);

	private:
		std::shared_ptr<HealthSystem> m_healthSystem;
		std::shared_ptr<PlayerController> m_playerController;

		uint8_t m_maxHunger;
		uint8_t m_hunger;

		float m_timer;
		float m_hungerDrainTimer;
	};
}