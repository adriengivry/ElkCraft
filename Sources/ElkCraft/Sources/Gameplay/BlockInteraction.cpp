#include "stdafx.h"

#include "ElkCraft/Gameplay/BlockInteraction.h"

using namespace ElkTools::Utils;
using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkCraft::Gameplay;
using namespace ElkCraft::Utils;

BlockInteraction::BlockInteraction(ElkCraft::Terrain::World* p_world) : 
	m_world{ p_world },
	m_inputManager{ ManagerLocator::Get<ElkGameEngine::Managers::InputManager>() },
	m_renderingManager { ManagerLocator::Get<ElkRendering::Managers::RenderingManager>() },
	m_audioManager{ ManagerLocator::Get<ElkTools::Managers::AudioManager>() }
{
	m_interactionDistance = IniLoader::GetFloat("player.ini", "raycast_distance");
	m_raycastSteps = IniLoader::GetFloat("player.ini", "raycast_precision");
	m_instantBreaking = IniLoader::GetBool("player.ini", "instant_breaking");
	m_inventory = nullptr;

	m_breakBlockTimer = 0.0f;

	m_breakSoundFrequency = 0.21f;
	m_breakSoundTimer = m_breakSoundFrequency;
}

void BlockInteraction::Update()
{
	if (!m_inventory)
		m_inventory = m_owner->GetComponent<ElkCraft::Gameplay::Inventory>();

	if (!m_playerController)
		m_playerController = m_owner->GetComponent<PlayerController>();
	
	m_previousBlockInFront = m_blockInFront;
	Raycast(m_owner->transform->GetPosition(), m_owner->transform->GetForward(), m_interactionDistance, m_blockInFront);
	if (m_blockInFront.found)
	{
		m_renderingManager.SetOutlineState(true);
		m_renderingManager.SetOutlinePosition(m_blockInFront.blockPosition);
	}
	else
	{
		m_renderingManager.SetOutlineState(false);
	}
	HandleInputs();
	
}

void BlockInteraction::HandleInputs()
{
	uint8_t blockInFrontDestructionLevel = 0;

	if (m_blockInFront.found)
	{
		if (BreakBlockAction())
		{
			if (m_instantBreaking)
				BreakBlock();
			else
			{
				m_breakBlockTimer += ElkTools::Utils::Time::GetDeltaTime() * CalculateBreakingMultiplier(m_inventory->GetCurrentSlot().type, m_blockInFront.type);

				if (m_blockInFront.blockPosition != m_previousBlockInFront.blockPosition || m_previousInventorySlot != m_inventory->GetCurrentSlotIndex())
				{
					m_breakBlockTimer = 0.f;
					m_breakSoundTimer = m_breakSoundFrequency * 0.5f;
				}

				m_previousInventorySlot = m_inventory->GetCurrentSlotIndex();

				if (m_breakSoundTimer >= m_breakSoundFrequency)
				{
					m_breakSoundTimer = 0.0f;
					PlayBlockSound(m_blockInFront.type, InteractionAction::BREAKING);
				}

				m_breakSoundTimer += Time::GetDeltaTime();

				float blockBreakDuration = BlocksInfo::GetBlockBreakDuration(m_blockInFront.type);
				if (m_playerController && m_playerController->IsSwimming())
					blockBreakDuration *= 5.f;
				blockInFrontDestructionLevel = static_cast<uint8_t>((m_breakBlockTimer / blockBreakDuration) * 10.f + 1);

				if (m_breakBlockTimer >= blockBreakDuration)
				{
					BreakBlock();
					m_breakBlockTimer = 0.f;
					m_breakSoundTimer = m_breakSoundFrequency;
				}
			}
		}
		else
		{
			m_breakBlockTimer = 0.f;
			m_breakSoundTimer = m_breakSoundFrequency;
		}

		if (PlaceBlockAction())
			PlaceBlock();
	}
	else
	{
		m_renderingManager.SetDestructionInfo(m_blockInFront.blockPosition, 0);
	}

	m_renderingManager.SetDestructionInfo(m_blockInFront.blockPosition, blockInFrontDestructionLevel);
}

