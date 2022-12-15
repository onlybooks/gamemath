#pragma once

typedef unsigned char		BYTE;		// 부호 없는 8비트
typedef unsigned int		UINT32;		// 부호 없는 32비트
typedef signed long long	INT64;	// 부호 있는 64비트.

#define SMALL_NUMBER		(1.e-8f)

enum class BoundCheckResult : UINT32
{
	Outside = 0,
	Intersect,
	Inside
};