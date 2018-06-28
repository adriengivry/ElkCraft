#pragma once

#include "ElkGameEngine/export.h"
#include "ElkGameEngine/Objects/Components/AComponent.h"

namespace ElkGameEngine
{
	namespace Objects
	{
		namespace Components
		{
			namespace Behaviours
			{
				/*
				 * The base class for every components that can be enabled/disabled
				 */
				class ELK_GAMEENGINE_EXPORT ABehaviour : public AComponent
				{
				public:
					ABehaviour();
					virtual ~ABehaviour() = default;

					bool IsActive() const;
					void SetActive(bool p_state);

				protected:
					bool m_active;
				};
			}
		}
	}
}