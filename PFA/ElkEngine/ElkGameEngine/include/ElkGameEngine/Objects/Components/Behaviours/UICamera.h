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
				* Camera component used to view the UI
				*/
				class ELK_GAMEENGINE_EXPORT UICamera : public ABehaviour
				{
				public:
					UICamera();
					virtual ~UICamera() = default;
				};
			}
		}
	}
}