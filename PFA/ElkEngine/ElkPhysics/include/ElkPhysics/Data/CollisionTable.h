#pragma once
#include "ElkPhysics/export.h"
namespace ElkPhysics
{
	namespace Data
	{
		struct CollisionTable
		{
			uint8_t xTable[12];
			uint8_t yTable[8];
			uint8_t zTable[12];
			uint8_t diagonalTable[12];
			uint8_t onPlayerTable[4];
			CollisionTable()
			{
				for(auto i = 0 ; i < 12 ; i++)
				{
					xTable[i] = 0;
					zTable[i] = 0;
					diagonalTable[i] = 0;
				}
				for(auto& j : yTable)
				{
					j = 0;
				}
				for(auto& k : onPlayerTable)
				{
					k = 0;
				}
			}
		};
	}
}