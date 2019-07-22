// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#include "LRenderDevice.h"

#include <dxgi.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include "Utils.h"

using namespace Microsoft::WRL;

std::shared_ptr<LRenderDevice> RenderDevicePtr = nullptr;

LRenderDevice::LRenderDevice() {}

HRESULT hr;

#pragma region DXGI_TYPES
ComPtr<IDXGIFactory3> pDxgiFactory = nullptr;
ComPtr<IDXGIAdapter1> pDxgiAdapterOrig = nullptr;
ComPtr<IDXGIAdapter3> pDxgiAdapter = nullptr;
ComPtr<IDXGIOutput> pDxgiOutputOrig = nullptr;
ComPtr<IDXGIOutput3> pDxgiOutput = nullptr;
#pragma endregion


std::shared_ptr<LRenderDevice> LRenderDevice::Instance() {
	if (!RenderDevicePtr) {
		LRenderDevice* newRenderDevice = new LRenderDevice();
		RenderDevicePtr = std::shared_ptr<LRenderDevice>(newRenderDevice);
	}
	return RenderDevicePtr;
}

void LRenderDevice::Initialize(LRenderAPI nApi, int nWidth, int nHeight) {
	if (bIsInitialized) {
		throw LRenderDeviceAlreadyInitializedException(__LINE__, __FILEW__);
		return;
	}
#pragma region DXGI_INIT
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&pDxgiFactory));
	if (FAILED(hr)) {
		throw LRenderDeviceUnableToInitializeException(__LINE__, __FILEW__, L"Unable to CreateDXGIFactory1!");
	}

	hr = pDxgiFactory->EnumAdapters1(0, &pDxgiAdapterOrig);
	if (FAILED(hr)) {
		throw LRenderDeviceUnableToInitializeException(__LINE__, __FILEW__, L"Unable to enumerate adapters!");
	}

	hr = pDxgiAdapterOrig->QueryInterface(IID_PPV_ARGS(&pDxgiAdapter));
	if (FAILED(hr)) {
		throw LRenderDeviceUnableToInitializeException(__LINE__, __FILEW__, L"Unable to query IDXGIAdapter3!");
	}

	DXGI_ADAPTER_DESC1 adapterDesc = {};
	hr = pDxgiAdapter->GetDesc1(&adapterDesc);
	if (FAILED(hr)) {
		throw LRenderDeviceUnableToInitializeException(__LINE__, __FILEW__, L"Unable to get adapter description!");
	}

	m_AdapterName = std::wstring(adapterDesc.Description);
	m_VideoMemory = adapterDesc.DedicatedVideoMemory;

	hr = pDxgiAdapter->EnumOutputs(0, &pDxgiOutputOrig);
	if (FAILED(hr)) {
		throw LRenderDeviceUnableToInitializeException(__LINE__, __FILEW__, L"Unable to enumerate outputs!");
	}

	hr = pDxgiOutputOrig->QueryInterface(IID_PPV_ARGS(&pDxgiOutput));
	if (FAILED(hr)) {
		throw LRenderDeviceUnableToInitializeException(__LINE__, __FILEW__, L"Unable to query IDXGIOutput3!");
	}

	UINT numModes = 0;
	hr = pDxgiOutput->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(hr)) {
		throw LRenderDeviceUnableToInitializeException(__LINE__, __FILEW__, L"Unable to get display mode list!");
	}

	DXGI_MODE_DESC1 * modeDescs = new DXGI_MODE_DESC1[numModes];

	hr = pDxgiOutput->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, modeDescs);
	if (FAILED(hr)) {
		throw LRenderDeviceUnableToInitializeException(__LINE__, __FILEW__, L"Unable to get display mode list!");
	}

	delete [] modeDescs;

	int sW = GetScreenWidth();
	int sH = GetScreenHeight();

	UINT numerator;
	UINT denominator;
	/*
	for (int i = 0; i < numModes; i++)
	{
		if (modeDescs[i].Width == (unsigned int)sW)
		{
			if (modeDescs[i].Height == (unsigned int)sH)
			{
				numerator = modeDescs[i].RefreshRate.Numerator;
				denominator = modeDescs[i].RefreshRate.Denominator;
			}
		}
	}
	*/

#pragma endregion

	bIsInitialized = true;
}

std::wstring LRenderDevice::GetAdapterName() {
	if (bIsInitialized) {
		return m_AdapterName;
	}
	throw LRenderDeviceNotInitializedException(__LINE__, __FILEW__);
}

std::uint64_t LRenderDevice::GetVideoMemory() {
	if (bIsInitialized) {
		return m_VideoMemory;
	}
	throw LRenderDeviceNotInitializedException(__LINE__, __FILEW__);
}

std::uint64_t LRenderDevice::GetUsedVideoMemory() {
	if (bIsInitialized) {
		DXGI_QUERY_VIDEO_MEMORY_INFO info = {};
		pDxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);
		return info.CurrentUsage;
	}
	throw LRenderDeviceNotInitializedException(__LINE__, __FILEW__);
}