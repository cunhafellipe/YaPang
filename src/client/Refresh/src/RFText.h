// Original: client\Refresh\src\RFText.h (JP 983 exe; RFText is not
// exported, the file name is assumed from the class)
#ifndef PG_CLIENT_REFRESH_SRC_RFTEXT_H
#define PG_CLIENT_REFRESH_SRC_RFTEXT_H

#include <stdint.h>

#include <deque>
#include <vector>

#include "RFLabel.h"
#include "defines.h"
#include "padding.h"

struct FontInfo;
class RFTextArea;
class RFTextComponent;

// The editing keys RFText::HandleKey takes (RFTextArea::OnKeyDown's key map,
// R249); 6 and 7 are not seen yet. The names are ours.
struct EditKeyTypes {
	enum Type {
		Up = 0,
		Down = 1,
		Left = 2,
		Right = 3,
		Home = 4,
		End = 5,
		Delete = 8,
		Back = 9
	};
};

// What RFParagraph::HandleKey (0x466830) leaves to RFText::HandleKey: a key
// that moves the caret out of the paragraph or joins it with a neighbour.
// RFText::HandleKey builds it zeroed. The names are ours.
struct ParagraphKeyResult {
	struct MoveTypes {
		enum Type {
			None = 0,
			ToPrevEnd = 1,        // Left at the start: the previous paragraph's end
			ToNextStart = 2,      // Right at the end: the next paragraph's start
			ToPrevLine = 3,       // Up on the first line: the previous paragraph's last line, at m_caretX
			ToNextLine = 4,       // Down on the last line: the next paragraph's first line, at m_caretX
			JoinWithPrev = 5,     // Back at the start: the paragraph joins the previous one
			JoinNext = 6          // Delete at the end: the next paragraph joins this one
		};
	};

	ParagraphKeyResult() : m_leavesParagraph(false), m_move(MoveTypes::None), m_caretX(0) {} // 0x456230

	bool m_leavesParagraph;                 // 0x0
	MoveTypes::Type m_move;                 // 0x4
	uint32_t m_caretX;                      // 0x8 (ToPrevLine, ToNextLine)
};
CHECK_SIZE(ParagraphKeyResult, 0xc);

// One paragraph of the laid-out text (RTTI RFParagraph, vtable 0xd668c4 with
// one slot, size 0x3c; its code is the unit client/Refresh/src/RFParagraph).
// The RFText constructor builds the first one and adds it with InsertParagraph.
class RFParagraph {
	LAYOUT_CHECK_FRIEND;

public:
	// Inlined by the RFText constructor (0x456260) and Clear (0x456570): the
	// owner's width, line spacing, fonts and context, one line, no pieces.
	RFParagraph(int32_t width, int32_t lineSpacing, FontInfo* font, FontInfo* linkFont, RFTextArea* context)
		: m_width(width), m_lineSpacing(lineSpacing), m_numOfLine(1), m_caretPos(0), m_layoutFlag(false),
		  m_caretX(0), m_caretY(0), m_font(font), m_linkFont(linkFont), m_context(context) {}
	virtual ~RFParagraph();                 // [0] scalar deleting destructor 0x465340, ~RFParagraph 0x465370

