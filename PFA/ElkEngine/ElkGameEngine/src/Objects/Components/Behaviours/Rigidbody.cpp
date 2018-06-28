#include "stdafx.h"

#include "ElkGameEngine/Objects/Components/Behaviours/Rigidbody.h"

using namespace ElkGameEngine::Objects::Components::Behaviours;

ElkGameEngine::Objects::Components::Behaviours::Rigidbody::Rigidbody() 
	: ABehaviour()
{
	SetName("Rigidbody");
}

const glm::vec3 & Rigidbody::GetVelocity()
{
	return m_rigidbodyData.velocity;
}

void Rigidbody::SetVelocity(const glm::vec3 & p_value)
{
	m_rigidbodyData.velocity = p_value;
}

void Rigidbody::SetGravityScale(float p_gravityScale)
{
	m_rigidbodyData.gravityScale = p_gravityScale;
}

float Rigidbody::GetGravityScale()
{
	return m_rigidbodyData.gravityScale;
}

void ElkGameEngine::Objects::Components::Behaviours::Rigidbody::SetKinematic(bool p_state)
{
	m_rigidbodyData.kinematic = p_state;
}

bool ElkGameEngine::Objects::Components::Behaviours::Rigidbody::IsKinematic()
{
	return m_rigidbodyData.kinematic;
}
