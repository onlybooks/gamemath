
#include "Precompiled.h"
using namespace CK;

const Vector3 Vector3::UnitX(1.f, 0.f, 0.f);
const Vector3 Vector3::UnitY(0.f, 1.f, 0.f);
const Vector3 Vector3::UnitZ(0.f, 0.f, 1.f);
const Vector3 Vector3::Zero(0.f, 0.f, 0.f);
const Vector3 Vector3::One(1.f, 1.f, 1.f);

std::string Vector3::ToString() const
{
	char result[64];
	std::snprintf(result, sizeof(result), "(%.3f, %.3f, %.3f)", X, Y, Z);
	return result;
}

Vector3 Vector3::GetNormalize() const
{
	float squareSum = SizeSquared();
	if (squareSum == 1.f)
	{
		return *this;
	}
	else if (squareSum == 0.f)
	{
		return Vector3::Zero;
	}

	float invLength = Math::InvSqrt(squareSum);
	return Vector3(X * invLength, Y * invLength, Z * invLength);
}