// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once

#include <string>
#include <Windows.h>

inline std::wstring convert_utf8_to_utf16(const std::string& source)
{
	if (source.empty())
		return std::wstring();

	const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, &source[0], static_cast<int>(source.size()), NULL, 0);
	std::wstring result(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &source[0], static_cast<int>(source.size()), &result[0], size_needed);
	return result;
}



inline std::string convert_utf16_to_utf8(const std::wstring& source)
{
	if (source.empty())
		return std::string();

	const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &source[0], static_cast<int>(source.size()), NULL, 0, NULL, NULL);
	std::string result(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &source[0], static_cast<int>(source.size()), &result[0], size_needed, NULL, NULL);
	return result;
}