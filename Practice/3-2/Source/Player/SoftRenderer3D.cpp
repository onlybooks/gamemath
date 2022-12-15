
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
	Matrix4x4 pvMatrix = g.GetMainCamera().GetPerspectiveViewMatrix();
	VertexShader3D(viewGizmo, viewMatRotationOnly);

	// 축 그리기
	Vector2 v0 = viewGizmo[0].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v1 = viewGizmo[1].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v2 = viewGizmo[2].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v3 = viewGizmo[3].Position.ToVector2() + _GizmoPositionOffset;
	r.DrawLine(v0, v1, LinearColor::Red);
	r.DrawLine(v0, v2, LinearColor::Green);
	r.DrawLine(v0, v3, LinearColor::Blue);

	// 바닥 기즈모
	DrawMode prevShowMode = GetDrawMode();
	SetDrawMode(DrawMode::Wireframe);
	{
		static float planeScale = 100.f;
		const Mesh& planeMesh = g.GetMesh(GameEngine::PlaneMesh);
		Transform pt(Vector3::Zero, Quaternion::Identity, Vector3::One * planeScale);
		DrawMesh3D(planeMesh, pvMatrix * pt.GetMatrix(), _WireframeColor);
	}
	SetDrawMode(prevShowMode);
}

// 게임 오브젝트 목록
static const std::string PlayerGo = "Player";
static const std::string CameraRigGo = "CameraRig";

// 최초 씬 로딩을 담당하는 함수
void SoftRenderer::LoadScene3D()
{
	GameEngine& g = Get3DGameEngine();

	// 플레이어
	constexpr float playerScale = 100.f;

	GameObject& goPlayer = g.CreateNewGameObject(PlayerGo);
	goPlayer.SetMesh(GameEngine::CharacterMesh);
	goPlayer.GetTransform().SetWorldScale(Vector3::One * playerScale);

	// 캐릭터 본을 표시할 화살표
	Mesh& cm = g.GetMesh(goPlayer.GetMeshKey());
	for (const auto& b : cm.GetBones())
	{
		if (!b.second.HasParent())
		{
			continue;
		}
		GameObject& goBoneArrow = g.CreateNewGameObject(b.second.GetName());
		goBoneArrow.SetGameObjectType(GameObjectType::Gizmo);
		goBoneArrow.SetMesh(GameEngine::ArrowMesh);
		goBoneArrow.SetColor(LinearColor::Red);
		g.GetBoneObjectPtrs().insert({ goBoneArrow.GetName(),&goBoneArrow });
	}

	// 카메라 릭
	GameObject& goCameraRig = g.CreateNewGameObject(CameraRigGo);
	goCameraRig.GetTransform().SetWorldPosition(Vector3(0.f, 150.f, 0.f));

	// 카메라 설정
	CameraObject& mainCamera = g.GetMainCamera();
	mainCamera.GetTransform().SetWorldPosition(Vector3(-500.f, 800.f, 1000.f));
	mainCamera.SetParent(goCameraRig);
	mainCamera.SetLookAtRotation(goCameraRig);
}

// 게임 로직과 렌더링 로직이 공유하는 변수

// 게임 로직을 담당하는 함수
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	// 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
	GameEngine& g = Get3DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직의 로컬 변수
	static float fovSpeed = 100.f;
	static float rotateSpeed = 180.f;
	static float moveSpeed = 500.f;

	// 게임 로직에서 사용할 게임 오브젝트 레퍼런스
	GameObject& goPlayer = g.GetGameObject(PlayerGo);
	GameObject& goCameraRig = g.GetGameObject(CameraRigGo);
	CameraObject& camera = g.GetMainCamera();

	// 입력에 따른 이동
	goPlayer.GetTransform().AddLocalYawRotation(-input.GetAxis(InputAxis::XAxis) * rotateSpeed * InDeltaSeconds);
	goPlayer.GetTransform().AddLocalPosition(goPlayer.GetTransform().GetLocalZ() * input.GetAxis(InputAxis::YAxis) * moveSpeed * InDeltaSeconds);

	// 카메라 화각 설정
	float newFOV = Math::Clamp(camera.GetFOV() + input.GetAxis(InputAxis::ZAxis) * fovSpeed * InDeltaSeconds, 5.f, 179.f);
	camera.SetFOV(newFOV);
}

