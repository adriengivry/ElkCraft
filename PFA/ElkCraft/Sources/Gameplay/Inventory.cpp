#include "stdafx.h"

#include "ElkCraft/Gameplay/Inventory.h"

using namespace ElkCraft::Gameplay;

Inventory::Inventory() : 
	m_inputManager(ElkTools::Utils::ManagerLocator::Get<ElkGameEngine::Managers::InputManager>()),
	m_currentSlotIndex(0)
{
	m_objectToAddToInventory = "";
	m_keypadCheatTimer = 0.f;
}

Inventory::Slot& Inventory::GetCurrentSlot()
{
	return m_slots[m_currentSlotIndex];
}

Inventory::Slot & Inventory::GetSlot(uint8_t p_index)
{
	return m_slots[p_index];
}

std::vector<std::reference_wrapper<ElkCraft::Gameplay::Inventory::Slot>> ElkCraft::Gameplay::Inventory::GetSlots()
{
	std::vector<std::reference_wrapper<ElkCraft::Gameplay::Inventory::Slot>> result;

	for (auto& slot : m_slots)
	{
		if (!slot.IsEmpty())
			result.push_back(slot);
	}

	return result;
}

uint8_t Inventory::GetCurrentSlotIndex()
{
	return m_currentSlotIndex;
}

bool ElkCraft::Gameplay::Inventory::Contains(uint8_t p_type)
{
	for (uint8_t i = 0; i < 10; ++i)
		if (m_slots[i].type == p_type)
			return true;

	return false;
}

bool Inventory::RemoveCurrentObject()
{
	if (!m_slots[m_currentSlotIndex].IsEmpty())
	{
		m_slots[m_currentSlotIndex].Remove(1);

		if (m_slots[m_currentSlotIndex].IsEmpty())
			m_slots[m_currentSlotIndex].Clear();

		return true;
	}
	else
	{
		ElkTools::Debug::Log::Process("Empty slot !");
		return false;
	}
}

bool ElkCraft::Gameplay::Inventory::RemoveObject(uint8_t p_type)
{
	for (Slot& slot : m_slots)
		if (!slot.IsEmpty() && slot.type == p_type)
		{
			slot.Remove(1);
			if (slot.IsEmpty())
				slot.Clear();
			return true;
		}

	return false;
}

bool Inventory::AddObject(uint8_t p_toAdd)
{
	if (p_toAdd == 0)
		return false;

	bool found = false;

	/* Try to complete a stack */
	for (uint8_t i = 0; i < 10; ++i)
	{
		if (m_slots[i].type == p_toAdd && !m_slots[i].IsFull())
		{
			m_slots[i].Add(1);
			found = true;
			break;
		}
	}

	/* If it failed, try to use a free space */
	if (!found)
	{
		for (uint8_t i = 0; i < 10; ++i)
		{
			if (m_slots[i].IsEmpty())
			{
				m_slots[i].type = p_toAdd;
				m_slots[i].Add(1);
				found = true;
				break;
			}
		}
	}

	/* No slot available... */
	if (!found)
		ElkTools::Debug::Log::Process("Your inventory is full...");

	return found;
}

void Inventory::SetCurrentSlot(uint8_t p_index)
{
	ElkTools::Debug::Log::Process("Switching to slot (" + std::to_string(p_index == 9 ? 0 : p_index + 1) + ")");

	m_currentSlotIndex = p_index;
}

void Inventory::GoToNextSlot()
{
	if (m_currentSlotIndex != 9)
		++m_currentSlotIndex;
	else
		m_currentSlotIndex = 0;
}

void Inventory::GoToPreviousSlot()
{
	if (m_currentSlotIndex != 0)
		--m_currentSlotIndex;
	else
		m_currentSlotIndex = 9;
}

bool ElkCraft::Gameplay::Inventory::IsOwned(uint8_t p_type, uint8_t p_quantity)
{
	for (uint8_t i = 0; i < 10; ++i)
		if (m_slots[i].type == p_type && m_slots[i].quantity >= p_quantity)
			return true;

	return false;
}

