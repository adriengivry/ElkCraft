#pragma once

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
				/*
				* Camera component used to view the scene
				*/
				class ELK_GAMEENGINE_EXPORT Camera : public ABehaviour
				{
				public:
					Camera();
					virtual ~Camera() = default;
				};
			}
		}
	}
}