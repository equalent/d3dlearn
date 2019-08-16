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

struct PakFile {
	uint64_t filenameHash;
	uint64_t size;
	uint64_t offset;
	uint64_t hash;
};

struct PakInfo {
	uint64_t filesCount;
	PakFile* files;
};

class LFile {
public:
	LFile(char* nPtr, uint64_t nSize);
	LFile(char* nPtr, uint64_t nSize, uint64_t nOffset, uint64_t nHash, uint64_t nFilenameHash);
	~LFile();
	char* GetPtr();
	uint64_t GetSize();
	uint64_t GetOffset();
	uint64_t GetHash();
	uint64_t GetFilenameHash();
	std::wstring ReadAsString();
private:
	char* mPtr;
	uint64_t mSize;
	uint64_t mOffset = 0;
	uint64_t mHash = 0;
	uint64_t mFilenameHash = 0;
};

class LFileSystem {
public:
	static std::shared_ptr<LFileSystem> Instance();
	void Initialize(std::experimental::filesystem::path nPakPath);
	void Shutdown();
	std::shared_ptr<LFile> GetFile(std::wstring nFileName);
	std::vector< std::shared_ptr<LFile> > GetAllFiles();
private:
	LFileSystem();
	std::map<uint64_t, std::vector<uint64_t>> mList;
	std::ifstream mPakStream;
};

extern "C" {
	LE_API int GetPakInfo(const wchar_t*, PakInfo*);
	LE_API void WriteFiles(const wchar_t*, const wchar_t** nPakFiles, uint64_t nPakFilesCount);
}