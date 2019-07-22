// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once
#include "LEngine.h"
#include "LException.h"
#include <memory>
#include <string>

DEFINE_EXCEPTION_WITH_MESSAGE(LRenderDeviceAlreadyInitializedException, L"The rendering device has already been initialized!");
DEFINE_EXCEPTION_WITH_MESSAGE(LRenderDeviceNotInitializedException, L"The rendering device has not been yet initialized!");
DEFINE_EXCEPTION(LRenderDeviceUnableToInitializeException);

enum LRenderAPI {
	Direct3D11,
	Direct3D12
};

class LRenderDevice {
public:
	static std::shared_ptr<LRenderDevice> Instance();
	void Initialize(LRenderAPI nApi, int nWidth, int nHeight);
	void Shutdown();
	void ResizeBuffers(int nWidth, int nHeight);
	std::wstring GetAdapterName();
	std::uint64_t GetVideoMemory();
	std::uint64_t GetUsedVideoMemory();
private:
	LRenderDevice();
	std::wstring m_AdapterName = L"";
	std::uint64_t m_VideoMemory = 0L;
	bool bIsInitialized = false;
};