
#pragma once

namespace CK
{

class RendererInterface
{
public:
	virtual bool Init(const ScreenPoint& InSize) = 0;
	virtual void Shutdown() = 0;
	virtual bool IsInitialized() const = 0;

	virtual void Clear(const LinearColor& InClearColor) = 0;
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void DrawPoint(const Vector2& InVectorPos, const LinearColor& InColor) = 0;
	virtual void DrawPoint(const ScreenPoint& InScreenPos, const LinearColor& InColor) = 0;
	virtual void DrawLine(const Vector2& InStartPos, const Vector2& InEndPos, const LinearColor& InColor) = 0;
	virtual void DrawLine(const Vector4& InStartPos, const Vector4& InEndPos, const LinearColor& InColor) = 0;

	virtual float GetDepthBufferValue(const ScreenPoint& InScreenPos) const = 0;
	virtual void SetDepthBufferValue(const ScreenPoint& InScreenPos, float InDepthValue) = 0;

	virtual void DrawFullVerticalLine(int InX, const LinearColor& InColor) = 0;
	virtual void DrawFullHorizontalLine(int InY, const LinearColor& InColor) = 0;

	virtual void PushStatisticText(std::string && InText) = 0;
	virtual void PushStatisticTexts(std::vector<std::string> && InTexts) = 0;
};

}