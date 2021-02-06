
#include "Precompiled.h"
using namespace CK::DDD;

void CameraObject::SetLookAtRotation(const GameObject& InGameObject, const Vector3& InUp)
{
	SetLookAtRotation(InGameObject.GetTransform().GetPosition(), InUp);
}

void CameraObject::SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp)
{
	_Transform.SetRotation(Quaternion(InTargetPosition - _Transform.GetPosition()));
}