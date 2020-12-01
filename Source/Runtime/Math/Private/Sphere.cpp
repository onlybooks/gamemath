
#include "Precompiled.h"
using namespace CK;

Sphere::Sphere(const std::vector<Vector3>& InVertices)
{
	size_t cnt = InVertices.size();
	if (cnt == 0)
	{
		return;
	}

	Vector3 sum;
	for (const auto& v : InVertices)
	{
		sum += v;
	}

	Center = sum / (float)cnt;
	float distanceSquared = 0.f;
	Radius = (*std::max_element(InVertices.begin(), InVertices.end(),
		[&](Vector3 const& lhs, Vector3 const& rhs)
	{
		return (Center - lhs).SizeSquared() < (Center - rhs).SizeSquared();
	})).Size();
}
