
#include "Precompiled.h"
using namespace CK;

Rectangle::Rectangle(const std::vector<Vector2> InVertices)
{
	for(const auto& v : InVertices)
	{
		*this += v;
	}
}
