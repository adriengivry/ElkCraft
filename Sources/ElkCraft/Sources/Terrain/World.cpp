#include "stdafx.h"

#include "ElkCraft/Terrain/World.h"

using namespace ElkTools::Utils;
using namespace ElkTools::Managers;
using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkCraft::Terrain;

World::World(EngineManager& p_gameEngine, ElkCraft::System::SaveManager& p_saveManager)
	: m_gameEngine {p_gameEngine},
	  m_sceneManager {ManagerLocator::Get<SceneManager>()},
	  m_saveManager(p_saveManager),
	  m_localWorldSize {static_cast<uint8_t>(IniLoader::GetInt("world.ini", "local_world_size"))},
	  m_chunkSize {static_cast<uint8_t>(IniLoader::GetInt("world.ini", "chunk_size"))}
{
	m_noiseProperties = m_saveManager.GetNoiseProperties();

	/* Some calculations that are used many times in this class */
	m_halfLocalWorldSize = m_localWorldSize * 0.5f;
	m_halfChunkSize = static_cast<uint8_t>(m_chunkSize * 0.5f);
	m_xChunkOffset = 0;
	m_zChunkOffset = 0;

	/* We already know the local world size so we reserve memory space for it */
	m_localWorld.reserve(m_localWorldSize * m_localWorldSize * 16);

	InitNoise();
	InitWorldProperties();

	m_fastNoise.SetSeed(m_noiseProperties.seed);
	

	std::string s_fractalType = IniLoader::GetString("world.ini", "fractal_type");
	FastNoise::FractalType fractalType = FastNoise::FastNoise::FBM;
	std::transform(s_fractalType.begin(), s_fractalType.end(), s_fractalType.begin(), ::tolower);

	if (s_fractalType == "billow")
		fractalType = FastNoise::FractalType::Billow;
	else if (s_fractalType == "rigidmulti")
		fractalType = FastNoise::FractalType::RigidMulti;
	else
		fractalType = FastNoise::FractalType::FBM;
	
	m_fastNoise.SetFractalType(fractalType);
	m_fastNoise.SetFractalGain(IniLoader::GetFloat("world.ini", "fractal_gain"));
	m_fastNoise.SetFractalOctaves(IniLoader::GetInt("world.ini", "fractal_octaves"));
	m_fastNoise.SetFrequency(IniLoader::GetFloat("world.ini", "fractal_frequency"));
	m_fastNoise.SetFractalLacunarity(IniLoader::GetFloat("world.ini", "fractal_lacunarity"));
}

World::~World()
{
}


void ElkCraft::Terrain::World::InitNoise()
{
	m_perlin = std::make_unique<PerlinNoise>(m_noiseProperties.seed);
}

void ElkCraft::Terrain::World::InitWorldProperties()
{
	m_terrainLayers.clear();
	m_terrainLayers.emplace_back("Grass", 1, 1);
	m_terrainLayers.emplace_back("Dirt", 2, 2);
	m_terrainLayers.emplace_back("Stone", 4, 255);

	m_terrainProperties.clear();
	m_terrainProperties.emplace_back("Sand", 8, 65, TerrainProperty::Mode::REPLACE);
	m_terrainProperties.emplace_back("Water", 13, 62, TerrainProperty::Mode::FILL);
}

uint64_t World::From2Dto1D(const uint64_t& p_x, const uint64_t& p_y) const
{
	return (p_x + p_y * m_chunkSize);
}

glm::vec2 World::From1Dto2D(const uint64_t& p_index) const
{
	return glm::vec2(p_index % m_chunkSize, floor(p_index / m_chunkSize));
}

uint64_t World::From3Dto1D(const uint64_t& p_x, const uint64_t& p_y, const uint64_t& p_z) const
{
	return p_x + m_localWorldSize * (p_z + p_y * m_localWorldSize);
}

