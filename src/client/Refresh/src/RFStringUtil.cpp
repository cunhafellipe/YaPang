// Original: client\Refresh\src\RFStringUtil.cpp (JP 983 exe, unit bounds in
// jp/units.tsv; the file name is assumed)
#include <string>

#include "RFStringUtil.h"

#include <malloc.h>
#include <windows.h>
#if !defined(_MSC_VER)
// The port's <malloc.h> has no _alloca yet.
#include <alloca.h>
#define _alloca alloca
#endif

// On the port wchar_t is 4 bytes: the shim's MultiByteToWideChar and
// WideCharToMultiByte convert UTF-8 and the code page to and from code points
// explicitly (port/shim/windows.h), so the same source serves both. The
// (wchar_t*)NULL picks the shim's wchar_t overload over its UTF-16 one.

// 0xF1DDB4
unsigned int g_codePage;

// 0x42aa00
std::wstring MultiByteToWString(char const* text) {
	int length = MultiByteToWideChar(g_codePage, 0, text, -1, (wchar_t*)NULL, 0);
	wchar_t* buffer = (wchar_t*)_alloca(length * sizeof(wchar_t));
	MultiByteToWideChar(g_codePage, 0, text, -1, buffer, length);
	return std::wstring(buffer);
}

// 0x42aaa0
std::wstring Utf8ToWString(char const* text) {
	int length = MultiByteToWideChar(CP_UTF8, 0, text, -1, (wchar_t*)NULL, 0);
	wchar_t* buffer = (wchar_t*)_alloca(length * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, text, -1, buffer, length);
	return std::wstring(buffer);
}

// 0x42ab40
std::string WStringToMultiByte(std::wstring const& text) {
	int length = WideCharToMultiByte(g_codePage, 0, text.c_str(), -1, NULL, 0, "?", NULL);
	char* buffer = (char*)_alloca(length);
	WideCharToMultiByte(g_codePage, 0, text.c_str(), -1, buffer, length, "?", NULL);
	return std::string(buffer);
}
