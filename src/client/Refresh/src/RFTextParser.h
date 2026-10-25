// Original: client\Refresh\src\RFTextParser.h (JP 983 exe; ParseFont's assert
// names this private header, docs/JP_REFRESH_UI.md; the class is not exported)
#ifndef PG_CLIENT_REFRESH_SRC_RFTEXTPARSER_H
#define PG_CLIENT_REFRESH_SRC_RFTEXTPARSER_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "RFStringUtil.h"
#include "RFTextComponent.h"
#include "TextBlock.h"
#include "defines.h"
#include "tinyxml/tinyxml.h"

namespace TextComponent {

// One run of text with its markup attributes (0x64 bytes; name invented), RFTextParser's at 0x4 (ctor
// 0x46c650 sets all but the color; dtor 0x46c6d0, esi). TextBlock 0x14-0x78: the same fields, strings swapped.
struct TextRun {
	std::wstring text;          // 0x00
	uint32_t style;             // 0x1c, bit 0x10: a hyperlink
	std::wstring hyperLink;     // 0x20, TextBlock's m_hyperLink (0x468d80 copies it there)
	std::wstring styleText;     // 0x3c, TextBlock's m_styleText (0x468d80 copies it there)
	uint32_t color;             // 0x58
	int32_t linkState;          // 0x5c, 2 after construction; GetHyperLinkAt matches it (or 3)
	bool styleFlag;             // 0x60, true after construction (role not read yet)
	TextRun();                  // 0x46bc90
	// Empty, style 0, opaque black, styleFlag set, linkState kept (0x46bcd0, LTCG: this in eax).
	void Reset();
	// Every attribute but the text equal (0x46c5b0, LTCG: this in edi, other in esi).
	bool HasSameStyle(TextRun const& other) const;
};
CHECK_SIZE(TextRun, 0x64);
CHECK_FIELD(TextRun, style, 0x1c);
CHECK_FIELD(TextRun, hyperLink, 0x20);
CHECK_FIELD(TextRun, styleText, 0x3c);
CHECK_FIELD(TextRun, color, 0x58);
CHECK_FIELD(TextRun, linkState, 0x5c);
CHECK_FIELD(TextRun, styleFlag, 0x60);
} // namespace TextComponent

// Splits marked-up text into text components: vtable 0xd66c9c, 1 slot, size 0x78 (RFText 0x466f20 builds
// one on its stack). Not exported: members pair as sub_<va>. Inline ctor 0x46c650 (LTCG: this in eax).
class RFTextParser {
	LAYOUT_CHECK_FRIEND;
public:
	RFTextParser();                             // 0x46c650
	virtual ~RFTextParser();                    // [0] scalar deleting destructor 0x46c6a0; 0x46c750
	// One tag ("<B>", "</IM>"): sets/clears its style bit (B 1, I 2, F 4, IM 8, A 0x10, U 0x20),
	// isEndTag set for a closing tag; false if unknown (LTCG: isEndTag in edx; name invented).
	bool ParseTag(bool& isEndTag, std::wstring const& tag);     // 0x46d380
	bool Parse(std::wstring const& source);     // 0x46c820 (described at its definition)
	std::vector<RFTextComponent*>& GetComponents() { return m_components; }  // RFParagraph 0x466f20 takes them, clears it
private:
	bool ParseTagAt(bool& isEndTag, uint32_t& pos, std::wstring const& text);  // 0x46d1d0
	static uint32_t ExtractTag(std::wstring const& text, std::wstring& tag);    // 0x46d300
	static void ReplaceAll(std::wstring& text, std::wstring const& from, std::wstring const& to);  // 0x46d680
	// The tag parsers, defined below (ParseFont's assert names this header, line 134): tag is
	// one tag as XML text, run the run it sets (LTCG: tag in ecx, run on the stack).
	// <img src="...">: the image name, kept as the run's style text.
	static void ParseImage(std::wstring const& tag, TextComponent::TextRun* run);  // 0x46bd20
	// <a href= activate="rbuttondown|rbuttonup" style="no">: link, activating event (2: neither), styled.
	static void ParseLink(std::wstring const& tag, TextComponent::TextRun* run);   // 0x46bf00
	// <font color="#rrggbb[aa]">: the run's color, 0xAARRGGBB (alpha 0xff for six digits).
	static void ParseFont(std::wstring const& tag, TextComponent::TextRun* run);   // 0x46c2b0
	// m_run's style bits (out of line unreferenced; LTCG: this in eax/edx; names invented).
	void AddStyle(uint32_t const& bit);         // 0x46c620
	void RemoveStyle(uint32_t const& bit);      // 0x46c630
	bool HasStyle(uint32_t const& bit) const;   // 0x46c640
	TextComponent::TextRun m_run;               // 0x04, the run being parsed
	std::vector<RFTextComponent*> m_components; // 0x68, owned: deleted by the destructor
};
CHECK_SIZE(RFTextParser, 0x78);
BEGIN_LAYOUT_CHECK(RFTextParser)
	CHECK_FIELD(RFTextParser, m_run, 0x04);
	CHECK_FIELD(RFTextParser, m_components, 0x68);