void BlockInteraction::BreakBlock()
{
	m_world->SetBlock(m_blockInFront.blockPosition.x, m_blockInFront.blockPosition.y, m_blockInFront.blockPosition.z, 0, true);

	/* 10% chance to get an apple on leaves */
	if (static_cast<BlocksInfo::BlockType>(m_blockInFront.type) == BlocksInfo::BlockType::LEAVES)
		if (ElkTools::Utils::Random::Generate(1, 10) == 1)
			m_inventory->AddObject(static_cast<uint8_t>(ItemsInfo::ItemType::APPLE));

	if (!BlocksInfo::IsOre(m_blockInFront.type) || ItemsInfo::IsEfficientOn(m_inventory->GetCurrentSlot().type, m_blockInFront.type))
		m_inventory->AddObject(BlocksInfo::GetBreakResult(m_blockInFront.type));	

	PlayBlockSound(m_blockInFront.type, InteractionAction::BREAK);

	/* Remove top block if is flower */
	uint8_t topBlock = m_world->GetBlockID(m_blockInFront.blockPosition.x, m_blockInFront.blockPosition.y + 1, m_blockInFront.blockPosition.z);
	if (BlocksInfo::IsFlower(topBlock))
	{
		m_world->SetBlock(m_blockInFront.blockPosition.x, m_blockInFront.blockPosition.y + 1, m_blockInFront.blockPosition.z, 0, true);
		m_inventory->AddObject(topBlock);
	}
}

void BlockInteraction::PlaceBlock()
{
	if (!m_inventory->GetCurrentSlot().IsEmpty() && !ItemsInfo::IsItem(m_inventory->GetCurrentSlot().type))
	{
		if (!BlocksInfo::IsFlower(m_blockInFront.type))
		{
			glm::vec3 newBlock = m_blockInFront.blockPosition + m_blockInFront.collisionFaceNormal;
			uint8_t blockToReplace = m_world->GetBlockID(newBlock.x, newBlock.y, newBlock.z);
			if (blockToReplace == 0 ||  ElkTools::Utils::BlocksInfo::IsLiquid(blockToReplace))
			{
				bool newBlockIsInPlayer = false;

				glm::vec3 roundedPlayerUpPosition = glm::round(m_owner->transform->GetPosition());
				glm::vec3 roundedPlayerDownPosition = roundedPlayerUpPosition - glm::vec3(0.f, 1.0f, 0.f);
				glm::vec3 roundedPlayerDownDownPosition = roundedPlayerUpPosition - glm::vec3(0.f, 2.f, 0.f);
				if (newBlock == roundedPlayerUpPosition || newBlock == roundedPlayerDownPosition || newBlock == roundedPlayerDownDownPosition)
					newBlockIsInPlayer = true;

				roundedPlayerUpPosition = glm::round(m_owner->transform->GetPosition() - glm::vec3(0.25f, 0.f, 0.f));
				roundedPlayerDownPosition = roundedPlayerUpPosition - glm::vec3(0.f, 1.0f, 0.f);
				roundedPlayerDownDownPosition = roundedPlayerUpPosition - glm::vec3(0.f, 2.f, 0.f);
				if (newBlock == roundedPlayerUpPosition || newBlock == roundedPlayerDownPosition || newBlock == roundedPlayerDownDownPosition)
					newBlockIsInPlayer = true;

				roundedPlayerUpPosition = glm::round(m_owner->transform->GetPosition() + glm::vec3(0.25f, 0.f, 0.f));
				roundedPlayerDownPosition = roundedPlayerUpPosition - glm::vec3(0.f, 1.0f, 0.f);
				roundedPlayerDownDownPosition = roundedPlayerUpPosition - glm::vec3(0.f, 2.f, 0.f);
				if (newBlock == roundedPlayerUpPosition || newBlock == roundedPlayerDownPosition || newBlock == roundedPlayerDownDownPosition)
					newBlockIsInPlayer = true;

				roundedPlayerUpPosition = glm::round(m_owner->transform->GetPosition() - glm::vec3(0.f, 0.f, 0.25f));
				roundedPlayerDownPosition = roundedPlayerUpPosition - glm::vec3(0.f, 1.0f, 0.f);
				roundedPlayerDownDownPosition = roundedPlayerUpPosition - glm::vec3(0.f, 2.f, 0.f);
				if (newBlock == roundedPlayerUpPosition || newBlock == roundedPlayerDownPosition || newBlock == roundedPlayerDownDownPosition)
					newBlockIsInPlayer = true;

				roundedPlayerUpPosition = glm::round(m_owner->transform->GetPosition() + glm::vec3(0.f, 0.f, 0.25f));
				roundedPlayerDownPosition = roundedPlayerUpPosition - glm::vec3(0.f, 1.0f, 0.f);
				roundedPlayerDownDownPosition = roundedPlayerUpPosition - glm::vec3(0.f, 2.f, 0.f);
				if (newBlock == roundedPlayerUpPosition || newBlock == roundedPlayerDownPosition || newBlock == roundedPlayerDownDownPosition)
					newBlockIsInPlayer = true;

				if (!newBlockIsInPlayer)
				{
					uint8_t blockToPlaceType = m_inventory->GetCurrentSlot().type;
					uint8_t bottomBlock = m_world->GetBlockID(newBlock - glm::vec3(0.f, 1.f, 0.f));

					if (!ElkTools::Utils::BlocksInfo::IsFlower(blockToPlaceType) || ElkTools::Utils::BlocksInfo::IsFertileSoil(bottomBlock))
					{
						m_world->SetBlock(newBlock.x, newBlock.y, newBlock.z, blockToPlaceType, true);
						PlayBlockSound(m_inventory->GetCurrentSlot().type, InteractionAction::PLACE);
						m_inventory->RemoveCurrentObject();
					}
					else
					{
						ElkTools::Debug::Log::Process("Flowers can only be placed on a fertile soil");
					}
				}
				else
				{
					ElkTools::Debug::Log::Process("You can't place a block on yourself");
				}
			}
		}
		else
		{
			ElkTools::Debug::Log::Process("Flower isn't a good place to place a block!");
		}
	}
}

