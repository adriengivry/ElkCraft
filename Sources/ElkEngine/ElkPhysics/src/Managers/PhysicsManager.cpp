#include "stdafx.h"

#include <ElkTools/Utils/IniLoader.h>
#include <ElkTools/Utils/Profiler.h>
#include <ElkTools/Utils/Time.h>

#include "ElkPhysics/Managers/PhysicsManager.h"

void ElkPhysics::Managers::PhysicsManager::Setup()
{
	SetGravity(ElkTools::Utils::IniLoader::GetFloat("physics.ini", "gravity"));
}

void ElkPhysics::Managers::PhysicsManager::Close()
{
}

void ElkPhysics::Managers::PhysicsManager::ClearPhysicsEntities()
{
	m_entities.clear();
}

void ElkPhysics::Managers::PhysicsManager::AddEntity(TransformData& p_transform, RigidbodyData& p_rigidbody)
{
	PROFILER_SPY("PhysicsManager::AddEntity");

	m_entities.emplace_back(p_transform, p_rigidbody);
}

void ElkPhysics::Managers::PhysicsManager::MoveEntites(double p_deltaTime)
{
	PROFILER_SPY("PhysicsManager::MoveEntities");

	for (Data::Physics::PhysicsEntity& entity : m_entities)
		entity.GetTransform().position += entity.GetRigidbody().velocity * static_cast<float>(p_deltaTime);
}
void ElkPhysics::Managers::PhysicsManager::SetGravity(float p_value)
{
	m_gravity = p_value;
}

void ElkPhysics::Managers::PhysicsManager::CheckEntityInBlock()
{
	for (Data::Physics::PhysicsEntity& entity : m_entities)
	{
		if (!entity.GetRigidbody().kinematic)
		{
			if (m_table.onPlayerTable[0] != 0 || m_table.onPlayerTable[1] != 0 || m_table.onPlayerTable[2] != 0 || m_table.onPlayerTable[3] != 0)
			{
				entity.GetTransform().position.y = static_cast<int>(entity.GetTransform().position.y) + 1.06f;
				entity.GetRigidbody().velocity.y = 0;
			}
		}
	}
}

void ElkPhysics::Managers::PhysicsManager::CheckCollisionXAxis(Data::Physics::PhysicsEntity& entity, double p_deltaTime)
{
	if(entity.GetRigidbody().velocity.x < 0)
	{
		if (m_table.xTable[0] != 0 || m_table.xTable[1] != 0 || m_table.xTable[2] != 0 || m_table.xTable[3] != 0 || m_table.xTable[4] != 0 || m_table.xTable[5] != 0)
		{
			float playerXposition = entity.GetTransform().position.x;
			float playerXvelocity = entity.GetRigidbody().velocity.x * static_cast<float>(p_deltaTime);
			if (playerXposition < 0)
			{
				if(abs(playerXposition - static_cast<int>(playerXposition)) <= 0.25f && abs((playerXposition + playerXvelocity) - static_cast<int>(playerXposition)) > 0.25f)
				{
					entity.GetRigidbody().velocity.x = 0;
				}
			}
			else if(playerXposition >= 0)
			{
				if(abs(playerXposition - static_cast<int>(playerXposition)) >= 0.75f && abs((playerXposition + playerXvelocity) - static_cast<int>(playerXposition)) < 0.75f)
				{
					entity.GetRigidbody().velocity.x = 0;
				}
			}
		}
	}
	else if(entity.GetRigidbody().velocity.x > 0)
	{
		if (m_table.xTable[6] != 0 || m_table.xTable[7] != 0 || m_table.xTable[8] != 0 || m_table.xTable[9] != 0 || m_table.xTable[10] != 0 || m_table.xTable[11] != 0)
		{
			float playerXposition = entity.GetTransform().position.x;
			float playerXvelocity = entity.GetRigidbody().velocity.x * static_cast<float>(p_deltaTime);
			if (playerXposition < 0)
			{
				if (abs(playerXposition - static_cast<int>(playerXposition)) >= 0.75f && abs(playerXposition + playerXvelocity - static_cast<int>(playerXposition)) < 0.75f)
				{
					entity.GetRigidbody().velocity.x = 0;
				}
			}
			else if (playerXposition >= 0)
			{
				if (abs(playerXposition - static_cast<int>(playerXposition)) <= 0.25f && abs(playerXposition + playerXvelocity - static_cast<int>(playerXposition)) > 0.25f)
				{
					entity.GetRigidbody().velocity.x = 0;
				}
			}
		}
	}
}

