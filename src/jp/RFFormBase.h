// The JP 983 game's base of a Refresh form (RTTI RFFormBase<RFWndEx>,
// vtable 0xd0e3e4, 34 slots: no new virtuals; handlers bound through
// MemberFunctionFunctor<RFFormBase<RFWndEx>>, vtable 0xd0e8f8). Its
// constructor is inlined in the derived ones (RFTooltip's 0x5ce0c0 calls
// RFWndEx::RFWndEx, then stores its own vtable). The members copied into
// the tooltip's object (0x5ce780, 0x5ce880) are written here; the others
// are folded with the RFCharMasteryCardSelectColumn rows of jp/pairing.tsv.
#ifndef PG_JP_RFFORMBASE_H
#define PG_JP_RFFORMBASE_H

#include <stdint.h>

#include "defines.h"
#include "RFEventArgs.h"
#include "RFEventHandler.h"
#include "RFManager.h"
#include "RFRect.h"
#include "RFWndEx.h"
#include "RFSceneManager.h"
#include "wmath.h"

template <class T>
class RFFormBase : public T {
	LAYOUT_CHECK_FRIEND;
public:
	// Inlined in RFTooltip's constructor, which leaves 0x180 as it is.
	RFFormBase() {
	}
	// [1] 0x5b85b0: scalar deleting destructor (restores the vtable, then
	// ~RFWndEx).
	virtual ~RFFormBase() {
	}

protected:
	virtual void OnDestroy();                                // [11] 0x5b8cb0
	virtual void OnShow(bool) {}                             // [13] 0x5b8490

public:
	// 0x5ce880 (name ours): Activate or Deactivate, then ShowForm(show).
	void ShowActivated(bool show);
	// 0x5ce780 (name ours): the first time, registers OnFormUpdate on its
	// own name for RFWnd::EventUpdate (m_updateRegistered); then
	// ShowWindow(show). 0x5b9cd0 is the same code in another instance.
	void ShowForm(bool show);
	// 0x5b8d60 (name ours): the EventUpdate handler ShowForm registers;
	// while visible, RFSceneManager +0x41 = the mouse is inside the
	// window's screen rect, then handled = false.
	void OnFormUpdate(EventArgs& args);

protected:
	bool m_updateRegistered;                                 // 0x180 set by ShowForm
};
CHECK_SIZE(RFFormBase<RFWndEx>, 0x184);
BEGIN_LAYOUT_CHECK(RFFormBase<RFWndEx>)
	CHECK_FIELD(RFFormBase<RFWndEx>, m_updateRegistered, 0x180);
END_LAYOUT_CHECK

// 0x5b8cb0 (folded with the other RFFormBase<RFWndEx> instances' copies):
// without a Refresh modal, the Fresh input switched back on when it was
// off; then RFSceneManager's +0x41 cleared. The manager is read as
// `instance ? instance +0x28 : 0` and dereferenced unchecked, as in
// RFFormDesktopBase.
template <class T>
void RFFormBase<T>::OnDestroy() {
	if (!RFSceneManager::Instance()->GetManager()->IsModalExist()
		&& RFSceneManager::IsFreshInputDisabled()) {
		RFSceneManager::SetFreshInputDisabled(false);
	}
	RFSceneManager::Instance()->SetMouseOverForm(false);
	T::OnDestroy();
}

// 0x5b8d60. JP guards the mouse test with a dword read at args+0xc, past
// the 0xc-byte UpdateEventArgs that InjectUpdate (its only creator) builds
// on its stack; on x86 that slot is InjectUpdate's EH record link, never
// zero, so the test always runs. The port runs it unconditionally instead
// of reading past the object.
template <class T>
void RFFormBase<T>::OnFormUpdate(EventArgs& args) {
	if (this->IsWindowVisible(false)) {
		RFRect rect = this->GetScreenRect();
		WVector const& mouse = RFSceneManager::Instance()->GetMousePosition();
		RFSceneManager::Instance()->SetMouseOverForm(rect.PtInRect(mouse.x, mouse.y));
		args.handled = false;
	}
}

// 0x5ce780, 0x5b9cd0
template <class T>
void RFFormBase<T>::ShowForm(bool show) {
	if (!m_updateRegistered) {
		this->RegisterTo(this->GetName(), RFWnd::EventUpdate,
			Subscriber(&RFFormBase<T>::OnFormUpdate, this));
		m_updateRegistered = true;
	}
	this->ShowWindow(show);
}

// 0x5ce880
template <class T>
void RFFormBase<T>::ShowActivated(bool show) {
	if (show) {
		this->Activate();
		ShowForm(true);
	} else {
		this->Deactivate();
		ShowForm(false);
	}
}

#endif
