
#include "Precompiled.h"
#include "SoftRenderer.h"

SoftRenderer::SoftRenderer(GameEngineType InGameEngineType, RendererInterface* InRSI) : _GameEngineType(InGameEngineType), _RSIPtr(InRSI)
{
}

void SoftRenderer::OnTick()
{
	if (!_AllInitialized)
	{
		// 퍼포먼스 카운터 초기화.
		if (!_PerformanceCheckInitialized)
		{
			if (_PerformanceInitFunc && _PerformanceMeasureFunc)
			{
				_CyclesPerMilliSeconds = _PerformanceInitFunc();
				_PerformanceCheckInitialized = true;
			}
			else
			{
				assert(false);
				return;
			}
		}

		// 스크린 크기 확인
		if (_ScreenSize.HasZero())
		{
			return;
		}

		// 소프트 렌더러 초기화.
		if (!_RendererInitialized)
		{
			_RendererInitialized = GetRenderer().Init(_ScreenSize);
			if (!_RendererInitialized)
			{
				assert(false);
				return;
			}
		}

		// 게임 엔진 초기화
		_GameEngineInitialized = GetGameEngine().IsInitialized();
		if (!_GameEngineInitialized)
		{
			GetGameEngine().OnScreenResize(_ScreenSize);
			InputManager& input = GetGameEngine().GetInputManager();
			if (!input.IsInputReady())
			{
				_InputBindingFunc(input);
			}
			
			_GameEngineInitialized = GetGameEngine().Init();
			if (!_GameEngineInitialized)
			{
				assert(false);
				return;
			}

			if (_GameEngineType == GameEngineType::DD)
			{
				LoadScene2D();
			}
			else
			{
				LoadScene3D();
			}
		}

		_AllInitialized = _RendererInitialized && _PerformanceCheckInitialized && _GameEngineInitialized;
		if (_AllInitialized)
		{
			_TickEnabled = true;
		}
	}
	else
	{
		assert(_RSIPtr != nullptr && GetRenderer().IsInitialized() && !_ScreenSize.HasZero());

		if (_TickEnabled)
		{
			PreUpdate();

			// 게임 엔진 교체로 함수 리셋 진행
			if (!_AllInitialized)
			{
				GetSystemInput().UpdateSystemInput();
				return;
			}

			if (_GameEngineType == GameEngineType::DD)
			{
				Update2D(_FrameTime / 1000.f);
				Render2D();
			}
			else
			{
				Update3D(_FrameTime / 1000.f);
				LateUpdate3D(_FrameTime / 1000.f);
				Render3D();
			}

			PostUpdate();
		}
	}
}

void SoftRenderer::OnResize(const ScreenPoint& InNewScreenSize)
{
	_ScreenSize = InNewScreenSize;

	// 크기가 변경되면 렌더러와 엔진 초기화
	if (_RendererInitialized)
	{
		GetRenderer().Init(InNewScreenSize);
	}

	if (_GameEngineInitialized)
	{
		GetGameEngine().OnScreenResize(_ScreenSize);
	}	
}

void SoftRenderer::OnShutdown()
{
	GetRenderer().Shutdown();
}

void SoftRenderer::SetDefaultGameEngine(GameEngineType InGameEngineType)
{
	_GameEngineType = InGameEngineType;
}

void SoftRenderer::PreUpdate()
{
	// 성능 측정 시작.
	_FrameTimeStamp = _PerformanceMeasureFunc();
	if (_FrameCount == 0)
	{
		_StartTimeStamp = _FrameTimeStamp;
	}

	// 배경 지우기.
	GetRenderer().Clear(_BackgroundColor);

	// 버퍼 시각화
	const SystemInputManager& sinput = GetSystemInput();

	if (sinput.IsReleased(SystemInputButton::F1)) { _CurrentDrawMode = DrawMode::Normal; }
	if (sinput.IsReleased(SystemInputButton::F2)) { _CurrentDrawMode = DrawMode::Wireframe; }
	if (sinput.IsReleased(SystemInputButton::F3)) { _CurrentDrawMode = DrawMode::DepthBuffer; }
	if (sinput.IsReleased(SystemInputButton::F10))
	{ 
		SetDefaultGameEngine((_GameEngineType == GameEngineType::DD) ? GameEngineType::DDD : GameEngineType::DD);
		_AllInitialized = false;
	}
}

void SoftRenderer::PostUpdate()
{
	// 렌더링 마무리.
	GetRenderer().EndFrame();

	// 입력 상태 업데이트
	GetGameEngine().GetInputManager().UpdateInput();
	GetSystemInput().UpdateSystemInput();

	// 성능 측정 마무리.
	_FrameCount++;
	INT64 currentTimeStamp = _PerformanceMeasureFunc();
	INT64 frameCycles = currentTimeStamp - _FrameTimeStamp;
	INT64 elapsedCycles = currentTimeStamp - _StartTimeStamp;
	_FrameTime = frameCycles / _CyclesPerMilliSeconds;
	_ElapsedTime = elapsedCycles / _CyclesPerMilliSeconds;
	_FrameFPS = _FrameTime == 0.f ? 0.f : 1000.f / _FrameTime;
	_AverageFPS = _ElapsedTime == 0.f ? 0.f : 1000.f / _ElapsedTime * _FrameCount;
}


