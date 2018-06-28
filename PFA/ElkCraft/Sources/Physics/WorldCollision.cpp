#include "stdafx.h"

#include "ElkCraft/Physics/WorldCollision.h"

using namespace ElkTools::Utils;
using namespace ElkTools::Managers;
using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkPhysics::Managers;
using namespace ElkCraft::Terrain;

ElkCraft::Physics::WorldCollision::WorldCollision(ElkCraft::Terrain::World & p_world)
	: m_physicsManager(ManagerLocator::Get<PhysicsManager>())
{
	m_world = &p_world;
}

uint8_t ElkCraft::Physics::WorldCollision::BlockIsCollidable(float p_x, float p_y, float p_z, bool p_lowRounding)
{
	if(p_lowRounding)
	{
		return static_cast<uint8_t>(ElkTools::Utils::BlocksInfo::IsCollidable(m_world->GetBlockIDLowRounding(p_x, p_y, p_z)));
	}
	return static_cast<uint8_t>(ElkTools::Utils::BlocksInfo::IsCollidable(m_world->GetBlockID(p_x, p_y, p_z)));
}

void ElkCraft::Physics::WorldCollision::Update(const glm::vec3 & p_playerPosition)
{
	UpdateCollisionX(p_playerPosition, m_physicsManager.GetCollisionTable().xTable);
	UpdateCollisionY(p_playerPosition, m_physicsManager.GetCollisionTable().yTable);
	UpdateCollisionZ(p_playerPosition, m_physicsManager.GetCollisionTable().zTable);
	UpdateCollisionDiagonal(p_playerPosition, m_physicsManager.GetCollisionTable().diagonalTable);
	UpdateEntityInBlock(p_playerPosition, m_physicsManager.GetCollisionTable().onPlayerTable);
}
void ElkCraft::Physics::WorldCollision::UpdateCollisionX(const glm::vec3& p_playerPosition,
	uint8_t(& p_collisionTableX)[12])
{
	if(p_playerPosition.z < 0)
	{
		p_collisionTableX[0] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 0.25f, true);
		p_collisionTableX[1] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 0.25f, true);
		p_collisionTableX[2] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y, p_playerPosition.z + 0.25f, true);
		p_collisionTableX[3] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y, p_playerPosition.z - 0.25f, true);
		p_collisionTableX[4] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y + 0.2f, p_playerPosition.z + 0.25f, true);
		p_collisionTableX[5] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y + 0.2f, p_playerPosition.z - 0.25f, true);

		p_collisionTableX[6] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 0.25f, true);
		p_collisionTableX[7] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 0.25f, true);
		p_collisionTableX[8] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y, p_playerPosition.z + 0.25f, true);
		p_collisionTableX[9] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y, p_playerPosition.z - 0.25f, true);
		p_collisionTableX[10] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y + 0.2f, p_playerPosition.z + 0.25f, true);
		p_collisionTableX[11] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y + 0.2f, p_playerPosition.z - 0.25f, true);
	}
	else if(p_playerPosition.z >= 0)
	{
		p_collisionTableX[0] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 0.25f, true);
		p_collisionTableX[1] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 0.25f, true);
		p_collisionTableX[2] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y, p_playerPosition.z - 0.25f, true);
		p_collisionTableX[3] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y, p_playerPosition.z + 0.25f, true);
		p_collisionTableX[4] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y + 0.2f, p_playerPosition.z - 0.25f, true);
		p_collisionTableX[5] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y + 0.2f, p_playerPosition.z + 0.25f, true);

		p_collisionTableX[6] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 0.25f, true);
		p_collisionTableX[7] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 0.25f, true);
		p_collisionTableX[8] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y, p_playerPosition.z - 0.25f, true);
		p_collisionTableX[9] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y, p_playerPosition.z + 0.25f, true);
		p_collisionTableX[10] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y + 0.2f, p_playerPosition.z - 0.25f, true);
		p_collisionTableX[11] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y + 0.2f, p_playerPosition.z + 0.25f, true);
	}
}

