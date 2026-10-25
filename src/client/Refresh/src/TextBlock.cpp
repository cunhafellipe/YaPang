// Original: client\Refresh\src\TextBlock.cpp (JP 983 exe, unit bounds in
// jp/units.tsv; no assert path names the file: TextComponent::TextBlock's
// out-of-line members, RFTextComponent's scalar deleting destructor at the
// start and RFTextParser's code 0x46c6a0-0x46c805 at the end; RFTextParser's
// tag dispatcher 0x46d380, outside the bounds, is placed here by
// jp/refresh_placed.tsv; the tag parsers are inlines of RFTextParser.h)
//
// The overrides are written with VS 2010 lambdas: the original's closure
// members stay out of line next to each user (LTCG kept them), e.g.
// 0x469410 / 0x469450 (HideSegments / ShowSegments), 0x4697a0 / 0x4697b0
// (GetWidth), 0x4698a0 / 0x4698b0 (GetSegmentFirstChar) and the same pairs
// after slots 16-18.
//
// lib: 0x468c40 (RFTextComponent scalar deleting destructor), 0x468d40
// (TextBlock scalar deleting destructor),
// 0x46b840 (std::_Destroy_range of TextSegment), 0x46c6a0 (RFTextParser scalar
// deleting destructor), 0x46c6d0 (TextRun destructor)
// lib: 0x46a540 (std::vector<TextSegment>::push_back), 0x46a700 (its
// reallocation), 0x46b7b0 (std::_Move of TextSegment, vector::erase/clear),
// 0x46b8e0 (TextSegment copy construction for push_back), 0x46ba20
// (std::_Uninitialized_copy of TextSegment), 0x46b020 /
// 0x46b0f0 / 0x46b1d0 (std::_Find_if of the HitTest / GetSegmentAt /
// GetHyperLinkAt lambdas, paired; LTCG register arguments)
// lib: 0x46b320 (std::_For_each of the Draw lambda, paired), 0x46d870
// (std::transform with towupper, ParseTag)
// lib: 0x46a2a0, 0x46bb10, 0x46bb40, 0x46bbd0, 0x46bc00 (std::wstring tidy of the
// string at +0x20, pieces of the implicit TextSegment / TextRun destructors),
// 0x46b740, 0x46b880, 0x46bb80, 0x46bc30 (allocator construction of a
// TextSegment copy through 0x46b8e0), 0x46b940 (the implicit TextSegment
// operator=), 0x46b010, 0x46b590 (unreferenced std:: iterator helpers)
// lib: 0x46b7a0 (std::_Ptr_cat tag), 0x46b820 (std::_Uninitialized_copy
// wrapper of 0x46ba20), 0x46b9d0, 0x46b9e0 (vector<TextSegment> iterator
// operator--), 0x46b9f0 (iterator operator==), 0x46bb70 (iterator copy),
// 0x46b0e0, 0x46b5f0 (`mov [eax],ecx` one-capture closure or iterator
// constructors between the std::_Find_if / std::find copies)
// lib: 0x46c810 (~RFTextParser's delete lambda body, paired), 0x46d150 (the implicit
// TextRun::operator=, paired), 0x46d1a0 (std::vector<RFTextComponent*>::erase(begin, end),
// unreferenced)
#include <new>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <wctype.h>

#include <algorithm>

#include "RFInterface.h"
#include "RFManager.h"
#include "RFRect.h"
#include "RFRenderContext.h"
#include "RFRenderer.h"
#include "RFStringUtil.h"
#include "RFTextArea.h"
#include "RFTextParser.h"
#include "RFVector2.h"
#include "TextBlock.h"
#include "tinyxml/tinyxml.h"

