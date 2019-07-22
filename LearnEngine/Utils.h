// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once

#include <string>
#include <Windows.h>
#include <algorithm> 
#include <cctype>
#include <locale>

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

inline int GetScreenWidth() {
	return GetSystemMetrics(SM_CXSCREEN);
}

inline int GetScreenHeight() {
	return GetSystemMetrics(SM_CYSCREEN);
}

// trim from start (in place)
static inline void ltrim(std::wstring& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::wstring& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::wstring& s) {
	ltrim(s);
	rtrim(s);
}

// trim from start (copying)
static inline std::wstring ltrim_copy(std::wstring s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
static inline std::wstring rtrim_copy(std::wstring s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
static inline std::wstring trim_copy(std::wstring s) {
	trim(s);
	return s;
}
