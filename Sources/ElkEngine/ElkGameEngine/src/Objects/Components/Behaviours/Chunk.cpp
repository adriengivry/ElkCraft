#include "stdafx.h"

#include "ElkGameEngine/Objects/Components/Behaviours/Chunk.h"

using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkTools::Utils;

bool Chunk::__CHUNK_SURFACE_CULLING = ElkTools::Utils::IniLoader::GetBool("rendering.ini", "chunk_surface_culling");
bool Chunk::__BLOCK_SURFACE_CULLING = ElkTools::Utils::IniLoader::GetBool("rendering.ini", "block_surface_culling");

Chunk::Chunk()
{
	SetName("Chunk");

	m_isChanged = false;
}

ElkRendering::Resources::RenderChunk& ElkGameEngine::Objects::Components::Behaviours::Chunk::GetRenderChunk()
{
	return m_renderChunk;
}

uint16_t ElkGameEngine::Objects::Components::Behaviours::Chunk::From3Dto1D(const uint8_t & p_x, const uint8_t & p_y, const uint8_t & p_z)
{
	return p_x + p_y * 16 + p_z * 16 * 16;
}

std::tuple<uint8_t, uint8_t, uint8_t> ElkGameEngine::Objects::Components::Behaviours::Chunk::From1Dto3D(uint16_t p_index)
{
	std::tuple<uint8_t, uint8_t, uint8_t> result;
	std::get<2>(result) = p_index / 256;
	p_index -= (std::get<2>(result) * 256);
	std::get<1>(result) = p_index / 16;
	std::get<0>(result) = p_index % 16;
	return result;
}

uint8_t Chunk::GetBlock(uint8_t p_x, uint8_t p_y, uint8_t p_z)
{
	return m_blocks[From3Dto1D(p_x, p_y, p_z)].type;
}

void ElkGameEngine::Objects::Components::Behaviours::Chunk::SetBlock(uint8_t p_x, uint8_t p_y, uint8_t p_z,
                                                                     uint8_t p_value, bool p_update, bool p_saveChange)
{
	if (p_saveChange)
	{
		m_isChanged = true;
	}

	if (m_blocks[From3Dto1D(p_x, p_y, p_z)].type == 0)
		--m_airCounter;

	if (p_value == 0)
		++m_airCounter;

	m_blocks[From3Dto1D(p_x, p_y, p_z)].type = p_value;

	if (p_update)
	{
		if (p_x == 0 && m_neighbours.left) m_neighbours.left->UpdateBlocks();
		if (p_x == 15 && m_neighbours.right) m_neighbours.right->UpdateBlocks();
		if (p_y == 0 && m_neighbours.bot) m_neighbours.bot->UpdateBlocks();
		if (p_y == 15 && m_neighbours.top) m_neighbours.top->UpdateBlocks();
		if (p_z == 0 && m_neighbours.back) m_neighbours.back->UpdateBlocks();
		if (p_z == 15 && m_neighbours.front) m_neighbours.front->UpdateBlocks();
		UpdateBlocks();
	}
}

bool ElkGameEngine::Objects::Components::Behaviours::Chunk::IsEmpty() const
{
	return m_airCounter == 4096;
}

bool Chunk::IsChanged() const
{
	return m_isChanged;
}

void ElkGameEngine::Objects::Components::Behaviours::Chunk::SetNeighboor(
	Chunk* p_left, Chunk* p_right, Chunk* p_top, Chunk* p_bot, Chunk* p_front, Chunk* p_back)
{
	m_neighbours.left = p_left;
	m_neighbours.right = p_right;
	m_neighbours.top = p_top;
	m_neighbours.bot = p_bot;
	m_neighbours.front = p_front;
	m_neighbours.back = p_back;
}

void Chunk::SetIsChanged(bool p_changed)
{
	m_isChanged = p_changed;
}

