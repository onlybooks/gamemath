#pragma once

namespace CK
{

struct Rectangle
{
public:
	FORCEINLINE constexpr Rectangle() = default;
	FORCEINLINE constexpr Rectangle(const Rectangle& InRectangle) : Min(InRectangle.Min), Max(InRectangle.Max) { }
	FORCEINLINE constexpr Rectangle(const Vector2& InMinVector, const Vector2& InMaxVector) : Min(InMinVector), Max(InMaxVector) { }
	Rectangle(const std::vector<Vector2> InVertices);

	FORCEINLINE constexpr bool Intersect(const Rectangle& InRectangle) const;
	FORCEINLINE constexpr bool IsInside(const Rectangle& InRectangle) const;
	FORCEINLINE constexpr bool IsInside(const Vector2& InVector) const;

	FORCEINLINE constexpr Rectangle operator+=(const Vector2& InVector);
	FORCEINLINE constexpr Rectangle operator+=(const Rectangle& InRectangle);

	FORCEINLINE constexpr Vector2 GetSize() const;
	FORCEINLINE constexpr Vector2 GetExtent() const;
	FORCEINLINE constexpr void GetCenterAndExtent(Vector2& OutCenter, Vector2& OutExtent) const;

public:
	Vector2 Min;
	Vector2 Max;
};

FORCEINLINE constexpr bool Rectangle::Intersect(const Rectangle& InRectangle) const
{
	if ((Min.X > InRectangle.Max.X) || (InRectangle.Min.X > Max.X))
	{
		return false;
	}

	if ((Min.Y > InRectangle.Max.Y) || (InRectangle.Min.Y > Max.Y))
	{
		return false;
	}

	return true;
}

FORCEINLINE constexpr bool Rectangle::IsInside(const Rectangle& InRectangle) const
{
	return (IsInside(InRectangle.Min) && IsInside(InRectangle.Max));
}

FORCEINLINE constexpr bool Rectangle::IsInside(const Vector2& InVector) const
{
	return ((InVector.X >= Min.X) && (InVector.X <= Max.X) && (InVector.Y >= Min.Y) && (InVector.Y <= Max.Y));
}

FORCEINLINE constexpr Rectangle Rectangle::operator+=(const Vector2& InVector)
{
	Min.X = Math::Min(Min.X, InVector.X);
	Min.Y = Math::Min(Min.Y, InVector.Y);

	Max.X = Math::Max(Max.X, InVector.X);
	Max.Y = Math::Max(Max.Y, InVector.Y);
	return *this;
}

FORCEINLINE constexpr Rectangle Rectangle::operator+=(const Rectangle& InRectangle)
{
	Min.X = Math::Min(Min.X, InRectangle.Min.X);
	Min.Y = Math::Min(Min.Y, InRectangle.Min.Y);

	Max.X = Math::Max(Max.X, InRectangle.Max.X);
	Max.Y = Math::Max(Max.Y, InRectangle.Max.Y);
	return *this;
}

FORCEINLINE constexpr Vector2 Rectangle::GetSize() const
{
	return (Max - Min);
}

FORCEINLINE constexpr Vector2 Rectangle::GetExtent() const
{
	return GetSize() * 0.5f;
}

FORCEINLINE constexpr void Rectangle::GetCenterAndExtent(Vector2 & OutCenter, Vector2 & OutExtent) const
{
	OutExtent = GetExtent();
	OutCenter = Min + OutExtent;
}

}