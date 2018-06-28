#pragma once

#include <vector>

#include "ElkRendering/export.h"
#include "ElkRendering/Data/Vertex.h"
#include "ElkRendering/Resources/Texture.h"
#include "ElkRendering/Resources/Shader.h"

namespace ElkRendering
{
	namespace Resources
	{
		class ELK_RENDERING_EXPORT RenderOutline
		{
		public:
			RenderOutline();
			~RenderOutline() = default;

			void Setup();
			void Draw() const;

		private:
			void InitCubePositions();
			void InitCubeVertices();
			void CreateLine(uint8_t pos1, uint8_t pos2);

			/* Buffers */
			GLuint m_vbo;
			GLuint m_vao;

			std::vector<glm::vec3> m_vertices;
			glm::vec3 m_positions[8];
		};
	}
}
