#pragma once

#include <ElkTools/Utils/ManagerLocator.h>
#include <ElkTools/Debug/Log.h>

#include "ElkGameEngine/export.h"
#include "ElkGameEngine/Objects/Components/Behaviours/ABehaviour.h"
#include "ElkGameEngine/Managers/InputManager.h"

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
				class ELK_GAMEENGINE_EXPORT AButton : public ABehaviour
				{
				public:
					AButton();
					virtual ~AButton() = default;

					void Update() override;
					void SetPosition(float p_x, float p_y);
					void SetExtends(float p_x, float p_y);

					bool MouseIsOver();

					virtual void OnClick() = 0;
					virtual void OnMouseOver() = 0;
					virtual void OnMouseNotOver() = 0;

				private:
					ElkGameEngine::Managers::InputManager& m_inputManager;
					glm::vec2 m_screenPosition;
					glm::vec2 m_extends;

					bool m_mouseIsOver;
				};
			}
		}
	}
}