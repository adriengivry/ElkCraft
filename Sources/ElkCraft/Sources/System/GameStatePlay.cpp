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
using namespace ElkCraft::Physics;
using namespace ElkCraft::Utils;

ElkCraft::System::GameStatePlay::GameStatePlay(EngineManager& p_engineManager, const bool& p_newSave) :
	m_engineManager(p_engineManager),
	m_sceneManager(ManagerLocator::Get<SceneManager>()),
	m_textureManager(ManagerLocator::Get<TextureManager>()),
	m_physicsManager(ManagerLocator::Get<PhysicsManager>()),
	m_renderingManager(ManagerLocator::Get<RenderingManager>()),
	m_inputManager(ManagerLocator::Get<InputManager>()),
	m_audioManager(ManagerLocator::Get<AudioManager>()),
	m_saveManager(p_newSave),
	m_renderingMaxFramerate(IniLoader::GetFloat("rendering.ini", "max_framerate")),
	m_fpsCounterUpdateFrequency(IniLoader::GetFloat("ui.ini", "framerate_refresh_rate")),
	m_fpsCounterTimer(0.f),
	m_daytimeTimer(400.f),
	m_days(1)
{
	m_sceneManager.CreateScene("Play");

	m_world = std::make_shared<World>(m_engineManager, m_saveManager);
	m_world->Populate();

	m_worldCollision = std::make_shared<WorldCollision>(*m_world);

	if (!p_newSave)
	{
		m_days = m_saveManager.GetPlayerData().days;
		m_daytimeTimer = static_cast<float>(m_saveManager.GetPlayerData().daytime);
	}

	CreateGameObjects();
	InitComponents();

	SetCraftMenuOpen(false);
	
	m_audioManager.StopAllSounds();
	m_audioManager.Play2DSound("../Resources/Audio/Musics/calm.mp3", true, 0.5f);

	m_engineManager.ParseScene();
}

ElkCraft::System::GameStatePlay::~GameStatePlay()
{
	Save();
	m_sceneManager.RemoveScene("Play");
}

void ElkCraft::System::GameStatePlay::UpdateWorldCollision()
{
	m_worldCollision->Update(m_player->transform->GetPosition());
}

void ElkCraft::System::GameStatePlay::UpdateDaytime()
{
	m_daytimeTimer += Time::GetDeltaTime();

	while (m_daytimeTimer >= 1200.f)
	{
		m_daytimeTimer -= 1200.f;
		++m_days;
	}

	float x = m_daytimeTimer;
	float lightCoefficient = static_cast<float>(0.2 + 0 * (x - 0) + 0.00002 * (x - 0) * (x - 300) + (-3.7037037037037036e-8) * (x - 0) * (x - 300) * (x - 400) + 3.7037037037037036e-11 * (x - 0) * (x - 300) * (x - 400) * (x - 900) + (-1.1223344556677888e-14) * (x - 0) * (x - 300) * (x - 400) * (x - 900) * (x - 1000));
	if (lightCoefficient < 0.2f)
		lightCoefficient = 0.2f;

	if (lightCoefficient > 1.0f)
		lightCoefficient = 1.0f;

	Sprite& screenFilterSprite = *m_screenFilter->GetComponent<Sprite>();
	screenFilterSprite.SetColor(glm::vec4(glm::vec3(1.f, 1.f, 1.f) * lightCoefficient, 1.f));

	Sprite& handSprite = *m_hand->GetComponent<Sprite>();
	handSprite.SetColor(glm::vec4(glm::vec3(1.f, 1.f, 1.f) * lightCoefficient, 1.f));

	m_engineManager.SetClearScreenCoefficient(lightCoefficient);
	m_renderingManager.SetDaytimeLightCoefficient(lightCoefficient);
}

void ElkCraft::System::GameStatePlay::Update()
{
	PROFILER_SPY("GameStatePlay::Update");

	UpdateWorldCollision();
	HandleInputs();
	UpdateDaytime();
	UpdateWorld();
	UpdatePlayer();
	UpdateScreenFilter();
	UpdateFPSCounter();
	UpdateDaytimeCounter();
	UpdateCurrentRecipeDisplay();
	UpdateInventorySelector();
	UpdateInventoryBarSlots();
	UpdateHearts();
	UpdateHunger();
	UpdateKeypadCheatResult();
	UpdateHand();
}