END_LAYOUT_CHECK

// 0x46bd20
inline void RFTextParser::ParseImage(std::wstring const& tag, TextComponent::TextRun* run) {
	rf::TiXmlDocument document;
	document.Parse(WStringToMultiByte(tag.c_str()).c_str(), 0, rf::TIXML_DEFAULT_ENCODING);
	rf::TiXmlElement* element = document.FirstChildElement("img");
	if (element != 0) {
		for (rf::TiXmlAttribute* attribute = element->FirstAttribute(); attribute != 0;
		     attribute = attribute->Next()) {
			if (strcmp(attribute->Name(), "src") == 0)
				run->styleText = MultiByteToWString(attribute->Value());
		}
	}
}

// 0x46bf00
inline void RFTextParser::ParseLink(std::wstring const& tag, TextComponent::TextRun* run) {
	rf::TiXmlDocument document;
	document.Parse(WStringToMultiByte(tag.c_str()).c_str(), 0, rf::TIXML_DEFAULT_ENCODING);
	rf::TiXmlElement* element = document.FirstChildElement("a");
	if (element != 0) {
		run->styleFlag = true;
		run->linkState = 2;
		for (rf::TiXmlAttribute* attribute = element->FirstAttribute(); attribute != 0;
		     attribute = attribute->Next()) {
			if (strcmp(attribute->Name(), "href") == 0) {
				run->hyperLink = MultiByteToWString(attribute->Value());
			} else if (strcmp(attribute->Name(), "activate") == 0) {
				std::string value(attribute->Value());
				if (strcmp(value.c_str(), "rbuttondown") == 0)
					run->linkState = 0;
				else if (strcmp(value.c_str(), "rbuttonup") == 0)
					run->linkState = 1;
			} else if (strcmp(attribute->Name(), "style") == 0) {
				std::string value(attribute->Value());
				if (strcmp(value.c_str(), "no") == 0)
					run->styleFlag = false;
			}
		}
	}
}

// 0x46c2b0
inline void RFTextParser::ParseFont(std::wstring const& tag, TextComponent::TextRun* run) {
	rf::TiXmlDocument document;
	document.Parse(WStringToMultiByte(tag.c_str()).c_str(), 0, rf::TIXML_DEFAULT_ENCODING);
	rf::TiXmlElement* element = document.FirstChildElement("font");
	if (element != 0) {
		for (rf::TiXmlAttribute* attribute = element->FirstAttribute(); attribute != 0;
		     attribute = attribute->Next()) {
			if (strcmp(attribute->Name(), "color") == 0) {
				std::string string(attribute->Value());
				if (string.at(0) == '#') {
					assert(string.size() == 7 || string.size() == 9);
					int rgba[4] = { 0xff, 0xff, 0xff, 0xff };
					char const* digits = string.c_str() + 1;
					uint32_t count = (string.size() - 1) / 2;
					for (uint32_t i = 0; i < count; ++i, digits += 2)
						sscanf_s(digits, "%02x", &rgba[i]);
					run->color = ((rgba[3] & 0xff) << 24) | ((rgba[0] & 0xff) << 16)
						| ((rgba[1] & 0xff) << 8) | (rgba[2] & 0xff);
				}
			}
		}
	}
}

#endif
