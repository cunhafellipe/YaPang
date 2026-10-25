// Original: client\Refresh\src\RFParagraph.cpp (JP 983 exe; RFParagraph is
// not exported and no function of it asserts, the file name is assumed from
// the class). Its code lies at 0x465340-0x468c40, apart from RFText's
// (jp/units.tsv).
// lib: 0x465340 (RFParagraph scalar deleting destructor), 0x468850 (std::find_if
// with LineEndCounter), 0x467770 (std::vector<RFTextComponent*>::clear, this in
// ebx), 0x468650 (vector const_iterator constructor, this in eax), 0x468a40
// (reverse_iterator constructor, this in eax), 0x468b20 (empty std helper, ret 4)
#include <new>

#include "RFText.h"

#include <algorithm>

#include "RFInterface.h"
#include "RFManager.h"
#include "RFSize.h"
#include "RFTextComponent.h"
#include "RFTextParser.h"
#include "TextBlock.h"

namespace {

// Whether a piece holds character pos, pos being reduced by the length of
// every piece passed (the std::find_if inlined in RFParagraph::FindComponent).
// The name is ours.
struct PositionFinder {
	explicit PositionFinder(uint32_t& pos) : m_pos(pos) {}
	bool operator()(RFTextComponent* component) const {
		uint32_t length = component->GetLength();
		if (length >= m_pos) {
			return true;
		}
		m_pos -= length;
		return false;
	}
	uint32_t& m_pos;
};

}  // namespace

// 0x465370 (scalar deleting destructor 0x465340)
RFParagraph::~RFParagraph() {
	std::for_each(m_components.begin(), m_components.end(), [](RFTextComponent* component) {
		delete component;
	});
	m_components.clear();
}

// 0x465430
void RFParagraph::TakeTextFrom(RFParagraph* paragraph) {
	if (paragraph == 0) {
		return;
	}
	uint32_t count = paragraph->m_components.size();
	for (uint32_t i = 0; i < count; ++i) {
		if (m_components.empty()) {
			m_components.push_back(paragraph->m_components[i]);
		} else if (m_components.back()->HasSameStyle(paragraph->m_components[i])) {
			TextComponent::TextBlock* last = static_cast<TextComponent::TextBlock*>(m_components.back());
			TextComponent::TextBlock* block = static_cast<TextComponent::TextBlock*>(paragraph->m_components[i]);
			last->AppendText(block->GetText());
			block->SetText(L"");
		} else {
			m_components.push_back(paragraph->m_components[i]);
		}
	}
	paragraph->DetachComponents();
	Rewrap();
}

// 0x465660
void RFParagraph::MoveTextAfterCaretTo(RFParagraph* paragraph) {
	if (paragraph == 0 || m_caretPos == GetLength()) {
		return;
	}
	uint32_t offset;
	RFTextComponent* component = FindComponent(m_caretPos, &offset, true);
	if (component != 0) {
		std::vector<RFTextComponent*> moved;
		if (offset == 0) {
			paragraph->AddComponent(component);
			moved.push_back(component);
		} else if (offset != component->GetLength()) {
			TextComponent::TextBlock* block = static_cast<TextComponent::TextBlock*>(component);
			std::wstring text = block->GetText();
			std::wstring head = text.substr(0, offset);
			std::wstring tail = text.substr(offset, text.size());
			block->SetText(head);
			TextComponent::TextBlock* rest = new TextComponent::TextBlock(block->GetStyle(),
				block->GetStyleText(), block->GetHyperLink(), block->GetColor(), block->GetLinkState(),
				block->GetStyleFlag());
			rest->AppendText(tail);
			paragraph->AddComponent(rest);
		}
		bool found = false;
		std::for_each(m_components.begin(), m_components.end(),
			[&found, &moved, paragraph, component](RFTextComponent* each) {
				if (found) {
					paragraph->AddComponent(each);
					moved.push_back(each);
				}
				if (each == component) {
					found = true;
				}
			});
		std::for_each(moved.begin(), moved.end(), [this](RFTextComponent* each) {
			std::vector<RFTextComponent*>::iterator it = std::find(m_components.begin(), m_components.end(), each);
			if (it != m_components.end()) {
				m_components.erase(it);
			}
		});
		moved.clear();
	}
	Rewrap();
	paragraph->Rewrap();
}

