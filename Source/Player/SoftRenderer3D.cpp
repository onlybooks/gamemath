
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DDD;

// 기즈모를 그리는 함수
void SoftRenderer::DrawGizmo3D()
{
	auto& r = GetRenderer();
	const GameEngine& g = Get3DGameEngine();

	// 뷰 기즈모 그리기
	std::vector<Vertex3D> viewGizmo = {
		Vertex3D(Vector4(Vector3::Zero)),
		Vertex3D(Vector4(Vector3::UnitX * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitY * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitZ * _GizmoUnitLength)),
	};

	Matrix4x4 viewMatRotationOnly = g.GetMainCamera().GetViewMatrixRotationOnly();
	VertexShader3D(viewGizmo, viewMatRotationOnly);

	// 축 그리기
	Vector2 v0 = viewGizmo[0].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v1 = viewGizmo[1].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v2 = viewGizmo[2].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v3 = viewGizmo[3].Position.ToVector2() + _GizmoPositionOffset;
	r.DrawLine(v0, v1, LinearColor::Red);
	r.DrawLine(v0, v2, LinearColor::Green);
	r.DrawLine(v0, v3, LinearColor::Blue);
}

// 게임 오브젝트 목록
static const std::string PlayerGo = "Player";

// 최초 씬 로딩을 담당하는 함수
void SoftRenderer::LoadScene3D()
{
	GameEngine& g = Get3DGameEngine();

	// 플레이어
	constexpr float playerScale = 200.f;

	// 플레이어 설정
	GameObject& goPlayer = g.CreateNewGameObject(PlayerGo);
	goPlayer.SetMesh(GameEngine::CubeMesh);
	goPlayer.GetTransform().SetPosition(Vector3::Zero);
	goPlayer.GetTransform().SetScale(Vector3::One * playerScale);
	goPlayer.GetTransform().SetRotation(Rotator(0.f, 0.f, 0.f));

	// 카메라 설정
	CameraObject& mainCamera = g.GetMainCamera();
	mainCamera.GetTransform().SetPosition(Vector3(0.f, 600.f, 600.f));
	mainCamera.GetTransform().SetRotation(Rotator(180.f, 0.f, 45.f));
}

// 게임 로직과 렌더링 로직이 공유하는 변수

// 게임 로직을 담당하는 함수
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	// 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
	GameEngine& g = Get3DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직의 로컬 변수
	static float rotateSpeed = 180.f;

	// 게임 로직에서 사용할 게임 오브젝트 레퍼런스
	GameObject& goPlayer = g.GetGameObject(PlayerGo);

	// 입력에 따른 플레이어 트랜스폼의 변경
	Rotator r = goPlayer.GetTransform().GetRotation();
	if (input.IsPressed(InputButton::Space))
	{
		r.Pitch = -90.f;
	}
	else
	{
		r.Pitch += input.GetAxis(InputAxis::ZAxis) * rotateSpeed * InDeltaSeconds;
	}
	r.Roll += input.GetAxis(InputAxis::XAxis) * rotateSpeed * InDeltaSeconds;
	r.Yaw += input.GetAxis(InputAxis::YAxis) * rotateSpeed * InDeltaSeconds;

	goPlayer.GetTransform().SetRotation(r);
}

// 애니메이션 로직을 담당하는 함수
void SoftRenderer::LateUpdate3D(float InDeltaSeconds)
{
	// 애니메이션 로직에서 사용하는 모듈 내 주요 레퍼런스
	GameEngine& g = Get3DGameEngine();

	// 애니메이션 로직의 로컬 변수

}

// 렌더링 로직을 담당하는 함수
void SoftRenderer::Render3D()
{
	// 렌더링 로직에서 사용하는 모듈 내 주요 레퍼런스
	const GameEngine& g = Get3DGameEngine();
	auto& r = GetRenderer();
	const CameraObject& mainCamera = g.GetMainCamera();

	// 배경에 기즈모 그리기
	DrawGizmo3D();

	// 렌더링 로직의 로컬 변수
	const Matrix4x4 pvMatrix = mainCamera.GetPerspectiveViewMatrix();

	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *(*it);
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		// 렌더링에 필요한 게임 오브젝트의 주요 레퍼런스를 얻기
		const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());
		const TransformComponent& transform = gameObject.GetTransform();

		Matrix4x4 finalMatrix = pvMatrix * transform.GetModelingMatrix();

		// 메시 그리기
		DrawMesh3D(mesh, finalMatrix, gameObject.GetColor());

		// 뷰 공간에서의 플레이어 위치를 화면에 표시
		if (gameObject == PlayerGo)
		{
			Rotator rotator = gameObject.GetTransform().GetRotation();
			r.PushStatisticText("Yaw : " + std::to_string(rotator.Yaw));
			r.PushStatisticText("Roll : " + std::to_string(rotator.Roll));
			r.PushStatisticText("Pitch : " + std::to_string(rotator.Pitch));
		}
	}
}

