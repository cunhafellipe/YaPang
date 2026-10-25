// Original: unknown (JP 983 exe). EventArgs' constructor 0x401620 and
// destructor 0x401640 sit in the object of Subscriber and RFEvent, whose
// asserts name include\RFEventHandler.h: the base was probably declared
// there. The derived classes' members sit with the RFWnd (0x401cf0) and
// RFManager (0x413e60-0x414170) code that raises them.
#ifndef PG_CLIENT_REFRESH_INCLUDE_RFEVENTARGS_H
#define PG_CLIENT_REFRESH_INCLUDE_RFEVENTARGS_H

#include <string>

#include "RFPoint.h"
#include "RFVector2.h"
#include "RFWnd.h"
#include "defines.h"

class RFAnimationObject;
class RFCheck;
class RFList;
class RFListItemBase;
class RFScrollBar;
class RFSlider;
class RFTextArea;

// The kind of input-method indicator PostIMStatusEvent 0x418270 reports: the
// language RFIme::UpdateLanguage 0x452300 detects from the keyboard layout
// (it stores 0 first and keeps it for the languages without an IME).
struct LocaleIndicatorTypes {
	enum Type {
		None = 0,
		Korean = 1,               // LANG_KOREAN
		Japanese = 2,             // LANG_JAPANESE
		SimplifiedChinese = 3,    // SUBLANG_CHINESE_SIMPLIFIED
		TraditionalChinese = 4    // SUBLANG_CHINESE_TRADITIONAL
	};
};

// The base of every event argument: vtable 0xd113c8, 1 slot, size 0x8, not
// exported. A handler sets handled to stop the event travelling up the
// parents (RFManager's Inject* loops stop when it is set).
class EventArgs {
public:
	EventArgs() : handled(false) {}  // 0x401620
	// [0] scalar deleting destructor 0x401640 (writes back the vptr only)
	virtual ~EventArgs() {}

	bool handled;  // 0x4
};
CHECK_SIZE(EventArgs, 0x8);
CHECK_FIELD(EventArgs, handled, 0x4);

// The events whose only argument is the window raising them (RTTI
// .?AV?$WindowEventArgs_@VRFWnd@@@@ and its siblings): size 0xc, one vtable
// per instantiation, [0] the scalar deleting destructor. The constructor
// (handled, vptr, sender) is kept unreferenced before each instance's code:
//   <RFWnd>       vtable 0xd63068, ctor 0x408ad0, [0] 0x409ef0 (RFWnd::Draw
//                 0x406960, OnMouseEnter/Leave; RFManager::
//                 PostGlobalHyperLinkLeaveEvent 0x4181f0)
//   <RFButton>    vtable 0xd6428c, ctor 0x421b70, [0] 0x421b80
//                 (RFButton::EventClicked: OnLButtonUp 0x421540,
//                 DefaultButtonEventHandler 0x421870)
//   <RFComboBox>  vtable 0xd65d54, ctor 0x455b70, [0] 0x455b80
//                 (RFComboBox::EventSelChanged)
//   <RFList> [0] 0x424ef0, <RFCheck> [0] 0x425f20, <RFTextArea> [0]
//   0x428450, <RFScrollBar> [0] 0x42a120, <RFSlider> [0] 0x44ff20
// Template instances: their code is emitted in their users' units.
template <class T>
class WindowEventArgs_ : public EventArgs {
public:
	WindowEventArgs_(T* sender_) : sender(sender_) {}

	T* sender;  // 0x8
};
CHECK_SIZE(WindowEventArgs_<RFWnd>, 0xc);
CHECK_FIELD(WindowEventArgs_<RFWnd>, sender, 0x8);

// RFWnd::EventClose, raised by RFWnd::Close 0x408080: vtable 0xd63070,
// size 0x10. [0] scalar deleting destructor 0x401d10.
class WindowCloseEventArgs : public EventArgs {
public:
	// constructor at 0x401cf0 (unpaired)
	WindowCloseEventArgs(RFWNDCLOSESTATE closeState_, RFWnd* window_)
	    : window(window_), closeState(closeState_) {}