bool BlockInteraction::BreakBlockAction()
{
	if (m_instantBreaking)
		return m_inputManager.IsMouseEventOccured(InputManager::MouseButton::MOUSE_BUTTON_LEFT, InputManager::MouseButtonState::MOUSE_DOWN);
	else
		return m_inputManager.IsMouseButtonDown(InputManager::MouseButton::MOUSE_BUTTON_LEFT);
}

bool BlockInteraction::PlaceBlockAction()
{
	return m_inputManager.IsMouseEventOccured(InputManager::MouseButton::MOUSE_BUTTON_RIGHT, InputManager::MouseButtonState::MOUSE_DOWN);
}

float ElkCraft::Gameplay::BlockInteraction::CalculateBreakingMultiplier(uint8_t p_type, uint8_t p_block)
{
	float result = 1.f;

	if (ItemsInfo::IsItem(p_type) && ItemsInfo::IsEfficientOn(p_type, p_block))
	{
		switch (ItemsInfo::GetQuality(p_type))
		{
		case ItemsInfo::Quality::NONE:
			result *= 1.f;
			break;

		case ItemsInfo::Quality::WOOD:
			result *= 2.f;
			break;

		case ItemsInfo::Quality::STONE:
			result *= 4.f;
			break;
		}
	}

	return result;
}

