#include "stdafx.h"

#include "ElkGameEngine/Objects/Components/Transform.h"
#include "ElkGameEngine/Objects/GameObject.h"

using namespace ElkGameEngine::Objects::Components;

Transform::Transform()
{
	SetName("Transform");

	m_transformData.position = glm::vec3(0.f, 0.f, 0.f);
	m_transformData.rotation = glm::vec3(0.f, 0.f, 0.f);
	m_transformData.scale = glm::vec3(1.f, 1.f, 1.f);
}

Transform::Transform(const glm::vec3& p_position) : Transform()
{
	m_transformData.position = p_position;
}

Transform::Transform(const glm::vec3& p_position, const glm::vec3& p_rotation) : Transform()
{
	m_transformData.position = p_position;
	m_transformData.rotation = p_rotation;
}

Transform::Transform(const glm::vec3& p_position, const glm::vec3& p_rotation, const glm::vec3& p_scale) : Transform()
{
	m_transformData.position = p_position;
	m_transformData.rotation = p_rotation;
	m_transformData.scale = p_scale;
}

glm::vec3& Transform::GetPosition()
{
	return m_transformData.position;
}

glm::vec3& Transform::GetRotation()
{
	return m_transformData.rotation;
}

glm::vec3& Transform::GetScale()
{
	return m_transformData.scale;
}

glm::vec3 Transform::GetForward() const
{
	return m_forward;
}

glm::vec3 Transform::GetRight() const
{
	return m_right;
}

glm::vec3 Transform::GetUp() const
{
	return m_up;
}

void Transform::UpdateForwardRightUp()
{
	if (m_transformData.rotation.x > 89)
		m_transformData.rotation.x = 89;
	else if (m_transformData.rotation.x < -89)
		m_transformData.rotation.x = -89;

	glm::vec3 front;
	front.x = cos(glm::radians(m_transformData.rotation.y)) * cos(glm::radians(m_transformData.rotation.x));
	front.y = sin(glm::radians(m_transformData.rotation.x));
	front.z = sin(glm::radians(m_transformData.rotation.y)) * cos(glm::radians(m_transformData.rotation.x));
	m_forward = glm::normalize(front);

	m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.f, 1.f, 0.f)));

	m_up = glm::normalize(glm::cross(m_right, m_forward));
}

void Transform::SetPosition(const glm::vec3& p_position)
{
	m_transformData.position = p_position;
}

void Transform::SetRotation(const glm::vec3& p_rotation)
{
	m_transformData.rotation = p_rotation;

	UpdateForwardRightUp();
}

void Transform::SetForward(const glm::vec3& p_forward)
{
	//m_forward = p_forward;
}

void Transform::SetScale(const glm::vec3& p_scale)
{
	m_transformData.scale = p_scale;
}

void Transform::Translate(const glm::vec3& p_translate)
{
	m_transformData.position += p_translate;
}

void Transform::Rotate(const glm::vec3& p_rotate)
{
	m_transformData.rotation += p_rotate;

	UpdateForwardRightUp();
}

void Transform::Scale(const glm::vec3& p_scale)
{
	m_transformData.scale *= p_scale;
}