glm::vec3 World::From1Dto3D(uint64_t p_index) const
{
	const uint64_t y = p_index / (m_localWorldSize * m_localWorldSize);
	p_index -= y * (m_localWorldSize * m_localWorldSize);
	const uint64_t x = p_index % m_localWorldSize;
	const uint64_t z = p_index / m_localWorldSize;

	return glm::vec3(x, y, z);
}

glm::uvec3 World::From1Dto3DChunks(uint16_t p_index)
{
	uint64_t index = static_cast<uint64_t>(p_index);

	const uint64_t y = index / (m_chunkSize * m_chunkSize);
	index -= y * (m_chunkSize * m_chunkSize);
	const uint64_t x = index % m_chunkSize;
	const uint64_t z = index / m_chunkSize;

	return glm::vec3(x, y, z);
}

int64_t ElkCraft::Terrain::World::LocalToWorldX(int64_t p_localX)
{
	return static_cast<int64_t>(p_localX - (m_halfLocalWorldSize - m_xChunkOffset) * m_chunkSize);
}

int64_t ElkCraft::Terrain::World::LocalToWorldZ(int64_t p_localZ)
{
	return static_cast<int64_t>(p_localZ - (m_halfLocalWorldSize - m_zChunkOffset) * m_chunkSize);
}

void World::Populate()
{
	CreateChunks();
	SetNeighbours();
	GenerateTerrain();
	UpdateLocalWorldChunks();
}

void World::Clear()
{
	for (auto chunk : m_localWorld)
		m_sceneManager.GetCurrentScene().DestroyGameObject(chunk->GetInstanceID());

	m_localWorld.clear();
}

uint8_t World::GetBlockID(const glm::vec3& p_coords)
{
	return GetBlockID(p_coords.x, p_coords.y, p_coords.z);
}

uint8_t World::GetBlockID(float p_x, float p_y, float p_z)
{
	p_x = round(p_x);
	p_y = round(p_y);
	p_z = round(p_z);

	const float xChunk = (p_x / m_chunkSize) + (m_halfLocalWorldSize) - m_xChunkOffset;
	const float yChunk = (p_y / m_chunkSize);
	const float zChunk = (p_z / m_chunkSize) + (m_halfLocalWorldSize) - m_zChunkOffset;

	if (xChunk < 0 || xChunk >= m_localWorldSize ||
		yChunk < 0 || yChunk >= 16 ||
		zChunk < 0 || zChunk >= m_localWorldSize)
		return 0;

	auto target = m_localWorld[From3Dto1D(static_cast<uint64_t>(xChunk), static_cast<uint64_t>(yChunk),
	                                      static_cast<uint64_t>(zChunk))]->GetComponent<Chunk>();

	return target->GetBlock(static_cast<uint8_t>(p_x + m_halfLocalWorldSize * m_chunkSize) % m_chunkSize,
	                        static_cast<int>(p_y) % m_chunkSize,
	                        static_cast<uint8_t>(p_z + m_halfLocalWorldSize * m_chunkSize) % m_chunkSize);;
}

uint8_t World::GetBlockIDLowRounding(float p_x, float p_y, float p_z)
{
	if (abs(p_x - static_cast<int>(p_x)) <= 0.5f)
	{
		p_x -= (p_x - static_cast<int>(p_x));
	}
	else if (abs(p_x - static_cast<int>(p_x)) > 0.5f)
	{
		p_x = round(p_x);
	}
	if (abs(p_y - static_cast<int>(p_y)) <= 0.5f)
	{
		p_y -= (p_y - static_cast<int>(p_y));
	}
	else if (abs(p_y - static_cast<int>(p_y)) > 0.5f)
	{
		p_y = round(p_y);
	}

	if (abs(p_z - static_cast<int>(p_z)) <= 0.5f)
	{
		p_z -= (p_z - static_cast<int>(p_z));
	}
	else if (abs(p_z - static_cast<int>(p_z)) > 0.5f)
	{
		p_z = round(p_z);
	}


	const float xChunk = (p_x / m_chunkSize) + (m_halfLocalWorldSize) - m_xChunkOffset;
	const float yChunk = (p_y / m_chunkSize);
	const float zChunk = (p_z / m_chunkSize) + (m_halfLocalWorldSize) - m_zChunkOffset;

	if (xChunk < 0 || xChunk >= m_localWorldSize ||
		yChunk < 0 || yChunk >= 16 ||
		zChunk < 0 || zChunk >= m_localWorldSize)
		return 0;

	auto target = m_localWorld[From3Dto1D(static_cast<uint64_t>(xChunk), static_cast<uint64_t>(yChunk),
	                                      static_cast<uint64_t>(zChunk))]->GetComponent<Chunk>();

	return target->GetBlock(static_cast<uint8_t>(p_x + m_halfLocalWorldSize * m_chunkSize) % m_chunkSize,
	                        static_cast<int>(p_y) % m_chunkSize,
	                        static_cast<uint8_t>(p_z + m_halfLocalWorldSize * m_chunkSize) % m_chunkSize);;
}

