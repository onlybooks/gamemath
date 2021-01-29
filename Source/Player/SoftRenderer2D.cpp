
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
Vector2 currentPosition;
float currentScale = 300.f;
float currentDegree = 0.f;

// 게임 로직을 담당하는 함수
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직의 로컬 변수
	static float moveSpeed = 100.f;
	static float scaleMin = 200.f;
	static float scaleMax = 400.f;
	static float scaleSpeed = 100.f;
	static float rotateSpeed = 180.f;

	Vector2 inputVector = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)).GetNormalize();
	Vector2 deltaPosition = inputVector * moveSpeed * InDeltaSeconds;
	float deltaScale = input.GetAxis(InputAxis::ZAxis) * scaleSpeed * InDeltaSeconds;
	float deltaDegree = input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds;

	// 물체의 최종 상태 설정
	currentPosition += deltaPosition;
	currentScale = Math::Clamp(currentScale + deltaScale, scaleMin, scaleMax);
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

    // 메시 데이터의 선언
    static constexpr size_t vertexCount = 3;
    static constexpr size_t triangleCount = 1;

    // 메시를 구성하는 정점 배열과 인덱스 배열의 생성
    static constexpr std::array<Vertex2D, vertexCount> rawVertices = {
        Vertex2D(Vector2(0.f, 0.25f), LinearColor(1.f, 0.f, 0.f)),
        Vertex2D(Vector2(-0.5f, -0.25f), LinearColor(0.f, 1.f, 0.f)),
        Vertex2D(Vector2(0.5f, -0.25f), LinearColor(0.f, 0.f, 1.f))
    };

    static constexpr std::array<size_t, triangleCount * 3> indices = {
        0, 2, 1
    };

    // 아핀 변환 행렬 ( 크기 ) 
    Vector3 sBasis1(currentScale, 0.f, 0.f);
    Vector3 sBasis2(0.f, currentScale, 0.f);
    Vector3 sBasis3 = Vector3::UnitZ;
    Matrix3x3 sMatrix(sBasis1, sBasis2, sBasis3);

    // 아핀 변환 행렬 ( 회전 ) 
    float sin = 0.f, cos = 0.f;
    Math::GetSinCos(sin, cos, currentDegree);
    Vector3 rBasis1(cos, sin, 0.f);
    Vector3 rBasis2(-sin, cos, 0.f);
    Vector3 rBasis3 = Vector3::UnitZ;
    Matrix3x3 rMatrix(rBasis1, rBasis2, rBasis3);

    // 아핀 변환 행렬 ( 이동 ) 
    Vector3 tBasis1 = Vector3::UnitX;
    Vector3 tBasis2 = Vector3::UnitY;
    Vector3 tBasis3(currentPosition.X, currentPosition.Y, 1.f);
    Matrix3x3 tMatrix(tBasis1, tBasis2, tBasis3);

    // 모든 아핀 변환의 조합 행렬. 크기-회전-이동 순으로 조합
    Matrix3x3 finalMatrix = tMatrix * rMatrix * sMatrix;

    // 행렬을 적용한 메시 정보를 사용해 물체를 렌더링
    static std::vector<Vertex2D> vertices(vertexCount);
    for (size_t vi = 0; vi < vertexCount; ++vi)
    {
        vertices[vi].Position = finalMatrix * rawVertices[vi].Position;
        vertices[vi].Color = rawVertices[vi].Color;
    }

    // 변환된 정점을 잇는 선 그리기
    for (size_t ti = 0; ti < triangleCount; ++ti)
    {
        size_t bi = ti * 3;
        std::array<Vertex2D, 3> tv = { vertices[indices[bi]] , vertices[indices[bi + 1]], vertices[indices[bi + 2]] };

        Vector2 minPos(Math::Min3(tv[0].Position.X, tv[1].Position.X, tv[2].Position.X), Math::Min3(tv[0].Position.Y, tv[1].Position.Y, tv[2].Position.Y));
        Vector2 maxPos(Math::Max3(tv[0].Position.X, tv[1].Position.X, tv[2].Position.X), Math::Max3(tv[0].Position.Y, tv[1].Position.Y, tv[2].Position.Y));

        // 무게중심좌표를 위한 준비작업
        Vector2 u = tv[1].Position - tv[0].Position;
        Vector2 v = tv[2].Position - tv[0].Position;

        // 공통 분모 ( uu * vv - uv * uv )
        float udotv = u.Dot(v);
        float vdotv = v.Dot(v);
        float udotu = u.Dot(u);
        float denominator = udotv * udotv - vdotv * udotu;

        // 퇴화삼각형은 그리지 않음
        if (denominator == 0.0f)
        {
            continue;
        }

        float invDenominator = 1.f / denominator;

        // 화면상의 점 구하기
        ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
        ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

        // 두 점이 화면 밖을 벗어나는 경우 클리핑 처리
        lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
        lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
        upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
        upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

        // 삼각형을 둘러싼 사각형 영역의 픽셀을 모두 순회
        for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
        {
            for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
            {
                ScreenPoint fragment = ScreenPoint(x, y);
                Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
                Vector2 w = pointToTest - tv[0].Position;
                float wdotu = w.Dot(u);
                float wdotv = w.Dot(v);

                // 분자 값을 구하고 최종 무게중심좌표 산출
                float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
                float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
                float oneMinusST = 1.f - s - t;

                // 컨벡스 조건을 만족할 때만 점 찍기
                if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
                {
                    LinearColor outColor = tv[0].Color * oneMinusST + tv[1].Color * s + tv[2].Color * t;
                    r.DrawPoint(fragment, outColor);
                }
            }
        }
    }

    // 현재 위치, 크기, 각도를 화면에 출력
    r.PushStatisticText(std::string("Position : ") + currentPosition.ToString());
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
