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
	class HealthSystem : public ElkGameEngine::Objects::Components::Behaviours::ABehaviour
	{
	public:
		HealthSystem(ElkCraft::Terrain::World* p_world);
		~HealthSystem() = default;

		void AddHealth(uint8_t p_value);
		void RemoveHealth(uint8_t p_value, bool p_makeInvulnerable = true, bool p_makeJump = true);

		void ApplyDamagesFrom(uint8_t p_blockType);
		void ApplyFallDamages(float p_heightTravelled);

		uint8_t GetHealth();
		bool IsAlive();

		bool IsVulnerable();

		void Update() override;
		void SetHealth(const uint8_t& p_health);

	private:
		ElkCraft::Terrain::World* m_world;
		ElkTools::Managers::AudioManager& m_audioManager;
		std::shared_ptr<PlayerController> m_playerController;
		std::shared_ptr<ElkGameEngine::Objects::Components::Behaviours::Rigidbody> m_rigidbody;

		uint8_t m_maxHealth;
		uint8_t m_health;

		bool m_vulnerable;

		float m_invulnerableTimer;
		float m_invulnerableDuration;

		float m_lastHeightWhenGrounded;
		bool m_wasGrounded;
	};
}