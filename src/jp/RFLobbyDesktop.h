// The JP 983 lobby's Refresh desktop, the window of lobby_desktop.xml (RTTI
// RFLobbyDesktop : RFFormDesktopBase<RFWndEx>, vtable 0xd1329c, 36 slots,
// size 0x2cc; constructor 0x5ffc60, which also stores the instance in the
// global 0xe78c38 and calls 0x5ff8f0). Created by 0x5fff30 (called from
// 0xabc356 and 0x7adab6) through the CreateWnd/CreateDesktop template
// instances 0x5ffe10/0x5ffea0 (docs/JP_REFRESH_UI.md, "Game call sites").
// The handlers are member functions bound in OnInitialUpdate with
// RFWnd::RegisterTo through MemberFunctionFunctor<RFLobbyDesktop> (vtable
// 0xd13218; its call slot 0x5f6de0 calls the member and returns true).
// Each handler's comment gives the KR CTaskMain/CLobbyMain code it
// corresponds to. Declarations and the JP x86 layout only.
#ifndef PG_JP_RFLOBBYDESKTOP_H
#define PG_JP_RFLOBBYDESKTOP_H

#include <stdint.h>
#include <map>
#include <string>

#include "defines.h"
#include "padding.h"
#include "RFFormDesktopBase.h"
#include "RFWndEx.h"

class Bitmap;
class EventArgs;
class WFont;
class CTaskMain;
class CLobbyMain;
class ntAvatarChatMain;
class MsgObject;

class RFLobbyDesktop : public RFFormDesktopBase<RFWndEx> {
	LAYOUT_CHECK_FRIEND;
	// The pop-up menu's handlers (JpLobbyDialogs.cpp) call FocusPopupPanel,
	// OnKillFocusPopup and the OnPopup*Clicked handlers below.
	friend class RFLobbyPopupMenuWnd;
	// The shop page's glue (JpShopWidget.cpp) runs the protected RefreshOnShow
	// on s_instance; defined in RFLobbyDesktop.cpp.
	friend void JpLobbyDesktop_RefreshOnShow(void);
	// The shop and My Room tab changes (JpShopMain 0xa90000 / 0xa8fb10,
	// JpRealMyRoomMain SetSubTab 0x61d7a0 / 0x61dd00) run the
	// protected SetBarSelection and UpdateLobbyCamera on s_instance
	// (JpLobbyGlue.h); defined in RFLobbyDesktop.cpp.
	friend void JpLobbyDesktop_SetBarSelection(int id);
	friend void JpLobbyDesktop_UpdateLobbyCamera(void);
	// The lobby player glue (JpLobbyPlayer.cpp) sets m_unknown244 / m_avatarPending
	// and reads m_unknown245 (0x568600, 0x566dee); the caddie's (JpLobbyCaddie.cpp,
	// defined in JpLobbyPlayer.cpp) sets m_unknown245 and reads m_unknown244
	// (0x560d70, 0x5604b0).
	friend void Jp_LobbyDesktop_SetShopCameraZoom(bool value);
	friend void Jp_LobbyDesktop_SetAvatarPending(bool value);
	friend bool Jp_LobbyDesktop_GetCaddieCameraZoom();
	friend void Jp_LobbyDesktop_SetCaddieCameraZoom(bool value);
	friend bool Jp_LobbyDesktop_GetShopCameraZoom();
public:
	// 0x5ffc60
	RFLobbyDesktop();
	// [1] 0x5f7ee0: scalar deleting destructor (body 0x5f7b70).
	virtual ~RFLobbyDesktop();

	// [3] Initialize 0x5f7840: inherited from RFFormDesktopBase.

