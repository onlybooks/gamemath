
#include "Precompiled.h"

WindowsRSI::~WindowsRSI()
{
}

bool WindowsRSI::Init(const ScreenPoint& InScreenSize)
{
	return InitializeGDI(InScreenSize);
}

void WindowsRSI::Shutdown()
{
	ReleaseGDI();
}

void WindowsRSI::Clear(const LinearColor & InClearColor)
{
	FillBuffer(InClearColor.ToColor32());
	ClearDepthBuffer();
}

void WindowsRSI::BeginFrame()
{
}

void WindowsRSI::EndFrame()
{
	SwapBuffer();
}

void WindowsRSI::DrawFullVerticalLine(int InX, const LinearColor & InColor)
{
	if (InX < 0 || InX >= _ScreenSize.X)
	{
		return;
	}

	for (int y = 0; y < _ScreenSize.Y; ++y)
	{
		SetPixel(ScreenPoint(InX, y), InColor);
	}
}

void WindowsRSI::DrawFullHorizontalLine(int InY, const LinearColor & InColor)
{
	if (InY < 0 || InY >= _ScreenSize.Y)
	{
		return;
	}

	for (int x = 0; x < _ScreenSize.X; ++x)
	{
		SetPixel(ScreenPoint(x, InY), InColor);
	}
}

void WindowsRSI::DrawPoint(const Vector2& InVectorPos, const LinearColor& InColor)
{
	SetPixel(ScreenPoint::ToScreenCoordinate(_ScreenSize, InVectorPos), InColor);
}

void WindowsRSI::DrawPoint(const ScreenPoint& InScreenPos, const LinearColor& InColor)
{
	SetPixel(InScreenPos, InColor);
}

int WindowsRSI::TestRegion(const Vector2& InVectorPos, const Vector2& InMinPos, const Vector2& InMaxPos)
{
	int result = 0;
	if (InVectorPos.X < InMinPos.X)
	{
		result = result | 0b0001;
	}
	else if (InVectorPos.X > InMaxPos.X)
	{
		result = result | 0b0010;
	}

	if (InVectorPos.Y < InMinPos.Y)
	{
		result = result | 0b0100;
	}
	else if (InVectorPos.Y > InMaxPos.Y)
	{
		result = result | 0b1000;
	}

	return result;
}

bool WindowsRSI::CohenSutherlandLineClip(Vector2& InOutStartPos, Vector2& InOutEndPos, const Vector2& InMinPos, const Vector2& InMaxPos)
{
	int startTest = TestRegion(InOutStartPos, InMinPos, InMaxPos);
	int endTest = TestRegion(InOutEndPos, InMinPos, InMaxPos);

	float width = (InOutEndPos.X - InOutStartPos.X);
	float height = (InOutEndPos.Y - InOutStartPos.Y);

	while (true)
	{
		if ((startTest == 0) && (endTest == 0)) // 화면 안에 두 점이 있으면 바로 그리기
		{
			return true;
		}
		else if (startTest & endTest) // 화면 밖에 선이 있으므로 그릴 필요가 없음
		{
			return false;
		}
		else // 양쪽을 조사해 클리핑 진행
		{
			Vector2 clippedPosition;
			bool isStartTest = (startTest != 0);
			int currentTest = isStartTest ? startTest : endTest;

			if (currentTest < 0b0100)
			{
				if (currentTest & 1)
				{
					clippedPosition.X = InMinPos.X;
				}
				else
				{
					clippedPosition.X = InMaxPos.X;
				}

				if (Math::EqualsInTolerance(height, 0.0f))
				{
					clippedPosition.Y = InOutStartPos.Y;

				}
				else
				{
					clippedPosition.Y = InOutStartPos.Y + height * (clippedPosition.X - InOutStartPos.X) / width;
				}
			}
			else
			{
				if (currentTest & 0b0100)
				{
					clippedPosition.Y = InMinPos.Y;
				}
				else
				{
					clippedPosition.Y = InMaxPos.Y;
				}

				if (Math::EqualsInTolerance(width, 0.0f))
				{
					clippedPosition.X = InOutStartPos.X;
				}
				else
				{
					clippedPosition.X = InOutStartPos.X + width * (clippedPosition.Y - InOutStartPos.Y) / height;
				}
			}

			// 클리핑한 결과로 다시 테스트 진행.
			if (isStartTest)
			{
				InOutStartPos = clippedPosition;
				startTest = TestRegion(InOutStartPos, InMinPos, InMaxPos);
			}
			else
			{
				InOutEndPos = clippedPosition;
				endTest = TestRegion(InOutEndPos, InMinPos, InMaxPos);
			}
		}
	}

	return true;
}