void ElkCraft::Physics::WorldCollision::UpdateCollisionY(const glm::vec3& p_playerPosition,
	uint8_t(& p_collisionTableY)[8])
{
	if(p_playerPosition.x < 0)
	{
		if(p_playerPosition.z < 0)
		{
			p_collisionTableY[0] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 2.5f, p_playerPosition.z, true);
			p_collisionTableY[1] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 2.5f, p_playerPosition.z, true);
			p_collisionTableY[2] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 2.5f, p_playerPosition.z - 0.25f, true);
			p_collisionTableY[3] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 2.5f, p_playerPosition.z - 0.25f, true);

			p_collisionTableY[4] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y + 1.15f, p_playerPosition.z, true);
			p_collisionTableY[5] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y + 1.15f, p_playerPosition.z, true);
			p_collisionTableY[6] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y + 1.15f, p_playerPosition.z - 0.25f, true);
			p_collisionTableY[7] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y + 1.15f, p_playerPosition.z - 0.25f, true);
		}
		else if(p_playerPosition.z >= 0)
		{
			p_collisionTableY[0] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 2.5f, p_playerPosition.z, true);
			p_collisionTableY[1] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 2.5f, p_playerPosition.z, true);
			p_collisionTableY[2] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 2.5f, p_playerPosition.z + 0.25f, true);
			p_collisionTableY[3] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 2.5f, p_playerPosition.z + 0.25f, true);

			p_collisionTableY[4] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y + 1.15f, p_playerPosition.z, true);
			p_collisionTableY[5] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y + 1.15f, p_playerPosition.z, true);
			p_collisionTableY[6] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y + 1.15f, p_playerPosition.z + 0.25f, true);
			p_collisionTableY[7] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y + 1.15f, p_playerPosition.z + 0.25f, true);
		}
	}
	else if(p_playerPosition.x >= 0)
	{
		if (p_playerPosition.z < 0)
		{
			p_collisionTableY[0] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 2.5f, p_playerPosition.z, true);
			p_collisionTableY[1] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 2.5f, p_playerPosition.z, true);
			p_collisionTableY[2] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 2.5f, p_playerPosition.z - 0.25f, true);
			p_collisionTableY[3] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 2.5f, p_playerPosition.z - 0.25f, true);

			p_collisionTableY[4] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y + 1.15f, p_playerPosition.z, true);
			p_collisionTableY[5] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y + 1.15f, p_playerPosition.z, true);
			p_collisionTableY[6] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y + 1.15f, p_playerPosition.z - 0.25f, true);
			p_collisionTableY[7] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y + 1.15f, p_playerPosition.z - 0.25f, true);
		}
		else if (p_playerPosition.z >= 0)
		{
			p_collisionTableY[0] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 2.5f, p_playerPosition.z, true);
			p_collisionTableY[1] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 2.5f, p_playerPosition.z, true);
			p_collisionTableY[2] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 2.5f, p_playerPosition.z + 0.25f, true);
			p_collisionTableY[3] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 2.5f, p_playerPosition.z + 0.25f, true);

			p_collisionTableY[4] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y + 1.15f, p_playerPosition.z, true);
			p_collisionTableY[5] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y + 1.15f, p_playerPosition.z, true);
			p_collisionTableY[6] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y + 1.15f, p_playerPosition.z + 0.25f, true);
			p_collisionTableY[7] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y + 1.15f, p_playerPosition.z + 0.25f, true);
		}
	}
}

void ElkCraft::Physics::WorldCollision::UpdateCollisionZ(const glm::vec3& p_playerPosition,
	uint8_t(& p_collisionTableZ)[12])
{
	if (p_playerPosition.x < 0)
	{
		p_collisionTableZ[0] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 1.25f, true);
		p_collisionTableZ[1] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 1.25f, true);
		p_collisionTableZ[2] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y, p_playerPosition.z - 1.25f, true);
		p_collisionTableZ[3] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y, p_playerPosition.z - 1.25f, true);
		p_collisionTableZ[4] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y + 0.15f, p_playerPosition.z - 1.25f, true);
		p_collisionTableZ[5] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y + 0.15f, p_playerPosition.z - 1.25f, true);

		p_collisionTableZ[6] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 1.25f, true);
		p_collisionTableZ[7] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 1.25f, true);
		p_collisionTableZ[8] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y, p_playerPosition.z + 1.25f, true);
		p_collisionTableZ[9] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y, p_playerPosition.z + 1.25f, true);
		p_collisionTableZ[10] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y + 0.15f, p_playerPosition.z + 1.25f, true);
		p_collisionTableZ[11] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y + 0.15f, p_playerPosition.z + 1.25f, true);
	}
	else if (p_playerPosition.x >= 0)
	{
		p_collisionTableZ[0] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 1.25f, true);
		p_collisionTableZ[1] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 1.25f, true);
		p_collisionTableZ[2] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y, p_playerPosition.z - 1.25f, true);
		p_collisionTableZ[3] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y, p_playerPosition.z - 1.25f, true);
		p_collisionTableZ[4] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y + 0.15f, p_playerPosition.z - 1.25f, true);
		p_collisionTableZ[5] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y + 0.15f, p_playerPosition.z - 1.25f, true);

		p_collisionTableZ[6] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 1.25f, true);
		p_collisionTableZ[7] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 1.25f, true);
		p_collisionTableZ[8] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y, p_playerPosition.z + 1.25f, true);
		p_collisionTableZ[9] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y, p_playerPosition.z + 1.25f, true);
		p_collisionTableZ[10] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y + 0.15f, p_playerPosition.z + 1.25f, true);
		p_collisionTableZ[11] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y + 0.15f, p_playerPosition.z + 1.25f, true);
	}
}

