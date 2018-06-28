#pragma once

#include <ElkGameEngine/ElkGameEngine.h>

#include "ElkCraft/Terrain/World.h"

namespace ElkCraft::Physics
{
	class WorldCollision
	{
	public:
		WorldCollision(ElkCraft::Terrain::World& p_world);
		~WorldCollision() = default;

		uint8_t BlockIsCollidable(float p_x, float p_y, float p_z, bool p_lowRounding);

		void Update(const glm::vec3& p_playerPosition);
		void UpdateCollisionX(const glm::vec3& p_playerPosition, uint8_t(&p_collisionTableX)[12]);
		void UpdateCollisionY(const glm::vec3& p_playerPosition, uint8_t(&p_collisionTableY)[8]);
		void UpdateCollisionZ(const glm::vec3& p_playerPosition, uint8_t(&p_collisionTableZ)[12]);
		void UpdateCollisionDiagonal(const glm::vec3& p_playerPosition, uint8_t(&p_collisionTableDiagonal)[12]);
		void UpdateEntityInBlock(const glm::vec3& p_playerPosition, uint8_t(&p_EntityInBlockTable)[4]);
	private:

		ElkPhysics::Managers::PhysicsManager& m_physicsManager;
		ElkCraft::Terrain::World* m_world;
	};
}