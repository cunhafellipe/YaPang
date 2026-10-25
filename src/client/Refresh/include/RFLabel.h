// Original: client\Refresh\include\RFLabel.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFLABEL_H
#define PG_CLIENT_REFRESH_INCLUDE_RFLABEL_H

#include <stdint.h>
#include <new>
#include <string>

#include "RFExport.h"
#include "RFRect.h"
#include "RFTypes.h"
#include "RFWindowFactory.h"
#include "RFWnd.h"
#include "defines.h"
#include "padding.h"

// The font of a text, size 0x30: copied member by member by
// RFTextArea::SetHyperLinkFontInfo (0x4156e0), passed to RFWnd::PrintText;
// the names follow the label's XML attributes (FontName, FontSize, color,
// OutlineColor, ShadowColor) and accessors.
struct FontInfo {
	// Inlined in RFLabel's constructor (0x41f0c0): the five words after the
	// name zeroed.
	FontInfo() : size(0), style(0), color(0), outlineColor(0), shadowColor(0) {}

	std::wstring name;     // 0x00
	int32_t size;          // 0x1c
	uint32_t style;        // 0x20
	uint32_t color;        // 0x24
	uint32_t outlineColor; // 0x28
	uint32_t shadowColor;  // 0x2c
};
CHECK_SIZE(FontInfo, 0x30);
BEGIN_LAYOUT_CHECK(FontInfo)
	CHECK_FIELD(FontInfo, name, 0x00);
	CHECK_FIELD(FontInfo, size, 0x1c);
	CHECK_FIELD(FontInfo, style, 0x20);
	CHECK_FIELD(FontInfo, color, 0x24);
	CHECK_FIELD(FontInfo, outlineColor, 0x28);
	CHECK_FIELD(FontInfo, shadowColor, 0x2c);
END_LAYOUT_CHECK

// The layout data of a label, read by RFLabel::Create (0x41f1c0; its RTTI
// name ".?AULabelBase@@", vtable 0xd65a28): the window's WndBaseStruct
// followed by the label's fields, most of them RFProperty members the
// constructor registers by name.
struct LabelBase : public WndBaseStruct {
	// [1] 0x43b890 (the label's WndType)
	virtual WndType GetType() const { return WndType_Label; }

	LabelBase();

	std::wstring m_label;                        // 0x17c, translated by the interface
	RFRectProperty m_padding;                    // 0x198 "Padding"
	RFStringProperty m_fontName;                 // 0x1c8 "FontName"
	RFIntegerProperty m_fontSize;                // 0x204 "FontSize"
	RFColorProperty m_color;                     // 0x228 "Color" (0xffffffff)
	RFColorProperty m_shadowColor;               // 0x24c "ShadowColor"
	RFColorProperty m_outlineColor;              // 0x270 "OutlineColor"
	int32_t m_style;                             // 0x294
	RFHAlignProperty m_hAlign;                   // 0x298 "HAlign"
	RFVAlignProperty m_vAlign;                   // 0x2bc "VAlign"
	RFTextTrimmingProperty m_textTrimmingType;   // 0x2e0 "TextTrimming"
	RFBooleanProperty m_autoFit;                 // 0x304 "FitToContent"
};
CHECK_SIZE(LabelBase, 0x328);
CHECK_FIELD(LabelBase, m_label, 0x17c);
CHECK_FIELD(LabelBase, m_padding, 0x198);
CHECK_FIELD(LabelBase, m_fontName, 0x1c8);
CHECK_FIELD(LabelBase, m_fontSize, 0x204);
CHECK_FIELD(LabelBase, m_color, 0x228);
CHECK_FIELD(LabelBase, m_shadowColor, 0x24c);
CHECK_FIELD(LabelBase, m_outlineColor, 0x270);
CHECK_FIELD(LabelBase, m_style, 0x294);
CHECK_FIELD(LabelBase, m_hAlign, 0x298);
CHECK_FIELD(LabelBase, m_vAlign, 0x2bc);
CHECK_FIELD(LabelBase, m_textTrimmingType, 0x2e0);
CHECK_FIELD(LabelBase, m_autoFit, 0x304);

// A Refresh text label: RFWnd with the label rendering (vtable 0xd63a3c, 34
// slots, no new virtuals), size 0x1e4 (operator new in its factory 0x432dd0).
// dllexport as in the original library: cl 16 then emits the vector deleting
// destructor of slot 1 and the exported copy constructor and assignment.
class RF_API RFLabel: public RFWnd {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe46c9c that GetTypeName returns.
	static std::wstring const TypeName;

