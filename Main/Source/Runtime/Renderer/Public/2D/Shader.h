
#pragma once

namespace CK
{
namespace DD
{

// 정점 변환 코드
FORCEINLINE void VertexShader2D(std::vector<Vertex2D>& InVertices, const Matrix3x3& InMatrix)
{
	// 위치 값에 최종 행렬을 적용해 변환
	for (Vertex2D& v : InVertices)
	{
		v.Position = InMatrix * v.Position;
	}
}

// 픽셀 변환 코드
FORCEINLINE LinearColor FragmentShader2D(LinearColor& InColor, const LinearColor& InColorParam)
{
	return InColor * InColorParam;
}

}
}