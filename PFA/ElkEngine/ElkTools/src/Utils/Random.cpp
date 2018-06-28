#include "stdafx.h"

#include "ElkTools/Utils/Random.h"

std::default_random_engine ElkTools::Utils::Random::__GENERATOR = std::default_random_engine(static_cast<unsigned int>(time(0)));

int ElkTools::Utils::Random::Generate(int p_min, int p_max)
{
	std::uniform_int_distribution<int> distribution(p_min, p_max);
	return distribution(__GENERATOR);
}