namespace TextComponent {

// 0x468c70
int32_t TextBlock::GetType() const {
	return 0;
}

// 0x468c80 (LTCG: this in eax)
TextSegment::TextSegment() : x(0), y(0) {
	id = 0;
	firstChar = 0;
	lastChar = 0;
	width = 0;
	height = 0;
	visible = false;
}

// 0x468cb0
TextBlock::TextBlock()
	: m_style(0), m_color(0xff000000), m_linkState(2), m_styleFlag(true) {
	m_selected = false;
}

// 0x468d80
TextBlock::TextBlock(TextRun const& run) {
	m_text = run.text;
	m_style = run.style;
	m_color = run.color;
	m_hyperLink = run.hyperLink;
	m_styleText = run.styleText;
	m_linkState = run.linkState;
	m_styleFlag = run.styleFlag;
	m_selected = false;
}

// 0x468e80
TextBlock::TextBlock(uint32_t const& style, std::wstring const& styleText,
	std::wstring const& hyperLink, uint32_t const& color, int32_t const& linkState, bool styleFlag) {
	m_style = style;
	m_color = color;
	m_hyperLink = hyperLink;
	m_styleText = styleText;
	m_linkState = linkState;
	m_styleFlag = styleFlag;
	m_selected = false;
}

// 0x468f70
TextBlock::~TextBlock() {
}

// 0x469050
bool TextBlock::HitTest(int32_t x, int32_t y) {
	return std::find_if(m_segments.begin(), m_segments.end(), [x, y](TextSegment const& segment) -> bool {
		if (segment.visible) {
			float left = (float)segment.x;
			float top = (float)segment.y;
			RFRect rect(left, top, left + (float)segment.width, top + (float)segment.height);
			if (rect.PtInRect(RFVector2((float)x, (float)y)))
				return true;
		}
		return false;
	}) != m_segments.end();
}

// 0x469150
bool TextBlock::GetSegmentAt(int32_t x, int32_t y, uint32_t hitWidth, bool* pastEnd, int32_t* segmentId) {
	std::vector<TextSegment>::iterator it = std::find_if(m_segments.begin(), m_segments.end(),
		[pastEnd, hitWidth, x, y](TextSegment const& segment) -> bool {
		if (segment.visible) {
			float left = (float)segment.x;
			float top = (float)segment.y;
			RFRect rect(left, top, left + (float)hitWidth, top + (float)segment.height);
			if (rect.PtInRect(RFVector2((float)x, (float)y))) {
				if ((int32_t)(segment.x + segment.width) <= x)
					*pastEnd = true;
				else
					*pastEnd = false;
				return true;
			}
		}
		return false;
	});
	if (it != m_segments.end()) {
		*segmentId = it->id;
		return true;
	}
	return false;
}

// 0x4692a0
bool TextBlock::GetHyperLinkAt(int32_t x, int32_t y, int32_t const* linkState, std::wstring* link) {
	if ((m_style & 0x10) != 0) {
		std::vector<TextSegment>::iterator it = std::find_if(m_segments.begin(), m_segments.end(),
			[x, y, this](TextSegment const& segment) -> bool {
			if (segment.visible) {
				float left = (float)segment.x;
				float top = (float)segment.y;
				RFRect rect(left, top, left + (float)segment.width, top + (float)segment.height);
				if (rect.PtInRect(RFVector2((float)x, (float)y))) {
					SetSelected(true);
					return true;
				}
			}
			SetSelected(false);
			return false;
		});
		if (it != m_segments.end()) {
			if (*linkState == m_linkState || *linkState == 3) {
				*link = m_hyperLink;
				return true;
			}
		}
	}
	return false;
}

// 0x4693f0
void TextBlock::HideSegments() {
	std::for_each(m_segments.begin(), m_segments.end(),
		[](TextSegment& segment) { segment.visible = false; });
}

// 0x469420
void TextBlock::ShowSegments(int32_t) {
	std::for_each(m_segments.begin(), m_segments.end(),
		[this](TextSegment& segment) { segment.visible = true; });
}

// 0x469460
void TextBlock::Draw(float alpha, RFTextArea* area) {
	std::for_each(m_segments.begin(), m_segments.end(),
		[this, area, alpha](TextSegment& segment) {
		segment.visible = true;
		if ((m_style & 8) != 0) {
			area->SendDrawImageEvent(RFVector2((float)segment.x, (float)segment.y), m_styleText);
		} else {
			// The font colour's alpha scaled by the area's.
			FontInfo font = m_font;
			uint32_t color = font.color;
			uint32_t a = static_cast<uint32_t>(static_cast<int32_t>(color >> 24) / 255.0 * alpha * 255.0);
			uint8_t red = static_cast<uint8_t>(color >> 16);
			uint8_t green = static_cast<uint8_t>(color >> 8);
			uint8_t blue = static_cast<uint8_t>(color);
			font.color = ((((a << 8) | red) << 8 | green) << 8) | blue;
			float depth = area->GetRC()->m_depth;
			if (m_selected) {
				RFManager::s_renderer->FillRectangle(0xfaadd6ff, (float)segment.x, (float)segment.y,
					(float)segment.width, (float)segment.height, depth);
			}
			RFManager::s_interface->PrintText(font, segment.x, segment.y,
				std::wstring(segment.text.c_str()), depth);
		}
	});
}

// 0x4696d0
void TextBlock::ClearSelected() {
	m_selected = false;
}

// 0x4696e0
void TextBlock::SetSelected(bool selected) {
	m_selected = selected;
}

// 0x4696f0
uint32_t TextBlock::EraseFront(uint32_t count) {
	return Erase(0, count);
}

// 0x469710
uint32_t TextBlock::Erase(uint32_t pos, uint32_t count) {
	uint32_t length = GetLength();
	if (count == 0 || pos >= length)
		return 0;
	uint32_t erased;
	if (length < pos + count)
		erased = length - pos;
	else
		erased = count;
	m_text.erase(pos, erased);
	return count - erased;
}

// 0x469770
uint32_t TextBlock::GetLength() const {
	return m_text.size();
}

// 0x469780
uint32_t TextBlock::GetWidth() const {
	uint32_t width = 0;
	std::for_each(m_segments.begin(), m_segments.end(),
		[&width](TextSegment const& segment) { width += segment.width; });
	return width;
}

// 0x4697c0
bool TextBlock::IsEmpty() const {
	if (m_text.empty())
		return true;
	return false;
}

// 0x4697d0
bool TextBlock::HasSameStyle(RFTextComponent const* other) const {
	TextBlock const* block = static_cast<TextBlock const*>(other);
	if (GetType() == other->GetType() && m_style == block->m_style && m_color == block->m_color
		&& m_hyperLink == block->m_hyperLink && m_styleText == block->m_styleText
		&& m_linkState == block->m_linkState && m_styleFlag == block->m_styleFlag)
		return true;
	return false;
}

// 0x469870
uint32_t TextBlock::GetSegmentFirstChar(int32_t segmentId) const {
	std::vector<TextSegment>::const_iterator it = std::find_if(m_segments.begin(),
		m_segments.end(), [segmentId](TextSegment const& segment) { return segmentId == segment.id; });
	if (it != m_segments.end())
		return it->firstChar;
	return 0xffffffff;
}

// 0x4698c0
uint32_t TextBlock::GetSegmentLastChar(int32_t segmentId) const {
	std::vector<TextSegment>::const_iterator it = std::find_if(m_segments.begin(),
		m_segments.end(), [segmentId](TextSegment const& segment) { return segmentId == segment.id; });
	if (it != m_segments.end())
		return it->lastChar;
	return 0xffffffff;
}

// 0x469910
uint32_t TextBlock::GetSegmentWidth(int32_t segmentId) const {
	std::vector<TextSegment>::const_iterator it = std::find_if(m_segments.begin(),
		m_segments.end(), [segmentId](TextSegment const& segment) { return segmentId == segment.id; });
	if (it != m_segments.end())
		return it->width;
	return 0xffffffff;
}

// 0x469960
uint32_t TextBlock::GetSegmentHeight(int32_t segmentId) const {
	std::vector<TextSegment>::const_iterator it = std::find_if(m_segments.begin(),
		m_segments.end(), [segmentId](TextSegment const& segment) { return segmentId == segment.id; });
	if (it != m_segments.end())
		return it->height;
	return 0xffffffff;
}

// 0x4699b0
void TextBlock::GetSegmentIdRange(int32_t* firstId, int32_t* lastId) const {
	*firstId = -1;
	*lastId = -1;
	if (!m_segments.empty()) {
		*firstId = m_segments.front().id;
		*lastId = m_segments.back().id;
	}
}

// 0x4699e0
uint32_t TextBlock::SplitSegment(int32_t segmentId, uint32_t* offset) {
	std::vector<TextSegment>::iterator it = std::find_if(m_segments.begin(),
		m_segments.end(), [segmentId](TextSegment const& segment) { return segmentId == segment.id; });
	if (it != m_segments.end()) {
		if (*offset <= it->width) {
			float width = 0.0f;
			uint32_t i;
			for (i = 0; i < it->text.size(); ++i) {
				wchar_t str[2] = {0};
				str[0] = it->text[i];
				RFSize size = RFManager::s_interface->MeasureText(m_font, std::wstring(str));
				width += size.width;
				if ((uint32_t)width > *offset)
					break;
			}
			*offset = 0;
			return i;
		}
		*offset -= it->width;
		return it->lastChar - it->firstChar;
	}
	return 0xffffffff;
}

// 0x469b70
void TextBlock::GetCharPos(uint32_t charPos, int32_t* segmentId, uint32_t* charOffset, uint32_t* x,
	bool fromFront) {
	if (fromFront)
		GetCharPosFromFront(charPos, segmentId, charOffset, x);
	else
		GetCharPosFromBack(charPos, segmentId, charOffset, x);
}

// 0x469bb0
void TextBlock::GetCharPosFromFront(uint32_t charPos, int32_t* segmentId, uint32_t* charOffset,
	uint32_t* x) {
	std::vector<TextSegment>::iterator it = std::find_if(m_segments.begin(), m_segments.end(),
		[charPos](TextSegment const& segment) -> bool {
		if (charPos >= segment.firstChar && charPos <= segment.lastChar)
			return true;
		return false;
	});
	if (it != m_segments.end()) {
		*segmentId = it->id;
		*charOffset = charPos - it->firstChar;
		std::wstring head = it->text.substr(0, *charOffset);
		RFSize size = RFManager::s_interface->MeasureText(m_font, std::wstring(head.c_str()));
		*x = (uint32_t)size.width;
	}
}

// 0x469d20
void TextBlock::GetCharPosFromBack(uint32_t charPos, int32_t* segmentId, uint32_t* charOffset,
	uint32_t* x) {
	std::vector<TextSegment>::reverse_iterator it = std::find_if(m_segments.rbegin(),
		m_segments.rend(), [charPos](TextSegment const& segment) -> bool {
		if (charPos >= segment.firstChar && charPos <= segment.lastChar)
			return true;
		return false;
	});
	if (it != m_segments.rend()) {
		*segmentId = it->id;
		*charOffset = charPos - it->firstChar;
		std::wstring head = it->text.substr(0, *charOffset);
		RFSize size = RFManager::s_interface->MeasureText(m_font, std::wstring(head.c_str()));
		*x = (uint32_t)size.width;
	}
}

// 0x469ea0
int32_t TextBlock::Layout(FontInfo const& font, FontInfo const& linkFont, uint32_t available,
	uint32_t lineWidth, int32_t* segmentId) {
	m_segments.clear();
	m_font = GetFont(font, linkFont);
	if (m_text.size() != 0) {
		TextSegment segment;
		RFSize extent;
		segment.id = *segmentId;
		for (uint32_t i = 0; i < m_text.size(); ++i) {
			wchar_t str[2] = {0};
			str[0] = m_text[i];
			RFSize size = RFManager::s_interface->MeasureText(m_font, std::wstring(str));
			extent.width += size.width;
			extent.height = std::max(extent.height, size.height);
			uint32_t segmentWidth = (uint32_t)extent.width;
			if (segmentWidth <= available) {
				segment.width = segmentWidth;
				segment.height = (uint32_t)extent.height;
				segment.text += str;
			} else {
				if (segment.text.size() != 0)
					m_segments.push_back(segment);
				++*segmentId;
				segment.id = *segmentId;
				available = lineWidth;
				segment.firstChar = i;
				segment.width = (uint32_t)size.width;
				segment.height = (uint32_t)size.height;
				segment.text = str;
				extent = size;
			}
			segment.lastChar = i + 1;
		}
		m_segments.push_back(segment);
		return available - m_segments.back().width;
	}
	return -1;
}

// 0x46a2d0
void TextBlock::SetSegmentPosition(int32_t segmentId, int32_t x, int32_t y) {
	std::vector<TextSegment>::iterator it = std::find_if(m_segments.begin(),
		m_segments.end(), [segmentId](TextSegment const& segment) { return segment.id == segmentId; });
	if (it != m_segments.end()) {
		it->x = x;
		it->y = y;
	}
}

// 0x46a320
void TextBlock::SetText(std::wstring const& text) {
	m_text = text;
}

// 0x46a330
void TextBlock::AppendText(std::wstring const& text) {
	m_text += text;
}

// 0x46a350
void TextBlock::InsertText(uint32_t pos, std::wstring const& text) {
	if (pos < m_text.size())
		m_text.insert(pos, text);
	else
		m_text += text;
}

// 0x46a370
FontInfo TextBlock::GetFont(FontInfo const& font, FontInfo const& linkFont) const {
	FontInfo result;
	if ((m_style & 0x10) == 0 || !m_styleFlag) {
		result = font;
		if ((m_style & 1) != 0)
			result.style |= 1;
		if ((m_style & 2) != 0)
			result.style |= 2;
		if ((m_style & 4) != 0)
			result.color = m_color;
		if ((m_style & 0x20) != 0)
			result.style |= 4;
	} else {
		result = linkFont;
	}
	return result;
}

// 0x46bc90 (LTCG: this in eax)
TextRun::TextRun() : style(0), linkState(2), styleFlag(true) {
}

// 0x46bcd0
void TextRun::Reset() {
	style = 0;
	text.clear();
	hyperLink.clear();
	styleText.clear();
	color = 0xff000000;
	styleFlag = true;
}

// 0x46c5b0
bool TextRun::HasSameStyle(TextRun const& other) const {
	if (style == other.style && color == other.color && hyperLink == other.hyperLink
		&& styleText == other.styleText && linkState == other.linkState
		&& styleFlag == other.styleFlag)
		return true;
	return false;
}

} // namespace TextComponent

