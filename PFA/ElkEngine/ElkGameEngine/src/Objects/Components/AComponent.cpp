#include "stdafx.h"

#include "ElkGameEngine/Objects/Components/AComponent.h"
#include "ElkGameEngine/Objects/GameObject.h"

using namespace ElkGameEngine::Objects::Components;

AComponent::AComponent()
{
	SetName("AComponent");
}

void AComponent::SetOwner(ElkGameEngine::Objects::GameObject& p_owner)
{
	m_owner = &p_owner;
}

ElkGameEngine::Objects::GameObject& AComponent::GetOwner() const
{
	return *m_owner;
}
