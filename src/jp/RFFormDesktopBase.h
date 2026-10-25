// The JP 983 game's base of a Refresh desktop screen (RTTI
// RFFormDesktopBase<RFWndEx>, vtable 0xd130b4, 34 slots: no new virtuals;
// handlers bound through MemberFunctionFunctor<RFFormDesktopBase<RFWndEx>>,
// vtable 0xd13248). Its constructor is inlined in each screen's (the lobby's
// 0x5ffc60 calls RFWndEx::RFWndEx, then sets 0x180..0x189). Declarations and
// the JP x86 layout only (port/refresh/TODO.md, checkpoint 3).
#ifndef PG_JP_RFFORMDESKTOPBASE_H
#define PG_JP_RFFORMDESKTOPBASE_H

#include <stdint.h>

#include "defines.h"
#include "RFEventArgs.h"
#include "RFEventHandler.h"
#include "RFManager.h"
#include "RFRect.h"
#include "RFSize.h"
#include "RFVector2.h"
#include "RFWndEx.h"
#include "RFSceneManager.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "wmath.h"

template <class T>
class RFFormDesktopBase : public T {
	LAYOUT_CHECK_FRIEND;
public:
	// Inlined in each screen's constructor (the lobby's 0x5ffc60).
	RFFormDesktopBase()
		: m_updateRegistered(false), m_pendingTaskChange(0),
		  m_unknown188(false), m_unknown189(true) {
	}
	// [1] 0x5f6080: scalar deleting destructor (restores the vtable, then
	// ~RFWndEx).
	virtual ~RFFormDesktopBase();
	// [3] 0x5f7840: m_pendingTaskChange = 0, the window's rect to (0, 0,
	// the RFManager root window's size, RFSceneManager +0x28 → +0x4c),
	// SetPreDraw(true), then RFWnd::Initialize.
	virtual void Initialize();
	// [11] 0x5f69e0: RFManager::DeleteDesktop, RFSceneManager 0x603ca0(0),
	// RFSceneManager +0x41 = 0, RFWndEx::OnDestroy, m_updateRegistered = 0.
	virtual void OnDestroy();

	// 0x5fb820 (not virtual; called by RFLobbyDesktop's slot 34): the first
	// time, registers OnDesktopUpdate on its own name for
	// RFWnd::EventUpdate (m_updateRegistered); when shown, clears
	// m_pendingTaskChange and makes itself the manager's desktop
	// (RFManager::GetDesktop, DeleteDesktop of the previous one).
	void ShowDesktop(bool show);
	// 0x5fa1f0: the EventUpdate handler ShowDesktop registers.
	void OnDesktopUpdate(EventArgs& args);
	// 0x5f8ba0 (name ours): when m_unknown189, matches the input
	// handler's state and RFSceneManager's Fresh input flags to the current
	// Fresh layout (JP g_pFresh 0xf022d0 +0x40, its +0xac against
	// m_pendingTaskChange) and to IsMouseOverOtherWindow.
	void SyncInputState();
	// 0x5f8a00 (name ours): RFManager::HitTest2 at the scene's mouse
	// position; true when a window is hit and its name differs from this
	// window's (the mouse is over one of its controls or another window).
	bool IsMouseOverOtherWindow();

	// Ours, for RFSceneManager::UpdateInputFocus (0x6039a0), which compares
	// the int at the desktop's +0x17c with the Fresh modal count
	// (0x6039d1). +0x17c is m_pendingTaskChange's offset in a
	// RFFormDesktopBase<RFWnd> (RFWnd is 0x178), but the exe has only the
	// RFWndEx instance (RTTI, vtable 0xd130b4): on its desktop the int is
	// RFWndEx::m_brushDesc. The exe is not large-address-aware (PE
	// characteristics 0x102), so a set m_brushDesc is a positive int that
	// never equals a modal count, and a null one is 0. True when the
	// exe's read is > 0 (and so differs from every modal count).
	bool IsFocusLayoutValuePositive() const { return this->m_brushDesc != NULL; }

protected:
	bool m_updateRegistered;      // 0x180 set by ShowDesktop, cleared by OnDestroy
	int32_t m_pendingTaskChange;  // 0x184 0 at Initialize/ShowDesktop(true); 1 after the shop/my-room buttons hand over to the avatar-chat task
	bool m_unknown188;            // 0x188 0 at construction
	bool m_unknown189;            // 0x189 1 at construction
};
CHECK_SIZE(RFFormDesktopBase<RFWndEx>, 0x18c);
BEGIN_LAYOUT_CHECK(RFFormDesktopBase<RFWndEx>)
	CHECK_FIELD(RFFormDesktopBase<RFWndEx>, m_updateRegistered, 0x180);
	CHECK_FIELD(RFFormDesktopBase<RFWndEx>, m_pendingTaskChange, 0x184);
	CHECK_FIELD(RFFormDesktopBase<RFWndEx>, m_unknown188, 0x188);
	CHECK_FIELD(RFFormDesktopBase<RFWndEx>, m_unknown189, 0x189);
END_LAYOUT_CHECK

// The template's members, from the copies of the RFFormDesktopBase<RFWndEx>
// instance in the lobby's object. RFSceneManager's input switch (0x6038f0,
// 0x603920, 0x603ca0) and its +0x41 flag are requested from its header,
// RFManager's root window (+0x4c, read directly by 0x5f7840) from RFManager.h.
// The exe reads the manager as `instance ? instance +0x28 : 0` and then
// dereferences it anyway (no test of the result): the source called
// RFSceneManager::Instance()->GetManager() unchecked, which is written here.

