#pragma once

#include <ElkPhysics/Data/Physics/RigidBodyData.h>

#include "ElkGameEngine/export.h"
#include "ElkGameEngine/Objects/Components/Behaviours/ABehaviour.h"

namespace ElkGameEngine
{
	namespace Objects
	{
		namespace Components
		{
			namespace Behaviours
			{
				class ELK_GAMEENGINE_EXPORT Rigidbody : public Objects::Components::Behaviours::ABehaviour
				{
				public:
					Rigidbody();
					virtual ~Rigidbody() = default;

					ElkPhysics::Data::Physics::RigidbodyData& GetRigidbodyData() { return m_rigidbodyData; }

					const glm::vec3& GetVelocity();
					void SetVelocity(const glm::vec3& p_value);

					void SetGravityScale(float p_gravityScale);
					float GetGravityScale();

					void SetKinematic(bool p_state);
					bool IsKinematic();
				private:
					ElkPhysics::Data::Physics::RigidbodyData m_rigidbodyData;
				};
			}
		}
	}
}
