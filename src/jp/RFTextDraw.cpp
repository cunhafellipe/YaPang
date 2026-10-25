// The JP 983 game's RFTextDraw (RFTextDraw.h), from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>).
#include "RFTextDraw.h"

#include "RFLabel.h"
#include "RFVector2.h"
#include "wfont.h"

// 0xe13328: the eight (dx, dy) pixel offsets of the outline pass, a
// writable int[8][2] read by DrawFontText (initial values read with
// node scripts/disasm.ts --jp --data 0xe13328 16 --as i32).
int g_rfTextDrawOutlineOffsets[8][2] = {
	{0, -1}, {-1, 0}, {1, 0}, {0, 1},
	{-1, -1}, {1, -1}, {-1, 1}, {1, 1},
};
// 0xd08974: the shadow offset of an outlined text; DrawFontText loads it
// with fld dword ptr [0xd08974] (0x602712), so a float: 1.5f (0x3fc00000).
extern const float g_rfTextDrawOutlinedShadowOffset;
const float g_rfTextDrawOutlinedShadowOffset = 1.5f;

// 0x602490
RFTextDraw::RFTextDraw(WView* view)
	: FrGraphicInterface(view) {
	Reset();
}

// 0x6024f0 (the scalar deleting destructor 0x6025f0 wraps it): restores the
// vtable and tail-calls ~FrGraphicInterface 0xbec2f0.
RFTextDraw::~RFTextDraw() {
}

// 0x602500
float RFTextDraw::GetColorAlpha(uint32_t color) {
	return (float)((color >> 24) / 255.0);
}

// 0x602690
float RFTextDraw::DrawFontText(WFont* font, const RFVector2& pos, const std::wstring& text, uint32_t style,
	uint32_t color, uint32_t outlineColor, uint32_t shadowColor) {
	if (font == NULL) {
		return 0.0f;
	}
	float x = pos.x;
	float y = pos.y;
	bool bold = (style & 1) != 0;
	bool outline = ((style >> 4) & 1) != 0;
	m_alpha = GetColorAlpha(color);

	if (((style >> 5) & 1) != 0) {
		float shadowAlpha = (float)((shadowColor >> 24) / 255.0);
		float offset = 1.0f;
		if (outline) {
			offset = g_rfTextDrawOutlinedShadowOffset;
		}
		if (bold) {
			offset = (float)(offset + 0.5);
		}
		if (m_alpha > 0.0f && shadowAlpha > 0.0f) {
			float ratio = shadowAlpha < m_alpha ? shadowAlpha / m_alpha : m_alpha / shadowAlpha;
			font->Print(m_pView, x + offset, y + offset, text.c_str(), 0x80000,
				FrALPHA(shadowColor, ratio), NULL);
		}
	}

	if (outline) {
		float outlineAlpha = (float)((outlineColor >> 24) / 255.0);
		if (m_alpha > 0.0f && outlineAlpha > 0.0f) {
			for (int i = 0; i < 8; i++) {
				int boldShift = 0;
				if (bold && g_rfTextDrawOutlineOffsets[i][0] == 1) {
					boldShift = 1;
				}
				float ratio = m_alpha > outlineAlpha ? outlineAlpha / m_alpha : m_alpha / outlineAlpha;
				font->Print(m_pView, x + g_rfTextDrawOutlineOffsets[i][0] + boldShift,
					y + g_rfTextDrawOutlineOffsets[i][1], text.c_str(), 0x80000,
					FrALPHA(outlineColor, ratio), NULL);
			}
		}
	}

	if (bold) {
		font->Print(m_pView, (float)(x + 1.0), y, text.c_str(), 0x80000, FrALPHA(color, m_alpha), NULL);
	}

	float width = font->Print(m_pView, x, y, text.c_str(), 0x80000, FrALPHA(color, m_alpha), NULL);
	// The JP WFont's [10] (the KR [9] Flush, one slot later in the JP vtable).
	font->Flush(m_pView);
	return width;
}

// 0x602a40
RFSize RFTextDraw::MeasureText(const FontInfo* font, const std::wstring& text) {
	float width;
	if (font->size > 11) {
		width = GetTextExtend(text.c_str());
	} else {
		width = GetTextExtend11(text.c_str());
	}
	return RFSize(width, (float)GetFontHeight());
}

// 0x602bd0
void RFTextDraw::DrawTextLarge(float x, float y, const std::wstring& text, uint32_t style, uint32_t color,
	uint32_t outlineColor, uint32_t shadowColor) {
	DrawFontText(m_pFont12, RFVector2(x, y), text, style, color, outlineColor, shadowColor);
}

// 0x602c10
void RFTextDraw::DrawTextSmall(float x, float y, const std::wstring& text, uint32_t style, uint32_t color,
	uint32_t outlineColor, uint32_t shadowColor) {
	DrawFontText(m_pFont11, RFVector2(x, y), text, style, color, outlineColor, shadowColor);
}