void World::SetBlock(const glm::vec3& p_coords, uint8_t p_newBlockID, bool p_updateChunk)
{
	SetBlock(p_coords.x, p_coords.y, p_coords.z, p_newBlockID, p_updateChunk);
}

void World::SetBlock(float p_x, float p_y, float p_z, uint8_t p_newBlockID, bool p_updateChunk)
{
	p_x = round(p_x);
	p_y = round(p_y);
	p_z = round(p_z);

	float xChunk = (p_x / m_chunkSize) + (m_halfLocalWorldSize) - m_xChunkOffset;
	float yChunk = (p_y / m_chunkSize);
	float zChunk = (p_z / m_chunkSize) + (m_halfLocalWorldSize) - m_zChunkOffset;

	if (xChunk < 0 || xChunk >= m_localWorldSize ||
		yChunk < 0 || yChunk >= 16 ||
		zChunk < 0 || zChunk >= m_localWorldSize)
		return;

	auto target = m_localWorld[From3Dto1D(static_cast<uint64_t>(xChunk), static_cast<uint64_t>(yChunk),
	                                      static_cast<uint64_t>(zChunk))]->GetComponent<Chunk>();
	target->SetBlock(static_cast<uint8_t>(p_x + m_halfLocalWorldSize * m_chunkSize) % m_chunkSize,
	                 static_cast<int>(p_y) % m_chunkSize,
	                 static_cast<uint8_t>(p_z + m_halfLocalWorldSize * m_chunkSize) % m_chunkSize, p_newBlockID,
	                 p_updateChunk);
}

void World::SetBlock(int64_t p_x, int64_t p_y, int64_t p_z, uint8_t p_newBlockID, bool p_updateChunk,
                     bool p_saveChanged)
{
	float p_x1 = static_cast<float>(p_x);
	float p_y1 = static_cast<float>(p_y);
	float p_z1 = static_cast<float>(p_z);

	float xChunk = (p_x1 / m_chunkSize) + (m_halfLocalWorldSize) - m_xChunkOffset;
	float yChunk = (p_y1 / m_chunkSize);
	float zChunk = (p_z1 / m_chunkSize) + (m_halfLocalWorldSize) - m_zChunkOffset;

	if (xChunk < 0 || xChunk >= m_localWorldSize ||
		yChunk < 0 || yChunk >= 16 ||
		zChunk < 0 || zChunk >= m_localWorldSize)
		return;

	auto target = m_localWorld[From3Dto1D(static_cast<uint64_t>(xChunk), static_cast<uint64_t>(yChunk),
	                                      static_cast<uint64_t>(zChunk))]->GetComponent<Chunk>();
	target->SetBlock(static_cast<uint8_t>(p_x1 + m_halfLocalWorldSize * m_chunkSize) % m_chunkSize,
	                 static_cast<int>(p_y1) % m_chunkSize,
	                 static_cast<uint8_t>(p_z1 + m_halfLocalWorldSize * m_chunkSize) % m_chunkSize, p_newBlockID,
	                 p_updateChunk, p_saveChanged);
}

