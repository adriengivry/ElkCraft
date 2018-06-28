#pragma once

#include <ElkGameEngine/ElkGameEngine.h>
#include <ElkTools/Utils/BlocksInfo.h>

#include "ElkCraft/Gameplay/Inventory.h"
#include "ElkCraft/Utils/ItemsInfo.h"

namespace ElkCraft::Utils
{
	class Recipe
	{
	public:
		struct Requirement
		{
			Requirement(uint8_t p_objectID, uint8_t p_quantity) : objectID(p_objectID), quantity(p_quantity) {}

			uint8_t objectID = 0;
			uint8_t quantity = 0;
		};

		Recipe();

		bool IsCraftableWith(std::vector<std::reference_wrapper<ElkCraft::Gameplay::Inventory::Slot>> p_slots);
		void AddRequirement(uint8_t p_objectID, uint8_t p_quantity);
		void SetName(const std::string& p_name);
		void SetResult(uint8_t p_result);
		void SetResultQuantity(uint8_t p_resultQuantity);

		std::string GetName();
		uint8_t GetResultType();
		uint8_t GetResultQuantity();
		std::vector<Requirement>& GetRequirements();

	private:
		std::string m_name;
		std::vector<Requirement> m_requirements;
		uint8_t m_result;
		uint8_t m_resultQuantity;
	};
}