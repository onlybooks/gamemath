#pragma once

namespace CK
{
namespace DDD
{

struct Weight
{
	std::vector<std::string> Bones;
	std::vector<float> Values;
};

class Mesh
{
public:
	Mesh() = default;

	// 속성 관련 함수
	void SetMeshType(const MeshType& _InMeshType) { _MeshType = _InMeshType; }
	FORCEINLINE bool HasColor() const { return _Colors.size() > 0; }
	FORCEINLINE bool HasUV() const { return _UVs.size() > 0; }
	std::vector<Vector3>& GetVertices() { return _Vertices; }
	const std::vector<Vector3>& GetVertices() const { return _Vertices; }
	std::vector<size_t>& GetIndices() { return _Indices; }
	const std::vector<size_t>& GetIndices() const { return _Indices; }
	std::vector<LinearColor>& GetColors() { return _Colors; }
	const std::vector<LinearColor>& GetColors() const { return _Colors; }
	std::vector<Vector2>& GetUVs() { return _UVs; }
	const std::vector<Vector2>& GetUVs() const { return _UVs; }

	// 바운딩 볼륨 관련 함수
	void CalculateBounds();
	const Sphere& GetSphereBound() const { return _SphereBound; }
	const Box& GetBoxBound() const { return _BoxBound; }

	// 스켈레탈 애니메이션 관련 함수
	FORCEINLINE bool IsSkinnedMesh() const { return _MeshType == MeshType::Skinned; }
	FORCEINLINE bool HasBone(const std::string& InBoneName) const { return _Bones.find(InBoneName) != _Bones.end(); }

	std::vector<Weight>& GetWeights() { return _Weights; }
	const std::vector<Weight>& GetWeights() const { return _Weights; }

	std::vector<BYTE>& GetConnectedBones() { return _ConnectedBones; }
	const std::vector<BYTE>& GetConnectedBones() const { return _ConnectedBones; }

	Bone& GetBone(const std::string& InBoneName) { return _Bones.at(InBoneName); }
	const Bone& GetBone(const std::string& InBoneName) const { return _Bones.at(InBoneName); }
	std::unordered_map<std::string, Bone>& GetBones() { return _Bones; }
	const std::unordered_map<std::string, Bone>& GetBones() const { return _Bones; }
	const Transform& GetBindPose(const std::string& InBoneName) const { return _Bones.at(InBoneName).GetBindPose(); }

private:
	std::vector<Vector3> _Vertices;
	std::vector<size_t> _Indices;
	std::vector<LinearColor> _Colors;
	std::vector<Vector2> _UVs;

	std::vector<BYTE> _ConnectedBones;
	std::vector<Weight> _Weights;
	std::unordered_map<std::string, Bone> _Bones;

private:
	MeshType _MeshType = MeshType::Normal;
	Sphere _SphereBound;
	Box _BoxBound;
};

}
}