void World::GenerateWorldAroundPlayer(const glm::vec3& p_playerPosition)
{
	if (IniLoader::GetBool("world.ini", "infinite_world"))
	{
		if (!m_shiftWorldQueue.empty())
		{
			ShiftWorld(m_shiftWorldQueue.front().shiftDirection);
		}
		else
		{
			/* Test if the player leaved a second chunk */
			const glm::vec2 distancePlayer = glm::vec2(p_playerPosition.x, p_playerPosition.z) - glm::vec2(
				m_xChunkOffset * m_chunkSize, m_zChunkOffset * m_chunkSize);

			if (glm::length(distancePlayer) < m_chunkSize * 1.5)
				return;

			/* Shift world */
			SetShiftWorldDirection(glm::normalize(distancePlayer));
		}
	}
}

void World::Save()
{
	m_saveManager.SaveNoiseProperties(m_noiseProperties);
	SaveChunks();
}

GameObject* World::CreateChunk(const glm::vec3& p_position) const
{
	GameObject* chunk = &m_sceneManager.GetCurrentScene().CreateGameObject("Chunk");
	auto chunkComponent = chunk->AddComponent<Chunk>();
	chunkComponent->FillChunk(0);

	chunk->transform->Translate(p_position);

	return chunk;
}

void World::CreateChunks()
{
	glm::vec3 coords;
	for (uint16_t i = 0; i < m_localWorld.capacity(); ++i)
	{
		coords = From1Dto3D(i);

		m_localWorld.push_back(CreateChunk(glm::vec3(
			((coords.x - m_halfLocalWorldSize) * m_chunkSize) + m_halfChunkSize,
			(coords.y * m_chunkSize) + m_halfChunkSize,
			((coords.z - m_halfLocalWorldSize) * m_chunkSize) + m_halfChunkSize)));
	}
}

void World::DestroyChunk(const uint16_t& p_index)
{
	const auto chunk = m_localWorld[p_index]->GetComponent<Chunk>();
	if (chunk->IsChanged())
		m_saveManager.SaveChunk(*chunk);

	m_gameEngine.Unconsider(m_localWorld[p_index]);
	m_sceneManager.GetCurrentScene().DestroyGameObject(m_localWorld[p_index]->GetInstanceID());
	m_localWorld[p_index] = nullptr;
}

void World::SetNeighbour(const glm::vec3& p_vec3)
{
	SetNeighbour(static_cast<uint8_t>(p_vec3.x), static_cast<uint8_t>(p_vec3.y), static_cast<uint8_t>(p_vec3.z));
}

void World::SetNeighbour(const uint8_t& p_x, const uint8_t& p_y, const uint8_t& p_z)
{
	auto chunkComponent = m_localWorld[From3Dto1D(p_x, p_y, p_z)]->GetComponent<Chunk>();

	Chunk* left = p_x != 0 ? m_localWorld[From3Dto1D(p_x - 1, p_y, p_z)]->GetComponent<Chunk>().get() : nullptr;
	Chunk* right = p_x != m_localWorldSize - 1
		               ? m_localWorld[From3Dto1D(p_x + 1, p_y, p_z)]->GetComponent<Chunk>().get()
		               : nullptr;
	Chunk* bot = p_y != 0 ? m_localWorld[From3Dto1D(p_x, p_y - 1, p_z)]->GetComponent<Chunk>().get() : nullptr;
	Chunk* top = p_y != 15 ? m_localWorld[From3Dto1D(p_x, p_y + 1, p_z)]->GetComponent<Chunk>().get() : nullptr;
	Chunk* back = p_z != 0 ? m_localWorld[From3Dto1D(p_x, p_y, p_z - 1)]->GetComponent<Chunk>().get() : nullptr;
	Chunk* front = p_z != m_localWorldSize - 1
		               ? m_localWorld[From3Dto1D(p_x, p_y, p_z + 1)]->GetComponent<Chunk>().get()
		               : nullptr;

	chunkComponent->SetNeighboor(left, right, top, bot, front, back);
}