	// Moves the text past the caret (m_caretPos) into paragraph, the
	// start of the next line (0x465660). RFText::AddText splits a paragraph
	// at each line break with it.
	void MoveTextAfterCaretTo(RFParagraph* paragraph);
	// Adds text at the caret, parsing its markup when parseTags is set, and
	// re-wraps; nothing happens for an empty text (0x465a10; no caller left:
	// RFText::AddText inlines it). The name is ours.
	void AddText(std::wstring const& text, bool parseTags);
	// Appends every piece of paragraph to this one, merging a piece into the
	// last one when their styles match, then empties paragraph and re-wraps
	// (0x465430; LTCG passes paragraph in ecx, which it null-checks, and this
	// in edx). RFText::HandleKey joins two paragraphs with it. The name is ours.
	void TakeTextFrom(RFParagraph* paragraph);
	void AddComponent(RFTextComponent* component); // 0x465a80
	// Whether a piece has a hyperlink under the point (x, y), its text stored
	// in *link (0x465b30; LTCG passes this in ebx, linkState in eax, link in
	// ecx). RFText::HitTestHyperLink. The name is ours.
	bool GetHyperLinkAt(int32_t x, int32_t y, int32_t const& linkState, std::wstring* link);
	// Hides the segments of every piece (0x465bd0). RFText::Draw.
	void HideSegments();
	// Draws every piece into the paragraph's text area (0x465c10). RFText::Draw.
	void Draw(float alpha);
	// Clears the selection of every piece (0x465c90). RFText::ResetHover.
	void ClearSelected();
	void EraseAtCaret(uint32_t count); // 0x466080
	// Sets the width (+0x14) and re-wraps the pieces in it (0x4661b0; no
	// caller left: RFText::SetWidth's ParagraphRewrap inlines it). The name
	// is ours.
	void SetWidth(int32_t width);
	uint32_t GetLength() const; // 0x466200
	// The number of laid-out lines (0x466250). RFText::GetNumOfLine.
	int32_t GetNumOfLine() const;
	// The sum of the pieces' widths (0x4662f0). The name is ours.
	uint32_t GetWidth() const;
	// The height of laid-out line line, 0 past the last line (0x466340). The
	// name is ours.
	uint32_t GetLineHeight(uint32_t line);
	// Puts the caret on the first character (0x466ac0). The name is ours.
	void MoveCaretToStart();
	// Deletes the empty pieces and forgets the others without deleting them
	// (0x4672b0); TakeTextFrom calls it on the paragraph it empties. The name
	// is ours.
	void DetachComponents();
	// Moves the caret count characters back; returns the count left over
	// past the paragraph's start (0x466a50; no caller left: RFText::InsertText
	// and HandleKey inline it). The name is ours.
	uint32_t MoveCaretBackward(uint32_t count);
	uint32_t MoveCaretForward(uint32_t count); // 0x466a70
	void SetCaretLineEnd(int32_t segmentId); // 0x466b80
	// The paragraph's height in pixels (0x4663d0; LTCG passes this in edi):
	// the sum of its lines' heights (+0x1c lines; a line without its own
	// height uses the height of "A" in the paragraph's font), each scaled by
	// the line spacing (+0x18, percent) except the last line of the last
	// paragraph. RFTextAreaSC sums it over the paragraphs.
	int32_t GetHeight(bool isLastParagraph);
	// Lays the paragraph out at (x, y) with the given alignment and returns
	// its height (0x465cd0; RFTextArea::Invaildate's pass).
	int32_t Layout(int32_t x, int32_t y, HorzAlignTypes::Type const& hAlign);
	// The text of every piece, by value (0x466620; LTCG passes this on the
	// stack). RFText::GetText.
	std::wstring GetText() const;
	// Removes count characters at pos from the pieces (0x4660e0; nothing happens past the paragraph's length).
	// RFText::InsertText removes the shown composition string with it.
	void EraseText(uint32_t pos, uint32_t count);
	// The index of the first line break of the paragraph's text, -1 when
	// there is none (0x4666c0). RFText::AddText.
	uint32_t FindLineBreak() const;
	// Puts the caret on the character of the segment hit at the screen point
	// (x, y), offsetX pixels into the segment; returns whether a segment
	// was hit (0x466750; RFText::MoveCaretTo's predicate inlines it).
	bool MoveCaretTo(int32_t x, int32_t y, uint32_t offsetX);
	// Puts the caret on the first character of segment segmentId (0x466ad0).
	void SetCaretLineStart(int32_t segmentId);
	// Puts the caret on the character of segment segmentId x pixels into it
	// (0x466c20).
	void SetCaret(int32_t segmentId, uint32_t x);
	// Moves the caret x pixels into its own line (0x466ce0; RFText::HandleKey).
	void SetCaretX(uint32_t x);
	// Puts the caret past the last character (0x466b40: m_caretPos = the
	// pieces' total length).
	void MoveCaretToEnd();
	// Moves the caret or erases a character for an editing key inside the
	// paragraph (0x466830; LTCG passes this in ecx, key in eax, result in
	// ebx); a key that leaves the paragraph is set in result for
	// RFText::HandleKey.
	void HandleKey(EditKeyTypes::Type const& key, ParagraphKeyResult& result);
	// The piece holding character pos, with the character's segment, offset
	// in it and pixel x from the line's start (0x4672f0); 0 past the end.
	RFTextComponent* GetCharPos(uint32_t pos, int32_t* segmentId, uint32_t* charOffset, uint32_t* x);
	// The width of laid-out line line, 0 past the last line (0x466260).
	uint32_t GetLineWidth(uint32_t line);
	// Adds text at the caret without parsing it (0x466d90).
	void InsertText(std::wstring const& text);
	// Adds text at the caret, parsing its markup (0x466f20; it calls the
	// TextBlock unit's RFTextParser).
	void InsertParsedText(std::wstring const& text);
	// Lays every piece out again in the paragraph's width and counts the
	// lines (0x467190; LTCG inlined it into RFText's callers).
	void Rewrap();
	// Deletes and removes every empty piece (0x467230; LTCG passes this in ebx).
	void RemoveEmptyComponents();
	// The piece holding character pos, *offset set to pos's offset in it, 0
	// past the end; searched from the first piece, or from the last when
	// fromFront is clear (0x4673e0; LTCG passes pos in ebx).
	RFTextComponent* FindComponent(uint32_t pos, uint32_t* offset, bool fromFront);
	// FindComponent's search from the first piece (0x467460; no caller left,
	// LTCG inlined it into FindComponent). The name is ours.
	RFTextComponent* FindComponentFromFront(uint32_t pos, uint32_t* offset);
	// FindComponent's search from the last piece (0x467500).
	RFTextComponent* FindComponentFromBack(uint32_t pos, uint32_t* offset);

