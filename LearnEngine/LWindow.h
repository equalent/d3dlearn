// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once
#include <Windows.h>
#include <memory>
#include <string>

DEFINE_EXCEPTION_WITH_MESSAGE(LWindowUnableToRegisterClass, L"Unable to register window class (RegisterClassExW returned 0)!");
DEFINE_EXCEPTION_WITH_MESSAGE(LWindowUnableToCreate, L"Unable to create window (CreateWindowExW returned 0)!");
DEFINE_EXCEPTION_WITH_MESSAGE(LWindowUnableToResize, L"Unable to resize window (SetWindowPos returned nonzero)!");
DEFINE_EXCEPTION_WITH_MESSAGE(LWindowUnableToEnterFullscreen, L"Unable to enter fullscreen (ChangeDisplaySettings didn't return DISP_CHANGE_SUCCESFUL)!");

class LWindow {
public:
	LWindow();
	void SetTitle(std::wstring nTitle);
	void SetVisible(bool nVisible);
	void SetFullscreen(bool nFullscreen);

	// size/position getters/setters
	int GetWidth();
	int GetHeight();
	void SetWidth(int nWidth);
	void SetHeight(int nHeight);
	void CenterOnScreen();

	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	int GetX();
	int GetY();
	void SetX(int nX);
	void SetY(int nY);

	static void CheckMessages();
private:
	void Resize();
	int Width, Height, X, Y;
	int w_Width, w_Height, w_X, w_Y;
	HWND hWnd;
};