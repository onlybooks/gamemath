#pragma once

namespace CK
{
namespace DDD
{

/// <summary>
/// 큐브 메시 정보
/// </summary>

constexpr static std::array<Vector3, 24> cubeMeshPositions = {
	Vector3(-1.f, -1.f, -1.f), Vector3(-1.f, -1.f, 1.f), Vector3(-1.f, 1.f, 1.f), Vector3(-1.f, 1.f, -1.f),
	Vector3(-1.f, -1.f, 1.f), Vector3(-1.f, 1.f, 1.f), Vector3(1.f, 1.f, 1.f), Vector3(1.f, -1.f, 1.f),
	Vector3(-1.f, -1.f, -1.f), Vector3(-1.f, 1.f, -1.f), Vector3(1.f, 1.f, -1.f), Vector3(1.f, -1.f, -1.f),
	Vector3(1.f, -1.f, -1.f), Vector3(1.f, -1.f, 1.f), Vector3(1.f, 1.f, 1.f), Vector3(1.f, 1.f, -1.f),
	Vector3(-1.f, 1.f, -1.f), Vector3(1.f, 1.f, -1.f), Vector3(1.f, 1.f, 1.f), Vector3(-1.f, 1.f, 1.f),
	Vector3(-1.f, -1.f, -1.f), Vector3(1.f, -1.f, -1.f), Vector3(1.f, -1.f, 1.f), Vector3(-1.f, -1.f, 1.f)
};

constexpr static std::array<size_t, 36> cubeMeshIndice = {
	0, 1, 2, 0, 2, 3, // Right
	4, 6, 5, 4, 7, 6, // Front
	8, 9, 10, 8, 10, 11, // Back
	12, 14, 13, 12, 15, 14, // Left
	16, 18, 17, 16, 19, 18, // Top
	20, 21, 22, 20, 22, 23  // Bottom
};

/// <summary>
/// 화살표 메시 정보
/// </summary>

constexpr static std::array<Vector3, 29> arrowPositions = {
	// Shaft
	Vector3(-0.2f, -0.2f, 0.f), Vector3(-0.2f, -0.2f, 0.75f), Vector3(-0.2f, 0.2f, 0.75f), Vector3(-0.2f, 0.2f, 0.f),
	Vector3(-0.2f, -0.2f, 0.75f), Vector3(-0.2f, 0.2f, 0.75f), Vector3(0.2f, 0.2f, 0.75f), Vector3(0.2f, -0.2f, 0.75f),
	Vector3(-0.2f, -0.2f, 0.f), Vector3(-0.2f, 0.2f, 0.f), Vector3(0.2f, 0.2f, 0.f), Vector3(0.2f, -0.2f, 0.f),
	Vector3(0.2f, -0.2f, 0.f), Vector3(0.2f, -0.2f, 0.75f), Vector3(0.2f, 0.2f, 0.75f), Vector3(0.2f, 0.2f, 0.f),
	Vector3(-0.2f, 0.2f, 0.f), Vector3(0.2f, 0.2f, 0.f), Vector3(0.2f, 0.2f, 0.75f), Vector3(-0.2f, 0.2f, 0.75f),
	Vector3(-0.2f, -0.2f, 0.f), Vector3(0.2f, -0.2f, 0.f), Vector3(0.2f, -0.2f, 0.75f), Vector3(-0.2f, -0.2f, 0.75f),

	// Head
	Vector3(-0.4f, -0.4f, 0.75f), Vector3(-0.4f, 0.4f, 0.75f), Vector3(0.4f, 0.4f, 0.75f), Vector3(0.4f, -0.4f, 0.75f),
	Vector3(0.f, 0.f, 1.f)
};

constexpr static std::array<size_t, 54> arrowIndice = {
	// Shaft
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	8, 9, 10, 8, 10, 11,
	12, 14, 13, 12, 15, 14,
	16, 18, 17, 16, 19, 18,
	20, 21, 22, 20, 22, 23,
	// Head
	24, 26, 25, 24, 27, 26,
	24, 28, 27, 24, 25, 28, 25, 26, 28, 26, 27, 28
};

}
}