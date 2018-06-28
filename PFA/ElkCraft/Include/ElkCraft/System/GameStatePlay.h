#pragma once

#include <ElkGameEngine/ElkGameEngine.h>

#include "ElkCraft/System/IGameState.h"
#include "ElkCraft/Gameplay/PlayerController.h"
#include "ElkCraft/Gameplay/BlockInteraction.h"
#include "ElkCraft/Gameplay/ItemInteraction.h"
#include "ElkCraft/Gameplay/Crafting.h"
#include "ElkCraft/Gameplay/HealthSystem.h"
#include "ElkCraft/Gameplay/HungerSystem.h"
#include "ElkCraft/UI/Buttons.h"
#include "ElkCraft/Terrain/World.h"
#include "ElkCraft/Physics/WorldCollision.h"
#include "ElkCraft/UI/HandAnimation.h"
#include "ElkCraft/System/SaveManager.h"
#include "ElkCraft/Utils/ItemsInfo.h"

namespace ElkCraft::System
{
	class GameStatePlay : public ElkCraft::System::IGameState
	{
	public:
		GameStatePlay(ElkGameEngine::Managers::EngineManager& p_engineManager, const bool& p_newSave = true);
		~GameStatePlay();

		void CreateGameObjects();
		void InitComponents();

		void Update() override;
		void UpdateWorldCollision();
		void UpdateDaytime();
		void HandleInputs();
		void UpdateScreenFilter();
		void UpdateFPSCounter();
		void UpdateDaytimeCounter();
		void UpdateCurrentRecipeDisplay();
		void UpdateInventorySelector();
		void UpdateInventoryBarSlots();
		void UpdatePlayer();
		void UpdateWorld();
		void UpdateHearts();
		void UpdateHunger();
		void UpdateKeypadCheatResult();
		void UpdateHand();

		void Save();

		void SetCraftMenuOpen(bool p_value);
		void RespawnPlayer();

		glm::vec3 GetPlayerStartPosition();

	private:
		/* Managers */
		ElkGameEngine::Managers::EngineManager& m_engineManager;
		ElkGameEngine::Managers::SceneManager& m_sceneManager;
		ElkRendering::Managers::TextureManager& m_textureManager;
		ElkPhysics::Managers::PhysicsManager& m_physicsManager;
		ElkRendering::Managers::RenderingManager& m_renderingManager;
		ElkGameEngine::Managers::InputManager& m_inputManager;
		ElkTools::Managers::AudioManager& m_audioManager;
		ElkCraft::System::SaveManager m_saveManager;

		/* GameObjects */
		ElkGameEngine::Objects::GameObject*		m_player;
		ElkGameEngine::Objects::GameObject*		m_UICamera;
		ElkGameEngine::Objects::GameObject*		m_crosshair;
		ElkGameEngine::Objects::GameObject*		m_inventorySelector;
		ElkGameEngine::Objects::GameObject*		m_inventoryBar;
		ElkGameEngine::Objects::GameObject*		m_inventoryKeypadCheatResult;
		ElkGameEngine::Objects::GameObject*		m_screenFilter;
		ElkGameEngine::Objects::GameObject*		m_fpsCounter;
		ElkGameEngine::Objects::GameObject*		m_daytimeCounter;
		ElkGameEngine::Objects::GameObject*		m_craftingMenu;
		ElkGameEngine::Objects::GameObject*		m_currentRecipeResult;
		ElkGameEngine::Objects::GameObject*		m_currentRecipeRequirement[3];
		ElkGameEngine::Objects::GameObject*		m_currentRecipeInformation;
		ElkGameEngine::Objects::GameObject*		m_inventoryBarSlots[10];
		ElkGameEngine::Objects::GameObject*		m_healthHeart[10];
		ElkGameEngine::Objects::GameObject*		m_hungerIcon[10];
		ElkGameEngine::Objects::GameObject*		m_hand;
		ElkGameEngine::Objects::GameObject*		m_handItem;

		/* World */
		std::shared_ptr<ElkCraft::Terrain::World>			m_world;
		std::shared_ptr<ElkCraft::Physics::WorldCollision>	m_worldCollision;

		/* FPS counter stuffs */
		float m_renderingMaxFramerate;
		float m_fpsCounterUpdateFrequency;
		float m_fpsCounterTimer;

		/* Craft menu state */
		bool m_craftMenuIsOpen;

		/* Game parameters */
		uint32_t m_days;
		float m_daytimeTimer;
	};
}