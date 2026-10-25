// Original: client\Refresh\src\RFStringUtil.h (JP 983 exe; private header,
// the file name is assumed: none of these helpers is exported)
#ifndef PG_CLIENT_REFRESH_SRC_RFSTRINGUTIL_H
#define PG_CLIENT_REFRESH_SRC_RFSTRINGUTIL_H

#include <string>

// The library's string conversions (client/Refresh/src/RFStringUtil,
// 0x42aa00-0x42abf0). Each converts into an alloca'd buffer with
// MultiByteToWideChar / WideCharToMultiByte and returns a copy of it.
//
// The code page of the narrow side of MultiByteToWString and
// WStringToMultiByte (0xF1DDB4; zero-initialized, so CP_ACP, and nothing in
// the exe writes it).
extern unsigned int g_codePage;

std::wstring MultiByteToWString(char const* text);         // 0x42aa00
std::wstring Utf8ToWString(char const* text);              // 0x42aaa0
std::string WStringToMultiByte(std::wstring const& text);  // 0x42ab40

#endif