void ElkPhysics::Managers::PhysicsManager::CheckCollisionYAxis(Data::Physics::PhysicsEntity& entity, double p_deltaTime)
{
	if (entity.GetRigidbody().velocity.y < 0)
	{
		float playerYposition = entity.GetTransform().position.y - 1;
		float playerYvelocity = entity.GetRigidbody().velocity.y * static_cast<float>(p_deltaTime);
		if (m_table.yTable[0] != 0 || m_table.yTable[1] != 0 || m_table.yTable[2] != 0 || m_table.yTable[3] != 0)
		{
			if(playerYposition < 0)
			{
				if(abs(playerYposition - static_cast<int>(playerYposition)) <= 0.95f && abs(playerYposition + playerYvelocity - static_cast<int>(playerYposition)) >= 0.95f)
				{
					entity.GetRigidbody().velocity.y = 0;
				}
			}
			else if(playerYposition >= 0)
			{
				if (abs(playerYposition - static_cast<int>(playerYposition)) >= 0.05f && abs(playerYposition + playerYvelocity - static_cast<int>(playerYposition)) <= 0.05f)
				{
					entity.GetRigidbody().velocity.y = 0;
				}
			}
		}
	}
	else if (entity.GetRigidbody().velocity.y > 0)
	{
		float playerYposition = entity.GetTransform().position.y;
		float playerYvelocity = entity.GetRigidbody().velocity.y * static_cast<float>(p_deltaTime);
		if (m_table.yTable[4] != 0 || m_table.yTable[5] != 0 || m_table.yTable[6] != 0 || m_table.yTable[7] != 0)
		{
			if (playerYposition < 0)
			{
				if (abs(playerYposition - static_cast<int>(playerYposition)) >= 0.65f && abs(playerYposition + playerYvelocity - static_cast<int>(playerYposition)) <= 0.65f)
				{
					entity.GetRigidbody().velocity.y = 0;
				}
			}
			else if (playerYposition >= 0)
			{
				if (abs(playerYposition - static_cast<int>(playerYposition)) <= 0.35f && abs(playerYposition + playerYvelocity - static_cast<int>(playerYposition)) >= 0.35f)
				{
					entity.GetRigidbody().velocity.y = 0;
				}
			}
		}
	}
}

void ElkPhysics::Managers::PhysicsManager::CheckCollisionZAxis(Data::Physics::PhysicsEntity& entity, double p_deltaTime)
{
	if (entity.GetRigidbody().velocity.z < 0)
	{
		if (m_table.zTable[0] != 0 || m_table.zTable[1] != 0 || m_table.zTable[2] != 0 || m_table.zTable[3] != 0 || m_table.zTable[4] != 0 || m_table.zTable[5] != 0)
		{
			float playerZposition = entity.GetTransform().position.z;
			float playerZvelocity = entity.GetRigidbody().velocity.z * static_cast<float>(p_deltaTime);
			if (playerZposition < 0)
			{
				if (abs(playerZposition - static_cast<int>(playerZposition)) <= 0.25f && abs(playerZposition + playerZvelocity - static_cast<int>(playerZposition)) >= 0.25f)
				{
					entity.GetRigidbody().velocity.z = 0;
				}
			}
			else if (playerZposition >= 0)
			{
				if (abs(playerZposition - static_cast<int>(playerZposition)) >= 0.75f && abs(playerZposition + playerZvelocity - static_cast<int>(playerZposition)) <= 0.75f)
				{
					entity.GetRigidbody().velocity.z = 0;
				}
			}
		}
	}
	else if (entity.GetRigidbody().velocity.z > 0)
	{
		if (m_table.zTable[6] != 0 || m_table.zTable[7] != 0 || m_table.zTable[8] != 0 || m_table.zTable[9] != 0 || m_table.zTable[10] != 0 || m_table.zTable[11] != 0)
		{
			float playerZposition = entity.GetTransform().position.z;
			float playerZvelocity = entity.GetRigidbody().velocity.z * static_cast<float>(p_deltaTime);
			if (playerZposition < 0)
			{
				if (abs(playerZposition - static_cast<int>(playerZposition)) >= 0.75f && abs(playerZposition + playerZvelocity - static_cast<int>(playerZposition)) <= 0.75f)
				{
					entity.GetRigidbody().velocity.z = 0;
				}
			}
			else if (playerZposition >= 0)
			{
				if (abs(playerZposition - static_cast<int>(playerZposition)) <= 0.25f && abs(playerZposition + playerZvelocity - static_cast<int>(playerZposition)) >= 0.25f)
				{
					entity.GetRigidbody().velocity.z = 0;
				}
			}
		}
	}
}

