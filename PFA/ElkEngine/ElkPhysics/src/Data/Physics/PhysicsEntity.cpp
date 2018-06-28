#include "stdafx.h"

#include "ElkPhysics/Data/Physics/PhysicsEntity.h"

#include <iostream>

ElkPhysics::Data::Physics::PhysicsEntity::PhysicsEntity(Physics::TransformData & p_transform, Physics::RigidbodyData & p_rigidbody)
	: m_transform(&p_transform), m_rigidbody(&p_rigidbody)
{
}

ElkPhysics::Data::Physics::RigidbodyData& ElkPhysics::Data::Physics::PhysicsEntity::GetRigidbody()
{
	return *m_rigidbody;
}

ElkPhysics::Data::Physics::TransformData& ElkPhysics::Data::Physics::PhysicsEntity::GetTransform()
{
	return *m_transform;
}