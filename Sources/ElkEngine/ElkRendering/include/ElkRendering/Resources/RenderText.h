#pragma once

#include "ElkRendering/export.h"

#include "ElkRendering/Resources/Texture.h"

namespace ElkRendering
{
	namespace Resources
	{
		class ELK_RENDERING_EXPORT RenderText
		{
		public:
			struct ELK_RENDERING_EXPORT PositionAndTexture
			{
				float position[3];
				float textureCoordinates[2];
			};

			RenderText();
			~RenderText() = default;

			void Setup();
			void Draw() const;
			void UpdateSSBO() const;

			void SetString(const std::string& p_string);
			std::string GetString();

		private:
			void InitBuffers();
			void InitPositions();
			void InitTextureCoordinates();
			void InitIndices();

			/* Buffers */
			GLuint m_vao;
			GLuint m_vbo;
			GLuint m_ssbo;
			GLuint m_ebo;

			PositionAndTexture m_positionAndTexture[4];

			std::string m_string;

			unsigned int m_indices[6];
		};
	}
}