	// Names ours (the exe has no symbols for them).
	static RFLobbyDesktop* s_instance;  // 0xe78c38 set by the constructor 0x5ffc60
	// The character the select window's carousel shows (15 without the
	// window); this is unused.
	int GetSelectedCharacterTypeId();      // 0x5f6410
	static int s_mode;                  // 0xe78c3c 0 lobby, 1 avatar chat
	// 0xe78c48: the layout names and their bar states ("" 0, "lobbymain" 1,
	// "shop|char" 2, "shop|caddie" 3, "shop|item" 4, "myroom|char" 5,
	// "myroom|caddie" 6, "myroom|item" 7, "tutorial" 8), filled by
	// InitAfterConstruct (operator[] 0x5ff690 is the std::map instance).
	static std::map<std::string, int> s_layoutStates;

	// Ours (0x566f30 reads +0x1fc directly).
	int32_t GetLobbyState() const { return m_lobbyState; }

protected:
	// [10] 0x5fd000: binds every control (below), then reads the task's
	// actors: m_taskMain (the current task's +0x30), m_lobbyMain ("Lobby"),
	// m_avatarChatMain ("AvatarChat"); "<CTaskMain> or <CLobbyMain> load
	// error." / "<CTaskMain> or <ntAvatarChatMain> load error." by mode
	// (0xe78c3c 0 / 1), setting 0xf02419 on failure; then 0x5f7210, five
	// ShowChild calls, the textarea_bubble child, the three number fonts,
	// the sub-windows (character select, carousel, pop-up menu, messenger
	// bubble), RefreshBarButtons, LoadBannerBitmaps,
	// RefreshOnShow_5fae60(false), the loading dialog.
	virtual void OnInitialUpdate();

public:
	// [11] 0x5f7080: sends message 0x183 to the LobbyBg actor and 0x184 to
	// LobbyAvatar, LobbyCaddie, LobbyMascot; Jp_GameState_SetLobbyCameraEnabled(true) (JP +0x10b27);
	// 0x566f30(0); then RFFormDesktopBase::OnDestroy's body (inlined).
	virtual void OnDestroy();
protected:
	// [14] 0x5f8d20: RFWnd::OnLayoutChanged, then lays out
	// wnd_char_select_area.
	virtual void OnLayoutChanged(uint32_t flags);

public:
	// [34] 0x5ff4f0: hidden (false): m_lobbyState = m_lobbyState1f8 = 1;
	// shown: 0x5fbe40, the character carousel (0xe781b0, 0x5d3b90), the
	// "Lobby" actor's message 0x77 when the mode (0xe78c3c) is the lobby,
	// RFSceneManager 0x6038f0(0); then RFFormDesktopBase::ShowDesktop.
	virtual void ShowLobby(bool show);
	// [35] 0x5d5520 (placed in another object, among the character-select
	// windows' code): the lobby's game-message handler (the KR MsgObject,
	// id at +4): 1/2/0x53/0x54 m_inputFrozen and the Fresh input flag, 0x55
	// a layout's bar state, 0x56/0x57 the loading dialog, 0x59
	// SetBarSelection, 0x88 a window opened, 0xca-0xd1 NotifyTopBar /
	// NotifyMessenger / the bubble, 0x106 NotifyCarousel, 0x180 the end of
	// a bar state's camera move. Its caller and KR counterpart are not
	// traced yet.
	virtual void HandleMsg(const MsgObject& msg);

protected:
	// --- The desktop's own events (the window's name, the empty string
	// 0xd62d40).
	// 0x5f67b0, EventFinalize: RFTooltipHelper (0xe74da8) 0x5dfb90.
	void OnFinalize(EventArgs& args);
	// 0x5fcb10, EventUpdate: per-frame 0x5f9610, 0x5f9780, in lobby mode
	// 0x5f7310 and 0x5fc490, then 0x5f9150 and 0x5f5ef0(dt).
	void OnUpdateDesktop(EventArgs& args);
	// 0x5f98f0, EventLButtonDown: closes the character carousel
	// (0xe781b8, wnd_char_select_anim_area) when it is open.
	void OnLButtonDownDesktop(EventArgs& args);
	// 0x5f67d0, EventKeyDown: without a modal window or open Fresh dialog,
	// Esc / Enter go to the character carousel (0xe781b8, 0x5d35b0).
	void OnKeyDownDesktop(EventArgs& args);
	// 0x5f6960, EventDraw: 0x5f64e0.
	void OnDrawDesktop(EventArgs& args);
	// 0x5f9a00, wnd_special_card_empty EventUpdate.
	void OnUpdateSpecialCardEmpty(EventArgs& args);

