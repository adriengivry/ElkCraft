#pragma once

#include <ElkGameEngine/ElkGameEngine.h>
#include <ElkTools/Utils/BlocksInfo.h>

#include "ElkCraft/Gameplay/Inventory.h"
#include "ElkCraft/Gameplay/PlayerController.h"
#include "ElkCraft/Terrain/World.h"
#include "ElkCraft/Utils/ItemsInfo.h"
#include "ElkCraft/Utils/Recipe.h"

namespace ElkCraft::Gameplay
{
	class Crafting : public ElkGameEngine::Objects::Components::Behaviours::ABehaviour
	{
	public:

		Crafting();
		~Crafting() = default;

		void TryToCraft(uint8_t p_recipeIndex);

		void SelectNextRecipe();
		void SelectPreviousRecipe();

		void LogRecipe(uint8_t p_recipeIndex);
		void LogCurrentRecipe();

		ElkCraft::Utils::Recipe& GetCurrentRecipe();

		void Update() override;
		void HandleInputs();

		void OpenMenu();
		void CloseMenu();

		bool CanCraftCurrentRecipe();
		bool InventoryHasRoomForCurrentRecipe();
		bool IsRequirementOK(uint8_t p_index);

		bool IsMenuOpened();

	private:
		ElkGameEngine::Managers::InputManager& m_inputManager;
		ElkTools::Managers::AudioManager& m_audioManager;

		std::shared_ptr<ElkCraft::Gameplay::Inventory> m_inventory;
		std::shared_ptr<PlayerController> m_playerController;

		uint8_t m_currentRecipe;
		std::vector<ElkCraft::Utils::Recipe> m_recipes;

		bool m_inMenu;
	};
}