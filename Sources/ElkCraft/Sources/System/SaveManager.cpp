#include "stdafx.h"

#include <ElkTools/Debug/Log.h>

#include "ElkCraft/System/SaveManager.h"
#include "ElkCraft/Gameplay/Inventory.h"
#include "ElkCraft/Gameplay/HealthSystem.h"
#include "ElkCraft/Gameplay/HungerSystem.h"

using namespace ElkCraft::System;
using namespace ElkGameEngine::Objects::Components::Behaviours;
namespace Fs = std::experimental::filesystem;

SaveManager::SaveManager(const bool& p_newSave)
	: m_savePath{"Save/"}, m_saveFile("save"), m_extention(".sav"), m_numberOfBlocks(0), m_isLoaded(!p_newSave)
{
	uint8_t lastSlotUsed = 0;

	std::ifstream inSaveInfo(m_savePath + "save.info", std::ios::in);
	if (inSaveInfo.is_open())
	{
		std::string line;
		std::getline(inSaveInfo, line);
		lastSlotUsed = std::atoi(line.c_str());
	}
	inSaveInfo.close();

	if (p_newSave)
	{
		++lastSlotUsed;

		std::ofstream outSaveInfo(m_savePath + "save.info", std::ios::out | std::ios::trunc);
		if (outSaveInfo.is_open())
		{
			outSaveInfo << std::to_string(lastSlotUsed);
		}
		ElkTools::Debug::Log::Process("NEW GAME: Save " + std::to_string(lastSlotUsed) + " created", ElkTools::Debug::Log::LogLevel::LOG_DEFAULT, ElkTools::Debug::Log::LogColor::GREEN);
		outSaveInfo.close();
	}
	else
	{
		ElkTools::Debug::Log::Process("LOAD GAME: Save " + std::to_string(lastSlotUsed) + " used", ElkTools::Debug::Log::LogLevel::LOG_DEFAULT, ElkTools::Debug::Log::LogColor::GREEN);
	}

	m_saveFile += std::to_string(lastSlotUsed);

	if (!p_newSave)
	{
		m_readFile.open(m_savePath + m_saveFile + m_extention, std::ios::binary);
		if (!m_readFile.is_open())
			ElkTools::Debug::Log::Process("Can't open save file : " + m_savePath + m_saveFile + m_extention, ElkTools::Debug::Log::LogLevel::LOG_ERROR);
		else
			Parse();
	}

	m_writeFile.open(m_savePath + "saveWrite.sav", std::ios::binary | std::ios::trunc);
}

SaveManager::~SaveManager()
{
	m_readFile.close();
	m_writeFile.close();

	const Fs::path path = Fs::current_path() / m_savePath;

	Fs::remove(path / (m_saveFile + m_extention));
	Fs::rename(path / "saveWrite.sav", path / (m_saveFile + m_extention));
}

bool SaveManager::SaveFileLoaded() const
{
	return m_readFile.is_open();
}

void SaveManager::Parse()
{
	ParseHeader();
	ParseNoiseProperties();
	ParsePlayer();
	ParseChunks();
}

void SaveManager::ParseHeader()
{
	SetCursorRead(3);

	ReadData<uint8_t>(&m_numberOfBlocks);

	m_blocksSizes.reserve(m_numberOfBlocks);

	for (uint8_t i = 0; i < m_numberOfBlocks; ++i)
	{
		m_blocksSizes.emplace_back(0);

		if (i >= m_numberOfBlocks - 1)
			m_blocksSizes[i] = ReadData<uint32_t>();
		else
			m_blocksSizes[i] = ReadData<uint8_t>();
	}
}

void SaveManager::ParseNoiseProperties()
{
	SetCursorRead(m_blocksSizes[0]);

	m_noiseProperties.redistribution = ReadData<float>();
	m_noiseProperties.persistance = ReadData<float>();
	m_noiseProperties.lacunarity = ReadData<float>();
	m_noiseProperties.scale = ReadData<float>();
	m_noiseProperties.seed = ReadData<uint16_t>();
	m_noiseProperties.octaves = ReadData<uint8_t>();
	m_noiseProperties.maxHeight = ReadData<uint8_t>();
	m_noiseProperties.additionalElevation = ReadData<uint8_t>();
	m_noiseProperties.waterLevel = ReadData<uint8_t>();
}

