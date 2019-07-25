// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once
#include "LEngine.h"
#include "LException.h"
#include <memory>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>

DEFINE_EXCEPTION_WITH_MESSAGE(LFileSystemPakNotFoundException, L"PAK file was not found!");
DEFINE_EXCEPTION_WITH_MESSAGE(LFileSystemPakBadFormat, L"PAK has unsupported format!");
DEFINE_EXCEPTION_WITH_MESSAGE(LFileSystemFileNotFoundException, L"File in the PAK was not found!");

class LFile {
public:
	LFile(char* nPtr, uint64_t nSize);
	~LFile();
	char* GetPtr();
	uint64_t GetSize();
	std::wstring ReadAsString();
private:
	char* mPtr;
	uint64_t mSize;
};

class LFileSystem {
public:
	static std::shared_ptr<LFileSystem> Instance();
	void Initialize(std::experimental::filesystem::path nPakPath);
	void Shutdown();
	std::shared_ptr<LFile> GetFile(std::wstring nFileName);
private:
	LFileSystem();
	std::map<uint64_t, std::vector<uint64_t>> mList;
	std::ifstream mPakStream;
};

extern "C" LE_API void WriteFiles(const wchar_t *, const wchar_t** nPakFiles, uint64_t nPakFilesCount);