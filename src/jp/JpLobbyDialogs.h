// Two JP 983 lobby windows the lobby desktop creates (RTTI names read from
// their vtables' complete object locators): RFLobbyPopupMenuWnd
// (lobby_popupmenu_wnd_jp.xml, CreateLobbyPopupMenuWnd 0x5d6170) and
// RFServerNoticeDlg (server_notice_dialog.xml, CreateServerNoticeDialog
// 0x5d7bd0), both : RFFormBase<RFWndEx>. Declarations and the JP x86
// layout, from the JP exe's bodies (node scripts/disasm.ts --jp 0x<va>);
// the members not written in JpLobbyDialogs.cpp are declared JP
// dependencies (each says why).
#ifndef PG_JP_JPLOBBYDIALOGS_H
#define PG_JP_JPLOBBYDIALOGS_H

#include "defines.h"
#include "padding.h"
#include "JpTooltipWnd.h"
#include "RFFormModalBase.h"

// The lobby's pop-up menu (vtable 0xd113d4, 34 slots: no new virtuals),
// size 0x184 (no members of its own).
class RFLobbyPopupMenuWnd : public RFFormBase<RFWndEx> {
public:
	// 0x5d5d20: RFWndEx::RFWndEx, then the instance is kept in
	// g_lobbyPopupMenuWnd (0xe781c0).
	RFLobbyPopupMenuWnd();
	// [1] 0x5d60b0 (scalar deleting destructor); ~RFLobbyPopupMenuWnd
	// 0x5d5d40 clears g_lobbyPopupMenuWnd, then restores
	// RFFormBase<RFWndEx>'s vtable and runs ~RFWndEx.
	virtual ~RFLobbyPopupMenuWnd();

	// 0xe781bc (name ours): the parent the last CreateLobbyPopupMenuWnd
	// was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5d62b0: OnKillFocus on its own EventKillFocus and on each
	// button's, then each button's RFButton::EventClicked handler.
	virtual void OnInitialUpdate();

	// The handlers below forward to the lobby desktop
	// (RFLobbyDesktop::s_instance, 0xe78c38) and do nothing without one.
	// 0x5d5d60 (name ours), EventKillFocus: when the mouse
	// (RFSceneManager::GetMousePosition) is outside the menu's screen rect,
	// the desktop's 0x5f8e50 (focuses its panel_popup); then the desktop's
	// OnKillFocusPopup (0x5fb2d0) with the args.
	void OnKillFocus(EventArgs& args);
	// RFButton::EventClicked of each button: the desktop's handler with the
	// args (a tail jump), each of which then closes the menu (0x5fafb0).
	void OnScratchClicked(EventArgs& args);        // 0x5d5dc0 btn_scratch: desktop 0x5fb620
	void OnBongClicked(EventArgs& args);           // 0x5d5de0 btn_bong: desktop 0x5fb670
	void OnItemTransformClicked(EventArgs& args);  // 0x5d5e00 btn_itemtransform: desktop 0x5fb690
	void OnQuestClicked(EventArgs& args);          // 0x5d5e20 btn_quest: desktop 0x5fb6b0
	void OnNoticeClicked(EventArgs& args);         // 0x5d5e40 btn_notice: desktop 0x5fb700
	void OnRankingClicked(EventArgs& args);        // 0x5d5e60 btn_ranking: desktop 0x5fb730
	void OnAchieveClicked(EventArgs& args);        // 0x5d5e80 btn_achieve: desktop 0x5fb760
	void OnMyGuildClicked(EventArgs& args);        // 0x5d5ea0 btn_myguild: desktop 0x5fb780
	void OnCenterClicked(EventArgs& args);         // 0x5d5ec0 btn_center: desktop 0x5fb7e0
	void OnGachaClicked(EventArgs& args);          // 0x5d5ee0 btn_gacha: desktop 0x5fb800
};
CHECK_SIZE(RFLobbyPopupMenuWnd, 0x184);

// The server notice dialog (vtable 0xd1166c, 35 slots: [34] new), size
// 0x188. The constructor keeps the instance in g_serverNoticeDlg (0xe781d0,
// which RFLobbyDesktop's RefreshOnShow_5fa980 tests before showing it).
// It closes itself once m_closeDelay (1 second) has run out.
class RFServerNoticeDlg : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5d7910: RFWndEx::RFWndEx, m_closeDelay = 1.0f, then the instance is
	// kept in g_serverNoticeDlg.
	RFServerNoticeDlg();
	// [1] 0x5d7b00 (scalar deleting destructor); ~RFServerNoticeDlg
	// 0x5d7930 clears g_serverNoticeDlg, then restores
	// RFFormBase<RFWndEx>'s vtable and runs ~RFWndEx.
	virtual ~RFServerNoticeDlg();

	// [34] 0x5d7bc0 (name ours): shows or hides the dialog (a tail jump to
	// RFFormBase<RFWndEx>::ShowForm's copy 0x5b9cd0).
	virtual void Show(bool show);

	// 0x5d78f0 / 0x5d7900 (names ours; uncalled, the dialog in ecx): keep
	// the instance in g_serverNoticeDlg / clear it, the stores the
	// constructor and destructor make.
	static void SetInstance(RFServerNoticeDlg* dialog);
	static void ClearInstance();

	// 0xe781cc (name ours): the parent the last CreateServerNoticeDialog
	// was given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5d7f70: OnUpdateNotice on its own EventUpdate.
	virtual void OnInitialUpdate();

	// 0x5d7d10 (name ours), EventUpdate: counts m_closeDelay down by the
	// elapsed time; when it reaches 0, plays "_Closing_"; once at 0 and
	// visible with no animation playing, closes the window (state 1).
	void OnUpdateNotice(EventArgs& args);

	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	float m_closeDelay;  // 0x184 seconds left before closing: 1.0f at construction
};
CHECK_SIZE(RFServerNoticeDlg, 0x188);
BEGIN_LAYOUT_CHECK(RFServerNoticeDlg)
	CHECK_FIELD(RFServerNoticeDlg, m_closeDelay, 0x184);
