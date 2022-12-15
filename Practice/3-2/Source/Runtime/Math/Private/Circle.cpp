
#include "Precompiled.h"
using namespace CK;

Circle::Circle(const std::vector<Vector2> InVertices)
{
	size_t cnt = InVertices.size();
	if (cnt == 0)
	{
		return;
	}

	Vector2 sum;
	for (const auto& v : InVertices)
	{
		sum += v;
	}

	Center = sum / (float)cnt;
	float distanceSquared = 0.f;
	Radius = (*std::max_element(InVertices.begin(), InVertices.end(),
		[&](Vector2 const& lhs, Vector2 const& rhs)
	{
		return (Center - lhs).SizeSquared() < (Center - rhs).SizeSquared();
	})).Size();
}