void ElkCraft::System::GameStatePlay::CreateGameObjects()
{
	/* Create player */
	auto playerData = m_saveManager.GetPlayerData();

	m_player = &m_sceneManager.GetCurrentScene().CreateGameObject("Player");
	m_player->AddComponent<Camera>();
	m_player->AddComponent<Rigidbody>();
	m_player->AddComponent<Inventory>();
	m_player->AddComponent<AudioListener>();
	m_player->AddComponent<PlayerController>(m_world.get());
	m_player->AddComponent<BlockInteraction>(m_world.get());
	m_player->AddComponent<ItemInteraction>();
	m_player->AddComponent<Crafting>();
	m_player->AddComponent<HealthSystem>(m_world.get());
	m_player->AddComponent<HungerSystem>();

	/* Create UI Camera */
	m_UICamera = &m_sceneManager.GetCurrentScene().CreateGameObject("UICamera");
	m_UICamera->AddComponent<UICamera>();

	/* Create crosshair */
	m_crosshair = &m_sceneManager.GetCurrentScene().CreateGameObject("Crosshair");
	m_crosshair->AddComponent<Sprite>();

	/* Create inventory selector */
	m_inventorySelector = &m_sceneManager.GetCurrentScene().CreateGameObject("InventorySelector");
	m_inventorySelector->AddComponent<Sprite>();

	/* Create inventory bar */
	m_inventoryBar = &m_sceneManager.GetCurrentScene().CreateGameObject("Inventory");
	m_inventoryBar->AddComponent<Sprite>();

	/* Create inventory slots */
	for (uint8_t i = 0; i < 10; ++i)
	{
		m_inventoryBarSlots[i] = &m_sceneManager.GetCurrentScene().CreateGameObject("InventorySlot" + std::to_string(i));
		m_inventoryBarSlots[i]->AddComponent<Sprite>();
		m_inventoryBarSlots[i]->AddComponent<Text>();
	}

	/* Create keypad cheat result */
	m_inventoryKeypadCheatResult = &m_sceneManager.GetCurrentScene().CreateGameObject("KeypadCheatResult");
	m_inventoryKeypadCheatResult->AddComponent<Sprite>();
	m_inventoryKeypadCheatResult->AddComponent<Text>();

	/* Create screen filter */
	m_screenFilter = &m_sceneManager.GetCurrentScene().CreateGameObject("ScreenFilter");
	m_screenFilter->AddComponent<Sprite>();

	/* Create FPS counter */
	m_fpsCounter = &m_sceneManager.GetCurrentScene().CreateGameObject("FPSCounter");
	m_fpsCounter->AddComponent<Text>();

	/* Create daytime counter */
	m_daytimeCounter = &m_sceneManager.GetCurrentScene().CreateGameObject("DaytimeCounter");
	m_daytimeCounter->AddComponent<Text>();

	/* Create crafting menu */
	m_craftingMenu = &m_sceneManager.GetCurrentScene().CreateGameObject("CraftingMenu");
	m_craftingMenu->AddComponent<Sprite>();

	/* Create current recipe name */
	m_currentRecipeResult = &m_sceneManager.GetCurrentScene().CreateGameObject("CurrentRecipeResult");
	m_currentRecipeResult->AddComponent<Text>();
	m_currentRecipeResult->AddComponent<Sprite>();

	/* Create current recipe information */
	m_currentRecipeInformation = &m_sceneManager.GetCurrentScene().CreateGameObject("CurrentRecipeInformation");
	m_currentRecipeInformation->AddComponent<Text>();

	/* Create current recipe requirements */
	for (uint8_t i = 0; i < 3; ++i)
	{
		m_currentRecipeRequirement[i] = &m_sceneManager.GetCurrentScene().CreateGameObject("CurrentRecipeRequirement" + std::to_string(i));
		m_currentRecipeRequirement[i]->AddComponent<Sprite>();
		m_currentRecipeRequirement[i]->AddComponent<Text>();
	}

	/* Create health hearts */
	for (uint8_t i = 0; i < 10; ++i)
	{
		m_healthHeart[i] = &m_sceneManager.GetCurrentScene().CreateGameObject("HealthHeart" + std::to_string(i));
		m_healthHeart[i]->AddComponent<Sprite>();
	}

	/* Create hunger icons */
	for (uint8_t i = 0; i < 10; ++i)
	{
		m_hungerIcon[i] = &m_sceneManager.GetCurrentScene().CreateGameObject("HungerIcon" + std::to_string(i));
		m_hungerIcon[i]->AddComponent<Sprite>();
	}

	/* Create hand */
	m_hand = &m_sceneManager.GetCurrentScene().CreateGameObject("Hand");
	m_hand->AddComponent<Sprite>();
	m_hand->AddComponent<HandAnimation>();

	m_handItem = &m_sceneManager.GetCurrentScene().CreateGameObject("HandItem");
	m_handItem->AddComponent<Sprite>();
}