void World::SetNeighbours()
{
	for (uint16_t i = 0; i < m_localWorld.capacity(); ++i)
		SetNeighbour(From1Dto3D(i));
}

void World::UpdateLocalWorldChunks()
{
	for (GameObject* chunk : m_localWorld)
		chunk->GetComponent<Chunk>()->UpdateChunk();
}

void World::GenerateChunk(const uint8_t& p_xChunk, const uint8_t& p_zChunk, const bool& p_update)
{
	for (uint8_t zBlock = 0; zBlock < m_chunkSize; ++zBlock)
	{
		for (uint8_t xBlock = 0; xBlock < m_chunkSize; ++xBlock)
		{
			int64_t localX = xBlock + p_xChunk * m_chunkSize;
			int64_t localZ = zBlock + p_zChunk * m_chunkSize;

			int64_t worldX = LocalToWorldX(localX);
			int64_t worldZ = LocalToWorldZ(localZ);

			int16_t surfaceHeight = HeightAt(static_cast<int16_t>(worldX), static_cast<int16_t>(worldZ));

			ApplyTerrainLayers(worldX, surfaceHeight, worldZ, p_update);
			ApplyTerrainProperties(worldX, surfaceHeight, worldZ, p_update);

			uint8_t surfaceBlock = GetBlockID(static_cast<float>(worldX), static_cast<float>(surfaceHeight),
			                                  static_cast<float>(worldZ));
			uint8_t onSurfaceBlock = GetBlockID(static_cast<float>(worldX), static_cast<float>(surfaceHeight + 1),
			                                    static_cast<float>(worldZ));

			if (surfaceHeight > 66 && surfaceBlock == 1 && onSurfaceBlock == 0 && abs(m_fastNoise.GetValueFractal(static_cast<FN_DECIMAL>(worldX), static_cast<FN_DECIMAL>(worldZ))) > 0.4f)
				GenerateFlower(worldX, surfaceHeight + 1, worldZ, p_update);
			else if (surfaceHeight > 66 && surfaceBlock == 1 && onSurfaceBlock == 0 && abs(m_fastNoise.GetValueFractal(static_cast<FN_DECIMAL>(worldX), static_cast<FN_DECIMAL>(worldZ))) < 0.01f)
				GenerateTree(worldX, surfaceHeight + 1, worldZ, p_update);
		}
	}

	LoadChunks(p_xChunk, p_zChunk);
}

void World::GenerateTerrain()
{
	ElkTools::Debug::Log::Process("Terrain generation started", ElkTools::Debug::Log::LogLevel::LOG_INFO,
	                              ElkTools::Debug::Log::LogColor::GREEN);

	for (uint16_t z = 0; z < m_localWorldSize; ++z)
	{
		for (uint16_t x = 0; x < m_localWorldSize; ++x)
		{
			GenerateChunk(static_cast<uint8_t>(x), static_cast<uint8_t>(z));
		}
	}

	ElkTools::Debug::Log::Process("Terrain generation completed", ElkTools::Debug::Log::LogLevel::LOG_INFO,
	                              ElkTools::Debug::Log::LogColor::GREEN);
}

void ElkCraft::Terrain::World::GenerateFlower(int64_t p_worldX, int64_t p_worldY, int64_t p_worldZ, bool p_update)
{
	SetBlock(p_worldX, p_worldY, p_worldZ, m_fastNoise.GetValueFractal(static_cast<FN_DECIMAL>(p_worldX), static_cast<FN_DECIMAL>(p_worldY), static_cast<FN_DECIMAL>(p_worldZ)) > 0.2f ? 15 : 16, p_update, false);
}

