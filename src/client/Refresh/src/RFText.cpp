// Original: client\Refresh\src\RFText.cpp (JP 983 exe; RFText is not
// exported and no function of it asserts, the file name is assumed from the
// class). Its code lies at 0x456130-0x45a480 (jp/units.tsv).
#include <new>

#include "RFText.h"

#include <algorithm>

#include "RFTextComponent.h"
#include "TextBlock.h"

// lib: 0x458250 (std::deque<RFParagraph*>::clear), 0x459270 (std::for_each
// with ParagraphDeleter), 0x4593e0 (std::for_each with SelectionClearer),
// 0x459890 (std::for_each with LineCounter), 0x457be0/0x457c00/0x457c20/0x457c40
// (deque begin/end), 0x457c60 (deque end const), 0x4592c0 (std::for_each with
// SegmentHider), 0x458890/0x459320 (std::for_each with ParagraphDrawer),
// 0x458ac0 (std::operator+(std::wstring&&, wchar_t const*)), 0x458b00/0x459630
// (std::for_each with ParagraphTextCollector), 0x458b90/0x4596c0 (std::for_each
// with TextLengthCounter), 0x458d30/0x4598f0 (std::find_if with
// ParagraphCaretHitTest), 0x4589c0/0x4594b0 (std::for_each with
// ParagraphRewrap), 0x458a20/0x459550 (std::find_if with HyperLinkHitTest),
// 0x458950/0x459440 (std::for_each with ParagraphLayout), 0x4563c0 (RFText
// scalar deleting destructor), 0x456140/0x456150 (_Iterator_base12 default
// and copy constructors, unreferenced), 0x4571b0 (an empty std member, ret)

namespace {

// Deletes a paragraph (std::for_each 0x459270 in RFText::DeleteParagraphs).
// Empty, passed by value. The name is ours.
struct ParagraphDeleter {
	// 0x456540 (unreferenced copy; LTCG passes paragraph in ecx)
	void operator()(RFParagraph* paragraph) const {
		delete paragraph;
	}
};

// Clears the selection of every piece of a paragraph (RFTextComponent slot
// 7; std::for_each 0x4593e0 in RFText::ResetHover). The name is ours.
struct SelectionClearer {
	// 0x456d50
	void operator()(RFParagraph* paragraph) const {
		paragraph->ClearSelected();
	}
};

// Adds a paragraph's line count to the caller's counter (std::for_each
// 0x459890 in RFText::GetNumOfLine). The name is ours.
struct LineCounter {
	explicit LineCounter(int32_t& count) : m_count(count) {}
	// 0x457540
	void operator()(RFParagraph* paragraph) const {
		m_count += paragraph->GetNumOfLine();
	}
	int32_t& m_count;
};

// Hides the segments of every piece of a paragraph (RFTextComponent slot 5;
// std::for_each 0x4592c0 in RFText::Draw). The name is ours.
struct SegmentHider {
	// 0x456c60
	void operator()(RFParagraph* paragraph) const {
		paragraph->HideSegments();
	}
};

// Draws every piece of a paragraph into the paragraph's text area
// (std::for_each 0x458890/0x459320 in RFText::Draw, TextBlock::Draw inlined).
// The name is ours.
struct ParagraphDrawer {
	explicit ParagraphDrawer(float const& alpha) : m_alpha(alpha) {}
	// 0x456ca0
	void operator()(RFParagraph* paragraph) const {
		paragraph->Draw(m_alpha);
	}
	float m_alpha;
};

// Whether a paragraph is the one searched, counting the paragraphs passed
// (the std::find_if of RFText::GetParagraphIndex). The name is ours.
struct ParagraphIndexFinder {
	ParagraphIndexFinder(uint32_t& index, RFParagraph* const& paragraph) : m_index(index), m_paragraph(paragraph) {}
	// 0x457b10
	bool operator()(RFParagraph* paragraph) const {
		if (paragraph == m_paragraph) {
			return true;
		}
		++m_index;
		return false;
	}
	uint32_t& m_index;
	RFParagraph* m_paragraph;
};

// Adds the length of a paragraph's pieces to the caller's counter
// (std::for_each 0x458b90/0x4596c0 in RFText::InsertText). The name is ours.
struct TextLengthCounter {
	explicit TextLengthCounter(uint32_t& count) : m_count(count) {}
	// 0x4572d0
	void operator()(RFParagraph* paragraph) const {
		m_count += paragraph->GetLength();
	}
	uint32_t& m_count;
};

}  // namespace

