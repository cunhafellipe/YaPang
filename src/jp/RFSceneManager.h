// The JP 983 game's owner of Refresh (RTTI RFSceneManager :
// WSingleton<RFSceneManager> : BaseObject, vtable 0xd13d70, size 0x48;
// constructor 0x604090, destructor 0x603d90; instance pointer 0xe73ddc).
// Created by the JP InitSingletonClasses (0xa65710, KR projectg.cpp:349),
// set up by CProjectG::Ready (0xa69400) through Setup (0x604160). Every
// game call into RFManager's Inject* and frame entry points goes through
// its wrappers. Declarations and the JP x86 layout only.
#ifndef PG_JP_RFSCENEMANAGER_H
#define PG_JP_RFSCENEMANAGER_H

#include <stdint.h>
#include "RefreshSharedPtr.h"

#include <windows.h>

#include "defines.h"
#include "padding.h"
#include "singleton.h"

class WVector;

class RFManager;
class RefreshInterface;
class RefreshRenderer;
class RefreshInputHandler;

// 0xf022d0 is the JP g_pFresh and its +0x40 the JP g_pFresh->GetManager()
// (CreateForm<FrNoticeDlg> 0x595560 gets it where the KR
// CLobbyMain::OnToppage_PcbangUp passes g_pFresh->GetManager(), KR +0x3c).
// The port runs the KR Fresh: the JP FrWndManager members the game reads
// (+0xa9, +0xaa, +0xac) are the KR FrWndManager's JP_PAKS members
// (frwndmanager.h), its +0x84 the KR m_pDevice (GetGDI()).

class RFSceneManager : public WSingleton<RFSceneManager> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x604090: members zeroed, an empty input handler pointer, m_enabled.
	RFSceneManager();
	// 0x603d90 ([0] scalar deleting destructor 0x603fa0): deletes the
	// RFTooltipHelper singleton (0xe74da8), the RFManager, the interface,
	// the renderer, then releases the input handler.
	virtual ~RFSceneManager();

	// 0x604160: new RefreshInterface, RefreshRenderer, RFManager (0x98),
	// RefreshInputHandler (shared_ptr); RFManager::SetInterface,
	// SetRenderer, InitializeInputMethod(the game HWND 0xf023ec, true);
	// SetResolution(the two floats at 0xd0562c/0xd05628, 800 x 600);
	// RFManager::SetVisualEffect; new RFTooltipHelper (0x80, 0x5dfbf0).
	bool Setup();
	// 0x6035b0: with a manager, RFWnd::SetSize + UpdateLayout on its root
	// window (+0x4c), RefreshRenderer 0x5f5490, then 0xb141b0 on the object
	// at 0xe74d18. Also called by 0x494120 (resolution change).
	void SetResolution(float width, float height);

	// 0x603850, from CProjectG::WinProc (0xa67dd0, call 0xa67f38):
	// WM_SETFOCUS → RFManager::RestoreInputMethod; WM_KEYDOWN, WM_KEYUP,
	// WM_LBUTTONUP → RefreshInputHandler::QueueMessage.
	void OnWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	// 0x603e80, from CTaskManager::ProcessTask (0x9c0bc0, call 0x9c0de6):
	// RFManager::InjectUpdate(dt), RefreshInputHandler::Update(dt), then
	// UpdateMouseOver and UpdateInputFocus.
	void Update(float dt);
	// 0x603290, from CTask::Display (0x9bb490, call 0x9bb6a9), when
	// m_enabled: RFManager::InjectPreDraw. Void: eax is left as it is.
	void PreDraw();
	// 0x6032b0, from CTaskManager::DisplayTask (0x9b96f0, call 0x9b974a):
	// when m_enabled: RFManager::InjectDraw. Void: eax is left as it is.
	void Draw();

	// The input wrappers RefreshInputHandler calls; each returns false
	// without a manager.
	bool InjectLButtonDown(float x, float y);    // 0x6032d0
	bool InjectLButtonUp(float x, float y);      // 0x603300
	bool InjectLButtonDblClick(float x, float y);  // 0x603330
	bool InjectRButtonDown(float x, float y);    // 0x603360
	bool InjectRButtonUp(float x, float y);      // 0x603390
	bool InjectMouseMove(float x, float y);      // 0x6033c0
	bool InjectMouseWheel(int delta, float x, float y);  // 0x6033f0
	bool InjectKeyDown(UINT key, UINT flags);    // 0x603420
	bool InjectKeyUp(UINT key, UINT flags);      // 0x603440

	// Ours: RefreshRenderer (0x5f56f0, 0x5f59d0, 0x5f5490) reads +0x28 and
	// +0x30 of the instance directly.
	RFManager* GetManager() const { return m_manager; }
	RefreshRenderer* GetRenderer() const { return m_renderer; }
	// 0x6038b0 (name ours): the input handler's mouse position
	// (RefreshInputHandler +0x2c), WVector::ZERO (0xf04154) without one.
	WVector const& GetMousePosition() const;
	// 0x6038d0 (name ours): the input handler's last wheel delta
	// (RefreshInputHandler +0x2c m_mouse, its m_wheel +0xc: +0x38), 0.0f
	// without one.
	float GetMouseWheel() const;

	// The input switches (names ours). None uses `this`: each reads the
	// instance pointer (0xe73ddc) and does nothing without it or without an
	// input handler, hence static.
	// 0x6038f0: the input handler's m_disabled = disabled, then its mouse
	// state is reset (0x5f38e0).
	static void SetInputDisabled(bool disabled);
	// 0x603920: the input handler's m_disabled; false without one.
	static bool IsInputDisabled();
	// 0x603ca0: SetFreshInputDisabled(disabled) (0x603610), then
	// SetInputDisabled(disabled).
	static void SetAllInputDisabled(bool disabled);
	// 0x603610 (name ours): with g_pFresh (JP 0xf022d0) and its
	// FrWndManager (+0x40, g_pFresh->GetManager()), stores `disabled` into
	// that manager's JP-only +0xaa (SetKeyInputDisabled) and +0xa9
	// (SetInputSuspended).
	static void SetFreshInputDisabled(bool disabled);
	// 0x603680 (name ours): either byte set.
	static bool IsFreshInputDisabled();
	// 0x603660 / 0x6036d0 (names ours): the same manager's +0xa9
	// (m_inputSuspended, SetInputSuspended) alone, set / read
	// (RFFormDesktopBase::SyncInputState).
	static void SetFreshInputSuspended(bool suspended);
	static bool IsFreshInputSuspended();
	// Ours (inlined everywhere): +0x41, whether the mouse is over a Refresh
	// form. RFFormBase::OnFormUpdate stores whether the mouse lies in the
	// form's screen rect; RFFormDesktopBase::ShowDesktop(true) sets it and
	// the forms' OnDestroy clear it.
	void SetMouseOverForm(bool over) { m_mouseOverForm = over; }
	// Ours (read inline by LobbyPlayer 0x566cd0 and others): the input
	// handler (+0x34), NULL before Setup.
	RefreshInputHandler* GetInput() const { return m_input.get(); }
	// 0x603710 (name ours): with the game's CMouseCursor (0xe73e0c), its
	// JP-only lock (+0x138) cleared, the JP SetCursor(cursor, 1) (0x891020),
	// then the lock and m_cursorLocked set to `lock`.
	void SetMouseCursor(int cursor, bool lock);