END_LAYOUT_CHECK

// The lobby's messenger bubble (RTTI RFLobbyMessengerBubble :
// RFFormBase<RFWndEx>, vtable 0xd1318c, 34 slots: no new virtuals), loaded
// from lobby_messenger_bubble_wnd.xml by CreateMessengerBubble 0x5f9cb0
// under panel_notice_box. Size 0x188.
class RFLobbyMessengerBubble : public RFFormBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
public:
	// 0x5f69a0: RFWndEx::RFWndEx, m_displayTime = 0.0f, then the instance
	// is kept in g_messengerBubble.
	RFLobbyMessengerBubble();
	// [1] 0x5f7810 (scalar deleting destructor); ~RFLobbyMessengerBubble
	// 0x5f69c0 clears g_messengerBubble, then restores
	// RFFormBase<RFWndEx>'s vtable and runs ~RFWndEx.
	virtual ~RFLobbyMessengerBubble();

	// 0x5f9f10 (name ours): hides the bubble; its notice_box_text text area
	// gets text (Shift-JIS), wnd_notice_box the text's height + 14, the
	// bubble and s_loadParent wnd_notice_box's rect; then shown, "_Opening_"
	// played, m_displayTime = 5 seconds.
	void SetText(std::string const& text);
	// 0x5f7f10 (name ours): ShowForm(false), m_displayTime = 0.0f.
	void Hide();

	// 0xe78c40 (name ours): the parent the last CreateMessengerBubble was
	// given.
	static RFWnd* s_loadParent;

protected:
	// [10] 0x5f9df0: OnUpdateBubble on its own EventUpdate, SetPreDraw(false).
	virtual void OnInitialUpdate();

	// 0x5f7f30 (name ours), EventUpdate: while visible, counts
	// m_displayTime down by the elapsed time and hides the bubble when it
	// reaches 0.
	void OnUpdateBubble(EventArgs& args);

	// 0x0-0x184: RFFormBase<RFWndEx> / RFWndEx.
	float m_displayTime;  // 0x184 seconds left on screen: 5 from SetText, 0 at construction and on Hide
};
CHECK_SIZE(RFLobbyMessengerBubble, 0x188);
BEGIN_LAYOUT_CHECK(RFLobbyMessengerBubble)
	CHECK_FIELD(RFLobbyMessengerBubble, m_displayTime, 0x184);
END_LAYOUT_CHECK

// The lobby's loading/waiting dialog (RTTI RFLoadingProgressDlg :
// RFFormModalBase<RFWndEx>, vtable 0xd10e94, 35 slots: [34] new; the
// RFModalRenderer sub-object at +0x180 has vtable 0xd10e8c), loaded from
// common_loading_waiting_dialog.xml by CreateLoadingWaitingDialog 0x5d1870.
// Size 0x188 (no members of its own).
class RFLoadingProgressDlg : public RFFormModalBase<RFWndEx> {
public:
	// 0x5d15b0: RFWndEx::RFWndEx, RFModalRenderer (0x5e51f0),
	// m_updateRegistered cleared, then the instance is kept in
	// g_loadingWaitingDialog.
	RFLoadingProgressDlg();
	// [1] 0x5d17d0 (scalar deleting destructor; the sub-object's [0]
	// 0x5d1630 adjusts this by -0x180 and jumps to it);
	// ~RFLoadingProgressDlg 0x5d1640 clears g_loadingWaitingDialog, then
	// restores RFFormModalBase<RFWndEx>'s vtables and runs ~RFModalRenderer
	// (0x5e5200) and ~RFWndEx.
	virtual ~RFLoadingProgressDlg();

	// 0xe781a0 (name ours): the parent the last CreateLoadingWaitingDialog
	// was given.
	static RFWnd* s_loadParent;
	// 0xe781a8 (name ours): cleared by OnDestroy, its only reference in the
	// exe (its meaning is not known).
	static int s_clearedOnDestroy;

protected:
	// [10] 0x5d15a0: SetPreDraw(false).
	virtual void OnInitialUpdate();
	// [11] 0x5d1800: s_clearedOnDestroy = 0, RFFormModalBase's OnDestroy,
	// then the Fresh input flag +0xa9 cleared.
	virtual void OnDestroy();

public:
	// [34] 0x5d19b0 (name ours): ShowModal, plays "_Opening_", then the
	// Fresh input flag +0xa9 set (RFSceneManager::SetFreshInputSuspended).
	virtual void Open();
};
CHECK_SIZE(RFLoadingProgressDlg, 0x188);

// 0xe03260 (name ours): which creator the modal forms' create functions use
// (CreateLoadingWaitingDialog, the common use-item / notice dialogs, the
// mastery dialogs): 0 CreateWnd under the parent, 1 CreateWnd without one,
// 2 CreateDesktopWnd.
extern int g_jpModalCreateMode;

#endif // PG_JP_JPLOBBYDIALOGS_H
