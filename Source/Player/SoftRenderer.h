#pragma once

enum class GameEngineType : UINT32
{
	DD = 0,
	DDD
};

enum class DrawMode : UINT32
{
	Normal = 0,
	Wireframe,
	DepthBuffer
};

enum class FillMode : UINT32
{
	None = 0x00,
	Color = 0x01,
	Texture = 0x02
};

FORCEINLINE FillMode operator|(FillMode InLhs, FillMode InRhs)
{
	return static_cast<FillMode> (
		static_cast<std::underlying_type<FillMode>::type>(InLhs) |
		static_cast<std::underlying_type<FillMode>::type>(InRhs)
	);
}

FORCEINLINE FillMode& operator|=(FillMode& InLhs, FillMode InRhs)
{
	InLhs = InLhs | InRhs;
	return InLhs;
}

FORCEINLINE bool operator&(FillMode InLhs, FillMode InRhs)
{
	BYTE rhsValue = static_cast<std::underlying_type<FillMode>::type>(InRhs);
	return ((static_cast<std::underlying_type<FillMode>::type>(InLhs) & rhsValue) == rhsValue);
}

class SoftRenderer
{
public:
	// 생성자
	SoftRenderer(GameEngineType InGameEngineType, RendererInterface* InRSI);

	// 윈도우 이벤트 처리
	void OnTick();
	void OnResize(const ScreenPoint& InNewScreenSize);
	void OnShutdown();

	// 프로그램 기본 정보
	SystemInputManager& GetSystemInput() { return _SystemInputManager; }
	const ScreenPoint& GetScreenSize() { return _ScreenSize; }
	float GetFrameFPS() const { return _FrameFPS; }
	FORCEINLINE float GetElapsedTime() const { return _ElapsedTime; }

	// 성능 측정
	std::function<float()> _PerformanceInitFunc;
	std::function<INT64()> _PerformanceMeasureFunc;
	std::function<void(InputManager&)> _InputBindingFunc;

	// 게임 엔진 레퍼런스 
	FORCEINLINE EngineInterface& GetGameEngine() { return (_GameEngineType == GameEngineType::DD) ? static_cast<EngineInterface&>(_GameEngine2) : static_cast<EngineInterface&>(_GameEngine3); }
	FORCEINLINE DD::GameEngine& Get2DGameEngine() { return _GameEngine2; }
	FORCEINLINE DDD::GameEngine& Get3DGameEngine() { return _GameEngine3; }

private:
	// 게임 엔진 설정
	void SetDefaultGameEngine(GameEngineType InGameEngineType);

	// 기본 루프 함수
	void PreUpdate();
	void PostUpdate();

	// 렌더러 레퍼런스
	FORCEINLINE RendererInterface& GetRenderer() { return *_RSIPtr.get(); }
	FORCEINLINE void SetBackgroundColor(const LinearColor& InLinearColor) { _BackgroundColor = InLinearColor; }
	FORCEINLINE void SetWireframeColor(const LinearColor& InLinearColor) { _WireframeColor = InLinearColor; }

	// 2D 그래픽스 구현
	void LoadScene2D();
	void Update2D(float InDeltaSeconds);
	void Render2D();
	void DrawGizmo2D();
	void DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor);
	void DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode);

	int _Grid2DUnit = 10;

	// 3D 그래픽스 구현
	void LoadScene3D();
	void Update3D(float InDeltaSeconds);
	void LateUpdate3D(float InDeltaSeconds);
	void Render3D();
	void DrawGizmo3D();
	void DrawMesh3D(const class DDD::Mesh& InMesh, const Matrix4x4& InMatrix, const LinearColor& InColor);
	void DrawTriangle3D(std::vector<DDD::Vertex3D>& InVertices, const LinearColor& InColor, FillMode InFillMode);

	bool IsDepthBufferDrawing() const { return _CurrentDrawMode == DrawMode::DepthBuffer; }
	bool IsWireframeDrawing() const { return _CurrentDrawMode == DrawMode::Wireframe; }
	DrawMode GetDrawMode() const { return _CurrentDrawMode; }
	void SetDrawMode(DrawMode InDrawMode) { _CurrentDrawMode = InDrawMode; }

	float _GizmoUnitLength = 50.f;
	Vector2 _GizmoPositionOffset = Vector2(-320.f, -250.f);
	DrawMode _CurrentDrawMode = DrawMode::Normal;

private:
	// 초기화 점검 변수
	bool _PerformanceCheckInitialized = false;
	bool _RendererInitialized = false;
	bool _GameEngineInitialized = false;
	bool _TickEnabled = false;
	bool _AllInitialized = false;

	// 화면 크기
	ScreenPoint _ScreenSize;

	// 배경 색상
	LinearColor _BackgroundColor = LinearColor::DimGray;

	// 와이어프레임 색상
	LinearColor _WireframeColor = LinearColor::DimGray;

	// 성능 측정
	long long _StartTimeStamp = 0;
	long long _FrameTimeStamp = 0;
	long _FrameCount = 0;
	float _CyclesPerMilliSeconds = 0.f;
	float _FrameTime = 0.f;
	float _ElapsedTime = 0.f;
	float _AverageFPS = 0.f;
	float _FrameFPS = 0.f;

	// 렌더러 인터페이스
	std::unique_ptr<RendererInterface> _RSIPtr;
	GameEngineType _GameEngineType = GameEngineType::DD;

	// 게임 엔진
	DD::GameEngine _GameEngine2;
	DDD::GameEngine _GameEngine3;

	// 응용 프로그램 입력
	SystemInputManager _SystemInputManager;
};
