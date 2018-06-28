#pragma once

#include <glm\glm.hpp>

#include <ElkAPI/IDataStructure.h>
#include <ElkTools\Utils\IniLoader.h>

#include "ElkRendering/export.h"

namespace ElkRendering
{
	namespace Data
	{
		// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
		class ELK_RENDERING_EXPORT CameraData : public ElkAPI::IDataStructure
		{
		private:
			// CameraData Attributes
			glm::vec3 m_position;
			glm::vec3 m_forward;
			glm::vec3 m_up;
			glm::vec3 m_right;
			glm::vec3 m_worldUp;

			// CameraData options
			float m_zoom;

		public:
			// Constructor with vectors
			explicit CameraData(const glm::vec3 p_position = glm::vec3(0.0f, 0.0f, 5.0f),
			                    const glm::vec3 p_up = glm::vec3(0.0f, 1.0f, 0.0f));

			// Constructor with scalar values
			CameraData(const float p_posX, const float p_posY, const float p_posZ,
			           const float p_upX, const float p_upY, const float p_upZ);

			virtual ~CameraData() = default;

			void Setup() override;

			// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
			glm::mat4 GetViewMatrix() const;

			const glm::vec3& GetPosition() const { return m_position; }
			void SetPosition(const glm::vec3& p_position) { m_position = p_position; }
			const glm::vec3& GetFront() const { return m_forward; }
			void SetForward(const glm::vec3& p_front) { m_forward = p_front; }
			const glm::vec3& GetUp() const { return m_up; }
			void SetUp(const glm::vec3& p_up) { m_up = p_up; }
			const glm::vec3& GetRight() const { return m_right; }
			void SetRight(const glm::vec3& p_right) { m_right = p_right; }
			const glm::vec3& GetWorldUp() const { return m_worldUp; }
			void SetWorldUp(const glm::vec3& p_worldUp) { m_worldUp = p_worldUp; }
			float GetZoom() const { return m_zoom; }
			void SetZoom(const float p_zoom) { m_zoom = p_zoom; }
			void ResetZoom();
		};
	}
}