bool ElkGameEngine::Objects::Components::Behaviours::Chunk::IsOccluded()
{
	if (!__CHUNK_SURFACE_CULLING)
		return false;

	if (IsOnSurface())
		return false;

	return (m_neighbours.left->IsOccluder(Side::RIGHT) &&
		m_neighbours.right->IsOccluder(Side::LEFT) &&
		m_neighbours.bot->IsOccluder(Side::TOP) &&
		m_neighbours.top->IsOccluder(Side::BOT) &&
		m_neighbours.back->IsOccluder(Side::FRONT) &&
		m_neighbours.front->IsOccluder(Side::BACK));
}

bool ElkGameEngine::Objects::Components::Behaviours::Chunk::IsOnSurface()
{
	return !m_neighbours.left || !m_neighbours.right || !m_neighbours.bot || !m_neighbours.top || !m_neighbours.back || !
		m_neighbours.front;
}

bool ElkGameEngine::Objects::Components::Behaviours::Chunk::IsOccluder(Side p_side)
{
	switch (p_side)
	{
	case Chunk::Side::LEFT:
		for (uint8_t y = 0; y <= 15; ++y)
			for (uint8_t z = 0; z <= 15; ++z)
				if (!ElkTools::Utils::BlocksInfo::IsOccluder(m_blocks[From3Dto1D(0, y, z)].type))
					return false;
		break;

	case Chunk::Side::RIGHT:
		for (uint8_t y = 0; y <= 15; ++y)
			for (uint8_t z = 0; z <= 15; ++z)
				if (!ElkTools::Utils::BlocksInfo::IsOccluder(m_blocks[From3Dto1D(15, y, z)].type))
					return false;
		break;

	case Chunk::Side::BOT:
		for (uint8_t x = 0; x <= 15; ++x)
			for (uint8_t z = 0; z <= 15; ++z)
				if (!ElkTools::Utils::BlocksInfo::IsOccluder(m_blocks[From3Dto1D(x, 0, z)].type))
					return false;
		break;

	case Chunk::Side::TOP:
		for (uint8_t x = 0; x <= 15; ++x)
			for (uint8_t z = 0; z <= 15; ++z)
				if (!ElkTools::Utils::BlocksInfo::IsOccluder(m_blocks[From3Dto1D(x, 15, z)].type))
					return false;
		break;

	case Chunk::Side::BACK:
		for (uint8_t x = 0; x <= 15; ++x)
			for (uint8_t y = 0; y <= 15; ++y)
				if (!ElkTools::Utils::BlocksInfo::IsOccluder(m_blocks[From3Dto1D(x, y, 0)].type))
					return false;
		break;

	case Chunk::Side::FRONT:
		for (uint8_t x = 0; x <= 15; ++x)
			for (uint8_t y = 0; y <= 15; ++y)
				if (!ElkTools::Utils::BlocksInfo::IsOccluder(m_blocks[From3Dto1D(x, y, 15)].type))
					return false;
		break;
	}

	return true;
}

bool ElkGameEngine::Objects::Components::Behaviours::Chunk::BlockIsOccluded(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z)
{
	if (!__BLOCK_SURFACE_CULLING)
		return false;

	return (!ElkTools::Utils::BlocksInfo::IsStackable(m_blocks[From3Dto1D(p_x, p_y, p_z)].type) && BlockIsStacked(p_x, p_y, p_z)) || !BlockIsSurface(p_x, p_y, p_z);
}

