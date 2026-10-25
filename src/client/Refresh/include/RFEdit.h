// Original: client\Refresh\include\RFEdit.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFEDIT_H
#define PG_CLIENT_REFRESH_INCLUDE_RFEDIT_H

#include "RFExport.h"
#include "RFLabel.h"
#include "RFWndEx.h"

class ImageSet;
class RFCaret;

// The input filter RFEdit::SetValidator installs (0x416520). Unexported, no
// RTTI, and no subclass or vtable in the exe (SetValidator has no caller), so
// only the slot RFEdit::OnChar calls is known: slot 1, an int test of the
// typed string (`test eax,eax`; nonzero accepts). Slot 0 is unknown (no
// call, no delete through the base is seen); it is declared only to give
// Validate its slot. Names ours.
class RFValidator {
public:
	virtual void UnknownSlot0() = 0;                   // [0] unknown
	virtual int32_t Validate(std::wstring const&) = 0; // [1]
};

// The layout data of an edit box, read by RFEdit::Create (0x435150; RTTI
// ".?AUEditBase@@", vtable 0xd65a1c, scalar deleting destructor 0x43b110):
// a WndExBase followed by the edit's fields, the properties registered by
// name in m_properties.
struct EditBase : public WndExBase {
	virtual WndType GetType() const { return WndType_Edit; } // [1] 0x43b100

	EditBase();                                  // 0x43aaf0 (RFXmlLoader.cpp)
	~EditBase();                                 // 0x43aef0 (RFXmlLoader.cpp)

	RFStringProperty m_fontName;                 // 0x180 "FontName"
	RFIntegerProperty m_fontSize;                // 0x1bc "FontSize"
	RFColorProperty m_color;                     // 0x1e0 "Color" (0xffffffff)
	RFColorProperty m_shadowColor;               // 0x204 "ShadowColor"
	RFColorProperty m_outlineColor;              // 0x228 "OutlineColor"
	uint32_t m_style;                            // 0x24c
	HorzAlignTypes::Type m_hAlign;               // 0x250
	VertAlignTypes::Type m_vAlign;               // 0x254
	RFRect m_padding;                            // 0x258
	wchar_t m_passwordChar;                      // 0x268
	ImageSet* m_caretBrushDesc;                  // 0x26c
};
CHECK_SIZE(EditBase, 0x270);
CHECK_FIELD(EditBase, m_fontName, 0x180);
CHECK_FIELD(EditBase, m_fontSize, 0x1bc);
CHECK_FIELD(EditBase, m_color, 0x1e0);
CHECK_FIELD(EditBase, m_shadowColor, 0x204);
CHECK_FIELD(EditBase, m_outlineColor, 0x228);
CHECK_FIELD(EditBase, m_style, 0x24c);
CHECK_FIELD(EditBase, m_hAlign, 0x250);
CHECK_FIELD(EditBase, m_vAlign, 0x254);
CHECK_FIELD(EditBase, m_padding, 0x258);
CHECK_FIELD(EditBase, m_passwordChar, 0x268);
CHECK_FIELD(EditBase, m_caretBrushDesc, 0x26c);

// Single-line text input: vtable 0xd639ac, 34 slots (no new virtuals), size
// 0x228 (operator new in its factory 0x432c70). dllexport as in the original
// library: cl 16 then emits the vector deleting destructor of slot 1 and the
// exported copy constructor and assignment.
class RF_API RFEdit: public RFWndEx {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe46fac that GetTypeName returns.
	static std::wstring const TypeName;
	// The event KeyDownReturn raises, the static const at 0xe46f90.
	static std::wstring const EventEnterKeyDown;

	virtual wchar_t const* ToString() const;                  // [0]  0x415f20
	// [1] vector deleting destructor 0x4163a0, ~RFEdit 0x4350c0
	virtual ~RFEdit();
	virtual std::wstring const& GetTypeName();                // [2]  0x415f30
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x435150
	virtual bool IsFocusable() const;                         // [7]  0x415f70

	RFEdit();                                                 //      0x435000