void ElkCraft::Terrain::World::GenerateTree(int64_t p_worldX, int64_t p_worldY, int64_t p_worldZ, bool p_update)
{
	uint8_t treeHeight = 5 + static_cast<uint8_t>(abs(m_fastNoise.GetValueFractal(static_cast<FN_DECIMAL>(p_worldX), static_cast<FN_DECIMAL>(p_worldY), static_cast<FN_DECIMAL>(p_worldZ))) * 20.f);

	uint8_t previousLeavesWidth = 0;

	for (uint8_t i = 0; i < treeHeight; ++i)
	{
		SetBlock(p_worldX, p_worldY + i, p_worldZ, 6, p_update, false);
	}

	for (uint8_t i = (treeHeight / 2) - 1; i < treeHeight; ++i)
	{
		uint8_t leavesWidth = 0;

		if (previousLeavesWidth == 0)
			leavesWidth = 1;
		else if (previousLeavesWidth == 1)
			leavesWidth = 2;
		else if (previousLeavesWidth == 2)
			leavesWidth = m_fastNoise.GetValueFractal(static_cast<FN_DECIMAL>(p_worldX), static_cast<FN_DECIMAL>(p_worldY), static_cast<FN_DECIMAL>(p_worldZ)) > 0 ? 1 : 3;
		else if (previousLeavesWidth == 3)
			leavesWidth = previousLeavesWidth = 2;

		GenerateLeavesStage(p_worldX, p_worldY + i, p_worldZ, leavesWidth, p_update);

		previousLeavesWidth = leavesWidth;
	}

	SetBlock(p_worldX, p_worldY + treeHeight - 1, p_worldZ, 12, p_update, false);
	SetBlock(p_worldX, p_worldY + treeHeight, p_worldZ, 12, p_update, false);
}

void ElkCraft::Terrain::World::GenerateLeavesStage(int64_t p_worldX, int64_t p_worldY, int64_t p_worldZ,
                                                   uint8_t p_width, bool p_update)
{
	for (uint8_t i = 1; i <= p_width; ++i)
	{
		SetBlock(p_worldX + i, p_worldY, p_worldZ, 12, p_update, false);
		SetBlock(p_worldX - i, p_worldY, p_worldZ, 12, p_update, false);

		SetBlock(p_worldX, p_worldY, p_worldZ + i, 12, p_update, false);
		SetBlock(p_worldX, p_worldY, p_worldZ - i, 12, p_update, false);

		if (p_width > 1)
		{
			SetBlock(p_worldX + i - 1, p_worldY, p_worldZ + 1, 12, p_update, false);
			SetBlock(p_worldX + i - 1, p_worldY, p_worldZ - 1, 12, p_update, false);

			SetBlock(p_worldX + 1, p_worldY, p_worldZ + i - 1, 12, p_update, false);
			SetBlock(p_worldX - 1, p_worldY, p_worldZ + i - 1, 12, p_update, false);
		}
	}
}

void ElkCraft::Terrain::World::ApplyTerrainLayers(int64_t p_worldX, int16_t p_height, int64_t p_worldZ, bool p_update)
{
	int16_t currentHeight = p_height;
	for (auto& terrainLayer : m_terrainLayers)
	{
		uint8_t counter = 0;
		while (counter != terrainLayer.height && currentHeight >= 0)
		{
			uint8_t newBlockID = terrainLayer.blockID;

			if (terrainLayer.blockID == 4)
				newBlockID = m_fastNoise.GetValueFractal(static_cast<FN_DECIMAL>(p_worldX), static_cast<FN_DECIMAL>(p_height), static_cast<FN_DECIMAL>(p_worldZ)) > 0.2f ? 17 : 4;

			SetBlock(p_worldX, currentHeight, p_worldZ, newBlockID, p_update, false);
			         
			++counter;
			--currentHeight;
		}
	}
}

