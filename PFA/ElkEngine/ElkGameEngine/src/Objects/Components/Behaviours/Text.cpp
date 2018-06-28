#include "stdafx.h"

#include "ElkGameEngine/Objects/Components/Behaviours/Text.h"

ElkGameEngine::Objects::Components::Behaviours::Text::Text()
{
	SetName("Text");

	m_spacing = 0.f;
	m_fontSize = 1.f;
	m_positionOffset = glm::vec3(0.f);
	m_fontColor = glm::vec4(1.f);
	m_scalingMode = ScalingMode::DEFAULT;
}

void ElkGameEngine::Objects::Components::Behaviours::Text::SetString(const std::string & p_string)
{
	m_renderText.SetString(p_string);
}

std::string ElkGameEngine::Objects::Components::Behaviours::Text::GetString()
{
	return m_renderText.GetString();
}

void ElkGameEngine::Objects::Components::Behaviours::Text::SetSpacing(float p_spacing)
{
	m_spacing = p_spacing;
}

float ElkGameEngine::Objects::Components::Behaviours::Text::GetSpacing()
{
	return m_spacing;
}

void ElkGameEngine::Objects::Components::Behaviours::Text::SetFontSize(float p_size)
{
	m_fontSize = p_size;
}

float ElkGameEngine::Objects::Components::Behaviours::Text::GetFontSize()
{
	return m_fontSize;
}

void ElkGameEngine::Objects::Components::Behaviours::Text::SetPositionOffset(const glm::vec3 & p_offset)
{
	m_positionOffset = p_offset;
}

const glm::vec3 & ElkGameEngine::Objects::Components::Behaviours::Text::GetPositionOffset()
{
	return m_positionOffset;
}

void ElkGameEngine::Objects::Components::Behaviours::Text::SetFontColor(const glm::vec4& p_color)
{
	m_fontColor = p_color;
}

const glm::vec4& ElkGameEngine::Objects::Components::Behaviours::Text::GetFontColor()
{
	return m_fontColor;
}

void ElkGameEngine::Objects::Components::Behaviours::Text::SetScalingMode(ScalingMode p_newScalingMode)
{
	m_scalingMode = p_newScalingMode;
}

ElkGameEngine::Objects::Components::Behaviours::Text::ScalingMode ElkGameEngine::Objects::Components::Behaviours::Text::GetScalingMode()
{
	return m_scalingMode;
}

ElkRendering::Resources::RenderText & ElkGameEngine::Objects::Components::Behaviours::Text::GetRenderText()
{
	return m_renderText;
}