// Appends each paragraph's text, a line break after every paragraph but the
// last (std::for_each 0x458b00/0x459630 in RFText::GetText). The name is ours.
struct ParagraphTextCollector {
	ParagraphTextCollector(std::wstring& text, std::deque<RFParagraph*>::const_iterator const& end)
		: m_text(text), m_end(end) {}
	// 0x457090
	void operator()(RFParagraph* paragraph) const {
		std::deque<RFParagraph*>::const_iterator last = m_end;
		--last;
		if (*last == paragraph) {
			m_text += paragraph->GetText();
		} else {
			m_text += paragraph->GetText() + L"\n";
		}
	}
	std::wstring& m_text;
	std::deque<RFParagraph*>::const_iterator m_end;
};

// Gives a paragraph the text's new width and lays it out again
// (std::for_each 0x4589c0/0x4594b0 in RFText::SetWidth, which inline
// operator(); its own copy gets this in eax, paragraph in esi). 4 bytes,
// passed by value. The name is ours.
struct ParagraphRewrap {
	explicit ParagraphRewrap(int32_t const& width_) : width(width_) {}
	// 0x456e60
	void operator()(RFParagraph* paragraph) const {
		paragraph->SetWidth(width);
	}
	int32_t width;  // 0x0
};

// Lays a paragraph out at (x, y) with the text's alignment and moves y down
// by its height (std::for_each 0x458950/0x459440 in RFText::Layout; its own
// constructor 0x456dd0 and operator() 0x456df0 get LTCG register arguments).
// 0xc bytes, passed by value. RFTextArea.cpp holds the same definition for
// RFTextArea::Invaildate, which inlines RFText::Layout. The name is ours.
struct ParagraphLayout {
	ParagraphLayout(int32_t& y_, int32_t const& x_, RFText* text_) : y(y_), x(x_), text(text_) {}

	// 0x456df0
	void operator()(RFParagraph* paragraph) const {
		y += paragraph->Layout(x, y, text->m_hAlign);
	}

	int32_t& y;     // 0x0
	int32_t x;      // 0x4
	RFText* text;   // 0x8
};

// Whether a paragraph has a hyperlink piece under the point, its text
// stored in *link (RFTextComponent slot 4; std::find_if 0x458a20/0x459550
// in RFText::HitTestHyperLink, which inline operator()). 0x10 bytes, passed
// by value. The name is ours.
struct HyperLinkHitTest {
	HyperLinkHitTest(std::wstring* link_, int32_t const& x_, int32_t const& y_, int32_t const& kind_)
		: link(link_), x(x_), y(y_), kind(kind_) {}
	// 0x456f40
	bool operator()(RFParagraph* paragraph) const {
		return paragraph->GetHyperLinkAt(x, y, kind, link);
	}
	std::wstring* link;  // 0x0
	int32_t x;           // 0x4
	int32_t y;           // 0x8
	// 2 for a left button press, 0 / 1 for a right button press / release,
	// 3 for a hover (RFTextArea).
	int32_t kind;        // 0xc
};

// The widest paragraph (std::for_each 0x458c60, its loop 0x459800: library
// instantiations; the unreferenced copy of operator() 0x457450 inlines
// RFParagraph::GetWidth): the caller's maximum takes the larger.
// RFTextArea.cpp holds the same definition for RFTextArea::FitToContent,
// which inlines RFText::GetMaxWidth. 4 bytes, passed by value. The name is ours.
struct ParagraphWidthMax {
	explicit ParagraphWidthMax(uint32_t* max) : m_max(max) {}

	// 0x457450
	void operator()(RFParagraph* paragraph) {
		uint32_t width = paragraph->GetWidth();
		if (*m_max > width) {
			width = *m_max;
		}
		*m_max = width;
	}

	uint32_t* m_max;  // 0x0
};

// Makes the paragraph hit at the point current, its caret at the hit
// character (std::find_if 0x458d30/0x4598f0 in RFText::MoveCaretTo). The name
// is ours.
struct ParagraphCaretHitTest {
	ParagraphCaretHitTest(int32_t const& x, int32_t const& y, int32_t const& offsetX, RFText* text)
		: m_x(x), m_y(y), m_offsetX(offsetX), m_text(text) {}
	// 0x4575f0
	bool operator()(RFParagraph* paragraph) const {
		if (!paragraph->MoveCaretTo(m_x, m_y, m_offsetX)) {
			return false;
		}
		m_text->SetCurrentParagraph(paragraph);
		return true;
	}
	int32_t m_x;
	int32_t m_y;
	int32_t m_offsetX;
	RFText* m_text;
};

