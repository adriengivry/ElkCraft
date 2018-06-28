#include "stdafx.h"

#include "ElkCraft/System/Game.h"

using namespace ElkTools::Utils;
using namespace ElkTools::Managers;
using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkRendering::Managers;
using namespace ElkPhysics::Managers;
using namespace ElkCraft::Terrain;
using namespace ElkCraft::Gameplay;
using namespace ElkCraft::UI;
using namespace ElkCraft::System;

ElkCraft::System::Game::Game() :
	m_sceneManager(ManagerLocator::Get<SceneManager>()),
	m_textureManager(ManagerLocator::Get<TextureManager>()),
	m_physicsManager(ManagerLocator::Get<PhysicsManager>()),
	m_inputManager(ManagerLocator::Get<InputManager>()),
	m_audioManager(ManagerLocator::Get<AudioManager>())
{
	ElkCraft::System::GameStateManager::SetCurrentState(ElkCraft::System::GameStateManager::GameState::MENU_STATE);
}

void ElkCraft::System::Game::UpdateGameState()
{
	if (ElkCraft::System::GameStateManager::IsGameStateChanged())
	{
		if (m_currentGameState)
		{
			delete m_currentGameState;
			m_currentGameState = nullptr;
		}

		switch (ElkCraft::System::GameStateManager::GetCurrentState())
		{
		case ElkCraft::System::GameStateManager::GameState::MENU_STATE:
			m_currentGameState = new ElkCraft::System::GameStateMenu(m_engineManager);
			break;

		case ElkCraft::System::GameStateManager::GameState::PLAY_STATE:
			m_currentGameState = new ElkCraft::System::GameStatePlay(m_engineManager, true);
			break;

		case ElkCraft::System::GameStateManager::GameState::CONTINUE_STATE:
			m_currentGameState = new ElkCraft::System::GameStatePlay(m_engineManager, false);
			break;

		case ElkCraft::System::GameStateManager::GameState::EXIT_STATE:
			m_engineManager.Quit();
			break;
		}

		ElkCraft::System::GameStateManager::ResetGameStateChange();
	}
}

void ElkCraft::System::Game::Run()
{
	while (m_engineManager.IsRunning())
	{
		if (m_currentGameState)
		{
			m_engineManager.PreUpdate();
			m_currentGameState->Update();
			m_engineManager.PostUpdate();
		}

		UpdateGameState();
	}
}