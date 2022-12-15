
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DDD;

// ����� �׸���
void SoftRenderer::DrawGizmo3D()
{
	auto& r = GetRenderer();
	const GameEngine& g = Get3DGameEngine();

	// �� ����� �׸���
	std::vector<Vertex3D> viewGizmo = {
		Vertex3D(Vector4(Vector3::Zero)),
		Vertex3D(Vector4(Vector3::UnitX * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitY * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitZ * _GizmoUnitLength)),
	};

	Matrix4x4 viewMatRotationOnly = g.GetMainCamera().GetViewMatrixRotationOnly();
	Matrix4x4 pvMatrix = g.GetMainCamera().GetPerspectiveViewMatrix();
	VertexShader3D(viewGizmo, viewMatRotationOnly);

	// �� �׸���
	Vector2 v0 = viewGizmo[0].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v1 = viewGizmo[1].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v2 = viewGizmo[2].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v3 = viewGizmo[3].Position.ToVector2() + _GizmoPositionOffset;
	r.DrawLine(v0, v1, LinearColor::Red);
	r.DrawLine(v0, v2, LinearColor::Green);
	r.DrawLine(v0, v3, LinearColor::Blue);

	// �ٴ� �����
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

// ���� ������Ʈ �̸�
const std::string PlayerGo("Player");
const std::string CameraTargetGo("CameraTarget");

// �� �ε�
void SoftRenderer::LoadScene3D()
{
	GameEngine& g = Get3DGameEngine();

	// �÷��̾�
	constexpr float playerScale = 100.f;

	GameObject& goPlayer = g.CreateNewGameObject(PlayerGo);
	goPlayer.SetMesh(GameEngine::CharacterMesh);
	goPlayer.SetColor(LinearColor::White);
	goPlayer.GetTransform().SetWorldScale(Vector3::One * playerScale);

	// ĳ���� ���� ǥ���� ȭ��ǥ
	Mesh& cm = g.GetMesh(goPlayer.GetMeshKey());
	for (const auto& b : cm.GetBones())
	{
		if (!b.second.HasParent())
		{
			continue;
		}
		GameObject& goBoneArrow = g.CreateNewGameObject(b.second.GetName());
		goBoneArrow.SetMesh(GameEngine::ArrowMesh);
		g.GetBoneObjectPtrs().insert({ goBoneArrow.GetName(),&goBoneArrow });
	}

	// ī�޶� ��
	GameObject& goCameraTarget = g.CreateNewGameObject(CameraTargetGo);
	goCameraTarget.GetTransform().SetWorldPosition(Vector3(0.f, 150.f, 0.f));
	goCameraTarget.SetParent(goPlayer);

	// ī�޶� ����
	CameraObject& mainCamera = g.GetMainCamera();
	mainCamera.GetTransform().SetWorldPosition(Vector3(-500.f, 800.f, 1000.f));
	mainCamera.SetLookAtRotation(goCameraTarget);
}

// �ǽ��� ���� ����

// ���� ������ ����ϴ� �Լ�
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	// ���� �������� ����ϴ� ��� �� �ֿ� ���۷���
	GameEngine& g = Get3DGameEngine();
	const InputManager& input = g.GetInputManager();

	// ���� ������ ���� ����
	static float fovSpeed = 100.f;
	static float minFOV = 15.f;
	static float maxFOV = 150.f;
	static float rotateSpeed = 180.f;
	static float moveSpeed = 500.f;

	// �Է¿� ���� ī�޶� �þ߰��� ����
	CameraObject& camera = g.GetMainCamera();
	float deltaFOV = input.GetAxis(InputAxis::WAxis) * fovSpeed * InDeltaSeconds;
	camera.SetFOV(Math::Clamp(camera.GetFOV() + deltaFOV, minFOV, maxFOV));

	// ���� �������� ����� ���� ������Ʈ ���۷���
	GameObject& goPlayer = g.GetGameObject(PlayerGo);
	GameObject& goCameraTarget = g.GetGameObject(CameraTargetGo);

	// �Է¿� ���� �̵�
	goPlayer.GetTransform().AddLocalYawRotation(-input.GetAxis(InputAxis::XAxis) * rotateSpeed * InDeltaSeconds);
	goPlayer.GetTransform().AddLocalPosition(goPlayer.GetTransform().GetLocalZ() * input.GetAxis(InputAxis::YAxis) * moveSpeed * InDeltaSeconds);

	// ī�޶��� ����
	camera.SetLookAtRotation(goCameraTarget);
}

