// The JP 983 game's Shift-JIS conversions for its Refresh glue
// (RefreshStringUtil.h).
#include "RefreshStringUtil.h"

#include <vector>

#include <windows.h>

// 0x5e4d60
std::string WStringToShiftJis(const std::wstring& text) {
	// The original converts into an alloca buffer; the port uses a vector.
	int size = WideCharToMultiByte(932, 0, text.c_str(), -1, NULL, 0, "?", NULL);
	std::vector<char> buffer(size > 0 ? size : 1, '\0');
	WideCharToMultiByte(932, 0, text.c_str(), -1, &buffer[0], size, "?", NULL);
	return std::string(&buffer[0]);
}

// 0x5e4ef0
std::wstring ShiftJisToWString(const std::string& text) {
	// The original converts into an alloca buffer; the port uses a vector.
	int size = MultiByteToWideChar(932, 0, text.c_str(), -1, static_cast<wchar_t*>(NULL), 0);
	std::vector<wchar_t> buffer(size > 0 ? size : 1, L'\0');
	MultiByteToWideChar(932, 0, text.c_str(), -1, &buffer[0], size);
	return std::wstring(&buffer[0]);
}
