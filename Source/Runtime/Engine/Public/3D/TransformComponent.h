#pragma once

namespace CK
{
namespace DDD
{

class TransformComponent
{
public:
	TransformComponent() = default;
	TransformComponent(const Transform& InLocalTransform) : _LocalTransform(InLocalTransform) 
	{ 
		UpdateWorld(); 
	}

public: // 로컬 트랜스폼 관련 함수
	FORCEINLINE Transform& GetLocalTransform() { return _LocalTransform; }
	FORCEINLINE void SetLocalTransform(const Transform& InTransform) { _LocalTransform = InTransform; UpdateWorld(); }
	FORCEINLINE const Transform& GetLocalTransform() const { return _LocalTransform; }

	FORCEINLINE void SetLocalPosition(const Vector3& InPosition);
	FORCEINLINE void AddLocalPosition(const Vector3& InDeltaPosition);
	FORCEINLINE void AddLocalYawRotation(float InDegree);
	FORCEINLINE void AddLocalRollRotation(float InDegree);
	FORCEINLINE void AddLocalPitchRotation(float InDegree);
	FORCEINLINE void SetLocalRotation(const Rotator& InRotator);
	FORCEINLINE void SetLocalRotation(const Quaternion& InQuaternion);
	FORCEINLINE void SetLocalRotation(const Matrix3x3& InMatrix);
	FORCEINLINE void SetLocalScale(const Vector3& InScale);

	FORCEINLINE Vector3 GetLocalPosition() const { return _LocalTransform.GetPosition(); }
	FORCEINLINE Rotator GetLocalRotator() const { return _LocalTransform.GetRotation().ToRotator(); }
	FORCEINLINE Quaternion GetLocalRotation() const { return _LocalTransform.GetRotation(); }
	FORCEINLINE Vector3 GetLocalScale() const { return _LocalTransform.GetScale(); }

	FORCEINLINE Vector3 GetLocalX() const { return _LocalTransform.GetXAxis(); }
	FORCEINLINE Vector3 GetLocalY() const { return _LocalTransform.GetYAxis(); }
	FORCEINLINE Vector3 GetLocalZ() const { return _LocalTransform.GetZAxis(); }
	FORCEINLINE Matrix4x4 GetLocalMatrix() const { return _LocalTransform.GetMatrix(); }

public: // 월드 트랜스폼 관련 함수
	FORCEINLINE Transform& GetWorldTransform() { return _WorldTransform; }
	FORCEINLINE void SetWorldTransform(const Transform& InTransform) { _WorldTransform = InTransform; UpdateLocal(); }
	FORCEINLINE const Transform& GetWorldTransform() const { return _WorldTransform; }

	FORCEINLINE void SetWorldPosition(const Vector3& InPosition);
	FORCEINLINE void AddWorldPosition(const Vector3& InDeltaPosition);
	FORCEINLINE void AddWorldYawRotation(float InDegree);
	FORCEINLINE void AddWorldRollRotation(float InDegree);
	FORCEINLINE void AddWorldPitchRotation(float InDegree);
	FORCEINLINE void SetWorldRotation(const Rotator& InRotator);
	FORCEINLINE void SetWorldRotation(const Matrix3x3& InMatrix);
	FORCEINLINE void SetWorldRotation(const Quaternion& InQuaternion);
	FORCEINLINE void SetWorldScale(const Vector3& InScale);

	FORCEINLINE Vector3 GetWorldPosition() const { return _WorldTransform.GetPosition(); }
	FORCEINLINE Rotator GetWorldRotator() const { return _WorldTransform.GetRotation().ToRotator(); }
	FORCEINLINE Quaternion GetWorldRotation() const { return _WorldTransform.GetRotation(); }
	FORCEINLINE Vector3 GetWorldScale() const { return _WorldTransform.GetScale(); }

	FORCEINLINE Matrix4x4 GetWorldMatrix() const { return _WorldTransform.GetMatrix(); }

public: // 계층 구조 관련 함수
	bool SetRoot();
	bool RemoveFromParent();
	TransformComponent& GetRoot();
	bool SetParent(TransformComponent& InTransform);

	FORCEINLINE bool HasParent() const { return _ParentPtr != nullptr; }
	std::vector<TransformComponent*>& GetChildren() { return _ChildrenPtr; }
	std::vector<TransformComponent*>::const_iterator ChildBegin() const { return _ChildrenPtr.begin(); }
	std::vector<TransformComponent*>::const_iterator ChildEnd() const { return _ChildrenPtr.end(); }

private: // 내부에서만 호출하는 함수
	FORCEINLINE TransformComponent* GetParentPtr() const { return _ParentPtr; }
	void UpdateLocal();
	void UpdateWorld();
	void UpdateChildrenWorld();

private: // 계층 구조를 위한 변수
	Transform _LocalTransform;
	Transform _WorldTransform;

	TransformComponent* _ParentPtr = nullptr;
	std::vector<TransformComponent*> _ChildrenPtr;
};


FORCEINLINE void TransformComponent::SetLocalPosition(const Vector3& InPosition)
{
	_LocalTransform.SetPosition(InPosition);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::AddLocalPosition(const Vector3& InDeltaPosition)
{
	_LocalTransform.AddPosition(InDeltaPosition);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::AddLocalYawRotation(float InDegree)
{
	_LocalTransform.AddYawRotation(InDegree);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::AddLocalRollRotation(float InDegree)
{
	_LocalTransform.AddRollRotation(InDegree);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::AddLocalPitchRotation(float InDegree)
{
	_LocalTransform.AddPitchRotation(InDegree);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::SetLocalRotation(const Rotator& InRotator)
{
	_LocalTransform.SetRotation(InRotator);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::SetLocalRotation(const Matrix3x3& InMatrix)
{
	_LocalTransform.SetRotation(InMatrix);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::SetLocalRotation(const Quaternion& InQuaternion)
{
	_LocalTransform.SetRotation(InQuaternion);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::SetLocalScale(const Vector3& InScale)
{
	_LocalTransform.SetScale(InScale);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::SetWorldPosition(const Vector3& InPosition)
{
	_WorldTransform.SetPosition(InPosition);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::AddWorldPosition(const Vector3& InDeltaPosition)
{
	_WorldTransform.AddPosition(InDeltaPosition);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::AddWorldYawRotation(float InDegree)
{
	_WorldTransform.AddYawRotation(InDegree);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::AddWorldRollRotation(float InDegree)
{
	_WorldTransform.AddRollRotation(InDegree);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::AddWorldPitchRotation(float InDegree)
{
	_WorldTransform.AddPitchRotation(InDegree);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::SetWorldRotation(const Rotator& InRotator)
{
	_WorldTransform.SetRotation(InRotator);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::SetWorldRotation(const Matrix3x3& InMatrix)
{
	_WorldTransform.SetRotation(InMatrix);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::SetWorldRotation(const Quaternion& InQuaternion)
{
	_WorldTransform.SetRotation(InQuaternion);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::SetWorldScale(const Vector3& InScale)
{
	_WorldTransform.SetScale(InScale);
	UpdateLocal();
}


}
}
