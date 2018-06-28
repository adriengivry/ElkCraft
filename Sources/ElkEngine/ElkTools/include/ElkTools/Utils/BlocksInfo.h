#pragma once

#include <inttypes.h>

#include "ElkTools/export.h"

namespace ElkTools
{
	namespace Utils
	{
		class ELK_TOOLS_EXPORT BlocksInfo
		{
		public:
			enum class Category
			{
				GRASS,
				GRAVEL,
				SAND,
				GLASS,
				STONE,
				WOOD
			};

			enum class BlockType : uint8_t
			{
				EMPTY			= 0,
				GRASS			= 1,
				DIRT			= 2,
				GRAVEL			= 3,
				STONE			= 4,
				COBBLESTONE		= 5,
				WOOD			= 6,
				WOODEN_PLANKS	= 7,
				SAND			= 8,
				GLASS			= 9,
				BRICK			= 10,
				STONE_BRICK		= 11,
				LEAVES			= 12,
				WATER			= 13,
				LAVA			= 14,
				RED_FLOWER		= 15,
				YELLOW_FLOWER	= 16,
				COAL_ORE		= 17
			};

			static Category GetBlockCategory(uint8_t p_type);
			static float GetBlockBreakDuration(uint8_t p_blockType);
			static uint8_t GetBreakResult(uint8_t p_blockType);
			static bool IsOccluder(uint8_t p_blockType);
			static bool IsStackable(uint8_t p_blockType);
			static bool IsSemiTransparent(uint8_t p_blockType);
			static bool IsIgnoringRaycast(uint8_t p_blockType);
			static bool IsLiquid(uint8_t p_blockType);
			static bool IsFlower(uint8_t p_blockType);
			static bool IsCollidable(uint8_t p_blockType);
			static bool IsFertileSoil(uint8_t p_blockType);
			static bool IsOre(uint8_t p_blockType);
		};
	}
}
