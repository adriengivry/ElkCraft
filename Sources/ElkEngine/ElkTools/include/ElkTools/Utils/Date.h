#pragma once

#include <string>
#include <ctime>

#include "ElkTools/export.h"

namespace ElkTools
{
	namespace Utils
	{
		/*
		 * Static function used to get the current date as a string (Can be used as a filename, no unsupported chars)
		 */
		class ELK_TOOLS_EXPORT Date
		{
		public:
			static std::string GetDate();
		};
	}
}