#include "stdafx.h"

#include "ElkCraft/UI/Buttons.h"

using namespace ElkCraft::UI;
using namespace ElkGameEngine::Objects::Components::Behaviours;

void MenuButton::OnMouseOver()
{
	if (m_sprite && m_text)
	{
		glm::vec4 colorToApply = glm::vec4(1.f, 1.f, 1.f, 1.f);

		m_sprite->SetColor(colorToApply);
		m_text->SetFontColor(colorToApply);
	}
}

void ElkCraft::UI::MenuButton::OnMouseNotOver()
{
	if (m_sprite && m_text)
	{
		glm::vec4 colorToApply = glm::vec4(1.f, 1.f, 1.f, 0.75f);

		m_sprite->SetColor(colorToApply);
		m_text->SetFontColor(colorToApply);
	}
}

void ElkCraft::UI::MenuButton::Update()
{
	if (!m_sprite)
		m_sprite = m_owner->GetComponent<Sprite>();

	if (!m_text)
		m_text = m_owner->GetComponent<Text>();

	AButton::Update();
}

void PlayButton::OnClick()
{
	ElkTools::Utils::ManagerLocator::Get<ElkTools::Managers::AudioManager>().Play2DSound("../Resources/Audio/Sounds/ui/click.ogg");
	ElkCraft::System::GameStateManager::SetCurrentState(ElkCraft::System::GameStateManager::GameState::PLAY_STATE);
}

void ContinueButton::OnClick()
{
	ElkTools::Utils::ManagerLocator::Get<ElkTools::Managers::AudioManager>().Play2DSound("../Resources/Audio/Sounds/ui/click.ogg");
	ElkCraft::System::GameStateManager::SetCurrentState(ElkCraft::System::GameStateManager::GameState::CONTINUE_STATE);
}

void QuitButton::OnClick()
{
	ElkCraft::System::GameStateManager::SetCurrentState(ElkCraft::System::GameStateManager::GameState::EXIT_STATE);
}
