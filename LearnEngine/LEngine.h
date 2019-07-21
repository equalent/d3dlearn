// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once
#include <Windows.h>
#include <memory>
#include "LearnEngineAPI.h"
#include <exception>

class LEngineAlreadyRunning {
	const char* what() const throw ()
	{
		return "LearnEngine is already running!";
	}
};

class LEngine {
public:
	static std::shared_ptr<LEngine> Instance();
	void Run(HINSTANCE);
private:
	bool bRunning = false;
	HINSTANCE hInstance = NULL;
	LEngine();
};

extern "C" LE_API int RunEngine(HINSTANCE);