// 0x456260
RFText::RFText(int32_t width, int32_t height, HorzAlignTypes::Type const& hAlign,
	VertAlignTypes::Type const& vAlign, FontInfo* font, FontInfo* linkFont, RFTextArea* context)
	: m_context(context), m_font(font), m_linkFont(linkFont), m_width(width), m_height(height),
	  m_lineSpacing(130), m_lineLimit(200), m_textLimit(500), m_reserved(0), m_hAlign(hAlign),
	  m_vAlign(vAlign), m_composing(false), m_compositionLength(0) {
	SetCurrentParagraph(InsertParagraph(
		new RFParagraph(m_width, m_lineSpacing, m_font, m_linkFont, m_context), 0));
}

// 0x456440 (scalar deleting destructor 0x4563c0)
RFText::~RFText() {
	DeleteParagraphs();
}

// 0x4564b0
void RFText::DeleteParagraphs() {
	m_currentParagraph = 0;
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(), ParagraphDeleter());
	m_paragraphs.clear();
}

// 0x456570
void RFText::Clear() {
	DeleteParagraphs();
	SetCurrentParagraph(InsertParagraph(
		new RFParagraph(m_width, m_lineSpacing, m_font, m_linkFont, m_context), 0));
}

// 0x456600
void RFText::AddText(wchar_t const* text, bool parseTags) {
	m_currentParagraph->AddText(text, parseTags);
	for (uint32_t pos = m_currentParagraph->FindLineBreak(); pos != (uint32_t)-1;
		pos = m_currentParagraph->FindLineBreak()) {
		m_currentParagraph->m_caretPos = pos;
		m_currentParagraph->EraseAtCaret(1);
		RFParagraph* next = new RFParagraph(m_width, m_lineSpacing, m_font, m_linkFont, m_context);
		m_currentParagraph->MoveTextAfterCaretTo(next);
		SetCurrentParagraph(InsertParagraph(next, GetParagraphIndex(m_currentParagraph) + 1));
	}
	if (m_lineLimit < (uint32_t)GetNumOfLine()) {
		RemoveParagraph(0);
	}
}

// 0x456ab0
void RFText::InsertText(wchar_t const* text, bool composing) {
	if (m_composing) {
		m_currentParagraph->MoveCaretBackward(m_compositionLength);
		m_currentParagraph->EraseAtCaret(m_compositionLength);
	}
	if (composing) {
		m_compositionLength = wcslen(text);
		m_composing = m_compositionLength != 0;
	} else {
		m_composing = false;
		m_compositionLength = 0;
	}
	if (wcslen(text) + GetLength() > m_textLimit) {
		return;
	}
	AddText(text, false);
}

// 0x456bc0
void RFText::Draw(float alpha) {
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(), SegmentHider());
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(), ParagraphDrawer(alpha));
}

// 0x456cd0
void RFText::ResetHover() {
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(), SelectionClearer());
}

// 0x456d80
void RFText::Layout(int32_t x, int32_t y) {
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(), ParagraphLayout(y, x, this));
}

// 0x456e10
void RFText::SetWidth(int32_t width) {
	m_width = width;
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(), ParagraphRewrap(width));
}

// 0x456eb0
bool RFText::HitTestHyperLink(std::wstring* link, int32_t x, int32_t y, int32_t const& kind) {
	std::deque<RFParagraph*>::iterator it = std::find_if(m_paragraphs.begin(), m_paragraphs.end(),
		HyperLinkHitTest(link, x, y, kind));
	return it != m_paragraphs.end();
}

// 0x456fa0
std::wstring RFText::GetText() const {
	std::wstring text;
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(),
		ParagraphTextCollector(text, m_paragraphs.end()));
	return text;
}

// 0x457270
uint32_t RFText::GetLength() const {
	uint32_t length = 0;
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(), TextLengthCounter(length));
	return length;
}

// 0x457310
uint32_t RFText::GetTotalHeight() const {
	uint32_t total = 0;
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(), ParagraphHeightSum(&total, this));
	return total;
}

// 0x4573f0
uint32_t RFText::GetMaxWidth() const {
	uint32_t width = 0;
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(), ParagraphWidthMax(&width));
	return width;
}

// 0x4574a0
int32_t RFText::GetNumOfLine() const {
	int32_t count = 0;
	std::for_each(m_paragraphs.begin(), m_paragraphs.end(), LineCounter(count));
	return count;
}