void ElkCraft::System::GameStatePlay::InitComponents()
{
	/* Init player */
	auto playerData = m_saveManager.GetPlayerData();
	m_player->transform->SetRotation(playerData.forward);
	m_player->transform->SetPosition(GetPlayerStartPosition());
	auto inventory = m_player->GetComponent<Inventory>();

	for (uint8_t i = 0; i < 20; i += 2)
	{
		inventory->GetSlot(i / 2).type = playerData.itemsIDAndAmount[i];
		inventory->GetSlot(i / 2).quantity = playerData.itemsIDAndAmount[i + 1];
	}

	m_player->GetComponent<HealthSystem>()->SetHealth(playerData.health);
	m_player->GetComponent<HungerSystem>()->SetHunger(playerData.energy);

	/* Init UI Camera */
	m_UICamera->transform->SetPosition(glm::vec3(0, 0, 0.1f));
	m_UICamera->transform->SetRotation(glm::vec3(0, -90, 0));

	/* Init FPS counter */
	Text& fpsCounterText = *m_fpsCounter->GetComponent<Text>();
	m_fpsCounter->transform->Translate(glm::vec3(0.09f, 0.052f, 0));
	m_fpsCounter->transform->SetScale(glm::vec3(0.001f, 0.001f, 0.001f));
	fpsCounterText.SetString("?? FPS");
	fpsCounterText.SetSpacing(0.3f);

	/* Init daytime counter */
	Text& daytimeCounterText = *m_daytimeCounter->GetComponent<Text>();
	m_daytimeCounter->transform->Translate(glm::vec3(-0.095f, 0.052f, 0));
	m_daytimeCounter->transform->SetScale(glm::vec3(0.001f, 0.001f, 0.001f));
	daytimeCounterText.SetString("00:00");
	daytimeCounterText.SetSpacing(0.3f);

	/* Init crafting menu */
	Sprite& craftingMenuSprite = *m_craftingMenu->GetComponent<Sprite>();
	craftingMenuSprite.SetTexture(*m_textureManager.RequireAndGet("Crafting_Menu"));
	m_craftingMenu->transform->SetScale(glm::vec3(0.02f * 1.764f, 0.02f, 0.02f));
	craftingMenuSprite.SetRenderLayer(3);

	/* Init current recipe name */
	Text& currentRecipeResultText = *m_currentRecipeResult->GetComponent<Text>();
	Sprite& currentRecipeResultSprite = *m_currentRecipeResult->GetComponent<Sprite>();
	m_currentRecipeResult->transform->Translate(glm::vec3(0.021f, 0.0f, 0.f));
	m_currentRecipeResult->transform->SetScale(glm::vec3(0.0032f, 0.0032f, 0.0032f));
	currentRecipeResultSprite.SetRenderLayer(4);
	currentRecipeResultText.SetPositionOffset(glm::vec3(0.0032f, -0.0025f, 0.f));
	currentRecipeResultText.SetSpacing(0.3f);
	currentRecipeResultText.SetFontSize(0.5f);

	/* Init current recipe information */
	Text& currentRecipeInformation = *m_currentRecipeInformation->GetComponent<Text>();
	m_currentRecipeInformation->transform->Translate(glm::vec3(-0.017f, -0.013f, 0.f));
	m_currentRecipeInformation->transform->SetScale(glm::vec3(0.0032f, 0.0032f, 0.0032f));
	currentRecipeInformation.SetFontSize(0.5f);
	currentRecipeInformation.SetSpacing(0.3f);

	/* Init current recipe requirements */
	for (uint8_t i = 0; i < 3; ++i)
	{
		Sprite& currentRecipeRequirementSprite = *m_currentRecipeRequirement[i]->GetComponent<Sprite>();
		Text& currentRecipeRequirementText = *m_currentRecipeRequirement[i]->GetComponent<Text>();
		m_currentRecipeRequirement[i]->transform->SetScale(glm::vec3(0.003f, 0.003f, 0.003f));
		currentRecipeRequirementSprite.SetRenderLayer(4);
		currentRecipeRequirementText.SetFontSize(0.5f);
		currentRecipeRequirementText.SetFontColor(glm::vec4(0.f, 0.f, 0.f, 1.f));
		currentRecipeRequirementText.SetPositionOffset(glm::vec3(0.0055f, 0.f, 0.f));
		currentRecipeRequirementText.SetSpacing(0.4f);
	}

	/* Init crosshair */
	Sprite& crosshairSprite = *m_crosshair->GetComponent<Sprite>();
	m_crosshair->transform->Translate(glm::vec3(0, 0, 0));
	m_crosshair->transform->SetScale(glm::vec3(0.002f, 0.002f, 0.002f));
	crosshairSprite.SetTexture(*m_textureManager.RequireAndGet("Crosshair"));
	crosshairSprite.SetRenderLayer(2);

	/* Init inventory selector */
	Sprite& inventorySelectorSprite = *m_inventorySelector->GetComponent<Sprite>();
	m_inventorySelector->transform->SetScale(glm::vec3(0.0045f, 0.0045f, 0.0045f));
	inventorySelectorSprite.SetTexture(*m_textureManager.RequireAndGet("Inventory_Selector"));
	inventorySelectorSprite.SetRenderLayer(4);

	/* Init inventory bar */
	Sprite& inventoryBarSprite = *m_inventoryBar->GetComponent<Sprite>();
	m_inventoryBar->transform->Translate(glm::vec3(0, -0.05f, 0));
	m_inventoryBar->transform->SetScale(glm::vec3(0.004f * 10, 0.004f, 0.004f));
	inventoryBarSprite.SetTexture(*m_textureManager.RequireAndGet("Inventory_Bar"));
	inventoryBarSprite.SetRenderLayer(2);

	/* Init inventory slots */
	for (uint8_t i = 0; i < 10; ++i)
	{
		Sprite& inventoryBarSlotSprite	= *m_inventoryBarSlots[i]->GetComponent<Sprite>();
		Text& inventoryBarSlotText	= *m_inventoryBarSlots[i]->GetComponent<Text>();
		m_inventoryBarSlots[i]->transform->SetPosition(glm::vec3(-0.036f, -0.05f, 0) + glm::vec3(0.008f, 0.0f, 0.0f) * float(i));
		m_inventoryBarSlots[i]->transform->SetScale(glm::vec3(0.003f, 0.003f, 0.003f));
		inventoryBarSlotSprite.SetRenderLayer(3);
		inventoryBarSlotText.SetFontSize(0.5f);
		inventoryBarSlotText.SetSpacing(0.3f);
	}

	/* Init Keypad Cheat result */
	Sprite& keypadCheatSprite = *m_inventoryKeypadCheatResult->GetComponent<Sprite>();
	Text& keypadCheatText = *m_inventoryKeypadCheatResult->GetComponent<Text>();
	m_inventoryKeypadCheatResult->transform->SetPosition(glm::vec3(0.09f, -0.045f, 0));
	m_inventoryKeypadCheatResult->transform->SetScale(glm::vec3(0.003f, 0.003f, 0.003f));
	keypadCheatText.SetSpacing(0.3f);
	keypadCheatText.SetFontSize(0.5f);
	keypadCheatText.SetPositionOffset(glm::vec3(0.f, -0.005f, 0.f));

	/* Init screen filter */
	Sprite& screenFilterSprite = *m_screenFilter->GetComponent<Sprite>();
	screenFilterSprite.SetRenderLayer(1);
	screenFilterSprite.SetActive(false);
	m_screenFilter->transform->SetPosition(glm::vec3(0.f, 0.f, -65.f));
	m_screenFilter->transform->SetScale(glm::vec3(10000.f));

	/* Init health hearts */
	for (uint8_t i = 0; i < 10; ++i)
	{
		Sprite& heartSprite = *m_healthHeart[i]->GetComponent<Sprite>();
		heartSprite.SetRenderLayer(2);
		heartSprite.SetTexture(*m_textureManager.RequireAndGet("Heart_Full"));
		m_healthHeart[i]->transform->SetPosition(glm::vec3(-23.f, -25.f, -60.f) + glm::vec3(1.8f, 0.f, 0.f) * float(i));
		m_healthHeart[i]->transform->SetScale(glm::vec3(1.f, 1.f, 1.f));
	}

	/* Init hunger icons */
	for (uint8_t i = 0; i < 10; ++i)
	{
		Sprite& hungerSprite = *m_hungerIcon[i]->GetComponent<Sprite>();
		hungerSprite.SetRenderLayer(2);
		hungerSprite.SetTexture(*m_textureManager.RequireAndGet("Hunger_Full"));
		m_hungerIcon[i]->transform->SetPosition(glm::vec3(23.f, -25.f, -60.f) - glm::vec3(1.8f, 0.f, 0.f) * float(i));
		m_hungerIcon[i]->transform->SetScale(glm::vec3(1.f, 1.f, 1.f));
	}

	/* Init hand */
	Sprite& handSprite = *m_hand->GetComponent<Sprite>();
	handSprite.SetTexture(*m_textureManager.RequireAndGet("Hand"));
	handSprite.SetRenderLayer(2);
	m_hand->transform->SetScale(glm::vec3(12.f * 1.1156f, 12.f, 12.f));

	Sprite& handItemSprite = *m_hand->GetComponent<Sprite>();
	handItemSprite.SetRenderLayer(3);
}