	// The pieces of the paragraph (the destructor deletes them).
	std::vector<RFTextComponent*> m_components;     // 0x04
	int32_t m_width;                        // 0x14 (the owner's width; the re-wrap pass sets it)
	int32_t m_lineSpacing;                  // 0x18 (percent, the owner's)
	int32_t m_numOfLine;                    // 0x1c (1 after construction; AddText counts the lines)
	// The caret's character index in the paragraph: SetCaretLineStart and
	// SetCaret store it, InsertText advances it, Layout finds the caret's
	// line from it, AddText sets it to the line break before EraseText.
	uint32_t m_caretPos;                    // 0x20
	bool m_layoutFlag;                      // 0x24 (AddText clears it; MoveCaretTo: the hit lies past the segment)
	int32_t m_caretX;                       // 0x28 (RFTextArea::DrawCaret)
	int32_t m_caretY;                       // 0x2c (RFTextArea::DrawCaret, RFTextAreaSC::OnKeyDown)
	FontInfo* m_font;                       // 0x30 (the owner's)
	FontInfo* m_linkFont;                   // 0x34 (the owner's)
	RFTextArea* m_context;                  // 0x38 (the owner's; RFText::Draw draws the pieces into it)
};
CHECK_SIZE(RFParagraph, 0x3c);
BEGIN_LAYOUT_CHECK(RFParagraph)
	CHECK_FIELD(RFParagraph, m_components, 0x04);
	CHECK_FIELD(RFParagraph, m_width, 0x14);
	CHECK_FIELD(RFParagraph, m_lineSpacing, 0x18);
	CHECK_FIELD(RFParagraph, m_numOfLine, 0x1c);
	CHECK_FIELD(RFParagraph, m_caretPos, 0x20);
	CHECK_FIELD(RFParagraph, m_layoutFlag, 0x24);
	CHECK_FIELD(RFParagraph, m_caretX, 0x28);
	CHECK_FIELD(RFParagraph, m_caretY, 0x2c);
	CHECK_FIELD(RFParagraph, m_font, 0x30);
	CHECK_FIELD(RFParagraph, m_linkFont, 0x34);
	CHECK_FIELD(RFParagraph, m_context, 0x38);
END_LAYOUT_CHECK

// The laid-out text of an RFTextArea (vtable 0xd65d98, size 0x54; its code
// is the unit client/Refresh/src/RFText).
class RFText {
	LAYOUT_CHECK_FRIEND;

public:
	// 0x456260: the first paragraph is built and made current.
	RFText(int32_t width, int32_t height, HorzAlignTypes::Type const& hAlign,
		VertAlignTypes::Type const& vAlign, FontInfo* font, FontInfo* linkFont, RFTextArea* context);
	// The only virtual: the vtable 0xd65d98 has one slot (the next dwords,
	// 0x00460052 ..., are the UTF-16 string L"RFThumbButton").
	virtual ~RFText();                      // [0] scalar deleting destructor 0x4563c0, ~RFText 0x456440

	// Deletes every paragraph and empties the deque, no paragraph current
	// (0x4564b0; the destructor and Clear).
	void DeleteParagraphs();

