// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#define LWINDOW_CLASS_NAME L"learnengine_wnds"
#define LWINDOW_DEFAULT_NAME L"LE Window"
#define LWINDOW_DEFAULT_WIDTH 1280
#define LWINDOW_DEFAULT_HEIGHT 720

#include "LEngine.h"
#include "LWindow.h"
#include "Utils.h"
#include <map>

bool WindowClassRegistered = false;
std::map<HWND, LWindow*> _WinMap;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//TODO delegate event processing to window
	try {
		return _WinMap.at(hWnd)->WndProc(message, wParam, lParam);
	}
	catch (std::out_of_range) {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	/*switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}*/

	return 0;
}

void LRegisterClass() {
	WNDCLASSEXW wc;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = &WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = (HICON)LoadImageW(LEngine::Instance()->GetInstanceHandle(), MAKEINTRESOURCEW(101), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = LWINDOW_CLASS_NAME;
	wc.hIconSm = wc.hIcon;
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

	ATOM result = RegisterClassExW(&wc);
	if (result == 0) {
		throw LWindowUnableToRegisterClassException(__LINE__, __FILEW__);
	}
}

LWindow::LWindow() {
	if (!WindowClassRegistered) {
		LRegisterClass();
		WindowClassRegistered = true;
	}
	HWND hwnd = CreateWindowExW(NULL, LWINDOW_CLASS_NAME, LWINDOW_DEFAULT_NAME, WS_OVERLAPPEDWINDOW, 0, 0, LWINDOW_DEFAULT_WIDTH, LWINDOW_DEFAULT_WIDTH, NULL, NULL, GetModuleHandle(NULL), NULL);
	if (hwnd == NULL) {
		throw LWindowUnableToCreateException(__LINE__, __FILEW__);
	}
	hWnd = hwnd;

	_WinMap.insert(std::pair<HWND, LWindow*>(hWnd, this));

	SetVisible(true);
	Width = LWINDOW_DEFAULT_WIDTH;
	Height = LWINDOW_DEFAULT_HEIGHT;
	X = Y = 0;
	CenterOnScreen();
	SetTitle(LWINDOW_DEFAULT_NAME);
}

std::wstring LWindow::GetTitle() {
	return Title;
}

void LWindow::SetTitle(std::wstring nTitle) {
	SetWindowTextW(hWnd, nTitle.c_str());
	Title = nTitle;
}

void LWindow::SetOption(LWindowOptions nOption, bool nValue) {
	switch (nOption) {
		case LWindowOptionsEnableFullscreenOnF11: {
			oEnableFullscreenOnF11 = nValue;
			break;
		}
	}
}

void LWindow::SetVisible(bool nVisible) {
	ShowWindowAsync(hWnd, nVisible ? SW_SHOW : SW_HIDE);
	Visible = nVisible;
}

bool LWindow::GetVisible() {
	return Visible;
}

void LWindow::SetFullscreen(bool nFullscreen) {
	if (nFullscreen && !Fullscreen) {
		ShowWindow(hWnd, SW_RESTORE);
		SetWindowRgn(hWnd, 0, false);

		DEVMODEW newSettings;
		newSettings.dmSize = sizeof(DEVMODEW);

		EnumDisplaySettingsW(0, 0, &newSettings);

		newSettings.dmPelsWidth = GetScreenWidth();
		newSettings.dmPelsHeight = GetScreenHeight();
		newSettings.dmBitsPerPel = 32;

		newSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// attempt to apply the new settings 
		long result = ChangeDisplaySettingsW(NULL, CDS_FULLSCREEN);

		if (result != DISP_CHANGE_SUCCESSFUL) {
			throw LWindowUnableToEnterFullscreenException(__LINE__, __FILEW__);
		}
		else {
			w_Width = Width;
			w_Height = Height;
			w_X = X;
			w_Y = Y;

			DWORD dwstyle = GetWindowLongW(hWnd, GWL_STYLE);
			dwstyle &= ~WS_CAPTION;
			dwstyle |= WS_POPUP;
			dwstyle &= ~WS_OVERLAPPEDWINDOW;
			SetWindowLongW(hWnd, GWL_STYLE, dwstyle);
			ShowWindow(hWnd, SW_SHOWMAXIMIZED);

			SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
		Fullscreen = true;
	}
	else {
		ChangeDisplaySettings(0, 0);
		Width = w_Width;
		Height = w_Height;
		X = w_X;
		Y = w_Y;
		Resize();

		DWORD dwstyle = GetWindowLongW(hWnd, GWL_STYLE);
		dwstyle |= WS_CAPTION;
		dwstyle &= ~WS_POPUP;
		dwstyle |= WS_OVERLAPPEDWINDOW;
		SetWindowLongW(hWnd, GWL_STYLE, dwstyle);

		ShowWindow(hWnd, SW_RESTORE);
		Fullscreen = false;
	}
}

void LWindow::ToggleFullscreen() {
	SetFullscreen(!Fullscreen);
}

int LWindow::GetWidth() {
	return Width;
}

int LWindow::GetHeight() {
	return Height;
}

void LWindow::SetWidth(int nWidth) {
	Width = nWidth;
	Resize();
}

void LWindow::SetHeight(int nHeight) {
	Height = nHeight;
	Resize();
}

int LWindow::GetX() {
	return X;
}

int LWindow::GetY() {
	return Y;
}

void LWindow::SetX(int nX) {
	X = nX;
	Resize();
}

void LWindow::SetY(int nY) {
	Y = nY;
	Resize();
}

void LWindow::Resize() {
	if (SetWindowPos(hWnd, HWND_NOTOPMOST, X, Y, Width, Height, NULL) == 0) {
		throw LWindowUnableToResizeException(__LINE__, __FILEW__);
	}
}

void LWindow::Destroy()
{
	_WinMap.erase(hWnd);
}

void LWindow::CenterOnScreen() {
	X = GetScreenWidth() / 2 - Width / 2;
	Y = GetScreenHeight() / 2 - Height / 2;
	Resize();
}

LRESULT LWindow::WndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_SIZE: {
		Width = LOWORD(lParam);
		Height = HIWORD(lParam);
		if (this->OnResize != nullptr) {
			OnResize(Width, Height);
		}
		break;
	}
	case WM_MOVE: {
		X = LOWORD(lParam);
		Y = HIWORD(lParam);
		if (this->OnMove != nullptr) {
			OnMove(X, Y);
		}
		break;
	}
	case WM_CLOSE: {
		if (this->OnClose != nullptr) {
			if (!OnClose()) {
				return true;
			}
		}
		Destroy();
		break;
	}
	case WM_KEYUP: {
		if (oEnableFullscreenOnF11 && wParam == VK_F11) {
			ToggleFullscreen();
			return 0;
		}
		break;
	}
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

void LWindow::CheckMessages() {
	MSG message;
	while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

void LWindow::BindOnClose(std::function<bool()> nOnClose) {
	OnClose = nOnClose;
}

void LWindow::BindOnResize(std::function<void(int, int)> nOnResize) {
	OnResize = nOnResize;
}