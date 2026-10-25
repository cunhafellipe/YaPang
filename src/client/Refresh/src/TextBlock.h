// Original: client\Refresh\src\TextBlock.h (JP 983 exe; the class is not
// exported and no assert path names its file: the name is assumed from the
// class, TextComponent::TextBlock in its RTTI)
#ifndef PG_CLIENT_REFRESH_SRC_TEXTBLOCK_H
#define PG_CLIENT_REFRESH_SRC_TEXTBLOCK_H

#include <stdint.h>
#include <string>
#include <vector>

#include "RFLabel.h"
#include "RFTextComponent.h"
#include "defines.h"

class RFTextArea;

namespace TextComponent {

struct TextRun;

// One laid-out piece of a TextBlock (0x3c bytes, the vector stride; built by
// 0x468c80, LTCG: this in eax). The hit tests read the box at 0x14 (x, y)
// and 0xc (width, height), the caret code the character range at 0x4-0x8.
struct TextSegment {
	int32_t id;                 // 0x00
	uint32_t firstChar;         // 0x04
	uint32_t lastChar;          // 0x08
	uint32_t width;             // 0x0c
	uint32_t height;            // 0x10
	int32_t x;                  // 0x14
	int32_t y;                  // 0x18
	bool visible;               // 0x1c, only visible segments are hit
	std::wstring text;          // 0x20

	TextSegment();              // 0x468c80
};
CHECK_SIZE(TextSegment, 0x3c);
CHECK_FIELD(TextSegment, width, 0xc);
CHECK_FIELD(TextSegment, x, 0x14);
CHECK_FIELD(TextSegment, visible, 0x1c);
CHECK_FIELD(TextSegment, text, 0x20);

// A run of text with one style: vtable 0xd66b24, 24 slots, size 0xac
// (constructor 0x468cb0, called by RFText 0x466d90; LTCG: this in eax). Not
// exported, so its members pair as sub_<va>.
class TextBlock : public RFTextComponent {
	LAYOUT_CHECK_FRIEND;

public:
	TextBlock();                                // 0x468cb0
	explicit TextBlock(TextRun const& run);     // 0x468d80 (LTCG: run in eax, this in ecx)
	// A block with the attributes of a run but no text yet.
	TextBlock(uint32_t const& style, std::wstring const& styleText, std::wstring const& hyperLink,
		uint32_t const& color, int32_t const& linkState, bool styleFlag);   // 0x468e80
	virtual ~TextBlock();                       // [0] scalar deleting destructor 0x468d40; 0x468f70

	virtual int32_t GetType() const;            // [1] 0x468c70
	virtual bool HitTest(int32_t x, int32_t y);         // [2] 0x469050
	virtual bool GetSegmentAt(int32_t x, int32_t y, uint32_t hitWidth, bool* pastEnd,
		int32_t* segmentId);                        // [3] 0x469150
	virtual bool GetHyperLinkAt(int32_t x, int32_t y, int32_t const* linkState,
		std::wstring* link);                        // [4] 0x4692a0
	virtual void HideSegments();                // [5] 0x4693f0
	virtual void ShowSegments(int32_t);                // [6] 0x469420
	virtual void ClearSelected();               // [7] 0x4696d0
	virtual void SetSelected(bool selected);    // [8] 0x4696e0
	virtual uint32_t Erase(uint32_t pos, uint32_t count);   // [9] 0x469710
	virtual uint32_t EraseFront(uint32_t count);        // [10] 0x4696f0
	virtual uint32_t GetLength() const;         // [11] 0x469770
	virtual uint32_t GetWidth() const;          // [12] 0x469780
	virtual bool IsEmpty() const;               // [13] 0x4697c0
	virtual bool HasSameStyle(RFTextComponent const* other) const;    // [14] 0x4697d0
	virtual uint32_t GetSegmentFirstChar(int32_t segmentId) const;   // [15] 0x469870
	virtual uint32_t GetSegmentLastChar(int32_t segmentId) const;    // [16] 0x4698c0
	virtual uint32_t GetSegmentWidth(int32_t segmentId) const;       // [17] 0x469910
	virtual uint32_t GetSegmentHeight(int32_t segmentId) const;      // [18] 0x469960
	virtual uint32_t SplitSegment(int32_t segmentId, uint32_t* offset);   // [19] 0x4699e0
	virtual void GetSegmentIdRange(int32_t* firstId, int32_t* lastId) const; // [20] 0x4699b0
	virtual void GetCharPos(uint32_t charPos, int32_t* segmentId, uint32_t* charOffset,
		uint32_t* x, bool fromFront);               // [21] 0x469b70
	virtual int32_t Layout(FontInfo const& font, FontInfo const& linkFont, uint32_t available,
		uint32_t lineWidth, int32_t* segmentId);    // [22] 0x469ea0
	virtual void SetSegmentPosition(int32_t segmentId, int32_t x, int32_t y);  // [23] 0x46a2d0

