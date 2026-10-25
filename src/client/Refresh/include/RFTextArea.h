// Original: client\Refresh\include\RFTextArea.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFTEXTAREA_H
#define PG_CLIENT_REFRESH_INCLUDE_RFTEXTAREA_H

#include <stdint.h>

#include <string>
#include <vector>

#include "RFExport.h"
#include "RFLabel.h"
#include "RFRect.h"
#include "RFVector2.h"
#include "RFWndEx.h"
#include "defines.h"
#include "padding.h"

// The laid-out text (vtable 0xd65d98, 0x54 bytes, built by Create);
// unexported, declared in src/RFText.h.
class RFText;
// The blinking caret (src/RFCaret.h), built by LoadResource.
class RFCaret;
class ImageSet;

// The layout data of a text area (RTTI ".?AUTextAreaBase@@", vtable
// 0xd65a7c), read by RFTextArea::Create (0x4267b0): a WndExBase followed by
// the text area's properties, registered by name in m_properties.
// RFTextAreaSC's data extends it from 0x2ec.
struct TextAreaBase : public WndExBase {
	// Registers the properties (values zero, strings empty).
	TextAreaBase();                              // 0x43c8d0 (RFXmlLoader.cpp)

	// Drops the reference to the caret's image set first.
	virtual ~TextAreaBase() {                    // [0] 0x43cec0
		if (m_caretBrushDesc) {
			if (m_caretBrushDesc->Release() == 0)
				delete m_caretBrushDesc;
			m_caretBrushDesc = 0;
		}
	}

	// [1] 0x43d1a0 (the text area's WndType)
	virtual WndType GetType() const { return WndType_TextArea; }

	RFStringProperty m_fontName;                 // 0x180 "FontName"
	RFIntegerProperty m_fontSize;                // 0x1bc "FontSize"
	RFColorProperty m_color;                     // 0x1e0 "Color"
	RFColorProperty m_shadowColor;               // 0x204 "ShadowColor"
	RFColorProperty m_outlineColor;              // 0x228 "OutlineColor"
	RFRect m_padding;                            // 0x24c
	uint32_t m_style;                            // 0x25c
	HorzAlignTypes::Type m_hAlign;               // 0x260
	RFBooleanProperty m_showCaret;               // 0x264 "Editable"
	RFStringProperty m_text;                     // 0x288 "Text" (Create sets it through SetText)
	RFBooleanProperty m_autoFit;                 // 0x2c4 "FitToContent"
	ImageSet* m_caretBrushDesc;                  // 0x2e8
};
CHECK_SIZE(TextAreaBase, 0x2ec);
CHECK_FIELD(TextAreaBase, m_fontName, 0x180);
CHECK_FIELD(TextAreaBase, m_fontSize, 0x1bc);
CHECK_FIELD(TextAreaBase, m_color, 0x1e0);
CHECK_FIELD(TextAreaBase, m_shadowColor, 0x204);
CHECK_FIELD(TextAreaBase, m_outlineColor, 0x228);
CHECK_FIELD(TextAreaBase, m_padding, 0x24c);
CHECK_FIELD(TextAreaBase, m_style, 0x25c);
CHECK_FIELD(TextAreaBase, m_hAlign, 0x260);
CHECK_FIELD(TextAreaBase, m_showCaret, 0x264);
CHECK_FIELD(TextAreaBase, m_text, 0x288);
CHECK_FIELD(TextAreaBase, m_autoFit, 0x2c4);
CHECK_FIELD(TextAreaBase, m_caretBrushDesc, 0x2e8);

// A left click OnLButtonDown defers during an IME composition (RFText +0x4c);
// OnChar replays it and clears it. The name is ours.
struct TextAreaPendingClick {
	// Defined in RFTextArea.cpp (its out-of-line copy 0x4261e0 lies in
	// the class's address run; LTCG inlines it into the constructor).
	TextAreaPendingClick();

	// Also inlined by OnChar once the click is replayed.
	void Clear() {
		pending = false;
		pos.x = 0.0f;
		pos.y = 0.0f;
	}

	bool pending;                                // 0x0
	RFVector2 pos;                               // 0x4 (screen position of the click)
};
CHECK_SIZE(TextAreaPendingClick, 0xc);

