#pragma once

#include <vector>

#include "ElkRendering/export.h"
#include "ElkRendering/Data/Vertex.h"
#include "ElkRendering/Resources/Texture.h"
#include "ElkRendering/Resources/Shader.h"
#include "ElkRendering/Data/BlockGeometry.h"

namespace ElkRendering
{
	namespace Resources
	{
		class ELK_RENDERING_EXPORT RenderChunk
		{
		public:
			RenderChunk();
			~RenderChunk() = default;

			void Setup();
			void Draw() const;
			void Update();

			void SetToDraw(bool p_value);

			void ClearBlocks();
			void AddBlock(const uint8_t& p_x, const uint8_t& p_y, const uint8_t& p_z, const uint8_t& p_value, const uint8_t& p_lightLevel, const bool& p_transparent);

		private:
			/* Buffers */
			GLuint m_vbo;
			GLuint m_vao;
			GLuint m_ssbo;

			std::vector<GLuint> m_transparentBlocks;
			std::vector<GLuint> m_opaqueBlocks;
			std::vector<GLuint> m_blocksToRender;

			bool m_toDraw;
		};
	}
}
