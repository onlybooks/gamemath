
#include "Precompiled.h"
using namespace CK;

Transform::Transform(const Matrix4x4& InMatrix)
{
	// 스케일 회전 행렬만 분리
	Matrix3x3 rotScaleMatrix = InMatrix.ToMatrix3x3();
	Position = InMatrix[3].ToVector3();

	// 크기 행렬부터 구한다. 
	Scale = Vector3::Zero;
	const float squareSumX = rotScaleMatrix[0].SizeSquared();
	const float squareSumY = rotScaleMatrix[1].SizeSquared();
	const float squareSumZ = rotScaleMatrix[2].SizeSquared();
	if (squareSumX > SMALL_NUMBER) { Scale.X = sqrtf(squareSumX); }
	if (squareSumY > SMALL_NUMBER) { Scale.Y = sqrtf(squareSumY); }
	if (squareSumZ > SMALL_NUMBER) { Scale.Z = sqrtf(squareSumZ); }

	// 크기 요소를 나눠 직교 정방 행렬을 구한다.
	rotScaleMatrix[0] /= squareSumX;
	rotScaleMatrix[1] /= squareSumY;
	rotScaleMatrix[2] /= squareSumZ;

	// 사원수로 변환한다.
	Rotation = Quaternion(rotScaleMatrix);
}

