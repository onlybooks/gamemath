#pragma once

namespace CK
{
namespace DDD
{

class TransformComponent
{
public:
	TransformComponent() = default;

public:
	void SetPosition(const Vector3 & InPosition) { _Position = InPosition; }
	void AddPosition(const Vector3 & InDeltaPosition) { _Position += InDeltaPosition; }
	void AddYawRotation(float InDegree)
	{
		Rotator r = _Rotation.ToRotator();
		r.Yaw += InDegree;
		r.Clamp();
		_Rotation = Quaternion(r);
	}
	void AddRollRotation(float InDegree)
	{
		Rotator r = _Rotation.ToRotator();
		r.Roll += InDegree;
		r.Clamp();
		_Rotation = Quaternion(r);
	}
	void AddPitchRotation(float InDegree)
	{
		Rotator r = _Rotation.ToRotator();
		r.Pitch += InDegree;
		r.Clamp();
		_Rotation = Quaternion(r);
	}
	constexpr void SetRotation(const Rotator& InRotator) { _Rotation = Quaternion(InRotator); }
	void SetRotation(const Matrix3x3& InMatrix) { _Rotation = Quaternion(InMatrix); }
	constexpr void SetRotation(const Quaternion& InQuaternion) { _Rotation = InQuaternion; }
	void SetScale(const Vector3 & InScale) { _Scale = InScale; }

	FORCEINLINE constexpr Vector3 GetPosition() const { return _Position; }
	FORCEINLINE constexpr Quaternion GetRotation() const { return _Rotation; }
	FORCEINLINE constexpr Vector3 GetScale() const { return _Scale; }

	FORCEINLINE Matrix4x4 GetModelingMatrix() const;
	FORCEINLINE constexpr Vector3 GetLocalX() const { return _Rotation * Vector3::UnitX; }
	FORCEINLINE constexpr Vector3 GetLocalY() const { return _Rotation * Vector3::UnitY; }
	FORCEINLINE constexpr Vector3 GetLocalZ() const { return _Rotation * Vector3::UnitZ; }

private:
	FORCEINLINE void Update();

	Vector3 _Position = Vector3::Zero;
	Quaternion _Rotation;
	Vector3 _Scale = Vector3::One;
};

FORCEINLINE Matrix4x4 TransformComponent::GetModelingMatrix() const
{
	return Matrix4x4(
		Vector4(GetLocalX() * _Scale.X, false),
		Vector4(GetLocalY() * _Scale.Y, false),
		Vector4(GetLocalZ() * _Scale.Z, false),
		Vector4(_Position, true)
	);
}

}
}