	// Draws the segments into area, the font colour's alpha scaled by alpha
	// (a picture block, style bit 8, asks the area to draw the image named by
	// m_styleText); marks every segment visible. No caller in the exe.
	void Draw(float alpha, RFTextArea* area);  // 0x469460

	// The block's text: replaced, appended to, or inserted into at pos (appended
	// past the end). No caller in the exe (LTCG: this and the arguments in
	// registers).
	void SetText(std::wstring const& text);                     // 0x46a320
	void AppendText(std::wstring const& text);                  // 0x46a330
	void InsertText(uint32_t pos, std::wstring const& text);    // 0x46a350
	// The block's text (RFParagraph::GetText 0x466620 appends every block's).
	std::wstring const& GetText() const { return m_text; }
	// The run attributes, read by RFParagraph::MoveTextAfterCaretTo 0x465660 to
	// build the block that takes the text after the caret (no out-of-line copy).
	uint32_t const& GetStyle() const { return m_style; }
	std::wstring const& GetStyleText() const { return m_styleText; }
	std::wstring const& GetHyperLink() const { return m_hyperLink; }
	uint32_t const& GetColor() const { return m_color; }
	int32_t const& GetLinkState() const { return m_linkState; }
	bool const& GetStyleFlag() const { return m_styleFlag; }

private:
	// GetCharPos's two searches: from the first segment / from the last one
	// (LTCG passes their arguments in registers).
	void GetCharPosFromFront(uint32_t charPos, int32_t* segmentId, uint32_t* charOffset,
		uint32_t* x);                               // 0x469bb0
	void GetCharPosFromBack(uint32_t charPos, int32_t* segmentId, uint32_t* charOffset,
		uint32_t* x);                               // 0x469d20

	// The font the block is drawn with: font with the style bits applied, or
	// linkFont for a styled hyperlink (LTCG: this in ebx, font in ecx).
	FontInfo GetFont(FontInfo const& font, FontInfo const& linkFont) const;   // 0x46a370

	std::vector<TextSegment> m_segments;        // 0x04
	// 0x14-0x78: the fields of RFTextParser's TextRun (its two strings in the
	// other order); TextBlock's constructors and destructor treat them as its own
	// members (the color is initialised between the strings and linkState).
	std::wstring m_text;                        // 0x14
	uint32_t m_style;                           // 0x30, bit 0x10: a hyperlink
	std::wstring m_styleText;                   // 0x34, compared by HasSameStyle; the image name of a style-8 block (Draw)
	std::wstring m_hyperLink;                   // 0x50, returned by GetHyperLinkAt
	uint32_t m_color;                           // 0x6c, 0xff000000 after construction
	int32_t m_linkState;                        // 0x70, 2 after construction; GetHyperLinkAt matches it (or 3)
	bool m_styleFlag;                           // 0x74, true after construction (role not read yet)
	FontInfo m_font;                            // 0x78, the font the layout measures with
	bool m_selected;                            // 0xa8
};
CHECK_SIZE(TextBlock, 0xac);
BEGIN_LAYOUT_CHECK(TextBlock)
	CHECK_FIELD(TextBlock, m_segments, 0x04);
	CHECK_FIELD(TextBlock, m_text, 0x14);
	CHECK_FIELD(TextBlock, m_style, 0x30);
	CHECK_FIELD(TextBlock, m_styleText, 0x34);
	CHECK_FIELD(TextBlock, m_hyperLink, 0x50);
	CHECK_FIELD(TextBlock, m_color, 0x6c);
	CHECK_FIELD(TextBlock, m_linkState, 0x70);
	CHECK_FIELD(TextBlock, m_styleFlag, 0x74);
	CHECK_FIELD(TextBlock, m_font, 0x78);
	CHECK_FIELD(TextBlock, m_selected, 0xa8);
END_LAYOUT_CHECK

} // namespace TextComponent

#endif
