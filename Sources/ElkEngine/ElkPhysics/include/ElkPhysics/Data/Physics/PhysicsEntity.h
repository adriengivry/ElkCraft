#pragma once

#include "ElkPhysics/export.h"
#include "ElkPhysics/Data/Physics/TransformData.h"
#include "ElkPhysics/Data/Physics/RigidbodyData.h"

namespace ElkPhysics
{
	namespace Data
	{
		namespace Physics
		{
			class ELK_PHYSICS_EXPORT PhysicsEntity
			{
			public:
				explicit PhysicsEntity(Physics::TransformData& p_transform, Physics::RigidbodyData& p_rigidBody);
				~PhysicsEntity() = default;

				Physics::TransformData& GetTransform();
				Physics::RigidbodyData& GetRigidbody();

			private:
				Physics::TransformData* m_transform;
				Physics::RigidbodyData* m_rigidbody;
			};
		}
	}
}