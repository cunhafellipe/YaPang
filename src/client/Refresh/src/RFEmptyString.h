// Original: unknown (JP 983 exe). The shared empty std::wstring at 0xe46e94
// is unexported and its defining unit is not identified (statics.tsv: its
// initializer 0xcc7890 sits between RFScrollBar's and RFDirector's); this
// declaration and its name are ours, a stand-in until the owner is found.
#ifndef PG_CLIENT_REFRESH_SRC_RFEMPTYSTRING_H
#define PG_CLIENT_REFRESH_SRC_RFEMPTYSTRING_H

#include <string>

// The empty string returned for a missing value (RFWnd::GetUserString
// 0x408350, RFAnimButton::GetAnimName, RFComboBox::GetText), 0xe46e94.
// Defined in RFAnimButton.cpp.
extern std::wstring const g_emptyWString;

#endif
