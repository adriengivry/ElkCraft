#include "stdafx.h"

#include <ElkTools/Utils/Profiler.h>
#include <ElkTools/Utils/ManagerLocator.h>

#include "ElkRendering/Managers/ShaderManager.h"
#include "ElkRendering/Resources/RenderChunk.h"
#include "ElkRendering/Debug/GLDebug.h"

using namespace ElkRendering::Resources;
using namespace ElkRendering::Data;

RenderChunk::RenderChunk()
{
	Setup();
}

void RenderChunk::Setup()
{
	BlockGeometry::Setup();

	auto& vertices = BlockGeometry::GetVertices();

	// VAO Stuffs
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// VBO stuffs
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	/* location 0 (position) */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	/* location 1 (texture coordinates) */
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, textureCoord)));
	/* location 2 (normals) */
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normals)));

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

void RenderChunk::Draw() const
{
	if (m_toDraw)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);

		glBindVertexArray(m_vao);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, static_cast<GLsizei>(m_blocksToRender.size()));
		glBindVertexArray(0);
	}
}

void ElkRendering::Resources::RenderChunk::SetToDraw(bool p_value)
{
	m_toDraw = p_value;
}

void ElkRendering::Resources::RenderChunk::Update()
{
	m_blocksToRender.clear();
	m_blocksToRender.insert(m_blocksToRender.end(), m_opaqueBlocks.begin(), m_opaqueBlocks.end());
	m_blocksToRender.insert(m_blocksToRender.end(), m_transparentBlocks.begin(), m_transparentBlocks.end());

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_blocksToRender.size() * sizeof(GLuint), m_blocksToRender.data(), GL_DYNAMIC_DRAW);
	GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memcpy(p, m_blocksToRender.data(), m_blocksToRender.size() * sizeof(GLuint));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void ElkRendering::Resources::RenderChunk::ClearBlocks()
{
	m_opaqueBlocks.clear();
	m_transparentBlocks.clear();
}

void ElkRendering::Resources::RenderChunk::AddBlock(const uint8_t& p_x, const uint8_t& p_y, const uint8_t& p_z, const uint8_t& p_value, const uint8_t& p_lightLevel, const bool& p_transparent)
{
	std::vector<GLuint>& targetVector = p_transparent ? m_transparentBlocks : m_opaqueBlocks;

	targetVector.push_back(static_cast<GLuint>(p_x | p_y << 4 | p_z << 8 | p_lightLevel << 12 | p_value << 20));
}