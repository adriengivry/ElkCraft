#include "stdafx.h"

#include <ElkTools/Debug/Log.h>
#include <ElkTools/Utils/Profiler.h>
#include <ElkTools/Utils/IniLoader.h>
#include <ElkTools/Utils/Time.h>

#include "ElkRendering/Managers/RenderingManager.h"
#include "ElkRendering/Debug/GLDebug.h"
#include "ElkTools/Utils/ManagerLocator.h"

ElkRendering::Managers::RenderingManager::RenderingManager(const uint16_t& p_windowWidth,
                                                           const uint16_t& p_windowHeight)
	: m_renderMode(RenderMode::DEFAULT), m_width(p_windowWidth), m_height(p_windowHeight)
{
	ElkRendering::Managers::RenderingManager::Setup();
}

ElkRendering::Managers::RenderingManager::~RenderingManager() { ElkRendering::Managers::RenderingManager::Close(); }

void ElkRendering::Managers::RenderingManager::Setup()
{
	m_textureManager = std::make_unique<TextureManager>();
	m_shaderManager = std::make_unique<ShaderManager>();

	ElkTools::Utils::ManagerLocator::Provide<ElkRendering::Managers::TextureManager>(*m_textureManager);
	ElkTools::Utils::ManagerLocator::Provide<ElkRendering::Managers::ShaderManager>(*m_shaderManager);

	UpdateRenderMode();

	m_outlineIsEnabled = false;

	m_near = ElkTools::Utils::IniLoader::GetFloat("rendering.ini", "near");
	m_far = ElkTools::Utils::IniLoader::GetFloat("rendering.ini", "far");
}

void ElkRendering::Managers::RenderingManager::Close()
{
}

void ElkRendering::Managers::RenderingManager::PrepareChunkDraw()
{
	m_currentShader = m_shaderManager->RequireAndGet("voxel");
	if (m_currentShader)
	{
		m_currentTexture = m_textureManager->RequireAndGet("Blocks");
		if (m_currentTexture)
		{
			m_currentShader->Bind();
			m_currentTexture->Bind();

			m_currentShader->SetUniformFloat("u_time_since_start", ElkTools::Utils::Time::GetTimeSinceStart());
			m_currentShader->SetUniformFloat("u_daytime_light_coefficient", m_daytimeLightCoefficient);
			m_currentShader->SetUniformVec3("u_destruction_position", m_currentDestructionPosition);
			m_currentShader->SetUniformInt("u_destruction_level", m_currentDestructionLevel);
		}
	}
}

void ElkRendering::Managers::RenderingManager::DrawChunk(const ElkRendering::Resources::RenderChunk& p_chunk,
                                                         const glm::vec3& p_position)
{
	if (m_currentTexture && m_currentShader)
	{
		m_currentShader->SetUniformVec3("u_transform_position", p_position);
		p_chunk.Draw();
	}
}

void ElkRendering::Managers::RenderingManager::EndChunkDraw()
{
	m_currentShader->Unbind();
	m_currentTexture->Unbind();
	m_currentShader = nullptr;
	m_currentTexture = nullptr;

	/* Draw outline after drawing chunks */
	if (m_outlineIsEnabled)
	{
		auto shader = m_shaderManager->RequireAndGet("outline");
		if (shader)
		{
			shader->Bind();
			m_outline.Draw();
			shader->Unbind();
		}
	}
}

void ElkRendering::Managers::RenderingManager::PrepareSpriteDraw()
{
	glClear(GL_DEPTH_BUFFER_BIT);

	m_currentShader = m_shaderManager->RequireAndGet("sprite");
	if (m_currentShader)
	{
		m_currentShader->Bind();
	}
}

void ElkRendering::Managers::RenderingManager::DrawSprite(const ElkRendering::Resources::RenderSprite& p_sprite,
                                                          const glm::vec3& p_position,
                                                          const glm::vec3& p_scale,
														  const glm::vec4& p_color)
{
	if (m_currentShader)
	{
		m_currentShader->SetUniformVec3("u_transform_position", p_position);
		m_currentShader->SetUniformVec3("u_transform_scale", p_scale);
		m_currentShader->SetUniformVec4("u_color", p_color);
		p_sprite.Draw();
	}
}

void ElkRendering::Managers::RenderingManager::EndSpriteDraw()
{
	m_currentShader->Unbind();
}

void ElkRendering::Managers::RenderingManager::PrepareStringDraw()
{
	glClear(GL_DEPTH_BUFFER_BIT);

	m_currentShader = m_shaderManager->RequireAndGet("text");
	if (m_currentShader)
	{
		m_currentTexture = m_textureManager->RequireAndGet("Ascii");
		if (m_currentTexture)
		{
			m_currentShader->Bind();
			m_currentTexture->Bind();
		}
	}
}

void ElkRendering::Managers::RenderingManager::DrawString(const ElkRendering::Resources::RenderText & p_text, const glm::vec3& p_position, const glm::vec3& p_scale, float p_spacing, const glm::vec4& p_color)
{
	if (m_currentTexture && m_currentShader)
	{
		m_currentShader->SetUniformVec3("u_transform_position", p_position);
		m_currentShader->SetUniformVec3("u_transform_scale", p_scale);
		m_currentShader->SetUniformFloat("u_spacing", p_spacing);
		m_currentShader->SetUniformVec4("u_font_color", p_color);
		p_text.Draw();
	}
}