bool ElkCraft::Gameplay::Inventory::HasRoomFor(uint8_t p_type, uint8_t p_quantity)
{
	uint8_t quantityToStore = p_quantity;

	for (uint8_t i = 0; i < 10; ++i)
	{
		if (m_slots[i].type == p_type)
			quantityToStore -= std::min(static_cast<uint8_t>(64 - m_slots[i].quantity), quantityToStore);
		else if (m_slots[i].IsEmpty())
			quantityToStore -= std::min(static_cast<uint8_t>(64), quantityToStore);

		if (quantityToStore == 0)
			return true;
	}

	return false;
}

uint8_t ElkCraft::Gameplay::Inventory::GetKeypadCheatCurrentBlock()
{
	uint8_t objectToAdd = static_cast<uint8_t>(std::atoi(m_objectToAddToInventory.c_str()));
	if ((objectToAdd >= 1 && objectToAdd <= 17) || (objectToAdd >= 100 && objectToAdd <= 108))
		return objectToAdd;
	else
		return 0;
}

void ElkCraft::Gameplay::Inventory::HandleKeypadCheat()
{
	m_keypadCheatTimer += ElkTools::Utils::Time::GetDeltaTime();

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_0, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory += '0';
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_1, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory += '1';
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_2, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory += '2';
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_3, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory += '3';
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_4, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory += '4';
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_5, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory += '5';
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_6, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory += '6';
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_7, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory += '7';
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_8, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory += '8';
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_9, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory += '9';
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_DECIMAL, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		m_objectToAddToInventory = "";
		m_keypadCheatTimer = 0.f;
	}

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_ENTER, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		uint8_t objectToAdd = GetKeypadCheatCurrentBlock();
		if (objectToAdd != 0)
		{		
			m_slots[m_currentSlotIndex].type = objectToAdd;
			m_slots[m_currentSlotIndex].quantity = 64;
		}
	}

	if (m_keypadCheatTimer >= 2.5f || GetKeypadCheatCurrentBlock() == 0)
		m_objectToAddToInventory = "";

}

void Inventory::Update()
{
	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_1, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCurrentSlot(0);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_2, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCurrentSlot(1);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_3, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCurrentSlot(2);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_4, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCurrentSlot(3);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_5, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCurrentSlot(4);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_6, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCurrentSlot(5);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_7, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCurrentSlot(6);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_8, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCurrentSlot(7);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_9, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCurrentSlot(8);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_0, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCurrentSlot(9);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_Q, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		ClearCurrentSlot();

	glm::vec2& scrollData = m_inputManager.GetScrollData();
	if (scrollData.y < 0)
		GoToNextSlot();
	else if (scrollData.y > 0)
		GoToPreviousSlot();

	HandleKeypadCheat();
}

void Inventory::Fill()
{
	ElkTools::Debug::Log::Process("Inventory filled with random objects");

	for (uint8_t i = 0; i < 10; ++i)
	{
		FillSlot(i);
	}
}

void ElkCraft::Gameplay::Inventory::FillSlot(uint8_t p_type)
{
	uint8_t objectToAdd = 0;

	do
	{
		if (ElkTools::Utils::Random::Generate(1, 2) == 1)
			objectToAdd = ElkTools::Utils::Random::Generate(1, 17);
		else
			objectToAdd = ElkTools::Utils::Random::Generate(100, 108);

	} while (Contains(objectToAdd));

	m_slots[p_type].type = objectToAdd;
	m_slots[p_type].quantity = 64;
}

void Inventory::Clear()
{
	ElkTools::Debug::Log::Process("Inventory cleared");

	for (uint8_t i = 0; i < 10; ++i)
	{
		m_slots[i].Clear();
	}
}

void ElkCraft::Gameplay::Inventory::ClearCurrentSlot()
{
	m_slots[m_currentSlotIndex].Clear();
}

void Inventory::Log()
{
	ElkTools::Debug::Log::Process("-- INVENTORY CONTENT --");
	std::string indent = "    ";

	for (uint8_t i = 0; i < 10; ++i)
		ElkTools::Debug::Log::Process(indent + "SLOT " + std::to_string(i == 9 ? 0 : i + 1) + ": [" + std::to_string(m_slots[i].type) + "] x" + std::to_string(m_slots[i].quantity));
}
