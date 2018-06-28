#pragma once

#include <random>
#include <type_traits> //for std::underlying_type
#include <queue>


#include <ElkGameEngine/ElkGameEngine.h>
#include <ElkTools/Utils/BlocksInfo.h>

#include "ElkCraft/Terrain/FastNoise.h"
#include "ElkCraft/Terrain/Perlin.h"
#include "ElkCraft/Terrain/WorldProperties.h"
#include "ElkCraft/System/SaveManager.h"

namespace ElkCraft::Terrain
{
	struct SetShiftWorldDirection
	{
		std::pair<uint8_t, int8_t> shiftDirection;
		uint8_t shiftState;

		explicit SetShiftWorldDirection(std::pair<uint8_t, int8_t> p_shiftDirection, const uint8_t& p_shiftState = 0)
			: shiftDirection(std::move(p_shiftDirection)), shiftState(p_shiftState)
		{
		}
	};

	class World
	{
	public:
		World(ElkGameEngine::Managers::EngineManager& p_gameEngine, System::SaveManager& p_saveManager);
		~World();

		void InitNoise();
		void InitWorldProperties();
		void Populate();
		void Clear();

		uint8_t GetBlockID(const glm::vec3& p_coords);
		uint8_t GetBlockID(float p_x, float p_y, float p_z);
		uint8_t GetBlockIDLowRounding(float p_x, float p_y, float p_z);
		void SetBlock(const glm::vec3& p_coords, uint8_t p_newBlockID, bool p_updateChunk);
		void SetBlock(float p_x, float p_y, float p_z, uint8_t p_newBlockID, bool p_updateChunk);
		void SetBlock(int64_t p_x, int64_t p_y, int64_t p_z, uint8_t p_newBlockID, bool p_updateChunk, bool p_saveChanged);

		void GenerateWorldAroundPlayer(const glm::vec3& p_playerPosition);

		void Save();

	private:
		/* Array tools */
		uint64_t From2Dto1D(const uint64_t& p_x, const uint64_t& p_y) const;
		glm::vec2 From1Dto2D(const uint64_t& p_index) const;
		uint64_t From3Dto1D(const uint64_t& p_x, const uint64_t& p_y, const uint64_t& p_z) const;
		glm::vec3 From1Dto3D(uint64_t p_index) const;
		glm::uvec3 From1Dto3DChunks(uint16_t p_index);

		/* Local world manipulation tools */
		int64_t LocalToWorldX(int64_t p_localX);
		int64_t LocalToWorldZ(int64_t p_localZ);

		/* Local world generation */
		ElkGameEngine::Objects::GameObject* CreateChunk(const glm::vec3& p_position) const;
		void CreateChunks();
		void DestroyChunk(const uint16_t& p_index);
		void SetNeighbour(const glm::vec3& p_vec3);
		void SetNeighbour(const uint8_t& p_x, const uint8_t& p_y, const uint8_t& p_z);
		void SetNeighbours();
		void UpdateLocalWorldChunks();
		void GenerateChunk(const uint8_t& p_xChunk, const uint8_t& p_zChunk, const bool& p_update = false);
		void GenerateTerrain();
		void GenerateFlower(int64_t p_worldX, int64_t p_worldY, int64_t p_worldZ, bool p_update = false);
		void GenerateTree(int64_t p_worldX, int64_t p_worldY, int64_t p_worldZ, bool p_update = false);
		void GenerateLeavesStage(int64_t p_worldX, int64_t p_worldY, int64_t p_worldZ, uint8_t p_width, bool p_update = false);
		void ApplyTerrainLayers(int64_t p_worldX, int16_t p_height, int64_t p_worldZ, bool p_update = false);
		void ApplyTerrainProperties(int64_t p_worldX, int64_t p_height, int64_t p_worldZ, bool p_update = false);

		/* Infinite world generation*/
		void SetShiftWorldDirection(const glm::vec2& p_direction);
		void ShiftWorld(const std::pair<uint8_t, int8_t>& p_data);
		void ShiftWorld(const uint8_t& p_axis, const int8_t& p_shiftDirection);

		/* World Save */
		void LoadChunks(const uint16_t& p_xChunk,const uint16_t& p_zChunk);
		void SaveChunks();

	private:
		/* Managers */
		ElkGameEngine::Managers::EngineManager&		m_gameEngine;
		ElkGameEngine::Managers::SceneManager&		m_sceneManager;
		ElkCraft::System::SaveManager&				m_saveManager;		

		/* World Parameters */
		uint8_t m_localWorldSize;
		float m_halfLocalWorldSize;
		uint8_t m_chunkSize;
		uint8_t m_halfChunkSize;
		int64_t m_xChunkOffset;
		int64_t m_zChunkOffset;

		FastNoise m_fastNoise;

		/* Perlin noise parameters */
		std::unique_ptr<PerlinNoise> m_perlin;
		NoiseProperties m_noiseProperties;

		uint8_t HeightAt(const int16_t& p_x, const int16_t& p_y) const;

		/* World properties */
		std::vector<TerrainLayer> m_terrainLayers;
		std::vector<TerrainProperty> m_terrainProperties;

		std::vector<ElkGameEngine::Objects::GameObject*> m_localWorld;
		std::queue<ElkCraft::Terrain::SetShiftWorldDirection> m_shiftWorldQueue;
	};
}
