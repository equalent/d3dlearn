// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once

#include <filesystem>
#include "inipp.h"
#include "LException.h"
#include <memory>

DEFINE_EXCEPTION_WITH_MESSAGE(LConfigParseErrorException, L"Unable to parse the config file!");

class LConfig {
public:
	LConfig();
	inipp::Ini<wchar_t>::Sections GetSections();
private:
	std::unique_ptr<inipp::Ini<wchar_t>> m_Ini;
};