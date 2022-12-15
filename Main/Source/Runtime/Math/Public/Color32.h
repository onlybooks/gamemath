#pragma once

namespace CK
{

struct Color32
{
public:
	FORCEINLINE constexpr Color32() : R(0), G(0), B(0), A(0) { }
	FORCEINLINE explicit constexpr Color32(BYTE InR, BYTE InG, BYTE InB, BYTE InA = 255) : B(InB), G(InG), R(InR), A(InA) { }
	FORCEINLINE explicit constexpr Color32(UINT32 InColor) : ColorValue(InColor) { }

	FORCEINLINE constexpr const UINT32& GetColorRef() const { return ColorValue; }
	FORCEINLINE constexpr UINT32& GetColorRef() { return ColorValue; }

	FORCEINLINE constexpr bool operator==(const Color32& InC) const;
	FORCEINLINE constexpr bool operator!=(const Color32& InC) const;
	FORCEINLINE constexpr void operator+=(const Color32& InC);

	static const Color32 Error;

public:
	union 
	{ 
		struct 
		{ 
			BYTE B, G, R, A; 
		};

		UINT32 ColorValue;
	};
};

FORCEINLINE constexpr bool Color32::operator==(const Color32& InC) const
{
	return GetColorRef() == InC.GetColorRef();
}

FORCEINLINE constexpr bool Color32::operator!=(const Color32& InC) const
{
	return GetColorRef() != InC.GetColorRef();
}

FORCEINLINE constexpr void Color32::operator+=(const Color32& InC)
{
	R = (BYTE)Math::Clamp((BYTE)R + (BYTE)InC.R, 0, 255);
	G = (BYTE)Math::Clamp((BYTE)G + (BYTE)InC.G, 0, 255);
	B = (BYTE)Math::Clamp((BYTE)B + (BYTE)InC.B, 0, 255);
	A = (BYTE)Math::Clamp((BYTE)A + (BYTE)InC.A, 0, 255);
}

}

