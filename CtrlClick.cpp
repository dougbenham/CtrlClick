#include "windows.h"

HHOOK hook;
MSLLHOOKSTRUCT msStruct;
char windowClass[256];
char windowText[256];

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && 
		(wParam == WM_LBUTTONDOWN ||
		 wParam == WM_LBUTTONUP))
	{
		const auto ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
		if (ctrl)
		{
			msStruct = *reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
			const auto hWnd = WindowFromPoint(msStruct.pt);
			if (hWnd && 
				GetClassNameA(hWnd, windowClass, 255) &&
				GetWindowTextA(hWnd, windowText, 255) &&
				(strcmp(windowClass, "Scintilla") == 0 || // Notepad++, SciTE, etc.
					strcmp(windowClass, "Chrome_RenderWidgetHostHWND") == 0 && strcmp(windowText, "Chrome Legacy Window") == 0)) // VS Code, maybe others?
			{
				if (wParam == WM_LBUTTONDOWN)
				{
					return 1; // don't allow mouse down with CTRL held
				}
				else if (wParam == WM_LBUTTONUP)
				{
					// release CTRL
					INPUT input;
					input.type = INPUT_KEYBOARD;
					input.ki.wVk = VK_CONTROL;
					input.ki.dwFlags = KEYEVENTF_KEYUP;
					SendInput(1, &input, sizeof(input));

					// double click in place
					POINT client;
					client.x = msStruct.pt.x;
					client.y = msStruct.pt.y;
					ScreenToClient(hWnd, &client);
					const auto mouseLParam = MAKELPARAM(client.x, client.y);							
					SendMessage(hWnd, WM_LBUTTONDOWN, 0, mouseLParam);
					SendMessage(hWnd, WM_LBUTTONUP, 0, mouseLParam);
					SendMessage(hWnd, WM_LBUTTONDOWN, 0, mouseLParam);
					SendMessage(hWnd, WM_LBUTTONUP, 0, mouseLParam);

					// press CTRL
					input.ki.dwFlags = 0;
					SendInput(1, &input, sizeof(input));
					return 1; // don't allow this current mouse up to be processed
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
		MessageBoxA(NULL, "Failed to install Ctrl+Click hook!", "Error", MB_ICONERROR);
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