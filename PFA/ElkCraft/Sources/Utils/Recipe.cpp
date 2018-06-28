#include "stdafx.h"

#include "ElkCraft/Utils/Recipe.h"

using namespace ElkTools::Utils;
using namespace ElkTools::Managers;
using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkCraft::Utils;

ElkCraft::Utils::Recipe::Recipe()
{
	m_name = "NO_NAME";
	m_result = 0;
	m_resultQuantity = 0;
}

bool ElkCraft::Utils::Recipe::IsCraftableWith(std::vector<std::reference_wrapper<ElkCraft::Gameplay::Inventory::Slot>> p_slots)
{
	for (auto& requirement : m_requirements)
	{
		bool achieved = false;

		for (auto slot : p_slots)
		{
			if (slot.get().type == requirement.objectID && slot.get().quantity >= requirement.quantity)
			{
				achieved = true;
			}
		}

		if (!achieved)
			return false;
	}

	return true;
}

void ElkCraft::Utils::Recipe::AddRequirement(uint8_t p_objectID, uint8_t p_quantity)
{
	m_requirements.emplace_back(p_objectID, p_quantity);
}

void ElkCraft::Utils::Recipe::SetName(const std::string & p_name)
{
	m_name = p_name;
}

void ElkCraft::Utils::Recipe::SetResult(uint8_t p_result)
{
	m_result = p_result;
}

void ElkCraft::Utils::Recipe::SetResultQuantity(uint8_t p_resultQuantity)
{
	m_resultQuantity = p_resultQuantity;
}

std::string ElkCraft::Utils::Recipe::GetName()
{
	return m_name;
}

uint8_t ElkCraft::Utils::Recipe::GetResultType()
{
	return m_result;
}

uint8_t ElkCraft::Utils::Recipe::GetResultQuantity()
{
	return m_resultQuantity;
}

std::vector<Recipe::Requirement>& ElkCraft::Utils::Recipe::GetRequirements()
{
	return m_requirements;
}
