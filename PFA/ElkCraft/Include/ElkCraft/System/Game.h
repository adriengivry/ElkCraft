#pragma once

#include <ElkGameEngine/ElkGameEngine.h>

#include "ElkCraft/System/GameStateManager.h"
#include "ElkCraft/System/IGameState.h"
#include "ElkCraft/System/GameStatePlay.h"
#include "ElkCraft/System/GameStateMenu.h"
#include "ElkCraft/Gameplay/PlayerController.h"
#include "ElkCraft/Gameplay/BlockInteraction.h"
#include "ElkCraft/UI/Buttons.h"
#include "ElkCraft/Terrain/World.h"

namespace ElkCraft::System
{
	class Game
	{
	public:
		Game();
		~Game() = default;

		void UpdateGameState();
		void Run();

	private:
		ElkGameEngine::Managers::EngineManager m_engineManager;

		ElkGameEngine::Managers::SceneManager& m_sceneManager;
		ElkRendering::Managers::TextureManager& m_textureManager;
		ElkPhysics::Managers::PhysicsManager& m_physicsManager;
		ElkGameEngine::Managers::InputManager& m_inputManager;
		ElkTools::Managers::AudioManager& m_audioManager;

		IGameState* m_currentGameState;
	};
}