	// [0] 0x4142e0, [2] 0x4142f0 and the accessors 0x414660..0x414750:
	// header inlines (their copies sit in the COMDAT run of the implicit copy
	// constructor, operator= and ??_E, after RFWndEx's)
	virtual wchar_t const* ToString() const {
		return L"RFLabel";
	}
	virtual ~RFLabel();                                       // [1]  vector deleting destructor 0x414560, ~RFLabel 0x41f140
	virtual std::wstring const& GetTypeName() {
		return TypeName;
	}
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x41f1c0

	RFLabel();                                                //      0x41f0c0
	std::wstring const& GetLabel() const { return m_label; }                      // 0x414660
	void SetColor(uint32_t color) { m_fontInfo.color = color; }                  // 0x414670
	uint32_t GetColor() const { return m_fontInfo.color; }                       // 0x414680
	void SetShadowColor(uint32_t color) { m_fontInfo.shadowColor = color; }      // 0x414690
	uint32_t GetShadowColor() const { return m_fontInfo.shadowColor; }           // 0x4146a0
	void SetOutlineColor(uint32_t color) { m_fontInfo.outlineColor = color; }    // 0x4146b0
	uint32_t GetOutlineColor() const { return m_fontInfo.outlineColor; }         // 0x4146c0
	void SetStyle(uint32_t style) { m_fontInfo.style = style; }                  // 0x4146d0
	uint32_t GetStyle() const { return m_fontInfo.style; }                       // 0x4146e0
	void SetVAlign(VertAlignTypes::Type align) { m_vAlign = align; }             // 0x4146f0
	void SetHAlign(HorzAlignTypes::Type align) { m_hAlign = align; }             // 0x414700
	VertAlignTypes::Type GetVAlign() const { return m_vAlign; }                  // 0x414710
	HorzAlignTypes::Type GetHAlign() const { return m_hAlign; }                  // 0x414720
	FontInfo const& GetFontInfo() const { return m_fontInfo; }                   // 0x414730
	void SetTextTrimmingType(TextTrimmingTypes::Type type) { m_textTrimmingType = type; }  // 0x414740
	TextTrimmingTypes::Type GetTextTrimmingType() const { return m_textTrimmingType; }      // 0x414750
	void FitToContent();                                      //      0x41f340
	void AutoFit(bool);                                       //      0x41f490
	void SetLabel(int32_t, bool);                             //      0x41f580
	void SetLabel(std::wstring const&);                       //      0x41f830
	void SetPadding(RFRect const&);                           //      0x41f860
	RFRect const& GetPadding() const;                         //      0x41f890

protected:
	virtual void PreInitialize();                             // [9]  0x41f4b0
	virtual void OnDraw();                                    // [32] 0x41f8a0
	void putComma_(wchar_t const*, wchar_t*);                 //      0x41f510

private:
	std::wstring m_label;                         // 0x178
	RFRect m_padding;                             // 0x194
	FontInfo m_fontInfo;                          // 0x1a4
	HorzAlignTypes::Type m_hAlign;                // 0x1d4
	VertAlignTypes::Type m_vAlign;                // 0x1d8
	TextTrimmingTypes::Type m_textTrimmingType;   // 0x1dc
	bool m_autoFit;                               // 0x1e0
};
CHECK_SIZE(RFLabel, 0x1e4);
BEGIN_LAYOUT_CHECK(RFLabel)
	CHECK_FIELD(RFLabel, m_label, 0x178);
	CHECK_FIELD(RFLabel, m_padding, 0x194);
	CHECK_FIELD(RFLabel, m_fontInfo, 0x1a4);
	CHECK_FIELD(RFLabel, m_hAlign, 0x1d4);
	CHECK_FIELD(RFLabel, m_vAlign, 0x1d8);
	CHECK_FIELD(RFLabel, m_textTrimmingType, 0x1dc);
	CHECK_FIELD(RFLabel, m_autoFit, 0x1e0);
END_LAYOUT_CHECK

// The factory RFWindowFactoryManager maps the label's layout type to (slot 0,
// the implicit scalar deleting destructor 0x432e60, is emitted where the
// factory is constructed).
class RFLabelFactory : public RFWindowFactory {
public:
	RFLabelFactory(); // 0x432d80 (RFLabel.cpp, beside Create)
	virtual RFWnd* Create();             // [1] 0x432dd0
	virtual void Destroy(RFWnd*);        // [2] 0x432e40
};

#endif
