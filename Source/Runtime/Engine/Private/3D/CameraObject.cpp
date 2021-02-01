
#include "Precompiled.h"
using namespace CK::DDD;

void CameraObject::SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp)
{
	Vector3 localX, localY, localZ;

	// 단위 Z축 생성
	localZ = (InTargetPosition - _Transform.GetPosition()).GetNormalize();
	// 시선 방향과 월드 Y축이 평행한 경우
	if (Math::Abs(localZ.Y) >= (1.f - SMALL_NUMBER))
	{
		// 특이 상황에서 로컬 X 좌표 값을 임의로 지정.
		localX = Vector3::UnitX;
	}
	else
	{
		localX = InUp.Cross(localZ).GetNormalize();
	}

	// 두 벡터가 직교하고 크기가 1이면 결과는 1을 보장
	localY = localZ.Cross(localX);

	_Transform.SetLocalAxes(localX, localY, localZ);
}