	// --- panel_popup (the pop-up menu's place).
	// 0x5fb260, EventKeyDown: Esc closes the pop-up menu (0x5fafb0(0)).
	void OnKeyDownPopup(EventArgs& args);
	// 0x5fb2d0, EventKillFocus: closes the pop-up menu.
	void OnKillFocusPopup(EventArgs& args);
	// 0x5f8e50 (name ours): RFManager::FocusWindow on panel_popup (the
	// pop-up menu's EventKillFocus, when the mouse is outside the menu).
	void FocusPopupPanel();

	// --- The pop-up menu's buttons (RFLobbyPopupMenuWnd forwards each
	// RFButton::EventClicked here). Each calls the task's code, then closes
	// the menu (ShowPopupMenu(false)). Names ours.
	// 0x5fb620 (btn_scratch): lobby → 0x658e70 on m_taskMain (JP only, the
	// gacha web page); avatar chat → m_avatarChatMain vtable +0x50 = KR
	// CTaskMain::OnUnderBar_GuildUp's slot.
	void OnPopupScratchClicked(EventArgs& args);
	// 0x5fb670 (btn_bong): m_taskMain vtable +0x54 = KR
	// CTaskMain::OnUnderBar_BongdariShopUp (non-virtual in KR).
	void OnPopupBongClicked(EventArgs& args);
	// 0x5fb690 (btn_itemtransform): 0x659340 on m_taskMain (JP only, the
	// "AlchemyDlg" dialog).
	void OnPopupItemTransformClicked(EventArgs& args);
	// 0x5fb6b0 (btn_quest): lobby → 0x593990 on m_lobbyMain; avatar chat →
	// m_avatarChatMain vtable +0x68 (both JP only, packet 0x151).
	void OnPopupQuestClicked(EventArgs& args);
	// 0x5fb700 (btn_notice): m_taskMain vtable +0x60 = KR
	// CTaskMain::OnUnderBar_NoticeUp (non-virtual in KR).
	void OnPopupNoticeClicked(EventArgs& args);
	// 0x5fb730 (btn_ranking): m_taskMain vtable +0x58 = KR
	// CTaskMain::OnUnderBar_RankingUp (non-virtual in KR; the JP
	// ntAvatarChatMain overrides the slot).
	void OnPopupRankingClicked(EventArgs& args);
	// 0x5fb760 (btn_achieve): 0x6582f0 on m_taskMain (JP only, packet 0x157).
	void OnPopupAchieveClicked(EventArgs& args);
	// 0x5fb780 (btn_myguild): m_taskMain or m_avatarChatMain by mode, vtable
	// +0x5c (JP only, the club web page).
	void OnPopupMyGuildClicked(EventArgs& args);
	// 0x5fb7e0 (btn_center): 0x658e20 on m_taskMain (JP only, the entrance
	// web page).
	void OnPopupCenterClicked(EventArgs& args);
	// 0x5fb800 (btn_gacha): 0x658e70 on m_taskMain (JP only, the gacha web
	// page).
	void OnPopupGachaClicked(EventArgs& args);

