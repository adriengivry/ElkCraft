#include "stdafx.h"

#include "ElkRendering/Data/CameraData.h"

using namespace ElkRendering::Data;

CameraData::CameraData(const glm::vec3 p_position,
                       const glm::vec3 p_up)
	: m_forward(glm::vec3(0.0f, 0.0f, -1.0f))
{
	Setup();
	m_position = p_position;
	m_worldUp = p_up;
}

CameraData::CameraData(const float p_posX, const float p_posY, const float p_posZ,
	const float p_upX, const float p_upY, const float p_upZ)
	: m_forward(glm::vec3(0.0f, 0.0f, -1.0f))
{
	Setup();
	m_position = glm::vec3(p_posX, p_posY, p_posZ);
	m_worldUp = glm::vec3(p_upX, p_upY, p_upZ);
}

void CameraData::Setup()
{
	ResetZoom();
}

glm::mat4 CameraData::GetViewMatrix() const
{
	return glm::lookAt(m_position, m_position + m_forward, m_up);
}

void ElkRendering::Data::CameraData::ResetZoom()
{
	m_zoom = ElkTools::Utils::IniLoader::GetFloat("rendering.ini", "default_fov");
}
