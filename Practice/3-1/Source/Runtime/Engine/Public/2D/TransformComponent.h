#pragma once

namespace CK
{
namespace DD
{ 

class TransformComponent
{
public:
	TransformComponent() = default;

public:
	// 게임 로직에서 사용할 멤버 함수
	void SetPosition(const Vector2& InPosition) { _Position = InPosition; }
	void AddPosition(const Vector2& InDeltaPosition) { _Position += InDeltaPosition; }
	void SetScale(const Vector2& InScale) { _Scale = InScale; }
	void SetRotation(float InDegree) { _Rotation = InDegree; Update(); }
	void AddRotation(float InDegree) { _Rotation += InDegree; Update(); }
	Vector2 GetPosition() const { return _Position; }
	Vector2 GetScale() const { return _Scale; }
	float GetRotation() const { return _Rotation; }
	const Vector2& GetLocalX() const { return _Right; }
	const Vector2& GetLocalY() const { return _Up; }

	// 행렬 생성 함수
	FORCEINLINE Matrix3x3 GetModelingMatrix() const;

private:
	FORCEINLINE void Update();

private:
	Vector2 _Position = Vector2::Zero;
	float _Rotation = 0.f;
	Vector2 _Scale = Vector2::One;

	Vector2 _Right = Vector2::UnitX;
	Vector2 _Up = Vector2::UnitY;
};

FORCEINLINE Matrix3x3 TransformComponent::GetModelingMatrix() const
{
	return Matrix3x3(
		Vector3(_Scale.X * _Right.X, _Scale.X * _Right.Y, 0.f),
		Vector3(_Scale.Y * _Up.X, _Scale.Y * _Up.Y, 0.f),
		Vector3(_Position.X, _Position.Y, 1.f)
	);
}

FORCEINLINE void TransformComponent::Update()
{
	float sin, cos;
	Math::GetSinCos(sin, cos, _Rotation);

	_Right = Vector2(cos, sin);
	_Up = Vector2(-sin, cos);
}

}
}