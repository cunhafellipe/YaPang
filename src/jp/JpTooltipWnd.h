// The JP 983 game's tooltip windows (RTTI RFTooltip : RFFormBase<RFWndEx> :
// RFWndEx; RFCommonTooltip and RFCommonTooltipByImage : RFTooltip), loaded
// from tooltip_common_wnd.xml / tooltip_common_wnd2.xml and shown by
// RFTooltipHelper (m_tooltips[1] / [2]). Declarations and the JP x86 layout;
// the members not written in JpTooltipWnd.cpp are declared JP dependencies.
#ifndef PG_JP_JPTOOLTIPWND_H
#define PG_JP_JPTOOLTIPWND_H

#include <stdint.h>
#include <string>

#include "defines.h"
#include "padding.h"
#include "RFEventArgs.h"
#include "RFManager.h"
#include "RFRect.h"
#include "RFSize.h"
#include "RFVector2.h"
#include "RFWndEx.h"
#include "RFSceneManager.h"
#include "RFFormBase.h"

class FrGraphicInterface;
class RFPicture;
class RFTextArea;

// 0xe3375c (name ours, 1 in the exe's data): RFTooltip::OnTooltipUpdate
// does nothing while it is clear.
extern bool g_jpTooltipFollowEnabled;

// 0xe73dd8 (name ours, 0 in the exe's data): how the game's window creators
// (the tooltips' Load, CreateServerNoticeDialog 0x5d7bd0) make their window:
// 0 CreateWnd under the given parent, 1 CreateWnd with no parent,
// 2 CreateDesktop.
extern int g_jpWndCreateMode;

// The tooltip window (RTTI RFTooltip, vtable 0xd108a4, 38 slots: [34]-[37]
// new, [36] pure), size 0x1bc. Its children (+0x1b0 .. +0x1b8) are found by
// name in the derived classes' Attach.
class RFTooltip : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5ce0c0: RFWndEx::RFWndEx, then the members (m_followMouse last).
	RFTooltip();
	// [1] 0x5ce410 (scalar deleting destructor); ~RFTooltip 0x5ce150
	// restores RFFormBase<RFWndEx>'s vtable, then ~RFWndEx.
	virtual ~RFTooltip();

protected:
	// [10] 0x5cedb0: registers OnTooltipUpdate (Subscriber through the
	// vcall thunk 0x5ce0b0, MemberFunctionFunctor<RFTooltip> 0xd10940) for
	// RFWnd::EventUpdate on L"", then m_freshLayoutData from the JP Fresh
	// manager (0xf022d0 +0x40, its +0x84) when there is one.
	virtual void OnInitialUpdate();

public:
	// [34] 0x5ce160 (names ours): places the tooltip for owner: below its
	// screen position (x + 0.7 of its width, y + height + 2), or, when it
	// follows the mouse, past the root window's size; no owner sets
	// m_followMouse.
	// RFCommonTooltip / RFCommonTooltipByImage override it (find their
	// children, then call this one). style is not used here.
	virtual void Attach(RFWnd* owner, int option, int style);
	// [35] 0x5ce010: m_shown = shown.
	virtual void SetShown(bool shown);
	// [36] _purecall: sizes the frame to the text (and the picture).
	virtual void UpdateLayout() = 0;
	// [37] 0x5ce8c0: the EventUpdate handler; while m_shown (and
	// g_jpTooltipFollowEnabled), hides itself when the mouse is over
	// another window than the owner and follows the mouse (kept inside the
	// root window); then shows or hides the window (ShowActivated) to
	// match m_shown.
	virtual void OnTooltipUpdate(EventArgs& args);

	// 0x5ce060 (name ours): the text area's text (its [34] SetText, then
	// FitToContent(false)), then UpdateLayout.
	void SetText(std::wstring const& text);
	// 0x5cf080 (name ours): the picture's image (SetPicture, FitToContent),
	// then the same as SetText.
	void SetImageAndText(std::wstring const& image, std::wstring const& text);

protected:
	// 0x5ce020 (name ours): the Fresh font's height for fontSize (above 11
	// GetFontHeight, else GetFontHeight11), 0 without m_freshLayoutData.
	float GetLineHeight(int32_t fontSize);
	// 0x5ceec0 (name ours): the width of one line of text in the font
	// size fontSize: the Fresh text extent (above 11 GetTextExtend, else
	// GetTextExtend11) of its Shift-JIS form; 0 without m_freshLayoutData.
	float GetLineWidth(std::wstring const& line, int32_t fontSize);
	// 0x5cf390 (name ours): the size of text in the font size fontSize,
	// line by line: the widest line, and per line the line height, plus
	// the text area's GetTextHeight less the line height between lines.
	RFSize MeasureText(std::wstring const& text, int32_t fontSize);

