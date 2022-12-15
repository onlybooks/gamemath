#pragma once

namespace CK
{

struct Vector4
{
public:
	// 생성자 
	FORCEINLINE constexpr Vector4() = default;
	FORCEINLINE explicit constexpr Vector4(const Vector2& InV, bool IsPoint = true) : X(InV.X), Y(InV.Y), Z(0.f) { W = IsPoint ? 1.f : 0.f; }
	FORCEINLINE explicit constexpr Vector4(const Vector3& InV, bool IsPoint = true) : X(InV.X), Y(InV.Y), Z(InV.Z) { W = IsPoint ? 1.f : 0.f; }
	FORCEINLINE explicit constexpr Vector4(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) { }
	FORCEINLINE explicit constexpr Vector4(float InX, float InY, float InZ, bool IsPoint = true) : X(InX), Y(InY), Z(InZ) { W = IsPoint ? 1.f : 0.f; }

	// 연산자 
	FORCEINLINE constexpr float operator[](BYTE InIndex) const;
	FORCEINLINE constexpr float& operator[](BYTE InIndex);
	FORCEINLINE constexpr Vector4 operator-() const;
	FORCEINLINE constexpr Vector4 operator*(float InScale) const;
	FORCEINLINE constexpr Vector4 operator/(float InScale) const;
	FORCEINLINE constexpr Vector4 operator*(const Vector4& InVector) const;
	FORCEINLINE constexpr Vector4 operator+(const Vector4& InVector) const;
	FORCEINLINE constexpr Vector4 operator-(const Vector4& InVector) const;
	FORCEINLINE constexpr Vector4& operator*=(float InScale);
	FORCEINLINE constexpr Vector4& operator/=(float InScale);
	FORCEINLINE constexpr Vector4& operator+=(const Vector4& InVector);
	FORCEINLINE constexpr Vector4& operator-=(const Vector4& InVector);

	// 멤버함수 
	FORCEINLINE constexpr Vector2 ToVector2() const;
	FORCEINLINE constexpr Vector3 ToVector3() const;
	FORCEINLINE float Size() const;
	FORCEINLINE constexpr float SizeSquared() const;
	void Normalize();
	[[nodiscard]] Vector4 GetNormalize() const;
	FORCEINLINE constexpr bool EqualsInTolerance(const Vector4& InVector, float InTolerance = SMALL_NUMBER) const;
	FORCEINLINE constexpr float Max() const;
	FORCEINLINE constexpr float Dot(const Vector4& InVector) const;

	std::string ToString() const;

	// 정적멤버변수 
	static const Vector4 UnitX;
	static const Vector4 UnitY;
	static const Vector4 UnitZ;
	static const Vector4 UnitW;
	static const Vector4 One;
	static const Vector4 Zero;
	static constexpr BYTE Dimension = 4;

	// 멤버변수
	union
	{
		struct
		{
			float X, Y, Z, W;
		};

		std::array<float, Dimension> Scalars = { 0.f, 0.f, 0.f, 0.f };
	};
};

FORCEINLINE constexpr Vector2 Vector4::ToVector2() const
{
	return Vector2(X, Y);
}

FORCEINLINE constexpr Vector3 Vector4::ToVector3() const
{
	return Vector3(X, Y, Z);
}

FORCEINLINE float Vector4::Size() const
{
	return sqrtf(SizeSquared());
}

FORCEINLINE constexpr float Vector4::SizeSquared() const
{
	return X * X + Y * Y + Z * Z + W * W;
}

FORCEINLINE void Vector4::Normalize()
{
	*this = GetNormalize();
}

FORCEINLINE constexpr float Vector4::operator[](BYTE InIndex) const
{
	assert(InIndex < Dimension);
	return Scalars[InIndex];
}

FORCEINLINE constexpr float& Vector4::operator[](BYTE InIndex)
{
	assert(InIndex < Dimension);
	return Scalars[InIndex];
}

FORCEINLINE constexpr Vector4 Vector4::operator-() const
{
	return Vector4(-X, -Y, -Z, -W);
}

FORCEINLINE constexpr Vector4 Vector4::operator*(float InScale) const
{
	return Vector4(X * InScale, Y * InScale, Z * InScale, W * InScale);
}

FORCEINLINE constexpr Vector4 Vector4::operator/(float InScale) const
{
	return Vector4(X / InScale, Y / InScale, Z / InScale, W / InScale);
}

FORCEINLINE constexpr Vector4 Vector4::operator*(const Vector4& InVector) const
{
	return Vector4(X * InVector.X, Y * InVector.Y, Z * InVector.Z, W * InVector.W);
}

FORCEINLINE constexpr Vector4 Vector4::operator+(const Vector4& InVector) const
{
	return Vector4(X + InVector.X, Y + InVector.Y, Z + InVector.Z, W + InVector.W);
}

FORCEINLINE constexpr Vector4 Vector4::operator-(const Vector4& InVector) const
{
	return Vector4(X - InVector.X, Y - InVector.Y, Z - InVector.Z, W - InVector.W);
}

FORCEINLINE constexpr Vector4& Vector4::operator*=(float InScale)
{
	X *= InScale;
	Y *= InScale;
	Z *= InScale;
	W *= InScale;
	return *this;
}

FORCEINLINE constexpr Vector4& Vector4::operator/=(float InScale)
{
	X /= InScale;
	Y /= InScale;
	Z /= InScale;
	W /= InScale;
	return *this;
}

FORCEINLINE constexpr Vector4& Vector4::operator+=(const Vector4& InVector)
{
	X += InVector.X;
	Y += InVector.Y;
	Z += InVector.Z;
	W += InVector.W;
	return *this;
}

FORCEINLINE constexpr Vector4& Vector4::operator-=(const Vector4& InVector)
{
	X -= InVector.X;
	Y -= InVector.Y;
	Z -= InVector.Z;
	W -= InVector.W;
	return *this;
}

FORCEINLINE constexpr bool Vector4::EqualsInTolerance(const Vector4& InVector, float InTolerance) const
{
	return (Math::Abs(this->X - InVector.X) <= InTolerance) &&
		(Math::Abs(this->Y - InVector.Y) < InTolerance) &&
		(Math::Abs(this->Z - InVector.Z) < InTolerance) &&
		(Math::Abs(this->W - InVector.W) < InTolerance);
}

FORCEINLINE constexpr float Vector4::Max() const
{
	float max = Math::Max(X, Y);
	max = Math::Max(max, Z);
	return Math::Max(max, W);
}


FORCEINLINE constexpr float Vector4::Dot(const Vector4& InVector) const
{
	return X * InVector.X + Y * InVector.Y + Z * InVector.Z + W * InVector.W;
}

}