void SaveManager::ParsePlayer()
{
	SetCursorRead(m_blocksSizes[0] + m_blocksSizes[1]);

	m_playerData.position.x = ReadData<float>();
	m_playerData.position.y = ReadData<float>();
	m_playerData.position.z = ReadData<float>();
	m_playerData.forward.x = ReadData<float>();
	m_playerData.forward.y = ReadData<float>();
	m_playerData.forward.z = ReadData<float>();

	for (uint8_t& i : m_playerData.itemsIDAndAmount)
	{
		i = ReadData<uint8_t>();
	}

	m_playerData.health = ReadData<uint8_t>();
	m_playerData.energy = ReadData<uint8_t>();

	m_playerData.days = ReadData<uint32_t>();
	m_playerData.daytime = ReadData<uint16_t>();
}

void SaveManager::ParseChunks()
{
	SetCursorRead(m_blocksSizes[0] + m_blocksSizes[1] + m_blocksSizes[2]);

	uint32_t chunksBlockSize;
	ReadData<uint32_t>(&chunksBlockSize);

	m_savedChunks.clear();

	glm::ivec3 chunkPos;

	for (uint32_t i = 0; i < chunksBlockSize; ++i)
	{
		chunkPos.x = ReadData<int32_t>();
		chunkPos.y = ReadData<int32_t>();
		chunkPos.z = ReadData<int32_t>();

		auto& chunk = m_savedChunks[chunkPos];

		for (uint8_t& block : chunk.blocks)
		{
			block = ReadData<uint8_t>();
		}
	}
}

void SaveManager::Serialize()
{
	SerializeHeader();
	SerializeNoiseProperties();
	SerializePlayer();
	SerializeChunks();
}

void SaveManager::CalculateAndWriteBlocksSizes()
{
	m_blocksSizes.clear();
	m_blocksSizes.emplace_back(11); // size of the Header block
	m_blocksSizes.emplace_back(22); // size of the Map Generation block
	m_blocksSizes.emplace_back(52); // size of the Player Data block
	m_blocksSizes.emplace_back(static_cast<uint32_t>(m_savedChunks.size() * 4108)); // 4108 is the Chunk size to save in bytes

	m_numberOfBlocks = static_cast<uint8_t>(m_blocksSizes.size());
	WriteData<uint8_t>(m_numberOfBlocks); // number of blocks

	for (uint8_t i = 0; i < m_numberOfBlocks; ++i)
	{
		if (i >= m_numberOfBlocks - 1)
			WriteData<uint32_t>(m_blocksSizes[i]);
		else
			WriteData<uint8_t>(m_blocksSizes[i]);
	}
}

void SaveManager::SerializeHeader()
{
	SetCursorWrite(0);

	uint8_t signature[3] = {'s', 'a', 'v'};
	WriteData<uint8_t>(signature[0]);
	WriteData<uint8_t>(signature[1]);
	WriteData<uint8_t>(signature[2]);

	CalculateAndWriteBlocksSizes();
}

void SaveManager::SerializeNoiseProperties()
{
	SetCursorWrite(m_blocksSizes[0]);

	WriteData<float>(m_noiseProperties.redistribution);
	WriteData<float>(m_noiseProperties.persistance);
	WriteData<float>(m_noiseProperties.lacunarity);
	WriteData<float>(m_noiseProperties.scale);
	WriteData<uint16_t>(m_noiseProperties.seed);
	WriteData<uint8_t>(m_noiseProperties.octaves);
	WriteData<uint8_t>(m_noiseProperties.maxHeight);
	WriteData<uint8_t>(m_noiseProperties.additionalElevation);
	WriteData<uint8_t>(m_noiseProperties.waterLevel);
}