	RFWnd* window;               // 0x8
	RFWNDCLOSESTATE closeState;  // 0xc
};
CHECK_SIZE(WindowCloseEventArgs, 0x10);
CHECK_FIELD(WindowCloseEventArgs, window, 0x8);
CHECK_FIELD(WindowCloseEventArgs, closeState, 0xc);

// RFWnd::EventVisibleChanged, raised by RFWnd::OnShow 0x407650 (also
// RFButton and RFComboBox's OnShow): vtable 0xd63078, size 0x10. [0] scalar
// deleting destructor 0x401d80.
class VisibleChangedArgs : public EventArgs {
public:
	VisibleChangedArgs(bool visible_, RFWnd* window_)  // 0x401d60
	    : window(window_), visible(visible_) {}

	RFWnd* window;  // 0x8
	bool visible;   // 0xc
};
CHECK_SIZE(VisibleChangedArgs, 0x10);
CHECK_FIELD(VisibleChangedArgs, window, 0x8);
CHECK_FIELD(VisibleChangedArgs, visible, 0xc);

// Raised by RFWnd::PlayAnimation 0x408150 and RFDirector::SendFinishEvent
// 0x430a10: vtable 0xd630b0, size 0x24. [0] scalar deleting destructor
// 0x408290 (destroys the name).
class AnimationEventArgs : public EventArgs {
public:
	AnimationEventArgs() {}  // 0x408240

	std::wstring animationName;  // 0x8
};
CHECK_SIZE(AnimationEventArgs, 0x24);
CHECK_FIELD(AnimationEventArgs, animationName, 0x8);

// The mouse events (RFManager::InjectLButtonDown 0x418ad0 ... InjectMouseWheel
// 0x419180): vtable 0xd640c8, size 0x1c. [0] scalar deleting destructor
// 0x413e80. The position is one RFVector2: RFCheck::OnLButtonUp 0x425c90
// passes &args + 0xc to HitTest(RFVector2 const&, bool), RFList::OnMouseMove
// 0x422cc0 copies it with integer moves.
class MouseEventArgs : public EventArgs {
public:
	MouseEventArgs() : window(0) {}  // 0x413e60 (inlines RFVector2() 0x4289b0)

	RFWnd* window;     // 0x8 (the window the event was injected into)
	RFVector2 pos;     // 0xc
	int button;        // 0x14 (0 for the button-down/up events, 3 for the wheel)
	float wheelDelta;  // 0x18 (InjectMouseWheel's int, as a float)
};
CHECK_SIZE(MouseEventArgs, 0x1c);
CHECK_FIELD(MouseEventArgs, window, 0x8);
CHECK_FIELD(MouseEventArgs, pos, 0xc);
CHECK_FIELD(MouseEventArgs, button, 0x14);
CHECK_FIELD(MouseEventArgs, wheelDelta, 0x18);

// RFManager::InjectKeyDown 0x419280 / InjectKeyUp 0x419340 (uint, uint):
// vtable 0xd640d0, size 0x14. [0] scalar deleting destructor 0x413ee0.
class KeyEventArgs : public EventArgs {
public:
	KeyEventArgs(RFWnd* window_, unsigned int keyCode_, unsigned int keyData_)  // 0x413ec0
	    : window(window_), keyCode(keyCode_), keyData(keyData_) {}

	RFWnd* window;         // 0x8
	unsigned int keyCode;  // 0xc (InjectKeyDown's first uint)
	unsigned int keyData;  // 0x10 (InjectKeyDown's second uint)
};
CHECK_SIZE(KeyEventArgs, 0x14);
CHECK_FIELD(KeyEventArgs, window, 0x8);
CHECK_FIELD(KeyEventArgs, keyCode, 0xc);
CHECK_FIELD(KeyEventArgs, keyData, 0x10);

// RFManager::InjectChar 0x419400 (wchar_t*, bool, RFPoint const&), once with
// the whole filtered string, and RFEdit::KeyDownCopyAndPaste 0x436dc0: vtable 0xd640d8, size
// 0x1c. [0] scalar deleting destructor 0x413f50.
class CharEventArgs : public EventArgs {
public:
	CharEventArgs(RFWnd* window_, wchar_t const* text_, bool composing_,  // 0x413f20
	              RFPoint const& point_)
	    : window(window_), text(text_), composing(composing_),
	      point(point_) {}

