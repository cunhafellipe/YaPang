// Original: client\Refresh\include\RFListSC.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFLISTSC_H
#define PG_CLIENT_REFRESH_INCLUDE_RFLISTSC_H

#include "RFExport.h"
#include "RFList.h"
#include "RFScrollBar.h"
#include "RFWindowFactory.h"
#include "defines.h"
#include "padding.h"

// The layout data of a scrolling list (RTTI ".?AUListSCBase@@", vtable
// 0xd65aa0), read by RFListSC::Create (0x4500b0): the list's ListBase and the
// layout data of its two scroll bars.
struct ListSCBase : public ListBase {
	ListSCBase();                           // 0x43d4a0 (RFXmlLoader.cpp)

	// [1] 0x43d4c0 (the scrolling list's WndType)
	virtual WndType GetType() const { return WndType_ListSC; }

	WndBaseStruct* m_hScrollBar;            // 0x1d4 (RFListSC::m_hScrollBar)
	WndBaseStruct* m_vScrollBar;            // 0x1d8 (RFListSC::m_vScrollBar)
};
CHECK_SIZE(ListSCBase, 0x1dc);
CHECK_FIELD(ListSCBase, m_hScrollBar, 0x1d4);
CHECK_FIELD(ListSCBase, m_vScrollBar, 0x1d8);

// The scrolling list: RFList with the scroll bars (vtable 0xd63bf4, 40 slots;
// 0–36 as RFList, the scroll handlers 37–39 are new), size 0x1e8 (from its
// scalar deleting destructor). dllexport as in the original library: cl 16
// then emits the implicit copy constructor (0x415560) and copy assignment
// (0x4155b0).
class RF_API RFListSC : public RFList {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe47054 that GetTypeName returns.
	static std::wstring const TypeName;

	// [0], [2] and the scroll bar getters are header inlines: their copies sit
	// in the COMDAT run of the implicit copy constructor, operator= and ??_E.
	virtual wchar_t const* ToString() const { return L"RFListSC"; } // [0] 0x415520
	virtual ~RFListSC();                                      // [1]  scalar deleting destructor 0x415620, ~RFListSC 0x4500a0
	virtual std::wstring const& GetTypeName() { return TypeName; } // [2] 0x415530
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x4500b0
	virtual bool HitTest(RFVector2 const&, bool);             // [6]  0x4505a0

	RFScrollBar* GetHScrollBar() { return m_hScrollBar; }     // 0x415540
	RFScrollBar* GetVScrollBar() { return m_vScrollBar; }     // 0x415550
	RFListSC();                                               //      0x450080
	void LockScrollbar(RFSCROLLBAR_TYPE, bool);               //      0x450640
	bool IsScrollBarAvailable(RFSCROLLBAR_TYPE) const;        //      0x4506d0
	void ScrollToTop();                                       //      0x4507b0
	void ScrollToBottom();                                    //      0x4507f0
	void ScrollToItem(int32_t);                               //      0x450830
	void SetScrollSize(uint32_t);                             //      0x450b70

	// The m_scrollSize that means "the list's own item size" (0xd64cc8).
	static uint32_t const PAGE = (uint32_t)-1;

protected:
	virtual void PreInitialize();                             // [9]  0x450270
	virtual void OnMouseWheel(MouseEventArgs&);               // [21] 0x450c30
	virtual void OnUpdate(UpdateEventArgs&);                  // [31] 0x450b40
	virtual void OnItemAdded();                               // [35] 0x450b20
	virtual void OnItemRemoved();                             // [36] 0x450b30
	virtual void OnHScrollPosChanged(EventArgs&);             // [37] 0x450da0
	virtual void OnVScrollPosChanged(EventArgs&);             // [38] 0x450de0
	virtual void OnScrollDraw(EventArgs&);                    // [39] 0x450e20

	void UpdateScrollbar();                                   //      0x450930
	float GetScrollSize();                                    //      0x450b80

private:
	RFScrollBar* m_hScrollBar;              // 0x1dc
	RFScrollBar* m_vScrollBar;              // 0x1e0
	uint32_t m_scrollSize;                  // 0x1e4, PAGE: the list's own item size
};
CHECK_SIZE(RFListSC, 0x1e8);
BEGIN_LAYOUT_CHECK(RFListSC)
	CHECK_FIELD(RFListSC, m_hScrollBar, 0x1dc);
	CHECK_FIELD(RFListSC, m_vScrollBar, 0x1e0);
	CHECK_FIELD(RFListSC, m_scrollSize, 0x1e4);
END_LAYOUT_CHECK

// The factory RFWindowFactoryManager creates scrolling lists with (vtable
// 0xd64f64, 3 slots; slot 0, the scalar deleting destructor, is 0x433110).
class RFListSCFactory : public RFWindowFactory {
public:
	RFListSCFactory(); // 0x433040 (RFListSC.cpp, beside Create)
	virtual RFWnd* Create();             // [1] 0x433090
	virtual void Destroy(RFWnd*);        // [2] 0x4330f0
};

#endif
