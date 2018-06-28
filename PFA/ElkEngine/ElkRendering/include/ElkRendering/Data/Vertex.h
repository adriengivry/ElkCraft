#pragma once

#include <glm\glm.hpp>

#include <ElkAPI\IDataStructure.h>

namespace ElkRendering
{
	namespace Data
	{
		struct Vertex
		{
			Vertex() = default;
			Vertex(glm::vec3 p_position, glm::vec2 p_textureCoord, glm::vec3 p_normals)
			{
				position = p_position;
				textureCoord = p_textureCoord;
				normals = p_normals;
			}

			glm::vec3 position;
			glm::vec2 textureCoord;
			glm::vec3 normals;
		};
	}
}
