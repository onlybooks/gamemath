
#include "Precompiled.h"
using namespace CK::DDD;

// 본 명칭
const std::string GameEngine::RootBone("RootBone");
const std::string GameEngine::PelvisBone("PelvisBone");
const std::string GameEngine::SpineBone("SpineBone");
const std::string GameEngine::LeftArmBone("LeftArmBone");
const std::string GameEngine::RightArmBone("RightArmBone");
const std::string GameEngine::NeckBone("NeckBone");
const std::string GameEngine::LeftLegBone("LeftLegBone");
const std::string GameEngine::RightLegBone("RightLegBone");

// 메시
const std::size_t GameEngine::CharacterMesh = std::hash<std::string>()("SK_Steve");
const std::size_t GameEngine::ArrowMesh = std::hash<std::string>()("SM_Arrow");
const std::size_t GameEngine::PlaneMesh = std::hash<std::string>()("SM_Plane");

// 텍스처
const std::size_t GameEngine::BaseTexture = std::hash<std::string>()("Base");
const std::string GameEngine::CharacterTexturePath("CKMan.png");

struct GameObjectCompare
{
	bool operator()(const std::unique_ptr<GameObject>& lhs, std::size_t rhs)
	{
		return lhs->GetHash() < rhs;
	}
};

void GameEngine::OnScreenResize(const ScreenPoint& InScreenSize)
{
	// 화면 크기의 설정
	_ScreenSize = InScreenSize;
	_MainCamera.SetViewportSize(_ScreenSize);
}

bool GameEngine::Init()
{
	// 이미 초기화되어 있으면 초기화 진행하지 않음.
	if (_IsInitialized)
	{
		return true;
	}

	// 화면 크기가 올바로 설정되어 있는지 확인
	if (_ScreenSize.HasZero())
	{
		return false;
	}

	if (!_InputManager.IsInputReady())
	{
		return false;
	}

	if (!LoadResources())
	{
		return false;
	}

	_IsInitialized = true;
	return _IsInitialized;
}