// 0x465a10
void RFParagraph::AddText(std::wstring const& text, bool parseTags) {
	if (!text.empty()) {
		m_layoutFlag = false;
		if (parseTags) {
			InsertParsedText(text);
		} else {
			InsertText(text);
		}
		Rewrap();
	}
}

// 0x465a80
void RFParagraph::AddComponent(RFTextComponent* component) {
	if (component != 0) {
		m_components.push_back(component);
		Rewrap();
	}
}

// 0x465b30
bool RFParagraph::GetHyperLinkAt(int32_t x, int32_t y, int32_t const& linkState, std::wstring* link) {
	std::vector<RFTextComponent*>::iterator it = std::find_if(m_components.begin(), m_components.end(),
		[link, x, y, linkState](RFTextComponent* component) -> bool {
			if (component->GetHyperLinkAt(x, y, &linkState, link)) {
				return true;
			}
			return false;
		});
	return it != m_components.end();
}

// 0x465bd0
void RFParagraph::HideSegments() {
	std::for_each(m_components.begin(), m_components.end(), [](RFTextComponent* component) {
		component->HideSegments();
	});
}

// 0x465c10
void RFParagraph::Draw(float alpha) {
	std::for_each(m_components.begin(), m_components.end(), [alpha, this](RFTextComponent* component) {
		static_cast<TextComponent::TextBlock*>(component)->Draw(alpha, m_context);
	});
}

// 0x465c90
void RFParagraph::ClearSelected() {
	std::for_each(m_components.begin(), m_components.end(), [](RFTextComponent* component) {
		component->ClearSelected();
	});
}

// 0x465cd0
int32_t RFParagraph::Layout(int32_t x, int32_t y, HorzAlignTypes::Type const& hAlign) {
	m_caretX = x;
	m_caretY = y;
	auto applySpacing = [this](uint32_t lineHeight) -> uint32_t {
		return (uint32_t)((float)(m_lineSpacing * lineHeight) / 100.0 + 0.5);
	};
	int32_t height = 0;
	uint32_t line = 0;
	if (!m_components.empty()) {
		for (; line < (uint32_t)m_numOfLine; ++line) {
			int32_t caretLine;
			uint32_t charOffset;
			uint32_t caretX;
			GetCharPos(m_caretPos, &caretLine, &charOffset, &caretX);
			int32_t lineX = x;
			uint32_t lineWidth = GetLineWidth(line);
			if (lineWidth < (uint32_t)m_width) {
				switch (hAlign) {
				case HorzAlignTypes::Center:
					lineX = x + (((uint32_t)m_width - lineWidth) >> 1);
					break;
				case HorzAlignTypes::Right:
					lineX = x + ((uint32_t)m_width - lineWidth);
					break;
				}
			}
			if (line == (uint32_t)caretLine) {
				m_caretX = caretX + lineX;
				m_caretY = height + y;
			}
			int32_t segmentX = 0;
			uint32_t lineHeight = 0;
			std::find_if(m_components.begin(), m_components.end(),
				[&segmentX, &lineHeight, line, lineX, y, height](RFTextComponent* component) -> bool {
					int32_t first;
					int32_t last;
					component->GetSegmentIdRange(&first, &last);
					if (line < (uint32_t)first) {
						return true;
					}
					if (line <= (uint32_t)last) {
						component->SetSegmentPosition(line, segmentX + lineX, height + y);
						segmentX += component->GetSegmentWidth(line);
						uint32_t segmentHeight = component->GetSegmentHeight(line);
						if (segmentHeight <= lineHeight) {
							segmentHeight = lineHeight;
						}
						lineHeight = segmentHeight;
					}
					return false;
				});
			height += applySpacing(lineHeight);
		}
		return height;
	}
	wchar_t const text[] = L"A";
	RFSize size = RFManager::s_interface->MeasureText(*m_font, std::wstring(text));
	uint32_t lineHeight = (uint32_t)(size.height + 0.5);
	height = applySpacing(lineHeight);
	switch (hAlign) {
	case HorzAlignTypes::Center:
		x += (uint32_t)m_width >> 1;
		break;
	case HorzAlignTypes::Right:
		x += m_width;
		break;
	}
	m_caretX = x;
	m_caretY = y;
	return height;
}