protected:
	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	bool m_shown;                                            // 0x184 SetShown; OnTooltipUpdate works only while set
	PADDING(0x185, 0x188);
	FrGraphicInterface* m_freshLayoutData;                   // 0x188 OnInitialUpdate (Fresh layout owner +0x40 → +0x84); UpdateLayout tests it
	RFVector2 m_position;                                    // 0x18c the screen position Attach computed
	RFRect m_ownerRect;                                      // 0x194 the owner's screen rect
	bool m_followMouse;                                      // 0x1a4 true at construction and with no owner: Attach parks the window off-screen, OnTooltipUpdate then moves it to the mouse
	PADDING(0x1a5, 0x1a8);
	RFWnd* m_owner;                                          // 0x1a8 the window it is shown for
	int m_option;                                            // 0x1ac Attach's option
	RFWnd* m_frame;                                          // 0x1b0 a child (GetChild) UpdateLayout sizes
	RFTextArea* m_textArea;                                  // 0x1b4 a child (dynamic_cast)
	RFPicture* m_picture;                                    // 0x1b8 a child (dynamic_cast), RFCommonTooltipByImage's image
};
CHECK_SIZE(RFTooltip, 0x1bc);
BEGIN_LAYOUT_CHECK(RFTooltip)
	CHECK_FIELD(RFTooltip, m_shown, 0x184);
	CHECK_FIELD(RFTooltip, m_freshLayoutData, 0x188);
	CHECK_FIELD(RFTooltip, m_position, 0x18c);
	CHECK_FIELD(RFTooltip, m_ownerRect, 0x194);
	CHECK_FIELD(RFTooltip, m_followMouse, 0x1a4);
	CHECK_FIELD(RFTooltip, m_owner, 0x1a8);
	CHECK_FIELD(RFTooltip, m_option, 0x1ac);
	CHECK_FIELD(RFTooltip, m_frame, 0x1b0);
	CHECK_FIELD(RFTooltip, m_textArea, 0x1b4);
	CHECK_FIELD(RFTooltip, m_picture, 0x1b8);
END_LAYOUT_CHECK

// The text tooltip (tooltip_common_wnd.xml; RTTI RFCommonTooltip, vtable
// 0xd1095c), size 0x1bc. Its constructor is inlined in the creators
// (0x5ce490 CreateWnd<RFCommonTooltip>, 0x5ce530 the CreateDesktop one).
class RFCommonTooltip : public RFTooltip {
public:
	RFCommonTooltip() {
	}
	// [1] 0x5ce380: scalar deleting destructor (vtable, then ~RFTooltip).
	virtual ~RFCommonTooltip();
	// [34] 0x5cf700: finds its children (0x5cf1f0, GetChild,
	// dynamic_casts), shows the window, then RFTooltip::Attach.
	virtual void Attach(RFWnd* owner, int option, int style);
	// [36] 0x5cf9d0
	virtual void UpdateLayout();

	// 0x5ceb30 (name ours; ecx = parent): creates the window from
	// tooltip_common_wnd.xml by g_jpWndCreateMode, then keeps parent.
	static RFCommonTooltip* Load(RFWnd* parent);

private:
	// 0x5cf1f0 (name ours; ecx = this, edx = name): the child named name
	// shown or hidden.
	void ShowChild(wchar_t const* name, bool show);

public:

	static RFWnd* s_loadParent;                              // 0xe78180 Load's last parent
};
CHECK_SIZE(RFCommonTooltip, 0x1bc);

// The image and text tooltip (tooltip_common_wnd2.xml; RTTI
// RFCommonTooltipByImage, vtable 0xd109fc), size 0x1bc. Creators 0x5ce5c0
// (CreateWnd) and 0x5ce660 (CreateDesktop).
class RFCommonTooltipByImage : public RFTooltip {
public:
	RFCommonTooltipByImage() {
	}
	// [1] 0x5ce3e0: scalar deleting destructor (vtable, then ~RFTooltip).
	virtual ~RFCommonTooltipByImage();
	// [34] 0x5cfab0: finds its children (0x5cf2c0, GetChild,
	// dynamic_casts), shows them, then RFTooltip::Attach.
	virtual void Attach(RFWnd* owner, int option, int style);
	// [36] 0x5cfcb0
	virtual void UpdateLayout();

	// 0x5cec70 (name ours; ecx = parent): creates the window from
	// tooltip_common_wnd2.xml by g_jpWndCreateMode, then keeps parent.
	static RFCommonTooltipByImage* Load(RFWnd* parent);

private:
	// 0x5cf2c0 (name ours; ecx = this, edx = name): RFCommonTooltip's
	// 0x5cf1f0, the same code.
	void ShowChild(wchar_t const* name, bool show);

public:

	static RFWnd* s_loadParent;                              // 0xe78188 Load's last parent
};
CHECK_SIZE(RFCommonTooltipByImage, 0x1bc);

#endif