void ElkRendering::Managers::RenderingManager::EndStringDraw()
{
	m_currentShader->Unbind();
	m_currentTexture->Unbind();
	m_currentTexture = nullptr;
}

void ElkRendering::Managers::RenderingManager::SetOutlinePosition(glm::vec3 p_position)
{
	auto shader = m_shaderManager->RequireAndGet("outline");

	if (shader)
	{
		shader->Bind();
		shader->SetUniformVec3("u_position", p_position);
		shader->Unbind();
	}
}

void ElkRendering::Managers::RenderingManager::SetOutlineState(bool p_state)
{
	m_outlineIsEnabled = p_state;
}

void ElkRendering::Managers::RenderingManager::SetDestructionInfo(glm::vec3 p_position, uint8_t p_value)
{
	m_currentDestructionPosition = p_position;
	m_currentDestructionLevel = p_value;
}

void ElkRendering::Managers::RenderingManager::UpdateRenderMode()
{
	GLenum renderMode;

	auto voxelShader = m_shaderManager->RequireAndGet("voxel");
	if (voxelShader)
		voxelShader->Bind();

	switch (m_renderMode)
	{
	default:
	case RenderMode::DEFAULT:
		renderMode = GL_FILL;
		if (voxelShader)
			voxelShader->SetUniformInt("u_render_mode", 0);
		break;
	case RenderMode::WIREFRAME:
		renderMode = GL_LINE;
		if (voxelShader)
			voxelShader->SetUniformInt("u_render_mode", 1);
		break;
	case RenderMode::NORMALS:
		renderMode = GL_FILL;
		if (voxelShader)
			voxelShader->SetUniformInt("u_render_mode", 2);
		break;
	}

	if (voxelShader)
		voxelShader->Unbind();

	GLCall(glPolygonMode(GL_FRONT_AND_BACK, renderMode));
}

glm::mat4 ElkRendering::Managers::RenderingManager::CalculateView() const
{
	return m_camera.GetViewMatrix();
}

glm::mat4 ElkRendering::Managers::RenderingManager::CalculateUIView() const
{
	return m_UICamera.GetViewMatrix();
}

glm::mat4 ElkRendering::Managers::RenderingManager::CalculateProjectionperspective() const
{
	const float screenRatio = static_cast<float>(m_width) / static_cast<float>(m_height);
	return glm::perspective(m_camera.GetZoom(), screenRatio, m_near, m_far);
}

glm::mat4 ElkRendering::Managers::RenderingManager::CalculateProjectionOrtho() const
{
	return glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, m_near, m_far);
}

void ElkRendering::Managers::RenderingManager::SetDaytimeLightCoefficient(float p_coefficient)
{
	m_daytimeLightCoefficient = p_coefficient;
}

void ElkRendering::Managers::RenderingManager::SetCamera(const glm::vec3& p_position, const glm::vec3& p_forward,
                                                         const glm::vec3& p_right, const glm::vec3& p_up)
{
	m_camera.SetPosition(p_position);
	m_camera.SetForward(p_forward);
	m_camera.SetRight(p_right);
	m_camera.SetUp(p_up);

	const glm::mat4 view = CalculateView();
	const glm::mat4 projection = CalculateProjectionperspective();

	auto voxelShader = m_shaderManager->RequireAndGet("voxel");
	if (voxelShader)
	{
		voxelShader->Bind();
		voxelShader->SetUniformMat4("u_view", view);
		voxelShader->SetUniformMat4("u_projection", projection);
		voxelShader->SetUniformVec3("u_camera_position", p_position);
		voxelShader->Unbind();
	}

	if (m_outlineIsEnabled)
	{
		auto outlineShader = m_shaderManager->RequireAndGet("outline");
		if (outlineShader)
		{
			outlineShader->Bind();
			outlineShader->SetUniformMat4("u_view", view);
			outlineShader->SetUniformMat4("u_projection", projection);
			outlineShader->Unbind();
		}
	}
}

void ElkRendering::Managers::RenderingManager::SetUICamera(const glm::vec3& p_position, const glm::vec3& p_forward,
                                                           const glm::vec3& p_right, const glm::vec3& p_up)
{
	m_UICamera.SetPosition(p_position);
	m_UICamera.SetForward(p_forward);
	m_UICamera.SetRight(p_right);
	m_UICamera.SetUp(p_up);

	const float screenRatio = static_cast<float>(m_width) / static_cast<float>(m_height);

	const glm::mat4 view = CalculateUIView();
	const glm::mat4 projection = glm::perspective(m_UICamera.GetZoom(), screenRatio, m_near, m_far);

	auto spriteShader = m_shaderManager->RequireAndGet("sprite");
	if (spriteShader)
	{
		spriteShader->Bind();
		spriteShader->SetUniformMat4("u_view_projection", projection * view);
		spriteShader->Unbind();
	}

	auto textShader = m_shaderManager->RequireAndGet("text");
	if (textShader)
	{
		textShader->Bind();
		textShader->SetUniformMat4("u_view_projection", projection * view);
		textShader->Unbind();
	}
}

void ElkRendering::Managers::RenderingManager::SetWindowSize(const uint16_t& p_width, const uint16_t& p_height)
{
	m_width = p_width;
	m_height = p_height;
}

void ElkRendering::Managers::RenderingManager::SetRenderMode(const RenderMode& p_renderMode)
{
	m_renderMode = p_renderMode;
	UpdateRenderMode();
}
