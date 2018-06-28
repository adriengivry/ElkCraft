#pragma once

#include <ElkGameEngine\ElkGameEngine.h>

using namespace ElkTools::Utils;

namespace ElkCraft::Terrain
{
	struct NoiseProperties
	{
		uint16_t seed;
		float scale;
		uint8_t octaves;
		float persistance;
		float lacunarity;
		float redistribution;
		float heightScale;
		uint8_t maxHeight;
		uint8_t additionalElevation;
		uint8_t waterLevel;

		NoiseProperties()
			: seed{ static_cast<uint16_t>(IniLoader::GetBool("world.ini", "random_seed") ? Random::Generate(1, 10000) : IniLoader::GetInt("world.ini", "map_seed")) },
			scale{ IniLoader::GetFloat("world.ini", "noise_scale") },
			octaves{ static_cast<uint8_t>(IniLoader::GetInt("world.ini", "noise_octaves")) },
			persistance{ IniLoader::GetFloat("world.ini", "noise_persistance") },
			lacunarity{ IniLoader::GetFloat("world.ini", "noise_lacunarity") },
			redistribution{ IniLoader::GetFloat("world.ini", "noise_redistribution") },
			heightScale{ IniLoader::GetFloat("world.ini", "noise_height_scale") },
			maxHeight{ static_cast<uint8_t>(ElkTools::Utils::IniLoader::GetInt("world.ini", "noise_max_height")) },
			additionalElevation{ static_cast<uint8_t>(IniLoader::GetInt("world.ini", "noise_additional_elevation")) },
			waterLevel{ static_cast<uint8_t>(ElkTools::Utils::IniLoader::GetInt("world.ini", "water_level")) }
		{
		}
	};

	struct TerrainLayer
	{
		TerrainLayer(const std::string& p_name, const uint8_t& p_blockID, const uint8_t& p_height)
			: name{ p_name }, blockID{ p_blockID }, height{ p_height }
		{
		}

		std::string name;
		uint8_t blockID;
		uint8_t height;
	};

	struct TerrainProperty
	{
		enum class Mode
		{
			FILL = 0,
			REPLACE,
			SURFACE
		};

		TerrainProperty(const std::string& p_name, const uint8_t& p_blockID, const uint16_t& p_threshold, const Mode& p_mode)
			: name{ p_name }, blockID{ p_blockID }, threshold{ p_threshold }, mode{ p_mode }
		{
		}

		std::string name;
		uint8_t blockID;
		uint16_t threshold;
		Mode mode;
	};
}