protected:
	// 0x603750 (name ours, this unused): CMouseCursor::GetCursor (JP
	// 0x891000), 0 without a cursor.
	int GetMouseCursor();
	// 0x603b90: unless the input is disabled, the cursor is locked or there
	// is no cursor or input handler, the cursor for the window under the
	// mouse (RFManager::HitTest2): buttons 1 when visible, a picture 0, an
	// edit 0xc when focusable or focused, a draggable window 8 (0 for a
	// button), nothing hit 0; set when it changed.
	void UpdateMouseOver();
	// 0x6039a0 (this unused): with a Fresh manager (JP g_pFresh 0xf022d0,
	// +0x40) whose layout matches the desktop's (the desktop's +0x17c, on
	// the exe's RFFormDesktopBase<RFWndEx> its RFWndEx::m_brushDesc, against
	// the manager's +0xac) or no desktop: when a Fresh layout is up
	// and no Refresh modal, the Refresh input is disabled and the Fresh one
	// enabled; otherwise the Refresh input is enabled again.
	void UpdateInputFocus();

	// 0x0-0x28: WSingleton<RFSceneManager> / BaseObject.
	RFManager* m_manager;                                    // 0x28
	RefreshInterface* m_interface;                           // 0x2c
	RefreshRenderer* m_renderer;                             // 0x30
	RF_SHARED_PTR<RefreshInputHandler> m_input;       // 0x34 (pointer 0x34, control block 0x38)
	void* m_unknown3c;                                       // 0x3c cleared by the constructor and destructor
	bool m_enabled;                                          // 0x40 1 at construction; PreDraw and Draw test it
	bool m_mouseOverForm;                                    // 0x41 SetMouseOverForm
	PADDING(0x42, 0x44);
	bool m_cursorLocked;                                     // 0x44 the lock SetMouseCursor last set
	PADDING(0x45, 0x48);
};
CHECK_SIZE(RFSceneManager, 0x48);
BEGIN_LAYOUT_CHECK(RFSceneManager)
	CHECK_FIELD(RFSceneManager, m_manager, 0x28);
	CHECK_FIELD(RFSceneManager, m_interface, 0x2c);
	CHECK_FIELD(RFSceneManager, m_renderer, 0x30);
	CHECK_FIELD(RFSceneManager, m_input, 0x34);
	CHECK_FIELD(RFSceneManager, m_unknown3c, 0x3c);
	CHECK_FIELD(RFSceneManager, m_enabled, 0x40);
	CHECK_FIELD(RFSceneManager, m_mouseOverForm, 0x41);
	CHECK_FIELD(RFSceneManager, m_cursorLocked, 0x44);
END_LAYOUT_CHECK

#endif
