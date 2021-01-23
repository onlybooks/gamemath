
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
float currentDegree = 0.f;

// 게임 로직을 담당하는 함수
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직의 로컬 변수
	static float rotateSpeed = 180.f;

	float deltaDegree = input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds;

	// 물체의 최종 상태 설정
	currentDegree += deltaDegree;
}

// 렌더링 로직을 담당하는 함수
void SoftRenderer::Render2D()
{
	// 렌더링 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 배경에 격자 그리기
	DrawGizmo2D();

	// 렌더링 로직의 로컬 변수
	static float halfSize = 100.f;
	static std::vector<Vector2> squares;

	// 사각형을 구성하는 점을 생성
	if (squares.empty())
	{
		for (float x = -halfSize; x <= halfSize; x += 0.25f)
		{
			for (float y = -halfSize; y <= halfSize; y += 0.25f)
			{
				squares.push_back(Vector2(x, y));
			}
		}
	}

	// 각도에 해당하는 사인과 코사인 함수 얻기
	float sin = 0.f, cos = 0.f;
	Math::GetSinCos(sin, cos, currentDegree);

	// 현재 화면의 크기로부터 길이를 비교할 기준양 정하기
	static float maxLength = Vector2(_ScreenSize.X, _ScreenSize.Y).Size() * 0.5f;

	// 원을 구성하는 점을 그린다.
	HSVColor hsv(0.f, 1.f, 0.85f);
	for (auto const& v : squares)
	{
		// 극 좌표계로 변경한다.
		Vector2 polarV = v.ToPolarCoordinate();

		// 극좌표계의 각 정보로부터 색상을 결정한다.
		if (polarV.Y < 0.f)
		{
			polarV.Y += Math::TwoPI;
		}
		hsv.H = polarV.Y / Math::TwoPI;

		// 극좌표계의 크기 정보로부터 회전양을 결정한다.
		float ratio = polarV.X / maxLength;
		float weight = Math::Lerp(1.f, 5.f, ratio);

		// 극좌표계를 사용해 회전을 부여한다.
		polarV.Y += Math::Deg2Rad(currentDegree) * weight;

		// 최종 값을 데카르트좌표계로 변환한다.
		Vector2 cartesianV = polarV.ToCartesianCoordinate();
		r.DrawPoint(cartesianV, hsv.ToLinearColor());
	}

	// 현재 각도를 화면에 출력
	r.PushStatisticText(std::string("Degree : ") + std::to_string(currentDegree));
}

// 메시를 그리는 함수
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// 삼각형을 그리는 함수
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
