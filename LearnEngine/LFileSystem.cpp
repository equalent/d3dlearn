// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#include "LFileSystem.h"
#include "xxhash.h"
#include "Utils.h"
#include <cstdio>

std::shared_ptr<LFileSystem> FileSystemPtr = nullptr;

namespace fs = std::experimental::filesystem;

void WriteFiles(const wchar_t* nPakPath, const wchar_t** nPakFiles, uint64_t nPakFilesCount) {
	std::ofstream ofs(nPakPath, std::ofstream::out | std::ofstream::binary);
	std::ofstream ofs_strings(std::wstring(nPakPath) + std::wstring(L".strings"), std::wofstream::out);
	ofs.seekp(0, std::ios::beg);
	ofs.write("LPAK", 5);
	// File entry:
	// uint64 Filename hash
	// uint64 File size
	// uint64 File offset
	// uint64 File hash

	// To calculate file list size:
	// uint64 * 4 * nPakFilesCount

	uint64_t fListSize = sizeof(uint64_t) * 4 * nPakFilesCount;
	// 5 (LPAK) + uint64_t (file count) + fListSize
	uint64_t fListEndOffset = 5 + sizeof(uint64_t) + fListSize;
	uint64_t fLastOffset = fListEndOffset;
	std::vector<std::vector<uint64_t>> _List;
	std::vector<char*> _BufList;
	for (uint64_t i = 0; i < nPakFilesCount; i++) {
		std::ifstream ifs(nPakFiles[i], std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
		std::wstring fName(nPakFiles[i]);
		fs::path fPath(fName);
		std::string fFileName = fPath.filename().string();
		uint64_t fFileNameHash = XXH64(fFileName.c_str(), fFileName.size() + 1, 0);
		uint64_t fFileSize = ifs.tellg();
		uint64_t fFileOffset = fLastOffset;
		fLastOffset += fFileSize;
		char* fBuf = new char[fFileSize];
		ifs.seekg(0, std::ios::beg);
		ifs.read(fBuf, fFileSize);
		uint64_t fFileHash = XXH64(fBuf, fFileSize, 0);
		_BufList.push_back(fBuf);
		std::vector<uint64_t> _Li;
		_Li.push_back(fFileNameHash);
		_Li.push_back(fFileSize);
		_Li.push_back(fFileOffset);
		_Li.push_back(fFileHash);
		_List.push_back(_Li);
		ifs.close();
		wprintf_s(L"FILE \"%s\":\n\tFilename hash: %lld\n\tFile size: %s\n\tFile offset: %lld\n\tFile hash: %lld\n\n", fPath.filename().wstring().c_str(), fFileNameHash, humanSize(fFileSize).c_str(), fFileOffset, fFileHash);

		// Write to .strings
		ofs_strings << fFileNameHash << "\t" << fFileName << "\t" << fFileSize << "\t" << fFileOffset << "\t" << fFileHash;
	}
	uint64_t fileCount = _List.size();
	ofs.write((const char*)& fileCount, sizeof(uint64_t));
	for (uint64_t i = 0; i < nPakFilesCount; i++) {
		uint64_t fFileNameHash = _List[i][0];
		uint64_t fFileSize = _List[i][1];
		uint64_t fFileOffset = _List[i][2];
		uint64_t fFileHash = _List[i][3];
		ofs.write((const char*)& fFileNameHash, sizeof(uint64_t));
		ofs.write((const char*)& fFileSize, sizeof(uint64_t));
		ofs.write((const char*)& fFileOffset, sizeof(uint64_t));
		ofs.write((const char*)& fFileHash, sizeof(uint64_t));
	}
	for (uint64_t i = 0; i < nPakFilesCount; i++) {
		ofs.write(_BufList[i], _List[i][1]);
		delete[] _BufList[i];
	}
	ofs.flush();
	ofs_strings.flush();
	ofs.close();
	ofs_strings.close();
}

int GetPakInfo(const wchar_t* pakPath, PakInfo* pakInfo) {
	auto pFS = LFileSystem::Instance();
	try {
		pFS->Initialize(pakPath);
	}
	catch (LFileSystemPakBadFormat) {
		return -2;
	}

	auto files = pFS->GetAllFiles();
	PakFile* pakFiles = new PakFile[files.size()];
	int i = 0;
	for (auto file : files) {
		pakFiles[i].filenameHash = file->GetFilenameHash();
		pakFiles[i].hash = file->GetHash();
		pakFiles[i].offset = file->GetOffset();
		pakFiles[i].size = file->GetSize();
		++i;
	}
	pakInfo->filesCount = files.size();
	pakInfo->files = pakFiles;
	return 0;
}

LFile::LFile(char* nPtr, uint64_t nSize) {
	mPtr = nPtr;
	mSize = nSize;
}

LFile::LFile(char* nPtr, uint64_t nSize, uint64_t nOffset, uint64_t nHash, uint64_t nFilenameHash) : LFile(nPtr, nSize) {
	mOffset = nOffset;
	mHash = nHash;
	mFilenameHash = nFilenameHash;
}

LFile::~LFile() {
	delete[] mPtr;
}

char* LFile::GetPtr() {
	return mPtr;
}

uint64_t LFile::GetSize() {
	return mSize;
}

uint64_t LFile::GetOffset() {
	return mOffset;
}

uint64_t LFile::GetHash() {
	return mHash;
}

uint64_t LFile::GetFilenameHash() {
	return mFilenameHash;
}

std::wstring LFile::ReadAsString() {
	std::vector<char> strv;
	for (uint64_t i = 0; i < GetSize(); i++) {
		char c = GetPtr()[i];
		if (c != -51) {
			strv.push_back(c);
		}
	}
	return convert_utf8_to_utf16(std::string(strv.begin(), strv.end()));
}

LFileSystem::LFileSystem() {}

std::shared_ptr<LFileSystem> LFileSystem::Instance() {
	if (!FileSystemPtr) {
		LFileSystem* newFileSystem = new LFileSystem();
		FileSystemPtr = std::shared_ptr<LFileSystem>(newFileSystem);
	}
	return FileSystemPtr;
}

void LFileSystem::Initialize(std::experimental::filesystem::path nPakPath) {
	mPakStream = std::ifstream(nPakPath);
	const char* magic = new char[5];
	mPakStream.read((char*)magic, 5);
	if (strcmp(magic, "LPAK") != 0) {
		throw LFileSystemPakBadFormat(__LINE__, __FILEW__);
	}
	uint64_t numFiles;
	mPakStream.read((char*)& numFiles, sizeof(uint64_t));
	for (uint64_t i = 0; i < numFiles; i++) {
		uint64_t fFileNameHash;
		mPakStream.read((char*)& fFileNameHash, sizeof(uint64_t));
		uint64_t fFileSize;
		mPakStream.read((char*)& fFileSize, sizeof(uint64_t));
		uint64_t fFileOffset;
		mPakStream.read((char*)& fFileOffset, sizeof(uint64_t));
		uint64_t fFileHash;
		mPakStream.read((char*)& fFileHash, sizeof(uint64_t));

		std::vector<uint64_t> _Li;
		_Li.push_back(fFileSize);
		_Li.push_back(fFileOffset);
		_Li.push_back(fFileHash);

		mList.insert(std::pair<uint64_t, std::vector<uint64_t>>(fFileNameHash, _Li));
	}
}

std::shared_ptr<LFile> LFileSystem::GetFile(std::wstring nFileName) {
	std::string sFName = convert_utf16_to_utf8(nFileName);
	uint64_t fFileNameHash = XXH64(sFName.c_str(), sFName.size() + 1, 0);
	try {
		std::vector<uint64_t> _Li = mList.at(fFileNameHash);
		mPakStream.seekg(_Li[1], std::ios::beg);
		char* fBuf = new char[_Li[0]];
		mPakStream.read(fBuf, _Li[0]);
		return std::make_shared<LFile>(fBuf, _Li[0]);
	}
	catch (std::out_of_range e) {
		throw LFileSystemFileNotFoundException(__LINE__, __FILEW__);
	}
}

std::vector<std::shared_ptr<LFile>> LFileSystem::GetAllFiles() {
	std::vector<std::shared_ptr<LFile>> res;
	for (auto& [_Hash, _Value] : mList) {
		mPakStream.seekg(_Value[1], std::ios::beg);
		char* fBuf = new char[_Value[0]];
		mPakStream.read(fBuf, _Value[0]);
		res.push_back(std::make_shared<LFile>(fBuf, _Value[0], _Value[1], _Value[2], _Hash));
	}
	return res;
}