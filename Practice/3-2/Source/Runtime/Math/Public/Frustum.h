#pragma once

namespace CK
{

struct Frustum
{
	// 생성자
	FORCEINLINE constexpr Frustum() = default;
	FORCEINLINE constexpr Frustum(const std::array<Plane, 6>& InPlanes);

	// 멤버함수 
	FORCEINLINE constexpr BoundCheckResult CheckBound(const Vector3& InPoint) const;
	FORCEINLINE constexpr BoundCheckResult CheckBound(const Sphere& InSphere) const;
	FORCEINLINE constexpr BoundCheckResult CheckBound(const Box& InBox) const;
	FORCEINLINE constexpr bool IsIntersect(const Box& InBox) const;

	// 멤버변수
	std::array<Plane, 6> Planes; // Y+, Y-, X+, X-, Z+, Z- 순으로 저장
};

FORCEINLINE constexpr Frustum::Frustum(const std::array<Plane, 6>& InPlanes) : Planes(InPlanes)
{
}

FORCEINLINE constexpr BoundCheckResult Frustum::CheckBound(const Vector3& InPoint) const
{
	for (const auto& p : Planes)
	{
		if (p.IsOutside(InPoint))
		{
			return BoundCheckResult::Outside;
		}
		else if (Math::EqualsInTolerance(p.Distance(InPoint), 0.f))
		{
			return BoundCheckResult::Intersect;
		}
	
	}

	return BoundCheckResult::Inside;
}

FORCEINLINE constexpr BoundCheckResult Frustum::CheckBound(const Sphere& InSphere) const
{
	for (const auto& p : Planes)
	{
		if (p.Distance(InSphere.Center) > InSphere.Radius)
		{
			return BoundCheckResult::Outside;
		}
		else if (Math::Abs(p.Distance(InSphere.Center)) <= InSphere.Radius)
		{
			return BoundCheckResult::Intersect;
		}

	}

	return BoundCheckResult::Inside;
}

FORCEINLINE constexpr BoundCheckResult Frustum::CheckBound(const Box& InBox) const
{
	for (const auto& p : Planes)
	{
		Vector3 pPoint = InBox.Min, nPoint = InBox.Max;
		if (p.Normal.X >= 0.f) { pPoint.X = InBox.Max.X; nPoint.X = InBox.Min.X; }
		if (p.Normal.Y >= 0.f) { pPoint.Y = InBox.Max.Y; nPoint.Y = InBox.Min.Y; }
		if (p.Normal.Z >= 0.f) { pPoint.Z = InBox.Max.Z; nPoint.Z = InBox.Min.Z; }

		if (p.Distance(nPoint) > 0.f)
		{
			return BoundCheckResult::Outside;
		}
		if (p.Distance(nPoint) <= 0.f && p.Distance(pPoint) >= 0.f)
		{
			return BoundCheckResult::Intersect;
		}
	}

	return BoundCheckResult::Inside;
}

FORCEINLINE constexpr bool Frustum::IsIntersect(const Box& InBox) const
{
	for (const auto& p : Planes)
	{
		Vector3 pVertex = InBox.Min, nVertex = InBox.Max;
		if (p.Normal.X >= 0.f) { pVertex.X = InBox.Max.X; nVertex.X = InBox.Min.X; }
		if (p.Normal.Y >= 0.f) { pVertex.Y = InBox.Max.Y; nVertex.Y = InBox.Min.Y; }
		if (p.Normal.Z >= 0.f) { pVertex.Z = InBox.Max.Z; nVertex.Z = InBox.Min.Z; }

		if (p.Distance(nVertex) <= 0.f && p.Distance(pVertex) >= 0.f)
		{
			return true;
		}
	}

	return false;
}

}