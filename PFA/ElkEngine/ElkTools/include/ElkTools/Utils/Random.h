#pragma once

#include "ElkTools/export.h"

namespace ElkTools
{
	namespace Utils
	{
		class ELK_TOOLS_EXPORT Random
		{
		public:
			static int Generate(int p_min, int p_max);

		private:
			static std::default_random_engine __GENERATOR;
		};
	}
}