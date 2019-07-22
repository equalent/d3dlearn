// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once
#include <Windows.h>
#include <memory>
#include <string>
#include <functional>

using std::placeholders::_1;

DEFINE_EXCEPTION_WITH_MESSAGE(LWindowUnableToRegisterClass, L"Unable to register window class (RegisterClassExW returned 0)!");
DEFINE_EXCEPTION_WITH_MESSAGE(LWindowUnableToCreate, L"Unable to create window (CreateWindowExW returned 0)!");
DEFINE_EXCEPTION_WITH_MESSAGE(LWindowUnableToResize, L"Unable to resize window (SetWindowPos returned nonzero)!");
DEFINE_EXCEPTION_WITH_MESSAGE(LWindowUnableToEnterFullscreen, L"Unable to enter fullscreen (ChangeDisplaySettings didn't return DISP_CHANGE_SUCCESFUL)!");

class LWindow {
public:
	LWindow();
	std::wstring GetTitle();
	void SetTitle(std::wstring nTitle);

	bool GetVisible();
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

	// EVENT BINDERS:
	void BindOnClose(std::function<bool()>);
	void BindOnResize(std::function<void(int, int)>);
private:
	std::wstring Title;
	void Resize();
	int Width, Height, X, Y;
	int w_Width, w_Height, w_X, w_Y;
	bool Visible;
	HWND hWnd;

	void Destroy();

	// EVENTS:
	// Will close window if true is returned
	std::function<bool()> OnClose = nullptr;
	// Width, Height
	std::function<void(int, int)> OnResize = nullptr;
	// X, Y
	std::function<void(int, int)> OnMove = nullptr;
};