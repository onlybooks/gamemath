
#include "Precompiled.h"
using namespace CK::DD;

void Mesh::CalculateBounds()
{
	_CircleBound = Circle(_Vertices);
	_RectBound = Rectangle(_Vertices);
}
