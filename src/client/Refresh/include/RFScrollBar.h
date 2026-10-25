// Original: client\Refresh\include\RFScrollBar.h (JP 983 exe; path from the
// library's layout, docs/JP_REFRESH_UI.md)
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFSCROLLBAR_H
#define PG_CLIENT_REFRESH_INCLUDE_RFSCROLLBAR_H

#include <stdint.h>

#include "RFBezierSpline.h"
#include "RFExport.h"
#include "RFPoint.h"
#include "RFRect.h"
#include "RFSize.h"
#include "RFThumbButton.h"
#include "RFVector2.h"
#include "RFWindowFactory.h"
#include "RFWnd.h"
#include "defines.h"
#include "padding.h"

// Which scroll bars of a scrolled control (RFListSC, RFTextAreaSC) a call
// means: the values from their LockScrollbar switch (1 the horizontal bar,
// 2 the vertical one, 3 both); the enumerator names are ours.
enum RFSCROLLBAR_TYPE {
	RFSCROLLBAR_HORIZONTAL = 1,
	RFSCROLLBAR_VERTICAL = 2,
	RFSCROLLBAR_BOTH = 3
};

// The layout data of a scroll bar, read by RFScrollBar::Create 0x428f30 (its
// RTTI name ".?AUScrollbarBase@@", type descriptor 0xe47204): the window's
// WndBaseStruct followed by the data of the three children and the track.
struct ScrollbarBase : public WndBaseStruct {
	ScrollbarBase();                        // 0x440290 (RFXmlLoader.cpp)

	// [1] 0x43c550 (emitted with the struct's vtable 0xd65a58, in RFXmlLoader).
	virtual WndType GetType() const { return WndType_Scrollbar; }

	WndBaseStruct* m_ltButton;              // 0x17c  the left/top button's data
	WndBaseStruct* m_rbButton;              // 0x180  the right/bottom button's data
	WndBaseStruct* m_thumb;                 // 0x184  the thumb's data
	RFRect m_trackRect;                     // 0x188  copied to RFScrollBar::m_trackRect
	bool m_horizontal;                      // 0x198  copied to RFScrollBar::m_horizontal
};
CHECK_SIZE(ScrollbarBase, 0x19c);  // RFXmlLoader::LoadElement 0x440cb0 allocates 0x19c
CHECK_FIELD(ScrollbarBase, m_ltButton, 0x17c);
CHECK_FIELD(ScrollbarBase, m_rbButton, 0x180);
CHECK_FIELD(ScrollbarBase, m_thumb, 0x184);
CHECK_FIELD(ScrollbarBase, m_trackRect, 0x188);
CHECK_FIELD(ScrollbarBase, m_horizontal, 0x198);

// The smooth-scroll animation a scroll bar keeps at 0x1a4: a 600 ms ease
// along an RFBezierSpline from one scroll position to another; size 0x1c.
// Defined in RFScrollBar.cpp (0x428c00, 0x428cd0, 0x428cf0); the names are
// ours.
struct RFScrollAnimation {
	RFScrollAnimation();  // 0x428c00

	// Inlined in ~RFScrollBar 0x428ec0.
	~RFScrollAnimation() {  // 0x4151b0
		if (m_spline) {
			delete m_spline;
			m_spline = 0;
		}
	}

	// 0x428cd0 (inlined in the constructor and in OnLButtonDown 0x429480).
	void Stop();

	// 0x428cf0: advances the elapsed time and returns the position at it
	// (m_to, and inactive, once the duration has passed).
	int32_t Update(float elapsed);

	// Inlined in MoveToScroll 0x429930.
	void Reset() {
		m_elapsed = 0.0f;
		m_from = 0;
		m_progress = 0.0f;
		m_to = 0;
		m_duration = 600;
	}

	// Inlined in MoveToScroll 0x429930.
	void Start(int32_t const& from, int32_t const& to) {
		Reset();
		m_from = from;
		m_active = true;
		m_to = to;
	}

	bool m_active;               // 0x0
	uint32_t m_duration;         // 0x4  milliseconds
	int32_t m_from;              // 0x8
	int32_t m_to;                // 0xc
	float m_elapsed;             // 0x10
	float m_progress;            // 0x14
	RFBezierSpline* m_spline;    // 0x18
};
CHECK_SIZE(RFScrollAnimation, 0x1c);
CHECK_FIELD(RFScrollAnimation, m_active, 0x0);
CHECK_FIELD(RFScrollAnimation, m_duration, 0x4);
CHECK_FIELD(RFScrollAnimation, m_from, 0x8);
CHECK_FIELD(RFScrollAnimation, m_to, 0xc);
CHECK_FIELD(RFScrollAnimation, m_elapsed, 0x10);
CHECK_FIELD(RFScrollAnimation, m_progress, 0x14);
CHECK_FIELD(RFScrollAnimation, m_spline, 0x18);

// Scroll bar: RFWnd with the scroll logic (vtable 0xd63dbc, 37 slots; 0-33 as
// RFWnd, the thumb and button handlers 34-36 are new). Size 0x1d4. The copy
// constructor 0x4151d0 and operator= 0x4152d0 are the implicit ones.
// dllexport as in the original library: the header inlines' kept copies
// (0x415190, 0x4151a0) pair in RFScrollBar.obj, and the includers read the
// members directly.
class RF_API RFScrollBar: public RFWnd {
	LAYOUT_CHECK_FRIEND;

public:
	// The class name, the static const at 0xe46e78 that GetTypeName returns.
	static std::wstring const TypeName;
	// The event SetScrollPos, OnLButtonDown and OnThumbMouseMove raise when
	// the position changes (the static const at 0xe46e5c), with a
	// ScrollBarPosChangedEventArgs.
	static std::wstring const EventPosChanged;