	// --- Under bar, RFButton::EventClicked. Each returns at once while
	// m_busy is set (except shop/my room, which test it on their KR path).
	// 0x5f74e0 (btn_game and btn_talk): RFSceneManager 0x6038f0(1), the
	// task's 0x9b7810 slot 2(1), then 0x6586c0 on m_taskMain = KR
	// CTaskMain::OnUnderBar_GameUp (JP: "roomlistselector", "TOPPAGE").
	void OnGameClicked(EventArgs& args);
	// 0x5f7540 (btn_practice): the same preamble, then 0x595f60 on
	// m_lobbyMain (the "single_mode_start" dialog). KR: no under-bar
	// button; its KR counterpart is not found yet.
	void OnPracticeClicked(EventArgs& args);
	// 0x5fcb70 (btn_shop): unless the "shopmain" layout is open: avatar
	// chat → m_avatarChatMain vtable +0x44 and m_pendingTaskChange = 1; lobby
	// → m_taskMain vtable +0x44 = KR CTaskMain::OnUnderBar_ShopUp (virtual),
	// 0x5fbe70(2), 0x5f8020.
	void OnShopClicked(EventArgs& args);
	// 0x5fcbf0 (btn_myroom): the same with "realmyroom", vtable +0x48 = KR
	// CTaskMain::OnUnderBar_MyRoomUp (virtual), 0x5fbe70(5).
	void OnMyRoomClicked(EventArgs& args);
	// 0x5f5f60 (btn_cookie, and btn_pang_cookie on the top bar): m_taskMain
	// vtable +0x4c = KR CTaskMain::OnUnderBar_CookieUp (virtual).
	void OnCookieClicked(EventArgs& args);
	// 0x5f9b00 (btn_post): when m_postPending, btn_post first goes back to
	// the post_n/o/d.tga images and m_postPending is cleared (0x5f9b47);
	// then 0x658420 on m_taskMain = KR CTaskMain::OnUnderBar_GiftUp (JP:
	// the "postbox" dialog).
	void OnPostClicked(EventArgs& args);
	// 0x5f75a0 (btn_channel): posts message 0x112 through the current
	// task's vtable +0x30. KR: CTaskMain::OnUnderBar_ServerUp (reached through
	// the message; not confirmed).
	void OnChannelClicked(EventArgs& args);
	// 0x5fb280 (btn_popup): toggles the pop-up menu (0x5f9090 open?,
	// 0x5fafb0(show), 0x5fa410). KR: CTaskMain::OnUnderBar_TabBtnUp (opens
	// utab).
	void OnPopupClicked(EventArgs& args);
	// 0x5f8470 (btn_exit): lobby mode → 0x658a20 on m_taskMain = KR
	// CTaskMain::OnUnderBar_ExitUp ("AVATARCHAT_MAIN", "quit"); avatar chat
	// → the CQuitWindow-like 0xe741d0 (0x6f0250) or a message box.
	void OnExitClicked(EventArgs& args);

	// --- Top bar, RFButton::EventClicked.
	// 0x5f9c40 (btn_spcard): the button's screen rect as a WRect, then
	// 0x656be0 on m_taskMain ("spcardbuff"), the JP
	// CTaskMain::OnOverBar_SpecialCardUp, which takes that rect (the KR one
	// takes none); then 0x5f92f0.
	void OnSpecialCardClicked(EventArgs& args);
	// 0x5fcc70 (btn_messenger): 0x5fb190, 0x65b7d0 on m_taskMain = KR
	// CTaskMain::OnOverBar_MessengerUp; 0x5fc520(0xcd).
	void OnMessengerClicked(EventArgs& args);
	// 0x5f5fa0 (btn_myinfo): 0x65ce40 on m_taskMain = KR
	// CTaskMain::OnOverBar_MyInfoUp.
	void OnMyInfoClicked(EventArgs& args);
	// 0x5f5fb0 (btn_option): 0x659d30 on m_taskMain ("option") = KR
	// CTaskMain::OnOverBar_OptionUp.
	void OnOptionClicked(EventArgs& args);
	// 0x5f5fc0 (btn_ad): 0x65d610 on m_taskMain ("notify", "oneline_req") =
	// KR CTaskMain::OnOverBar_OneLineReqBtnUp.
	void OnAdClicked(EventArgs& args);
	// 0x5f5fd0 (btn_premium): 0x596370 on m_lobbyMain (the "notice_popup"
	// dialog with premiumservice.ini). KR: none (JP only).
	void OnPremiumClicked(EventArgs& args);