void ElkCraft::System::GameStatePlay::HandleInputs()
{
	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_ADD, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN)
		|| m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_EQUAL, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		ElkTools::Utils::Time::Scale(2.f);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_KP_SUBTRACT, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN)
		|| m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_MINUS, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		ElkTools::Utils::Time::Scale(0.5f);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_R, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		m_player->transform->SetPosition(GetPlayerStartPosition());

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_E, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
		SetCraftMenuOpen(!m_craftMenuIsOpen);

	if (m_inputManager.IsKeyEventOccured(ElkGameEngine::Managers::InputManager::Key::KEY_ESCAPE, ElkGameEngine::Managers::InputManager::KeyState::KEY_DOWN))
	{
		if (m_craftMenuIsOpen)
			SetCraftMenuOpen(false);
		else
			ElkCraft::System::GameStateManager::SetCurrentState(ElkCraft::System::GameStateManager::GameState::MENU_STATE);
	}
}

void ElkCraft::System::GameStatePlay::UpdateScreenFilter()
{
	PlayerController& playerController = *m_player->GetComponent<PlayerController>();
	Sprite& screenFilterSprite = *m_screenFilter->GetComponent<Sprite>();

	switch (m_player->GetComponent<PlayerController>()->GetBlockNearFromHead())
	{
	case 13:
		screenFilterSprite.SetTexture(*m_textureManager.RequireAndGet("Water_Filter"));
		screenFilterSprite.SetActive(true);
		break;

	case 14:
		screenFilterSprite.SetTexture(*m_textureManager.RequireAndGet("Lava_Filter"));
		screenFilterSprite.SetActive(true);
		break;

	default:
		screenFilterSprite.SetActive(false);
		break;
	}
}

