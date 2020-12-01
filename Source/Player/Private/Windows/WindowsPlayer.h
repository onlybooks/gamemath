#pragma once

namespace WindowsPlayer
{
	static HINSTANCE gInstance;
	static HWND gHandle;
	static std::function<void(ScreenPoint& InNewScreenSize)> gOnResizeFunc;

	static const TCHAR *gClassName = _T("SOFTRENDERER_PLAYER");
	static TCHAR gTitle[64];
	static TCHAR gPlayTitle[128];

	LRESULT CALLBACK WndProc(HWND hwnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DISPLAYCHANGE:
		case WM_SIZE:
		{
			int newWidth = (lParam & 0xffff);
			int newHeight = ((lParam >> 16) & 0xffff);
			if (gOnResizeFunc)
			{
				gOnResizeFunc(ScreenPoint(newWidth, newHeight));
			}
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_SYSCOMMAND:
		{
			if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER || wParam == SC_KEYMENU)
			{
				return 0;
			}
			break;
		}
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	bool Create(HINSTANCE InInstance, const ScreenPoint& InDefaultScreenSize)
	{
		gInstance = InInstance;

		#define SIZEOF(a) ((int)(sizeof(a) / sizeof((a)[0])))
		::LoadString(gInstance, IDS_APP_TITLE, gTitle, SIZEOF(gTitle));

		WNDCLASSEX wcex;
		memset(&wcex, 0, sizeof(wcex));
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = gInstance;
		wcex.hIcon = LoadIcon(gInstance, MAKEINTRESOURCE(IDI_SOFTRENDERER));
		wcex.hIconSm = LoadIcon(gInstance, MAKEINTRESOURCE(IDI_SMALL));
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = gClassName;

		if (!::RegisterClassEx(&wcex))
		{
			::MessageBox(nullptr, "Window registration failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = InDefaultScreenSize.X - 1;
		rect.bottom = InDefaultScreenSize.Y - 1;
		::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		int WindowWidth = rect.right - rect.left + 1;
		int WindowHeight = rect.bottom - rect.top + 1;
			
		gHandle = ::CreateWindow(gClassName, gTitle, WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
			(GetSystemMetrics(SM_CXFULLSCREEN) - WindowWidth) / 2, (GetSystemMetrics(SM_CYFULLSCREEN) - WindowHeight) / 2, WindowWidth, WindowHeight,
			NULL, NULL, gInstance, NULL);

		if (!gHandle)
		{
			::MessageBox(nullptr, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		return true;
	}

	FORCEINLINE bool Tick()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				return false;
		}

		return true;
	}

	void Destroy()
	{
		DestroyWindow(gHandle);
	}

	FORCEINLINE void SetWindowsStatTitle(float frameFPS)
	{
		_stprintf_s(gPlayTitle, "%s  [%.2f fps]", gTitle, frameFPS);
		::SetWindowText(gHandle, gPlayTitle);
	}
}