bool ElkGameEngine::Objects::Components::Behaviours::Chunk::BlockIsSurface(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z)
{
	BlockInfo* leftBlock = GetBlock(p_x, p_y, p_z, Side::LEFT);
	bool left = !leftBlock || ElkTools::Utils::BlocksInfo::IsOccluder(leftBlock->type);
	if (!left)
		return true;

	BlockInfo* rightBlock = GetBlock(p_x, p_y, p_z, Side::RIGHT);
	bool right = !rightBlock || ElkTools::Utils::BlocksInfo::IsOccluder(rightBlock->type);
	if (!right)
		return true;

	BlockInfo* botBlock = GetBlock(p_x, p_y, p_z, Side::BOT);
	bool bot = !botBlock || ElkTools::Utils::BlocksInfo::IsOccluder(botBlock->type);
	if (!bot)
		return true;

	BlockInfo* topBlock = GetBlock(p_x, p_y, p_z, Side::TOP);
	bool top = !topBlock || ElkTools::Utils::BlocksInfo::IsOccluder(topBlock->type);
	if (!top)
		return true;

	BlockInfo* backBlock = GetBlock(p_x, p_y, p_z, Side::BACK);
	bool back = !backBlock || ElkTools::Utils::BlocksInfo::IsOccluder(backBlock->type);
	if (!back)
		return true;

	BlockInfo* frontBlock = GetBlock(p_x, p_y, p_z, Side::FRONT);
	bool front = !frontBlock || ElkTools::Utils::BlocksInfo::IsOccluder(frontBlock->type);
	if (!front)
		return true;

	return false;
}

bool ElkGameEngine::Objects::Components::Behaviours::Chunk::BlockIsOccluder(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z)
{
	return ElkTools::Utils::BlocksInfo::IsOccluder(m_blocks[From3Dto1D(p_x, p_y, p_z)].type);
}

bool ElkGameEngine::Objects::Components::Behaviours::Chunk::BlockIsStacked(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z)
{
	BlockInfo* top = GetBlock(p_x, p_y, p_z, Side::TOP);
	return (top && top->type == m_blocks[From3Dto1D(p_x, p_y, p_z)].type);
}

void ElkGameEngine::Objects::Components::Behaviours::Chunk::BlockPropagateLight(
	uint8_t p_x, uint8_t p_y, uint8_t p_z, uint8_t p_lightLevel)
{
	Side sidesToPropagate[4] =
	{
		Side::LEFT,
		Side::RIGHT,
		Side::BACK,
		Side::FRONT,
	};

	for (uint8_t sideIndex = 0; sideIndex < 4; ++sideIndex)
	{
		for (uint8_t i = 1; i <= 5; ++i)
		{
			BlockInfo* block = GetBlock(p_x, p_y, p_z, sidesToPropagate[sideIndex], i);
			if (block)
				block->SetLightLevel(static_cast<uint8_t>(std::max(p_lightLevel - i, static_cast<int>(block->GetLightLevel()))));
		}
	}
}

bool ElkGameEngine::Objects::Components::Behaviours::Chunk::IsHigherLayer(uint8_t& p_x, uint8_t& p_y, uint8_t& p_z)
{
	Chunk* currentChunk = this;
	uint8_t currentY = p_y;

	while (currentChunk)
	{
		if (currentY != p_y)
		{
			if (currentChunk->BlockIsOccluder(p_x, currentY, p_z))
				return false;
		}

		if (currentY == 15)
		{
			currentY = 0;
			currentChunk = currentChunk->m_neighbours.top;
		}
		else
		{
			++currentY;
		}
	}

	return true;
}

