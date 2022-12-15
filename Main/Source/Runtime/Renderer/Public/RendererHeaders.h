#pragma once

#include <functional>

#include "2D/Vertex.h"
#include "2D/Shader.h"
#include "3D/Vertex.h"
#include "3D/Shader.h"
#include "3D/PerspectiveTest.h"

#include "RendererInterface.h"

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#include "Windows/WindowsGDI.h"
#include "Windows/WindowsRSI.h"
#endif

using namespace CK;
