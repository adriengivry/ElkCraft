#pragma once

#include "ElkPhysics/export.h"
#include "ElkPhysics/Math/Math.h"

namespace ElkPhysics
{
	namespace Data
	{
		namespace Physics
		{
			struct ELK_PHYSICS_EXPORT RigidbodyData
			{
				glm::vec3 velocity	= glm::vec3(0.f);
				float gravityScale	= 1.f;
				bool kinematic		= false;
			};
		}
	}
}