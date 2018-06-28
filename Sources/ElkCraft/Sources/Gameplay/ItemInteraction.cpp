#include "stdafx.h"

#include "ElkCraft/Gameplay/ItemInteraction.h"

using namespace ElkTools::Utils;
using namespace ElkTools::Managers;
using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkCraft::Gameplay;
using namespace ElkCraft::Utils;

ElkCraft::Gameplay::ItemInteraction::ItemInteraction()
	: m_inputManager(ManagerLocator::Get<InputManager>()),
	m_audioManager(ManagerLocator::Get<AudioManager>())
{
	m_inventory = nullptr;
}

void ElkCraft::Gameplay::ItemInteraction::Update()
{
	if (!m_inventory)
		m_inventory = m_owner->GetComponent<Inventory>();

	if (!m_healthSystem)
		m_healthSystem = m_owner->GetComponent<HealthSystem>();

	if (!m_hungerSystem)
		m_hungerSystem = m_owner->GetComponent<HungerSystem>();

	HandleInputs();
}

void ElkCraft::Gameplay::ItemInteraction::HandleInputs()
{
	if (m_inputManager.IsMouseEventOccured(InputManager::MouseButton::MOUSE_BUTTON_RIGHT, InputManager::MouseButtonState::MOUSE_DOWN))
		UseItem();
}

void ElkCraft::Gameplay::ItemInteraction::UseItem()
{
	Inventory::Slot& currentSlot = m_inventory->GetCurrentSlot();

	if (ItemsInfo::IsItem(currentSlot.type) && currentSlot.quantity > 0)
	{
		switch (ItemsInfo::GetItemType(currentSlot.type))
		{
		case ItemsInfo::ItemType::APPLE:
			if (m_hungerSystem->GetHunger() < 20)
			{
				m_hungerSystem->AddHunger(3);
				std::string soundToPlay = m_audioManager.FindRandomSound("../Resources/Audio/Sounds/items/", "eat", ".ogg", 3);
				m_audioManager.Play3DSound(soundToPlay, m_owner->transform->GetPosition().x, m_owner->transform->GetPosition().y, m_owner->transform->GetPosition().z);
				currentSlot.Remove(1);
				if (currentSlot.IsEmpty())
					currentSlot.Clear();
			}
			break;
		}
	}
}
