
#include "Precompiled.h"
using namespace CK::DDD;

void Mesh::CalculateBounds()
{
	_SphereBound = Sphere(_Vertices);
	_BoxBound = Box(_Vertices);
}