// 0x46c620
void RFTextParser::AddStyle(uint32_t const& bit) {
	m_run.style |= bit;
}

// 0x46c630
void RFTextParser::RemoveStyle(uint32_t const& bit) {
	if ((m_run.style & bit) != 0)
		m_run.style ^= bit;
}

// 0x46c640
bool RFTextParser::HasStyle(uint32_t const& bit) const {
	if (m_run.style & bit)
		return true;
	return false;
}

// 0x46c650 (LTCG: this in eax)
RFTextParser::RFTextParser() {
}

// 0x46c750
RFTextParser::~RFTextParser() {
	std::for_each(m_components.begin(), m_components.end(),
		[](RFTextComponent* component) { delete component; });
	m_components.clear();
}

// Splits marked-up text into TextBlocks (m_components), one per run of equal style; "&lt;"/"&gt;"
// are literal brackets. False if source is empty (LTCG: this and source on the stack; name invented).
// 0x46c820
bool RFTextParser::Parse(std::wstring const& source) {
	if (source.length() == 0)
		return false;
	// Escaped brackets are kept as U+3008/U+3009 while the tags are parsed.
	std::wstring text(source);
	ReplaceAll(text, L"&lt;", L"\x3008");
	ReplaceAll(text, L"&gt;", L"\x3009");
	m_run.Reset();
	TextComponent::TextRun run;
	uint32_t pos = 0;
	bool isEndTag = false;
	bool tagClosed = false;
	while (pos < text.length()) {
		if (text[pos] == L'<') {
			if (!ParseTagAt(isEndTag, pos, text)) {
				run.text.append(1, text[pos]);
				++pos;
			} else if (isEndTag) {
				tagClosed = true;
			}
		} else {
			if (run.HasSameStyle(m_run) && !tagClosed) {
				run.text.append(1, text[pos]);
			} else {
				tagClosed = false;
				if (run.text.length() != 0) {
					ReplaceAll(run.text, L"\x3008", L"<");
					ReplaceAll(run.text, L"\x3009", L">");
					m_components.push_back(new TextComponent::TextBlock(run));
				}
				run.Reset();
				run = m_run;
				run.text = text[pos];
			}
			++pos;
		}
	}
	if (run.text.length() != 0) {
		ReplaceAll(run.text, L"\x3008", L"<");
		ReplaceAll(run.text, L"\x3009", L">");
		m_components.push_back(new TextComponent::TextBlock(run));
	}
	return true;
}