// A string with a rectangle (0x2c bytes; the vector's copy 0x41a7e0 copies
// the string, then the four floats). Only the constructors and the
// destructor of RFTextArea touch the vector so far: the role is not known.
// The name is ours.
struct TextAreaRegion {
	std::wstring name;                           // 0x00
	RFRect rect;                                 // 0x1c
};
CHECK_SIZE(TextAreaRegion, 0x2c);

// Multi-line text display: vtable 0xd63eec, 39 slots (0–33 as RFWndEx,
// the text virtuals 34–38 are new). Size 0x274 (the vector deleting
// destructor 0x415bf0).
class RF_API RFTextArea: public RFWndEx {
	LAYOUT_CHECK_FRIEND;

public:
	// The events it raises (values in the .cpp).
	static std::wstring const EventHyperLinkClicked;          //      0xe46dec
	static std::wstring const EventHyperLinkEnter;            //      0xe46e08
	static std::wstring const EventHyperLinkLeave;            //      0xe46e24
	static std::wstring const EventRichTextDrawImage;         //      0xe46e40

	RFTextArea();                                             //      0x426580

	virtual wchar_t const* ToString() const;                  // [0]  0x415690
	virtual ~RFTextArea();                                    // [1]  scalar deleting destructor 0x415bf0, ~RFTextArea 0x426690
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x4267b0

protected:
	virtual void LoadResource();                              // [8]  0x426bf0
	virtual void PreInitialize();                             // [9]  0x426b20
	virtual void OnDestroy();                                 // [11] 0x426b90
	virtual void OnEnabled(bool);                             // [12] 0x426d90
	virtual void OnLayoutChanged(uint32_t);                   // [14] 0x426c90
	virtual void OnLButtonDown(MouseEventArgs&);              // [15] 0x426e00
	virtual void OnRButtonDown(MouseEventArgs&);              // [18] 0x426ee0
	virtual void OnRButtonUp(MouseEventArgs&);                // [19] 0x426f30
	virtual void OnMouseMove(MouseEventArgs&);                // [20] 0x427170
	virtual void OnMouseWheel(MouseEventArgs&);               // [21] 0x427b80
	virtual void OnMouseLeave();                              // [23] 0x4274e0
	virtual void OnKeyDown(KeyEventArgs&);                    // [24] 0x4275f0
	virtual void OnChar(CharEventArgs&);                      // [26] 0x427810
	virtual void OnSetFocus();                                // [29] 0x4278d0
	virtual void OnKillFocus();                               // [30] 0x427950
	virtual void OnUpdate(UpdateEventArgs&);                  // [31] 0x427b30
	virtual void OnDraw();                                    // [32] 0x4279c0

public:
	virtual void SetText(std::wstring const&);                // [34] 0x427bc0
	virtual void AddText(std::wstring const&);                // [35] 0x427bf0
	virtual void AddPlainText(std::wstring const&);           // [36] 0x427c50
	virtual std::wstring const& GetText();                    // [37] 0x427c90
	virtual void ClearText();                                 // [38] 0x427d10

	void SetPadding(RFRect const&);                           //      0x4156a0
	RFRect const& GetPadding() const;                         //      0x4156d0
	void SetHyperLinkFontInfo(FontInfo const&);               //      0x4156e0
	void SetColor(uint32_t);                                  //      0x415c50
	uint32_t GetColor() const;                                //      0x415c60
	void SetShadowColor(uint32_t);                            //      0x415c70
	uint32_t GetShadowColor() const;                          //      0x415c80
	void SetOutlineColor(uint32_t);                           //      0x415c90
	uint32_t GetOutlineColor() const;                         //      0x415ca0
	void SetStyle(uint32_t);                                  //      0x415cb0
	uint32_t GetStyle() const;                                //      0x415cc0
	FontInfo const& GetFontInfo() const;                      //      0x415cd0
	void SetHAlign(HorzAlignTypes::Type);                     //      0x427c30
	void SetTextLimit(uint32_t);                              //      0x427d30
	uint32_t GetTextLimit() const;                            //      0x427d50
	uint32_t GetLineLimit() const;                            //      0x427d60
	void FitToContent(bool);                                  //      0x427d70
	void AutoFit(bool);                                       //      0x427f90
	void Invaildate();                                        //      0x427fb0
	void SendDrawImageEvent(RFVector2 const&, std::wstring const&); // 0x428040
	int32_t GetTextHeight() const;                            //      0x428220
	int32_t GetNumOfLine() const;                             //      0x428300

protected:
	bool HitTestHyperLink(MouseEventArgs&, bool);             //      0x426f60
	void ApplyGlobalPadding(RFRect&) const;                   //      0x428130
	void DrawCaret(float);                                    //      0x428310
	void ResetCaretBlink();                                   //      0x4283a0