void ElkPhysics::Managers::PhysicsManager::CheckCollisionOnDiagonal(Data::Physics::PhysicsEntity& p_entity,
	double p_deltaTime)
{
	if (p_entity.GetRigidbody().velocity.x < 0)
	{
		if(p_entity.GetRigidbody().velocity.z < 0)
		{
			if(m_table.diagonalTable[0] != 0 || m_table.diagonalTable[1] != 0 || m_table.diagonalTable[2] != 0)
			{
				float playerXposition = p_entity.GetTransform().position.x;
				float playerZposition = p_entity.GetTransform().position.z;
				float playerXvelocity = p_entity.GetRigidbody().velocity.x * static_cast<float>(p_deltaTime);
				float playerZvelocity = p_entity.GetRigidbody().velocity.z * static_cast<float>(p_deltaTime);
				if(playerXposition < 0 && abs(playerXposition - static_cast<int>(playerXposition)) <= 0.25f && abs((playerXposition + playerXvelocity) - static_cast<int>(playerXposition)) > 0.25f)
				{
					if(playerZposition < 0 && abs(playerZposition - static_cast<int>(playerZposition)) <= 0.25f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) > 0.25f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
					else if(playerZposition >= 0 && abs(playerZposition - static_cast<int>(playerZposition)) >= 0.75f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) < 0.75f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
				}
				else if(playerXposition >= 0 && abs(playerXposition - static_cast<int>(playerXposition)) >= 0.75f && abs((playerXposition + playerXvelocity) - static_cast<int>(playerXposition)) < 0.75f)
				{
					if (playerZposition < 0 && abs(playerZposition - static_cast<int>(playerZposition)) <= 0.25f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) > 0.25f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
					else if (playerZposition >= 0 && abs(playerZposition - static_cast<int>(playerZposition)) >= 0.75f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) < 0.75f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
				}
			}
		}
		else if(p_entity.GetRigidbody().velocity.z > 0)
		{
			if (m_table.diagonalTable[3] != 0 || m_table.diagonalTable[4] != 0 || m_table.diagonalTable[5] != 0)
			{
				float playerXposition = p_entity.GetTransform().position.x;
				float playerZposition = p_entity.GetTransform().position.z;
				float playerXvelocity = p_entity.GetRigidbody().velocity.x * static_cast<float>(p_deltaTime);
				float playerZvelocity = p_entity.GetRigidbody().velocity.z * static_cast<float>(p_deltaTime);
				if (playerXposition < 0 && abs(playerXposition - static_cast<int>(playerXposition)) <= 0.25f && abs((playerXposition + playerXvelocity) - static_cast<int>(playerXposition)) > 0.25f)
				{
					if (playerZposition < 0 && abs(playerZposition - static_cast<int>(playerZposition)) >= 0.75f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) < 0.75f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
					else if (playerZposition >= 0 && abs(playerZposition - static_cast<int>(playerZposition)) <= 0.25f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) > 0.25f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
				}
				else if (playerXposition >= 0 && abs(playerXposition - static_cast<int>(playerXposition)) >= 0.75f && abs((playerXposition + playerXvelocity) - static_cast<int>(playerXposition)) < 0.75f)
				{
					if (playerZposition < 0 && abs(playerZposition - static_cast<int>(playerZposition)) >= 0.75f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) < 0.75f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
					else if (playerZposition >= 0 && abs(playerZposition - static_cast<int>(playerZposition)) <= 0.25f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) > 0.25f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
				}
			}
		}
	}
	else if(p_entity.GetRigidbody().velocity.x > 0)
	{
		if (p_entity.GetRigidbody().velocity.z < 0)
		{
			if (m_table.diagonalTable[6] != 0 || m_table.diagonalTable[7] != 0 || m_table.diagonalTable[8] != 0)
			{
				float playerXposition = p_entity.GetTransform().position.x;
				float playerZposition = p_entity.GetTransform().position.z;
				float playerXvelocity = p_entity.GetRigidbody().velocity.x * static_cast<float>(p_deltaTime);
				float playerZvelocity = p_entity.GetRigidbody().velocity.z * static_cast<float>(p_deltaTime);
				if (playerXposition < 0 && abs(playerXposition - static_cast<int>(playerXposition)) >= 0.75f && abs((playerXposition + playerXvelocity) - static_cast<int>(playerXposition)) < 0.75f)
				{
					if (playerZposition < 0 && abs(playerZposition - static_cast<int>(playerZposition)) <= 0.25f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) > 0.25f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
					else if (playerZposition >= 0 && abs(playerZposition - static_cast<int>(playerZposition)) >= 0.75f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) < 0.75f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
				}
				else if (playerXposition >= 0 && abs(playerXposition - static_cast<int>(playerXposition)) <= 0.25f && abs((playerXposition + playerXvelocity) - static_cast<int>(playerXposition)) > 0.25f)
				{
					if (playerZposition < 0 && abs(playerZposition - static_cast<int>(playerZposition)) <= 0.25f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) > 0.25f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
					else if (playerZposition >= 0 && abs(playerZposition - static_cast<int>(playerZposition)) >= 0.75f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) < 0.75f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
				}
			}
		}
		else if (p_entity.GetRigidbody().velocity.z > 0)
		{
			if (m_table.diagonalTable[9] != 0 || m_table.diagonalTable[10] != 0 || m_table.diagonalTable[11] != 0)
			{
				float playerXposition = p_entity.GetTransform().position.x;
				float playerZposition = p_entity.GetTransform().position.z;
				float playerXvelocity = p_entity.GetRigidbody().velocity.x * static_cast<float>(p_deltaTime);
				float playerZvelocity = p_entity.GetRigidbody().velocity.z * static_cast<float>(p_deltaTime);
				if (playerXposition < 0 && abs(playerXposition - static_cast<int>(playerXposition)) >= 0.75f && abs((playerXposition + playerXvelocity) - static_cast<int>(playerXposition)) < 0.75f)
				{
					if (playerZposition < 0 && abs(playerZposition - static_cast<int>(playerZposition)) >= 0.75f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) < 0.75f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
					else if (playerZposition >= 0 && abs(playerZposition - static_cast<int>(playerZposition)) <= 0.25f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) > 0.25f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
				}
				else if (playerXposition >= 0 && abs(playerXposition - static_cast<int>(playerXposition)) <= 0.25f && abs((playerXposition + playerXvelocity) - static_cast<int>(playerXposition)) > 0.25f)
				{
					if (playerZposition < 0 && abs(playerZposition - static_cast<int>(playerZposition)) >= 0.75f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) < 0.75f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
					else if (playerZposition >= 0 && abs(playerZposition - static_cast<int>(playerZposition)) <= 0.25f && abs((playerZposition + playerZvelocity) - static_cast<int>(playerZposition)) > 0.25f)
					{
						p_entity.GetRigidbody().velocity.x = 0;
						p_entity.GetRigidbody().velocity.z = 0;
					}
				}
			}
		}
	}

}