// 0x466080
void RFParagraph::EraseAtCaret(uint32_t count) {
	EraseText(m_caretPos, count);
	RemoveEmptyComponents();
	Rewrap();
}

// 0x4660e0
void RFParagraph::EraseText(uint32_t pos, uint32_t count) {
	if (count == 0) {
		return;
	}
	uint32_t length = 0;
	for (std::vector<RFTextComponent*>::iterator it = m_components.begin(), end = m_components.end();
		it != end; ++it) {
		length += (*it)->GetLength();
	}
	if (pos >= length) {
		return;
	}
	std::find_if(m_components.begin(), m_components.end(), [&pos, &count](RFTextComponent* component) -> bool {
		if (pos < component->GetLength()) {
			count = component->Erase(pos, count);
			if (count == 0) {
				return true;
			}
			pos = 0;
		} else {
			pos -= component->GetLength();
		}
		return false;
	});
}

// 0x4661b0
void RFParagraph::SetWidth(int32_t width) {
	m_width = width;
	Rewrap();
}

// 0x466200
uint32_t RFParagraph::GetLength() const {
	uint32_t length = 0;
	std::for_each(m_components.begin(), m_components.end(), [&length](RFTextComponent* component) {
		length += component->GetLength();
	});
	return length;
}

// 0x466250
int32_t RFParagraph::GetNumOfLine() const {
	return m_numOfLine;
}

// 0x466260
uint32_t RFParagraph::GetLineWidth(uint32_t line) {
	uint32_t width = 0;
	if (line < (uint32_t)m_numOfLine) {
		std::find_if(m_components.begin(), m_components.end(), [&width, line](RFTextComponent* component) -> bool {
			int32_t first;
			int32_t last;
			component->GetSegmentIdRange(&first, &last);
			if (line < (uint32_t)first) {
				return true;
			}
			if (line <= (uint32_t)last) {
				width += component->GetSegmentWidth(line);
			}
			return false;
		});
	}
	return width;
}

// 0x4662f0
uint32_t RFParagraph::GetWidth() const {
	uint32_t width = 0;
	std::for_each(m_components.begin(), m_components.end(), [&width](RFTextComponent* component) {
		width += component->GetWidth();
	});
	return width;
}

// 0x466340
uint32_t RFParagraph::GetLineHeight(uint32_t line) {
	uint32_t height = 0;
	if (line < (uint32_t)m_numOfLine) {
		std::find_if(m_components.begin(), m_components.end(), [&height, line](RFTextComponent* component) -> bool {
			int32_t first;
			int32_t last;
			component->GetSegmentIdRange(&first, &last);
			if (line < (uint32_t)first) {
				return true;
			}
			if (line <= (uint32_t)last) {
				uint32_t segmentHeight = component->GetSegmentHeight(line);
				if (segmentHeight <= height) {
					segmentHeight = height;
				}
				height = segmentHeight;
			}
			return false;
		});
	}
	return height;
}

// 0x4663d0
int32_t RFParagraph::GetHeight(bool isLastParagraph) {
	auto applySpacing = [this](uint32_t lineHeight) -> uint32_t {
		return (uint32_t)((float)(m_lineSpacing * lineHeight) / 100.0 + 0.5);
	};
	int32_t height = 0;
	for (uint32_t line = 0; line < (uint32_t)m_numOfLine; ++line) {
		uint32_t lineHeight = 0;
		std::vector<RFTextComponent*>::iterator it =
			std::find_if(m_components.begin(), m_components.end(), [&lineHeight, line](RFTextComponent* component) -> bool {
				int32_t first;
				int32_t last;
				component->GetSegmentIdRange(&first, &last);
				if (line < (uint32_t)first) {
					return true;
				}
				if (line <= (uint32_t)last) {
					uint32_t height = component->GetSegmentHeight(line);
					if (height <= lineHeight) {
						height = lineHeight;
					}
					lineHeight = height;
				}
				return false;
			});
		if (it == m_components.end()) {
			wchar_t const text[] = L"A";
			RFSize size = RFManager::s_interface->MeasureText(*m_font, std::wstring(text));
			lineHeight = (uint32_t)(size.height + 0.5);
		}
		if (!isLastParagraph || line != (uint32_t)m_numOfLine - 1) {
			height += applySpacing(lineHeight);
		} else {
			height += lineHeight;
		}
	}
	return height;
}

