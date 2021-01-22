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
	void SetParent(GameObject& InGameObject) { _Transform.SetParent(InGameObject.GetTransform()); }

	// 카메라 값을 가져오는 함수
	float GetFOV() const { return _FOV; }
	float GetNearZ() const { return _NearZ; }
	float GetFarZ() const { return _FarZ; }
	const ScreenPoint& GetViewportSize() const { return _ViewportSize; }

	// 카메라 값을 설정하는 함수
	void SetLookAtRotation(const GameObject& InGameObject, const Vector3& InUp = Vector3::UnitY);
	void SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp = Vector3::UnitY);
	void SetFOV(float InFOV) { _FOV = InFOV; }
	void SetNearZ(float InNearZ) { _NearZ = InNearZ; }
	void SetFarZ(float InFarZ) { _FarZ = InFarZ; }
	void SetViewportSize(const ScreenPoint& InViewportSize) { _ViewportSize = InViewportSize; }

	// 행렬 생성
	FORCEINLINE void GetViewAxes(Vector3& OutViewX, Vector3& OutViewY, Vector3& OutViewZ) const;
	FORCEINLINE Matrix4x4 GetViewMatrix() const;
	FORCEINLINE Matrix4x4 GetViewMatrixRotationOnly() const;
	FORCEINLINE Matrix4x4 GetPerspectiveMatrix() const;
	FORCEINLINE Matrix4x4 GetPerspectiveViewMatrix() const;

private:
	TransformComponent _Transform;

	float _FOV = 60.f;
	float _NearZ = 5.5f;
	float _FarZ = 5000.f;
	ScreenPoint _ViewportSize;
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
	Vector3 pos = _Transform.GetWorldPosition();

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
	// 투영 행렬. 깊이 값의 범위는 -1~1
	float invA = 1.f / _ViewportSize.AspectRatio();
	float d = 1.f / tanf(Math::Deg2Rad(_FOV) * 0.5f);

	// 근평면과 원평면에 반대 부호를 붙여서 계산
	float invNF = 1.f / (_NearZ - _FarZ);
	float k = (_FarZ + _NearZ) * invNF;
	float l = 2.f * _FarZ * _NearZ * invNF;
	return Matrix4x4(
		Vector4::UnitX * invA * d,
		Vector4::UnitY * d,
		Vector4(0.f, 0.f, k, -1.f),
		Vector4(0.f, 0.f, l, 0.f));
}

FORCEINLINE Matrix4x4 CameraObject::GetPerspectiveViewMatrix() const
{
	// 뷰 행렬 관련 요소
	Vector3 viewX, viewY, viewZ;
	GetViewAxes(viewX, viewY, viewZ);
	Vector3 pos = _Transform.GetWorldPosition();
	float zPos = viewZ.Dot(pos);

	// 투영 행렬 관련 요소
	float invA = 1.f / _ViewportSize.AspectRatio();
	float d = 1.f / tanf(Math::Deg2Rad(_FOV) * 0.5f);
	float dx = invA * d;
	float invNF = 1.f / (_NearZ - _FarZ);
	float k = (_FarZ + _NearZ) * invNF;
	float l = 2.f * _FarZ * _NearZ * invNF;

	return Matrix4x4(
		Vector4(dx * viewX.X, d * viewY.X, k * viewZ.X, -viewZ.X),
		Vector4(dx * viewX.Y, d * viewY.Y, k * viewZ.Y, -viewZ.Y),
		Vector4(dx * viewX.Z, d * viewY.Z, k * viewZ.Z, -viewZ.Z),
		Vector4(-dx * viewX.Dot(pos), -d * viewY.Dot(pos), -k * zPos + l, zPos)
	);
}

}
}