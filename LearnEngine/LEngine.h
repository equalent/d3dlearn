// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once
#include <Windows.h>
#include <memory>
#include "LearnEngineAPI.h"
#include "LException.h"
#include "LWindow.h"

DEFINE_EXCEPTION_WITH_MESSAGE(LEngineAlreadyRunningException, L"LearnEngine is already running!");

class LEngine {
public:
	static std::shared_ptr<LEngine> Instance();
	void Run(HINSTANCE);
	void Shutdown();
	HINSTANCE GetInstanceHandle();
	HWND GetMainWindowHandle();
private:
	void MainLoop(float DeltaTime);
	bool bRunning = false;
	HINSTANCE hInstance = NULL;
	std::shared_ptr<LWindow> pMainWindow = nullptr;
	LEngine();
};

extern "C" LE_API int RunEngine(HINSTANCE);
