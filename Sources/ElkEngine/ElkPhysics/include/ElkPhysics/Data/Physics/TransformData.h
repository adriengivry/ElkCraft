#pragma once

#include "ElkPhysics/export.h"
#include "ElkPhysics/Math/Math.h"

namespace ElkPhysics
{
	namespace Data
	{
		namespace Physics
		{
			struct ELK_PHYSICS_EXPORT TransformData
			{
				glm::vec3 position	= glm::vec3(0.f);
				glm::vec3 rotation	= glm::vec3(0.f);
				glm::vec3 scale		= glm::vec3(1.f);
			};
		}
	}
}