	RFWnd* window;           // 0x8
	wchar_t const* text;     // 0xc (InjectChar's filtered string's c_str(); RFEdit::KeyDownCopyAndPaste's buffer)
	bool composing;          // 0x10 (InjectChar's bool)
	RFPoint point;           // 0x14 (InjectChar's RFPoint)
};
CHECK_SIZE(CharEventArgs, 0x1c);
CHECK_FIELD(CharEventArgs, window, 0x8);
CHECK_FIELD(CharEventArgs, text, 0xc);
CHECK_FIELD(CharEventArgs, composing, 0x10);
CHECK_FIELD(CharEventArgs, point, 0x14);

// RFManager::InjectUpdate 0x419830 (float): vtable 0xd640e0, size 0xc. [0]
// scalar deleting destructor 0x413fb0.
class UpdateEventArgs : public EventArgs {
public:
	UpdateEventArgs(float elapsed_) : elapsed(elapsed_) {}  // 0x413f90

	float elapsed;  // 0x8 (InjectUpdate's float)
};
CHECK_SIZE(UpdateEventArgs, 0xc);
CHECK_FIELD(UpdateEventArgs, elapsed, 0x8);

// RFManager::PostHyperLinkEvent 0x417fd0 / PostGlobalHyperLinkEnterEvent
// 0x4180e0 (RFWnd*, wstring const&, wstring const&): vtable 0xd640e8, size
// 0x44. [0] scalar deleting destructor 0x414080 (destroys both strings).
// RFTextArea 0x427000 posts (this, this->m_name, the clicked link).
class HyperLinkEventArgs : public EventArgs {
public:
	HyperLinkEventArgs(RFWnd* window_, std::wstring const& windowName_,  // 0x413ff0
	                   std::wstring const& link_)
	    : window(window_), windowName(windowName_), link(link_) {}

	RFWnd* window;            // 0x8
	std::wstring windowName;  // 0xc (the Post*Event's first string: the window's m_name)
	std::wstring link;        // 0x28 (the Post*Event's second string)
};
CHECK_SIZE(HyperLinkEventArgs, 0x44);
CHECK_FIELD(HyperLinkEventArgs, window, 0x8);
CHECK_FIELD(HyperLinkEventArgs, windowName, 0xc);
CHECK_FIELD(HyperLinkEventArgs, link, 0x28);

// RFManager::EventInputMethodStatusChanged, raised by PostIMStatusEvent
// 0x418270 (LocaleIndicatorTypes::Type const&, uint const&): vtable 0xd640f0,
// size 0x10. [0] scalar deleting destructor 0x414170.
class IMStatusEventArgs : public EventArgs {
public:
	IMStatusEventArgs(LocaleIndicatorTypes::Type const& indicatorType_,  // 0x414150
	                  unsigned int const& status_)
	    : indicatorType(indicatorType_), status(status_) {}

	LocaleIndicatorTypes::Type indicatorType;  // 0x8
	unsigned int status;                       // 0xc
};
CHECK_SIZE(IMStatusEventArgs, 0x10);
CHECK_FIELD(IMStatusEventArgs, indicatorType, 0x8);
CHECK_FIELD(IMStatusEventArgs, status, 0xc);

// The control events below are built on the stack by the code that raises
// them; their constructors use LTCG's register convention (this in eax, the
// sender in ecx, the value in edx) and are kept unreferenced next to the
// raising class's code, each followed by its scalar deleting destructor.

// RFList::EventSelChanged, raised by SelectItem 0x424440 / 0x424540,
// UnselectItem 0x4246e0 / 0x4247d0, RelaseAllSelectedItem 0x422990 and
// ButtonDown_Impl 0x422e00: vtable 0xd644e4, size 0x10. [0] scalar deleting
// destructor 0x4225b0.
class ListSelChangedEventArgs : public EventArgs {
public:
	// constructor at 0x422590 (unpaired)
	ListSelChangedEventArgs(RFList* sender_, RFListItemBase* item_)
	    : sender(sender_), item(item_) {}