	// --- Top bar, EventMouseEnter / EventMouseLeave: the tooltips of
	// RFTooltipHelper (0xe74da8; enter 0x5e01c0, leave 0x5dfad0). KR: the
	// Fresh buttons' hover tooltips (CIconToolTip).
	void OnPangCookieEnter(EventArgs& args);    // 0x5f5f80 empty
	void OnPangCookieLeave(EventArgs& args);    // 0x5f5f90 empty
	void OnServerNameEnter(EventArgs& args);    // 0x5fb440 (pic_server_name: m_serverName, m_channelName)
	void OnServerNameLeave(EventArgs& args);    // 0x5f6870
	void OnSpecialCardEnter(EventArgs& args);   // 0x5f8580
	void OnSpecialCardLeave(EventArgs& args);   // 0x5f6890
	void OnMessengerEnter(EventArgs& args);     // 0x5f8640
	void OnMessengerLeave(EventArgs& args);     // 0x5f68b0
	void OnMyInfoEnter(EventArgs& args);        // 0x5f8700
	void OnMyInfoLeave(EventArgs& args);        // 0x5f68d0
	void OnOptionEnter(EventArgs& args);        // 0x5f87c0
	void OnOptionLeave(EventArgs& args);        // 0x5f68f0
	void OnAdEnter(EventArgs& args);            // 0x5f6910 empty
	void OnAdLeave(EventArgs& args);            // 0x5f6940
	void OnPremiumEnter(EventArgs& args);       // 0x5f8880
	void OnPremiumLeave(EventArgs& args);       // 0x5f6920

	// --- Top bar, EventDraw: the numbers drawn by the game's fonts.
	// 0x5f7610 (wnd_pang): m_pangText with m_pangFont through the WView
	// (0xf023e8, 0xb61750), right-aligned in the window's screen rect.
	// KR: CTaskMain::OnOverBar_PangCookieOwnerDraw.
	void OnDrawPang(EventArgs& args);
	// 0x5f7710 (wnd_cookie): m_cookieText with m_cookieFont, the same way.
	void OnDrawCookie(EventArgs& args);
	// 0x5f6970 (wnd_won): 0xa62c60 on 0xe47db0.
	void OnDrawWon(EventArgs& args);
	// 0x5f6980 (label_ad): the ad ticker (0xe78c58, 0x897c40). KR:
	// CTaskMain::OnOverBar_OneLineViewOwnerDraw.
	void OnDrawAd(EventArgs& args);

