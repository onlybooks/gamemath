
#include "Precompiled.h"
using namespace CK::DDD;

// 메시
const std::size_t GameEngine::QuadMesh = std::hash<std::string>()("SM_Quad");

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
	// 사각 메시
	static float halfSize = 0.5f;
	Mesh& quadMesh = CreateMesh(GameEngine::QuadMesh);
	auto& v = quadMesh.GetVertices();
	auto& i = quadMesh.GetIndices();
	auto& uv = quadMesh.GetUVs();

	v = {
		Vector3(-1.f, -1.f, 0.f) * halfSize, Vector3(-1.f, 1.f, 0.f) * halfSize, Vector3(1.f, 1.f, 0.f) * halfSize, Vector3(1.f, -1.f, 0.f) * halfSize
	};

	i = {
		0, 2, 1, 0, 3, 2
	};

	uv = {
		Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(16.f, 48.f) / 64.f
	};

	// 스킨드 메시 설정
	quadMesh.SetMeshType(MeshType::Skinned);

	// 리깅 수행
	auto& bones = quadMesh.GetBones();
	auto& connectedBones = quadMesh.GetConnectedBones();
	auto& weights = quadMesh.GetWeights();

	bones = {
		{"left", Bone("left", Transform(Vector3(-1.f, 0.f, 0.f) * halfSize))},
		{"right", Bone("right", Transform(Vector3(1.f, 0.f, 0.f) * halfSize))}
	};
	connectedBones = { 1, 1, 1, 1 };
	weights = {
		{ {"left"}, {1.f} },
		{ {"left"}, {1.f} },
		{ {"right"}, {1.f} },
		{ {"right"}, {1.f} }
	};

	// 메시의 바운딩 볼륨 생성
	quadMesh.CalculateBounds();

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
