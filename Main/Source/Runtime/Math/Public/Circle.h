#pragma once

namespace CK
{

struct Circle
{
public:
	FORCEINLINE constexpr Circle() = default;
	FORCEINLINE constexpr Circle(const Circle& InCircle) : Center(InCircle.Center), Radius(InCircle.Radius) {};
	Circle(const std::vector<Vector2> InVertices);

	FORCEINLINE constexpr bool IsInside(const Vector2& InVector) const;
	FORCEINLINE constexpr bool Intersect(const Circle& InCircle) const;

public:
	Vector2 Center = Vector2::Zero;
	float Radius = 0.f;
};

FORCEINLINE constexpr bool Circle::IsInside(const Vector2& InVector) const
{
	return ((Center - InVector).SizeSquared() <= (Radius * Radius));
}

FORCEINLINE constexpr bool Circle::Intersect(const Circle& InCircle) const
{
	float twoRadiusSum = Radius + InCircle.Radius;
	return (Center - InCircle.Center).SizeSquared() <= (twoRadiusSum * twoRadiusSum);
}

}