	RFText *m_text;                         // 0x180, its +0x38 is the line limit, +0x3c the text limit
	std::wstring m_textCache;               // 0x184 (GetText returns the laid-out text here)
	bool m_focused;                         // 0x1a0 (OnKillFocus clears it; OnEnabled tests it)
	int32_t m_scrollX;                      // 0x1a4 (ClearText clears it)
	int32_t m_scrollY;                      // 0x1a8 (ClearText clears it; RFTextAreaSC stores -pos)
	// No code reads or writes it except the implicit copy constructor
	// (0x415720) and operator= (0x4159d0), which copy it as a dword: its
	// role is not known.
	uint32_t m_unknownValue;                // 0x1ac
	RFCaret *m_caret;                      // 0x1b0 (OnDestroy deletes it)
	FontInfo m_hyperLinkFontInfo;           // 0x1b4
	TextAreaPendingClick m_pendingClick;    // 0x1e4
	std::vector<TextAreaRegion> m_regions;  // 0x1f0
	bool m_hyperLinkHovered;                // 0x200 (OnMouseMove sets it, OnMouseLeave clears it)
	std::wstring m_hoveredHyperLink;        // 0x204 (the link under the mouse, OnMouseMove)
	int32_t m_lineHeight;                   // 0x220 (Create; RFTextAreaSC's scrolling steps)
	FontInfo m_fontInfo;                    // 0x224
	RFRect m_padding;                       // 0x254
	HorzAlignTypes::Type m_hAlign;          // 0x264
	VertAlignTypes::Type m_vAlign;          // 0x268 (Create: 0, passed to the RFText with m_hAlign)
	bool m_showCaret;                       // 0x26c (the caret is drawn and blinks only while set)
	bool m_autoFit;                         // 0x26d
	ImageSet* m_caretBrushDesc;             // 0x270 (Create; LoadResource gives it to the caret)
};
CHECK_SIZE(RFTextArea, 0x274);
BEGIN_LAYOUT_CHECK(RFTextArea)
	CHECK_FIELD(RFTextArea, m_text, 0x180);
	CHECK_FIELD(RFTextArea, m_textCache, 0x184);
	CHECK_FIELD(RFTextArea, m_focused, 0x1a0);
	CHECK_FIELD(RFTextArea, m_scrollX, 0x1a4);
	CHECK_FIELD(RFTextArea, m_scrollY, 0x1a8);
	CHECK_FIELD(RFTextArea, m_unknownValue, 0x1ac);
	CHECK_FIELD(RFTextArea, m_caret, 0x1b0);
	CHECK_FIELD(RFTextArea, m_hyperLinkFontInfo, 0x1b4);
	CHECK_FIELD(RFTextArea, m_pendingClick, 0x1e4);
	CHECK_FIELD(RFTextArea, m_regions, 0x1f0);
	CHECK_FIELD(RFTextArea, m_hyperLinkHovered, 0x200);
	CHECK_FIELD(RFTextArea, m_hoveredHyperLink, 0x204);
	CHECK_FIELD(RFTextArea, m_lineHeight, 0x220);
	CHECK_FIELD(RFTextArea, m_fontInfo, 0x224);
	CHECK_FIELD(RFTextArea, m_padding, 0x254);
	CHECK_FIELD(RFTextArea, m_hAlign, 0x264);
	CHECK_FIELD(RFTextArea, m_vAlign, 0x268);
	CHECK_FIELD(RFTextArea, m_showCaret, 0x26c);
	CHECK_FIELD(RFTextArea, m_autoFit, 0x26d);
	CHECK_FIELD(RFTextArea, m_caretBrushDesc, 0x270);
END_LAYOUT_CHECK

#endif