	// Appends text, parsing its markup when parseTags is set (0x456600;
	// LTCG passes the text in ecx and this in edx).
	void AddText(wchar_t const* text, bool parseTags);
	// Clears the hover state of every piece of every paragraph (0x456cd0:
	// std::for_each calling each piece's slot 7; LTCG passes this in eax).
	// RFTextArea::OnMouseMove calls it before its hit test.
	void ResetHover();
	// Removes every line (0x456570; LTCG passes this in esi).
	void Clear();
	// The number of laid-out lines (0x4574a0; LTCG passes this in eax).
	int32_t GetNumOfLine() const;
	// Takes a new width and lays every paragraph out again in it (0x456e10;
	// LTCG passes this in ecx, width in eax, and inlines it into
	// RFTextArea::OnLayoutChanged). The name is ours.
	void SetWidth(int32_t width);
	// Whether a hyperlink piece lies under the point, its text stored in
	// *link; kind: 2 left press, 0 / 1 right press / release, 3 hover
	// (0x456eb0; LTCG passes this on the stack, link in edx, kind in eax,
	// and inlines it into RFTextArea's HitTestHyperLink and OnMouseMove).
	// The name is ours.
	bool HitTestHyperLink(std::wstring* link, int32_t x, int32_t y, int32_t const& kind);
	// The text of every line, by value (0x456fa0; LTCG passes this in edx).
	std::wstring GetText() const;
	// The number of characters of every paragraph (0x457270; InsertText
	// inlines the same count). The name is ours.
	uint32_t GetLength() const;
	// The width of the widest paragraph (0x4573f0; RFTextArea::FitToContent
	// inlines it). The name is ours.
	uint32_t GetMaxWidth() const;
	// The height of every paragraph, the last one's last line not scaled by
	// the line spacing (0x457310; LTCG passes this in eax; it calls the
	// const begin()/end() 0x457c00/0x457c40; RFTextArea and RFTextAreaSC
	// inline it). The name is ours.
	uint32_t GetTotalHeight() const;
	// Inserts typed text at the caret, replacing the shown composition
	// string; composing marks the new text as one (0x456ab0; LTCG passes
	// this on the stack). RFTextArea::OnChar.
	void InsertText(wchar_t const* text, bool composing);
	// Draws every paragraph with the given alpha (0x456bc0; LTCG passes
	// this in eax). RFTextArea::OnDraw.
	void Draw(float alpha);
	// Lays every paragraph out from the screen point (x, y), each one below
	// the previous, with the text's alignment (0x456d80; RFTextArea::Invaildate
	// inlines it). The name is ours.
	void Layout(int32_t x, int32_t y);
	// Moves the caret to the screen point (x, y), offsetX being x from the
	// text's left edge (0x457550; LTCG passes this in ecx, x in edx): a
	// std::find_if (0x458d30) over the paragraphs asks each paragraph's
	// pieces (slot 3) for the point and makes the hit paragraph current.
	// Returns whether a paragraph was hit (the text area ignores it).
	bool MoveCaretTo(int32_t x, int32_t y, int32_t offsetX);
	// Moves the caret or erases a character for an editing key (0x457620;
	// LTCG passes this in edi). RFTextArea::OnKeyDown.
	void HandleKey(EditKeyTypes::Type const& key);
	// Inserts the paragraph at index, appending it past the end, and returns
	// it (0x457890; LTCG passes this in eax, index in edi). The constructor
	// and Clear make it current.
	RFParagraph* InsertParagraph(RFParagraph* paragraph, uint32_t index);
	// Deletes the paragraph at index and removes it; nothing happens past the
	// end (0x4577b0). AddText drops the first paragraph past the line limit.
	void RemoveParagraph(uint32_t index);
	// The paragraph before paragraph, 0 when there is none (0x457910; LTCG
	// passes this in edx, paragraph in ecx). The name is ours.
	RFParagraph* GetPrevParagraph(RFParagraph* paragraph);
	// The paragraph after paragraph, 0 when there is none (0x4579d0; LTCG
	// passes this in ebx, paragraph in ecx). The name is ours.
	RFParagraph* GetNextParagraph(RFParagraph* paragraph);
	// The index of paragraph in the deque, -1 when it is null or not there
	// (0x457a90; LTCG passes this in edx, paragraph in ecx, and inlines it
	// into AddText, GetPrevParagraph and GetNextParagraph). The name is ours.
	uint32_t GetParagraphIndex(RFParagraph* paragraph);

