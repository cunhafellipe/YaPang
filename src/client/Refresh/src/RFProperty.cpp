// Original: client\Refresh\src\RFProperty.cpp (JP 983 exe; the RF*Property
// text parsers (slot 3), one block 0x464f40-0x465240 placed in
// jp/refresh_placed.tsv. The file name is assumed: the only assert in the block
// is RFManager.cpp's string-to-bool converter (line 1180) inlined by LTCG,
// whose sibling converters 0x419d50-0x41a310 stay out of line in RFManager's
// code, so the parsers are another file's callers of them.)
#include <stdlib.h>
#include <string.h>

#include <string>

#include "RFManager.h"
#include "RFProperty.h"
#include "RFStringUtil.h"

// 0x464f40: the attribute text is UTF-8
static std::wstring ToWString(std::string const& text) {
	return Utf8ToWString(text.c_str());
}

// 0x464f70
void RFStringProperty::Parse(std::string const& text) {
	m_value = ToWString(text);
}

// 0x464ff0 (LTCG inlined ParseBoolean, whose assert names src\RFManager.cpp,
// line 1180)
void RFBooleanProperty::Parse(std::string const& text) {
	m_value = ParseBoolean(text);
}

// 0x465070
void RFIntegerProperty::Parse(std::string const& text) {
	m_value = atoi(text.c_str());
}

// 0x4650a0 (LTCG inlined ParseStartPosition)
void RFStartPositionProperty::Parse(std::string const& text) {
	m_value = ParseStartPosition(text);
}

// 0x465110 (LTCG inlined ParseEscKeyAction)
void RFEscKeyActionProperty::Parse(std::string const& text) {
	m_value = ParseEscKeyAction(text);
}

// 0x465140
void RFAnchorProperty::Parse(std::string const& text) {
	m_value = ParseAnchor(text);
}

// 0x465160
void RFColorProperty::Parse(std::string const& text) {
	m_value = ParseColor(text);
}

// 0x465180
void RFHAlignProperty::Parse(std::string const& text) {
	m_value = ParseHorzAlign(text);
}

// 0x4651a0
void RFVAlignProperty::Parse(std::string const& text) {
	m_value = ParseVertAlign(text);
}

// 0x4651c0
void RFTextTrimmingProperty::Parse(std::string const& text) {
	m_value = ParseTextTrimming(text);
}

// 0x4651e0 (LTCG inlined ParseOrientation)
void RFOrientationProperty::Parse(std::string const& text) {
	m_value = ParseOrientation(text);
}