	// --- The desktop's own helpers the handlers call (names ours).
	void InitAfterConstruct();             // 0x5ff8f0, the constructor's last call: fills s_layoutStates
	void RefreshOnShow();                  // 0x5fbe40, ShowLobby(true)'s first call
	void SetBarSelection(int id);          // 0x5fbe70 (the ids of s_layoutStates: 1 lobby, 2 shop, 5 my room ...)
	void UpdateLobbyCamera();              // 0x5f8020
	void OnBarSelectionChanged(int id);    // 0x5fafe0, SetBarSelection's last call
public:
	// 0x55fe80 (placed far from the class's other code): the shop states
	// of s_layoutStates (shop|char, shop|caddie, shop|item).
	bool IsShopState() {  // 0x55fe80
		if (m_lobbyState == 2 || m_lobbyState == 3 || m_lobbyState == 4) {
			return true;
		}
		return false;
	}
	// 0x5fb250: shows wnd_special_card_empty (the JP
	// CTaskMain::OnOverBar_SpecialCardUp with no special card).
	void ShowSpecialCardEmpty();           // 0x5fb250
	// m_busy read inline by the JP CTaskMain::ProcessLobbyKeys (0x65c77e):
	// ESCAPE does not go back while the desktop is busy.
	bool IsBusy() const { return m_busy; }
protected:
	// 0x5b8740 is this template's RFButton instance (GetChild then
	// dynamic_cast); some callers use it, others inline the same two steps.
	template <class T>
	T* GetChildAs(std::wstring const& name) {
		return dynamic_cast<T*>(GetChild(name));
	}
	bool IsPopupMenuOpen();                // 0x5f9090
	void ShowPopupMenu(bool show);         // 0x5fafb0
	void NotifyTopBar(int id);             // 0x5f92f0 (0xca from btn_spcard)
	void NotifyMessenger(int id);          // 0x5fc520 (0xcc-0xd1; 0xcd from btn_messenger)
	void SetMessengerBubble(int index, std::string const& text);  // 0x5fb190
	void DrawDesktop();                    // 0x5f64e0 (the banner, while m_bannerShown)
	void ResetBanner();                    // 0x5f5f40 (DrawDesktop once the banner has faded out)
	void NotifyCarousel();                 // 0x5f6450 (HandleMsg 0x106)
	static int GetLayoutState(std::string const& name);  // 0x5f8af0 (s_layoutStates lookup)
	// 0x5fa270 (name in edx): shows or hides the named child.
	void ShowChild(const wchar_t* name, bool show);
	// 0x5fa410 (name in edx): RFManager::FocusWindow on the named child.
	void FocusChild(const wchar_t* name);
	// 0x5fa340 (name in edx): RFWnd::EnableWindow on the named child.
	void EnableChild(const wchar_t* name, bool enable);
	void InitialUpdate_5f7210();           // 0x5f7210, OnInitialUpdate's first call after the actors: the lobby actors' start messages
	// RefreshOnShow's calls.
	void RefreshOnShow_5f8380();           // 0x5f8380: m_pangText, m_cookieText, m_string1dc from the game state
	void RefreshOnShow_5fa980();           // 0x5fa980: label_channel_name, the server notice, btn_talk / btn_game
	void RefreshOnShow_5f8f20();           // 0x5f8f20 (through the thunk j_sub_5f8f20): the ad ticker's place
	// 0x5fae60 (1 from RefreshOnShow, 0 from OnInitialUpdate): btn_premium
	// follows the game state's premium bit (the banner is shown when it
	// turns on and `showBanner`), then m_premiumText from the premium item's
	// remaining time.
	void RefreshOnShow_5fae60(bool showBanner);
	// 0x5f6470 (name ours): the banner's bitmaps, then ResetBanner.
	void LoadBannerBitmaps();
	// 0x5fa7c0 (name ours): btn_talk / btn_game, the bar's enabled buttons
	// by mode, the character select window, the tooltips' reset.
	void RefreshBarButtons();
	// OnUpdateDesktop's per-frame calls.
	// 0x5f9610: while m_specialCardImageSet, btn_spcard toggles between over and
	// normal every second (m_specialCardBlinkTime).
	void UpdateSpecialCardBlink(float dt);
	// 0x5f9780: the same for btn_messenger while m_messengerBlink.
	void UpdateMessengerBlink(float dt);
	void UpdateLobby_5f7310();             // 0x5f7310 (lobby mode only): the wheel zooms the shop / caddie camera
	// 0x5fc490 (lobby mode only): m_pendingTaskChange by bar state; in
	// state 9, back to the lobby bar once no Fresh layout is up.
	void UpdateLobby_5fc490();
	void Update_5f9150();                  // 0x5f9150: input freeze, enabled state, m_lobbyCovered
	void UpdateBannerFade(float dt);       // 0x5f5ef0

