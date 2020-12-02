#include "Precompiled.h"
using namespace CK::DDD;

bool TransformComponent::RemoveFromParent()
{
	if (!HasParent())
	{
		return true;
	}

	TransformComponent& parent = *GetParentPtr();
	auto it = std::find(parent.ChildBegin(), parent.ChildEnd(), this);
	if (it != parent.ChildEnd())
	{
		// 오류 발생.
		return false;
	}

	// 부모 트랜스폼에서 자식 정보를 제거
	parent.GetChildren().erase(it);

	// 자신에게서 부모 정보를 제거
	_ParentPtr = nullptr;
	return true;
}

bool TransformComponent::SetRoot()
{
	if (!RemoveFromParent())
	{
		return false;
	}

	// 로컬 정보를 월드 정보로 변경
	UpdateLocal();
	return true;
}

FORCEINLINE TransformComponent& TransformComponent::GetRoot()
{
	TransformComponent* parent = nullptr;
	TransformComponent* current = this;
	while ((parent = current->GetParentPtr()) != nullptr)
	{
		current = parent;
	}

	return *current;
}

bool TransformComponent::SetParent(TransformComponent& InTransform)
{
	// 현재 노드를 부모로부터 분리
	if (!SetRoot())
	{
		return false;
	}

	// 새로운 부모의 자식으로 등록. 이미 있는 경우에는 문제가 있는 상황.
	auto it = std::find(InTransform.ChildBegin(), InTransform.ChildEnd(), this);
	if (it != InTransform.ChildEnd())
	{
		return false;
	}

	// 새로운 트랜스폼 노드로 부모 재설정
	InTransform.GetChildren().emplace_back(this);
	_ParentPtr = &InTransform;
	TransformComponent& newParent = *_ParentPtr;

	// 자신의 로컬과 모든 자식의 월드를 업데이트한다.
	UpdateLocal();

	return true;
}

// 부모의 월드 정보와 자신의 월드 정보를 비교해 로컬 정보를 갱신
void TransformComponent::UpdateLocal()
{
	if (HasParent())
	{
		const TransformComponent& parent = *GetParentPtr();
		_LocalTransform = _WorldTransform.WorldToLocal(parent.GetWorldTransform());
	}
	else
	{
		_LocalTransform = _WorldTransform;
	}
}

// 부모의 월드 정보를 기준으로 자신의 로컬 정보를 추가해 월드 정보를 갱신
void TransformComponent::UpdateWorld()
{
	// 자신의 월드 정보 업데이트
	if (HasParent())
	{
		const TransformComponent& parent = *GetParentPtr();
		_WorldTransform = _LocalTransform.LocalToWorld(parent.GetWorldTransform());
	}
	else
	{
		_WorldTransform = _LocalTransform;
	}

	// 자신의 월드 정보 변경 시 자식의 월드 정보도 함께 업데이트
	UpdateChildrenWorld();
}

void TransformComponent::UpdateChildrenWorld()
{
	for (auto it = ChildBegin(); it != ChildEnd(); ++it)
	{
		(*it)->UpdateWorld();
	}
}

