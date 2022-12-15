#pragma once

namespace CK
{

struct Vector2
{
public:
	// 생성자 
	FORCEINLINE constexpr Vector2() = default;
	FORCEINLINE explicit constexpr Vector2(int InX, int InY) : X((float)InX), Y((float)InY) { }
	FORCEINLINE explicit constexpr Vector2(float InX, float InY) : X(InX), Y(InY) { }

	// 연산자 
	FORCEINLINE constexpr float operator[](BYTE InIndex) const;
	FORCEINLINE constexpr float& operator[](BYTE InIndex);
	FORCEINLINE constexpr Vector2 operator-() const;
	FORCEINLINE constexpr Vector2 operator*(float InScalar) const;
	FORCEINLINE constexpr Vector2 operator/(float InScalar) const;
	FORCEINLINE constexpr Vector2 operator*(const Vector2& InVector) const;
	FORCEINLINE constexpr Vector2 operator+(const Vector2& InVector) const;
	FORCEINLINE constexpr Vector2 operator-(const Vector2& InVector) const;
	FORCEINLINE constexpr Vector2& operator*=(float InScale);
	FORCEINLINE constexpr Vector2& operator/=(float InScale);
	FORCEINLINE constexpr Vector2& operator+=(const Vector2& InVector);
	FORCEINLINE constexpr Vector2& operator-=(const Vector2& InVector);

	// 멤버함수 
	FORCEINLINE float Size() const;
	FORCEINLINE constexpr float SizeSquared() const;
	void Normalize();
	[[nodiscard]] Vector2 GetNormalize() const;
	FORCEINLINE constexpr bool EqualsInTolerance(const Vector2& InVector, float InTolerance = SMALL_NUMBER) const;
	FORCEINLINE constexpr float Max() const;
	FORCEINLINE constexpr float Dot(const Vector2& InVector) const;
	FORCEINLINE float Angle() const;
	FORCEINLINE float AngleInDegree() const;
	FORCEINLINE Vector2 ToPolarCoordinate() const;
	FORCEINLINE constexpr Vector2 ToCartesianCoordinate() const;

	std::string ToString() const;

	// 정적멤버변수 
	static const Vector2 UnitX;
	static const Vector2 UnitY;
	static const Vector2 One;
	static const Vector2 Zero;
	static constexpr BYTE Dimension = 2;

	// 멤버변수
	union
	{
		struct
		{
			float X, Y;
		};

		std::array<float, Dimension> Scalars = { 0.f, 0.f };
	};
};

FORCEINLINE float Vector2::Size() const
{
	return sqrtf(SizeSquared());
}

FORCEINLINE constexpr float Vector2::SizeSquared() const
{
	return X * X + Y * Y;
}

FORCEINLINE void Vector2::Normalize()
{
	*this = GetNormalize();
}

FORCEINLINE constexpr float Vector2::operator[](BYTE InIndex) const
{
	assert(InIndex < Dimension);
	return Scalars[InIndex];
}

FORCEINLINE constexpr float &Vector2::operator[](BYTE InIndex)
{
	assert(InIndex < Dimension);
	return Scalars[InIndex];
}

FORCEINLINE constexpr Vector2 Vector2::operator-() const
{
	return Vector2(-X, -Y);
}

FORCEINLINE constexpr Vector2 Vector2::operator*(float InScalar) const
{
	return Vector2(X * InScalar, Y * InScalar);
}

FORCEINLINE constexpr Vector2 Vector2::operator/(float InScalar) const
{
	return Vector2(X / InScalar, Y / InScalar);
}

FORCEINLINE constexpr Vector2 Vector2::operator+(const Vector2& InVector) const
{
	return Vector2(X + InVector.X, Y + InVector.Y);
}

FORCEINLINE constexpr Vector2 Vector2::operator-(const Vector2& InVector) const
{
	return Vector2(X - InVector.X, Y - InVector.Y);
}

FORCEINLINE constexpr Vector2 Vector2::operator*(const Vector2& InVector) const
{
	return Vector2(X * InVector.X, Y * InVector.Y);
}

FORCEINLINE constexpr Vector2& Vector2::operator*=(float InScale)
{
	X *= InScale;
	Y *= InScale;
	return *this;
}

FORCEINLINE constexpr Vector2& Vector2::operator/=(float InScale)
{
	X /= InScale;
	Y /= InScale;
	return *this;
}

FORCEINLINE constexpr Vector2& Vector2::operator+=(const Vector2& InVector)
{
	X += InVector.X;
	Y += InVector.Y;
	return *this;
}

FORCEINLINE constexpr Vector2& Vector2::operator-=(const Vector2& InVector)
{
	X -= InVector.X;
	Y -= InVector.Y;
	return *this;
}

FORCEINLINE constexpr bool Vector2::EqualsInTolerance(const Vector2& InVector, float InTolerance) const
{
	return (Math::Abs(this->X - InVector.X) <= InTolerance) &&
		(Math::Abs(this->Y - InVector.Y) < InTolerance);
}

FORCEINLINE constexpr float Vector2::Max() const
{
	return Math::Max(X, Y);
}

FORCEINLINE constexpr float Vector2::Dot(const Vector2& InVector) const
{
	return X * InVector.X + Y * InVector.Y;
}

FORCEINLINE float Vector2::Angle() const
{
	return atan2f(Y,X);
}

FORCEINLINE float Vector2::AngleInDegree() const
{
	return Math::Rad2Deg(atan2f(Y, X));
}

FORCEINLINE Vector2 Vector2::ToPolarCoordinate() const
{
	return Vector2(Size(), Angle());
}

FORCEINLINE constexpr Vector2 Vector2::ToCartesianCoordinate() const
{
	// 극좌표계에서 X는 반지름, Y는 각(rad)으로 활용
	float sin = 0.f, cos = 0.f;
	Math::GetSinCosRad(sin, cos, Y);
	return Vector2(X * cos, X * sin);
}

}