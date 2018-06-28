#include "stdafx.h"

#include "ElkCraft/Gameplay/Crafting.h"

using namespace ElkTools::Utils;
using namespace ElkTools::Managers;
using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkCraft::Gameplay;
using namespace ElkCraft::Utils;

ElkCraft::Gameplay::Crafting::Crafting() :
	m_inputManager(ManagerLocator::Get<InputManager>()),
	m_audioManager(ManagerLocator::Get<AudioManager>())
{
	m_currentRecipe = 0;

	m_recipes.resize(11);

	m_recipes[0].SetName("Wooden Planks");
	m_recipes[0].SetResult(7);
	m_recipes[0].SetResultQuantity(4);
	m_recipes[0].AddRequirement(6, 1);

	m_recipes[1].SetName("Stick");
	m_recipes[1].SetResult(100);
	m_recipes[1].SetResultQuantity(4);
	m_recipes[1].AddRequirement(7, 2);

	m_recipes[2].SetName("Wood Axe");
	m_recipes[2].SetResult(101);
	m_recipes[2].SetResultQuantity(1);
	m_recipes[2].AddRequirement(7, 3);
	m_recipes[2].AddRequirement(100, 2);

	m_recipes[3].SetName("Wood Pickaxe");
	m_recipes[3].SetResult(102);
	m_recipes[3].SetResultQuantity(1);
	m_recipes[3].AddRequirement(7, 3);
	m_recipes[3].AddRequirement(100, 2);

	m_recipes[4].SetName("Wood Shovel");
	m_recipes[4].SetResult(103);
	m_recipes[4].SetResultQuantity(1);
	m_recipes[4].AddRequirement(7, 1);
	m_recipes[4].AddRequirement(100, 2);

	m_recipes[5].SetName("Stone Axe");
	m_recipes[5].SetResult(104);
	m_recipes[5].SetResultQuantity(1);
	m_recipes[5].AddRequirement(5, 3);
	m_recipes[5].AddRequirement(100, 2);

	m_recipes[6].SetName("Stone Pickaxe");
	m_recipes[6].SetResult(105);
	m_recipes[6].SetResultQuantity(1);
	m_recipes[6].AddRequirement(5, 3);
	m_recipes[6].AddRequirement(100, 2);

	m_recipes[7].SetName("Stone Shovel");
	m_recipes[7].SetResult(106);
	m_recipes[7].SetResultQuantity(1);
	m_recipes[7].AddRequirement(5, 1);
	m_recipes[7].AddRequirement(100, 2);

	m_recipes[8].SetName("Glass");
	m_recipes[8].SetResult(9);
	m_recipes[8].SetResultQuantity(2);
	m_recipes[8].AddRequirement(8, 2);
	m_recipes[8].AddRequirement(108, 1);

	m_recipes[9].SetName("Stone");
	m_recipes[9].SetResult(4);
	m_recipes[9].SetResultQuantity(2);
	m_recipes[9].AddRequirement(5, 2);
	m_recipes[9].AddRequirement(108, 1);

	m_recipes[10].SetName("Stone bricks");
	m_recipes[10].SetResult(11);
	m_recipes[10].SetResultQuantity(1);
	m_recipes[10].AddRequirement(4, 4);
}

void ElkCraft::Gameplay::Crafting::TryToCraft(uint8_t p_recipeIndex)
{
	Recipe& recipe = m_recipes[p_recipeIndex];
	if (recipe.IsCraftableWith(m_inventory->GetSlots()) && m_inventory->HasRoomFor(recipe.GetResultType(), recipe.GetResultQuantity()))
	{
		m_audioManager.Play2DSound("../Resources/Audio/Sounds/ui/successful_hit.ogg", false, 0.5f);

		for (Recipe::Requirement& requirement : m_recipes[p_recipeIndex].GetRequirements())
			for (uint8_t i = 0; i < requirement.quantity; ++i)
				m_inventory->RemoveObject(requirement.objectID);

		for (uint8_t i = 0; i < m_recipes[p_recipeIndex].GetResultQuantity(); ++i)
			m_inventory->AddObject(m_recipes[p_recipeIndex].GetResultType());
	}
	else
	{
		m_audioManager.Play2DSound("../Resources/Audio/Sounds/ui/break.ogg", false, 0.5f);
	}
}

