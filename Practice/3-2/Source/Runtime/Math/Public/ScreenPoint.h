#pragma once

namespace CK
{

struct ScreenPoint
{
public:
	FORCEINLINE constexpr ScreenPoint() = default;
	FORCEINLINE explicit constexpr ScreenPoint(int InX, int InY) : X(InX), Y(InY) { }
	FORCEINLINE explicit constexpr ScreenPoint(float InX, float InY) : X(Math::FloorToInt(InX)), Y(Math::FloorToInt(InY)) { }
	FORCEINLINE explicit constexpr ScreenPoint(const Vector2& InPos) : ScreenPoint(InPos.X, InPos.Y) {}

	constexpr ScreenPoint GetHalf() const { return ScreenPoint(Math::FloorToInt(0.5f * X), Math::FloorToInt(0.5f * Y)); }
	constexpr float AspectRatio() const { return (float)X / (float)Y; } // Y축 기준으로 측정
	FORCEINLINE constexpr bool HasZero() const { return ( X == 0 || Y == 0 ); }

	FORCEINLINE static constexpr ScreenPoint ToScreenCoordinate(const ScreenPoint& InScreenSize, const Vector2& InPos)
	{
		return ScreenPoint(InPos.X + InScreenSize.X * 0.5f, -InPos.Y + InScreenSize.Y * 0.5f);
	}

	FORCEINLINE constexpr Vector2 ToCartesianCoordinate(const ScreenPoint& InScreenSize)
	{
		return Vector2(X - InScreenSize.X * 0.5f + 0.5f, -(Y + 0.5f) + InScreenSize.Y * 0.5f);
	}

	FORCEINLINE constexpr ScreenPoint operator-(const ScreenPoint& InPoint) const;
	FORCEINLINE constexpr ScreenPoint operator+(const ScreenPoint& InPoint) const;

	int X = 0;
	int Y = 0;
};

FORCEINLINE constexpr ScreenPoint ScreenPoint::operator-(const ScreenPoint& InPoint) const
{
	return ScreenPoint(X - InPoint.X, Y - InPoint.Y);
}

FORCEINLINE constexpr ScreenPoint ScreenPoint::operator+(const ScreenPoint& InPoint) const
{
	return ScreenPoint(X + InPoint.X, Y + InPoint.Y);
}

}
