#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include <ElkGameEngine/ElkGameEngine.h>

#include "ElkCraft/Terrain/WorldProperties.h"

using namespace ElkGameEngine::Objects::Components::Behaviours;

namespace ElkCraft::System
{
	class SaveManager
	{
	private:
		struct Vec3Hash
		{
			uint64_t operator()(const glm::ivec3& v) const
			{
				const uint64_t* data = reinterpret_cast<const uint64_t*>(&v);
				uint64_t result = 0;
				constexpr size_t size = sizeof(v) / sizeof(uint64_t);
				for (size_t i = 0; i < size; ++i)
				{
					result ^= data[i] << i;
				}
				return result;
			}
		};

		struct ChunkData
		{
			uint8_t blocks[4096]{};

			ChunkData() = default;

			explicit ChunkData(Chunk& p_chunk)
			{
				for (uint16_t i = 0; i < 4096; ++i)
				{
					const glm::uvec3 coords = From1Dto3D(i);
					blocks[i] = p_chunk.GetBlock(coords.x, coords.y, coords.z);
				}
			}
		};

		struct PlayerData
		{
			glm::vec3 position;
			glm::vec3 forward;
			uint8_t itemsIDAndAmount[20]{};
			uint8_t health{20};
			uint8_t energy{20};
			uint16_t daytime;
			uint32_t days;
		};

		std::ifstream m_readFile;
		std::ofstream m_writeFile;
		std::string m_savePath;
		std::string m_saveFile;
		std::string m_extention;

		uint8_t m_numberOfBlocks;
		std::vector<uint32_t> m_blocksSizes;

		ElkCraft::Terrain::NoiseProperties m_noiseProperties;
		PlayerData m_playerData;
		std::unordered_map<glm::ivec3, ChunkData, Vec3Hash> m_savedChunks;

		bool m_isLoaded;

	private:
		void SetCursorRead(const uint16_t& p_offsetFromBeg);
		void SetCursorWrite(const uint16_t& p_offsetFromBeg);
		static glm::uvec3 From1Dto3D(uint64_t p_index);

		/* Parsing save file */
		void Parse();
		void ParseHeader();
		void ParseNoiseProperties();
		void ParsePlayer();
		void ParseChunks();


		/* Serialize save file */
		void Serialize();
		void CalculateAndWriteBlocksSizes();
		void SerializeHeader();
		void SerializeNoiseProperties();
		void SerializePlayer();
		void SerializeChunks();

	public:
		SaveManager(const bool& p_newSave);
		~SaveManager();

		bool SaveFileLoaded() const;

		void SaveToFile() { Serialize(); }

		void SaveNoiseProperties(const ElkCraft::Terrain::NoiseProperties& p_noiseProperties);
		void SavePlayer(ElkGameEngine::Objects::GameObject& p_player, uint8_t p_days, uint16_t p_daytime);
		void SaveChunk(Chunk& p_chunk);

		const ElkCraft::Terrain::NoiseProperties& GetNoiseProperties() const;
		const PlayerData& GetPlayerData() const;
		std::shared_ptr<ChunkData> GetChunkData(const glm::ivec3& p_coords) const;

		bool IsLoaded() const;

		template <typename T>
		void WriteData(const T* p_dataToWrite)
		{
			m_writeFile.write((char*)p_dataToWrite, sizeof(T));
		}

		template <typename T>
		void WriteData(const T p_dataToWrite)
		{
			m_writeFile.write((char*)&p_dataToWrite, sizeof(T));
		}

		template <typename T>
		void ReadData(const T* p_toGet)
		{
			m_readFile.read((char*)p_toGet, sizeof(T));
		}

		template <typename T>
		T ReadData()
		{
			T toGet;
			m_readFile.read((char*)&toGet, sizeof(T));
			return toGet;
		}

	};
}
