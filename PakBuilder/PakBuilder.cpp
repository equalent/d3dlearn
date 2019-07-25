#include <cstdint>
#include <cstdio>
#include <csignal>

extern "C" __declspec(dllexport) void WriteFiles(const wchar_t*, const wchar_t** nPakFiles, uint64_t nPakFilesCount);

int wmain(uint64_t argc, wchar_t** argv)
{
	if (argc < 3) {
		printf("Usage: PakBuilder <output filename> <input filename> [input filename...]\n");
		return 1;
	}
	const wchar_t** pakFiles = new const wchar_t* [((uint64_t)argc)-2];
	for (uint64_t i = 0; i < ((uint64_t)argc) - 2; i++) {
		pakFiles[i] = argv[i+2];
	}
	WriteFiles(argv[1], pakFiles, ((uint64_t)argc) - 2);
}