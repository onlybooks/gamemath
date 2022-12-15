#pragma once

namespace CK
{

struct Rotator
{
public:
	FORCEINLINE constexpr Rotator() = default;
	FORCEINLINE constexpr Rotator(float InYaw, float InRoll, float InPitch) : Yaw(InYaw), Roll(InRoll), Pitch(InPitch) { }
	FORCEINLINE void Clamp()
	{
		Yaw = GetAxisClampedValue(Yaw);
		Roll = GetAxisClampedValue(Roll);
		Pitch = GetAxisClampedValue(Pitch);
	}

	FORCEINLINE float GetAxisClampedValue(float InRotatorValue)
	{
		float angle = Math::FMod(InRotatorValue, 360.f);
		if (angle < 0.f)
		{
			angle += 360.f;
		}

		return angle;
	}

	FORCEINLINE void GetLocalAxes(Vector3& OutRight, Vector3& OutUp, Vector3& OutForward)
	{
		float cy = 0.f, sy = 0.f, cp = 0.f, sp = 0.f, cr = 0.f, sr = 0.f;
		Math::GetSinCos(sy, cy, Yaw);
		Math::GetSinCos(sp, cp, Pitch);
		Math::GetSinCos(sr, cr, Roll);

		OutRight = Vector3(cy * cr + sy * sp * sr, cp * sr, -sy * cr + cy * sp * sr);
		OutUp = Vector3(-cy * sr + sy * sp * cr, cp * cr, sy * sr + cy * sp * cr);
		OutForward = Vector3(sy * cp, -sp, cy * cp);
	}

	std::string ToString() const;
	static const Rotator Identity;

public:
	float Yaw = 0.f;
	float Roll = 0.f;
	float Pitch = 0.f;
};

}
