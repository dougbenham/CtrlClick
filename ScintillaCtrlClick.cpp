#include "windows.h"

HHOOK hook;
MSLLHOOKSTRUCT msStruct;
char windowClass[256];

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		if (wParam == WM_LBUTTONDOWN ||
			wParam == WM_LBUTTONUP)
		{
			const auto ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
			if (ctrl)
			{
				msStruct = *reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
				const auto hWnd = WindowFromPoint(msStruct.pt);
				if (hWnd)
				{
					if (GetClassNameA(hWnd, windowClass, 255) && strcmp(windowClass, "Scintilla") == 0)
					{
						if (wParam == WM_LBUTTONDOWN)
						{
							return 1;
						}
						else if (wParam == WM_LBUTTONUP)
						{
							POINT client;
							client.x = msStruct.pt.x;
							client.y = msStruct.pt.y;
							ScreenToClient(hWnd, &client);

							const auto mouseLParam = MAKELPARAM(client.x, client.y);

							SendMessage(hWnd, WM_LBUTTONDOWN, 0, mouseLParam);
							SendMessage(hWnd, WM_LBUTTONUP, 0, mouseLParam);
							SendMessage(hWnd, WM_LBUTTONDOWN, 0, mouseLParam);
							SendMessage(hWnd, WM_LBUTTONUP, 0, mouseLParam);
							return 1;
						}
					}
				}
			}
		}
	}

	return CallNextHookEx(hook, nCode, wParam, lParam);
}

void SetHook()
{
	if (!(hook = SetWindowsHookEx(WH_MOUSE_LL, HookCallback, GetModuleHandle(NULL), 0)))
	{
		MessageBoxA(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
	}
}

void ReleaseHook()
{
	UnhookWindowsHookEx(hook);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	SetHook();

	MSG message;
	while (GetMessage(&message, NULL, 0, 0) != 0) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	ReleaseHook();
	return 0;
}