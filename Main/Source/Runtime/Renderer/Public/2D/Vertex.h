
#pragma once

namespace CK
{
namespace DD
{

struct Vertex2D
{
public:
	constexpr Vertex2D() = default;
	constexpr Vertex2D(const Vector2& InPosition) : Position(InPosition) { }
	constexpr Vertex2D(const Vector2& InPosition, const LinearColor& InColor) : Position(InPosition), Color(InColor) { }
	constexpr Vertex2D(const Vector2& InPosition, const LinearColor& InColor, const Vector2& InUV) : Position(InPosition), Color(InColor), UV(InUV) { }

	constexpr Vertex2D operator*(float InScalar) const
	{
		return Vertex2D(
			Position * InScalar,
			Color * InScalar,
			UV * InScalar
		);
	}

	constexpr Vertex2D operator+(const Vertex2D& InVector) const
	{
		return Vertex2D(
			Position + InVector.Position,
			Color + InVector.Color,
			UV + InVector.UV
		);
	}

	Vector2 Position;
	LinearColor Color;
	Vector2 UV;
};

}
}