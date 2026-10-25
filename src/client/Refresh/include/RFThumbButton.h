// Original: client\Refresh\include\RFThumbButton.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFTHUMBBUTTON_H
#define PG_CLIENT_REFRESH_INCLUDE_RFTHUMBBUTTON_H

#include "RFButton.h"
#include "RFRect.h"
#include "RFWnd.h"
#include "defines.h"

// The draggable thumb of a scroll bar or a slider: an RFButton that keeps
// itself on its track while dragged (vtable 0xd65dbc, 35 slots, the RFButton
// ones with slots 0, 1, 4, 9 and 20 replaced). Not exported: RFScrollBar::Create
// 0x428f30 and RFSlider::Create 0x44edb0 new it (0x208) and call the
// constructor 0x45a490 directly; its destructor slot is a scalar deleting
// destructor (0x45a510, tests bit 0 only), not the ??_E of the dllexport
// classes. The owner's PreInitialize copies its m_horizontal and m_trackRect
// into the thumb (RFSlider 0x44ef60, RFScrollBar 0x429110 / OnLayoutChanged
// 0x429390), the inlined form of SetTrack.
class RFThumbButton : public RFButton {
	LAYOUT_CHECK_FRIEND;
	// RFSlider::OnThumbMouseMove 0x44f8e0 reads its m_lButtonDown through
	// its RFThumbButton* m_thumb.
	friend class RFSlider;

public:
	RFThumbButton();                                          //      0x45a490

	virtual wchar_t const* ToString() const;                  // [0]  0x45a480 (L"RFThumbButton")
	virtual ~RFThumbButton();                                 // [1]  scalar deleting destructor 0x45a510, ~RFThumbButton 0x45a540
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x45a550 (calls RFButton::Create)

	// Unreferenced in the exe (the owners inline it); the name is ours.
	void SetTrack(bool horizontal, RFRect const& track);      //      0x45a580
	// Whether the normal-state image is a nine-patch, so the thumb can be
	// stretched. Unreferenced in the exe (RFScrollBar::SetScrollRange 0x429670
	// inlines it); the name is ours.
	bool HasNinePatchImage() const;                           //      0x45a7d0
	// Whether the left button went down on the thumb (RFWnd::m_lButtonDown):
	// the drag test of the owners' OnThumbMouseMove, which inline it. Its
	// out-of-line copy 0x428e20 (this in eax, LTCG) lies in RFScrollBar.cpp's
	// address run, not exported (the class is not dllexport); the name is ours.
	bool IsLButtonDown() const;

protected:
	virtual void PreInitialize();                             // [9]  0x45a560 (RFButton::PreInitialize, then m_dragable = true)
	virtual void OnMouseMove(MouseEventArgs&);                // [20] 0x45a5b0

private:
	// Which axis the thumb moves on: x between m_trackRect.left/right when
	// set (OnMouseMove), y between m_trackRect.top/bottom otherwise. The
	// constructor sets it.
	bool m_horizontal;                      // 0x1f4
	// The track in the owner's coordinates (RFRect is left, right, top,
	// bottom); OnMouseMove clamps the dragged position to it. Zero after
	// construction.
	RFRect m_trackRect;                     // 0x1f8
};
CHECK_SIZE(RFThumbButton, 0x208);
BEGIN_LAYOUT_CHECK(RFThumbButton)
	CHECK_FIELD(RFThumbButton, m_horizontal, 0x1f4);
	CHECK_FIELD(RFThumbButton, m_trackRect, 0x1f8);
END_LAYOUT_CHECK

#endif
