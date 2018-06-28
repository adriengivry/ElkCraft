#pragma once

#include <ElkAPI\IManager.h>

#include "ElkRendering/export.h"
#include "ElkRendering/Managers/TextureManager.h"
#include "ElkRendering/Managers/ShaderManager.h"
#include "ElkRendering/Data/CameraData.h"
#include "ElkRendering/Resources/RenderChunk.h"
#include "ElkRendering/Resources/RenderSprite.h"
#include "ElkRendering/Resources/RenderOutline.h"
#include "ElkRendering/Resources/RenderText.h"

namespace ElkRendering
{
	namespace Resources
	{
		class Model;
	}
}

namespace ElkRendering
{
	namespace Managers
	{
		/*
		* The renderer is pretty usefull to print things on the screen !
		*/
		class ELK_RENDERING_EXPORT RenderingManager : public ElkAPI::IManager
		{
		public:
			enum class RenderMode : int
			{
				DEFAULT = 0,
				WIREFRAME = 1,
				NORMALS = 2
			};

			RenderingManager(const uint16_t& p_windowWidth, const uint16_t& p_windowHeight);
			~RenderingManager();

			void Setup() override;
			void Close() override;

			void SetCamera(const glm::vec3& p_position, const glm::vec3& p_forward, const glm::vec3& p_right,
			               const glm::vec3& p_up);
			void SetUICamera(const glm::vec3& p_position, const glm::vec3& p_forward, const glm::vec3& p_right,
			                 const glm::vec3& p_up);
			void SetWindowSize(const uint16_t& p_width, const uint16_t& p_height);
			void SetRenderMode(const RenderMode& p_renderMode);

			inline glm::mat4 CalculateView() const;
			inline glm::mat4 CalculateUIView() const;
			glm::mat4 CalculateProjectionperspective() const;
			glm::mat4 CalculateProjectionOrtho() const;

			void SetDaytimeLightCoefficient(float p_coefficient);

			ElkRendering::Data::CameraData& GetCamera() { return m_camera; }

			void PrepareChunkDraw();
			void DrawChunk(const ElkRendering::Resources::RenderChunk& p_chunk, const glm::vec3& p_position);
			void EndChunkDraw();

			void PrepareSpriteDraw();
			void DrawSprite(const ElkRendering::Resources::RenderSprite& p_sprite, const glm::vec3& p_position, const glm::vec3& p_scale, const glm::vec4& p_color);
			void EndSpriteDraw();

			void PrepareStringDraw();
			void DrawString(const ElkRendering::Resources::RenderText& p_text, const glm::vec3& p_position, const glm::vec3& p_scale, float p_spacing, const glm::vec4& p_color);
			void EndStringDraw();

			void SetOutlinePosition(glm::vec3 p_position);
			void SetOutlineState(bool p_state);
			void SetDestructionInfo(glm::vec3 p_position, uint8_t p_value);

		private:
			void UpdateRenderMode();

			Resources::Shader* m_currentShader;
			Resources::Texture* m_currentTexture;

			RenderMode m_renderMode;

			std::unique_ptr<TextureManager> m_textureManager;
			std::unique_ptr<ShaderManager> m_shaderManager;

			Data::CameraData m_camera;
			Data::CameraData m_UICamera;

			bool m_outlineIsEnabled;
			Resources::RenderOutline m_outline;

			uint16_t m_width;
			uint16_t m_height;

			glm::vec3 m_currentDestructionPosition;
			uint8_t m_currentDestructionLevel;

			float m_daytimeLightCoefficient;

			float m_near;
			float m_far;
		};
	}
}
