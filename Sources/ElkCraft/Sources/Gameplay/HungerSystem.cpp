#include "stdafx.h"

#include "ElkCraft/Gameplay/HungerSystem.h"

using namespace ElkTools::Utils;
using namespace ElkTools::Managers;
using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkCraft::Gameplay;
using namespace ElkCraft::Utils;

ElkCraft::Gameplay::HungerSystem::HungerSystem()
{
	m_maxHunger = 20;
	m_hunger = m_maxHunger;

	m_timer = 0.f;
	m_hungerDrainTimer = 0.f;
}

void ElkCraft::Gameplay::HungerSystem::Update()
{
	if (!m_playerController)
		m_playerController = m_owner->GetComponent<PlayerController>();

	if (!m_healthSystem)
		m_healthSystem = m_owner->GetComponent<HealthSystem>();

	m_timer += Time::GetDeltaTime();
	m_hungerDrainTimer += Time::GetDeltaTime();

	if (m_hungerDrainTimer >= 30.f)
	{
		RemoveHunger(1);
		m_hungerDrainTimer = 0.f;
	}

	if (m_hunger == 20)
	{
		if (m_timer >= 0.5f)
		{
			m_healthSystem->AddHealth(1);
			m_timer = 0.f;
		}
	}
	else if (m_hunger >= 18)
	{
		if (m_timer >= 4.f)
		{
			m_healthSystem->AddHealth(1);
			m_timer = 0.f;
		}
	}
	else if (m_hunger == 0)
	{
		if (m_timer >= 4.f)
		{
			if (m_healthSystem->GetHealth() > 1)
			{
				m_healthSystem->RemoveHealth(1, false, false);
				m_timer = 0.f;
			}
		}
	}


	if (m_hunger > 6)
	{
		m_playerController->SetCanRun(true);
	}
	else
	{
		m_playerController->SetCanRun(false);
	}
}

uint8_t ElkCraft::Gameplay::HungerSystem::GetHunger()
{
	return m_hunger;
}

void ElkCraft::Gameplay::HungerSystem::RemoveHunger(uint8_t p_amount)
{
	if (m_hunger - p_amount < 0)
		m_hunger = 0;
	else
		--m_hunger;
}

void ElkCraft::Gameplay::HungerSystem::AddHunger(uint8_t p_amount)
{
	if (m_hunger + p_amount > m_maxHunger)
		m_hunger = m_maxHunger;
	else
		++m_hunger;
}

void HungerSystem::SetHunger(const uint8_t& p_energy)
{
	m_hunger = p_energy;
}
