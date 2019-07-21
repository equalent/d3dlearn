// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once

#include <string>
#include <sstream>
#include <exception>

#include "Utils.h"

class LException {
public:
	LException(int nLine, std::string nFile, std::wstring nMessage) {
		Line = nLine;
		File = nFile;
		Message = nMessage;
		Name = "LException";
	}

	std::wstring Format() {
		std::wostringstream woss;
		woss << L"EXCEPTION: [";
		woss << convert_utf8_to_utf16(Name);
		woss << L"@";
		woss << Line;
		woss << L"@";
		woss << convert_utf8_to_utf16(File);
		woss << L"]: ";
		woss << Message;

		return woss.str().c_str();
	}
protected:
	int Line;
	std::string Name;
	std::string File;
	std::wstring Message;
};

#define DEFINE_EXCEPTION(ExceptionType); class ExceptionType:public LException{public:\
 ExceptionType (int nLine, std::string nFile, std::wstring nMessage) :\
 LException(nLine, nFile, nMessage) {Name = #ExceptionType;}};

#define DEFINE_EXCEPTION_WITH_MESSAGE(ExceptionType, ExceptionMessage); class ExceptionType:\
 public LException{public:ExceptionType (int nLine, std::string nFile) :\
 LException(nLine, nFile, ExceptionMessage) {Name = #ExceptionType;}};

#define IMPLEMENT_EXCEPTION(ExceptionType); public:\
 ExceptionType (int nLine, std::string nFile, std::wstring nMessage) :\
 LException(nLine, nFile, nMessage) {Name = #ExceptionType;}

#define IMPLEMENT_EXCEPTION_WITH_MESSAGE(ExceptionType, ExceptionMessage); public:\
 ExceptionType (int nLine, std::string nFile) :\
 LException(nLine, nFile, ExceptionMessage) {Name = #ExceptionType;}