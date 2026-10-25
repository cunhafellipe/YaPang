// The JP 983 game's text renderer for Refresh (RTTI RFTextDraw :
// FrGraphicInterface, vtable 0xd13d08, size 0x2c from RFTextManager's
// operator new; constructor 0x602490(WView*), destructor 0x6024f0). It is
// Fresh's drawing front end (FrGraphicInterface, built on the game's WView
// 0xf023e8) under another vtable; the text goes through the JP Fresh's
// wide-text members (GetTextExtend/GetTextExtend11 on wchar_t, WFont::Print
// on wchar_t: declared under JP_PAKS in frgraphicinterface.h / wfont.h).
#ifndef PG_JP_RFTEXTDRAW_H
#define PG_JP_RFTEXTDRAW_H

#include <stdint.h>
#include <string>

#include "defines.h"
#include "frgraphicinterface.h"
#include "RFSize.h"

class RFVector2;
class WFont;
struct FontInfo;

// size 0x2c (RFTextManager's operator new): FrGraphicInterface's members
// only, no member of its own.
class RFTextDraw : public FrGraphicInterface {
public:
	// 0x602490: FrGraphicInterface(view) (0xbec520), then Reset (0xbec4b0).
	explicit RFTextDraw(WView* view);
	// [0] 0x6025f0 scalar deleting destructor (body 0x6024f0, tail-calls
	// ~FrGraphicInterface 0xbec2f0).
	virtual ~RFTextDraw();

	// 0x602a40: the extent of a text (RefreshInterface slot 2 passes its
	// FontInfo): the 12 font's width above size 11, the 11 font's below;
	// the height is GetFontHeight's (0xbec420).
	RFSize MeasureText(const FontInfo* font, const std::wstring& text);

	// 0x602bd0 / 0x602c10: DrawFontText with m_pFont12 (large) or m_pFont11
	// (small) at (x, y); the width it returns is dropped.
	void DrawTextLarge(float x, float y, const std::wstring& text, uint32_t style, uint32_t color,
		uint32_t outlineColor, uint32_t shadowColor);
	void DrawTextSmall(float x, float y, const std::wstring& text, uint32_t style, uint32_t color,
		uint32_t outlineColor, uint32_t shadowColor);

private:
	// 0x602500: the alpha byte of a colour, over 255.
	float GetColorAlpha(uint32_t color);
	// 0x602690: the text in font at pos with the style bits 0x20 (shadow),
	// 0x10 (outline) and 0x1 (bold), then the text itself; returns its
	// width (the last Print's) after flushing the font.
	float DrawFontText(WFont* font, const RFVector2& pos, const std::wstring& text, uint32_t style,
		uint32_t color, uint32_t outlineColor, uint32_t shadowColor);
};

CHECK_SIZE(RFTextDraw, 0x2c);

#endif
