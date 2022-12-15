
#include "Precompiled.h"
using namespace CK;

Plane::Plane(const Vector3& InPoint1, const Vector3& InPoint2, const Vector3& InPoint3)
{
	Vector3 v1 = InPoint2 - InPoint1;
	Vector3 v2 = InPoint3 - InPoint1;

	Normal = v1.Cross(v2).GetNormalize();
	D = -Normal.Dot(InPoint1);
}

Plane::Plane(const Vector4& InVector4)
{
	Normal = InVector4.ToVector3();
	D = InVector4.W;
	Normalize();
}

void Plane::Normalize()
{
	float squaredSize = Normal.SizeSquared();
	if (Math::EqualsInTolerance(squaredSize, 1.f))
	{
		return;
	}

	float invLength = Math::InvSqrt(squaredSize);
	Normal *= invLength;
	D *= invLength;
}
