
#include "Precompiled.h"
using namespace CK;

Box::Box(const std::vector<Vector3> InVertices)
{
	for (const auto& v : InVertices)
	{
		*this += v;
	}
}

