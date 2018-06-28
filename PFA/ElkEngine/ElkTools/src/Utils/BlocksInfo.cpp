#include "stdafx.h"

#include "ElkTools/Utils/BlocksInfo.h"

using namespace ElkTools::Utils;

BlocksInfo::Category BlocksInfo::GetBlockCategory(uint8_t p_type)
{
	switch (p_type)
	{
	default:
	case 1: case 12: case 15: case 16:			return Category::GRASS;
	case 4: case 5: case 10: case 11: case 17:	return Category::STONE;
	case 2: case 3:								return Category::GRAVEL;
	case 6: case 7:								return Category::WOOD;
	case 8:										return Category::SAND;
	case 9:										return Category::GLASS;
	}
}

float BlocksInfo::GetBlockBreakDuration(uint8_t p_blockType)
{
	switch (static_cast<BlockType>(p_blockType))
	{
	case BlockType::GRASS:			return 0.9f;
	case BlockType::DIRT:			return 0.75f;
	case BlockType::GRAVEL:			return 0.9f;
	case BlockType::STONE:			return 10.f;
	case BlockType::COBBLESTONE:	return 10.f;
	case BlockType::WOOD:			return 3.f;
	case BlockType::WOODEN_PLANKS:	return 3.f;
	case BlockType::SAND:			return 0.75f;
	case BlockType::GLASS:			return 0.45f;
	case BlockType::BRICK:			return 10.f;
	case BlockType::STONE_BRICK:	return 10.f;
	case BlockType::LEAVES:			return 0.35f;
	case BlockType::RED_FLOWER:		return 0.0f;
	case BlockType::YELLOW_FLOWER:	return 0.0f;
	case BlockType::COAL_ORE:		return 15.0f;
	default: return 0.f;
	}
}

uint8_t BlocksInfo::GetBreakResult(uint8_t p_blockType)
{
	BlockType blockType = static_cast<BlockType>(p_blockType);

	switch (blockType)
	{
	case BlockType::GRASS: return static_cast<uint8_t>(BlockType::DIRT);
	case BlockType::STONE: return static_cast<uint8_t>(BlockType::COBBLESTONE);
	case BlockType::GLASS: return static_cast<uint8_t>(BlockType::EMPTY);
	case BlockType::LEAVES: return static_cast<uint8_t>(BlockType::EMPTY);
	case BlockType::COAL_ORE: return 108; // Drop coal

	default: return p_blockType;
	}
}

bool ElkTools::Utils::BlocksInfo::IsOccluder(uint8_t p_blockType)
{
	BlockType blockType = static_cast<BlockType>(p_blockType);

	return blockType != BlockType::EMPTY 
		&& blockType != BlockType::GLASS
		&& blockType != BlockType::LEAVES
		&& blockType != BlockType::WATER
		&& blockType != BlockType::LAVA
		&& blockType != BlockType::RED_FLOWER
		&& blockType != BlockType::YELLOW_FLOWER;
}

bool ElkTools::Utils::BlocksInfo::IsStackable(uint8_t p_blockType)
{
	BlockType blockType = static_cast<BlockType>(p_blockType);

	return blockType != BlockType::WATER;
}

bool ElkTools::Utils::BlocksInfo::IsSemiTransparent(uint8_t p_blockType)
{
	BlockType blockType = static_cast<BlockType>(p_blockType);

	return blockType == BlockType::WATER;
}

bool ElkTools::Utils::BlocksInfo::IsIgnoringRaycast(uint8_t p_blockType)
{
	BlockType blockType = static_cast<BlockType>(p_blockType);

	return blockType == BlockType::EMPTY
		|| IsLiquid(p_blockType);
}

bool ElkTools::Utils::BlocksInfo::IsLiquid(uint8_t p_blockType)
{
	BlockType blockType = static_cast<BlockType>(p_blockType);

	return blockType == BlockType::WATER
		|| blockType == BlockType::LAVA;
}

bool ElkTools::Utils::BlocksInfo::IsFlower(uint8_t p_blockType)
{
	BlockType blockType = static_cast<BlockType>(p_blockType);

	return blockType == BlockType::RED_FLOWER
		|| blockType == BlockType::YELLOW_FLOWER;
}

bool ElkTools::Utils::BlocksInfo::IsCollidable(uint8_t p_blockType)
{
	BlockType blockType = static_cast<BlockType>(p_blockType);

	return blockType != BlockType::EMPTY
		&& !IsFlower(p_blockType)
		&& !IsLiquid(p_blockType);
}

bool ElkTools::Utils::BlocksInfo::IsFertileSoil(uint8_t p_blockType)
{
	BlockType blockType = static_cast<BlockType>(p_blockType);

	return blockType == BlockType::GRASS || blockType == BlockType::DIRT;
}

bool ElkTools::Utils::BlocksInfo::IsOre(uint8_t p_blockType)
{
	return (static_cast<BlocksInfo::BlockType>(p_blockType) == BlocksInfo::BlockType::COAL_ORE);
}