// �ִϸ��̼� ������ ����ϴ� �Լ�
void SoftRenderer::LateUpdate3D(float InDeltaSeconds)
{
	// �ִϸ��̼� �������� ����ϴ� ��� �� �ֿ� ���۷���
	GameEngine& g = Get3DGameEngine();

	// �ִϸ��̼� ������ ���� ����
	static float elapsedTime = 0.f;
	static float neckLength = 5.f;
	static float armLegLength = 0.7f;
	static float neckDegree = 15.f;
	static float armLegDegree = 30.f;
	elapsedTime += InDeltaSeconds;

	// �ִϸ��̼��� ���� Ŀ�� ���� 
	float neckCurrent = Math::FMod(elapsedTime, neckLength) * Math::TwoPI / neckLength;
	float armLegCurrent = Math::FMod(elapsedTime, armLegLength) * Math::TwoPI / armLegLength;

	float neckCurve = sinf(neckCurrent) * neckDegree;
	float armLegCurve = sinf(armLegCurrent) * armLegDegree;

	// ĳ���� ���۷���
	GameObject& goPlayer = g.GetGameObject(PlayerGo);

	// ĳ���� �޽�
	Mesh& m = g.GetMesh(goPlayer.GetMeshKey());

	// ���� ȸ��
	Bone& neckBone = m.GetBone(GameEngine::NeckBone);
	neckBone.GetTransform().SetLocalRotation(Rotator(neckCurve, 0.f, 0.f));

	// ���� ȸ��
	Bone& leftArmBone = m.GetBone(GameEngine::LeftArmBone);
	leftArmBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, armLegCurve));

	Bone& rightArmBone = m.GetBone(GameEngine::RightArmBone);
	rightArmBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, -armLegCurve));

	// �ٸ��� ȸ��
	Bone& leftLegBone = m.GetBone(GameEngine::LeftLegBone);
	leftLegBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, -armLegCurve));

	Bone& rightLegBone = m.GetBone(GameEngine::RightLegBone);
	rightLegBone.GetTransform().SetLocalRotation(Rotator(0.f, 0.f, armLegCurve));
}

// ������ ������ ����ϴ� �Լ�
void SoftRenderer::Render3D()
{
	// ������ �������� ����ϴ� ��� �� �ֿ� ���۷���
	const GameEngine& g = Get3DGameEngine();
	auto& r = GetRenderer();
	const CameraObject& mainCamera = g.GetMainCamera();

	// ��濡 ����� �׸���
	DrawGizmo3D();

	// ������ ������ ���� ����
	const Matrix4x4 pvMatrix = mainCamera.GetPerspectiveViewMatrix();

	// ����ü �ø� �׽�Ʈ�� ���� ��� ����
	size_t totalObjects = g.GetScene().size();
	size_t culledObjects = 0;
	size_t intersectedObjects = 0;
	size_t renderedObjects = 0;

	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *(*it);
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		// �������� �ʿ��� ���� ������Ʈ�� �ֿ� ���۷����� ���
		const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());
		const TransformComponent& transform = gameObject.GetTransform();

		// ���� ��� ���
		Matrix4x4 finalMatrix = pvMatrix * transform.GetWorldMatrix();
		LinearColor finalColor = gameObject.GetColor();

		// ���� ��ȯ ��ķκ��� ����� �����İ� ����ü ����
		Matrix4x4 finalTransposedMatrix = finalMatrix.Transpose();
		std::array<Plane, 6> frustumPlanesFromMatrix = {
			Plane(-(finalTransposedMatrix[3] - finalTransposedMatrix[1])), // up
			Plane(-(finalTransposedMatrix[3] + finalTransposedMatrix[1])), // bottom
			Plane(-(finalTransposedMatrix[3] - finalTransposedMatrix[0])), // right
			Plane(-(finalTransposedMatrix[3] + finalTransposedMatrix[0])), // left 
			Plane(-(finalTransposedMatrix[3] - finalTransposedMatrix[2])),  // far
			Plane(-(finalTransposedMatrix[3] + finalTransposedMatrix[2])), // near
		};
		Frustum frustumFromMatrix(frustumPlanesFromMatrix);

		// �ٿ�� ������ ����� ����ü �ø��� ����
		Box boxBound = mesh.GetBoxBound();
		auto checkResult = frustumFromMatrix.CheckBound(boxBound);
		if (checkResult == BoundCheckResult::Outside)
		{
			culledObjects++;
			continue;
		}
		else if (checkResult == BoundCheckResult::Intersect)
		{
			// ��ģ ���� ������Ʈ�� ��迡 ����
			intersectedObjects++;
		}

		// ��Ű���̰� WireFrame�� ��� ���� �׸���
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

				// �𵨸� ���������� ���� ��ġ
				const Transform& t1 = parentBone.GetTransform().GetWorldTransform();
				const Transform& t2 = bone.GetTransform().GetWorldTransform();

				// ���� ���� ���������� ���� ��ġ
				const Transform& wt1 = t1.LocalToWorld(tGameObject);
				const Transform& wt2 = t2.LocalToWorld(tGameObject);

				Vector3 boneVector = wt2.GetPosition() - wt1.GetPosition();
				Transform tboneObject(wt1.GetPosition(), Quaternion(boneVector), Vector3(10.f, 10.f, boneVector.Size()));
				Matrix4x4 boneMatrix = pvMatrix * tboneObject.GetMatrix();
				DrawMesh3D(boneMesh, boneMatrix, _BoneWireframeColor);
			}
		}

		// �޽� �׸���
		DrawMesh3D(mesh, finalMatrix, gameObject.GetColor());

		// �׸� ��ü�� ��迡 ����
		renderedObjects++;

		if (gameObject == PlayerGo)
		{
			r.PushStatisticText("Player : " + gameObject.GetTransform().GetWorldPosition().ToString());
		}
	}
}

