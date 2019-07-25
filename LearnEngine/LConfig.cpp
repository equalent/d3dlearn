// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#include "LConfig.h"
#include "LFileSystem.h"
#include "Utils.h"
#include <sstream>

namespace fs = std::experimental::filesystem;

LConfig::LConfig() {
	std::shared_ptr<LFile> pFile = LFileSystem::Instance()->GetFile(L"learn.ini");
	m_Ini = std::make_unique<inipp::Ini<wchar_t>>();
	OutputDebugStringW(pFile->ReadAsString().c_str());
	std::wistringstream wis(pFile->ReadAsString());
	m_Ini->parse(wis);
	if (m_Ini->errors.size() > 0) {
		throw LConfigParseErrorException(__LINE__, __FILEW__);
	}
}

inipp::Ini<wchar_t>::Sections LConfig::GetSections() {
	return m_Ini->sections;
}