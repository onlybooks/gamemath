#pragma once

namespace CK
{
namespace DD
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
	const ScreenPoint& GetViewportSize() const { return _ViewportSize; }

	// 카메라 값을 설정하는 함수
	void SetViewportSize(const ScreenPoint & InViewportSize) { _ViewportSize = InViewportSize; }

	// 행렬 생성
	FORCEINLINE Matrix3x3 GetViewMatrix() const;

private:
	TransformComponent _Transform;
	ScreenPoint _ViewportSize;
};

FORCEINLINE Matrix3x3 CameraObject::GetViewMatrix() const
{
	return Matrix3x3(Vector3::UnitX, Vector3::UnitY, Vector3(-_Transform.GetPosition()));
}

}
}