// 0x457550
bool RFText::MoveCaretTo(int32_t x, int32_t y, int32_t offsetX) {
	std::deque<RFParagraph*>::iterator it = std::find_if(m_paragraphs.begin(), m_paragraphs.end(),
		ParagraphCaretHitTest(x, y, offsetX, this));
	return it != m_paragraphs.end();
}

// 0x457620
void RFText::HandleKey(EditKeyTypes::Type const& key) {
	ParagraphKeyResult result;
	m_currentParagraph->HandleKey(key, result);
	if (!result.m_leavesParagraph) {
		return;
	}
	switch (result.m_move) {
	case ParagraphKeyResult::MoveTypes::ToPrevEnd: {
		RFParagraph* prev = GetPrevParagraph(m_currentParagraph);
		if (prev != 0) {
			m_currentParagraph = prev;
			prev->MoveCaretToEnd();
		}
		break;
	}
	case ParagraphKeyResult::MoveTypes::ToNextStart: {
		RFParagraph* next = GetNextParagraph(m_currentParagraph);
		if (next != 0) {
			m_currentParagraph = next;
			next->MoveCaretToStart();
		}
		break;
	}
	case ParagraphKeyResult::MoveTypes::ToPrevLine: {
		RFParagraph* prev = GetPrevParagraph(m_currentParagraph);
		if (prev != 0) {
			m_currentParagraph = prev;
			prev->MoveCaretToEnd();
			prev->SetCaretX(result.m_caretX);
		}
		break;
	}
	case ParagraphKeyResult::MoveTypes::ToNextLine: {
		RFParagraph* next = GetNextParagraph(m_currentParagraph);
		if (next != 0) {
			m_currentParagraph = next;
			next->MoveCaretToStart();
			next->SetCaretX(result.m_caretX);
		}
		break;
	}
	case ParagraphKeyResult::MoveTypes::JoinWithPrev: {
		RFParagraph* prev = GetPrevParagraph(m_currentParagraph);
		if (prev != 0) {
			RFParagraph* paragraph = m_currentParagraph;
			prev->MoveCaretToEnd();
			prev->TakeTextFrom(paragraph);
			m_currentParagraph = prev;
			RemoveParagraph(GetParagraphIndex(paragraph));
		}
		break;
	}
	case ParagraphKeyResult::MoveTypes::JoinNext: {
		RFParagraph* next = GetNextParagraph(m_currentParagraph);
		if (next != 0) {
			m_currentParagraph->TakeTextFrom(next);
			RemoveParagraph(GetParagraphIndex(next));
		}
		break;
	}
	default:
		break;
	}
}

// 0x4577b0
void RFText::RemoveParagraph(uint32_t index) {
	if (index >= m_paragraphs.size()) {
		return;
	}
	std::deque<RFParagraph*>::iterator it = m_paragraphs.begin() + index;
	if (*it != 0) {
		delete *it;
		*it = 0;
	}
	m_paragraphs.erase(it);
}

// 0x457890
RFParagraph* RFText::InsertParagraph(RFParagraph* paragraph, uint32_t index) {
	if (index >= m_paragraphs.size()) {
		m_paragraphs.push_back(paragraph);
		return paragraph;
	}
	m_paragraphs.insert(m_paragraphs.begin() + index, paragraph);
	return paragraph;
}

// 0x457910
RFParagraph* RFText::GetPrevParagraph(RFParagraph* paragraph) {
	uint32_t index = GetParagraphIndex(paragraph);
	if (index != (uint32_t)-1) {
		uint32_t prev = index - 1;
		if (prev != (uint32_t)-1) {
			return *(m_paragraphs.begin() + prev);
		}
	}
	return 0;
}

// 0x4579d0
RFParagraph* RFText::GetNextParagraph(RFParagraph* paragraph) {
	uint32_t index = GetParagraphIndex(paragraph);
	if (index != (uint32_t)-1) {
		uint32_t next = index + 1;
		if (next < m_paragraphs.size()) {
			return *(m_paragraphs.begin() + next);
		}
	}
	return 0;
}

// 0x457a90
uint32_t RFText::GetParagraphIndex(RFParagraph* paragraph) {
	if (paragraph != 0) {
		uint32_t index = 0;
		std::deque<RFParagraph*>::iterator it = std::find_if(m_paragraphs.begin(), m_paragraphs.end(),
			ParagraphIndexFinder(index, paragraph));
		if (it != m_paragraphs.end()) {
			return index;
		}
	}
	return (uint32_t)-1;
}
