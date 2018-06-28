#include "stdafx.h"

#include "ElkRendering/Resources/RenderText.h"

using namespace ElkRendering::Resources;

RenderText::RenderText()
{
	Setup();
}

void RenderText::Setup()
{
	InitPositions();
	InitTextureCoordinates();
	InitIndices();
	InitBuffers();
}

void RenderText::Draw() const
{
	if (m_string != "")
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(m_string.size()));
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void RenderText::UpdateSSBO() const
{
	if (m_string != "")
	{
		std::vector<GLuint> textAsUint;

		for (char c : m_string)
			textAsUint.push_back(static_cast<GLuint>(c));

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * textAsUint.size(), textAsUint.data(), GL_DYNAMIC_DRAW);
		GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		memcpy(p, textAsUint.data(), sizeof(GLuint) * textAsUint.size());
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}
}

void ElkRendering::Resources::RenderText::SetString(const std::string & p_string)
{
	m_string = p_string;

	UpdateSSBO();
}

std::string ElkRendering::Resources::RenderText::GetString()
{
	return m_string;
}

void RenderText::InitBuffers()
{
	// VAO stuffs
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// VBO stuffs
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(PositionAndTexture), m_positionAndTexture, GL_STATIC_DRAW);

	// EBO stuffs
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), m_indices, GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PositionAndTexture), nullptr);

	// vertex textures
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(PositionAndTexture),
		reinterpret_cast<void*>(offsetof(PositionAndTexture, textureCoordinates)));

	// SSBO
	glGenBuffers(1, &m_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);

	// Unbind all
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RenderText::InitPositions()
{
	m_positionAndTexture[0].position[0] = -1;
	m_positionAndTexture[0].position[1] = -1;
	m_positionAndTexture[0].position[2] = 0;

	m_positionAndTexture[1].position[0] = 1;
	m_positionAndTexture[1].position[1] = -1;
	m_positionAndTexture[1].position[2] = 0;

	m_positionAndTexture[2].position[0] = 1;
	m_positionAndTexture[2].position[1] = 1;
	m_positionAndTexture[2].position[2] = 0;

	m_positionAndTexture[3].position[0] = -1;
	m_positionAndTexture[3].position[1] = 1;
	m_positionAndTexture[3].position[2] = 0;
}

void RenderText::InitTextureCoordinates()
{
	m_positionAndTexture[0].textureCoordinates[0] = 0;
	m_positionAndTexture[0].textureCoordinates[1] = 0;

	m_positionAndTexture[1].textureCoordinates[0] = 1;
	m_positionAndTexture[1].textureCoordinates[1] = 0;

	m_positionAndTexture[2].textureCoordinates[0] = 1;
	m_positionAndTexture[2].textureCoordinates[1] = 1;

	m_positionAndTexture[3].textureCoordinates[0] = 0;
	m_positionAndTexture[3].textureCoordinates[1] = 1;
}

void RenderText::InitIndices()
{
	m_indices[0] = 0;
	m_indices[1] = 1;
	m_indices[2] = 2;
	m_indices[3] = 2;
	m_indices[4] = 3;
	m_indices[5] = 0;
}