void ElkCraft::Physics::WorldCollision::UpdateCollisionDiagonal(const glm::vec3& p_playerPosition,
	uint8_t(& p_collisionTableDiagonal)[12])
{
	//x minus, z minus
	p_collisionTableDiagonal[0] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 1.25f, true);
	p_collisionTableDiagonal[1] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y, p_playerPosition.z - 1.25f, true);
	p_collisionTableDiagonal[2] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y + 0.15f, p_playerPosition.z - 1.25f, true);

	//x minus, z positif
	p_collisionTableDiagonal[3] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 1.25f, true);
	p_collisionTableDiagonal[4] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y, p_playerPosition.z + 1.25f, true);
	p_collisionTableDiagonal[5] = BlockIsCollidable(p_playerPosition.x - 1.25f, p_playerPosition.y + 0.15f, p_playerPosition.z + 1.25f, true);

	//x positif, z minus
	p_collisionTableDiagonal[6] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 1.25f, true);
	p_collisionTableDiagonal[7] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y, p_playerPosition.z - 1.25f, true);
	p_collisionTableDiagonal[8] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y + 0.15f, p_playerPosition.z - 1.25f, true);

	//x positif, z positif
	p_collisionTableDiagonal[9] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 1.25f, true);
	p_collisionTableDiagonal[10] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y, p_playerPosition.z + 1.25f, true);
	p_collisionTableDiagonal[11] = BlockIsCollidable(p_playerPosition.x + 1.25f, p_playerPosition.y + 0.15f, p_playerPosition.z + 1.25f, true);

}

void ElkCraft::Physics::WorldCollision::UpdateEntityInBlock(const glm::vec3& p_playerPosition,
	uint8_t(& p_EntityInBlockTable)[4])
{
	if(p_playerPosition.x < 0)
	{
		if(p_playerPosition.z < 0)
		{
			p_EntityInBlockTable[0] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 1.5f, p_playerPosition.z, true);
			p_EntityInBlockTable[1] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z, true);
			p_EntityInBlockTable[2] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 1.5f, p_playerPosition.z - 0.25f, true);
			p_EntityInBlockTable[3] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 0.25f, true);
		}
		else if(p_playerPosition.z >= 0)
		{
			p_EntityInBlockTable[0] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 1.5f, p_playerPosition.z, true);
			p_EntityInBlockTable[1] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z, true);
			p_EntityInBlockTable[2] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 1.5f, p_playerPosition.z + 0.25f, true);
			p_EntityInBlockTable[3] = BlockIsCollidable(p_playerPosition.x - 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 0.25f, true);
		}
	}
	else if(p_playerPosition.x > 0)
	{
		if (p_playerPosition.z < 0)
		{
			p_EntityInBlockTable[0] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 1.5f, p_playerPosition.z, true);
			p_EntityInBlockTable[1] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z, true);
			p_EntityInBlockTable[2] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 1.5f, p_playerPosition.z - 0.25f, true);
			p_EntityInBlockTable[3] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z - 0.25f, true);
		}
		else if (p_playerPosition.z >= 0)
		{
			p_EntityInBlockTable[0] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 1.5f, p_playerPosition.z, true);
			p_EntityInBlockTable[1] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z, true);
			p_EntityInBlockTable[2] = BlockIsCollidable(p_playerPosition.x, p_playerPosition.y - 1.5f, p_playerPosition.z + 0.25f, true);
			p_EntityInBlockTable[3] = BlockIsCollidable(p_playerPosition.x + 0.25f, p_playerPosition.y - 1.5f, p_playerPosition.z + 0.25f, true);
		}
	}
}
