
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DD;

// 격자를 그리는 함수
void SoftRenderer::DrawGizmo2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// 좌측 하단에서부터 격자 그리기
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// 그리드가 시작되는 좌하단 좌표 값 계산
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// 게임 오브젝트 목록


// 최초 씬 로딩을 담당하는 함수
void SoftRenderer::LoadScene2D()
{
	// 최초 씬 로딩에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();

}

// 게임 로직과 렌더링 로직이 공유하는 변수
Vector2 lightPosition;
LinearColor lightColor;
Vector2 circlePosition;

// 게임 로직을 담당하는 함수
void SoftRenderer::Update2D(float InDeltaSeconds)
{
    // 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
    auto& g = Get2DGameEngine();
    const InputManager& input = g.GetInputManager();

    // 게임 로직의 로컬 변수
    static float duration = 20.f;
    static float elapsedTime = 0.f;
    static float currentDegree = 0.f;
    static float lightDistance = 200.f;
    static HSVColor lightHSVColor;

    // 경과 시간에 따른 현재 각과 이를 사용한 [0,1]값의 생성
    elapsedTime += InDeltaSeconds;
    elapsedTime = Math::FMod(elapsedTime, duration);
    float currentRad = (elapsedTime / duration) * Math::TwoPI;
    float alpha = (sinf(currentRad) + 1) * 0.5f;

    // [0,1]을 활용해 주기적으로 크기를 반복하기
    currentDegree = Math::Lerp(0.f, 360.f, alpha);

    // 광원의 좌표와 색상
    float sin = 0.f, cos = 0.f;
    Math::GetSinCosRad(sin, cos, currentRad);
    lightPosition = Vector2(cos, sin) * lightDistance;

    lightHSVColor.H = currentRad * Math::InvPI * 0.5f;
    lightColor = lightHSVColor.ToLinearColor();
}

// 렌더링 로직을 담당하는 함수
void SoftRenderer::Render2D()
{
    // 렌더링 로직에서 사용하는 모듈 내 주요 레퍼런스
    auto& r = GetRenderer();
    const auto& g = Get2DGameEngine();

    // 렌더링 로직의 로컬 변수
    static std::vector<Vector2> light;
    static float lightRadius = 10.f;
    static std::vector<Vector2> circle;
    static float circleRadius = 50.f;

    // 광원을 표현하는 구체
    if (light.empty())
    {
        float lightRadius = 10.f;
        for (float x = -lightRadius; x <= lightRadius; ++x)
        {
            for (float y = -lightRadius; y <= lightRadius; ++y)
            {
                Vector2 target(x, y);
                float sizeSquared = target.SizeSquared();
                float rr = lightRadius * lightRadius;
                if (sizeSquared < rr)
                {
                    light.push_back(target);
                }
            }
        }
    }

    // 빛을 받는 물체
    if (circle.empty())
    {
        for (float x = -circleRadius; x <= circleRadius; ++x)
        {
            for (float y = -circleRadius; y <= circleRadius; ++y)
            {
                Vector2 target(x, y);
                float sizeSquared = target.SizeSquared();
                float rr = circleRadius * circleRadius;
                if (sizeSquared < rr)
                {
                    circle.push_back(target);
                }
            }
        }
    }

    // 광원 그리기
    static float lightLineLength = 50.f;
    r.DrawLine(lightPosition, lightPosition - lightPosition.GetNormalize() * lightLineLength, lightColor);
    for (auto const& v : light)
    {
        r.DrawPoint(v + lightPosition, lightColor);
    }

    // 광원을 받는 구체의 모든 픽셀에 NdotL을 계산해 음영을 산출하고 이를 최종 색상에 반영
    for (auto const& v : circle)
    {
        Vector2 n = (v - circlePosition).GetNormalize();
        Vector2 l = (lightPosition - v).GetNormalize();
        float shading = Math::Clamp(n.Dot(l), 0.f, 1.f);
        r.DrawPoint(v, lightColor * shading);
    }

    // 현재 조명의 위치를 화면에 출력
    r.PushStatisticText(std::string("Position : ") + lightPosition.ToString());
}

// 메시를 그리는 함수
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// 삼각형을 그리는 함수
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
