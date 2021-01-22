
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
float currentScale = 10.f;
float deltaDegree = 0.f;
float deltaShear = 0.f;

// 게임 로직을 담당하는 함수
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직의 로컬 변수
	static float scaleMin = 5.f;
	static float scaleMax = 10.f;
	static float scaleSpeed = 10.f;
	static float rotateSpeed = 180.f;
	static float shearSpeed = 2.f;

	deltaShear = input.GetAxis(InputAxis::XAxis) * shearSpeed * InDeltaSeconds;
	float deltaScale = input.GetAxis(InputAxis::ZAxis) * scaleSpeed * InDeltaSeconds;
	deltaDegree = input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds;

	currentScale = Math::Clamp(currentScale + deltaScale, scaleMin, scaleMax);
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
	static float currentDegree = 0.f;
	static float currentShear = 0.f;
	static const Vector2 pivot(200.f, 0.f);

	currentDegree += deltaDegree;
	currentShear += deltaShear;

	// 하트를 구성하는 점 생성
	static float increment = 0.001f;
	float rad = 0.f;
	static std::vector<Vector2> hearts;
	if (hearts.empty())
	{
		for (rad = 0.f; rad < Math::TwoPI; rad += increment)
		{
			float sin = sinf(rad);
			float cos = cosf(rad);
			float cos2 = cosf(2 * rad);
			float cos3 = cosf(3 * rad);
			float cos4 = cosf(4 * rad);
			float x = 16.f * sin * sin * sin;
			float y = 13 * cos - 5 * cos2 - 2 * cos3 - cos4;
			hearts.push_back(Vector2(x, y));
		}
	}

	// 크기 행렬
	Vector2 sBasis1(currentScale, 0.f);
	Vector2 sBasis2(0.f, currentScale);
	Matrix2x2 sMatrix(sBasis1, sBasis2);

	// 회전 행렬
	float sin, cos;
	Math::GetSinCos(sin, cos, currentDegree);
	Vector2 rBasis1(cos, sin);
	Vector2 rBasis2(-sin, cos);
	Matrix2x2 rMatrix(rBasis1, rBasis2);

	// 밀기 행렬
	Vector2 shBasis1 = Vector2::UnitX;
	Vector2 shBasis2(currentShear, 1.f);
	Matrix2x2 shMatrix(shBasis1, shBasis2);

	// 합성 행렬
	Matrix2x2 cMatrix = shMatrix * rMatrix * sMatrix;

	// 크기 행렬의 역행렬
	float invScale = 1.f / currentScale;
	Vector2 isBasis1(invScale, 0.f);
	Vector2 isBasis2(0.f, invScale);
	Matrix2x2 isMatrix(isBasis1, isBasis2);

	// 회전 행렬의 역행렬
	Matrix2x2 irMatrix = rMatrix.Tranpose();

	// 밀기 행렬의 역행렬
	Vector2 ishBasis1 = Vector2::UnitX;
	Vector2 ishBasis2(-currentShear, 1.f);
	Matrix2x2 ishMatrix(ishBasis1, ishBasis2);

	// 역행렬의 합성행렬. ( 역순으로 조합하기 )
	Matrix2x2 icMatrix = isMatrix * irMatrix * ishMatrix;

	// 각 값을 초기화한 후 동일하게 증가시키면서 색상 값을 지정
	rad = 0.f;
	HSVColor hsv(0.f, 1.f, 0.85f);
	for (auto const& v : hearts)
	{
		// 왼쪽 하트 ( 변환 행렬을 적용하기 )
		Vector2 left = cMatrix * v;
		r.DrawPoint(left - pivot, hsv.ToLinearColor());

		// 오른쪽 하트 ( 왼쪽 하트에 추가로 역행렬을 적용하기 )
		Vector2 right = icMatrix * left;
		r.DrawPoint(right + pivot, hsv.ToLinearColor());

		hsv.H = rad / Math::TwoPI;
		rad += increment;
	}

	// 현재 밀기, 스케일, 회전각을 화면에 출력
	r.PushStatisticText(std::string("Shear : ") + std::to_string(currentShear));
	r.PushStatisticText(std::string("Scale : ") + std::to_string(currentScale));
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
