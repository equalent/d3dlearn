// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#include "LConfig.h"
#include <fstream>

namespace fs = std::experimental::filesystem;

LConfig::LConfig(fs::path ConfigPath) {
	m_Ini = std::make_unique<inipp::Ini<wchar_t>>();
	std::wifstream wis(ConfigPath);
	m_Ini->parse(wis);
	if (m_Ini->errors.size() > 0) {
		throw LConfigParseErrorException(__LINE__, __FILEW__);
	}
}

inipp::Ini<wchar_t>::Sections LConfig::GetSections() {
	return m_Ini->sections;
}