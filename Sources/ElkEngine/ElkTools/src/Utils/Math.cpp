#include "stdafx.h"

#include "ElkTools/Utils/Math.h"

float ElkTools::Utils::Math::Lerp(float a, float b, float t)
{
	return (1 - t) * a + t * b;
}