void BlockInteraction::PlayBlockSound(uint8_t p_blockType, InteractionAction p_action)
{
	std::string soundsFolder = "../Resources/Audio/Sounds/block_interaction/";
	std::string randomSound;
	float volume = 1.f;
	float playbackSpeed = 1.f;

	if (p_action == InteractionAction::BREAKING)
	{
		soundsFolder = "../Resources/Audio/Sounds/step/";
		playbackSpeed = 0.6f;
		
		switch (BlocksInfo::GetBlockCategory(m_blockInFront.type))
		{
		case BlocksInfo::Category::GRASS:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "grass", ".ogg", 6);
			break;

		case BlocksInfo::Category::GRAVEL:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "gravel", ".ogg", 4);
			break;

		case BlocksInfo::Category::SAND:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "sand", ".ogg", 5);
			break;

		case BlocksInfo::Category::GLASS:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "cloth", ".ogg", 4);
			break;

		case BlocksInfo::Category::STONE:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "stone", ".ogg", 6);
			break;

		case BlocksInfo::Category::WOOD:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "wood", ".ogg", 6);
			break;
		}

		m_audioManager.Play3DSound(randomSound, m_blockInFront.blockPosition.x, m_blockInFront.blockPosition.y, m_blockInFront.blockPosition.z, false, playbackSpeed, volume);
	}
	else
	{
		soundsFolder = "../Resources/Audio/Sounds/block_interaction/";

		switch (BlocksInfo::GetBlockCategory(p_blockType))
		{
		default:
			break;

		case BlocksInfo::Category::GRASS:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "grass", ".ogg", 4);
			break;

		case BlocksInfo::Category::GRAVEL:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "gravel", ".ogg", 4);
			break;

		case BlocksInfo::Category::SAND:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "sand", ".ogg", 4);
			break;

		case BlocksInfo::Category::GLASS:
			if (p_action == InteractionAction::BREAK)
				randomSound = m_audioManager.FindRandomSound(soundsFolder, "glass", ".ogg", 3);
			else if (p_action == InteractionAction::PLACE)
				randomSound = m_audioManager.FindRandomSound(soundsFolder, "cloth", ".ogg", 4);
			break;

		case BlocksInfo::Category::STONE:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "stone", ".ogg", 4);
			break;

		case BlocksInfo::Category::WOOD:
			randomSound = m_audioManager.FindRandomSound(soundsFolder, "wood", ".ogg", 4);
			break;
		}
	}

	m_audioManager.Play3DSound(randomSound, m_blockInFront.blockPosition.x, m_blockInFront.blockPosition.y, m_blockInFront.blockPosition.z, false, volume, playbackSpeed);
}

void BlockInteraction::Raycast(glm::vec3 p_source, glm::vec3 p_direction, float p_maxDistance, RaycastCollision& p_out)
{
	RaycastCollision& result = p_out;

	result.found = false;
	result.type = 0;
	result.collisionPoint = glm::vec3(0.0f);
	result.blockPosition = glm::vec3(0.0f);
	result.collisionFaceNormal = glm::vec3(0.0f);

	for (uint64_t distance = 0; distance < (p_maxDistance / m_raycastSteps) && !result.found; ++distance)
	{
		glm::vec3 destination = p_source + p_direction * (distance * m_raycastSteps);

		result.type = m_world->GetBlockID(destination.x, destination.y, destination.z);

		if (!BlocksInfo::IsIgnoringRaycast(result.type))
		{
			result.found = true;
			result.collisionPoint = destination;
			result.blockPosition.x = std::round(destination.x);
			result.blockPosition.y = std::round(destination.y);
			result.blockPosition.z = std::round(destination.z);
			result.blockToCollisionPointDirection = glm::normalize(result.collisionPoint - result.blockPosition);

			/* Calculate face normal */
			glm::vec3 unsignedDirectionFromBlockToImpact = glm::vec3(std::abs(result.blockToCollisionPointDirection.x), std::abs(result.blockToCollisionPointDirection.y), std::abs(result.blockToCollisionPointDirection.z));
			if (unsignedDirectionFromBlockToImpact.x > unsignedDirectionFromBlockToImpact.y && unsignedDirectionFromBlockToImpact.x > unsignedDirectionFromBlockToImpact.z)
				result.collisionFaceNormal = result.blockToCollisionPointDirection.x > 0 ? glm::vec3(1.f, 0.f, 0.f) : glm::vec3(-1.f, 0.f, 0.f);
			else if (unsignedDirectionFromBlockToImpact.y > unsignedDirectionFromBlockToImpact.x && unsignedDirectionFromBlockToImpact.y > unsignedDirectionFromBlockToImpact.z)
				result.collisionFaceNormal = result.blockToCollisionPointDirection.y > 0 ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(0.f, -1.f, 0.f);
			else
				result.collisionFaceNormal = result.blockToCollisionPointDirection.z > 0 ? glm::vec3(0.f, 0.f, 1.f) : glm::vec3(0.f, 0.f, -1.f);

			result.collisionFaceNormal;
		}
	}
}
