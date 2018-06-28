#pragma once

#include <ElkPhysics/Data/Physics/TransformData.h>

#include "ElkGameEngine/export.h"
#include "ElkGameEngine/Objects/Components/AComponent.h"

namespace ElkGameEngine
{
	namespace Objects
	{
		namespace Components
		{
			/*
			* Position, rotation and scale of any GameObject
			*/
			class ELK_GAMEENGINE_EXPORT Transform : public AComponent
			{
			public:
				Transform();
				explicit Transform(const glm::vec3& p_position);
				explicit Transform(const glm::vec3& p_position, const glm::vec3& p_rotation);
				explicit Transform(const glm::vec3& p_position, const glm::vec3& p_rotation, const glm::vec3& p_scale);
				virtual ~Transform() = default;

				glm::vec3& GetPosition();
				glm::vec3& GetRotation();
				glm::vec3& GetScale();

				glm::vec3 GetForward() const;
				glm::vec3 GetRight() const;
				glm::vec3 GetUp() const;

				void UpdateForwardRightUp();

				void SetPosition(const glm::vec3& p_position);
				void SetRotation(const glm::vec3& p_rotation);
				void SetForward(const glm::vec3& p_forward);
				void SetScale(const glm::vec3& p_scale);

				void Translate(const glm::vec3& p_translate);
				void Rotate(const glm::vec3& p_rotate);
				void Scale(const glm::vec3& p_scale);

				ElkPhysics::Data::Physics::TransformData& GetTransformData() { return m_transformData; }

			private:
				ElkPhysics::Data::Physics::TransformData m_transformData;
				glm::vec3 m_forward;
				glm::vec3 m_right;
				glm::vec3 m_up;
			};
		}
	}
}
