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
		// ���� �߻�.
		return false;
	}

	// �θ� Ʈ���������� �ڽ� ������ �����Ѵ�.
	parent.GetChildren().erase(it);

	// �ڽſ��Լ� �θ� ������ �����Ѵ�.
	_ParentPtr = nullptr;
	return true;
}

bool TransformComponent::SetRoot()
{
	if (!RemoveFromParent())
	{
		return false;
	}

	// ���� ������ ���� ������ �����Ѵ�.
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
	// ���� ��带 �θ�κ��� �и��Ѵ�. 
	if (!SetRoot())
	{
		return false;
	}

	// ���ο� �θ��� �ڽ����� ���. �̹� �ִ� ��쿡�� ������ �ִ� ��Ȳ.
	auto it = std::find(InTransform.ChildBegin(), InTransform.ChildEnd(), this);
	if (it != InTransform.ChildEnd())
	{
		return false;
	}

	// ���ο� Ʈ������ ���� �θ� �缳��
	InTransform.GetChildren().emplace_back(this);
	_ParentPtr = &InTransform;
	TransformComponent& newParent = *_ParentPtr;

	// ���ο� �θ� ���� �ڽ��� ���� ������ ������Ʈ�Ѵ�.
	UpdateLocal();

	return true;
}

// ���� ����, Ȥ�� �θ� ����Ǹ� �̸� ������� ���� ������ ����
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

// ���� ������ ������Ʈ �Ǿ ���� ������ �ٽ� ���
void TransformComponent::UpdateWorld()
{
	// �ڽ��� ���� ���� ������Ʈ
	if (HasParent())
	{
		const TransformComponent& parent = *GetParentPtr();
		_WorldTransform = _LocalTransform.LocalToWorld(parent.GetWorldTransform());
	}
	else
	{
		_WorldTransform = _LocalTransform;
	}

	// ���� ���� ���� �� �ڽ��� ���� ������ ������Ʈ ( ���� ������ ���� ����. )
	UpdateChildrenWorld();
}

void TransformComponent::UpdateChildrenWorld()
{
	for (auto it = ChildBegin(); it != ChildEnd(); ++it)
	{
		(*it)->UpdateWorld();
	}
}

