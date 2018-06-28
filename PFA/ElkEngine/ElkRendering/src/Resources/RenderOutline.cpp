#include "stdafx.h"

#include <ElkTools/Utils/Profiler.h>
#include <ElkTools/Utils/ManagerLocator.h>

#include "ElkRendering/Managers/ShaderManager.h"
#include "ElkRendering/Resources/RenderOutline.h"
#include "ElkRendering/Debug/GLDebug.h"

using namespace ElkRendering::Resources;
using namespace ElkRendering::Data;

RenderOutline::RenderOutline()
{
	Setup();
}

void RenderOutline::Setup()
{
	InitCubePositions();
	InitCubeVertices();

	// VAO Stuffs
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// VBO stuffs
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), m_vertices.data(), GL_STATIC_DRAW);
	/* location 0 (position) */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

	// Unbind all
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderOutline::Draw() const
{
	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_vertices.size()));
	glBindVertexArray(0);
}

void ElkRendering::Resources::RenderOutline::InitCubePositions()
{
	m_positions[0] = glm::vec3(-0.5, +0.5, -0.5); // A,V,O
	m_positions[1] = glm::vec3(-0.5, +0.5, +0.5); // C,X,E
	m_positions[2] = glm::vec3(+0.5, +0.5, +0.5); // D,S,F
	m_positions[3] = glm::vec3(+0.5, +0.5, -0.5); // B,Q,P
	m_positions[4] = glm::vec3(-0.5, -0.5, -0.5); // U,K,M
	m_positions[5] = glm::vec3(-0.5, -0.5, +0.5); // W,G,I
	m_positions[6] = glm::vec3(+0.5, -0.5, +0.5); // T,H,J
	m_positions[7] = glm::vec3(+0.5, -0.5, -0.5); // R,L,N
}

void ElkRendering::Resources::RenderOutline::InitCubeVertices()
{
	/* Top */
	CreateLine(0, 3);
	CreateLine(3, 2);
	CreateLine(2, 1);
	CreateLine(1, 0);

	/* Bottom */
	CreateLine(7, 4);
	CreateLine(4, 5);
	CreateLine(5, 6);
	CreateLine(6, 7);

	/* Sides */
	CreateLine(5, 1);
	CreateLine(6, 2);
	CreateLine(4, 0);
	CreateLine(7, 3);
}

void ElkRendering::Resources::RenderOutline::CreateLine(uint8_t pos1, uint8_t pos2)
{
	m_vertices.emplace_back(m_positions[pos1]);
	m_vertices.emplace_back(m_positions[pos2]);
}