void ElkPhysics::Managers::PhysicsManager::ApplyPhysics()
{
	PROFILER_SPY("PhysicsManager::ApplyPhysics");

	ApplyGravity(ElkTools::Utils::Time::GetDeltaTime());
	HandleCollisions(ElkTools::Utils::Time::GetDeltaTime());
	MoveEntites(ElkTools::Utils::Time::GetDeltaTime());
	CheckEntityInBlock();
}

void ElkPhysics::Managers::PhysicsManager::ApplyGravity(double p_deltaTime)
{
	PROFILER_SPY("PhysicsManager::ApplyGravity");

	for (Data::Physics::PhysicsEntity& entity : m_entities)
		if (!entity.GetRigidbody().kinematic)
			entity.GetRigidbody().velocity += glm::vec3(0, -1, 0) * (m_gravity * static_cast<float>(p_deltaTime)) * entity.GetRigidbody().gravityScale;
}

void ElkPhysics::Managers::PhysicsManager::HandleCollisions(double p_deltaTime)
{
	PROFILER_SPY("PhysicsManager::HandleCollisions");
	for (Data::Physics::PhysicsEntity& entity : m_entities)
	{
		if (!entity.GetRigidbody().kinematic)
		{
			CheckCollisionXAxis(entity, p_deltaTime);
			CheckCollisionYAxis(entity, p_deltaTime);
			CheckCollisionZAxis(entity, p_deltaTime);
			CheckCollisionOnDiagonal(entity, p_deltaTime);
		}
	}
}

uint16_t ElkPhysics::Managers::PhysicsManager::GetNumberOfEntities() const
{
	return static_cast<unsigned int>(m_entities.size());
}

CollisionTable& ElkPhysics::Managers::PhysicsManager::GetCollisionTable()
{
	return m_table;
}