void ElkCraft::Terrain::World::ApplyTerrainProperties(int64_t p_worldX, int64_t p_height, int64_t p_worldZ,
                                                      bool p_update)
{
	int8_t increment;

	for (auto& terrainProperty : m_terrainProperties)
	{
		int16_t currentHeight = static_cast<int16_t>(p_height);
		if (terrainProperty.mode == TerrainProperty::Mode::FILL || terrainProperty.mode == TerrainProperty::Mode::REPLACE)
		{
			increment = terrainProperty.mode == TerrainProperty::Mode::REPLACE ? -1 : 1;

			while (currentHeight < terrainProperty.threshold && currentHeight >= 0)
			{
				SetBlock(p_worldX, currentHeight, p_worldZ, terrainProperty.blockID, p_update, false);
				currentHeight += increment;
			}
		}
		else if (terrainProperty.mode == TerrainProperty::Mode::SURFACE)
		{
			if (currentHeight <= terrainProperty.threshold)
			{
				SetBlock(static_cast<float>(p_worldX), static_cast<float>(currentHeight), static_cast<float>(p_worldZ),
				         terrainProperty.blockID, p_update);
			}
		}
	}
}

uint8_t World::HeightAt(const int16_t& p_x, const int16_t& p_y) const
{
	float noiseHeight = 0;
	float amplitude = 1;
	float frequency = 1;

	//Octaves loop
	for (uint8_t i = 0; i < m_noiseProperties.octaves; ++i)
	{
		float sampleX = ((p_x) / m_noiseProperties.scale * frequency);
		float sampleY = ((p_y) / m_noiseProperties.scale * frequency);

		float perlinValue = static_cast<float>(m_fastNoise.GetPerlinFractal(static_cast<FN_DECIMAL>(sampleX), static_cast<FN_DECIMAL>(sampleY), static_cast<FN_DECIMAL>(0.2)));

		noiseHeight += perlinValue * amplitude;
		amplitude *= m_noiseProperties.persistance;
		frequency *= m_noiseProperties.lacunarity;
	}

	noiseHeight = m_noiseProperties.maxHeight - floor(noiseHeight * m_noiseProperties.maxHeight);

	noiseHeight = std::pow(noiseHeight * m_noiseProperties.heightScale, m_noiseProperties.redistribution);

	return static_cast<uint8_t>(noiseHeight + m_noiseProperties.additionalElevation);
}

void World::SetShiftWorldDirection(const glm::vec2& p_direction)
{
	if (p_direction.x > 0.33f)
	{
		m_shiftWorldQueue.emplace(std::make_pair(0, 1));
	}
	else if (p_direction.x < -0.33f)
	{
		m_shiftWorldQueue.emplace(std::make_pair(0, -1));
	}

	if (p_direction.y > 0.33f)
	{
		m_shiftWorldQueue.emplace(std::make_pair(2, 1));
	}
	else if (p_direction.y < -0.33f)
	{
		m_shiftWorldQueue.emplace(std::make_pair(2, -1));
	}
}

void World::ShiftWorld(const std::pair<uint8_t, int8_t>& p_data)
{
	ShiftWorld(p_data.first, p_data.second);
}