	bool IsEnableCopyAndPaste() const;                        //      0x415f40
	void SetLimitText(int32_t);                               //      0x415f50
	int32_t GetLimitText() const;                             //      0x415f60
	void SetColor(uint32_t);                                  //      0x416400
	uint32_t GetColor() const;                                //      0x416410
	void SetShadowColor(uint32_t);                            //      0x416420
	uint32_t GetShadowColor() const;                          //      0x416430
	void SetOutlineColor(uint32_t);                           //      0x416440
	uint32_t GetOutlineColor() const;                         //      0x416450
	void SetSelectionColor(uint32_t, uint32_t);               //      0x416460
	void SetStyle(uint32_t);                                  //      0x416480
	uint32_t GetStyle() const;                                //      0x416490
	void SetPadding(RFRect const&);                           //      0x4164a0
	RFRect const& GetPadding() const;                         //      0x4164d0
	void SetPasswordChar(wchar_t);                            //      0x4164e0
	wchar_t GetPasswordChar() const;                          //      0x416500
	bool IsFocused() const;                                   //      0x416510
	void SetValidator(RFValidator*);                          //      0x416520
	RFValidator* GetValidator() const;                        //      0x416530
	void SetText(wchar_t const*);                             //      0x435390
	wchar_t const* GetText() const;                           //      0x4353e0
	void ClearText();                                         //      0x435400
	void EnableCopyAndPaste(bool);                            //      0x436a50

protected:
	virtual void LoadResource();                              // [8]  0x435310
	virtual void PreInitialize();                             // [9]  0x435230
	virtual void OnDestroy();                                 // [11] 0x4352d0
	virtual void OnEnabled(bool);                             // [12] 0x4354a0
	virtual void OnLButtonDown(MouseEventArgs&);              // [15] 0x435690
	virtual void OnKeyDown(KeyEventArgs&);                    // [24] 0x4358a0
	virtual void OnChar(CharEventArgs&);                      // [26] 0x435ab0
	virtual void OnSetFocus();                                // [29] 0x435510
	virtual void OnKillFocus();                               // [30] 0x4355d0
	virtual void OnUpdate(UpdateEventArgs&);                  // [31] 0x4361c0
	virtual void OnDraw();                                    // [32] 0x435cd0

	int32_t GetCaretLocate() const;                           //      0x415f80
	// Not exported (InsertStringAtCaretPos, GetTextHAlignPos, GetTextVAlignPos,
	// GetCaretPos): names ours, decorated as in port/refresh/exports.tsv's
	// unexported (`?`) rows, which pair them.
	std::wstring InsertStringAtCaretPos(std::wstring const&); //      0x436210
	void DeleteCharAtCaretPos();                              //      0x4363b0
	void DeleteSelectionChar();                               //      0x436400
	void DeleteChar(int32_t, int32_t);                        //      0x436460
	void CleanupPasswordText();                               //      0x4364e0
	int32_t GetTextHAlignPos(RFRect const&, std::wstring const&); //  0x436530
	int32_t GetTextVAlignPos(RFRect const&, std::wstring const&); //  0x4367c0
	int32_t GetCaretPos(std::wstring const&, bool);           //      0x436930
	void DrawCaret(RFVector2 const&, float);                  //      0x436a70
	void ResetCaretBlink();                                   //      0x436ad0
	void ResetSelection();                                    //      0x436af0
	void DrawSelection(RFVector2 const&, float);              //      0x436b20
	void SetCaretLocate(int32_t);                             //      0x436d40
	void AddCaretLocate(int32_t);                             //      0x436d80
	bool KeyDownCopyAndPaste(uint32_t);                       //      0x436dc0
	void KeyDownSelectStart();                                //      0x436ff0
	void KeyDownSelectEnd();                                  //      0x437030
	void KeyDownHome();                                       //      0x437060
	void KeyDownEnd();                                        //      0x437080
	void KeyDownDeleteChar(bool);                             //      0x4370a0
	void KeyDownArrow(int32_t);                               //      0x437140
	void KeyDownReturn(KeyEventArgs&);                        //      0x437180

private:
	// Members as the constructor (0x435000, new 0x228 in 0x432c70) and the
	// accessors show them.
	std::wstring m_text;                    // 0x180 (CleanupPasswordText blanks it)
	bool m_focused;                         // 0x19c
	bool m_enableCopyAndPaste;              // 0x19d
	int32_t m_limitText;                    // 0x1a0
	int32_t m_textOffset[2];                // 0x1a4 (OnLButtonDown adds [0] to the glyph positions)
	int32_t m_caretLocate;                  // 0x1ac (0 .. m_text.size())
	// A pending range DeleteCharAtCaretPos deletes instead of one character
	// (presumably the IME composition), reset to the caret afterwards. One
	// aggregate: the copy constructor copies it as three dwords.
	struct Composition {
		Composition() : composing(false), start(0), end(0) {}

