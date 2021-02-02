#pragma once

namespace CK
{
namespace DDD
{

class CameraObject
{
public:
	CameraObject() = default;
	~CameraObject() { }

public:
	// 트랜스폼
	TransformComponent& GetTransform() { return _Transform; }
	const TransformComponent& GetTransform() const { return _Transform; }

	// 카메라 값을 가져오는 함수
	float GetFOV() const { return _FOV; }
	const ScreenPoint& GetViewportSize() const { return _ViewportSize; }

	// 카메라 값을 설정하는 함수
	void SetFOV(float InFOV) { _FOV = InFOV; }
	void SetViewportSize(const ScreenPoint& InViewportSize) { _ViewportSize = InViewportSize; }
	void SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp = Vector3::UnitY);

	// 행렬 생성
	FORCEINLINE void GetViewAxes(Vector3& OutViewX, Vector3& OutViewY, Vector3& OutViewZ) const;
	FORCEINLINE Matrix4x4 GetViewMatrix() const;
	FORCEINLINE Matrix4x4 GetViewMatrixRotationOnly() const;
	FORCEINLINE Matrix4x4 GetPerspectiveMatrix() const;

private:
	TransformComponent _Transform;
	ScreenPoint _ViewportSize;
	float _FOV = 60.f;
};

FORCEINLINE void CameraObject::GetViewAxes(Vector3& OutViewX, Vector3& OutViewY, Vector3& OutViewZ) const
{
	// 로컬 Z축 값의 반대 방향을 뷰 공간의 Z축으로 설정
	OutViewZ = -_Transform.GetLocalZ();
	OutViewY = _Transform.GetLocalY();
	OutViewX = -_Transform.GetLocalX();
}

FORCEINLINE Matrix4x4 CameraObject::GetViewMatrix() const
{
	Vector3 viewX, viewY, viewZ;
	GetViewAxes(viewX, viewY, viewZ);
	Vector3 pos = _Transform.GetPosition();

	return Matrix4x4(
		Vector4(Vector3(viewX.X, viewY.X, viewZ.X), false),
		Vector4(Vector3(viewX.Y, viewY.Y, viewZ.Y), false),
		Vector4(Vector3(viewX.Z, viewY.Z, viewZ.Z), false),
		Vector4(-viewX.Dot(pos), -viewY.Dot(pos), -viewZ.Dot(pos), 1.f)
	);
}

FORCEINLINE Matrix4x4 CameraObject::GetViewMatrixRotationOnly() const
{
	Vector3 viewX, viewY, viewZ;
	GetViewAxes(viewX, viewY, viewZ);

	return Matrix4x4(
		Vector4(Vector3(viewX.X, viewY.X, viewZ.X), false),
		Vector4(Vector3(viewX.Y, viewY.Y, viewZ.Y), false),
		Vector4(Vector3(viewX.Z, viewY.Z, viewZ.Z), false),
		Vector4::UnitW
	);
}

FORCEINLINE Matrix4x4 CameraObject::GetPerspectiveMatrix() const
{
	float invA = 1.f / _ViewportSize.AspectRatio();
	float d = 1.f / tanf(Math::Deg2Rad(_FOV) * 0.5f);

	return Matrix4x4(
		Vector4::UnitX * invA * d,
		Vector4::UnitY * d,
		Vector4(0.f, 0.f, -1.f, 0.f),
		Vector4(0.f, 0.f, 0, 1.f)
	);
}

}
}