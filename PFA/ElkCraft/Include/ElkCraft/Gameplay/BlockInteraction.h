#pragma once

#include <ElkGameEngine/ElkGameEngine.h>
#include <ElkTools/Utils/BlocksInfo.h>

#include "ElkCraft/Gameplay/Inventory.h"
#include "ElkCraft/Gameplay/PlayerController.h"
#include "ElkCraft/Terrain/World.h"
#include "ElkCraft/Utils/ItemsInfo.h"

namespace ElkCraft::Gameplay
{
	class BlockInteraction : public ElkGameEngine::Objects::Components::Behaviours::ABehaviour
	{
	public:
		struct RaycastCollision
		{
			bool found;
			uint8_t type;
			glm::vec3 collisionPoint;
			glm::vec3 blockPosition;
			glm::vec3 blockToCollisionPointDirection;
			glm::vec3 collisionFaceNormal;
		};

		enum class InteractionAction
		{
			BREAKING,
			BREAK,
			PLACE
		};

		explicit BlockInteraction(ElkCraft::Terrain::World* p_world);
		~BlockInteraction() = default;

		void Update() override;

		void HandleInputs();
		void BreakBlock();
		void PlaceBlock();

		bool BreakBlockAction();
		bool PlaceBlockAction();

		float CalculateBreakingMultiplier(uint8_t p_item, uint8_t p_block);

		void PlayBlockSound(uint8_t p_blockType, InteractionAction p_action);

		void Raycast(glm::vec3 p_source, glm::vec3 p_direction, float p_maxDistance, RaycastCollision& p_out);

	private:
		ElkGameEngine::Managers::InputManager& m_inputManager;
		ElkRendering::Managers::RenderingManager& m_renderingManager;
		ElkTools::Managers::AudioManager& m_audioManager;

		std::shared_ptr<ElkCraft::Gameplay::Inventory> m_inventory;
		std::shared_ptr<PlayerController> m_playerController;
		ElkCraft::Terrain::World* m_world;
		
		float m_interactionDistance;
		float m_raycastSteps;
		RaycastCollision m_previousBlockInFront;
		RaycastCollision m_blockInFront;
		uint8_t m_previousInventorySlot;

		bool m_instantBreaking;

		float m_breakBlockTimer;
		float m_breakSoundTimer;
		float m_breakSoundFrequency;
	};
}