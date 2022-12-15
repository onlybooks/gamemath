#pragma once

namespace CK
{

struct Box
{
public:
	FORCEINLINE constexpr Box() = default;
	FORCEINLINE constexpr Box(const Box& InBox) : Min(InBox.Min), Max(InBox.Max) { }
	FORCEINLINE constexpr Box(const Vector3& InMinVector, const Vector3& InMaxVector) : Min(InMinVector), Max(InMaxVector) { }
	Box(const std::vector<Vector3> InVertices);

	FORCEINLINE constexpr bool Intersect(const Box& InBox) const;
	FORCEINLINE constexpr bool IsInside(const Box& InBox) const;
	FORCEINLINE constexpr bool IsInside(const Vector3& InVector) const;

	FORCEINLINE constexpr Box operator+=(const Vector3& InVector);
	FORCEINLINE constexpr Box operator+=(const Box& InBox);

	FORCEINLINE constexpr Vector3 GetSize() const;
	FORCEINLINE constexpr Vector3 GetExtent() const;
	FORCEINLINE constexpr void GetCenterAndExtent(Vector3& OutCenter, Vector3& OutExtent) const;

public:
	Vector3 Min;
	Vector3 Max;
};

FORCEINLINE constexpr bool Box::Intersect(const Box& InBox) const
{
	if ((Min.X > InBox.Max.X) || (InBox.Min.X > Max.X))
	{
		return false;
	}

	if ((Min.Y > InBox.Max.Y) || (InBox.Min.Y > Max.Y))
	{
		return false;
	}

	if ((Min.Z > InBox.Max.Z) || (InBox.Min.Z > Max.Z))
	{
		return false;
	}

	return true;
}

FORCEINLINE constexpr bool Box::IsInside(const Box& InBox) const
{
	return (IsInside(InBox.Min) && IsInside(InBox.Max));
}

FORCEINLINE constexpr bool Box::IsInside(const Vector3& InVector) const
{
	return ((InVector.X >= Min.X) && (InVector.X <= Max.X) && (InVector.Y >= Min.Y) && (InVector.Y <= Max.Y) && (InVector.Z >= Min.Z) && (InVector.Z <= Max.Z));
}

FORCEINLINE constexpr Box Box::operator+=(const Vector3& InVector)
{
	Min.X = Math::Min(Min.X, InVector.X);
	Min.Y = Math::Min(Min.Y, InVector.Y);
	Min.Z = Math::Min(Min.Z, InVector.Z);

	Max.X = Math::Max(Max.X, InVector.X);
	Max.Y = Math::Max(Max.Y, InVector.Y);
	Max.Z = Math::Max(Max.Z, InVector.Z);

	return *this;
}

FORCEINLINE constexpr Box Box::operator+=(const Box& InBox)
{
	Min.X = Math::Min(Min.X, InBox.Min.X);
	Min.Y = Math::Min(Min.Y, InBox.Min.Y);
	Min.Z = Math::Min(Min.Z, InBox.Min.Z);

	Max.X = Math::Max(Max.X, InBox.Max.X);
	Max.Y = Math::Max(Max.Y, InBox.Max.Y);
	Max.Z = Math::Max(Max.Z, InBox.Max.Z);
	return *this;
}

FORCEINLINE constexpr Vector3 Box::GetSize() const
{
	return (Max - Min);
}

FORCEINLINE constexpr Vector3 Box::GetExtent() const
{
	return GetSize() * 0.5f;
}

FORCEINLINE constexpr void Box::GetCenterAndExtent(Vector3 & OutCenter, Vector3 & OutExtent) const
{
	OutExtent = GetExtent();
	OutCenter = Min + OutExtent;
}

}


