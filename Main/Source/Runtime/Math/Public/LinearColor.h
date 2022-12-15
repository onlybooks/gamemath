#pragma once

namespace CK
{

struct LinearColor
{
public:
	FORCEINLINE constexpr LinearColor() = default;
	FORCEINLINE explicit constexpr LinearColor(float InR, float InG, float InB, float InA = 1.f) : R(InR), G(InG), B(InB), A(InA) {}
	FORCEINLINE explicit constexpr LinearColor(const Color32& InColor32)
	{
		R = float(InColor32.R) * OneOver255;
		G = float(InColor32.G) * OneOver255;
		B = float(InColor32.B) * OneOver255;
		A = float(InColor32.A) * OneOver255;
	}

	FORCEINLINE constexpr Color32 ToColor32(const bool bSRGB = false) const;

	FORCEINLINE constexpr LinearColor operator+(const LinearColor& InColor) const;
	FORCEINLINE constexpr LinearColor operator-(const LinearColor& InColor) const;
	FORCEINLINE constexpr LinearColor operator*(const LinearColor& InColor) const;
	FORCEINLINE constexpr LinearColor operator*(float InScalar) const;
	FORCEINLINE constexpr LinearColor& operator*=(float InScale);
	FORCEINLINE constexpr LinearColor& operator/=(float InScale);
	FORCEINLINE constexpr LinearColor& operator+=(const LinearColor& InColor);
	FORCEINLINE constexpr LinearColor& operator-=(const LinearColor& InColor);

	FORCEINLINE constexpr bool operator==(const LinearColor& InColor) const;
	FORCEINLINE constexpr bool operator!=(const LinearColor& InColor) const;

	FORCEINLINE constexpr bool EqualsInRange(const LinearColor& InColor, float InTolerance = SMALL_NUMBER) const;

	static constexpr float OneOver255 = { 1.f / 255.f };
	static const LinearColor Error;
	static const LinearColor White;
	static const LinearColor Black;
	static const LinearColor Gray;
	static const LinearColor Silver;
	static const LinearColor WhiteSmoke;
	static const LinearColor LightGray;
	static const LinearColor DimGray;
	static const LinearColor Red;
	static const LinearColor Green;
	static const LinearColor Blue;
	static const LinearColor Yellow;
	static const LinearColor Cyan;
	static const LinearColor Magenta;

public:
	float R = 0.f;
	float G = 0.f;
	float B = 0.f;
	float A = 1.f;
};

FORCEINLINE constexpr Color32 LinearColor::ToColor32(const bool bSRGB) const
{
	float FloatR = Math::Clamp(R, 0.f, 1.f);
	float FloatG = Math::Clamp(G, 0.f, 1.f);
	float FloatB = Math::Clamp(B, 0.f, 1.f);
	float FloatA = Math::Clamp(A, 0.f, 1.f);

	return Color32(
		(int)(FloatR * 255.999f),
		(int)(FloatG * 255.999f),
		(int)(FloatB * 255.999f),
		(int)(FloatA * 255.999f)
	);
}

FORCEINLINE constexpr LinearColor LinearColor::operator+(const LinearColor& InColor) const
{
	return LinearColor(
		R + InColor.R,
		G + InColor.G,
		B + InColor.B,
		A + InColor.A
	);
}

FORCEINLINE constexpr LinearColor LinearColor::operator-(const LinearColor& InColor) const
{
	return LinearColor(
		R - InColor.R,
		G - InColor.G,
		B - InColor.B,
		A - InColor.A
	);
}

FORCEINLINE constexpr LinearColor LinearColor::operator*(const LinearColor& InColor) const
{
	return LinearColor(
		R * InColor.R,
		G * InColor.G,
		B * InColor.B,
		A * InColor.A
	);
}

FORCEINLINE constexpr LinearColor LinearColor::operator*(float InScalar) const
{
	return LinearColor(
		R * InScalar,
		G * InScalar,
		B * InScalar,
		A * InScalar
	);
}

FORCEINLINE constexpr LinearColor& LinearColor::operator*=(float InScale)
{
	R *= InScale;
	G *= InScale;
	B *= InScale;
	A *= InScale;
	return *this;
}

FORCEINLINE constexpr LinearColor& LinearColor::operator/=(float InScale)
{
	R /= InScale;
	G /= InScale;
	B /= InScale;
	A /= InScale;
	return *this;
}

FORCEINLINE constexpr LinearColor& LinearColor::operator+=(const LinearColor& InColor)
{
	R += InColor.R;
	G += InColor.G;
	B += InColor.B;
	A += InColor.A;
	return *this;
}

FORCEINLINE constexpr LinearColor& LinearColor::operator-=(const LinearColor& InColor)
{
	R -= InColor.R;
	G -= InColor.G;
	B -= InColor.B;
	A -= InColor.A;
	return *this;
}

FORCEINLINE constexpr bool LinearColor::operator==(const LinearColor& InColor) const
{
	return this->R == InColor.R && this->G == InColor.G && this->B == InColor.B && this->A == InColor.A;
}

FORCEINLINE constexpr bool LinearColor::operator!=(const LinearColor& InColor) const
{
	return this->R != InColor.R || this->G != InColor.G || this->B != InColor.B || this->A != InColor.A;
}

FORCEINLINE constexpr bool LinearColor::EqualsInRange(const LinearColor& InColor, float InTolerance) const
{
	return (Math::Abs(this->R - InColor.R) < InTolerance) &&
		(Math::Abs(this->G - InColor.G) < InTolerance) &&
		(Math::Abs(this->B - InColor.B) < InTolerance) &&
		(Math::Abs(this->A - InColor.A) < InTolerance);
}

}
