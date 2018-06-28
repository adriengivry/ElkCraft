#pragma once

#include <ElkGameEngine/ElkGameEngine.h>
#include <ElkTools/Utils/BlocksInfo.h>

namespace ElkCraft::Utils
{
	class ItemsInfo
	{
	public:
		enum class Quality
		{
			NONE,
			WOOD,
			STONE
		};

		enum class ToolType
		{
			NONE,
			AXE,
			PICKAXE,
			SHOVEL
		};

		enum class ItemType : uint8_t
		{
			STICK			= 100,
			WOOD_AXE		= 101,
			WOOD_PICKAXE	= 102,
			WOOD_SHOVEL		= 103,
			STONE_AXE		= 104,
			STONE_PICKAXE	= 105,
			STONE_SHOVEL	= 106,
			APPLE			= 107

		};

		static bool IsItem(uint8_t p_type);
		static bool IsTool(uint8_t p_type);
		static Quality GetQuality(uint8_t p_type);
		static ToolType GetToolType(uint8_t p_type);
		static ItemType GetItemType(uint8_t p_type);
		static bool IsEfficientOn(uint8_t p_tool, uint8_t p_block);
	};
}