void ElkCraft::System::GameStatePlay::UpdateFPSCounter()
{
	Text& fpsCounterText = *m_fpsCounter->GetComponent<Text>();

	if (m_fpsCounterTimer > m_fpsCounterUpdateFrequency)
	{
		m_fpsCounterTimer = 0.f;
		fpsCounterText.SetString(std::to_string(static_cast<uint16_t>(std::min(m_renderingMaxFramerate, Time::GetFramerate()))) + " FPS");
	}
	else
	{
		m_fpsCounterTimer += Time::GetDeltaTime();
	}
}

void ElkCraft::System::GameStatePlay::UpdateDaytimeCounter()
{
	Text& daytimeCounter = *m_daytimeCounter->GetComponent<Text>();

	uint8_t hours = (uint16_t(m_daytimeTimer * 1.2f) / 60) % 24;
	uint8_t minutes = uint16_t(m_daytimeTimer * 1.2f) % 60;

	std::string s_hours = std::to_string(hours);
	std::string s_minutes = std::to_string(minutes);

	if (s_hours.size() == 1)
		s_hours = '0' + s_hours;

	if (s_minutes.size() == 1)
		s_minutes = '0' + s_minutes;

	daytimeCounter.SetString(s_hours + ":" + s_minutes + " (DAY " + std::to_string(m_days) + ")");
}