// 0x466620
std::wstring RFParagraph::GetText() const {
	std::wstring text;
	std::for_each(m_components.begin(), m_components.end(), [&text](RFTextComponent* component) {
		text += static_cast<TextComponent::TextBlock*>(component)->GetText();
	});
	return text;
}

// 0x4666c0
uint32_t RFParagraph::FindLineBreak() const {
	std::wstring text = GetText();
	std::wstring::size_type pos = text.find(L"\n");
	if (pos != std::wstring::npos) {
		return (uint32_t)pos;
	}
	return (uint32_t)-1;
}

// 0x466750
bool RFParagraph::MoveCaretTo(int32_t x, int32_t y, uint32_t offsetX) {
	int32_t segmentId;
	std::vector<RFTextComponent*>::iterator it = std::find_if(m_components.begin(), m_components.end(),
		[&segmentId, x, y, this](RFTextComponent* component) -> bool {
			if (component->GetSegmentAt(x, y, m_width, &m_layoutFlag, &segmentId)) {
				return true;
			}
			return false;
		});
	if (it != m_components.end()) {
		SetCaret(segmentId, offsetX);
		return true;
	}
	return false;
}

// 0x466830
void RFParagraph::HandleKey(EditKeyTypes::Type const& key, ParagraphKeyResult& result) {
	int32_t segmentId;
	uint32_t charOffset;
	uint32_t x;
	switch (key) {
	case EditKeyTypes::Delete:
		if (m_caretPos < GetLength()) {
			EraseAtCaret(1);
		} else {
			result.m_leavesParagraph = true;
			result.m_move = ParagraphKeyResult::MoveTypes::JoinNext;
		}
		break;
	case EditKeyTypes::Back:
		if (m_caretPos >= 1) {
			--m_caretPos;
			EraseAtCaret(1);
		} else {
			m_caretPos = 0;
			result.m_leavesParagraph = true;
			result.m_move = ParagraphKeyResult::MoveTypes::JoinWithPrev;
		}
		break;
	case EditKeyTypes::Home:
		if (GetCharPos(m_caretPos, &segmentId, &charOffset, &x) != 0) {
			SetCaretLineStart(segmentId);
		}
		m_layoutFlag = false;
		break;
	case EditKeyTypes::End:
		if (GetCharPos(m_caretPos, &segmentId, &charOffset, &x) != 0) {
			SetCaretLineEnd(segmentId);
			m_layoutFlag = true;
		}
		break;
	case EditKeyTypes::Left:
		if (MoveCaretBackward(1) != 0) {
			result.m_leavesParagraph = true;
			result.m_move = ParagraphKeyResult::MoveTypes::ToPrevEnd;
		}
		m_layoutFlag = false;
		break;
	case EditKeyTypes::Right:
		if (MoveCaretForward(1) != 0) {
			result.m_leavesParagraph = true;
			result.m_move = ParagraphKeyResult::MoveTypes::ToNextStart;
		}
		m_layoutFlag = false;
		break;
	case EditKeyTypes::Up:
		if (GetCharPos(m_caretPos, &segmentId, &charOffset, &x) != 0) {
			if ((uint32_t)segmentId > 0) {
				--segmentId;
				SetCaret(segmentId, x);
				break;
			}
			result.m_leavesParagraph = true;
			result.m_move = ParagraphKeyResult::MoveTypes::ToPrevLine;
			result.m_caretX = x;
		} else {
			result.m_leavesParagraph = true;
			result.m_move = ParagraphKeyResult::MoveTypes::ToPrevLine;
			result.m_caretX = 0;
		}
		m_layoutFlag = false;
		break;
	case EditKeyTypes::Down:
		if (GetCharPos(m_caretPos, &segmentId, &charOffset, &x) != 0) {
			if ((uint32_t)segmentId < (uint32_t)(m_numOfLine - 1)) {
				++segmentId;
				SetCaret(segmentId, x);
				break;
			}
			result.m_leavesParagraph = true;
			result.m_move = ParagraphKeyResult::MoveTypes::ToNextLine;
			result.m_caretX = x;
		} else {
			result.m_leavesParagraph = true;
			result.m_move = ParagraphKeyResult::MoveTypes::ToNextLine;
			result.m_caretX = 0;
		}
		m_layoutFlag = false;
		break;
	}
}

