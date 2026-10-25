// Original: client\Refresh\include\RFSlider.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFSLIDER_H
#define PG_CLIENT_REFRESH_INCLUDE_RFSLIDER_H

#include <stdint.h>

#include <string>

#include "RFExport.h"
#include "RFPoint.h"
#include "RFRect.h"
#include "RFThumbButton.h"
#include "RFWindowFactory.h"
#include "RFWnd.h"

#include "defines.h"
#include "padding.h"

class RFVector2;

// The layout data of a slider, read by RFSlider::Create (its RTTI name
// ".?AUSliderBase@@", type descriptor 0xe476a4): the window's WndBaseStruct
// followed by the data of the three children and the track.
struct SliderBase : public WndBaseStruct {
	SliderBase();                           // 0x4402c0 (RFXmlLoader.cpp)

	virtual WndType GetType() const { return WndType_Slider; } // [1] 0x43c560

	WndBaseStruct* m_ltButton;              // 0x17c  the left/top button's data
	WndBaseStruct* m_rbButton;              // 0x180  the right/bottom button's data
	WndBaseStruct* m_thumb;                 // 0x184  the thumb's data
	RFRect m_trackRect;                     // 0x188  copied to RFSlider::m_trackRect
	bool m_horizontal;                      // 0x198  copied to RFSlider::m_horizontal
};
CHECK_SIZE(SliderBase, 0x19c);  // RFXmlLoader::LoadElement 0x440cb0 allocates 0x19c
CHECK_FIELD(SliderBase, m_ltButton, 0x17c);
CHECK_FIELD(SliderBase, m_rbButton, 0x180);
CHECK_FIELD(SliderBase, m_thumb, 0x184);
CHECK_FIELD(SliderBase, m_trackRect, 0x188);
CHECK_FIELD(SliderBase, m_horizontal, 0x198);

// A Refresh slider: RFWnd plus the thumb/level handlers (vtable 0xd63e54,
// 37 slots; 0–33 as RFWnd, the slider handlers 34–36 are new); size 0x1ac
// (the factory's new, sub_4335e0).
class RF_API RFSlider: public RFWnd {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe47038 that GetTypeName returns.
	static std::wstring const TypeName;
	// The event SetSlidePos, SetSlideRange and the handlers raise when the
	// position changes (the static const at 0xe4701c).
	static std::wstring const EventPosChanged;

	virtual wchar_t const* ToString() const;                  // [0]  0x416810
	virtual ~RFSlider();                                      // [1]  scalar deleting destructor 0x4169e0, ~RFSlider 0x44eda0
	virtual std::wstring const& GetTypeName();                // [2]  0x416820
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x44edb0

	RFSlider();                                               //      0x44ed60
	void SetSlideSize(int32_t);                               //      0x44f470
	void SetSlideRange(int32_t, int32_t);                     //      0x44f480
	void SetSlidePos(int32_t);                                //      0x44f530
	int32_t GetSlidePos() const;                              //      0x44f5f0
	void Next();                                              //      0x44f600
	void Prev();                                              //      0x44f660
	void SlideToTop();                                        //      0x44f6c0
	void SlideToBottom();                                     //      0x44f6d0

protected:
	virtual void PreInitialize();                             // [9]  0x44ef60
	virtual void OnEnabled(bool);                             // [12] 0x44f1e0
	virtual void OnLButtonDown(MouseEventArgs&);              // [15] 0x44f270
	virtual void OnLButtonUp(MouseEventArgs&);                // [16] 0x44f420
	virtual void OnMouseMove(MouseEventArgs&);                // [20] 0x44f6e0
	virtual void OnMouseWheel(MouseEventArgs&);               // [21] 0x44f870
	virtual void OnLTButClicked(EventArgs&);                  // [34] 0x44f9d0
	virtual void OnRBButClicked(EventArgs&);                  // [35] 0x44fa80
	virtual void OnThumbMouseMove(EventArgs&);                // [36] 0x44f8e0

	void PositionToLevel(RFVector2 const&);                   //      0x44fb30
	void LevelToThumbPosition();                              //      0x44fcb0

private:
	// The three children PreInitialize subscribes to: the two buttons'
	// EventClicked (OnLTButClicked, OnRBButClicked) and the thumb's
	// EventMouseMove (OnThumbMouseMove).
	RFWnd* m_ltButton;                      // 0x178
	RFWnd* m_rbButton;                      // 0x17c
	RFThumbButton* m_thumb;                 // 0x180
	// Minimum (SlideToTop, the lower clamp of SetSlidePos) and maximum
	// (SlideToBottom, the upper clamp), set by SetSlideRange.
	RFPoint m_slideRange;                   // 0x184
	int32_t m_slidePos;                     // 0x18c
	int32_t m_slideSize;                    // 0x190  the step of OnLTButClicked / OnRBButClicked
	bool m_dragging;                        // 0x194  cleared by OnLButtonUp
	// The track inside the slider (left/right for a horizontal one, top/bottom
	// for a vertical one, PositionToLevel); copied to the thumb's 0x1f8..0x204
	// by PreInitialize.
	RFRect m_trackRect;                     // 0x198
	bool m_horizontal;                      // 0x1a8  PositionToLevel works on x when set, y otherwise; copied to the thumb's 0x1f4
};
CHECK_SIZE(RFSlider, 0x1ac);
BEGIN_LAYOUT_CHECK(RFSlider)
	CHECK_FIELD(RFSlider, m_ltButton, 0x178);
	CHECK_FIELD(RFSlider, m_rbButton, 0x17c);
	CHECK_FIELD(RFSlider, m_thumb, 0x180);
	CHECK_FIELD(RFSlider, m_slideRange, 0x184);
	CHECK_FIELD(RFSlider, m_slidePos, 0x18c);
	CHECK_FIELD(RFSlider, m_slideSize, 0x190);
	CHECK_FIELD(RFSlider, m_dragging, 0x194);
	CHECK_FIELD(RFSlider, m_trackRect, 0x198);
	CHECK_FIELD(RFSlider, m_horizontal, 0x1a8);
END_LAYOUT_CHECK

// The factory RFFactoryManager creates sliders with.
class RFSliderFactory : public RFWindowFactory {
public:
	RFSliderFactory(); // 0x433590 (RFSlider.cpp, beside Create)
	virtual RFWnd *Create();          // [1] 0x4335e0
	virtual void Destroy(RFWnd *);    // [2] 0x433680
};

#endif