	// Field accessors, header inlines: RFTextArea inlines each one; the
	// exe keeps an unreferenced out-of-line copy of each in RFTextArea's run
	// (LTCG conventions: this in eax, the argument in ecx), paired with the
	// copies RFTextArea.cpp emits. The names are ours.
	bool IsComposing() const { return m_composing; }                         // 0x426520
	void SetHeight(int32_t height) { m_height = height; }                    // 0x426530
	void SetHAlign(HorzAlignTypes::Type hAlign) { m_hAlign = hAlign; }      // 0x426540
	void SetTextLimit(uint32_t textLimit) { m_textLimit = textLimit; }       // 0x426550
	uint32_t GetTextLimit() const { return m_textLimit; }                    // 0x426560
	uint32_t GetLineLimit() const { return m_lineLimit; }                    // 0x426570
	// Makes paragraph current unless it is null (the unreferenced copy 0x457790
	// lies in RFText's run; the constructor, Clear, AddText and the
	// MoveCaretTo predicate inline it). The name is ours.
	void SetCurrentParagraph(RFParagraph* paragraph) {                       // 0x457790
		if (paragraph != 0) {
			m_currentParagraph = paragraph;
		}
	}

	// The paragraph the text is being added to (the constructor stores the
	// one sub_457890 returns for its first paragraph).
	RFParagraph *m_currentParagraph;        // 0x04
	// The paragraphs (VS 2010 deque: the container proxy at 0x8, the constructor
	// allocates it; map, map size, offset and size at 0xc-0x18). Its
	// begin()/end() (0x457c00/0x457c40, LTCG register arguments) and
	// std::for_each (0x458bf0) are library instantiations: RFTextAreaSC
	// sums the paragraph heights with them. The element type is assumed
	// (one pointer per paragraph).
	std::deque<RFParagraph*> m_paragraphs;  // 0x08
	RFTextArea* m_context;                  // 0x20 (the owning text area, passed to every paragraph)
	FontInfo* m_font;                       // 0x24 (passed to every paragraph)
	FontInfo* m_linkFont;                   // 0x28 (passed to every paragraph)
	int32_t m_width;                        // 0x2c (RFTextArea::OnLayoutChanged: padded client width)
	int32_t m_height;                       // 0x30 (RFTextArea::OnLayoutChanged: padded client height)
	int32_t m_lineSpacing;                  // 0x34, 130 (percent) after construction
	uint32_t m_lineLimit;                   // 0x38, 200 after construction (RFTextArea::GetLineLimit)
	uint32_t m_textLimit;                   // 0x3c, 500 after construction (RFTextArea::SetTextLimit)
	int32_t m_reserved;                     // 0x40, 0 after construction (no reader known yet)
	HorzAlignTypes::Type m_hAlign;          // 0x44 (RFTextArea::SetHAlign)
	VertAlignTypes::Type m_vAlign;          // 0x48
	// An IME composition string is shown (InsertText first removes its
	// m_compositionLength characters). While set, RFTextArea::OnLButtonDown
	// defers a click and OnChar replays it after the next insertion.
	bool m_composing;                       // 0x4c
	uint32_t m_compositionLength;           // 0x50

};
CHECK_SIZE(RFText, 0x54);
BEGIN_LAYOUT_CHECK(RFText)
	CHECK_FIELD(RFText, m_currentParagraph, 0x04);
	CHECK_FIELD(RFText, m_paragraphs, 0x08);
	CHECK_FIELD(RFText, m_context, 0x20);
	CHECK_FIELD(RFText, m_font, 0x24);
	CHECK_FIELD(RFText, m_linkFont, 0x28);
	CHECK_FIELD(RFText, m_width, 0x2c);
	CHECK_FIELD(RFText, m_height, 0x30);
	CHECK_FIELD(RFText, m_lineSpacing, 0x34);
	CHECK_FIELD(RFText, m_lineLimit, 0x38);
	CHECK_FIELD(RFText, m_textLimit, 0x3c);
	CHECK_FIELD(RFText, m_reserved, 0x40);
	CHECK_FIELD(RFText, m_hAlign, 0x44);
	CHECK_FIELD(RFText, m_vAlign, 0x48);
	CHECK_FIELD(RFText, m_composing, 0x4c);
	CHECK_FIELD(RFText, m_compositionLength, 0x50);
END_LAYOUT_CHECK

// Sums the heights of a text's paragraphs (RFTextArea::FitToContent,
// RFTextAreaSC::IsScrollBarAvailable and UpdateScrollbar; std::for_each
// 0x458bf0, in RFText's run, inlines operator()). The last paragraph's last
// line is not scaled by the line spacing. The name is ours.
struct ParagraphHeightSum {
	ParagraphHeightSum(uint32_t* total, RFText const* text) : m_total(total), m_text(text) {} // 0x457360

	void operator()(RFParagraph* paragraph) { // 0x457370
		*m_total += paragraph->GetHeight(paragraph == m_text->m_paragraphs.back());
	}

	uint32_t* m_total;
	RFText const* m_text;
};

#endif