bool GameEngine::LoadResources()
{
	// 캐릭터 메시 생성
	constexpr Vector3 headSize(0.5f, 0.5f, 0.5f);
	constexpr Vector3 bodySize(0.5f, 0.75f, 0.25f);
	constexpr Vector3 armLegSize(0.25f, 0.75f, 0.25f);

	constexpr BYTE totalCharacterParts = 6;
	Mesh& characterMesh = CreateMesh(GameEngine::CharacterMesh);
	auto& v = characterMesh.GetVertices();
	auto& i = characterMesh.GetIndices();
	auto& uv = characterMesh.GetUVs();

	static constexpr std::array<Vector3, totalCharacterParts> cubeMeshSize = {
		headSize, bodySize, armLegSize, armLegSize, armLegSize, armLegSize
	};

	static constexpr std::array<Vector3, totalCharacterParts> cubeMeshOffset = {
		Vector3(0.f, 3.5f, 0.f), Vector3(0.f, 2.25f, 0.f), Vector3(-0.75f, 2.25f, 0.f), Vector3(0.75f, 2.25f, 0.f), Vector3(-0.25f, 0.75f, 0.f), Vector3(0.25f, 0.75f, 0.f)
	};

	for (size_t part = 0; part < totalCharacterParts; part++)
	{
		std::transform(cubeMeshPositions.begin(), cubeMeshPositions.end(), std::back_inserter(v), [&](auto& p) { return p * cubeMeshSize[part] + cubeMeshOffset[part]; });
		std::transform(cubeMeshIndice.begin(), cubeMeshIndice.end(), std::back_inserter(i), [&](auto& p) { return p + 24 * part; });
	}

	uv = {
		// HeadRight
		Vector2(0.f, 48.f) / 64.f, Vector2(0.f, 56.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(8.f, 48.f) / 64.f,
		// HeadFront
		Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(16.f, 48.f) / 64.f,
		// HeadBack
		Vector2(24.f, 48.f) / 64.f, Vector2(24.f, 56.f) / 64.f, Vector2(32.f, 56.f) / 64.f, Vector2(32.f, 48.f) / 64.f,
		// HeadLeft
		Vector2(16.f, 48.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(24.f, 56.f) / 64.f, Vector2(24.f, 48.f) / 64.f,
		// HeadTop
		Vector2(8.f, 56.f) / 64.f, Vector2(8.f, 64.f) / 64.f, Vector2(16.f, 64.f) / 64.f, Vector2(16.f, 56.f) / 64.f,
		// HeadBottom
		Vector2(16.f, 56.f) / 64.f, Vector2(16.f, 64.f) / 64.f, Vector2(24.f, 64.f) / 64.f, Vector2(24.f, 56.f) / 64.f,
		// BodyRight
		Vector2(16.f, 32.f) / 64.f, Vector2(16.f, 44.f) / 64.f, Vector2(20.f, 44.f) / 64.f, Vector2(20.f, 32.f) / 64.f,
		// BodyFront
		Vector2(20.f, 32.f) / 64.f, Vector2(20.f, 44.f) / 64.f, Vector2(28.f, 44.f) / 64.f, Vector2(28.f, 32.f) / 64.f,
		// BodyBack
		Vector2(32.f, 32.f) / 64.f, Vector2(32.f, 44.f) / 64.f, Vector2(40.f, 44.f) / 64.f, Vector2(40.f, 32.f) / 64.f,
		// BodyLeft
		Vector2(28.f, 32.f) / 64.f, Vector2(28.f, 44.f) / 64.f, Vector2(32.f, 44.f) / 64.f, Vector2(32.f, 32.f) / 64.f,
		// BodyTop
		Vector2(20.f, 44.f) / 64.f, Vector2(20.f, 48.f) / 64.f, Vector2(28.f, 48.f) / 64.f, Vector2(28.f, 44.f) / 64.f,
		// BodyBottom
		Vector2(28.f, 44.f) / 64.f, Vector2(28.f, 48.f) / 64.f, Vector2(36.f, 48.f) / 64.f, Vector2(36.f, 44.f) / 64.f,
		// LeftArmRight
		Vector2(32.f, 0.f) / 64.f, Vector2(32.f, 12.f) / 64.f, Vector2(36.f, 12.f) / 64.f, Vector2(36.f, 0.f) / 64.f,
		// LeftArmFront
		Vector2(36.f, 0.f) / 64.f, Vector2(36.f, 12.f) / 64.f, Vector2(40.f, 12.f) / 64.f, Vector2(40.f, 0.f) / 64.f,
		// LeftArmBack
		Vector2(44.f, 0.f) / 64.f, Vector2(44.f, 12.f) / 64.f, Vector2(48.f, 12.f) / 64.f, Vector2(48.f, 0.f) / 64.f,
		// LeftArmLeft
		Vector2(40.f, 0.f) / 64.f, Vector2(40.f, 12.f) / 64.f, Vector2(44.f, 12.f) / 64.f, Vector2(44.f, 0.f) / 64.f,
		// LeftArmTop
		Vector2(36.f, 12.f) / 64.f, Vector2(36.f, 16.f) / 64.f, Vector2(40.f, 16.f) / 64.f, Vector2(40.f, 12.f) / 64.f,
		// LeftArmBottom
		Vector2(40.f, 12.f) / 64.f, Vector2(40.f, 16.f) / 64.f, Vector2(44.f, 16.f) / 64.f, Vector2(44.f, 12.f) / 64.f,
		// RightArmRight
		Vector2(40.f, 32.f) / 64.f, Vector2(40.f, 44.f) / 64.f, Vector2(44.f, 44.f) / 64.f, Vector2(44.f, 32.f) / 64.f,
		// RightArmFront
		Vector2(44.f, 32.f) / 64.f, Vector2(44.f, 44.f) / 64.f, Vector2(48.f, 44.f) / 64.f, Vector2(48.f, 32.f) / 64.f,
		// RightArmBack
		Vector2(52.f, 32.f) / 64.f, Vector2(52.f, 44.f) / 64.f, Vector2(56.f, 44.f) / 64.f, Vector2(56.f, 32.f) / 64.f,
		// RightArmLeft
		Vector2(48.f, 32.f) / 64.f, Vector2(48.f, 44.f) / 64.f, Vector2(52.f, 44.f) / 64.f, Vector2(52.f, 32.f) / 64.f,
		// RightArmTop
		Vector2(44.f, 44.f) / 64.f, Vector2(44.f, 48.f) / 64.f, Vector2(48.f, 48.f) / 64.f, Vector2(48.f, 44.f) / 64.f,
		// RightArmBottom
		Vector2(48.f, 44.f) / 64.f, Vector2(48.f, 48.f) / 64.f, Vector2(52.f, 48.f) / 64.f, Vector2(52.f, 44.f) / 64.f,
		// LeftLegRight
		Vector2(16.f, 0.f) / 64.f, Vector2(16.f, 12.f) / 64.f, Vector2(20.f, 12.f) / 64.f, Vector2(20.f, 0.f) / 64.f,
		// LeftLegFront
		Vector2(20.f, 0.f) / 64.f, Vector2(20.f, 12.f) / 64.f, Vector2(24.f, 12.f) / 64.f, Vector2(24.f, 0.f) / 64.f,
		// LeftLegBack
		Vector2(28.f, 0.f) / 64.f, Vector2(28.f, 12.f) / 64.f, Vector2(32.f, 12.f) / 64.f, Vector2(32.f, 0.f) / 64.f,
		// LeftLegLeft
		Vector2(24.f, 0.f) / 64.f, Vector2(24.f, 12.f) / 64.f, Vector2(28.f, 12.f) / 64.f, Vector2(28.f, 0.f) / 64.f,
		// LeftLegTop
		Vector2(20.f, 12.f) / 64.f, Vector2(20.f, 16.f) / 64.f, Vector2(24.f, 16.f) / 64.f, Vector2(24.f, 12.f) / 64.f,
		// LeftLegBottom
		Vector2(24.f, 12.f) / 64.f, Vector2(24.f, 16.f) / 64.f, Vector2(28.f, 16.f) / 64.f, Vector2(28.f, 12.f) / 64.f,
		// RightLegRight
		Vector2(0.f, 32.f) / 64.f, Vector2(0.f, 44.f) / 64.f, Vector2(4.f, 44.f) / 64.f, Vector2(4.f, 32.f) / 64.f,
		// RightLegFront
		Vector2(4.f, 32.f) / 64.f, Vector2(4.f, 44.f) / 64.f, Vector2(8.f, 44.f) / 64.f, Vector2(8.f, 32.f) / 64.f,
		// RightLegBack
		Vector2(12.f, 32.f) / 64.f, Vector2(12.f, 44.f) / 64.f, Vector2(16.f, 44.f) / 64.f, Vector2(16.f, 32.f) / 64.f,
		// RightLegLeft
		Vector2(8.f, 32.f) / 64.f, Vector2(8.f, 44.f) / 64.f, Vector2(12.f, 44.f) / 64.f, Vector2(12.f, 32.f) / 64.f,
		// RightLegTop
		Vector2(4.f, 44.f) / 64.f, Vector2(4.f, 48.f) / 64.f, Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 44.f) / 64.f,
		// RightLegBottom
		Vector2(8.f, 44.f) / 64.f, Vector2(8.f, 48.f) / 64.f, Vector2(12.f, 48.f) / 64.f, Vector2(12.f, 44.f) / 64.f
	};

	// 캐릭터 스켈레탈 메시 설정
	characterMesh.SetMeshType(MeshType::Skinned);
	auto& cb = characterMesh.GetConnectedBones();
	auto& w = characterMesh.GetWeights();
	auto& bones = characterMesh.GetBones();

	// 본 생성
	bones = {
		{ GameEngine::RootBone, Bone(GameEngine::RootBone, Transform()) },
		{ GameEngine::PelvisBone, Bone(GameEngine::PelvisBone, Transform(Vector3(0.f, 1.5f, 0.f))) },
		{ GameEngine::SpineBone, Bone(GameEngine::SpineBone, Transform(Vector3(0.f, 2.25f, 0.f))) },
		{ GameEngine::LeftArmBone, Bone(GameEngine::LeftArmBone, Transform(Vector3(-0.75f, 3.f, 0.f))) },
		{ GameEngine::RightArmBone, Bone(GameEngine::RightArmBone, Transform(Vector3(0.75f, 3.f, 0.f))) },
		{ GameEngine::LeftLegBone, Bone(GameEngine::LeftLegBone, Transform(Vector3(0.25f, 1.5f, 0.f))) },
		{ GameEngine::RightLegBone, Bone(GameEngine::RightLegBone, Transform(Vector3(-0.25f, 1.5f, 0.f))) },
		{ GameEngine::NeckBone, Bone(GameEngine::NeckBone, Transform(Vector3(0.f, 3.f, 0.f))) }
	};

	// 본의 계층 구조 생성
	Bone& root = characterMesh.GetBone(GameEngine::RootBone);
	Bone& pelvis = characterMesh.GetBone(GameEngine::PelvisBone);
	pelvis.SetParent(root);
	Bone& spine = characterMesh.GetBone(GameEngine::SpineBone);
	spine.SetParent(pelvis);
	Bone& leftArm = characterMesh.GetBone(GameEngine::LeftArmBone);
	leftArm.SetParent(spine);
	Bone& rightArm = characterMesh.GetBone(GameEngine::RightArmBone);
	rightArm.SetParent(spine);
	Bone& leftLeg = characterMesh.GetBone(GameEngine::LeftLegBone);
	leftLeg.SetParent(pelvis);
	Bone& rightLeg = characterMesh.GetBone(GameEngine::RightLegBone);
	rightLeg.SetParent(pelvis);
	Bone& neck = characterMesh.GetBone(GameEngine::NeckBone);
	neck.SetParent(spine);

	// 메시에 리깅 
	static std::array<std::string, 6> boneOrder = {
		GameEngine::NeckBone, GameEngine::SpineBone, GameEngine::LeftArmBone, GameEngine::RightArmBone, GameEngine::LeftLegBone, GameEngine::RightLegBone
	};

	cb.resize(v.size());
	w.resize(v.size());
	std::fill(cb.begin(), cb.end(), 1);

	for (size_t part = 0; part < 6; part++)
	{
		Weight weight;
		weight.Bones = { boneOrder[part] };
		weight.Values = { 1.f };
		auto startIt = w.begin() + part * 24;
		std::fill(startIt, startIt + 24, weight);
	}

	characterMesh.CalculateBounds();

	// 화살표 메시 (본의 표시 용도)
	Mesh& arrow = CreateMesh(GameEngine::ArrowMesh);
	arrow.GetVertices().resize(arrowPositions.size());
	arrow.GetIndices().resize(arrowIndice.size());
	arrow.GetColors().resize(arrowPositions.size());
	std::copy(arrowPositions.begin(), arrowPositions.end(), arrow.GetVertices().begin());
	std::copy(arrowIndice.begin(), arrowIndice.end(), arrow.GetIndices().begin());
	std::fill(arrow.GetColors().begin(), arrow.GetColors().end(), LinearColor::Gray);

	// 바닥 메시 (기즈모 용)
	int planeHalfSize = 3;
	Mesh& plane = CreateMesh(GameEngine::PlaneMesh);
	for (int z = -planeHalfSize; z <= planeHalfSize; z++)
	{
		for (int x = -planeHalfSize; x <= planeHalfSize; x++)
		{
			plane.GetVertices().push_back(Vector3((float)x, 0.f, (float)z));
		}
	}

	int xIndex = 0;
	for (int tx = 0; tx < planeHalfSize * 2; tx++)
	{
		for (int ty = 0; ty < planeHalfSize * 2; ty++)
		{
			int v0 = xIndex + tx + (planeHalfSize * 2 + 1) * (ty + 1);
			int v1 = xIndex + tx + (planeHalfSize * 2 + 1) * ty;
			int v2 = v1 + 1;
			int v3 = v0 + 1;
			std::vector<size_t> quad = { (size_t)v0, (size_t)v2, (size_t)v1, (size_t)v0, (size_t)v3, (size_t)v2 };
			plane.GetIndices().insert(plane.GetIndices().end(), quad.begin(), quad.end());
		}
	}

	// 텍스쳐 로딩
	Texture& diffuseTexture = CreateTexture(GameEngine::BaseTexture, GameEngine::CharacterTexturePath);
	assert(diffuseTexture.IsIntialized());

	return true;
}

Mesh& GameEngine::CreateMesh(const std::size_t& InKey)
{
	auto meshPtr = std::make_unique<Mesh>();
	_Meshes.insert({ InKey, std::move(meshPtr) });
	return *_Meshes.at(InKey).get();
}

Texture& GameEngine::CreateTexture(const std::size_t& InKey, const std::string& InTexturePath)
{
	auto texturePtr = std::make_unique<Texture>(InTexturePath);
	_Textures.insert({ InKey, std::move(texturePtr) });
	return *_Textures.at(InKey).get();
}

GameObject& GameEngine::CreateNewGameObject(const std::string& InName)
{
	std::size_t inHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), inHash, GameObjectCompare());

	auto newGameObject = std::make_unique<GameObject>(InName);
	if (it != _Scene.end())
	{
		std::size_t targetHash = (*it)->GetHash();
		if (targetHash == inHash)
		{
			// 중복된 키 발생. 무시.
			assert(false);
			return GameObject::Invalid;
		}
		else if (targetHash < inHash)
		{
			_Scene.insert(it + 1, std::move(newGameObject));
		}
		else
		{
			_Scene.insert(it, std::move(newGameObject));
		}
	}
	else
	{
		_Scene.push_back(std::move(newGameObject));
	}

	return GetGameObject(InName);
}

GameObject& GameEngine::GetGameObject(const std::string& InName)
{
	std::size_t targetHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), targetHash, GameObjectCompare());

	return (it != _Scene.end()) ? *(*it).get() : GameObject::Invalid;
}
