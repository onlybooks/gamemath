
#include "Precompiled.h"
using namespace CK;

//const float Math::PI = 3.14159265358979323846f;
//const float Math::TwoPI = 2.f * PI;
//const float Math::HalfPI = 1.57079632679f;
//const float Math::InvPI = 0.31830988618f;
//const int Math::IntMin = (-2147483647 - 1);
//const int Math::IntMax = 2147483647;
//
//const std::string Math::InvalidHashName("!@CK_INVALIDHASH#$");
const std::size_t Math::InvalidHash = std::hash<std::string>()(InvalidHashName);

