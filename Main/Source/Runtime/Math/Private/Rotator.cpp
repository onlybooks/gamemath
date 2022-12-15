
#include "Precompiled.h"
using namespace CK;

const Rotator Rotator::Identity(0.f, 0.f, 0.f);

std::string Rotator::ToString() const
{
	char result[64];
	std::snprintf(result, sizeof(result), "(Y : %.1f, R: %.1f, P : %.1f)", Yaw, Roll, Pitch);
	return result;
}