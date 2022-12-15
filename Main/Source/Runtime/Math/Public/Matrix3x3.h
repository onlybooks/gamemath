#pragma once

namespace CK
{

struct Matrix3x3
{
public:
	// 생성자 
	FORCEINLINE constexpr Matrix3x3() = default;
	FORCEINLINE explicit constexpr Matrix3x3(const Vector3& InCol0, const Vector3& InCol1, const Vector3& InCol2) { Cols = { InCol0, InCol1, InCol2 }; }

	// 연산자 
	FORCEINLINE const Vector3& operator[](BYTE InIndex) const;
	FORCEINLINE Vector3& operator[](BYTE InIndex);

	FORCEINLINE Matrix3x3 operator*(float InScalar) const;
	FORCEINLINE Matrix3x3 operator*(const Matrix3x3& InMatrix) const;
	FORCEINLINE Vector3 operator*(const Vector3& InVector) const;
	FORCEINLINE friend Vector3 operator*=(Vector3& InVector, const Matrix3x3& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}
	FORCEINLINE Vector2 operator*(const Vector2& InVector) const;
	FORCEINLINE friend Vector2 operator*=(Vector2& InVector, const Matrix3x3& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}

	// 멤버함수 
	FORCEINLINE Matrix2x2 ToMatrix2x2() const;
	FORCEINLINE void SetIdentity();
	FORCEINLINE Matrix3x3 Transpose() const;

	std::vector<std::string> ToStrings() const;

	// 정적멤버변수 
	static const Matrix3x3 Identity;
	static constexpr BYTE Rank = 3;

	// 멤버변수 
	std::array<Vector3, Rank> Cols = { Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ };
};

FORCEINLINE void Matrix3x3::SetIdentity()
{
	*this = Matrix3x3::Identity;
}

FORCEINLINE Matrix3x3 Matrix3x3::Transpose() const
{
	return Matrix3x3(
		Vector3(Cols[0].X, Cols[1].X, Cols[2].X),
		Vector3(Cols[0].Y, Cols[1].Y, Cols[2].Y),
		Vector3(Cols[0].Z, Cols[1].Z, Cols[2].Z)
	);
}

FORCEINLINE const Vector3& Matrix3x3::operator[](BYTE InIndex) const
{
	assert(InIndex < Rank);
	return Cols[InIndex];
}

FORCEINLINE Vector3& Matrix3x3::operator[](BYTE InIndex)
{
	assert(InIndex < Rank);
	return Cols[InIndex];
}

FORCEINLINE Matrix3x3 Matrix3x3::operator*(float InScalar) const
{
	return Matrix3x3(
		Cols[0] * InScalar,
		Cols[1] * InScalar,
		Cols[2] * InScalar
	);
}

FORCEINLINE Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &InMatrix) const
{
	Matrix3x3 transposedMatrix = Transpose();
	return Matrix3x3(
		Vector3(transposedMatrix[0].Dot(InMatrix[0]), transposedMatrix[1].Dot(InMatrix[0]), transposedMatrix[2].Dot(InMatrix[0])),
		Vector3(transposedMatrix[0].Dot(InMatrix[1]), transposedMatrix[1].Dot(InMatrix[1]), transposedMatrix[2].Dot(InMatrix[1])),
		Vector3(transposedMatrix[0].Dot(InMatrix[2]), transposedMatrix[1].Dot(InMatrix[2]), transposedMatrix[2].Dot(InMatrix[2]))
	);

}

FORCEINLINE Vector3 Matrix3x3::operator*(const Vector3& InVector) const
{
	Matrix3x3 transposedMatrix = Transpose();
	return Vector3(
		transposedMatrix[0].Dot(InVector),
		transposedMatrix[1].Dot(InVector),
		transposedMatrix[2].Dot(InVector)
	);
}

FORCEINLINE Vector2 Matrix3x3::operator*(const Vector2& InVector) const
{
	Vector3 v3(InVector);
	v3 *= *this;

	return v3.ToVector2();
}

FORCEINLINE Matrix2x2 Matrix3x3::ToMatrix2x2() const
{
	return Matrix2x2(Cols[0].ToVector2(), Cols[1].ToVector2());
}

}