Chunk::BlockInfo* ElkGameEngine::Objects::Components::Behaviours::Chunk::GetBlock(
	uint8_t& p_x, uint8_t& p_y, uint8_t& p_z, Side p_side, uint8_t p_shifting)
{
	switch (p_side)
	{
	case Side::LEFT:
		return (int(p_x - p_shifting) >= 0)
			       ? &m_blocks[From3Dto1D(p_x - p_shifting, p_y, p_z)]
			       : (m_neighbours.left ? &m_neighbours.left->m_blocks[From3Dto1D(15 - p_shifting + 1, p_y, p_z)] : nullptr);

	case Side::RIGHT:
		return (int(p_x + p_shifting) <= 15)
			       ? &m_blocks[From3Dto1D(p_x + p_shifting, p_y, p_z)]
			       : (m_neighbours.right ? &m_neighbours.right->m_blocks[From3Dto1D(0 + p_shifting - 1, p_y, p_z)] : nullptr);

	case Side::BOT:
		return (int(p_y - p_shifting) >= 0)
			       ? &m_blocks[From3Dto1D(p_x, p_y - p_shifting, p_z)]
			       : (m_neighbours.bot ? &m_neighbours.bot->m_blocks[From3Dto1D(p_x, 15 - p_shifting + 1, p_z)] : nullptr);

	case Side::TOP:
		return (int(p_y + p_shifting) <= 15)
			       ? &m_blocks[From3Dto1D(p_x, p_y + p_shifting, p_z)]
			       : (m_neighbours.top ? &m_neighbours.top->m_blocks[From3Dto1D(p_x, 0 + p_shifting - 1, p_z)] : nullptr);

	case Side::BACK:
		return (int(p_z - p_shifting) >= 0)
			       ? &m_blocks[From3Dto1D(p_x, p_y, p_z - p_shifting)]
			       : (m_neighbours.back ? &m_neighbours.back->m_blocks[From3Dto1D(p_x, p_y, 15 - p_shifting + 1)] : nullptr);

	case Side::FRONT:
		return (int(p_z + p_shifting) <= 15)
			       ? &m_blocks[From3Dto1D(p_x, p_y, p_z + p_shifting)]
			       : (m_neighbours.front ? &m_neighbours.front->m_blocks[From3Dto1D(p_x, p_y, 0 + p_shifting - 1)] : nullptr);
	}

	return nullptr;
}

void ElkGameEngine::Objects::Components::Behaviours::Chunk::UpdateChunk()
{
	if (IsEmpty() || IsOccluded())
		m_renderChunk.SetToDraw(false);
	else
		UpdateBlocks();
}

void ElkGameEngine::Objects::Components::Behaviours::Chunk::UpdateBlocks()
{
	uint16_t blocksAdded = 0;

	m_renderChunk.ClearBlocks();

	for (uint16_t i = 0; i < 4096; ++i)
	{
		auto position = From1Dto3D(i);
		uint8_t x = std::get<0>(position);
		uint8_t y = std::get<1>(position);
		uint8_t z = std::get<2>(position);

		m_blocks[i].SetToRender(m_blocks[i].type != 0 && !BlockIsOccluded(x, y, z));

		if (m_blocks[i].GetToRender())
		{
			if (IsHigherLayer(x, y, z))
				m_blocks[i].SetLightLevel(15);

			if (m_blocks[i].GetLightLevel() != 10)
				BlockPropagateLight(x, y, z, m_blocks[i].GetLightLevel());

			m_renderChunk.AddBlock(x, y, z, m_blocks[i].type, m_blocks[i].GetLightLevel(), BlocksInfo::IsSemiTransparent(m_blocks[i].type));
			++blocksAdded;
		}		
	}

	if (blocksAdded)
	{
		m_renderChunk.SetToDraw(true);
		m_renderChunk.Update();
	}
}

void ElkGameEngine::Objects::Components::Behaviours::Chunk::UpdateNeighbours()
{
	if (m_neighbours.left)
		m_neighbours.left->UpdateChunk();

	if (m_neighbours.right)
		m_neighbours.right->UpdateChunk();

	if (m_neighbours.bot)
		m_neighbours.bot->UpdateChunk();

	if (m_neighbours.top)
		m_neighbours.top->UpdateChunk();

	if (m_neighbours.back)
		m_neighbours.back->UpdateChunk();

	if (m_neighbours.front)
		m_neighbours.front->UpdateChunk();
}

void Chunk::FillChunk(uint8_t p_blockType)
{
	if (p_blockType == 0)
		m_airCounter = 4096;
	else
		m_airCounter = 0;

	memset(m_blocks, p_blockType, sizeof(m_blocks));
}
