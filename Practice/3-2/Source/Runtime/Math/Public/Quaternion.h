#pragma once

namespace CK
{

struct Quaternion
{
public:
	FORCEINLINE constexpr Quaternion() = default;
	FORCEINLINE explicit constexpr Quaternion(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) { }
	FORCEINLINE explicit constexpr Quaternion(const Vector3& InAxis, float InAngleDegree)
	{
		FromAxisAngle(InAxis, InAngleDegree);
	}

	FORCEINLINE explicit constexpr Quaternion(const Rotator& InRotator)
	{
		FromRotator(InRotator);
	}

	FORCEINLINE Quaternion(const Vector3& InVector, const Vector3& InUp = Vector3::UnitY)
	{
		FromVector(InVector, InUp);
	}

	FORCEINLINE Quaternion(const Matrix3x3& InMatrix)
	{
		FromMatrix(InMatrix);
	}

	// 생성
	FORCEINLINE void FromMatrix(const Matrix3x3& InMatrix);
	FORCEINLINE void FromVector(const Vector3& InVector, const Vector3& InUp = Vector3::UnitY);
	FORCEINLINE constexpr void FromAxisAngle(const Vector3& InAxis, float InAngleDegree);
	FORCEINLINE constexpr void FromRotator(const Rotator& InRotator);

	FORCEINLINE constexpr Quaternion operator-() const;
	FORCEINLINE constexpr Quaternion operator*(const Quaternion& InQuaternion) const;
	FORCEINLINE constexpr Quaternion operator*=(const Quaternion& InQuaternion);
	FORCEINLINE constexpr Vector3 operator*(const Vector3& InVector) const;
	static Quaternion Slerp(const Quaternion &InQuaternion1, const Quaternion &InQuaternion2, float InRatio);

	FORCEINLINE constexpr Vector3 RotateVector(const Vector3& InVector) const;
	FORCEINLINE constexpr Quaternion Inverse() const { return Quaternion(-X, -Y, -Z, W); }
	FORCEINLINE void Normalize();
	FORCEINLINE Rotator ToRotator() const;
	FORCEINLINE bool IsUnitQuaternion() const
	{
		float size = sqrtf(X * X + Y * Y + Z * Z + W * W);
		if (Math::EqualsInTolerance(size, 1.f))
		{
			return true;
		}
		
		return false;
	}

	FORCEINLINE constexpr float RealPart() const { return W; }
	FORCEINLINE constexpr Vector3 ImaginaryPart() const { return Vector3(X, Y, Z); }
	std::string ToString() const;

