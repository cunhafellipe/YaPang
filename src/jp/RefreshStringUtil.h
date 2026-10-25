// The JP 983 game's Shift-JIS conversions used by its Refresh glue
// (RefreshInterface::PlaySfx/FOpen, RefreshRenderer::CreateTexture,
// RFTooltipHelper). The names are ours: the exe has no symbol for them.
#ifndef PG_JP_REFRESHSTRINGUTIL_H
#define PG_JP_REFRESHSTRINGUTIL_H

#include <string>

// 0x5e4d60: WideCharToMultiByte(932 (Shift-JIS), 0, text, -1, ...) with the
// default character "?" (0xd64784), sized by a first call, into a stack
// buffer (alloca), returned as a std::string.
std::string WStringToShiftJis(const std::wstring& text);

// 0x5e4ef0: MultiByteToWideChar(932 (Shift-JIS), 0, text, -1, ...), sized by
// a first call, into a stack buffer (alloca), assigned to the result up to
// its terminator.
std::wstring ShiftJisToWString(const std::string& text);

#endif
