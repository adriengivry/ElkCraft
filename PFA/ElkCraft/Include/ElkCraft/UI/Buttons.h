#pragma once

#include <iostream>

#include <ElkGameEngine/ElkGameEngine.h>

#include "ElkCraft/System/GameStateManager.h"

namespace ElkCraft::UI
{
	class MenuButton : public ElkGameEngine::Objects::Components::Behaviours::AButton
	{
	public:
		virtual void OnClick() = 0;
		void OnMouseOver() override;
		void OnMouseNotOver() override;
		void Update() override;

	private:
		std::shared_ptr<ElkGameEngine::Objects::Components::Behaviours::Sprite> m_sprite;
		std::shared_ptr<ElkGameEngine::Objects::Components::Behaviours::Text>	m_text;
	};

	class PlayButton : public MenuButton
	{
	public:
		void OnClick() override;
	};

	class ContinueButton : public MenuButton
	{
	public:
		void OnClick() override;
	};

	class QuitButton : public MenuButton
	{
	public:
		void OnClick() override;
	};
}