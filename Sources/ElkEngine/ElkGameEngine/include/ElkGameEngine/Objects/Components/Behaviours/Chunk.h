#pragma once

#include <ElkRendering/Resources/RenderChunk.h>
#include <ElkTools/Utils/IniLoader.h>
#include <ElkTools/Utils/Profiler.h>
#include <ElkTools/Utils/BlocksInfo.h>

#include "ElkGameEngine/export.h"
#include "ElkGameEngine/Objects/Components/Behaviours/ABehaviour.h"

namespace ElkGameEngine
{
	namespace Objects
	{
		namespace Components
		{
			namespace Behaviours
			{
				class ELK_GAMEENGINE_EXPORT Chunk : public ABehaviour
				{
				public:
					struct Neighbours
					{
						Chunk* left = nullptr;
						Chunk* right = nullptr;
						Chunk* bot = nullptr;
						Chunk* top = nullptr;
						Chunk* back = nullptr;
						Chunk* front = nullptr;
					};

					struct BlockInfo
					{
						uint8_t	type = 0;
						uint8_t	info = 0 | 1 << 4; // Light level (4 bits) | to render (1 bit)

						void SetLightLevel(const uint8_t& p_lightLevel)
						{
							info = p_lightLevel | GetToRender() << 4;
						}

						uint8_t GetLightLevel()
						{
							return uint8_t(info % 16);
						}

						void SetToRender(const bool& p_value)
						{
							info = GetLightLevel() | p_value << 4;
						}

						bool GetToRender()
						{
							return bool((info >> 4) % 2);
						}

						bool operator=(uint8_t p_value) { return type == p_value; }
						int operator+(BlockInfo& p_other) { return type + p_other.type; }
					};

					enum class Side
					{
						LEFT,
						RIGHT,
						BOT,
						TOP,
						BACK,
						FRONT
					};

					Chunk();
					virtual ~Chunk() = default;

					ElkRendering::Resources::RenderChunk& GetRenderChunk();

					uint16_t From3Dto1D(const uint8_t& p_x, const uint8_t& p_y, const uint8_t& p_z);
					std::tuple<uint8_t, uint8_t, uint8_t> From1Dto3D(uint16_t p_index);

					uint8_t GetBlock(uint8_t p_x, uint8_t p_y, uint8_t p_z);
					void SetBlock(uint8_t p_x, uint8_t p_y, uint8_t p_z, uint8_t p_value, bool p_update = false, bool p_saveChange = true);
					bool IsEmpty() const;
					bool IsChanged() const;

					void SetNeighboor(Chunk* p_left, Chunk* p_right, Chunk* p_top, Chunk* p_bot, Chunk* p_front, Chunk* p_back);
					void SetIsChanged(bool p_changed);

					/* Chunk occlusion */
					bool IsOccluded();
					bool IsOnSurface();
					bool IsOccluder(Side p_side);

					/* Block occlusion */
					bool BlockIsOccluded(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z);
					bool BlockIsSurface(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z);
					bool BlockIsOccluder(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z);
					bool BlockIsStacked(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z);

					void BlockPropagateLight(uint8_t p_x, uint8_t p_y, uint8_t p_z, uint8_t p_lightLevel);

					bool IsHigherLayer(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z);

					BlockInfo* GetBlock(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z, Side p_side, uint8_t p_shifting = 1);

					void FillChunk(uint8_t p_blockType);
					void UpdateChunk();
					void UpdateBlocks();
					void UpdateNeighbours();

					Neighbours m_neighbours;
					BlockInfo m_blocks[4096];

				private:
					ElkRendering::Resources::RenderChunk m_renderChunk;
					int16_t m_airCounter = 0;
					bool m_isChanged;

					static bool __CHUNK_SURFACE_CULLING;
					static bool __BLOCK_SURFACE_CULLING;
				};
			}
		}
	}
}
