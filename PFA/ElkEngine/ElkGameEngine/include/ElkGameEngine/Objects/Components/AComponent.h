#pragma once

#include "ElkGameEngine/export.h"
#include "ElkGameEngine/Objects/AObject.h"

namespace ElkGameEngine
{
	namespace Objects
	{
		class GameObject;

		namespace Components
		{

			/*
			* The base class for every components in GyvrEngine
			*/
			class ELK_GAMEENGINE_EXPORT AComponent : public AObject
			{
			public:
				AComponent();
				virtual ~AComponent() = default;

				virtual void Update() {}

				void SetOwner(ElkGameEngine::Objects::GameObject& p_owner);
				ElkGameEngine::Objects::GameObject& GetOwner() const;

			protected:
				ElkGameEngine::Objects::GameObject* m_owner = nullptr;
			};
		}
	}
}