void ElkCraft::Gameplay::Crafting::SelectNextRecipe()
{
	if (m_currentRecipe == m_recipes.size() - 1)
		m_currentRecipe = 0;
	else
		++m_currentRecipe;
}

void ElkCraft::Gameplay::Crafting::SelectPreviousRecipe()
{
	if (m_currentRecipe == 0)
		m_currentRecipe = static_cast<uint8_t>(m_recipes.size() - 1);
	else
		--m_currentRecipe;
}

void ElkCraft::Gameplay::Crafting::LogRecipe(uint8_t p_recipeIndex)
{
	ElkTools::Debug::Log::Process("Name: " + m_recipes[p_recipeIndex].GetName());

	auto requirements = m_recipes[p_recipeIndex].GetRequirements();
	for (uint8_t i = 0; i < requirements.size(); ++i)
		ElkTools::Debug::Log::Process("Requirement " + std::to_string(i + 1) + ": " + std::to_string(requirements[i].objectID) + " x" + std::to_string(requirements[i].quantity));
}

void ElkCraft::Gameplay::Crafting::LogCurrentRecipe()
{
	ElkTools::Debug::Log::Process("-- Current recipe --");
	LogRecipe(m_currentRecipe);
}

ElkCraft::Utils::Recipe & ElkCraft::Gameplay::Crafting::GetCurrentRecipe()
{
	return m_recipes[m_currentRecipe];
}

void ElkCraft::Gameplay::Crafting::Update()
{
	if (m_inMenu)
	{
		if (!m_inventory)
			m_inventory = m_owner->GetComponent<ElkCraft::Gameplay::Inventory>();

		if (!m_playerController)
			m_playerController = m_owner->GetComponent<PlayerController>();

		HandleInputs();
	}
}

void ElkCraft::Gameplay::Crafting::HandleInputs()
{
	if (m_inputManager.IsKeyEventOccured(InputManager::Key::KEY_F, InputManager::KeyState::KEY_DOWN))
	{
		TryToCraft(m_currentRecipe);
	}

	if (m_inputManager.IsKeyEventOccured(InputManager::Key::KEY_A, InputManager::KeyState::KEY_DOWN))
		SelectPreviousRecipe();

	if (m_inputManager.IsKeyEventOccured(InputManager::Key::KEY_D, InputManager::KeyState::KEY_DOWN))
		SelectNextRecipe();
}

void ElkCraft::Gameplay::Crafting::OpenMenu()
{
	m_inMenu = true;
}

void ElkCraft::Gameplay::Crafting::CloseMenu()
{
	m_inMenu = false;
}

bool ElkCraft::Gameplay::Crafting::CanCraftCurrentRecipe()
{
	return m_recipes[m_currentRecipe].IsCraftableWith(m_inventory->GetSlots());
}

bool ElkCraft::Gameplay::Crafting::InventoryHasRoomForCurrentRecipe()
{
	return m_inventory->HasRoomFor(m_recipes[m_currentRecipe].GetResultType(), m_recipes[m_currentRecipe].GetResultQuantity());
}

bool ElkCraft::Gameplay::Crafting::IsRequirementOK(uint8_t p_index)
{
	Recipe::Requirement& targetRequirement = m_recipes[m_currentRecipe].GetRequirements()[p_index];
	return m_inventory->IsOwned(targetRequirement.objectID, targetRequirement.quantity);
}

bool ElkCraft::Gameplay::Crafting::IsMenuOpened()
{
	return m_inMenu;
}