// �޽ø� �׸��� �Լ�
void SoftRenderer::DrawMesh3D(const Mesh& InMesh, const Matrix4x4& InMatrix, const LinearColor& InColor)
{
	size_t vertexCount = InMesh.GetVertices().size();
	size_t indexCount = InMesh.GetIndices().size();
	size_t triangleCount = indexCount / 3;

	// �������� ����� ���� ���ۿ� �ε��� ���۷� ��ȯ
	std::vector<Vertex3D> vertices(vertexCount);
	std::vector<size_t> indice(InMesh.GetIndices());
	for (size_t vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = Vector4(InMesh.GetVertices()[vi]);

		// ��ġ�� ���� ��Ű�� ���� ����
		if (InMesh.IsSkinnedMesh())
		{
			Vector4 totalPosition = Vector4::Zero;
			Weight w = InMesh.GetWeights()[vi];
			for (size_t wi = 0; wi < InMesh.GetConnectedBones()[vi]; ++wi)
			{
				std::string boneName = w.Bones[wi];
				if (InMesh.HasBone(boneName))
				{
					/** å�� ���� �ڵ�
					const Bone& b = InMesh.GetBone(boneName);
					const Transform& t = b.GetTransform().GetWorldTransform();  // �𵨸� ����
					const Transform& bindPose = b.GetBindPose(); // �𵨸� ����

					// BindPose ������ �߽����� Bone�� ���� ������ ���
					Transform boneLocal = t.WorldToLocal(bindPose);

					// BindPose �������� ���� ��ȭ
					Vector3 localPosition = bindPose.WorldToLocalVector(vertices[vi].Position.ToVector3());

					// BindPose ���������� ���� ���� ��ġ
					Vector3 skinnedLocalPosition = boneLocal.GetMatrix() * localPosition;

					// �𵨸� �������� �ٽ� ����
					Vector3 skinnedWorldPosition = bindPose.GetMatrix() * skinnedLocalPosition;

					// ����ġ�� �ݿ��� �� ���� ��ġ�� ����
					totalPosition += Vector4(skinnedWorldPosition, true) * w.Values[wi];
					*/

					// ���ʿ��� �߰� ����� ������ ������ �ڵ�
					const Transform& bindPoseTransform = InMesh.GetBindPose(boneName);
					const Transform& boneTransform = InMesh.GetBone(boneName).GetTransform().GetWorldTransform();
					Vector4 localPosition = boneTransform.GetMatrix() * bindPoseTransform.Inverse().GetMatrix() * vertices[vi].Position;

					totalPosition += localPosition * w.Values[wi];
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

	// ���� ��ȯ ����
	VertexShader3D(vertices, InMatrix);

	// �ﰢ�� ���� �׸���
	for (int ti = 0; ti < triangleCount; ++ti)
	{
		int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
		std::vector<Vertex3D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };

		// ������ǥ�迡�� Ŭ������ ���� ����
		std::vector<PerspectiveTest> testPlanes = {
			{ TestFuncW0, EdgeFuncW0 },
			{ TestFuncNY, EdgeFuncNY },
			{ TestFuncPY, EdgeFuncPY },
			{ TestFuncNX, EdgeFuncNX },
			{ TestFuncPX, EdgeFuncPX },
			{ TestFuncFar, EdgeFuncFar },
			{ TestFuncNear, EdgeFuncNear }
		};

		// ������ǥ�迡�� Ŭ���� ����
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

// �ﰢ���� �׸��� �Լ�
void SoftRenderer::DrawTriangle3D(std::vector<Vertex3D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
	auto& r = GetRenderer();
	const GameEngine& g = Get3DGameEngine();
	const CameraObject& mainCamera = g.GetMainCamera();

	// ī�޶��� ������ ����� ��
	float n = mainCamera.GetNearZ();
	float f = mainCamera.GetFarZ();

	// Ŭ�� ��ǥ�� NDC ��ǥ�� ����
	for (auto& v : InVertices)
	{
		// ���� ������ ���, �ణ �������ش�.
		if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;
	}

	// �����̽� �ø�
	Vector3 edge1 = (InVertices[1].Position - InVertices[0].Position).ToVector3();
	Vector3 edge2 = (InVertices[2].Position - InVertices[0].Position).ToVector3();
	Vector3 faceNormal = -edge1.Cross(edge2);
	Vector3 viewDirection = Vector3::UnitZ;
	if (faceNormal.Dot(viewDirection) >= 0.f)
	{
		return;
	}

	// NDC ��ǥ�� ȭ�� ��ǥ�� �ø���
	for (auto& v : InVertices)
	{
		v.Position.X *= _ScreenSize.X * 0.5f;
		v.Position.Y *= _ScreenSize.Y * 0.5f;
	}

	if (IsWireframeDrawing())
	{
		LinearColor finalColor = _WireframeColor;
		if (InColor == _BoneWireframeColor)
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

		// �ﰢ�� ĥ�ϱ�
		// �ﰢ���� ���� ����
		Vector2 minPos(Math::Min3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Min3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));
		Vector2 maxPos(Math::Max3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Max3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));

		// �����߽���ǥ�� ���� ���� ���ͷ� ��ȯ
		Vector2 u = InVertices[1].Position.ToVector2() - InVertices[0].Position.ToVector2();
		Vector2 v = InVertices[2].Position.ToVector2() - InVertices[0].Position.ToVector2();

		// ���� �и� �� ( uu * vv - uv * uv )
		float udotv = u.Dot(v);
		float vdotv = v.Dot(v);
		float udotu = u.Dot(u);
		float denominator = udotv * udotv - vdotv * udotu;

		// ��ȭ �ﰢ�� ����.
		if (denominator == 0.f)
		{
			return;
		}

		float invDenominator = 1.f / denominator;

		ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
		ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

		// �� ���� ȭ�� ���� ����� ��� Ŭ���� ó��
		lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
		lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
		upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
		upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

		// �� �������� ������ �� ������ z��
		float invZ0 = 1.f / InVertices[0].Position.W;
		float invZ1 = 1.f / InVertices[1].Position.W;
		float invZ2 = 1.f / InVertices[2].Position.W;

		// �ﰢ�� ���� �� ��� ���� �����ϰ� ��ĥ
		for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
		{
			for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
			{
				ScreenPoint fragment = ScreenPoint(x, y);
				Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
				Vector2 w = pointToTest - InVertices[0].Position.ToVector2();
				float wdotu = w.Dot(u);
				float wdotv = w.Dot(v);

				float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
				float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
				float oneMinusST = 1.f - s - t;

				// ���������� ����� ���� �и�
				float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
				float invZ = 1.f / z;

				if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
				{
					// ���� �׽���
					float newDepth = InVertices[0].Position.Z * oneMinusST + InVertices[1].Position.Z * s + InVertices[2].Position.Z * t;
					float prevDepth = r.GetDepthBufferValue(fragment);
					if (newDepth < prevDepth)
					{
						// �ȼ��� ó���ϱ� �� ���� ���� ���ۿ� ����
						r.SetDepthBufferValue(fragment, newDepth);
					}
					else
					{
						// �̹� �տ� ���� �׷��������Ƿ� �ȼ��׸���� ����
						continue;
					}

					if (IsDepthBufferDrawing())
					{
						// ī�޶�κ����� �Ÿ��� ���� �����ϰ� �����ϴ� ��� ������ ��ȯ
						float grayScale = (invZ - n) / (f - n);

						// ���� ���� �׸���
						r.DrawPoint(fragment, LinearColor::White * grayScale);
					}
					else
					{
						// ���� ���������� UV ��ǥ
						Vector2 targetUV = (InVertices[0].UV * oneMinusST * invZ0 + InVertices[1].UV * s * invZ1 + InVertices[2].UV * t * invZ2) * invZ;
						r.DrawPoint(fragment, FragmentShader3D(mainTexture.GetSample(targetUV), InColor));
					}
				}
			}
		}
	}
}