// 0x5f6080 is the scalar deleting destructor; the body is ~RFWndEx's.
template <class T>
RFFormDesktopBase<T>::~RFFormDesktopBase() {
}

// 0x5f7840
template <class T>
void RFFormDesktopBase<T>::Initialize() {
	m_pendingTaskChange = 0;
	RFSize size = RFSceneManager::Instance()->GetManager()->GetRootWnd()->GetSize();
	this->SetRect(RFRect(0.0f, 0.0f, size.width, size.height));
	this->SetPreDraw(true);
	RFWnd::Initialize();
}

// 0x5f69e0
template <class T>
void RFFormDesktopBase<T>::OnDestroy() {
	RFSceneManager::Instance()->GetManager()->DeleteDesktop();
	RFSceneManager::Instance()->SetAllInputDisabled(false);
	RFSceneManager::Instance()->SetMouseOverForm(false);
	T::OnDestroy();
	m_updateRegistered = false;
}

// 0x5fa1f0
template <class T>
void RFFormDesktopBase<T>::OnDesktopUpdate(EventArgs& args) {
	if (this->IsWindowVisible(false)) {
		if (RFSceneManager::Instance()->GetManager()->IsModalExist()) {
			if (RFSceneManager::Instance()->IsInputDisabled()) {
				RFSceneManager::Instance()->SetInputDisabled(false);
			}
		} else {
			SyncInputState();
		}
	}
	args.handled = false;
}

// 0xf022d0 is the JP g_pFresh; its FrWndManager (+0x40,
// g_pFresh->GetManager()) holds at +0xac, a JP-only member, the count of
// open modal forms (JP 0xbe4650, FrWndManager::UpdateModalFormCount under
// JP_PAKS) that SyncInputState compares with m_pendingTaskChange. The exe
// has no copy of either: every reader inlines the reads (SyncInputState
// 0x5f8bb4-0x5f8bc2 and UpdateInputFocus 0x6039b8-0x6039cf test
// `g_pFresh && +0x40`; both read +0xac directly), so they are header
// inlines (names ours).
inline bool Jp_HasFreshLayoutWindow() {
	return g_pFresh != NULL && g_pFresh->GetManager() != NULL;
}
// Unchecked, as the exe reads it: SyncInputState's no-window path
// (0x5f8c51-0x5f8c59) loads g_pFresh, then +0x40, then +0xac with no test of
// either pointer.
inline int32_t Jp_GetFreshModalFormCount() {
	return g_pFresh->GetManager()->GetModalFormCount();
}

// 0x5f8ba0
template <class T>
void RFFormDesktopBase<T>::SyncInputState() {
	if (!m_unknown189) {
		return;
	}
	if (Jp_HasFreshLayoutWindow()) {
		bool otherLayout;
		if (m_pendingTaskChange <= 0) {
			otherLayout = Jp_GetFreshModalFormCount() != 0;
		} else {
			otherLayout = m_pendingTaskChange != Jp_GetFreshModalFormCount();
		}
		if (otherLayout) {
			if (RFSceneManager::IsFreshInputDisabled()) {
				RFSceneManager::SetFreshInputDisabled(false);
			}
			if (!RFSceneManager::IsInputDisabled()) {
				RFSceneManager::SetInputDisabled(true);
			}
			return;
		}
	}
	if (RFSceneManager::IsInputDisabled()) {
		RFSceneManager::SetInputDisabled(false);
	}
	bool over = IsMouseOverOtherWindow();
	if (m_pendingTaskChange == Jp_GetFreshModalFormCount()) {
		if (over != RFSceneManager::IsFreshInputSuspended()) {
			RFSceneManager::SetFreshInputSuspended(over);
		}
	} else if (over != RFSceneManager::IsFreshInputDisabled()) {
		RFSceneManager::SetFreshInputDisabled(over);
	}
}

// 0x5f8a00
template <class T>
bool RFFormDesktopBase<T>::IsMouseOverOtherWindow() {
	WVector const& mouse = RFSceneManager::Instance()->GetMousePosition();
	RFWnd* wnd = RFSceneManager::Instance()->GetManager()->HitTest2(
		RFVector2(mouse.x, mouse.y), true, true);
	if (wnd != NULL) {
		return wnd->GetName() != this->GetName();
	}
	return false;
}

// 0x5fb820
template <class T>
void RFFormDesktopBase<T>::ShowDesktop(bool show) {
	if (!m_updateRegistered) {
		this->RegisterTo(this->GetName(), RFWnd::EventUpdate,
			Subscriber(&RFFormDesktopBase<T>::OnDesktopUpdate, this));
		m_updateRegistered = true;
	}
	if (show) {
		m_pendingTaskChange = 0;
		if (RFSceneManager::Instance()->GetManager()->GetDesktop() != this) {
			RFSceneManager::Instance()->GetManager()->DeleteDesktop();
		}
		RFSceneManager::Instance()->GetManager()->RegisterDesktop(this);
		RFSceneManager::Instance()->SetMouseOverForm(true);
		RFSceneManager::Instance()->SetInputDisabled(false);
	}
	this->ShowWindow(show);
}

#endif