// The tag starting at text[pos] ('<'): parsed with ParseTag, pos moved past it; false (pos kept) if
// it is not a whole known tag (LTCG: isEndTag in ecx, pos in edi, this and text on the stack; name invented).
// 0x46d1d0
bool RFTextParser::ParseTagAt(bool& isEndTag, uint32_t& pos, std::wstring const& text) {
	std::wstring tag;
	uint32_t length = ExtractTag(std::wstring(text.c_str() + pos), tag);
	if (length == 0)
		return false;
	if (ParseTag(isEndTag, tag)) {
		pos += length;
		return true;
	}
	return false;
}

// Copies text's first tag ("<...>") into tag and returns its length; 0 if another '<' comes first
// or it is not closed (LTCG: text in ebx; name invented).
// 0x46d300
uint32_t RFTextParser::ExtractTag(std::wstring const& text, std::wstring& tag) {
	uint32_t length = text.length();
	for (uint32_t i = 0; i < length; ++i) {
		tag.append(1, text[i]);
		if (text[i] == L'<' && i != 0)
			break;
		if (text[i] == L'>' && i != 0)
			return i + 1;
	}
	return 0;
}

// 0x46d380
bool RFTextParser::ParseTag(bool& isEndTag, std::wstring const& tag) {
	std::wstring upper(tag);
	std::transform(upper.begin(), upper.end(), upper.begin(), towupper);
	if (upper.at(1) == L'/') {
		isEndTag = true;
		if (upper.at(2) == L'B') {
			RemoveStyle(1);
		} else if (upper.at(2) == L'I') {
			if (upper.at(3) == L'M') {
				RemoveStyle(8);
				m_run.styleText.clear();
			} else {
				RemoveStyle(2);
			}
		} else if (upper.at(2) == L'A') {
			RemoveStyle(0x10);
			m_run.hyperLink.clear();
		} else if (upper.at(2) == L'F') {
			RemoveStyle(4);
			m_run.color = 0xff000000;
		} else if (upper.at(2) == L'U') {
			RemoveStyle(0x20);
		} else {
			return false;
		}
		return true;
	}
	isEndTag = false;
	if (upper.at(1) == L'B') {
		AddStyle(1);
	} else if (upper.at(1) == L'I') {
		if (upper.at(2) == L'M') {
			AddStyle(8);
			ParseImage(tag, &m_run);
		} else {
			AddStyle(2);
		}
	} else if (upper.at(1) == L'A') {
		AddStyle(0x10);
		ParseLink(tag, &m_run);
	} else if (upper.at(1) == L'F') {
		AddStyle(4);
		ParseFont(tag, &m_run);
	} else if (upper.at(1) == L'U') {
		AddStyle(0x20);
	} else {
		return false;
	}
	return true;
}

// Replaces every from in text with to (unreferenced: LTCG inlines it into Parse; name invented).
// 0x46d680
void RFTextParser::ReplaceAll(std::wstring& text, std::wstring const& from, std::wstring const& to) {
	std::wstring::size_type pos;
	while ((pos = text.find(from)) != std::wstring::npos)
		text.replace(pos, from.length(), to);
}
