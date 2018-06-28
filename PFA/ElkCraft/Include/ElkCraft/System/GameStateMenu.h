#pragma once

#include <ElkGameEngine/ElkGameEngine.h>

#include "ElkCraft/System/IGameState.h"
#include "ElkCraft/Gameplay/PlayerController.h"
#include "ElkCraft/Gameplay/BlockInteraction.h"
#include "ElkCraft/UI/Buttons.h"
#include "ElkCraft/Terrain/World.h"

namespace ElkCraft::System
{
	class GameStateMenu : public ElkCraft::System::IGameState
	{
	public:
		GameStateMenu(ElkGameEngine::Managers::EngineManager& p_engineManager);
		~GameStateMenu();

		void CreateGameObjects();
		void InitGameObjects();

		void UpdateButtonsAnimationSettings();
		void UpdateButton(ElkGameEngine::Objects::GameObject& p_button);
		void UpdateButtonAnimation(ElkGameEngine::Objects::GameObject& p_button);

		void Update() override;
		void HandleInputs();

	private:
		/* Managers */
		ElkGameEngine::Managers::EngineManager& m_engineManager;
		ElkGameEngine::Managers::SceneManager& m_sceneManager;
		ElkGameEngine::Managers::WindowManager& m_windowManager;
		ElkRendering::Managers::TextureManager& m_textureManager;
		ElkPhysics::Managers::PhysicsManager& m_physicsManager;
		ElkGameEngine::Managers::InputManager& m_inputManager;
		ElkTools::Managers::AudioManager& m_audioManager;

		/* Game Objects */
		ElkGameEngine::Objects::GameObject* m_UICamera;
		ElkGameEngine::Objects::GameObject* m_background;
		ElkGameEngine::Objects::GameObject* m_logo;
		ElkGameEngine::Objects::GameObject* m_playButton;
		ElkGameEngine::Objects::GameObject* m_continueButton;
		ElkGameEngine::Objects::GameObject* m_quitButton;

		float m_currentButtonScale;
		float m_targetButtonScale;

		/* Others */
		float m_windowWidth;
		float m_windowHeight;
	};
}