// 0x466a50
uint32_t RFParagraph::MoveCaretBackward(uint32_t count) {
	if (m_caretPos >= count) {
		m_caretPos -= count;
		return 0;
	}
	m_caretPos = 0;
	return count;
}

// 0x466a70
uint32_t RFParagraph::MoveCaretForward(uint32_t count) {
	uint32_t length = GetLength();
	uint32_t pos = m_caretPos + count;
	if (pos <= length) {
		m_caretPos = pos;
		return 0;
	}
	m_caretPos = length;
	return count;
}

// 0x466ac0
void RFParagraph::MoveCaretToStart() {
	m_caretPos = 0;
}

// 0x466ad0
void RFParagraph::SetCaretLineStart(int32_t segmentId) {
	uint32_t caret = 0;
	std::find_if(m_components.begin(), m_components.end(), [&caret, segmentId](RFTextComponent* component) -> bool {
		uint32_t first = component->GetSegmentFirstChar(segmentId);
		if (first == (uint32_t)-1) {
			caret += component->GetLength();
			return false;
		}
		caret += first;
		return true;
	});
	m_caretPos = caret;
}

// 0x466b40
void RFParagraph::MoveCaretToEnd() {
	uint32_t length = 0;
	for (std::vector<RFTextComponent*>::iterator it = m_components.begin(), end = m_components.end();
		it != end; ++it) {
		length += (*it)->GetLength();
	}
	m_caretPos = length;
}

// 0x466b80
void RFParagraph::SetCaretLineEnd(int32_t segmentId) {
	uint32_t caret = 0;
	bool found = false;
	std::find_if(m_components.begin(), m_components.end(), [&caret, &found, segmentId](RFTextComponent* component) -> bool {
		uint32_t last = component->GetSegmentLastChar(segmentId);
		if (last == (uint32_t)-1) {
			if (found) {
				return true;
			}
			caret += component->GetLength();
			return false;
		}
		found = true;
		caret += last;
		return false;
	});
	m_caretPos = caret;
}

// 0x466c20
void RFParagraph::SetCaret(int32_t segmentId, uint32_t x) {
	SetCaretLineStart(segmentId);
	std::find_if(m_components.begin(), m_components.end(), [&x, segmentId, this](RFTextComponent* component) -> bool {
		int32_t first;
		int32_t last;
		component->GetSegmentIdRange(&first, &last);
		if ((uint32_t)segmentId < (uint32_t)first) {
			return true;
		}
		if ((uint32_t)segmentId <= (uint32_t)last) {
			uint32_t split = component->SplitSegment(segmentId, &x);
			m_caretPos += split;
			if (x == 0) {
				return true;
			}
		}
		return false;
	});
}

// 0x466ce0
void RFParagraph::SetCaretX(uint32_t x) {
	int32_t segmentId;
	uint32_t charOffset;
	uint32_t caretX;
	if (GetCharPos(m_caretPos, &segmentId, &charOffset, &caretX) == 0) {
		return;
	}
	SetCaret(segmentId, x);
}

// 0x466d90
void RFParagraph::InsertText(std::wstring const& text) {
	if (m_components.empty()) {
		TextComponent::TextBlock* block = new TextComponent::TextBlock;
		block->AppendText(text);
		m_components.push_back(block);
	} else {
		uint32_t pos = m_caretPos;
		std::vector<RFTextComponent*>::iterator it =
			std::find_if(m_components.begin(), m_components.end(), PositionFinder(pos));
		TextComponent::TextBlock* block = 0;
		if (it != m_components.end()) {
			block = static_cast<TextComponent::TextBlock*>(*it);
		}
		if (block != 0) {
			block->InsertText(pos, text);
		} else {
			block = new TextComponent::TextBlock;
			block->AppendText(text);
			m_components.push_back(block);
		}
	}
	uint32_t length = 0;
	for (std::vector<RFTextComponent*>::iterator it = m_components.begin(), end = m_components.end();
		it != end; ++it) {
		length += (*it)->GetLength();
	}
	m_caretPos = std::min((uint32_t)(m_caretPos + text.size()), length);
}

