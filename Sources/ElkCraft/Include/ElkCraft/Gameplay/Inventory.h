#pragma once

#include <ElkGameEngine/ElkGameEngine.h>

namespace ElkCraft::Gameplay
{
	class Inventory : public ElkGameEngine::Objects::Components::Behaviours::ABehaviour
	{
	public:
		struct Slot
		{
			uint8_t type;
			uint8_t quantity;

			Slot() { Clear(); }

			void Clear()
			{
				type = 0;
				quantity = 0;
			}

			bool IsEmpty() { return type == 0 || quantity == 0; }
			bool IsFull() { return quantity == 64; }

			void Remove(uint8_t p_quantity)
			{
				if (!IsEmpty())
				{
					if (quantity - p_quantity <= 0)
						Clear();
					else
						--quantity;
				}
			}

			void Add(uint8_t p_quantity)
			{
				if (!IsFull())
				{
					if (quantity + p_quantity >= 64)
						quantity = 64;
					else
						++quantity;
				}
			}
		};

		Inventory();
		~Inventory() = default;

		Inventory::Slot& GetCurrentSlot();
		Inventory::Slot& GetSlot(uint8_t p_index);
		std::vector<std::reference_wrapper<ElkCraft::Gameplay::Inventory::Slot>> GetSlots();
		uint8_t GetCurrentSlotIndex();
		bool Contains(uint8_t p_type);
		bool RemoveCurrentObject();
		bool RemoveObject(uint8_t p_type);
		bool AddObject(uint8_t p_toAdd);
		void SetCurrentSlot(uint8_t p_index);
		void GoToNextSlot();
		void GoToPreviousSlot();
		bool IsOwned(uint8_t p_type, uint8_t p_quantity);
		bool HasRoomFor(uint8_t p_type, uint8_t p_quantity);
		uint8_t GetKeypadCheatCurrentBlock();

		void HandleKeypadCheat();

		void Update() override;
		void Fill();
		void FillSlot(uint8_t p_type);
		void Clear();
		void ClearCurrentSlot();

		void Log();

	private:
		ElkGameEngine::Managers::InputManager& m_inputManager;
		uint8_t m_currentSlotIndex;
		Slot m_slots[10];

		/* God mode relative */
		std::string m_objectToAddToInventory;
		float m_keypadCheatTimer;
	};
}