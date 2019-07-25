// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#include "LTextRenderer.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Utils.h"

std::shared_ptr<LTextRenderer> TextRendererPtr = nullptr;

FT_Library  pFTLibrary;

LTextRenderer::LTextRenderer(){}

std::shared_ptr<LTextRenderer> LTextRenderer::Instance() {
	if (!TextRendererPtr) {
		LTextRenderer* newTextRenderer = new LTextRenderer();
		TextRendererPtr = std::shared_ptr<LTextRenderer>(newTextRenderer);
	}
	return TextRendererPtr;
}

void LTextRenderer::Initialize() {
	FT_Error error = FT_Init_FreeType(&pFTLibrary);
	if (error != FT_Err_Ok)
	{
		throw LTextRendererUnableToInitializeFTException(__LINE__, __FILEW__);
	}
	else {
		OutputDebugStringW(L"Successfully initialized FreeType!\n");
		int major, minor, patch;
		FT_Library_Version(pFTLibrary, &major, &minor, &patch);
		std::wostringstream woss;
		woss << L"FreeeType version: " << major << L"." << minor << L"." << patch << L".\n";
		OutputDebugStringW(woss.str().c_str());
	}
}