void World::ShiftWorld(const uint8_t& p_axis, const int8_t& p_shiftDirection)
{
	uint8_t indiceChunksToDelete = p_shiftDirection > 0 ? 0 : m_localWorldSize - 1;
	uint8_t indiceChunksToAdd = p_shiftDirection > 0 ? m_localWorldSize - 1 : 0;

	std::vector<GameObject*> buffer;
	glm::ivec3 coords;

	switch (m_shiftWorldQueue.front().shiftState)
	{
	case 0:

		if (p_axis == 0)
			m_xChunkOffset += p_shiftDirection;
		else if (p_axis == 2)
			m_zChunkOffset += p_shiftDirection;

		buffer.reserve(m_localWorld.capacity()); // local world size

		for (uint16_t i = 0; i < m_localWorld.capacity(); ++i)
		{
			coords = From1Dto3D(i);
			if (coords[p_axis] == indiceChunksToDelete)
			{
				DestroyChunk(i);
			}

			if (coords[p_axis] == indiceChunksToAdd)
			{
				auto newChunk = CreateChunk(glm::vec3(
					((coords.x + m_xChunkOffset - m_halfLocalWorldSize) * m_chunkSize) + m_halfChunkSize,
					(coords.y * m_chunkSize) + m_halfChunkSize,
					((coords.z + m_zChunkOffset - m_halfLocalWorldSize) * m_chunkSize) + m_halfChunkSize));

				buffer.emplace_back(newChunk);

				m_gameEngine.Consider(newChunk);
			}
			else
			{
				coords[p_axis] += p_shiftDirection;
				buffer.push_back(std::move(m_localWorld[From3Dto1D(coords.x, coords.y, coords.z)]));
			}
		}
		m_localWorld = std::move(buffer);
		buffer.clear();

		break;
	case 1:

		if (p_axis == 0)
		{
			for (uint8_t zChunk = 0; zChunk < m_localWorldSize; ++zChunk)
			{
				GenerateChunk(indiceChunksToAdd, zChunk);
			}
		}
		else if (p_axis == 2)
		{
			for (uint8_t xChunk = 0; xChunk < m_localWorldSize; ++xChunk)
			{
				GenerateChunk(xChunk, indiceChunksToAdd);
			}
		}

		break;
	default:

		if (p_axis == 0)
		{
			const int yChunk = 17 - m_shiftWorldQueue.front().shiftState;

			for (uint8_t zChunk = 0; zChunk < m_localWorldSize; ++zChunk)
			{
				SetNeighbour(indiceChunksToAdd - p_shiftDirection, yChunk, zChunk);
				SetNeighbour(indiceChunksToAdd, yChunk, zChunk);

				m_localWorld[From3Dto1D(indiceChunksToAdd - p_shiftDirection, yChunk, zChunk)]
					->GetComponent<Chunk>()->UpdateChunk();
				m_localWorld[From3Dto1D(indiceChunksToAdd, yChunk, zChunk)]->GetComponent<Chunk>()->UpdateChunk();
			}
		}
		else if (p_axis == 2)
		{
			const int yChunk = 17 - m_shiftWorldQueue.front().shiftState;

			for (uint8_t xChunk = 0; xChunk < m_localWorldSize; ++xChunk)
			{
				SetNeighbour(xChunk, yChunk, indiceChunksToAdd - p_shiftDirection);
				SetNeighbour(xChunk, yChunk, indiceChunksToAdd);

				m_localWorld[From3Dto1D(xChunk, yChunk, indiceChunksToAdd - p_shiftDirection)]
					->GetComponent<Chunk>()->UpdateChunk();
				m_localWorld[From3Dto1D(xChunk, yChunk, indiceChunksToAdd)]->GetComponent<Chunk>()->UpdateChunk();
			}
		}

		break;
	}

	++m_shiftWorldQueue.front().shiftState;

	if (m_shiftWorldQueue.front().shiftState == 18)
		m_shiftWorldQueue.pop();
}

void World::LoadChunks(const uint16_t& p_xChunk, const uint16_t& p_zChunk)
{
	for (uint8_t y = 0; y < 16; ++y)
	{
		const auto chunkData = m_saveManager.GetChunkData(glm::ivec3(LocalToWorldX(p_xChunk * m_chunkSize + m_halfChunkSize),
		                                                            y * m_chunkSize + m_halfChunkSize,
		                                                            LocalToWorldX(p_zChunk * m_chunkSize + m_halfChunkSize)));

		if (chunkData)
		{
			auto chunk = m_localWorld[From3Dto1D(p_xChunk, y, p_zChunk)]->GetComponent<Chunk>();

			for (uint16_t i = 0; i < 4096; ++i)
			{
				const glm::uvec3 coords = From1Dto3DChunks(i);
				chunk->SetBlock(coords.x, coords.y, coords.z, chunkData->blocks[i], false, false);
			}

			chunk->SetIsChanged(true);
		}
	}
}

void World::SaveChunks()
{
	for (GameObject* gameObject : m_localWorld)
	{
		auto chunk = gameObject->GetComponent<Chunk>();
		if (chunk->IsChanged())
		{
			m_saveManager.SaveChunk(*chunk);
		}
	}
}
