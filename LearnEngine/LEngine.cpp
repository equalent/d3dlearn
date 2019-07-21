// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#include "LEngine.h"

#include <string>
#include <sstream>

std::shared_ptr<LEngine> EnginePtr;
bool EnginePtrInitialized = false;

LE_API int RunEngine(HINSTANCE hInstance)
{	
#if defined(_DEBUG)
	wchar_t* lpFilename = new wchar_t[2048];
	GetModuleFileNameW(hInstance, lpFilename, 2048);
	std::wostringstream woss;
	woss << L"Initializing LearnEngine...\nExecutable filename: " << lpFilename << L"\n";
	OutputDebugStringW(woss.str().c_str());
	delete [] lpFilename;
#endif
	LEngine::Instance()->Run(hInstance);
	return 0;
}


LEngine::LEngine() {}

void LEngine::Run(HINSTANCE nhInstance) {
	if (!bRunning) {
		this->hInstance = nhInstance;
		bRunning = true;
	}
	else {
		throw LEngineAlreadyRunning();
	}
}

std::shared_ptr<LEngine> LEngine::Instance() {
	if (!EnginePtrInitialized) {
		LEngine* newEngine = new LEngine();
		EnginePtr = std::shared_ptr<LEngine>(newEngine);
		EnginePtrInitialized = true;
	}
	return EnginePtr;
}