	RFList* sender;         // 0x8
	RFListItemBase* item;   // 0xc (the item selected or unselected)
};
CHECK_SIZE(ListSelChangedEventArgs, 0x10);
CHECK_FIELD(ListSelChangedEventArgs, sender, 0x8);
CHECK_FIELD(ListSelChangedEventArgs, item, 0xc);

// RFList::EventDrawItem, raised by RFList 0x423930 for every item that
// intersects the view: vtable 0xd644ec, size 0x18. [0] scalar deleting
// destructor 0x422620.
class ListDrawItemEventArgs : public EventArgs {
public:
	// constructor at 0x422600 (unpaired; inlines RFVector2() 0x4289b0, the
	// pointers stay uninitialised)
	ListDrawItemEventArgs() {}

	RFList* sender;         // 0x8
	RFListItemBase* item;   // 0xc
	RFVector2 pos;          // 0x10 (the item's position, its slot 16)
};
CHECK_SIZE(ListDrawItemEventArgs, 0x18);
CHECK_FIELD(ListDrawItemEventArgs, sender, 0x8);
CHECK_FIELD(ListDrawItemEventArgs, item, 0xc);
CHECK_FIELD(ListDrawItemEventArgs, pos, 0x10);

// RFCheck::EventChanged, raised by RFCheck::OnLButtonUp 0x425c90 (base
// WindowEventArgs_<RFCheck>, vtable 0xd64530, per the RTTI hierarchy): vtable
// 0xd64538, size 0x10. [0] scalar deleting destructor 0x425a30.
class CheckChangedEventArgs : public WindowEventArgs_<RFCheck> {
public:
	// constructor at 0x425a10 (unpaired)
	CheckChangedEventArgs(RFCheck* sender_, bool checked_)
	    : WindowEventArgs_<RFCheck>(sender_), checked(checked_) {}

	bool checked;     // 0xc (the new state, RFCheck 0x178)
};
CHECK_SIZE(CheckChangedEventArgs, 0x10);
CHECK_FIELD(CheckChangedEventArgs, sender, 0x8);
CHECK_FIELD(CheckChangedEventArgs, checked, 0xc);

// RFTextArea::EventHyperLinkClicked, raised by RFTextArea 0x427000 (which
// then posts RFManager's HyperLinkEventArgs): vtable 0xd646e4, size 0x28. [0]
// scalar deleting destructor 0x4262a0 (destroys the link).
class HyperLinkClickedEventArgs : public EventArgs {
public:
	// constructor at 0x426230 (unpaired)
	HyperLinkClickedEventArgs(RFTextArea* sender_, std::wstring const& link_)
	    : sender(sender_), link(link_) {}

	RFTextArea* sender;  // 0x8
	std::wstring link;   // 0xc
};
CHECK_SIZE(HyperLinkClickedEventArgs, 0x28);
CHECK_FIELD(HyperLinkClickedEventArgs, sender, 0x8);
CHECK_FIELD(HyperLinkClickedEventArgs, link, 0xc);

// RFTextArea's hyperlink-enter event, raised by RFTextArea::OnMouseMove
// 0x427170 (twice): vtable 0xd646ec, size 0x28. [0] scalar deleting
// destructor 0x4263a0 (destroys the link).
class HyperLinkEnterEventArgs : public EventArgs {
public:
	HyperLinkEnterEventArgs(RFTextArea* sender_, std::wstring const& link_)  // 0x426330
	    : sender(sender_), link(link_) {}

	RFTextArea* sender;  // 0x8
	std::wstring link;   // 0xc
};
CHECK_SIZE(HyperLinkEnterEventArgs, 0x28);
CHECK_FIELD(HyperLinkEnterEventArgs, sender, 0x8);
CHECK_FIELD(HyperLinkEnterEventArgs, link, 0xc);

// RFTextArea::EventRichTextDrawImage, raised by
// RFTextArea::SendDrawImageEvent 0x428040 (RFVector2 const&, wstring const&):
// vtable 0xd646f4, size 0x30. [0] scalar deleting destructor 0x4264a0
// (destroys the name). The class name's typo is the original's (RTTI).
class RichTextDarwIamgeEventArgs : public EventArgs {
public:
	RichTextDarwIamgeEventArgs(RFTextArea* sender_, RFVector2 const& pos_,  // 0x426420
	                           std::wstring const& imageName_)
	    : sender(sender_), pos(pos_), imageName(imageName_) {}

