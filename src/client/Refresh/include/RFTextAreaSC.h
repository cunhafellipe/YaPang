// Original: client\Refresh\include\RFTextAreaSC.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFTEXTAREASC_H
#define PG_CLIENT_REFRESH_INCLUDE_RFTEXTAREASC_H

#include "RFScrollBar.h"
#include "RFExport.h"
#include "RFTextArea.h"
#include "RFWindowFactory.h"
#include "defines.h"

// The layout data of a text area with scroll bars (RTTI
// ".?AUTextAreaSCBase@@", vtable 0xd65a88), read by RFTextAreaSC::Create
// (0x44d6f0): the text area's data, the layout data of its two scroll bars
// and the scroll bar options.
struct TextAreaSCBase : public TextAreaBase {
	// The scroll bar data null, the rectangle zero; the bools are left
	// uninitialized.
	TextAreaSCBase();                       // 0x43d1e0 (RFXmlLoader.cpp)

	// [1] 0x43d220 (the scrolling text area's WndType)
	virtual WndType GetType() const { return WndType_TextAreaSC; }

	WndBaseStruct* m_hScrollBar;            // 0x2ec (RFTextAreaSC::m_hScrollBar)
	WndBaseStruct* m_vScrollBar;            // 0x2f0 (RFTextAreaSC::m_vScrollBar)
	bool m_useHScrollBar;                   // 0x2f4
	bool m_enableVScrollBar;                // 0x2f5
	bool m_unusedScrollFlag;                // 0x2f6
	bool m_autoShowVScrollBar;              // 0x2f7
	RFRect m_scrollBarRect;                 // 0x2f8
};
CHECK_SIZE(TextAreaSCBase, 0x308);
CHECK_FIELD(TextAreaSCBase, m_hScrollBar, 0x2ec);
CHECK_FIELD(TextAreaSCBase, m_vScrollBar, 0x2f0);
CHECK_FIELD(TextAreaSCBase, m_useHScrollBar, 0x2f4);
CHECK_FIELD(TextAreaSCBase, m_enableVScrollBar, 0x2f5);
CHECK_FIELD(TextAreaSCBase, m_unusedScrollFlag, 0x2f6);
CHECK_FIELD(TextAreaSCBase, m_autoShowVScrollBar, 0x2f7);
CHECK_FIELD(TextAreaSCBase, m_scrollBarRect, 0x2f8);

// RFTextArea with scrollbars: vtable 0xd63f8c, 42 slots (the scroll
// handlers 39–41 are new). Size 0x294 (the scalar deleting destructor
// 0x415e90).
class RF_API RFTextAreaSC: public RFTextArea {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe46fe4 that GetTypeName returns.
	static std::wstring const TypeName;

	virtual wchar_t const* ToString() const { return L"RFTextAreaSC"; } // [0] 0x415ce0
	virtual ~RFTextAreaSC();                                  // [1]  scalar deleting destructor 0x415e90, ~RFTextAreaSC 0x44d6e0
	virtual std::wstring const& GetTypeName() { return TypeName; } // [2] 0x415cf0
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x44d6f0
	virtual bool HitTest(RFVector2 const&, bool);             // [6]  0x44dc10

protected:
	virtual void PreInitialize();                             // [9]  0x44d910
	virtual void OnLayoutChanged(uint32_t);                   // [14] 0x44dc90
	virtual void OnMouseWheel(MouseEventArgs&);               // [21] 0x44de30
	virtual void OnKeyDown(KeyEventArgs&);                    // [24] 0x44dce0
	virtual void OnChar(CharEventArgs&);                      // [26] 0x44dcc0
	virtual void OnUpdate(UpdateEventArgs&);                  // [31] 0x44dde0

public:
	virtual void SetText(std::wstring const&);                // [34] 0x44df00
	virtual void AddText(std::wstring const&);                // [35] 0x44df30
	virtual void AddPlainText(std::wstring const&);           // [36] 0x44df70
	virtual void ClearText();                                 // [38] 0x44dfb0

protected:
	virtual void OnHScrollPosChanged(EventArgs&);             // [39] 0x44e460
	virtual void OnVScrollPosChanged(EventArgs&);             // [40] 0x44e470
	virtual void OnScrollDraw(EventArgs&);                    // [41] 0x44e4b0

	void StickToBottomProcess();                              //      0x44e170
	void UpdateScrollbar();                                   //      0x44e220

public:
	void StickToBottom(bool stick) { m_stickToBottom = stick; } // 0x415d00
	RFScrollBar* GetHScrollBar() { return m_hScrollBar; }     // 0x415f00
	RFScrollBar* GetVScrollBar() { return m_vScrollBar; }     // 0x415f10
	RFTextAreaSC();                                           //      0x44d6a0
	void LockScrollbar(RFSCROLLBAR_TYPE, bool);               //      0x44dfe0
	bool IsScrollBarAvailable(RFSCROLLBAR_TYPE);              //      0x44e070
	void ScrollToTop();                                       //      0x44e130
	void ScrollToBottom();                                    //      0x44e150

private:
	RFScrollBar* m_hScrollBar;              // 0x274
	RFScrollBar* m_vScrollBar;              // 0x278
	// The scroll bar options Create copies from WndBaseStruct+0x2f4..0x2f7.
	bool m_useHScrollBar;                   // 0x27c  0: PreInitialize hides the horizontal bar
	bool m_enableVScrollBar;                // 0x27d  1: UpdateScrollbar enables the vertical bar
	bool m_unusedScrollFlag;                // 0x27e  only Create copies it
	bool m_autoShowVScrollBar;              // 0x27f  1: UpdateScrollbar shows the vertical bar and moves the rect
	RFRect m_scrollBarRect;                 // 0x280  offset added to the window rect by UpdateScrollbar
	bool m_stickToBottom;                   // 0x290  set by PreInitialize, read by StickToBottomProcess
};
CHECK_SIZE(RFTextAreaSC, 0x294);
BEGIN_LAYOUT_CHECK(RFTextAreaSC)
	CHECK_FIELD(RFTextAreaSC, m_hScrollBar, 0x274);
	CHECK_FIELD(RFTextAreaSC, m_vScrollBar, 0x278);
	CHECK_FIELD(RFTextAreaSC, m_useHScrollBar, 0x27c);
	CHECK_FIELD(RFTextAreaSC, m_enableVScrollBar, 0x27d);
	CHECK_FIELD(RFTextAreaSC, m_unusedScrollFlag, 0x27e);
	CHECK_FIELD(RFTextAreaSC, m_autoShowVScrollBar, 0x27f);
	CHECK_FIELD(RFTextAreaSC, m_scrollBarRect, 0x280);
	CHECK_FIELD(RFTextAreaSC, m_stickToBottom, 0x290);
END_LAYOUT_CHECK

// The factory RFManager creates text areas with (unexported; slot 0, the
// scalar deleting destructor, is 0x433990).
class RFTextAreaSCFactory : public RFWindowFactory {
public:
	RFTextAreaSCFactory(); // 0x433870 (RFTextAreaSC.cpp, beside Create)
	virtual RFWnd* Create();                // [1] 0x4338c0
	virtual void Destroy(RFWnd*);           // [2] 0x433970
};

#endif
