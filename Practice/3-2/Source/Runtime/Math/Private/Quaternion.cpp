
#include "Precompiled.h"
using namespace CK;

const Quaternion Quaternion::Identity(0.f, 0.f, 0.f, 1.f);

std::string Quaternion::ToString() const
{
	return ToRotator().ToString();
}