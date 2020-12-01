
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
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

	// 월드의 원점
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}


// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	static float moveSpeed = 200.f;
	static float rotateSpeed = 180.f;
	static float scaleSpeed = 180.f;

	// 플레이어 게임 오브젝트 트랜스폼의 변경
	GameObject& goPlayer = g.GetGameObject(GameEngine::PlayerGo);
	assert(goPlayer.IsValid());

	// 입력에 따른 플레이어 위치와 크기의 변경
	TransformComponent& transform = goPlayer.GetTransform();
	transform.AddPosition(Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)) * moveSpeed * InDeltaSeconds);
	transform.AddRotation(input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds);
	float newScale = Math::Clamp(transform.GetScale().X + scaleSpeed * input.GetAxis(InputAxis::ZAxis) * InDeltaSeconds, 15.f, 30.f);
	transform.SetScale(Vector2::One * newScale);
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 격자 그리기
	DrawGrid2D();

	// 전체 그릴 물체의 수
	size_t totalObjectCount = g.GetScene().size();

	// 랜덤하게 생성된 모든 게임 오브젝트들
	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		// 게임 오브젝트에 필요한 내부 정보를 가져오기
		const GameObject& gameObject = *(*it);
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());
		const TransformComponent& transform = gameObject.GetTransform();
		Matrix3x3 finalMatrix = transform.GetModelingMatrix();

		if (gameObject != GameEngine::PlayerGo)
		{
			DrawMesh2D(mesh, finalMatrix, gameObject.GetColor());

			r.PushStatisticText("Player Position : " + transform.GetPosition().ToString());
			r.PushStatisticText("Player Rotation : " + std::to_string(transform.GetRotation()) + " (deg)");
			r.PushStatisticText("Player Scale : " + std::to_string(transform.GetScale().X));
		}
		else
		{
			DrawMesh2D(mesh, finalMatrix, gameObject.GetColor());
		}
	}

	r.PushStatisticText("Total Game Objects : " + std::to_string(totalObjectCount));
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
	size_t vertexCount = InMesh.GetVertices().size();
	size_t indexCount = InMesh.GetIndices().size();
	size_t triangleCount = indexCount / 3;

	// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼로 변환
	std::vector<Vertex2D> vertices(vertexCount);
	std::vector<size_t> indice(InMesh.GetIndices());
	for (size_t vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = InMesh.GetVertices()[vi];
		if (InMesh.HasColor())
		{
			vertices[vi].Color = InMesh.GetColors()[vi];
		}

		if (InMesh.HasUV())
		{
			vertices[vi].UV = InMesh.GetUVs()[vi];
		}
	}

	// 정점 변환 진행
	VertexShader2D(vertices, InMatrix);

	// 그리기모드 설정
	FillMode fm = FillMode::None;
	if (InMesh.HasColor())
	{
		fm |= FillMode::Color;
	}
	if (InMesh.HasUV())
	{
		fm |= FillMode::Texture;
	}

	// 삼각형 별로 그리기
	for (int ti = 0; ti < triangleCount; ++ti)
	{
		int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
		std::vector<Vertex2D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };
		DrawTriangle2D(tvs, InColor, fm);
	}
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
	auto& r = GetRenderer();
	const GameEngine& g = Get2DGameEngine();
	const Texture& mainTexture = g.GetTexture(GameEngine::DiffuseTexture);

	if (IsWireframeDrawing())
	{
		LinearColor finalColor = _WireframeColor;
		if (InColor != LinearColor::Error)
		{
			finalColor = InColor;
		}

		r.DrawLine(InVertices[0].Position, InVertices[1].Position, finalColor);
		r.DrawLine(InVertices[0].Position, InVertices[2].Position, finalColor);
		r.DrawLine(InVertices[1].Position, InVertices[2].Position, finalColor);
	}
	else
	{
		// 삼각형 칠하기
		// 삼각형의 영역 설정
		Vector2 minPos(Math::Min3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Min3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));
		Vector2 maxPos(Math::Max3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Max3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));

		// 무게중심좌표를 위해 점을 벡터로 변환
		Vector2 u = InVertices[1].Position - InVertices[0].Position;
		Vector2 v = InVertices[2].Position - InVertices[0].Position;

		// 공통 분모 값 ( uu * vv - uv * uv )
		float udotv = u.Dot(v);
		float vdotv = v.Dot(v);
		float udotu = u.Dot(u);
		float denominator = udotv * udotv - vdotv * udotu;

		// 퇴화 삼각형 판정.
		if (Math::EqualsInTolerance(denominator, 0.f))
		{
			return;
		}

		float invDenominator = 1.f / denominator;

		ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
		ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

		// 두 점이 화면 밖을 벗어나는 경우 클리핑 처리
		lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
		lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
		upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
		upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

		// 삼각형 영역 내 모든 점을 점검하고 색칠
		for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
		{
			for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
			{
				ScreenPoint fragment = ScreenPoint(x, y);
				Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
				Vector2 w = pointToTest - InVertices[0].Position;
				float wdotu = w.Dot(u);
				float wdotv = w.Dot(v);

				float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
				float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
				float oneMinusST = 1.f - s - t;
				if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
				{
					Vector2 targetUV = InVertices[0].UV * oneMinusST + InVertices[1].UV * s + InVertices[2].UV * t;
					r.DrawPoint(fragment, FragmentShader2D(mainTexture.GetSample(targetUV), LinearColor::White));
				}
			}
		}
	}
}