void SaveManager::SerializePlayer()
{
	SetCursorWrite(m_blocksSizes[0] + m_blocksSizes[1]);

	WriteData<float>(m_playerData.position.x);
	WriteData<float>(m_playerData.position.y);
	WriteData<float>(m_playerData.position.z);
	WriteData<float>(m_playerData.forward.x);
	WriteData<float>(m_playerData.forward.y);
	WriteData<float>(m_playerData.forward.z);

	for (uint8_t& i : m_playerData.itemsIDAndAmount)
	{
		WriteData<uint8_t>(i);
	}

	WriteData<uint8_t>(m_playerData.health);
	WriteData<uint8_t>(m_playerData.energy);

	WriteData<uint32_t>(m_playerData.days);
	WriteData<uint16_t>(m_playerData.daytime);
}

void SaveManager::SerializeChunks()
{
	SetCursorWrite(m_blocksSizes[0] + m_blocksSizes[1] + m_blocksSizes[2]);

	WriteData<uint32_t>(static_cast<uint32_t>(m_savedChunks.size()));

	for (auto& it : m_savedChunks)
	{
		WriteData<int32_t>(&it.first.x);
		WriteData<int32_t>(&it.first.y);
		WriteData<int32_t>(&it.first.z);

		for (uint8_t& block : it.second.blocks)
		{
			WriteData<uint8_t>(block);
		}
	}
}

void SaveManager::SaveNoiseProperties(const ElkCraft::Terrain::NoiseProperties& p_noiseProperties)
{
	m_noiseProperties = p_noiseProperties;
}

void SaveManager::SavePlayer(ElkGameEngine::Objects::GameObject& p_player, uint8_t p_days, uint16_t p_daytime)
{
	m_playerData.position = p_player.transform->GetPosition();
	m_playerData.forward = p_player.transform->GetRotation();
	
	if (auto inventory = p_player.GetComponent<Gameplay::Inventory>())
	{
		for (uint8_t i = 0; i < 20; i += 2)
		{
			m_playerData.itemsIDAndAmount[i] = inventory->GetSlot(i / 2).type;
			m_playerData.itemsIDAndAmount[i + 1] = inventory->GetSlot(i / 2).quantity;
		}
	}
	if (auto healthSystem = p_player.GetComponent<Gameplay::HealthSystem>())
	{
		m_playerData.health = healthSystem->GetHealth();
	}
	if (auto hungerSystem = p_player.GetComponent<Gameplay::HungerSystem>())
	{
		m_playerData.energy = hungerSystem->GetHunger();
	}

	m_playerData.days = p_days;
	m_playerData.daytime = p_daytime;
}

void SaveManager::SaveChunk(Chunk& p_chunk)
{
	glm::ivec3 pos = p_chunk.GetOwner().transform->GetPosition();

	m_savedChunks[pos] = ChunkData(p_chunk);
}

const ElkCraft::Terrain::NoiseProperties& SaveManager::GetNoiseProperties() const
{
	return m_noiseProperties;
}

const SaveManager::PlayerData& SaveManager::GetPlayerData() const
{
	return m_playerData;
}

std::shared_ptr<SaveManager::ChunkData> SaveManager::GetChunkData(const glm::ivec3& p_coords) const
{
	if (m_savedChunks.find(p_coords) != m_savedChunks.end())
	{
		auto it = m_savedChunks.find(p_coords);
		return std::make_shared<SaveManager::ChunkData>(m_savedChunks.at(p_coords));
	}

	return nullptr;
}

bool SaveManager::IsLoaded() const
{
	return m_isLoaded;
}

void SaveManager::SetCursorRead(const uint16_t& p_offsetFromBeg)
{
	m_readFile.seekg(p_offsetFromBeg, std::ios::beg);
}

void SaveManager::SetCursorWrite(const uint16_t& p_offsetFromBeg)
{
	m_writeFile.seekp(p_offsetFromBeg, std::ios::beg);
}


glm::uvec3 SaveManager::From1Dto3D(uint64_t p_index)
{
	const uint64_t y = p_index / (16 * 16);
	p_index -= y * (16 * 16);
	const uint64_t x = p_index % 16;
	const uint64_t z = p_index / 16;

	return glm::uvec3(x, y, z);
}
