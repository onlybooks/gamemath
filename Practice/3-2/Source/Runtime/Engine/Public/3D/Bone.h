#pragma once

namespace CK
{
namespace DDD
{

class Bone
{
public:
	Bone() = default;
	Bone(const std::string& InName, const Transform& InTransform) : _Name(InName), _ParentName(InName)
	{
		_Hash = std::hash<std::string>()(_Name);
		_BindPose = InTransform;
		_Transform.SetLocalTransform(InTransform);
	}

public:
	// Ʈ������
	TransformComponent& GetTransform() { return _Transform; }
	const TransformComponent& GetTransform() const { return _Transform; }
	const Transform& GetBindPose() const { return _BindPose; }
	void SetParent(Bone& InBone) 
	{
		_ParentName = InBone.GetName();
		_Transform.SetParent(InBone.GetTransform());
	}

	// Ű ����
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }
	bool HasParent() const { return _Name.compare(_ParentName) != 0; }
	const std::string& GetParentName() const { return _ParentName; }

private:
	std::size_t _Hash = 0;
	std::string _Name;

	// ���� Ʈ������ ����
	TransformComponent _Transform;

	// ���� Ʈ������ ����
	Transform _BindPose;

	// �θ� ����
	std::string _ParentName;
};

}
}