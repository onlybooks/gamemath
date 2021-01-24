
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
float fovAngle = 60.f;
Vector2 playerPosition(0.f, 0.f);
LinearColor playerColor = LinearColor::Gray;
Vector2 targetPosition(100.f, 100.f);
LinearColor targetColor = LinearColor::Blue;

// 게임 로직을 담당하는 함수
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직의 로컬 변수
	static float moveSpeed = 100.f;
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static std::uniform_real_distribution<float> randomPosX(-300.f, 300.f);
	static std::uniform_real_distribution<float> randomPosY(-200.f, 200.f);
	static float duration = 3.f;
	static float elapsedTime = 0.f;
	static Vector2 targetStart = targetPosition;
	static Vector2 targetDestination = Vector2(randomPosX(mt), randomPosY(mt));

	// 시야각의 코사인 값은 최초 1회만 계산해 보관한다.
	static float fovCos = cosf(Math::Deg2Rad(fovAngle * 0.5f));

	elapsedTime = Math::Clamp(elapsedTime + InDeltaSeconds, 0.f, duration);

	// 지정한 시간이 경과하면 새로운 이동 지점을 랜덤하게 설정
	if (elapsedTime == duration)
	{
		targetStart = targetDestination;
		targetPosition = targetDestination;
		targetDestination = Vector2(randomPosX(mt), randomPosY(mt));

		elapsedTime = 0.f;
	}
	else // 비율에 따라 목표지점까지 선형보간하면서 이동
	{
		float ratio = elapsedTime / duration;
		targetPosition = Vector2(
			Math::Lerp(targetStart.X, targetDestination.X, ratio),
			Math::Lerp(targetStart.Y, targetDestination.Y, ratio)
		);
	}

	Vector2 inputVector = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)).GetNormalize();
	Vector2 deltaPosition = inputVector * moveSpeed * InDeltaSeconds;

	Vector2 f = Vector2::UnitY;
	Vector2 v = (targetPosition - playerPosition).GetNormalize(); // 보초에서 플레이어로 향하는 정규화된 벡터

	// 물체의 최종 상태 설정
	if (v.Dot(f) >= fovCos)
	{
		playerColor = LinearColor::Red;
		targetColor = LinearColor::Red;
	}
	else
	{
		playerColor = LinearColor::Gray;
		targetColor = LinearColor::Blue;
	}

	playerPosition += deltaPosition;
}

// 렌더링 로직을 담당하는 함수
void SoftRenderer::Render2D()
{
	// 렌더링 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 렌더링 로직의 로컬 변수
	static float radius = 5.f;
	static std::vector<Vector2> sphere;
	static float sightLength = 300.f;

	if (sphere.empty())
	{
		for (float x = -radius; x <= radius; ++x)
		{
			for (float y = -radius; y <= radius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = radius * radius;
				if (sizeSquared < rr)
				{
					sphere.push_back(target);
				}
			}
		}
	}

	// 플레이어 렌더링. 
	float fovSin = 0.f, fovCos = 0.f;
	Math::GetSinCos(fovSin, fovCos, fovAngle * 0.5f);

	r.DrawLine(playerPosition, playerPosition + Vector2(sightLength * fovSin, sightLength * fovCos), playerColor);
	r.DrawLine(playerPosition, playerPosition + Vector2(-sightLength * fovSin, sightLength * fovCos), playerColor);
	r.DrawLine(playerPosition, playerPosition + Vector2::UnitY * 50.f, playerColor);
	for (auto const& v : sphere)
	{
		r.DrawPoint(v + playerPosition, playerColor);
	}

	// 타겟 렌더링
	for (auto const& v : sphere)
	{
		r.DrawPoint(v + targetPosition, targetColor);
	}

	// 주요 정보 출력
	r.PushStatisticText(std::string("Player Position : ") + playerPosition.ToString());
	r.PushStatisticText(std::string("Target Position : ") + targetPosition.ToString());
}

// 메시를 그리는 함수
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// 삼각형을 그리는 함수
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