// 애니메이션 로직을 담당하는 함수
void SoftRenderer::LateUpdate3D(float InDeltaSeconds)
{
	// 애니메이션 로직에서 사용하는 모듈 내 주요 레퍼런스
	GameEngine& g = Get3DGameEngine();

	// 애니메이션 로직의 로컬 변수
	static float elapsedTime = 0.f;
	static float neckLength = 5.f;
	static float armLegLength = 0.7f;
	static float neckDegree = 15.f;
	static float armLegDegree = 30.f;
	elapsedTime += InDeltaSeconds;

	// 애니메이션을 위한 커브 생성 
	float armLegCurrent = Math::FMod(elapsedTime, armLegLength) * Math::TwoPI / armLegLength;
	float neckCurrent = Math::FMod(elapsedTime, neckLength) * Math::TwoPI / neckLength;

	float armLegCurve = sinf(armLegCurrent) * armLegDegree;
	float neckCurve = sinf(neckCurrent) * neckDegree;

	// 캐릭터 레퍼런스
	GameObject& goPlayer = g.GetGameObject(PlayerGo);

	// 캐릭터 메시
	Mesh& m = g.GetMesh(goPlayer.GetMeshKey());

	// 목의 회전
	Bone& neckBone = m.GetBone(GameEngine::NeckBone);
	neckBone.GetTransform().SetLocalRotation(Rotator(neckCurve, 0.f, 0.f));

	// 팔의 회전
	Bone& leftArmBone = m.GetBone(GameEngine::LeftArmBone);
	leftArmBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, armLegCurve));

	Bone& rightArmBone = m.GetBone(GameEngine::RightArmBone);
	rightArmBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, -armLegCurve));

	// 다리의 회전
	Bone& leftLegBone = m.GetBone(GameEngine::LeftLegBone);
	leftLegBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, -armLegCurve));

	Bone& rightLegBone = m.GetBone(GameEngine::RightLegBone);
	rightLegBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, armLegCurve));
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
	const ScreenPoint viewportSize = mainCamera.GetViewportSize();

	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *(*it);
		const TransformComponent& transform = gameObject.GetTransform();
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		// 최종 변환 행렬
		Matrix4x4 finalMatrix = pvMatrix * transform.GetWorldMatrix();

		// 최종 변환 행렬로부터 평면의 방정식과 절두체 생성
		Matrix4x4 finalTranposedMatrix = finalMatrix.Transpose();
		std::array<Plane, 6> frustumPlanesFromMatrix = {
			Plane(-(finalTranposedMatrix[3] - finalTranposedMatrix[1])), // up
			Plane(-(finalTranposedMatrix[3] + finalTranposedMatrix[1])), // bottom
			Plane(-(finalTranposedMatrix[3] - finalTranposedMatrix[0])), // right
			Plane(-(finalTranposedMatrix[3] + finalTranposedMatrix[0])), // left 
			Plane(-(finalTranposedMatrix[3] - finalTranposedMatrix[2])),  // far
			Plane(-(finalTranposedMatrix[3] + finalTranposedMatrix[2])), // near
		};
		Frustum frustumFromMatrix(frustumPlanesFromMatrix);

		// 메시 정보 얻어오기
		const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());

		// 바운딩 영역은 로컬 정보를 그대로 사용
		const Box& boxBound = mesh.GetBoxBound();

		// 절두체에서 로컬 바운딩 정보로 판정
		auto checkResult = frustumFromMatrix.CheckBound(boxBound);
		if (checkResult == BoundCheckResult::Outside)
		{
			continue;
		}

		// 스키닝이고 WireFrame인 경우 본을 그리기
		if (mesh.IsSkinnedMesh() && IsWireframeDrawing())
		{
			const Mesh& boneMesh = g.GetMesh(GameEngine::ArrowMesh);
			for (const auto& b : mesh.GetBones())
			{
				if (!b.second.HasParent())
				{
					continue;
				}
				const Bone& bone = b.second;
				const Bone& parentBone = mesh.GetBone(bone.GetParentName());
				const Transform& tGameObject = transform.GetWorldTransform();

				// 모델링 공간에서의 본의 위치
				const Transform& t1 = parentBone.GetTransform().GetWorldTransform();
				const Transform& t2 = bone.GetTransform().GetWorldTransform();

				// 게임 월드 공간에서의 본의 위치
				const Transform& wt1 = t1.LocalToWorld(tGameObject);
				const Transform& wt2 = t2.LocalToWorld(tGameObject);

				Vector3 boneVector = wt2.GetPosition() - wt1.GetPosition();
				Transform tboneObject(wt1.GetPosition(), Quaternion(boneVector), Vector3(10.f, 10.f, boneVector.Size()));
				Matrix4x4 boneMatrix = pvMatrix * tboneObject.GetMatrix();
				DrawMesh3D(boneMesh, boneMatrix, LinearColor::Red);
			}
		}

		// 메시 그리기
		DrawMesh3D(mesh, finalMatrix, LinearColor::White);

		// 플레이어 위치 정보
		if (gameObject == PlayerGo)
		{
			r.PushStatisticText("Player:" + gameObject.GetTransform().GetWorldPosition().ToString());
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

		// 위치에 대해 스키닝 연산 수행
		if (InMesh.IsSkinnedMesh())
		{
			Vector4 totalPosition = Vector4::Zero;
			Weight w = InMesh.GetWeights()[vi];
			for (size_t wi = 0; wi < InMesh.GetConnectedBones()[vi]; ++wi)
			{
				std::string boneName = w.Bones[wi];
				if (InMesh.HasBone(boneName))
				{
					const Bone& b = InMesh.GetBone(boneName);
					const Transform& t = b.GetTransform().GetWorldTransform();  // 월드 공간
					const Transform& bindPose = b.GetBindPose(); // 월드 공간

					// BindPose 공간을 중심으로 Bone의 로컬 공간을 계산
					Transform boneLocal = t.WorldToLocal(bindPose);

					// BindPose 공간으로 점을 변화
					Vector3 localPosition = bindPose.WorldToLocalVector(vertices[vi].Position.ToVector3());

					// BindPose 공간에서의 점의 최종 위치
					Vector3 skinnedLocalPosition = boneLocal.GetMatrix() * localPosition;

					// 월드 공간으로 다시 변경
					Vector3 skinnedWorldPosition = bindPose.GetMatrix() * skinnedLocalPosition;

					// 가중치를 곱해서 더해줌
					totalPosition += Vector4(skinnedWorldPosition, true) * w.Values[wi];
				}
			}

			vertices[vi].Position = totalPosition;
		}

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

	// 삼각형 별로 그리기
	for (int ti = 0; ti < triangleCount; ++ti)
	{
		int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
		std::vector<Vertex3D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };

		// 동차좌표계에서 클리핑을 위한 설정
		std::vector<PerspectiveTest> testPlanes = {
			{ TestFuncW0, EdgeFuncW0 },
			{ TestFuncNY, EdgeFuncNY },
			{ TestFuncPY, EdgeFuncPY },
			{ TestFuncNX, EdgeFuncNX },
			{ TestFuncPX, EdgeFuncPX },
			{ TestFuncFar, EdgeFuncFar },
			{ TestFuncNear, EdgeFuncNear }
		};

		// 동차좌표계에서 클리핑 진행
		for (auto& p : testPlanes)
		{
			p.ClipTriangles(tvs);
		}

		size_t triangles = tvs.size() / 3;
		for (size_t ti = 0; ti < triangles; ++ti)
		{
			size_t si = ti * 3;
			std::vector<Vertex3D> sub(tvs.begin() + si, tvs.begin() + si + 3);
			DrawTriangle3D(sub, InColor, FillMode::Color);
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
	// 왼손 좌표계를 사용하므로 반대 방향으로 설정
	Vector3 faceNormal = -edge1.Cross(edge2);
	Vector3 viewDirection = Vector3::UnitZ;
	if (faceNormal.Dot(viewDirection) >= 0.f)
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
		if (InColor != LinearColor::White)
		{
			finalColor = InColor;
		}

		r.DrawLine(InVertices[0].Position, InVertices[1].Position, finalColor);
		r.DrawLine(InVertices[0].Position, InVertices[2].Position, finalColor);
		r.DrawLine(InVertices[1].Position, InVertices[2].Position, finalColor);
	}
	else
	{
		const Texture& mainTexture = g.GetTexture(GameEngine::BaseTexture);

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

		// 퇴화 삼각형 판정.
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

					// 깊이 버퍼 테스팅
					float newDepth = (InVertices[0].Position.Z * oneMinusST * invZ0 + InVertices[1].Position.Z * s * invZ1 + InVertices[2].Position.Z * t * invZ2) * invZ;
					float prevDepth = r.GetDepthBufferValue(fragment);
					if (newDepth < prevDepth)
					{
						// 픽셀을 처리하기 전 깊이 값을 버퍼에 보관
						r.SetDepthBufferValue(fragment, newDepth);
					}
					else
					{
						// 이미 앞에 무언가 그려져있으므로 픽셀그리기는 생략
						continue;
					}

					if (IsDepthBufferDrawing())
					{
						float n = g.GetMainCamera().GetNearZ();
						float f = g.GetMainCamera().GetFarZ();

						// 시각화를 위해 선형화된 흑백 값으로 변환
						float grayScale = (invZ - n) / (f - n);

						// 뎁스 버퍼 그리기
						r.DrawPoint(fragment, LinearColor::White * grayScale);
					}
					else
					{
						// 최종 보정보간된 UV 좌표
						Vector2 targetUV = (InVertices[0].UV * oneMinusST * invZ0 + InVertices[1].UV * s * invZ1 + InVertices[2].UV * t * invZ2) * invZ;
						r.DrawPoint(fragment, FragmentShader3D(mainTexture.GetSample(targetUV), InColor));
					}
				}
			}
		}
	}
}