// 메시를 그리는 함수
void SoftRenderer::DrawMesh3D(const Mesh& InMesh, const Matrix4x4& InMatrix, const LinearColor& InColor)
{
	size_t vertexCount = InMesh.GetVertices().size();
	size_t indexCount = InMesh.GetIndices().size();
	size_t triangleCount = indexCount / 3;

	// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼로 변환
	std::vector<Vertex3D> vertices(vertexCount);
	std::vector<size_t> indice(InMesh.GetIndices());
	for (size_t vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = Vector4(InMesh.GetVertices()[vi]);

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
	VertexShader3D(vertices, InMatrix);

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
		std::vector<Vertex3D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };

		size_t triangles = tvs.size() / 3;
		for (size_t ti = 0; ti < triangles; ++ti)
		{
			size_t si = ti * 3;
			std::vector<Vertex3D> sub(tvs.begin() + si, tvs.begin() + si + 3);
			DrawTriangle3D(sub, InColor, fm);
		}
	}
}

// 삼각형을 그리는 함수
void SoftRenderer::DrawTriangle3D(std::vector<Vertex3D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
	auto& r = GetRenderer();
	const GameEngine& g = Get3DGameEngine();

	for (auto& v : InVertices)
	{
		// 무한 원점인 경우, 약간 보정해준다.
		if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;
	}

	// 백페이스 컬링 ( 뒷면이면 그리기 생략 )
	Vector3 edge1 = (InVertices[1].Position - InVertices[0].Position).ToVector3();
	Vector3 edge2 = (InVertices[2].Position - InVertices[0].Position).ToVector3();
	Vector3 faceNormal = edge1.Cross(edge2);
	Vector3 viewDirection = Vector3::UnitZ;
	if (faceNormal.Dot(viewDirection) <= 0.f)
	{
		return;
	}

	if (IsWireframeDrawing())
	{
		for (auto& v : InVertices)
		{
			v.Position.X *= _ScreenSize.X * 0.5f;
			v.Position.Y *= _ScreenSize.Y * 0.5f;
		}

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
		Vector2 u = InVertices[1].Position.ToVector2() - InVertices[0].Position.ToVector2();
		Vector2 v = InVertices[2].Position.ToVector2() - InVertices[0].Position.ToVector2();

		// 공통 분모 값 ( uu * vv - uv * uv )
		float udotv = u.Dot(v);
		float vdotv = v.Dot(v);
		float udotu = u.Dot(u);
		float denominator = udotv * udotv - vdotv * udotu;

		// 퇴화 삼각형이면 그리기 생략
		if (denominator == 0.f)
		{
			return;
		}

		float invDenominator = 1.f / denominator;

		// 화면상의 점 구하기
		minPos.X *= _ScreenSize.X * 0.5f;
		minPos.Y *= _ScreenSize.Y * 0.5f;
		maxPos.X *= _ScreenSize.X * 0.5f;
		maxPos.Y *= _ScreenSize.Y * 0.5f;

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
				pointToTest.X *= (2.f / _ScreenSize.X);
				pointToTest.Y *= (2.f / _ScreenSize.Y);
				Vector2 w = pointToTest - InVertices[0].Position.ToVector2();
				float wdotu = w.Dot(u);
				float wdotv = w.Dot(v);

				float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
				float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
				float oneMinusST = 1.f - s - t;
				if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
				{
					// 각 점마다 보존된 뷰 공간의 z값
					float invZ0 = 1.f / InVertices[0].Position.W;
					float invZ1 = 1.f / InVertices[1].Position.W;
					float invZ2 = 1.f / InVertices[2].Position.W;

					// 투영 보정보간에 사용할 공통 분모
					float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
					float invZ = 1.f / z;

					float newDepth = (InVertices[0].Position.Z * oneMinusST * invZ0 + InVertices[1].Position.Z * s * invZ1 + InVertices[2].Position.Z * t * invZ2) * invZ;
					float prevDepth = r.GetDepthBufferValue(fragment);
					if (newDepth < prevDepth)
					{
						r.SetDepthBufferValue(fragment, newDepth);
					}
					else
					{
						// 이미 앞에 무언가 그려져있으므로 픽셀그리기는 생략
						continue;
					}

					// 색상 파라미터가 설정 안된 경우에는 흰색을 사용
					LinearColor paramColor = LinearColor::White;
					if (InColor != LinearColor::Error)
					{
						paramColor = InColor;
					}

					// 버텍스 컬러 또는 텍스쳐 매핑으로 최종 보간된 색상
					LinearColor fragmentColor = LinearColor::White;
					if (InFillMode & FillMode::Color)
					{
						fragmentColor = (InVertices[0].Color * oneMinusST * invZ0 + InVertices[1].Color * s * invZ1 + InVertices[2].Color * t * invZ2) * invZ;
					}

					if (InFillMode & FillMode::Texture)
					{
						// 투영보정보간으로 보간한 해당 픽셀의 UV 값
						Vector2 targetUV = (InVertices[0].UV * oneMinusST * invZ0 + InVertices[1].UV * s * invZ1 + InVertices[2].UV * t * invZ2) * invZ;

						// 텍스쳐 매핑 진행
						LinearColor textureColor = g.GetTexture(GameEngine::BaseTexture).GetSample(targetUV);
						fragmentColor = fragmentColor * textureColor;
					}

					r.DrawPoint(fragment, FragmentShader3D(fragmentColor, paramColor));
				}
			}
		}
	}
}