	virtual wchar_t const* ToString() const;                  // [0]  0x415170
	virtual ~RFScrollBar();                                   // [1]  vector deleting destructor 0x415450, ~RFScrollBar 0x428ec0
	virtual std::wstring const& GetTypeName();                // [2]  0x415180
	virtual void Create(WndBaseStruct*, RFWnd*);              // [4]  0x428f30
	RFScrollBar();                                            //      0x428e30
	// 0x415190
	RFPoint const& GetScrollRange() const { return m_scrollRange; }
	// 0x4151a0
	void EnableSmoothScroll(bool enable) { m_smoothScroll = enable; }
	void SetScrollSize(int32_t);                              //      0x429660
	void SetScrollRange(int32_t, int32_t);                    //      0x429670
	void SetScrollPos(int32_t);                               //      0x429770
	int32_t GetScrollPos() const;                             //      0x429830
	void Next();                                              //      0x429840
	void Prev();                                              //      0x4298a0
	void ScrollToTop();                                       //      0x429900
	void ScrollToBottom();                                    //      0x429910
	bool IsBottom();                                          //      0x429920
	void MoveToScroll(int32_t);                               //      0x429930

protected:
	virtual void PreInitialize();                             // [9]  0x429110
	virtual void OnEnabled(bool);                             // [12] 0x429430
	virtual void OnLayoutChanged(uint32_t);                   // [14] 0x429390
	virtual void OnLButtonDown(MouseEventArgs&);              // [15] 0x429480
	virtual void OnLButtonUp(MouseEventArgs&);                // [16] 0x429440
	virtual void OnRButtonUp(MouseEventArgs&);                // [19] 0x429460
	virtual void OnMouseWheel(MouseEventArgs&);               // [21] 0x429a30
	virtual void OnUpdate(UpdateEventArgs&);                  // [31] 0x429620
	virtual void OnLTButClicked(EventArgs&);                  // [34] 0x429b80
	virtual void OnRBButClicked(EventArgs&);                  // [35] 0x429bd0
	virtual void OnThumbMouseMove(EventArgs&);                // [36] 0x429a90

	void PositionToLevel(RFVector2 const&);                   //      0x429c20
	void LevelToThumbPosition();                              //      0x429da0
	void SmoothScrolling(float);                              //      0x42a090

private:
	// The three children PreInitialize subscribes to: the two buttons'
	// EventClicked (OnLTButClicked, OnRBButClicked) and the thumb's
	// EventMouseMove (OnThumbMouseMove).
	RFWnd* m_ltButton;                      // 0x178
	RFWnd* m_rbButton;                      // 0x17c
	RFThumbButton* m_thumb;                 // 0x180
	// The thumb's size when Create made it (its GetSize); SetScrollRange keeps
	// the thumb at least that tall.
	RFSize m_thumbSize;                     // 0x184
	RFPoint m_scrollRange;                  // 0x18c  minimum (ScrollToTop), maximum (ScrollToBottom)
	int32_t m_scrollPos;                    // 0x194
	int32_t m_scrollSize;                   // 0x198
	bool m_smoothScroll;                    // 0x19c
	int32_t m_smoothSpeed;                  // 0x1a0  1..5, grows while a smooth scroll is still running
	RFScrollAnimation m_animation;          // 0x1a4
	// The track inside the bar (SetScrollRange, PositionToLevel); copied to the
	// thumb's 0x1f8..0x204 by PreInitialize.
	RFRect m_trackRect;                     // 0x1c0
	bool m_horizontal;                      // 0x1d0  PositionToLevel tests it; copied to the thumb by PreInitialize
};
CHECK_SIZE(RFScrollBar, 0x1d4);
BEGIN_LAYOUT_CHECK(RFScrollBar)
	CHECK_FIELD(RFScrollBar, m_ltButton, 0x178);
	CHECK_FIELD(RFScrollBar, m_rbButton, 0x17c);
	CHECK_FIELD(RFScrollBar, m_thumb, 0x180);
	CHECK_FIELD(RFScrollBar, m_thumbSize, 0x184);
	CHECK_FIELD(RFScrollBar, m_scrollRange, 0x18c);
	CHECK_FIELD(RFScrollBar, m_scrollPos, 0x194);
	CHECK_FIELD(RFScrollBar, m_scrollSize, 0x198);
	CHECK_FIELD(RFScrollBar, m_smoothScroll, 0x19c);
	CHECK_FIELD(RFScrollBar, m_smoothSpeed, 0x1a0);
	CHECK_FIELD(RFScrollBar, m_animation, 0x1a4);
	CHECK_FIELD(RFScrollBar, m_trackRect, 0x1c0);
	CHECK_FIELD(RFScrollBar, m_horizontal, 0x1d0);
END_LAYOUT_CHECK

// The factory RFFactoryManager creates scroll bars with (vtable 0xd64f94).
class RFScrollBarFactory : public RFWindowFactory {
public:
	RFScrollBarFactory(); // 0x433430 (RFScrollBar.cpp, beside Create)
	virtual RFWnd* Create();          // [1] 0x433480
	virtual void Destroy(RFWnd*);     // [2] 0x4334f0
};

#endif
