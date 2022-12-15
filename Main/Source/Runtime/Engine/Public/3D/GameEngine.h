#pragma once

namespace CK
{
namespace DDD
{

class GameEngine : public EngineInterface
{
public:
	GameEngine() = default;

public:
	// ���� �������̽�
	virtual bool Init() override;
	virtual bool IsInitialized() override { return _IsInitialized; }
	virtual void OnScreenResize(const ScreenPoint& InScreenSize) override;
	virtual InputManager& GetInputManager() override { return _InputManager; }

	// ���� ���� �뵵 
	const InputManager& GetInputManager() const { return _InputManager; }

	// ���ҽ� ����
	Mesh& CreateMesh(const std::size_t& InKey);
	Texture& CreateTexture(const std::size_t& InKey, const std::string& InTexturePath);

	// ���� ������Ʈ
	const std::vector<std::unique_ptr<GameObject>>& GetScene() const { return _Scene; }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneBegin() const { return _Scene.begin(); }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneEnd() const { return _Scene.end(); }
	GameObject& CreateNewGameObject(const std::string& InName);
	GameObject& GetGameObject(const std::string& InName);

	// �޽�
	Mesh& GetMesh(const std::size_t& InMeshKey) { return *_Meshes.at(InMeshKey).get(); }
	const Mesh& GetMesh(const std::size_t& InMeshKey) const { return *_Meshes.at(InMeshKey).get(); }

	// ī�޶� 
	FORCEINLINE CameraObject& GetMainCamera() { return _MainCamera; }
	FORCEINLINE const CameraObject& GetMainCamera() const { return _MainCamera; }

	// ���� �ؽ�ó
	FORCEINLINE const Texture& GetTexture(const std::size_t& InTextureKey) const { return *_Textures.at(InTextureKey).get(); }

	// ���� �׸��� ���� ���
	std::unordered_map<std::string, GameObject*> GetBoneObjectPtrs() { return _BoneGameObjectPtrs; }

private:
	bool LoadResources();

public: // �ֿ� Ű ��
	// ��
	static const std::string RootBone;
	static const std::string PelvisBone;
	static const std::string SpineBone;
	static const std::string LeftArmBone;
	static const std::string RightArmBone;
	static const std::string NeckBone;
	static const std::string LeftLegBone;
	static const std::string RightLegBone;

	// �޽�
	static const std::size_t CharacterMesh;
	static const std::size_t ArrowMesh;
	static const std::size_t PlaneMesh;

	// �ؽ�ó
	static const std::size_t BaseTexture;
	static const std::string CharacterTexturePath;

private:
	bool _IsInitialized = false;

	ScreenPoint _ScreenSize;
	InputManager _InputManager;
	CameraObject _MainCamera;

	std::vector<std::unique_ptr<GameObject>> _Scene;
	std::unordered_map<std::size_t, std::unique_ptr<Mesh>> _Meshes;
	std::unordered_map<std::size_t, std::unique_ptr<Texture>> _Textures;
	std::unordered_map<std::string, GameObject*> _BoneGameObjectPtrs;
};

}
}