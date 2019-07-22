// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#include "LEngine.h"

#include <string>
#include <sstream>
#include <ctime>
#include <exception>

#define HANDLE_EXCEPTIONS 1

std::shared_ptr<LEngine> EnginePtr;
bool EnginePtrInitialized = false;

namespace fs = std::experimental::filesystem;

LE_API int RunEngine(HINSTANCE hInstance)
{
#if defined(_DEBUG)
	wchar_t* lpFilename = new wchar_t[2048];
	GetModuleFileNameW(hInstance, lpFilename, 2048);
	std::wostringstream woss;
	woss << L"Initializing LearnEngine...\nExecutable filename: " << lpFilename << L"\n";
	OutputDebugStringW(woss.str().c_str());
	delete[] lpFilename;
#endif
#if HANDLE_EXCEPTIONS
	try {
#endif
		LEngine::Instance()->Run(hInstance);
#if HANDLE_EXCEPTIONS
	}
	catch (LException e) {
		MessageBoxW(LEngine::Instance()->GetMainWindowHandle(), e.Format().c_str(), L"UNHANDLED EXCEPTION in LearnEngine", MB_OK | MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
		return -1;
	}
	catch (std::exception e) {
		std::wostringstream woss;
		woss << L"STD::EXCEPTION: ";
		woss << convert_utf8_to_utf16(std::string(e.what()));
		MessageBoxW(LEngine::Instance()->GetMainWindowHandle(), woss.str().c_str(), L"UNHANDLED EXCEPTION in LearnEngine", MB_OK | MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
		return -1;
	}
#endif

	return 0;
}


LEngine::LEngine() {}

void LEngine::Run(HINSTANCE nhInstance) {
	if (!bRunning) {
		this->hInstance = nhInstance;
		bRunning = true;

		// INITIALIZE FS
		wchar_t* lpFilename = new wchar_t[2048];
		GetModuleFileNameW(hInstance, lpFilename, 2048);
		RootPath = fs::path(std::wstring(lpFilename)).parent_path();

		// LOAD CONFIG
		pConfig = std::make_shared<LConfig>(RootPath / fs::path(L"learn.ini"));

		pMainWindow = std::make_shared<LWindow>();
		pMainWindow->BindOnResize([this](int nWidth, int nHeight) {
			// TODO resize swapchain buffers
			});

		pMainWindow->BindOnClose([this]() {
			this->Shutdown();
			return true;
			});

		pMainWindow->SetOption(LWindowOptionsEnableFullscreenOnF11, true);

#pragma warning (push)
#pragma warning (disable: 4244)
		clock_t oldTime = clock();
		clock_t newTime = clock();

		while (bRunning) {
			oldTime = newTime;
			newTime = clock();
			MainLoop((float)(newTime-oldTime)/CLOCKS_PER_SEC);
		}
#pragma warning (pop)
	}
	else {
		throw LEngineAlreadyRunningException(__LINE__, __FILEW__);
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

void LEngine::Shutdown() {
	bRunning = false;
}

std::shared_ptr<LConfig> LEngine::GetConfig() {
	return pConfig;
}

HINSTANCE LEngine::GetInstanceHandle() {
	return hInstance;
}

HWND LEngine::GetMainWindowHandle() {
	if (pMainWindow) {
		return pMainWindow->GetHandle();
	}
	else return NULL;
}

void LEngine::MainLoop(float DeltaTime) {
	pMainWindow->CheckMessages();
}