	RFTextArea* sender;      // 0x8
	RFVector2 pos;           // 0xc (SendDrawImageEvent's RFVector2)
	std::wstring imageName;  // 0x14 (SendDrawImageEvent's string)
};
CHECK_SIZE(RichTextDarwIamgeEventArgs, 0x30);
CHECK_FIELD(RichTextDarwIamgeEventArgs, sender, 0x8);
CHECK_FIELD(RichTextDarwIamgeEventArgs, pos, 0xc);
CHECK_FIELD(RichTextDarwIamgeEventArgs, imageName, 0x14);

// RFScrollBar::EventPosChanged, raised by RFScrollBar::SetScrollPos 0x429770,
// OnLButtonDown 0x429480 and OnThumbMouseMove 0x429a90: vtable 0xd64730,
// size 0x10 (SetScrollPos 0x4297ce stores handled, sender, vptr, pos into
// consecutive dwords: sender +8, pos +0xc). [0] scalar deleting destructor
// 0x428dd0.
class ScrollBarPosChangedEventArgs : public WindowEventArgs_<RFScrollBar> {
public:
	// constructor at 0x428db0 (unpaired)
	ScrollBarPosChangedEventArgs(RFScrollBar* sender_, int32_t pos_)
	    : WindowEventArgs_<RFScrollBar>(sender_), pos(pos_) {}

	int32_t pos;          // 0xc (the clamped scroll position, RFScrollBar 0x194)
};
CHECK_SIZE(ScrollBarPosChangedEventArgs, 0x10);
CHECK_FIELD(ScrollBarPosChangedEventArgs, sender, 0x8);
CHECK_FIELD(ScrollBarPosChangedEventArgs, pos, 0xc);

// vtable 0xd648b4, size 0xc. [0] scalar deleting destructor 0x42adf0. No
// code builds one outside its kept constructor; the sender's type is
// inferred from AnimDrawEventArgs, its neighbour.
class AnimEventArgs : public EventArgs {
public:
	// constructor at 0x42ade0 (unpaired)
	AnimEventArgs(RFAnimationObject* sender_) : sender(sender_) {}

	RFAnimationObject* sender;  // 0x8
};
CHECK_SIZE(AnimEventArgs, 0xc);
CHECK_FIELD(AnimEventArgs, sender, 0x8);

// The event RFAnimationObject::Render 0x42c7d0 raises before drawing (a
// handler sets handled to draw the object itself; RFAnimation's 0x45cfb0
// inlines Render): vtable 0xd648bc, size 0x10. [0] scalar deleting
// destructor 0x42ae50.
class AnimDrawEventArgs : public EventArgs {
public:
	AnimDrawEventArgs(RFAnimationObject* sender_, float alpha_)  // 0x42ae30
	    : sender(sender_), alpha(alpha_) {}

	RFAnimationObject* sender;  // 0x8
	float alpha;                // 0xc (Render's float)
};
CHECK_SIZE(AnimDrawEventArgs, 0x10);
CHECK_FIELD(AnimDrawEventArgs, sender, 0x8);
CHECK_FIELD(AnimDrawEventArgs, alpha, 0xc);

// RFSlider::EventPosChanged, raised by RFSlider::SetSlideRange, SetSlidePos
// and its handlers: vtable 0xd65ba0, size 0x10. [0] scalar deleting
// destructor 0x44ed10.
class SliderPosChangedEventArgs : public WindowEventArgs_<RFSlider> {
public:
	// constructor at 0x44ecf0 (unpaired)
	SliderPosChangedEventArgs(RFSlider* sender_, int32_t pos_)
	    : WindowEventArgs_<RFSlider>(sender_), pos(pos_) {}

	int32_t pos;       // 0xc (the new slide position)
};
CHECK_SIZE(SliderPosChangedEventArgs, 0x10);
CHECK_FIELD(SliderPosChangedEventArgs, sender, 0x8);
CHECK_FIELD(SliderPosChangedEventArgs, pos, 0xc);

#endif
