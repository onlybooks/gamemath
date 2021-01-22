
#include "Precompiled.h"
#include "SoftRenderer.h"
#include "WindowsUtil.h"
#include "WindowsPlayer.h"

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	ScreenPoint defScreenSize(800, 600);
	SoftRenderer instance(GameEngineType::DD, new WindowsRSI());
	WindowsPlayer::gOnResizeFunc = [&instance](const ScreenPoint& InNewScreenSize) { 
		if (InNewScreenSize.HasZero()) {
			return;
		}
		instance.OnResize(InNewScreenSize); 
	};
	instance._PerformanceInitFunc = WindowsUtil::GetCyclesPerMilliSeconds;
	instance._PerformanceMeasureFunc = WindowsUtil::GetCurrentTimeStamp;
	instance._InputBindingFunc = WindowsUtil::BindInput;
	WindowsUtil::BindSystemInput(instance.GetSystemInput());

	if (!WindowsPlayer::Create(hInstance, defScreenSize))
	{
		return -1;
	}

	WindowsUtil::Show(WindowsPlayer::gHandle);
	WindowsUtil::CenterWindow(WindowsPlayer::gHandle);

	static float previousTimer = 0.f;
	static float updatePeriod = 500.f;
	while (WindowsPlayer::Tick())
	{
		instance.OnTick();

		float currentTime = instance.GetElapsedTime();
		if (currentTime - previousTimer > updatePeriod)
		{
			float frameFPS = instance.GetFrameFPS();
			WindowsPlayer::SetWindowsStatTitle(frameFPS);
			previousTimer = currentTime;
		}
	}

	instance.OnShutdown();
	WindowsPlayer::Destroy();
	return 0;
}
