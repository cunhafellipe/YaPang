// The JP 983 game's mastery tooltip (RTTI RFMasteryToolTipWnd :
// RFFormBase<RFWndEx> : RFWndEx), loaded from kam_mastery_tooltip.xml by
// RFMasteryToolTipWnd::Load (0x5d76c0) for the mastery card (0x5bbe70),
// tile (0x5c5270) and stat detail (0x5cbcd0) windows, which keep it, give
// it its text (SetText), place it (SetPosition) and show it ([34] Show).
// Declarations and the JP x86 layout, from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>).
#ifndef PG_JP_JPMASTERYTOOLTIPWND_H
#define PG_JP_JPMASTERYTOOLTIPWND_H

#include <stdint.h>
#include <string>

#include "defines.h"
#include "padding.h"
#include "RFTextArea.h"
#include "RFWndEx.h"
#include "RFFormBase.h"
#include "RefreshStringUtil.h"

// The mastery tooltip (vtable 0xd115a4, 35 slots: [34] new), size 0x188:
// one text area ("message") sized to its text.
class RFMasteryToolTipWnd : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5d71e0: RFWndEx::RFWndEx, then m_textArea = NULL.
	RFMasteryToolTipWnd();
	// [1] 0x5d7440 (scalar deleting destructor); ~RFMasteryToolTipWnd
	// 0x5d7200 restores RFFormBase<RFWndEx>'s vtable, then ~RFWndEx.
	virtual ~RFMasteryToolTipWnd();

protected:
	// [10] 0x5d7840: m_textArea = the "message" child (dynamic_cast); the
	// base's OnInitialUpdate is not called.
	virtual void OnInitialUpdate();

public:
	// [34] 0x5d7800 (name ours): with a text area, sizes it to its text
	// (FitToText), the window to the text area, then ShowWindow(show).
	virtual void Show(bool show);

	// 0x5d7210 (name ours): SetScreenPos(x, y) with x and y clamped to 0
	// and kept inside the game's view (g_view's width and height less the
	// window's size).
	void SetPosition(float x, float y);
	// 0x5d7300 (name ours): the text area's colour (RFTextArea::SetColor,
	// inlined in the JP exe).
	void SetTextColor(uint32_t color);

	// 0x5b9db0 (name ours; the copy sits in the mastery card unit, called
	// from the card and tile windows): the text area's text ([34] SetText),
	// widened from Shift-JIS.
	void SetText(char const* text) {  // 0x5b9db0
		if (m_textArea != NULL) {
			m_textArea->SetText(ShiftJisToWString(std::string(text)));
		}
	}
	// 0x5c8330 (name ours; the copy sits in the mastery stat unit, called
	// from the stat detail window): the text area's text ([34] SetText).
	void SetText(std::wstring const& text) {  // 0x5c8330
		if (m_textArea != NULL) {
			m_textArea->SetText(text.c_str());
		}
	}

	// 0x5d76c0 (name ours; ecx = parent): creates the window from
	// kam_mastery_tooltip.xml by g_jpWndCreateMode (0 CreateWnd under
	// parent, 1 CreateWnd with no parent, 2 CreateDesktopWnd), then keeps
	// parent in s_loadParent.
	static RFMasteryToolTipWnd* Load(RFWnd* parent);

	static RFWnd* s_loadParent;                              // 0xe781c4 Load's last parent

private:
	// 0x5d7490 (name ours): sizes the text area to its text: 12 per
	// character of the longest line (counted up to and with its line
	// break) and 19 per line, plus the text area's padding; the width is
	// at most 300.
	void FitToText();

	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	RFTextArea* m_textArea;                                  // 0x184 the "message" child (OnInitialUpdate)
};
CHECK_SIZE(RFMasteryToolTipWnd, 0x188);
BEGIN_LAYOUT_CHECK(RFMasteryToolTipWnd)
	CHECK_FIELD(RFMasteryToolTipWnd, m_textArea, 0x184);
END_LAYOUT_CHECK

#endif // PG_JP_JPMASTERYTOOLTIPWND_H