	// 0x18c-0x197: the KR actors the handlers call.
	CTaskMain* m_taskMain;                 // 0x18c the current task's main actor (task +0x30)
	CLobbyMain* m_lobbyMain;               // 0x190 the "Lobby" actor
	ntAvatarChatMain* m_avatarChatMain;    // 0x194 the "AvatarChat" actor
	WFont* m_pangFont;                     // 0x198 game font (vtable +0x18 measures, +0x24 height) for wnd_pang; the destructor releases it
	WFont* m_cookieFont;                   // 0x19c the same for wnd_cookie
	WFont* m_thirdFont;                    // 0x1a0 0 at construction
	std::string m_pangText;                // 0x1a4
	std::string m_cookieText;              // 0x1c0
	std::string m_string1dc;               // 0x1dc the cookies / 100, formatted (RefreshOnShow_5f8380)
	int32_t m_lobbyState1f8;               // 0x1f8 1 at construction and when hidden
	int32_t m_lobbyState;                  // 0x1fc 1 at construction and when hidden; HandleMsg's 0x180 switches on it
	bool m_lobbyCovered;                   // 0x200 Update_5f9150: the pop-up menu, the carousel or a modal covers the lobby
	float m_specialCardEmptyTime;          // 0x204 0; wnd_special_card_empty's shown time (OnUpdateSpecialCardEmpty)
	std::wstring m_wstring208;             // 0x208
	std::string m_string224;               // 0x224
	bool m_busy;                           // 0x240 the bar buttons do nothing while set; HandleMsg(0x180) clears it
	bool m_popupMenuOpen;                  // 0x241 ShowPopupMenu's last value
	bool m_inputFrozen;                    // 0x242 HandleMsg: set by messages 0x1 and 0x53, cleared by 0x2 and 0x54; Update_5f9150 passes it to RFManager::FreezeInputs
	bool m_shopCameraZoom;                 // 0x243 the camera's zoom: UpdateLobbyCamera picks "ShopCamera_z" over "ShopCamera" when set; UpdateLobby_5f7310 toggles it with the wheel; SetBarSelection clears it
	bool m_unknown244;                     // 0x244 UpdateLobby_5f7310: the shop camera zooms only while set
	bool m_unknown245;                     // 0x245 UpdateLobby_5f7310: the caddie camera zooms only while set
	bool m_messageBubbleShown;             // 0x246 HandleMsg(0xcc) sets it with the messenger bubble, clears it without
	bool m_premium;                        // 0x247 RefreshOnShow_5fae60: the game state's premium bit, as last shown on btn_premium
	int32_t m_specialCardImageSet;         // 0x248 NotifyTopBar's btn_spcard image set (0 normal, 1 premium, 2 on); UpdateSpecialCardBlink blinks btn_spcard while non-zero
	bool m_messengerBlink;                 // 0x24c NotifyMessenger sets / clears it; UpdateMessengerBlink blinks btn_messenger while set
	PADDING(0x24d, 0x250);
	float m_specialCardBlinkTime;          // 0x250 0; UpdateSpecialCardBlink's time since the last toggle
	float m_messengerBlinkTime;            // 0x254 0; the same for UpdateMessengerBlink
	bool m_avatarPending;                  // 0x258 SetBarSelection sends the LobbyAvatar its leave messages when set, then clears it
	bool m_caddiePending;                  // 0x259 the same for LobbyCaddie
	bool m_postPending;                    // 0x25a btn_post takes its other path when set
	PADDING(0x25b, 0x25c);
	// 0x25c-0x26c: zeroed by one memset (0x11 bytes). DrawDesktop draws
	// [0], [1], [2] side by side, centred, over [3] stretched across the
	// screen (the Fresh FrGraphicInterface::DrawTexture, JP 0xbec640).
	Bitmap* m_bannerBitmaps[4];            // 0x25c
	bool m_bannerShown;                    // 0x26c RefreshOnShow_5fae60 sets it; ResetBanner clears it
	PADDING(0x26d, 0x270);
	float m_bannerAlpha;                   // 0x270 the float at 0xcfec5c (constructor, ResetBanner); UpdateBannerFade lowers it to 0
	float m_bannerFadeSpeed;               // 0x274 0; ResetBanner: the float at 0xd0913c
	std::string m_serverName;              // 0x278 RefreshOnShow_5fa980: the game state's server name, as last shown
	std::string m_channelName;             // 0x294 RefreshOnShow_5fa980: the game state's channel name, as last shown
	std::string m_premiumText;             // 0x2b0 RefreshOnShow_5fae60: the premium item's remaining time
};
CHECK_SIZE(RFLobbyDesktop, 0x2cc);

