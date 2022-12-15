#pragma once

namespace CK
{

struct Vector3
{
public:
	// 생성자 
	FORCEINLINE constexpr Vector3() = default;
	FORCEINLINE explicit constexpr Vector3(const Vector2& InV, bool IsPoint = true) : X(InV.X), Y(InV.Y) { Z = IsPoint ? 1.f : 0.f; }
	FORCEINLINE explicit constexpr Vector3(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) { }

	// 연산자 
	FORCEINLINE constexpr float operator[](BYTE InIndex) const;
	FORCEINLINE constexpr float& operator[](BYTE InIndex);
	FORCEINLINE constexpr Vector3 operator-() const;
	FORCEINLINE constexpr Vector3 operator*(float InScale) const;
	FORCEINLINE constexpr Vector3 operator/(float InScale) const;
	FORCEINLINE constexpr Vector3 operator*(const Vector3& InVector) const;
	FORCEINLINE constexpr Vector3 operator+(const Vector3& InVector) const;
	FORCEINLINE constexpr Vector3 operator-(const Vector3& InVector) const;
	FORCEINLINE constexpr Vector3& operator*=(float InScale);
	FORCEINLINE constexpr Vector3& operator/=(float InScale);
	FORCEINLINE constexpr Vector3& operator+=(const Vector3& InVector);
	FORCEINLINE constexpr Vector3& operator-=(const Vector3& InVector);

	// 멤버함수 
	FORCEINLINE constexpr Vector2 ToVector2() const;
	FORCEINLINE float Size() const;
	FORCEINLINE constexpr float SizeSquared() const;
	void Normalize();
	[[nodiscard]] Vector3 GetNormalize() const;
	FORCEINLINE constexpr bool EqualsInTolerance(const Vector3& InVector, float InTolerance = SMALL_NUMBER) const;
	FORCEINLINE constexpr float Max() const;
	FORCEINLINE constexpr float Dot(const Vector3& InVector) const;
	FORCEINLINE constexpr Vector3 Cross(const Vector3& InVector) const;

	std::string ToString() const;


	// 정적멤버변수 
	static const Vector3 UnitX;
	static const Vector3 UnitY;
	static const Vector3 UnitZ;
	static const Vector3 One;
	static const Vector3 Zero;
	static constexpr BYTE Dimension = 3;

	// 멤버변수
	union
	{
		struct
		{
			float X, Y, Z;
		};

		std::array<float, Dimension> Scalars = { 0.f, 0.f, 0.f };
	};
};

FORCEINLINE constexpr Vector2 Vector3::ToVector2() const
{
	return Vector2(X, Y);
}

FORCEINLINE float Vector3::Size() const
{
	return sqrtf(SizeSquared());
}

FORCEINLINE constexpr float Vector3::SizeSquared() const
{
	return X * X + Y * Y + Z * Z;
}

FORCEINLINE void Vector3::Normalize()
{
	*this = GetNormalize();
}

FORCEINLINE constexpr float Vector3::operator[](BYTE InIndex) const
{
	assert(InIndex < Dimension);
	return Scalars[InIndex];
}

FORCEINLINE constexpr float &Vector3::operator[](BYTE InIndex)
{
	assert(InIndex < Dimension);
	return Scalars[InIndex];
}

FORCEINLINE constexpr Vector3 Vector3::operator-() const
{
	return Vector3(-X, -Y, -Z);
}

FORCEINLINE constexpr Vector3 Vector3::operator*(float InScale) const
{
	return Vector3(X * InScale, Y * InScale, Z * InScale);
}

FORCEINLINE constexpr Vector3 Vector3::operator/(float InScale) const
{
	return Vector3(X / InScale, Y / InScale, Z / InScale);
}

FORCEINLINE constexpr Vector3 Vector3::operator*(const Vector3& InVector) const
{
	return Vector3(X * InVector.X, Y * InVector.Y, Z * InVector.Z);
}

FORCEINLINE constexpr Vector3 Vector3::operator+(const Vector3& InVector) const
{
	return Vector3(X + InVector.X, Y + InVector.Y, Z + InVector.Z);
}

FORCEINLINE constexpr Vector3 Vector3::operator-(const Vector3& InVector) const
{
	return Vector3(X - InVector.X, Y - InVector.Y, Z - InVector.Z);
}

FORCEINLINE constexpr Vector3& Vector3::operator*=(float InScale)
{
	X *= InScale;
	Y *= InScale;
	Z *= InScale;
	return *this;
}

FORCEINLINE constexpr Vector3& Vector3::operator/=(float InScale)
{
	X /= InScale;
	Y /= InScale;
	Z /= InScale;
	return *this;
}

FORCEINLINE constexpr Vector3& Vector3::operator+=(const Vector3& InVector)
{
	X += InVector.X;
	Y += InVector.Y;
	Z += InVector.Z;
	return *this;
}

FORCEINLINE constexpr Vector3& Vector3::operator-=(const Vector3& InVector)
{
	X -= InVector.X;
	Y -= InVector.Y;
	Z -= InVector.Z;
	return *this;
}

FORCEINLINE constexpr bool Vector3::EqualsInTolerance(const Vector3& InVector, float InTolerance) const
{
	return (Math::Abs(this->X - InVector.X) <= InTolerance) &&
		(Math::Abs(this->Y - InVector.Y) < InTolerance) &&
		(Math::Abs(this->Z - InVector.Z) < InTolerance);
}

FORCEINLINE constexpr float Vector3::Max() const
{
	float max = Math::Max(X, Y);
	return Math::Max(max, Z);
}

FORCEINLINE constexpr float Vector3::Dot(const Vector3& InVector) const
{
	return X * InVector.X + Y * InVector.Y + Z * InVector.Z;
}

FORCEINLINE constexpr Vector3 Vector3::Cross(const Vector3& InVector) const
{
	return Vector3(
		Y * InVector.Z - Z * InVector.Y,
		Z * InVector.X - X * InVector.Z,
		X * InVector.Y - Y * InVector.X);
}

}