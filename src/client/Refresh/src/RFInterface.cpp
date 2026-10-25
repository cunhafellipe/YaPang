// Original: client\Refresh\src\RFInterface.cpp (JP 983 exe, unit bounds in
// jp/refresh_units.tsv; the file name is assumed from the class)
#include <new>

#include "RFInterface.h"

#include <stdio.h>
#include <windows.h>

// lib: 0x4036c0 (scalar deleting destructor), 0x4036f0 (vector deleting
// destructor); both paired through map_symbol.ts

// 0x403660
void RFInterface::OnKeyboardAcquired(bool) {
}

// 0x403670
void RFInterface::OnInputMethodStateChanged(InputMethodStateTypes::Type) {
}

// 0x403680
RFInterface::~RFInterface() {
}

// 0x403690
RFInterface::RFInterface() {
}

// 0x4036a0
RFInterface::RFInterface(RFInterface const&) {
}

// 0x4036b0
RFInterface& RFInterface::operator=(RFInterface const&) {
	return *this;
}

// 0x42ac40
std::wstring RFInterface::GetBasePath(RFBASEPATHTYPE) {
	return L"";
}

// 0x42ac80
void* RFInterface::FOpen(const std::wstring& path) {
	FILE* file;
	return _wfopen_s(&file, path.c_str(), L"rb") == 0 ? file : NULL;
}

// 0x42acb0
void RFInterface::FClose(void* file) {
	fclose(static_cast<FILE*>(file));
}

// 0x42acd0
int32_t RFInterface::FTell(void* file) {
	return ftell(static_cast<FILE*>(file));
}

// 0x42acf0
int32_t RFInterface::FSeek(void* file, int32_t offset, int32_t origin) {
	return fseek(static_cast<FILE*>(file), offset, origin);
}

// 0x42ad10
int32_t RFInterface::FRead(void* file, void* buffer, int32_t size) {
	return static_cast<int32_t>(fread(buffer, size, 1, static_cast<FILE*>(file))) * size;
}

// 0x42ad40
std::wstring RFInterface::TranslateText(const std::wstring& text) {
	return text;
}

// 0x42ad80
void RFInterface::DebugPrint(const wchar_t* text) {
	OutputDebugStringW(text);
}