void ElkCraft::System::GameStatePlay::UpdateCurrentRecipeDisplay()
{
	if (m_craftMenuIsOpen)
	{
		/* Get recipe */
		Crafting& playerCraftingAbility = *m_player->GetComponent<Crafting>();
		ElkCraft::Utils::Recipe& currentRecipe = playerCraftingAbility.GetCurrentRecipe();
		auto currentRecipeRequirements = currentRecipe.GetRequirements();

		/* Update recipe display name */
		Sprite& currentRecipeResultSprite = *m_currentRecipeResult->GetComponent<Sprite>();
		Text& currentRecipeResultText = *m_currentRecipeResult->GetComponent<Text>();

		/* Update recipe information */
		Text& currentRecipeInformation = *m_currentRecipeInformation->GetComponent<Text>();
		currentRecipeInformation.SetString("");

		if (!playerCraftingAbility.CanCraftCurrentRecipe())
		{
			currentRecipeResultSprite.SetColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
			currentRecipeInformation.SetFontColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
			currentRecipeInformation.SetString("MISSING COMPONENTS");
		}
		else if (!playerCraftingAbility.InventoryHasRoomForCurrentRecipe())
		{
			currentRecipeResultSprite.SetColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
			currentRecipeInformation.SetFontColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
			currentRecipeInformation.SetString("INVENTORY IS FULL");
		}
		else
		{
			currentRecipeResultSprite.SetColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
			currentRecipeInformation.SetFontColor(glm::vec4(0.f, 0.f, 0.f, 1.f));
			currentRecipeInformation.SetString("PRESS [F] TO CRAFT");
		}

		currentRecipeResultSprite.SetTexture(*m_textureManager.RequireAndGet("Object_" + std::to_string(currentRecipe.GetResultType())));
		currentRecipeResultText.SetString(std::to_string(currentRecipe.GetResultQuantity()));

		glm::vec3 firstRequirementPosition = glm::vec3(-0.012f, 0.f, 0.f);
		glm::vec3 requirementYOffset = glm::vec3(0.f, -0.009f, 0.f);

		switch (currentRecipeRequirements.size())
		{
		case 1:
			firstRequirementPosition += glm::vec3(0.f, 0.f, 0.f);
			break;

		case 2:
			firstRequirementPosition += glm::vec3(0.f, 0.005f, 0.f);
			break;

		case 3:
			firstRequirementPosition += glm::vec3(0.f, 0.01f, 0.f);
			break;
		}

		/* Update recipe content info */
		for (uint8_t i = 0; i < 3; ++i)
		{
			m_currentRecipeRequirement[i]->transform->SetPosition(firstRequirementPosition + requirementYOffset * float(i));
			Sprite& currentRecipeRequirementSprite = *m_currentRecipeRequirement[i]->GetComponent<Sprite>();
			Text& currentRecipeRequirementText = *m_currentRecipeRequirement[i]->GetComponent<Text>();

			if (i < currentRecipeRequirements.size())
			{
				if (playerCraftingAbility.IsRequirementOK(i))
					currentRecipeRequirementSprite.SetColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
				else
					currentRecipeRequirementSprite.SetColor(glm::vec4(1.f, 0.f, 0.f, 1.f));

				currentRecipeRequirementSprite.SetTexture(*m_textureManager.RequireAndGet("Object_" + std::to_string(currentRecipeRequirements[i].objectID)));
				currentRecipeRequirementText.SetString("x" + std::to_string(currentRecipeRequirements[i].quantity));
			}
			else
			{
				currentRecipeRequirementSprite.SetTexture(*m_textureManager.RequireAndGet("Object_0"));
				currentRecipeRequirementText.SetString("");
			}
		}
	}
}

