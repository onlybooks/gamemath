#pragma once

namespace WindowsUtil
{
	void GetWindowSize(HWND handle, float* width, float* height)
	{
		RECT rect;
		::GetClientRect(handle, &rect);
		*width = static_cast<float>(rect.right - rect.left);
		*height = static_cast<float>(rect.bottom - rect.top);
	}

	void Show(HWND handle)
	{
		::ShowWindow(handle, SW_SHOW);
		::SetForegroundWindow(handle);
		::SetFocus(handle);
	}

	void CenterWindow(HWND handle)
	{
		// center on parent or screen
		RECT rc, rcOwner, rcWindow;
		HWND ownerHandle = GetDesktopWindow();
		GetWindowRect(ownerHandle, &rcOwner);
		GetWindowRect(handle, &rcWindow);
		CopyRect(&rc, &rcOwner);

		OffsetRect(&rcWindow, -rcWindow.left, -rcWindow.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcWindow.right, -rcWindow.bottom);

		::SetWindowPos(handle, HWND_TOP,
			rcOwner.left + (rc.right / 2),
			rcOwner.top + (rc.bottom / 2),
			0, 0, 
			SWP_NOSIZE);
	}

	float GetCyclesPerMilliSeconds()
	{
		LARGE_INTEGER frequency;
		if (!QueryPerformanceFrequency(&frequency))
		{
			return 0.f;
		}

		return (float)(frequency.QuadPart / 1000.f);
	}

	long long GetCurrentTimeStamp()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		return currentTime.QuadPart;
	}

	float GetXAxisInput()
	{
		bool isLeft = GetAsyncKeyState(VK_LEFT);
		bool isRight = GetAsyncKeyState(VK_RIGHT);
		if (isLeft ^ isRight)
		{
			return isLeft ? -1.f : 1.f;
		}
		return 0.f;
	}

	float GetYAxisInput()
	{
		bool isDown = GetAsyncKeyState(VK_DOWN);
		bool isUp = GetAsyncKeyState(VK_UP);
		if (isDown ^ isUp)
		{
			return isDown ? -1.f : 1.f;
		}
		return 0.f;
	}

	float GetZAxisInput()
	{
		bool isDown = GetAsyncKeyState(VK_NEXT);
		bool isUp = GetAsyncKeyState(VK_PRIOR);
		if (isDown ^ isUp)
		{
			return isDown ? -1.f : 1.f;
		}
		return 0.f;
	}

	float GetWAxisInput()
	{
		bool isDown = GetAsyncKeyState(VK_END);
		bool isUp = GetAsyncKeyState(VK_HOME);
		if (isDown ^ isUp)
		{
			return isDown ? -1.f : 1.f;
		}
		return 0.f;
	}

	#define ISPRESSED(KeyCode) return (::GetKeyState(KeyCode) & 0x8000) != 0

	void BindInput(InputManager& InInputManager)
	{
		InInputManager.SetInputAxis(InputAxis::XAxis, GetXAxisInput);
		InInputManager.SetInputAxis(InputAxis::YAxis, GetYAxisInput);
		InInputManager.SetInputAxis(InputAxis::ZAxis, GetZAxisInput);
		InInputManager.SetInputAxis(InputAxis::WAxis, GetWAxisInput);
		InInputManager.SetInputButton(InputButton::Space, []() { ISPRESSED(VK_SPACE); });
		InInputManager.SetInputButton(InputButton::Z, []() { ISPRESSED(0x5A); });
		InInputManager.SetInputButton(InputButton::X, []() { ISPRESSED(0x58); });
	}

	void BindSystemInput(SystemInputManager& InSystemInputManager)
	{
		InSystemInputManager.SetSystemInputButton(SystemInputButton::F1, []() { ISPRESSED(VK_F1); });
		InSystemInputManager.SetSystemInputButton(SystemInputButton::F2, []() { ISPRESSED(VK_F2); });
		InSystemInputManager.SetSystemInputButton(SystemInputButton::F3, []() { ISPRESSED(VK_F3); });
		InSystemInputManager.SetSystemInputButton(SystemInputButton::F4, []() { ISPRESSED(VK_F4); });
		InSystemInputManager.SetSystemInputButton(SystemInputButton::F5, []() { ISPRESSED(VK_F5); });
		InSystemInputManager.SetSystemInputButton(SystemInputButton::F6, []() { ISPRESSED(VK_F6); });
		InSystemInputManager.SetSystemInputButton(SystemInputButton::F7, []() { ISPRESSED(VK_F7); });
		InSystemInputManager.SetSystemInputButton(SystemInputButton::F8, []() { ISPRESSED(VK_F8); });
		InSystemInputManager.SetSystemInputButton(SystemInputButton::F9, []() { ISPRESSED(VK_F9); });
		InSystemInputManager.SetSystemInputButton(SystemInputButton::F10, []() { ISPRESSED(VK_F10); });
	}
}