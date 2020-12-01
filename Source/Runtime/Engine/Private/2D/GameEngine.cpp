
#include "Precompiled.h"
#include <random>
using namespace CK::DD;

// 메시
const std::size_t GameEngine::QuadMesh = std::hash<std::string>()("SM_Quad");

// 게임 오브젝트
const std::string GameEngine::PlayerGo("Player");

// 텍스처
const std::size_t GameEngine::DiffuseTexture = std::hash<std::string>()("Diffuse");
const std::string GameEngine::SteveTexturePath("Steve.png");

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

	if (!LoadScene())
	{
		return false;
	}

	_IsInitialized = true;
	return true;
}

bool GameEngine::LoadResources()
{
	// 메시 데이터 로딩
	Mesh& quadMesh = CreateMesh(GameEngine::QuadMesh);

	constexpr float squareHalfSize = 0.5f;
	constexpr int vertexCount = 4;
	constexpr int triangleCount = 2;
	constexpr int indexCount = triangleCount * 3;

	auto& v = quadMesh.GetVertices();
	auto& i = quadMesh.GetIndices();
	auto& uv = quadMesh.GetUVs();

	v = {
		Vector2(-squareHalfSize, -squareHalfSize),
		Vector2(-squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, -squareHalfSize)
	};

	uv = {
		Vector2(0.125f, 0.75f),
		Vector2(0.125f, 0.875f),
		Vector2(0.25f, 0.875f),
		Vector2(0.25f, 0.75f)
	};

	i = {
		0, 2, 1, 0, 3, 2
	};

	quadMesh.CalculateBounds();

	// 텍스처 로딩
	Texture& diffuseTexture = CreateTexture(GameEngine::DiffuseTexture, GameEngine::SteveTexturePath);
	if (!diffuseTexture.IsIntialized())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadScene()
{
	// 플레이어
	constexpr float playerScale = 30.f;
	constexpr float squareScale = 20.f;

	GameObject& goPlayer = CreateNewGameObject(GameEngine::PlayerGo);
	goPlayer.SetMesh(GameEngine::QuadMesh);
	goPlayer.GetTransform().SetScale(Vector2::One * playerScale);
	goPlayer.SetColor(LinearColor::Red);

	// 고정 시드로 랜덤하게 생성
	std::mt19937 generator(0);
	std::uniform_real_distribution<float> dist(-1000.f, 1000.f);

	// 100개의 배경 게임 오브젝트 생성
	char name[64];
	for (int i = 0; i < 100; ++i)
	{
		std::snprintf(name, sizeof(name), "GameObject%d", i);
		GameObject& newGo = CreateNewGameObject(name);
		newGo.GetTransform().SetPosition(Vector2(dist(generator), dist(generator)));
		newGo.GetTransform().SetScale(Vector2::One * squareScale);
		newGo.SetMesh(GameEngine::QuadMesh);
		newGo.SetColor(LinearColor::Blue);
	}

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