void ElkCraft::System::GameStatePlay::UpdateInventorySelector()
{
	Inventory& playerInventory = *m_player->GetComponent<Inventory>();

	m_inventorySelector->transform->SetPosition(glm::vec3(-0.036f, -0.05f, 0) + glm::vec3(0.008f, 0.0f, 0.0f) * static_cast<float>(playerInventory.GetCurrentSlotIndex()));
}

void ElkCraft::System::GameStatePlay::UpdateInventoryBarSlots()
{
	Inventory& playerInventory = *m_player->GetComponent<Inventory>();

	for (uint8_t i = 0; i < 10; ++i)
	{
		Sprite& inventoryBarSlotSprite	= *m_inventoryBarSlots[i]->GetComponent<Sprite>();
		Text& inventoryBarSlotText		= *m_inventoryBarSlots[i]->GetComponent<Text>();
		uint8_t blockType				= playerInventory.GetSlot(i).type;
		uint8_t quantity				= playerInventory.GetSlot(i).quantity;

		inventoryBarSlotSprite.SetTexture(*m_textureManager.RequireAndGet("Object_" + std::to_string(blockType)));
		inventoryBarSlotText.SetString(quantity > 0 ? std::to_string(quantity) : "");
		inventoryBarSlotText.SetPositionOffset(quantity > 9 ? glm::vec3(0.0012f, -0.0015f, 0.f) : glm::vec3(0.0025f, -0.0015f, 0.f));
	}
}

void ElkCraft::System::GameStatePlay::UpdatePlayer()
{
	m_player->UpdateComponents();
}

void ElkCraft::System::GameStatePlay::UpdateWorld()
{
	m_world->GenerateWorldAroundPlayer(m_player->transform->GetPosition());
}

void ElkCraft::System::GameStatePlay::UpdateHearts()
{
	HealthSystem& healthSystem = *m_player->GetComponent<HealthSystem>();

	if (!healthSystem.IsAlive())
	{
		RespawnPlayer();
	}
	else
	{
		for (uint8_t i = 0; i < 10; ++i)
		{
			Sprite& heartSprite = *m_healthHeart[i]->GetComponent<Sprite>();

			uint8_t indexAsLife = i * 2;

			if (indexAsLife + 1 == healthSystem.GetHealth())
				heartSprite.SetTexture(*m_textureManager.RequireAndGet("Heart_Half"));
			else if (indexAsLife + 1 > healthSystem.GetHealth())
				heartSprite.SetTexture(*m_textureManager.RequireAndGet("Heart_Empty"));
			else
				heartSprite.SetTexture(*m_textureManager.RequireAndGet("Heart_Full"));
		}
	}
}

void ElkCraft::System::GameStatePlay::UpdateHunger()
{
	HungerSystem& hungerSystem = *m_player->GetComponent<HungerSystem>();

	for (uint8_t i = 0; i < 10; ++i)
	{
		Sprite& hungerSprite = *m_hungerIcon[i]->GetComponent<Sprite>();

		uint8_t indexAsHunger = i * 2;

		if (indexAsHunger + 1 == hungerSystem.GetHunger())
			hungerSprite.SetTexture(*m_textureManager.RequireAndGet("Hunger_Half"));
		else if (indexAsHunger + 1 > hungerSystem.GetHunger())
			hungerSprite.SetTexture(*m_textureManager.RequireAndGet("Hunger_Empty"));
		else
			hungerSprite.SetTexture(*m_textureManager.RequireAndGet("Hunger_Full"));
	}
}