		bool composing;                     // 0x0
		int32_t start;                      // 0x4
		int32_t end;                        // 0x8
	};
	Composition m_composition;              // 0x1b0
	RFCaret* m_caret;                       // 0x1bc (new 0x1c, constructor 0x455f80, vtable 0xd65d7c)
	int32_t m_selectionStart;               // 0x1c0
	int32_t m_selectionEnd;                 // 0x1c4
	bool m_unreadFlag;                      // 0x1c8 (only PreInitialize clears it; no reader found in RFEdit)
	RFValidator* m_validator;               // 0x1cc
	uint32_t m_selectionColor[2];           // 0x1d0 (SetSelectionColor's two arguments)
	FontInfo m_fontInfo;                    // 0x1d8 (OnDraw copies it; the constructor's memset is its constructor)
	HorzAlignTypes::Type m_hAlign;          // 0x208 (after the font as in RFLabel; Create reads both)
	VertAlignTypes::Type m_vAlign;          // 0x20c
	RFRect m_padding;                       // 0x210
	wchar_t m_passwordChar;                 // 0x220
	ImageSet* m_caretBrushDesc;             // 0x224 (LoadResource builds the caret's brush from it)
};
CHECK_SIZE(RFEdit, 0x228);
BEGIN_LAYOUT_CHECK(RFEdit)
	CHECK_SIZE(RFEdit::Composition, 0xc);
	CHECK_FIELD(RFEdit::Composition, start, 0x4);
	CHECK_FIELD(RFEdit::Composition, end, 0x8);
	CHECK_FIELD(RFEdit, m_text, 0x180);
	CHECK_FIELD(RFEdit, m_focused, 0x19c);
	CHECK_FIELD(RFEdit, m_enableCopyAndPaste, 0x19d);
	CHECK_FIELD(RFEdit, m_limitText, 0x1a0);
	CHECK_FIELD(RFEdit, m_caretLocate, 0x1ac);
	CHECK_FIELD(RFEdit, m_composition, 0x1b0);
	CHECK_FIELD(RFEdit, m_caret, 0x1bc);
	CHECK_FIELD(RFEdit, m_selectionStart, 0x1c0);
	CHECK_FIELD(RFEdit, m_selectionEnd, 0x1c4);
	CHECK_FIELD(RFEdit, m_unreadFlag, 0x1c8);
	CHECK_FIELD(RFEdit, m_validator, 0x1cc);
	CHECK_FIELD(RFEdit, m_selectionColor, 0x1d0);
	CHECK_FIELD(RFEdit, m_textOffset, 0x1a4);
	CHECK_FIELD(RFEdit, m_fontInfo, 0x1d8);
	CHECK_FIELD(RFEdit, m_hAlign, 0x208);
	CHECK_FIELD(RFEdit, m_vAlign, 0x20c);
	CHECK_FIELD(RFEdit, m_padding, 0x210);
	CHECK_FIELD(RFEdit, m_passwordChar, 0x220);
	CHECK_FIELD(RFEdit, m_caretBrushDesc, 0x224);
END_LAYOUT_CHECK

// The window factory of the "RFEdit" type (slot 0, the scalar deleting
// destructor, is 0x432d00).
class RFEditFactory : public RFWindowFactory {
public:
	RFEditFactory(); // 0x432c20 (RFEdit.cpp, beside Create)
	virtual RFWnd* Create();             // [1] 0x432c70
	virtual void Destroy(RFWnd*);        // [2] 0x432ce0
};

#endif
