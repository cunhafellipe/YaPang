// The JP 983 game's base of a modal Refresh form (RTTI
// RFFormModalBase<RFWndEx> : RFWndEx, RFModalRenderer; vtable 0xd0e47c, 34
// slots: no new virtuals; the RFModalRenderer sub-object at +0x180 has
// vtable 0xd0e470; handlers bound through
// MemberFunctionFunctor<RFFormModalBase<RFWndEx>>, vtable 0xcfe650). Its
// constructor is inlined in the derived ones (RFLoadingProgressDlg's
// 0x5d15b0). From the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>);
// RFModalRenderer's members are in RFFormModalBase.cpp.
#ifndef PG_JP_RFFORMMODALBASE_H
#define PG_JP_RFFORMMODALBASE_H

#include <list>

#include "defines.h"
#include "RFEventArgs.h"
#include "RFEventHandler.h"
#include "RFManager.h"
#include "RFWndEx.h"
#include "RFSceneManager.h"

#include "fresh.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "wmath.h"
#include "wtypes.h"

// RTTI RFModalRenderer (vtable 0xd12c14, 1 slot), size 0x4: only its
// vtable pointer. The open modal forms are kept, in the order they were
// shown, in s_modals; the last one shown darkens the screen behind it.
class RFModalRenderer {
public:
	// 0x5e51f0 (inlined in RFLoadingProgressDlg's constructor's call).
	RFModalRenderer();
	// [0] 0x5e5400 (scalar deleting destructor); ~RFModalRenderer 0x5e5200.
	virtual ~RFModalRenderer();

	// 0x5e5b70 (name ours): appends this to s_modals.
	void AddModal();
	// 0x5e5ab0 (name ours): erases this from s_modals (its first entry).
	void RemoveModal();
	// 0x5e58f0 (name ours): when this is the last modal shown, fills the
	// root window's area with black, more opaque the more modals are open.
	void DrawModalBackground(float alpha);

	// 0xe78bf8 (name ours; the dynamic initializer 0xcd1fc0 builds it).
	static std::list<RFModalRenderer*> s_modals;
};

template <class T>
class RFFormModalBase : public T, public RFModalRenderer {
	LAYOUT_CHECK_FRIEND;
public:
	// Inlined in the derived constructors: T(), RFModalRenderer(), then
	// m_updateRegistered cleared.
	RFFormModalBase()
		: m_updateRegistered(false) {
	}
	// [1] 0x5b85e0 (scalar deleting destructor; the sub-object's [0] is
	// 0x5b8780).
	virtual ~RFFormModalBase() {
	}

protected:
	virtual void OnDestroy();                                // [11] 0x5b8d00
	virtual void OnShow(bool) {}                             // [13] 0x5b8580

public:
	// 0x4783e0 (name ours; it hides RFWnd::ShowModal, which it calls): the
	// first time, registers OnModalUpdate and OnModalDraw on its own name;
	// then the Fresh input off, the mouse-over-form flag set, the window shown
	// modal on top (priority -3), added to the modal list and focused.
	// RFLoadingProgressDlg's [34] calls it.
	void ShowModal();

protected:
	// 0x477ca0 (name ours), EventUpdate: with g_pFresh and its manager, the
	// Fresh form under the mouse (the JP-only FrWndManager 0xbe47e0, given
	// RFSceneManager::GetMousePosition's x and y): when there is one and its
	// window flags (FrWnd::m_nFlags, JP +0x1c) have 0x4000 (FrForm::Open
	// sets it for form flag 0x800) and the Fresh input is disabled
	// (RFSceneManager::IsFreshInputDisabled), the Fresh input enabled again
	// (SetFreshInputDisabled(false)); when there is none and the Fresh input
	// is enabled, it is disabled (SetFreshInputDisabled(true)). 0xbe47e0
	// is FrWndManager::GetFormFromPoint (JP_PAKS, frwndmanager.h).
	void OnModalUpdate(EventArgs& args);
	// 0x4777c0 (name ours), EventDraw.
	void OnModalDraw(EventArgs& args);

	bool m_updateRegistered;                                 // 0x184 tested and set by ShowModal
};

// 0x5b8d00: without a Refresh modal, the Fresh input switched back on;
// RFSceneManager's +0x41 cleared; out of the modal list. The manager is
// read as `instance ? instance +0x28 : 0` and dereferenced unchecked, as in
// RFFormBase.
template <class T>
void RFFormModalBase<T>::OnDestroy() {
	if (!RFSceneManager::Instance()->GetManager()->IsModalExist()) {
		RFSceneManager::SetFreshInputDisabled(false);
	}
	RFSceneManager::Instance()->SetMouseOverForm(false);
	RemoveModal();
	T::OnDestroy();
	m_updateRegistered = false;
}

// 0x477ca0
template <class T>
void RFFormModalBase<T>::OnModalUpdate(EventArgs& args) {
	if (g_pFresh == NULL || g_pFresh->GetManager() == NULL) {
		return;
	}
	WVector const& mouse = RFSceneManager::Instance()->GetMousePosition();
	FrWnd* form = g_pFresh->GetManager()->GetFormFromPoint(mouse.x, mouse.y);
	if (form != NULL) {
		if (form->m_nFlags.GetFlag(0x4000) && RFSceneManager::IsFreshInputDisabled()) {
			RFSceneManager::SetFreshInputDisabled(false);
		}
	} else if (!RFSceneManager::IsFreshInputDisabled()) {
		RFSceneManager::SetFreshInputDisabled(true);
	}
}

// 0x4777c0
template <class T>
void RFFormModalBase<T>::OnModalDraw(EventArgs& args) {
	DrawModalBackground(this->GetAlpha(false));
	args.handled = false;
}

// 0x4783e0
template <class T>
void RFFormModalBase<T>::ShowModal() {
	if (!m_updateRegistered) {
		this->RegisterTo(this->GetName(), RFWnd::EventUpdate,
			Subscriber(&RFFormModalBase<T>::OnModalUpdate, this));
		this->RegisterTo(this->GetName(), RFWnd::EventDraw,
			Subscriber(&RFFormModalBase<T>::OnModalDraw, this));
		m_updateRegistered = true;
		this->SetPreDraw(false);
	}
	RFSceneManager::SetFreshInputDisabled(true);
	RFSceneManager::Instance()->SetMouseOverForm(true);
	this->SetPriority(-3);
	T::ShowModal();
	AddModal();
	RFSceneManager::Instance()->GetManager()->FocusWindow(this);
}

#endif // PG_JP_RFFORMMODALBASE_H
