#pragma once

namespace CK
{
namespace DDD
{

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

private:
	std::vector<Vector3> _Vertices;
	std::vector<size_t> _Indices;
	std::vector<LinearColor> _Colors;
	std::vector<Vector2> _UVs;

private:
	MeshType _MeshType = MeshType::Normal;
	Sphere _SphereBound;
	Box _BoxBound;
};

}
}