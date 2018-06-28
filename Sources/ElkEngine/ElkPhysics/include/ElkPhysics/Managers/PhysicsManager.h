#pragma once

#include <vector>

#include <ElkAPI\IManager.h>

#include "ElkPhysics/export.h"
#include "ElkPhysics/Data/Physics/PhysicsEntity.h"
#include "ElkPhysics/Data/CollisionTable.h"

using namespace ElkPhysics::Data::Physics;
using namespace ElkPhysics::Data;

namespace ElkPhysics
{
	namespace Managers
	{
		class ELK_PHYSICS_EXPORT PhysicsManager : public ElkAPI::IManager
		{
		public:
			PhysicsManager() { Setup(); }
			~PhysicsManager() { Close(); }

			void Setup();
			void Close();

			void ClearPhysicsEntities();
			void AddEntity(TransformData& p_transform, RigidbodyData& p_rigidbody);

			void ApplyPhysics();
			void ApplyGravity(double p_deltaTime);
			void HandleCollisions(double p_deltaTime);
			void MoveEntites(double p_deltaTime);
			void SetGravity(float p_value);
			void CheckEntityInBlock();
			void CheckCollisionXAxis(Data::Physics::PhysicsEntity& p_entity, double p_deltaTime);
			void CheckCollisionYAxis(Data::Physics::PhysicsEntity& p_entity, double p_deltaTime);
			void CheckCollisionZAxis(Data::Physics::PhysicsEntity& p_entity, double p_deltaTime);
			void CheckCollisionOnDiagonal(Data::Physics::PhysicsEntity& p_entity, double p_deltaTime);
			uint16_t GetNumberOfEntities() const;
			CollisionTable& GetCollisionTable();

		private:
			std::vector<Data::Physics::PhysicsEntity> m_entities;
			CollisionTable m_table;
			float m_gravity;
		};
	}
}