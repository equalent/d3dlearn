// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once
#include "LEngine.h"
#include "LException.h"
#include <memory>
#include <string>

DEFINE_EXCEPTION_WITH_MESSAGE(LTextRendererUnableToInitializeFTException, L"Unable to initialize FreeType!");

class LTextRenderer {
public:
	static std::shared_ptr<LTextRenderer> Instance();
	void Initialize();
	void Shutdown();
private:
	LTextRenderer();
};