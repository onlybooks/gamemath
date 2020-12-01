#pragma once

namespace CK
{
namespace DD
{

class Mesh
{
public:
	Mesh() = default;

	// 속성 관련 함수
	void SetMeshType(const MeshType & _InMeshType) { _MeshType = _InMeshType; }
	FORCEINLINE bool HasColor() const { return _Colors.size() > 0; }
	FORCEINLINE bool HasUV() const { return _UVs.size() > 0; }
	std::vector<Vector2>& GetVertices() { return _Vertices; }
	const std::vector<Vector2>& GetVertices() const { return _Vertices; }
	std::vector<size_t>& GetIndices() { return _Indices; }
	const std::vector<size_t>& GetIndices() const { return _Indices; }
	std::vector<LinearColor>& GetColors() { return _Colors; }
	const std::vector<LinearColor>& GetColors() const { return _Colors; }
	std::vector<Vector2>& GetUVs() { return _UVs; }
	const std::vector<Vector2>& GetUVs() const { return _UVs; }

	// 바운딩 볼륨 관련 함수
	void CalculateBounds();
	const Circle& GetCircleBound() const { return _CircleBound; }
	const Rectangle& GetRectBound() const { return _RectBound; }

private:
	std::vector<Vector2> _Vertices;
	std::vector<size_t> _Indices;
	std::vector<LinearColor> _Colors;
	std::vector<Vector2> _UVs;

private:
	MeshType _MeshType = MeshType::Normal;
	Circle _CircleBound;
	Rectangle _RectBound;
};

}
}