void ElkCraft::System::GameStatePlay::UpdateKeypadCheatResult()
{
	uint8_t cheatResult = m_player->GetComponent<Inventory>()->GetKeypadCheatCurrentBlock();
	Sprite& keypadCheatSprite = *m_inventoryKeypadCheatResult->GetComponent<Sprite>();
	Text& keypadCheatText = *m_inventoryKeypadCheatResult->GetComponent<Text>();

	keypadCheatSprite.SetTexture(*m_textureManager.RequireAndGet("Object_" + std::to_string(cheatResult)));
	keypadCheatText.SetString(cheatResult != 0 ? std::to_string(cheatResult) : "");
}

void ElkCraft::System::GameStatePlay::UpdateHand()
{
	m_hand->UpdateComponents();

	Inventory& playerInventory = *m_player->GetComponent<Inventory>();
	Sprite& handItemSprite = *m_handItem->GetComponent<Sprite>();
	float scaleCoefficient = 0.65f;
	glm::vec3 positionOffset = glm::vec3(0.f);

	handItemSprite.SetTexture(*m_textureManager.RequireAndGet("Object_" + std::to_string(playerInventory.GetCurrentSlot().type)));

	if (ItemsInfo::IsTool(playerInventory.GetCurrentSlot().type))
	{
		scaleCoefficient = 1.5f;
		positionOffset = glm::vec3(7.f, 3.f, 0.f);
	}

	m_handItem->transform->SetPosition(m_hand->transform->GetPosition() + glm::vec3(-7.f, 5.f, 0.01f) + positionOffset);
	m_handItem->transform->SetScale(m_hand->transform->GetScale() * scaleCoefficient);
}

void GameStatePlay::Save()
{
	m_world->Save();
	m_saveManager.SavePlayer(*m_player, m_days, static_cast<uint16_t>(m_daytimeTimer));
	m_saveManager.SaveToFile();
}

void ElkCraft::System::GameStatePlay::SetCraftMenuOpen(bool p_value)
{
	m_craftingMenu->GetComponent<Sprite>()->SetActive(p_value);

	for (uint8_t i = 0; i < 3; ++i)
	{
		m_currentRecipeRequirement[i]->GetComponent<Sprite>()->SetActive(p_value);
		m_currentRecipeRequirement[i]->GetComponent<Text>()->SetActive(p_value);
	}

	m_currentRecipeResult->GetComponent<Sprite>()->SetActive(p_value);
	m_currentRecipeResult->GetComponent<Text>()->SetActive(p_value);
	m_currentRecipeInformation->GetComponent<Text>()->SetActive(p_value);

	if (p_value)
		m_player->GetComponent<Crafting>()->OpenMenu();
	else
		m_player->GetComponent<Crafting>()->CloseMenu();

	if (p_value)
		m_player->GetComponent<PlayerController>()->LockMovement();
	else
		m_player->GetComponent<PlayerController>()->UnlockMovement();

	m_craftMenuIsOpen = p_value;
}

void ElkCraft::System::GameStatePlay::RespawnPlayer()
{
	HealthSystem& healthSystem = *m_player->GetComponent<HealthSystem>();
	HungerSystem& hungerSystem = *m_player->GetComponent<HungerSystem>();
	Inventory& inventory = *m_player->GetComponent<Inventory>();
	Rigidbody& rigidbody = *m_player->GetComponent<Rigidbody>();

	healthSystem.SetHealth(20);
	hungerSystem.SetHunger(20);

	for (auto& slot : inventory.GetSlots())
		slot.get().Clear();
	
	m_player->transform->SetPosition(GetPlayerStartPosition());
	rigidbody.SetVelocity(glm::vec3(0.f));
}

glm::vec3 ElkCraft::System::GameStatePlay::GetPlayerStartPosition()
{
	glm::vec3 playerStartPosition;

	if (m_saveManager.IsLoaded())
	{
		playerStartPosition = m_saveManager.GetPlayerData().position;
	}
	else
	{
		playerStartPosition.x = IniLoader::GetFloat("player.ini", "start_position_x");
		playerStartPosition.y = IniLoader::GetFloat("player.ini", "start_position_y");
		playerStartPosition.z = IniLoader::GetFloat("player.ini", "start_position_z");

		while (m_world->GetBlockID(playerStartPosition) != 0)
			playerStartPosition += glm::vec3(0.f, 1.f, 0.f);

		playerStartPosition += glm::vec3(0.f, 2.f, 0.f);
	}

	return playerStartPosition;
}
