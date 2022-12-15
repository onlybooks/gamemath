
#pragma once

namespace CK
{
namespace DDD
{

// 정점 변환 코드
FORCEINLINE void VertexShader3D(std::vector<Vertex3D>& InVertices, const Matrix4x4& InMatrix)
{
	// 위치 값에 최종 행렬을 적용해 변환
	for (Vertex3D& v : InVertices)
	{
		v.Position = InMatrix * v.Position;
	}
}

// 픽셀 변환 코드
FORCEINLINE LinearColor FragmentShader3D(LinearColor& InColor, const LinearColor& InColorParam)
{
	return InColor * InColorParam;
}


}
}