// 0x5fff30 (name ours): creates the lobby desktop under parent, through
// CreateWnd (0x5ffe10) or CreateDesktop (0x5ffea0) depending on the global
// 0xe03268.
RFWnd* CreateLobbyDesktop(RFWnd* parent);
BEGIN_LAYOUT_CHECK(RFLobbyDesktop)
	CHECK_FIELD(RFLobbyDesktop, m_taskMain, 0x18c);
	CHECK_FIELD(RFLobbyDesktop, m_lobbyMain, 0x190);
	CHECK_FIELD(RFLobbyDesktop, m_avatarChatMain, 0x194);
	CHECK_FIELD(RFLobbyDesktop, m_pangFont, 0x198);
	CHECK_FIELD(RFLobbyDesktop, m_cookieFont, 0x19c);
	CHECK_FIELD(RFLobbyDesktop, m_thirdFont, 0x1a0);
	CHECK_FIELD(RFLobbyDesktop, m_pangText, 0x1a4);
	CHECK_FIELD(RFLobbyDesktop, m_cookieText, 0x1c0);
	CHECK_FIELD(RFLobbyDesktop, m_string1dc, 0x1dc);
	CHECK_FIELD(RFLobbyDesktop, m_lobbyState1f8, 0x1f8);
	CHECK_FIELD(RFLobbyDesktop, m_lobbyState, 0x1fc);
	CHECK_FIELD(RFLobbyDesktop, m_lobbyCovered, 0x200);
	CHECK_FIELD(RFLobbyDesktop, m_specialCardEmptyTime, 0x204);
	CHECK_FIELD(RFLobbyDesktop, m_wstring208, 0x208);
	CHECK_FIELD(RFLobbyDesktop, m_string224, 0x224);
	CHECK_FIELD(RFLobbyDesktop, m_busy, 0x240);
	CHECK_FIELD(RFLobbyDesktop, m_popupMenuOpen, 0x241);
	CHECK_FIELD(RFLobbyDesktop, m_inputFrozen, 0x242);
	CHECK_FIELD(RFLobbyDesktop, m_shopCameraZoom, 0x243);
	CHECK_FIELD(RFLobbyDesktop, m_unknown244, 0x244);
	CHECK_FIELD(RFLobbyDesktop, m_messageBubbleShown, 0x246);
	CHECK_FIELD(RFLobbyDesktop, m_specialCardImageSet, 0x248);
	CHECK_FIELD(RFLobbyDesktop, m_messengerBlink, 0x24c);
	CHECK_FIELD(RFLobbyDesktop, m_specialCardBlinkTime, 0x250);
	CHECK_FIELD(RFLobbyDesktop, m_messengerBlinkTime, 0x254);
	CHECK_FIELD(RFLobbyDesktop, m_avatarPending, 0x258);
	CHECK_FIELD(RFLobbyDesktop, m_caddiePending, 0x259);
	CHECK_FIELD(RFLobbyDesktop, m_postPending, 0x25a);
	CHECK_FIELD(RFLobbyDesktop, m_bannerBitmaps, 0x25c);
	CHECK_FIELD(RFLobbyDesktop, m_bannerShown, 0x26c);
	CHECK_FIELD(RFLobbyDesktop, m_bannerAlpha, 0x270);
	CHECK_FIELD(RFLobbyDesktop, m_bannerFadeSpeed, 0x274);
	CHECK_FIELD(RFLobbyDesktop, m_serverName, 0x278);
	CHECK_FIELD(RFLobbyDesktop, m_channelName, 0x294);
	CHECK_FIELD(RFLobbyDesktop, m_premiumText, 0x2b0);
END_LAYOUT_CHECK

// 0x5fbe40 on s_instance (0xe78c38) when there is one: the JP shop page's
// inline call after a purchase / the cash refresh (CShopWidget::OnPacket
// 0x5e0ff0). Callers that cannot include this header (R_OK) declare it.
void JpLobbyDesktop_RefreshOnShow(void);

#endif