void WindowsRSI::DrawLine(const Vector4& InStartPos, const Vector4& InEndPos, const LinearColor& InColor)
{
	DrawLine(InStartPos.ToVector2(), InEndPos.ToVector2(), InColor);
}


float WindowsRSI::GetDepthBufferValue(const ScreenPoint& InPos) const
{
	if (_DepthBuffer == nullptr)
	{
		return INFINITY;
	}

	if (!IsInScreen(InPos))
	{
		return INFINITY;
	}

	return *(_DepthBuffer + GetScreenBufferIndex(InPos));
}

void WindowsRSI::SetDepthBufferValue(const ScreenPoint& InPos, float InDepthValue)
{
	if (_DepthBuffer == nullptr)
	{
		return;
	}

	if (!IsInScreen(InPos))
	{
		return;
	}

	*(_DepthBuffer + GetScreenBufferIndex(InPos)) = InDepthValue;
}

void WindowsRSI::DrawLine(const Vector2& InStartPos, const Vector2& InEndPos, const LinearColor& InColor)
{
	Vector2 clippedStart = InStartPos;
	Vector2 clippedEnd = InEndPos;
	Vector2 screenExtend = Vector2(_ScreenSize.X, _ScreenSize.Y) * 0.5f;
	Vector2 minScreen = -screenExtend;
	Vector2 maxScreen = screenExtend;
	if (!CohenSutherlandLineClip(clippedStart, clippedEnd, minScreen, maxScreen))
	{
		return;
	}

	ScreenPoint startPosition = ScreenPoint::ToScreenCoordinate(_ScreenSize, clippedStart);
	ScreenPoint endPosition = ScreenPoint::ToScreenCoordinate(_ScreenSize, clippedEnd);

	int width = endPosition.X - startPosition.X;
	int height = endPosition.Y - startPosition.Y;

	bool isGradualSlope = (Math::Abs(width) >= Math::Abs(height));
	int dx = (width >= 0) ? 1 : -1;
	int dy = (height > 0) ? 1 : -1;
	int fw = dx * width;
	int fh = dy * height;

	int f = isGradualSlope ? fh * 2 - fw : 2 * fw - fh;
	int f1 = isGradualSlope ? 2 * fh : 2 * fw;
	int f2 = isGradualSlope ? 2 * (fh - fw) : 2 * (fw - fh);
	int x = startPosition.X;
	int y = startPosition.Y;

	if (isGradualSlope)
	{
		while (x != endPosition.X)
		{
			SetPixel(ScreenPoint(x, y), InColor);

			if (f < 0)
			{
				f += f1;
			}
			else
			{
				f += f2;
				y += dy;
			}

			x += dx;
		}
	}
	else
	{
		while (y != endPosition.Y)
		{
			SetPixel(ScreenPoint(x, y), InColor);

			if (f < 0)
			{
				f += f1;
			}
			else
			{
				f += f2;
				x += dx;
			}

			y += dy;
		}
	}
}

void WindowsRSI::PushStatisticText(std::string && InText)
{
	_StatisticTexts.emplace_back(InText);
}

void WindowsRSI::PushStatisticTexts(std::vector<std::string> && InTexts)
{
	std::move(InTexts.begin(), InTexts.end(), std::back_inserter(_StatisticTexts));
}
