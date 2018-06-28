#pragma once

#include <vector>

#include <glm\glm.hpp>

#include "ElkRendering/export.h"
#include "ElkRendering/Data/Vertex.h"

namespace ElkRendering
{
	namespace Data
	{
		class ELK_RENDERING_EXPORT BlockGeometry
		{
		public:
			static void Setup();
			static std::vector<ElkRendering::Data::Vertex>& GetVertices();

		private:
			static void InitCubePositions();
			static void InitCubeTextureCoordinates();
			static void InitCubeNormals();
			static void InitCubeVertices();
			static void CreateTriangle(uint8_t pos1, uint8_t pos2, uint8_t pos3, uint8_t text1, uint8_t text2, uint8_t text3, uint8_t p_normals);

			static std::vector<ElkRendering::Data::Vertex> m_vertices;
			static glm::vec3 m_positions[8];
			static glm::vec3 m_normals[6];
			static glm::vec2 m_textureCoordinates[4];
			static bool m_initialized;
		};
	}
}
