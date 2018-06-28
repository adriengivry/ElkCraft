#include "stdafx.h"

#include "ElkCraft/Gameplay/HealthSystem.h"

using namespace ElkTools::Utils;
using namespace ElkTools::Managers;
using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkCraft::Gameplay;
using namespace ElkCraft::Utils;


ElkCraft::Gameplay::HealthSystem::HealthSystem(ElkCraft::Terrain::World * p_world)
	: m_world(p_world),
	m_audioManager(ManagerLocator::Get<AudioManager>())
{
	m_maxHealth = 20;
	m_health = m_maxHealth;

	m_invulnerableTimer = 0.f;
	m_invulnerableDuration = 0.5f;

	m_wasGrounded = true;
	m_lastHeightWhenGrounded = 1997498.f;
	m_vulnerable = true;
}

uint8_t ElkCraft::Gameplay::HealthSystem::GetHealth()
{
	return m_health;
}

void ElkCraft::Gameplay::HealthSystem::AddHealth(uint8_t p_value)
{
	if (m_health + p_value > m_maxHealth)
		m_health = m_maxHealth;
	else
		m_health += p_value;
}

void ElkCraft::Gameplay::HealthSystem::RemoveHealth(uint8_t p_value, bool p_makeInvulnerable, bool p_makeJump)
{
	if (m_health - p_value < 0)
		m_health = 0;
	else
		m_health -= p_value;

	std::string sound = m_audioManager.FindRandomSound("../Resources/Audio/Sounds/damage/", "hit", ".ogg", 3);
	m_audioManager.Play3DSound(sound, m_owner->transform->GetPosition().x, m_owner->transform->GetPosition().y, m_owner->transform->GetPosition().z, false, 1.f, 1.f);

	if (p_makeJump)
		m_rigidbody->SetVelocity(m_rigidbody->GetVelocity() + glm::vec3(0.f, 2.0f, 0.f));

	if (p_makeInvulnerable)
		m_vulnerable = false;
}

void ElkCraft::Gameplay::HealthSystem::ApplyDamagesFrom(uint8_t p_blockType)
{
	switch (static_cast<BlocksInfo::BlockType>(p_blockType))
	{
	case BlocksInfo::BlockType::LAVA:
		RemoveHealth(1);
		break;
	}
}

void ElkCraft::Gameplay::HealthSystem::ApplyFallDamages(float p_heightTravelled)
{
	if (p_heightTravelled < 0)
		return;

	uint16_t blocksTravelled = static_cast<uint16_t>(p_heightTravelled + 0.5f);

	if (blocksTravelled > 3)
	{
		glm::vec3 playerPosition = m_owner->transform->GetPosition();
		if (blocksTravelled > 6)
			m_audioManager.Play3DSound("../Resources/Audio/Sounds/damage/fallbig.ogg",playerPosition.x, playerPosition.y, playerPosition.z, false);
		else
			m_audioManager.Play3DSound("../Resources/Audio/Sounds/damage/fallsmall.ogg", playerPosition.x, playerPosition.y, playerPosition.z, false);

		RemoveHealth(blocksTravelled - 3, false, false);
	}
}

bool ElkCraft::Gameplay::HealthSystem::IsAlive()
{
	return m_health > 0;
}

bool ElkCraft::Gameplay::HealthSystem::IsVulnerable()
{
	return m_vulnerable;
}

void ElkCraft::Gameplay::HealthSystem::Update()
{
	if (!m_playerController)
		m_playerController = m_owner->GetComponent<PlayerController>();

	if (!m_rigidbody)
		m_rigidbody = m_owner->GetComponent<Rigidbody>();

	if (m_lastHeightWhenGrounded != 1997498.f)
	{
		if (m_playerController->IsGrounded() && !m_wasGrounded)
		{
			float travelledDuringFall = m_lastHeightWhenGrounded - m_owner->transform->GetPosition().y;
			ApplyFallDamages(travelledDuringFall);
		}
	}

	if (m_playerController->IsGrounded() || m_playerController->IsSwimming() || m_rigidbody->GetGravityScale() == 0.f)
	{
		m_lastHeightWhenGrounded = m_owner->transform->GetPosition().y;
	}

	if (m_playerController)

	m_wasGrounded = m_playerController->IsGrounded();

	if (!m_vulnerable)
	{
		m_invulnerableTimer += ElkTools::Utils::Time::GetDeltaTime();
		if (m_invulnerableTimer > m_invulnerableDuration)
		{
			m_invulnerableTimer = 0.f;
			m_vulnerable = true;
		}
	}
	else
	{
		ApplyDamagesFrom(m_world->GetBlockID(m_playerController->GetHeadPosition()));
		ApplyDamagesFrom(m_world->GetBlockID(m_playerController->GetFeetPosition()));
	}
}

void HealthSystem::SetHealth(const uint8_t& p_health)
{
	m_health = p_health;
}