// 0x466f20
void RFParagraph::InsertParsedText(std::wstring const& text) {
	uint32_t added = 0;
	RFTextParser parser;
	if (parser.Parse(text)) {
		std::vector<RFTextComponent*>& parsed = parser.GetComponents();
		uint32_t count = parsed.size();
		for (uint32_t i = 0; i < count; ++i) {
			m_components.push_back(parsed[i]);
			added += parsed[i]->GetLength();
		}
		parsed.clear();
	}
	m_caretPos = std::min(m_caretPos + added, GetLength());
}

// 0x467190
void RFParagraph::Rewrap() {
	uint32_t available = m_width;
	int32_t segmentId = 0;
	std::for_each(m_components.begin(), m_components.end(), [&available, &segmentId, this](RFTextComponent* component) {
		available = component->Layout(*m_font, *m_linkFont, available, m_width, &segmentId);
	});
	m_numOfLine = segmentId + 1;
}

// 0x467230
void RFParagraph::RemoveEmptyComponents() {
	for (;;) {
		std::vector<RFTextComponent*>::iterator it =
			std::find_if(m_components.begin(), m_components.end(), [](RFTextComponent* component) -> bool {
				if (component->IsEmpty()) {
					delete component;
					return true;
				}
				return false;
			});
		if (it == m_components.end()) {
			break;
		}
		m_components.erase(it);
	}
}

// 0x4672b0
void RFParagraph::DetachComponents() {
	RemoveEmptyComponents();
	m_components.clear();
}

// 0x4672f0
RFTextComponent* RFParagraph::GetCharPos(uint32_t pos, int32_t* segmentId, uint32_t* charOffset,
	uint32_t* x) {
	uint32_t offset;
	RFTextComponent* component = FindComponent(pos, &offset, m_layoutFlag);
	if (component != 0) {
		component->GetCharPos(offset, segmentId, charOffset, x, m_layoutFlag);
		uint32_t& caretX = *x;
		int32_t& line = *segmentId;
		std::find_if(m_components.begin(), m_components.end(), [&caretX, line, component](RFTextComponent* each) -> bool {
			int32_t first;
			int32_t last;
			each->GetSegmentIdRange(&first, &last);
			if ((uint32_t)line < (uint32_t)first) {
				return true;
			}
			if ((uint32_t)line <= (uint32_t)last) {
				if (component == each) {
					return true;
				}
				caretX += each->GetSegmentWidth(line);
			}
			return false;
		});
	}
	return component;
}

// 0x4673e0
RFTextComponent* RFParagraph::FindComponent(uint32_t pos, uint32_t* offset, bool fromFront) {
	if (fromFront) {
		return FindComponentFromFront(pos, offset);
	}
	return FindComponentFromBack(pos, offset);
}

// 0x467460
RFTextComponent* RFParagraph::FindComponentFromFront(uint32_t pos, uint32_t* offset) {
	*offset = 0;
	std::vector<RFTextComponent*>::iterator it =
		std::find_if(m_components.begin(), m_components.end(), [&pos](RFTextComponent* component) -> bool {
			uint32_t length = component->GetLength();
			if (length >= pos) {
				return true;
			}
			pos -= length;
			return false;
		});
	if (it != m_components.end()) {
		*offset = pos;
		return *it;
	}
	return 0;
}

// 0x467500
RFTextComponent* RFParagraph::FindComponentFromBack(uint32_t pos, uint32_t* offset) {
	*offset = 0;
	uint32_t total = 0;
	for (std::vector<RFTextComponent*>::iterator it = m_components.begin(), end = m_components.end();
		it != end; ++it) {
		total += (*it)->GetLength();
	}
	std::vector<RFTextComponent*>::reverse_iterator it = std::find_if(
		m_components.rbegin(), m_components.rend(), [&pos, &total](RFTextComponent* component) -> bool {
			total -= component->GetLength();
			if (total <= pos) {
				return true;
			}
			return false;
		});
	if (it != m_components.rend()) {
		*offset = pos - total;
		return *it;
	}
	return 0;
}
