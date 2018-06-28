#include "stdafx.h"
#include "ElkRendering/Resources/RenderSprite.h"

using namespace ElkRendering::Resources;


RenderSprite::RenderSprite()
{
	Setup();
}

void RenderSprite::Setup()
{
	m_texture = nullptr;

	InitSpritePositions();
	InitSpriteTextureCoordinates();
	InitSpriteIndices();
	InitBuffers();

	SetToDraw(true);
}

void RenderSprite::Draw() const
{
	if (m_toDraw)
	{
		if (m_texture)
			m_texture->Bind();

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		if (m_texture)
			m_texture->Unbind();
	}
}

void RenderSprite::Update()
{
}

void RenderSprite::SetToDraw(bool p_value)
{
	m_toDraw = p_value;
}

Texture* ElkRendering::Resources::RenderSprite::GetTexture()
{
	return m_texture;
}

void ElkRendering::Resources::RenderSprite::SetTexture(Texture& p_texture)
{
	m_texture = &p_texture;
}

glm::vec3 RenderSprite::GetCoordinateMin() const
{
	return glm::vec3(m_positionAndTexture[0].textureCoordinates[0],
	                 m_positionAndTexture[0].textureCoordinates[1],
	                 m_positionAndTexture[0].textureCoordinates[2]);
}

glm::vec3 RenderSprite::GetCoordinateMax() const
{
	return glm::vec3(m_positionAndTexture[2].textureCoordinates[0],
	                 m_positionAndTexture[2].textureCoordinates[1],
	                 m_positionAndTexture[2].textureCoordinates[2]);
}

void RenderSprite::InitBuffers()
{
	// VAO stuffs
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// VBO stuffs
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(PositionAndTexture), m_positionAndTexture, GL_STATIC_DRAW);

	// EBO struffs
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

	// Unbind all
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderSprite::InitSpritePositions()
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

void RenderSprite::InitSpriteTextureCoordinates()
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

void RenderSprite::InitSpriteIndices()
{
	m_indices[0] = 0;
	m_indices[1] = 1;
	m_indices[2] = 2;
	m_indices[3] = 2;
	m_indices[4] = 3;
	m_indices[5] = 0;
}
