#include "stdafx.h"

#include "ElkGameEngine/Objects/Components/Behaviours/Sprite.h"

using namespace ElkGameEngine::Objects::Components::Behaviours;

Sprite::Sprite()
{
	SetName("Sprite");

	m_renderLayer = 0;
	m_color = glm::vec4(1.f);
}

ElkRendering::Resources::RenderSprite& Sprite::GetRenderSprite()
{
	return m_renderSprite;
}

ElkRendering::Resources::Texture* ElkGameEngine::Objects::Components::Behaviours::Sprite::GetTexture()
{
	return m_renderSprite.GetTexture();
}

void ElkGameEngine::Objects::Components::Behaviours::Sprite::SetTexture(ElkRendering::Resources::Texture& p_texture)
{
	m_renderSprite.SetTexture(p_texture);
}

void ElkGameEngine::Objects::Components::Behaviours::Sprite::SetRenderLayer(uint16_t p_renderLayer)
{
	m_renderLayer = p_renderLayer;
}

uint16_t ElkGameEngine::Objects::Components::Behaviours::Sprite::GetRenderLayer()
{
	return m_renderLayer;
}

void ElkGameEngine::Objects::Components::Behaviours::Sprite::SetColor(const glm::vec4 & p_color)
{
	m_color = p_color;
}

const glm::vec4 & ElkGameEngine::Objects::Components::Behaviours::Sprite::GetColor()
{
	return m_color;
}
