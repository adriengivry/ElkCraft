#include "stdafx.h"

#include "ElkCraft/Utils/ItemsInfo.h"

using namespace ElkCraft::Utils;
using namespace ElkTools::Utils;

bool ItemsInfo::IsItem(uint8_t p_type)
{
	return p_type >= 100;
}

bool ElkCraft::Utils::ItemsInfo::IsTool(uint8_t p_type)
{
	return p_type >= 101 && p_type <= 106;
}

ItemsInfo::Quality ElkCraft::Utils::ItemsInfo::GetQuality(uint8_t p_type)
{
	switch (GetItemType(p_type))
	{
	case ItemType::WOOD_AXE:
	case ItemType::WOOD_PICKAXE:
	case ItemType::WOOD_SHOVEL:
		return Quality::WOOD;

	case ItemType::STONE_AXE:
	case ItemType::STONE_PICKAXE:
	case ItemType::STONE_SHOVEL:
		return Quality::STONE;

	default:
		return Quality::NONE;
	}
}

ItemsInfo::ToolType ElkCraft::Utils::ItemsInfo::GetToolType(uint8_t p_type)
{
	switch (GetItemType(p_type))
	{
	case ItemType::WOOD_AXE:
	case ItemType::STONE_AXE:
		return ToolType::AXE;

	case ItemType::WOOD_PICKAXE:
	case ItemType::STONE_PICKAXE:
		return ToolType::PICKAXE;

	case ItemType::WOOD_SHOVEL:
	case ItemType::STONE_SHOVEL:
		return ToolType::SHOVEL;

	default:
		return ToolType::NONE;
	}
}

ItemsInfo::ItemType ElkCraft::Utils::ItemsInfo::GetItemType(uint8_t p_type)
{
	return static_cast<ItemType>(p_type);
}

bool ElkCraft::Utils::ItemsInfo::IsEfficientOn(uint8_t p_tool, uint8_t p_block)
{
	ToolType neededTool;

	switch (static_cast<BlocksInfo::BlockType>(p_block))
	{
	case BlocksInfo::BlockType::GRASS:
		neededTool = ToolType::SHOVEL;
		break;

	case BlocksInfo::BlockType::DIRT:
		neededTool = ToolType::SHOVEL;
		break;

	case BlocksInfo::BlockType::GRAVEL:
		neededTool = ToolType::SHOVEL;
		break;

	case BlocksInfo::BlockType::STONE:
		neededTool = ToolType::PICKAXE;
		break;

	case BlocksInfo::BlockType::COBBLESTONE:
		neededTool = ToolType::PICKAXE;
		break;

	case BlocksInfo::BlockType::WOOD:
		neededTool = ToolType::AXE;
		break;

	case BlocksInfo::BlockType::WOODEN_PLANKS:
		neededTool = ToolType::AXE;
		break;

	case BlocksInfo::BlockType::SAND:
		neededTool = ToolType::SHOVEL;
		break;

	case BlocksInfo::BlockType::BRICK:
		neededTool = ToolType::PICKAXE;
		break;

	case BlocksInfo::BlockType::STONE_BRICK:
		neededTool = ToolType::PICKAXE;
		break;

	case BlocksInfo::BlockType::COAL_ORE:
		neededTool = ToolType::PICKAXE;
		break;

	default:
		return false;
	}

	return (GetToolType(p_tool) == neededTool);
}
