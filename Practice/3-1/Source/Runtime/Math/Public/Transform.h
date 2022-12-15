#pragma once

namespace CK
{

struct Transform
{
public:
	FORCEINLINE constexpr Transform() = default;
	FORCEINLINE constexpr Transform(const Vector3& InPosition) : Position(InPosition) { }
	FORCEINLINE constexpr Transform(const Vector3& InPosition, const Quaternion& InRotation) : Position(InPosition), Rotation(InRotation) { }
	FORCEINLINE constexpr Transform(const Vector3& InPosition, const Quaternion& InRotation, const Vector3& InScale) : Position(InPosition), Rotation(InRotation), Scale(InScale) { }
	Transform(const Matrix4x4& InMatrix);

public: // Ʈ������ �����Լ�
	constexpr void SetPosition(const Vector3& InPosition) { Position = InPosition; }
	constexpr void AddPosition(const Vector3& InDeltaPosition) { Position += InDeltaPosition; }
	void AddYawRotation(float InDegree)
	{
		Rotator r = Rotation.ToRotator();
		r.Yaw += InDegree;
		r.Clamp();
		Rotation = Quaternion(r);
	}
	void AddRollRotation(float InDegree)
	{ 
		Rotator r = Rotation.ToRotator();
		r.Roll += InDegree;
		r.Clamp();
		Rotation = Quaternion(r);
	}
	void AddPitchRotation(float InDegree)
	{ 
		Rotator r = Rotation.ToRotator();
		r.Pitch += InDegree;
		r.Clamp();
		Rotation = Quaternion(r);
	}

	constexpr void SetRotation(const Rotator& InRotator) { Rotation = Quaternion(InRotator); }
	void SetRotation(const Matrix3x3& InMatrix) { Rotation = Quaternion(InMatrix); }
	constexpr void SetRotation(const Quaternion& InQuaternion) { Rotation = InQuaternion; }
	constexpr void SetScale(const Vector3& InScale) { Scale = InScale; }

	FORCEINLINE constexpr Vector3 GetXAxis() const { return Rotation * Vector3::UnitX; }
	FORCEINLINE constexpr Vector3 GetYAxis() const { return Rotation * Vector3::UnitY; }
	FORCEINLINE constexpr Vector3 GetZAxis() const { return Rotation * Vector3::UnitZ; }
	constexpr Matrix4x4 GetMatrix() const;

	FORCEINLINE constexpr Vector3 GetPosition() const { return Position; }
	FORCEINLINE constexpr Quaternion GetRotation() const { return Rotation; }
	FORCEINLINE constexpr Vector3 GetScale() const { return Scale; }

	// Ʈ������ ��ȯ
	FORCEINLINE constexpr Transform Inverse() const;
	FORCEINLINE constexpr Transform LocalToWorld(const Transform& InParentWorldTransform) const;
	FORCEINLINE constexpr Transform WorldToLocal(const Transform& InParentWorldTransform) const;
	FORCEINLINE constexpr Vector3 WorldToLocalVector(const Vector3& InWorldVector) const;

private: // Ʈ�������� ���õ� ����
	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scale = Vector3::One;

};

FORCEINLINE constexpr Matrix4x4 Transform::GetMatrix() const
{
	return Matrix4x4(
		Vector4(GetXAxis() * Scale.X, false),
		Vector4(GetYAxis() * Scale.Y, false),
		Vector4(GetZAxis() * Scale.Z, false),
		Vector4(Position, true)
	);
}

FORCEINLINE constexpr Transform Transform::Inverse() const
{
	// ���� ������ ����� ���� Ʈ������ ( ����� )
	Vector3 reciprocalScale = Vector3::Zero;
	if (!Math::EqualsInTolerance(Scale.X, 0.f)) reciprocalScale.X = 1.f / Scale.X;
	if (!Math::EqualsInTolerance(Scale.Y, 0.f)) reciprocalScale.Y = 1.f / Scale.Y;
	if (!Math::EqualsInTolerance(Scale.Z, 0.f)) reciprocalScale.Z = 1.f / Scale.Z;

	Transform result;
	result.SetScale(reciprocalScale);
	result.SetRotation(Rotation.Inverse());
	result.SetPosition(result.GetScale() * (result.GetRotation() * -Position));
	return result;
}


FORCEINLINE constexpr Transform Transform::LocalToWorld(const Transform& InParentWorldTransform) const
{
	// ���� Ʈ������ ������ ������ ���
	Transform result;
	result.SetScale(InParentWorldTransform.GetScale() * GetScale());
	result.SetRotation(InParentWorldTransform.GetRotation() * GetRotation());
	result.SetPosition(InParentWorldTransform.GetPosition() + InParentWorldTransform.GetScale() * (InParentWorldTransform.GetRotation() * GetPosition()));
	return result;
}

FORCEINLINE constexpr Transform Transform::WorldToLocal(const Transform& InParentWorldTransform) const
{
	// ���� Ʈ������ ������ ������ ���
	Transform invParent = InParentWorldTransform.Inverse();

	Transform result;
	result.SetScale(invParent.GetScale() * GetScale());
	result.SetRotation(invParent.GetRotation() * GetRotation());
	result.SetPosition(invParent.GetPosition() + invParent.GetScale() * (invParent.GetRotation() * GetPosition()));
	return result;
}

FORCEINLINE constexpr Vector3 Transform::WorldToLocalVector(const Vector3& InWorldVector) const
{
	// ���� Ʈ�������� �������� ���� ���͸� ���� ���ͷ� ��ȯ
	Transform invTransform = Inverse();

	return invTransform.GetPosition() + invTransform.GetScale() * (invTransform.GetRotation() * InWorldVector);
}

}