	static const Quaternion Identity;

public:
	float X = 0.f;
	float Y = 0.f;
	float Z = 0.f;
	float W = 1.f;
};

FORCEINLINE void Quaternion::FromMatrix(const Matrix3x3& InMatrix)
{
	float root = 0.f;
	float trace = InMatrix[0][0] + InMatrix[1][1] + InMatrix[2][2];

	if (!Math::EqualsInTolerance(InMatrix[0].SizeSquared(), 1.f) || !Math::EqualsInTolerance(InMatrix[1].SizeSquared(), 1.f) || !Math::EqualsInTolerance(InMatrix[2].SizeSquared(), 1.f))
	{
		*this = Quaternion::Identity;
	}

	if (trace > 0.f)
	{
		// W 요소를 구하고 나머지 X,Y,Z를 계산
		root = sqrtf(trace + 1.f);
		W = 0.5f * root;
		root = 0.5f / root;

		X = (InMatrix[1][2] - InMatrix[2][1]) * root;
		Y = (InMatrix[2][0] - InMatrix[0][2]) * root;
		Z = (InMatrix[0][1] - InMatrix[1][0]) * root;
	}
	else
	{
		BYTE i = 0;

		// X,Y,Z 중에서 가장 큰 요소를 파악
		if (InMatrix[1][1] > InMatrix[0][0]) { i = 1; }
		if (InMatrix[2][2] > InMatrix[i][i]) { i = 2; }

		// i, j, k 의 순서 지정
		static const BYTE next[3] = { 1, 2, 0 };
		BYTE j = next[i];
		BYTE k = next[j];

		// 가장 큰 요소의 값을 구하기
		root = sqrtf(InMatrix[i][i] - InMatrix[j][j] - InMatrix[k][k] + 1.f);

		float* qt[3] = { &X, &Y, &Z };
		*qt[i] = 0.5f * root;

		root = 0.5f / root;

		// 나머지 두 요소의 값을 구하기
		*qt[j] = (InMatrix[i][j] + InMatrix[j][i]) * root;
		*qt[k] = (InMatrix[i][k] + InMatrix[k][i]) * root;

		// 마지막 W 값 구하기
		W = (InMatrix[j][k] - InMatrix[k][j]) * root;
	}
}

FORCEINLINE void Quaternion::FromVector(const Vector3& InVector, const Vector3& InUp)
{
	Vector3 localX, localY, localZ;

	// 단위 Z축 생성
	localZ = InVector.GetNormalize();
	// 시선 방향과 월드 Y축이 평행한 경우
	if (Math::Abs(localZ.Y) >= (1.f - SMALL_NUMBER))
	{
		// 특이 상황에서 로컬 X 좌표 값을 임의로 지정.
		localX = Vector3::UnitX;
	}
	else
	{
		localX = InUp.Cross(localZ).GetNormalize();
	}
	localY = localZ.Cross(localX).GetNormalize();

	FromMatrix(Matrix3x3(localX, localY, localZ));
}

FORCEINLINE constexpr void Quaternion::FromAxisAngle(const Vector3& InAxis, float InAngleDegree)
{
	float sin = 0.f, cos = 0.f;
	Math::GetSinCos(sin, cos, InAngleDegree * 0.5f);
	W = cos;
	X = sin * InAxis.X;
	Y = sin * InAxis.Y;
	Z = sin * InAxis.Z;
}

FORCEINLINE constexpr void Quaternion::FromRotator(const Rotator& InRotator)
{
	float sp = 0.f, sy = 0.f, sr = 0.f;
	float cp = 0.f, cy = 0.f, cr = 0.f;

	Math::GetSinCos(sp, cp, InRotator.Pitch * 0.5f);
	Math::GetSinCos(sy, cy, InRotator.Yaw * 0.5f);
	Math::GetSinCos(sr, cr, InRotator.Roll * 0.5f);

	W = sy * sp * sr + cy * cp * cr;
	X = sy * sr * cp + sp * cy * cr;
	Y = sy * cp * cr - sp * sr * cy;
	Z = -sy * sp * cr + sr * cy * cp;
}

FORCEINLINE constexpr Quaternion Quaternion::operator*(const Quaternion & InQuaternion) const
{
	Quaternion result;
	Vector3 v1(X, Y, Z), v2(InQuaternion.X, InQuaternion.Y, InQuaternion.Z);
	result.W = W * InQuaternion.W - v1.Dot(v2);
	Vector3 v = v2 * W + v1 * InQuaternion.W + v1.Cross(v2);
	result.X = v.X;
	result.Y = v.Y;
	result.Z = v.Z;

	return result;
}

FORCEINLINE constexpr Quaternion Quaternion::operator-() const
{
	return Quaternion(-X, -Y, -Z, -W);
}

FORCEINLINE constexpr Quaternion Quaternion::operator*=(const Quaternion & InQuaternion)
{
	Vector3 v1(X, Y, Z), v2(InQuaternion.X, InQuaternion.Y, InQuaternion.Z);
	W = W * InQuaternion.W - v1.Dot(v2);
	Vector3 v = v2 * W + v1 * InQuaternion.W + v1.Cross(v2);
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	return *this;
}

FORCEINLINE constexpr Vector3 Quaternion::operator*(const Vector3& InVector) const
{
	return RotateVector(InVector);
}

FORCEINLINE Quaternion Quaternion::Slerp(const Quaternion & InQuaternion1, const Quaternion & InQuaternion2, float InRatio)
{
	Quaternion q1 = InQuaternion1, q2 = InQuaternion2;

	// 사원수의 내적 구하기
	float dot = q1.X * q2.X + q1.Y * q2.Y + q1.Z * q2.Z + q1.W * q2.W;

	// 내적 값이 0보다 작으면 최단거리를 사용하도록 방향을 전환
	if (dot < 0.0f) {
		q1 = -q1;
		dot = -dot;
	}

	float alpha = 1.f, beta = 0.f;
	// 두 사원수의 사잇각이 작으면 선형 보간으로 수행
	if (dot > 0.9995f)
	{
		alpha = 1.0f - InRatio;
		beta = InRatio;
	}
	else
	{
		const float theta = acosf(dot);
		const float invSin = 1.f / sinf(theta);
		alpha = sinf((1.f - InRatio) * theta) * invSin;
		beta = sinf(InRatio * theta) * invSin;
	}

	Quaternion result;
	result.X = alpha * q1.X + beta * q2.X;
	result.Y = alpha * q1.Y + beta * q2.Y;
	result.Z = alpha * q1.Z + beta * q2.Z;
	result.W = alpha * q1.W + beta * q2.W;

	return result;
}

FORCEINLINE constexpr Vector3 Quaternion::RotateVector(const Vector3& InVector) const
{
	Vector3 q(X, Y, Z);
	Vector3 t = q.Cross(InVector) * 2.f;
	Vector3 result = InVector + (t * W) + q.Cross(t);
	return result;
}

FORCEINLINE void Quaternion::Normalize()
{
	const float squareSum = X * X + Y * Y + Z * Z + W * W;

	if (squareSum >= SMALL_NUMBER)
	{
		const float scale = 1.f / sqrtf(squareSum);

		X *= scale;
		Y *= scale;
		Z *= scale;
		W *= scale;
	}
	else
	{
		*this = Quaternion::Identity;
	}
}

FORCEINLINE Rotator Quaternion::ToRotator() const
{
	Rotator result;
	float sinrCosp = 2 * (W * Z + X * Y);
	float cosrCosp = 1 - 2 * (Z * Z + X * X);
	result.Roll = Math::Rad2Deg(atan2f(sinrCosp, cosrCosp));

	float pitchTest = W * X - Y * Z;
	float asinThreshold = 0.4999995f;
	float sinp = 2 * pitchTest;
	if (pitchTest < -asinThreshold)
	{
		result.Pitch = -90.f;
	}
	else if (pitchTest > asinThreshold)
	{
		result.Pitch = 90.f;
	}
	else
	{
		result.Pitch = Math::Rad2Deg(asinf(sinp));
	}

	float sinyCosp = 2 * (W * Y + X * Z);
	float cosyCosp = 1.f - 2 * (X * X + Y * Y);
	result.Yaw = Math::Rad2Deg(atan2f(sinyCosp, cosyCosp));
	
	return result;
}

}