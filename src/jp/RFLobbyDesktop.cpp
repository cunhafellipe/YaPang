// The JP 983 lobby's Refresh desktop (RFLobbyDesktop.h), from the JP exe's
// bodies (node scripts/jp_hlil.ts). Where a JP handler calls JP game code
// that the KR game has (the bar handlers of CTaskMain, docs/JP_REFRESH_UI.md,
// "The lobby desktop"), the KR function is called and the comment says so;
// JP-only game code is declared below and left unresolved.
//
// <windows.h> first: RFManager.h's CreateDesktop is spelled by windows.h's
// macro wherever the header is read (the JP object has CreateDesktopA, 0x402e30),
// so the declaration and the call below must both see it.
#include <windows.h>

#include "RFLobbyDesktop.h"

#include <string.h>

#include "ProjectG.h"
#include "QuitWindow.h"
#include "commonutil.h"
#include "binstr.h"
#include "wtypes.h"
#include "wfont.h"
#include "wview.h"
#include "bitmap.h"
#include "frgraphicinterface.h"
#include "fresh.h"

#include "RFButton.h"
#include "RFEventArgs.h"
#include "RFEventHandler.h"
#include "RFLabel.h"
#include "RFManager.h"
#include "RFTextArea.h"
#include "RFSceneManager.h"
#include "RFTooltipHelper.h"
#include "RefreshStringUtil.h"

#include "TaskManager.h"
#include "TaskMain.h"
#include "LobbyMain.h"
#include "avatar_main.h"
#include "actor.h"
#include "wresrcmng.h"
#include "SharedDoc.h"

#include "JpLobbyCharacterSelectWnd.h"
#include "JpLobbyDialogs.h"
#include "JpLobbyGlue.h"
#include "JpLobbyWidgets.h"
#include "RFTextManager.h"
#include "onelineboard.h"

// 0xf02419: the KR g_bQuit (ProjectG.cpp; CProjectG's frame quits once it
// is set). OnInitialUpdate's load-error path stores 1 there, as the JP
// "exit" button 0xadc2e0 does.
extern bool g_bQuit;

// More JP game code with no KR counterpart, not decompiled (declared only;
// names ours).

// 0xe781b8: the character carousel (RFLobbyCharacterSelectAnimWnd,
// JpLobbyCharacterSelectWnd.h; wnd_char_select_anim_area's owner). The
// desktop closes it with SetOpen(OpenAction_Close, true) and confirms it
// with SetOpen(OpenAction_Close, false) on Enter.
extern RFLobbyCharacterSelectAnimWnd* g_lobbyCharacterCarousel;
// 0xe78c44: the messenger bubble (RFLobbyMessengerBubble,
// JpLobbyDialogs.h); SetText 0x5f9f10 sets its text, Hide 0x5f7f10 hides
// it.
extern RFLobbyMessengerBubble* g_messengerBubble;
// 0xcff088: the JP double RefreshOnShow_5f8f20 adds to the ad ticker's y
// (the ticker is the KR COneLineBoard, JpLobbyWidgets.h).
extern const double g_lobbyAdTickerOffsetY;
// 0xa62c60: the CProjectG member wnd_won's draw calls (JP only; the KR
// CProjectG has no counterpart found).
void ProjectG_DrawWon(CProjectG* game);
// The keys the top-bar tooltips pass Jp_LookupText (0xb4eb80,
// RFTextManager.h; also btn_exit's message-box text, key 0xd13338); their
// texts are in the definitions below (names ours, from the windows that use
// them).
extern const char g_jpTextKeySpecialCardTooltip[];  // 0xd13364 (btn_spcard)
extern const char g_jpTextKeyMessengerTooltip[];    // 0xd13370 (btn_messenger)
extern const char g_jpTextKeyMyInfoTooltip[];       // 0xd13378 (btn_myinfo)
extern const char g_jpTextKeyOptionTooltip[];       // 0xd13380 (btn_option)
extern const char g_jpTextKeyServerNameLabel[];     // 0xd13868 (pic_server_name, first line)
extern const char g_jpTextKeyServerName2Label[];    // 0xd13838 (pic_server_name, second line)
// 0x5d3920: RFLobbyCharacterSelectAnimWnd::ResetCharacters, which
// HandleMsg(0x106) reaches through NotifyCarousel, with 0xf.
// 0x5d1870 (name ours): creates the loading/waiting dialog
// (RFLoadingProgressDlg, JpLobbyDialogs.h, from
// common_loading_waiting_dialog.xml) under parent; 0xe781a4 holds it while
// it is up. HandleMsg calls the created dialog's [34] Open.
RFWnd* CreateLoadingWaitingDialog(RFWnd* parent);
extern RFLoadingProgressDlg* g_loadingWaitingDialog;
// 0xe73e10 (an unidentified game object): HandleMsg(0x58) sets its byte
// +0xb0 when no Refresh modal window is up.
extern char* g_jpObjectE73e10;
// 0x540a10 (id in ecx): a text of the game's string table by id ("" when
// missing; the table is 0xe75114). Not identified: declared only.
const char* Jp_GetTableText(int id);
// 0xd05118: wnd_pang's number x offset (a double; wnd_cookie's 3.0 and the
// y offset 1.0 are the literals __real@4008000000000000 /
// __real@3ff0000000000000). 0xcfe8d4: wnd_special_card_empty's display
// time.
extern const double g_lobbyPangOffsetX;
extern const float g_specialCardEmptyTime;

// DrawDesktop's banner: the JP doubles that place the background strip
// (0xd05d90 above the banner's top, 0xd04f80 added to its height); the
// graphic interface is Jp_GetFreshGDI (JpLobbyGlue.h).
extern const double g_lobbyBannerStripTop;     // 0xd05d90
extern const double g_lobbyBannerStripExtra;   // 0xd04f80
// 0xd0913c: the banner's fade speed (ResetBanner).
extern const float g_lobbyBannerFadeSpeed;
// 0xb30710 (not placed): fills a zeroed SYSTEMTIME from a 64-bit time
// (thiscall on the SYSTEMTIME, the time as two dwords).
void Jp_SystemTimeFromTime(SYSTEMTIME* out, int64_t time);
// 0x6333c0 (in the JP SharedDoc unit, on 0xe47dbc): the hours left until
// the given time.
int64_t Jp_GameState_HoursUntil(SYSTEMTIME const& time);
// The premium texts' keys for Jp_LookupText: 0xd0ba30 the "%d days %d
// hours" format (through MakeStr), 0xd005b0 the text for no time left.
extern const char g_jpTextKeyPremiumRemain[];  // 0xd0ba30
extern const char g_jpTextKeyPremiumNone[];    // 0xd005b0

// OnInitialUpdate's tail: the textarea_bubble text's key (0xd13a74, for
// Jp_LookupText) and the number fonts' texture files (0xe13230, the
// tagWTITLEFONT filename list; the fonts have one page, so one entry).
extern const char g_jpTextKeySpecialCardEmpty[];  // 0xd13a74
extern char* g_lobbyNumberFontFiles[];           // 0xe13230

// 0xe47dbc's money (accessors ours, reading it): the pang (int64 +0x91e)
// and the cookies (int64 +0x3858) RefreshOnShow_5f8380 formats.
int64_t Jp_GameState_Pang();
int64_t Jp_GameState_Cookies();

// RefreshOnShow_5fa980's game state (0xe47dbc; accessors ours, reading it):
// the offline-play dword +0x10d30, the server name (char array +0x10aa9)
// and the channel name (char array +0x10a54; pic_server_name's tooltip
// labels them "접속서버" / "접속채널").
bool Jp_GameState_IsOfflinePlay();
const char* Jp_GameState_ServerName();
const char* Jp_GameState_ChannelName();
// 0xe781d0: the server notice dialog (RFServerNoticeDlg,
// JpLobbyDialogs.h); RefreshOnShow_5fa980 shows the server notice only
// while it and the loading dialog are NULL.
extern RFServerNoticeDlg* g_serverNoticeDlg;
// 0x5d7bd0 (name ours): creates the server notice dialog
// (server_notice_dialog.xml, JpLobbyDialogs.cpp) under parent; keeps parent
// in 0xe781cc. 0x5d8070 sets its text (ServerNoticeDialog_SetText,
// JpLobbyWidgets.h), its [34] Show shows it.
RFWnd* CreateServerNoticeDialog(RFWnd* parent);
void ServerNoticeDialog_Show(RFWnd* dialog, bool show);

// OnInitialUpdate's tail: the creators of the lobby's sub-windows (names
// ours; each builds its window under the given parent and keeps it in its
// global): 0x5d2980 the character select window (0xe781b0, under
// wnd_char_select_area), 0x5d2ef0 the character carousel (0xe781b8, under
// wnd_char_select_anim_area), 0x5d6170 the pop-up menu (0xe781c0, under
// panel_popup), 0x5f9cb0 the messenger bubble (0xe78c44, under
// panel_notice_box). Declared only.
RFWnd* CreateLobbyCharacterSelectWnd(RFWnd* parent);
RFWnd* CreateLobbyCharacterCarousel(RFWnd* parent);
RFWnd* CreateLobbyPopupMenuWnd(RFWnd* parent);
RFLobbyMessengerBubble* CreateMessengerBubble(RFWnd* parent);
// 0xe781c0: the pop-up menu window (OnInitialUpdate creates it when NULL).
extern RFWnd* g_lobbyPopupMenuWnd;
// 0x5b9cd0: what OnInitialUpdate calls with true on each created
// sub-window, a copy of RFFormBase<RFWndEx>::ShowForm (RFFormBase.h).


// 0x5ffea0: the CreateDesktop template's RFLobbyDesktop instance (one per
// screen class, docs/JP_REFRESH_UI.md "Game call sites"); its CreateWnd
// sibling 0x5ffe10 is RFManager.h's CreateWnd<T>.
// 0x5ffea0
template <class T>
RFWnd* CreateDesktop(RFManager* manager, std::wstring const& name) {
	T* wnd = new T;
	RFWnd* result = manager->CreateDesktop(name, wnd);
	if (result == NULL) {
		delete wnd;
		result = NULL;
	}
	return result;
}

// The JP data declared above, with the initial values of the JP exe
// (node scripts/disasm.ts --jp --data). The text keys are CP949 byte
// strings (Jp_LookupText's keys), the text in the comment.
// 0xe781b8
RFLobbyCharacterSelectAnimWnd* g_lobbyCharacterCarousel = NULL;
// 0xe78c44
RFLobbyMessengerBubble* g_messengerBubble = NULL;
// 0xcff088
const double g_lobbyAdTickerOffsetY = 1.5;
// 0xd13364
const char g_jpTextKeySpecialCardTooltip[] = "\xc8\xbf\xb0\xfa\xc1\xa4\xba\xb8";  // "효과정보"
// 0xd13370
const char g_jpTextKeyMessengerTooltip[] = "\xb8\xde\xbd\xc5\xc0\xfa";  // "메신저"
// 0xd13378
const char g_jpTextKeyMyInfoTooltip[] = "\xb3\xbb\xc1\xa4\xba\xb8";  // "내정보"
// 0xd13380
const char g_jpTextKeyOptionTooltip[] = "\xbf\xc9\xbc\xc7";  // "옵션"
// 0xd13868
const char g_jpTextKeyServerNameLabel[] = "\xc1\xa2\xbc\xd3\xbc\xad\xb9\xf6:";  // "접속서버:"
// 0xd13838
const char g_jpTextKeyServerName2Label[] = "\xc1\xa2\xbc\xd3\xc3\xa4\xb3\xce:";  // "접속채널:"
// 0xe781a4
RFLoadingProgressDlg* g_loadingWaitingDialog = NULL;
// 0xe73e10
char* g_jpObjectE73e10 = NULL;
// 0xe781d0
RFServerNoticeDlg* g_serverNoticeDlg = NULL;
// 0xe781c0
RFWnd* g_lobbyPopupMenuWnd = NULL;
// 0xd05118
const double g_lobbyPangOffsetX = 8.0;
// 0xcfe8d4
const float g_specialCardEmptyTime = 3.0f;
// 0xd05d90
const double g_lobbyBannerStripTop = 20.0;
// 0xd04f80
const double g_lobbyBannerStripExtra = 40.0;
// 0xd0913c
const float g_lobbyBannerFadeSpeed = 30.0f;
// 0xd0ba30
const char g_jpTextKeyPremiumRemain[] = "%d\xc0\xcf %d\xbd\xc3\xb0\xa3";  // "%d일 %d시간"
// 0xd005b0
const char g_jpTextKeyPremiumNone[] =
	"\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3: 1\xbd\xc3\xb0\xa3\xb9\xcc\xb8\xb8\n";  // "남은시간: 1시간미만\n"
// 0xd13a74
const char g_jpTextKeySpecialCardEmpty[] =
	"\xbb\xe7\xbf\xeb\xc1\xdf\xc0\xce \xc8\xbf\xb0\xfa\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";  // "사용중인 효과가 없습니다."
// 0xe13230 (0xd064c4; the next dword, 0xd61c1c, is another datum)
char* g_lobbyNumberFontFiles[] = { const_cast<char*>("[font_wind.jpg") };

// 0xe78c38
RFLobbyDesktop* RFLobbyDesktop::s_instance = NULL;
// 0xe78c3c
int RFLobbyDesktop::s_mode = 0;
// 0xe78c48
std::map<std::string, int> RFLobbyDesktop::s_layoutStates;

// 0x5ffc60
RFLobbyDesktop::RFLobbyDesktop()
	: m_taskMain(NULL), m_lobbyMain(NULL),
	  m_pangFont(NULL), m_cookieFont(NULL), m_thirdFont(NULL) {
	m_lobbyState1f8 = 1;
	m_specialCardEmptyTime = 0.0f;
	m_lobbyState = 1;
	m_lobbyCovered = false;
	m_specialCardBlinkTime = 0.0f;
	m_messengerBlinkTime = 0.0f;
	m_busy = false;  // 0x240-0x243: one dword store
	m_popupMenuOpen = false;
	m_inputFrozen = false;
	m_shopCameraZoom = false;
	m_unknown244 = false;  // 0x244-0x247: one dword store
	m_unknown245 = false;
	m_messageBubbleShown = false;
	m_premium = false;
	m_bannerAlpha = g_lobbyDesktopFloatCfec5c;
	m_specialCardImageSet = 0;
	m_messengerBlink = false;
	m_bannerFadeSpeed = 0.0f;
	m_avatarPending = false;  // 0x258-0x259: one word store
	m_caddiePending = false;
	m_postPending = false;
	for (int i = 0; i < 4; i++) {  // 0x25c-0x26c: one memset of 0x11 bytes
		m_bannerBitmaps[i] = NULL;
	}
	m_bannerShown = false;
	s_instance = this;
	InitAfterConstruct();
}

// 0x5f7b70 (the scalar deleting destructor 0x5f7ee0 calls it)
RFLobbyDesktop::~RFLobbyDesktop() {
	m_taskMain = NULL;
	m_lobbyMain = NULL;
	if (g_resrcmng != NULL) {
		if (m_pangFont != NULL) {
			g_resrcmng->Release(m_pangFont);
			m_pangFont = NULL;
		}
		if (g_resrcmng != NULL) {
			if (m_cookieFont != NULL) {
				g_resrcmng->Release(m_cookieFont);
				m_cookieFont = NULL;
			}
			if (g_resrcmng != NULL) {
				if (m_thirdFont != NULL) {
					g_resrcmng->Release(m_thirdFont);
					m_thirdFont = NULL;
				}
			}
		}
	}
	s_instance = NULL;
}

// 0x5fd000
void RFLobbyDesktop::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventFinalize, Subscriber(&RFLobbyDesktop::OnFinalize, this));
	RegisterTo(L"", RFWnd::EventUpdate, Subscriber(&RFLobbyDesktop::OnUpdateDesktop, this));
	RegisterTo(L"", RFWnd::EventLButtonDown, Subscriber(&RFLobbyDesktop::OnLButtonDownDesktop, this));
	RegisterTo(L"wnd_special_card_empty", RFWnd::EventUpdate, Subscriber(&RFLobbyDesktop::OnUpdateSpecialCardEmpty, this));
	RegisterTo(L"", RFWnd::EventKeyDown, Subscriber(&RFLobbyDesktop::OnKeyDownDesktop, this));
	RegisterTo(L"panel_popup", RFWnd::EventKeyDown, Subscriber(&RFLobbyDesktop::OnKeyDownPopup, this));
	RegisterTo(L"panel_popup", RFWnd::EventKillFocus, Subscriber(&RFLobbyDesktop::OnKillFocusPopup, this));
	RegisterTo(L"btn_pang_cookie", RFWnd::EventMouseEnter, Subscriber(&RFLobbyDesktop::OnPangCookieEnter, this));
	RegisterTo(L"btn_pang_cookie", RFWnd::EventMouseLeave, Subscriber(&RFLobbyDesktop::OnPangCookieLeave, this));
	RegisterTo(L"pic_server_name", RFWnd::EventMouseEnter, Subscriber(&RFLobbyDesktop::OnServerNameEnter, this));
	RegisterTo(L"pic_server_name", RFWnd::EventMouseLeave, Subscriber(&RFLobbyDesktop::OnServerNameLeave, this));
	RegisterTo(L"btn_spcard", RFWnd::EventMouseEnter, Subscriber(&RFLobbyDesktop::OnSpecialCardEnter, this));
	RegisterTo(L"btn_spcard", RFWnd::EventMouseLeave, Subscriber(&RFLobbyDesktop::OnSpecialCardLeave, this));
	RegisterTo(L"btn_messenger", RFWnd::EventMouseEnter, Subscriber(&RFLobbyDesktop::OnMessengerEnter, this));
	RegisterTo(L"btn_messenger", RFWnd::EventMouseLeave, Subscriber(&RFLobbyDesktop::OnMessengerLeave, this));
	RegisterTo(L"btn_myinfo", RFWnd::EventMouseEnter, Subscriber(&RFLobbyDesktop::OnMyInfoEnter, this));
	RegisterTo(L"btn_myinfo", RFWnd::EventMouseLeave, Subscriber(&RFLobbyDesktop::OnMyInfoLeave, this));
	RegisterTo(L"btn_option", RFWnd::EventMouseEnter, Subscriber(&RFLobbyDesktop::OnOptionEnter, this));
	RegisterTo(L"btn_option", RFWnd::EventMouseLeave, Subscriber(&RFLobbyDesktop::OnOptionLeave, this));
	RegisterTo(L"btn_ad", RFWnd::EventMouseEnter, Subscriber(&RFLobbyDesktop::OnAdEnter, this));
	RegisterTo(L"btn_ad", RFWnd::EventMouseLeave, Subscriber(&RFLobbyDesktop::OnAdLeave, this));
	RegisterTo(L"btn_premium", RFWnd::EventMouseEnter, Subscriber(&RFLobbyDesktop::OnPremiumEnter, this));
	RegisterTo(L"btn_premium", RFWnd::EventMouseLeave, Subscriber(&RFLobbyDesktop::OnPremiumLeave, this));
	RegisterTo(L"btn_game", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnGameClicked, this));
	RegisterTo(L"btn_talk", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnGameClicked, this));
	RegisterTo(L"btn_practice", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnPracticeClicked, this));
	RegisterTo(L"btn_shop", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnShopClicked, this));
	RegisterTo(L"btn_myroom", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnMyRoomClicked, this));
	RegisterTo(L"btn_cookie", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnCookieClicked, this));
	RegisterTo(L"btn_post", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnPostClicked, this));
	RegisterTo(L"btn_channel", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnChannelClicked, this));
	RegisterTo(L"btn_popup", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnPopupClicked, this));
	RegisterTo(L"btn_exit", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnExitClicked, this));
	RegisterTo(L"btn_spcard", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnSpecialCardClicked, this));
	RegisterTo(L"btn_messenger", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnMessengerClicked, this));
	RegisterTo(L"btn_myinfo", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnMyInfoClicked, this));
	RegisterTo(L"btn_option", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnOptionClicked, this));
	RegisterTo(L"btn_ad", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnAdClicked, this));
	RegisterTo(L"btn_pang_cookie", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnCookieClicked, this));
	RegisterTo(L"btn_premium", RFButton::EventClicked, Subscriber(&RFLobbyDesktop::OnPremiumClicked, this));
	RegisterTo(L"", RFWnd::EventDraw, Subscriber(&RFLobbyDesktop::OnDrawDesktop, this));
	RegisterTo(L"wnd_pang", RFWnd::EventDraw, Subscriber(&RFLobbyDesktop::OnDrawPang, this));
	RegisterTo(L"wnd_cookie", RFWnd::EventDraw, Subscriber(&RFLobbyDesktop::OnDrawCookie, this));
	RegisterTo(L"wnd_won", RFWnd::EventDraw, Subscriber(&RFLobbyDesktop::OnDrawWon, this));
	RegisterTo(L"label_ad", RFWnd::EventDraw, Subscriber(&RFLobbyDesktop::OnDrawAd, this));

	m_taskMain = static_cast<CTaskMain*>(AfxGetTask()->GetMainActor());
	m_lobbyMain = static_cast<CLobbyMain*>(AfxGetTask()->GetActor("Lobby"));
	m_avatarChatMain = static_cast<ntAvatarChatMain*>(AfxGetTask()->GetActor("AvatarChat"));
	std::string error;
	bool loaded;
	if (s_mode == 0) {
		if (m_taskMain != NULL) {
			loaded = true;
		}
		if (m_taskMain == NULL || m_lobbyMain == NULL) {
			loaded = false;
		}
		error.assign("<CTaskMain> or <CLobbyMain> load error.", 0x27);
		if (!loaded) {
			g_bQuit = true;
		}
	} else if (s_mode == 1) {
		if (m_taskMain != NULL) {
			loaded = true;
		}
		if (m_taskMain == NULL || m_avatarChatMain == NULL) {
			loaded = false;
		}
		error.assign("<CTaskMain> or <ntAvatarChatMain> load error.", 0x2d);
		if (!loaded) {
			g_bQuit = true;
		}
	}

	// From 0x5fef00:
	InitialUpdate_5f7210();
	ShowChild(L"panel_popup", false);
	ShowChild(L"wnd_special_card_empty", false);
	ShowChild(L"pic_pang_bar_treecash", false);
	ShowChild(L"pic_pang_bar_frame_tree", false);
	ShowChild(L"btn_premium", false);
	// The dynamic_cast's type descriptor is 0xe4628c; RFTextArea from the
	// window's name and its slot [34] SetText(std::wstring const&).
	RFTextArea* bubble = GetChildAs<RFTextArea>(L"textarea_bubble");
	if (bubble != NULL) {
		bubble->SetText(ShiftJisToWString(std::string(Jp_LookupText(g_jpTextKeySpecialCardEmpty))));
		RFWnd* specialCardEmpty = GetChild(std::wstring(L"wnd_special_card_empty"));
		if (specialCardEmpty != NULL) {
			bubble->CenterWindow(specialCardEmpty);
		}
	}
	// The three number fonts (JP 0xb55c10 = KR WResourceManager::GetTitleFont,
	// 0xb5fe00 = KR WTitleFont::Create).
	tagWTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = g_lobbyNumberFontFiles;
	font.numPages = 1;
	font.fontw = 0x10;
	font.fonth = 0x10;
	font.texw = 0x80;
	font.texh = 0x40;
	font.pCharSet = const_cast<char*>("1234567890ym%-.?/:,");
	WTitleFont* titleFont = g_resrcmng->GetTitleFont();
	m_pangFont = titleFont;
	titleFont->Create(&font);
	titleFont = g_resrcmng->GetTitleFont();
	m_cookieFont = titleFont;
	titleFont->Create(&font);
	titleFont = g_resrcmng->GetTitleFont();
	m_thirdFont = titleFont;
	titleFont->Create(&font);
	// The sub-windows, each created once (its global).
	if (g_lobbyCharacterSelectWnd == NULL) {
		RFWnd* area = GetChild(std::wstring(L"wnd_char_select_area"));
		if (area != NULL) {
			RFWnd* wnd = CreateLobbyCharacterSelectWnd(area);
			if (wnd != NULL) {
				static_cast<RFFormBase<RFWndEx>*>(wnd)->ShowForm(true);
			}
		}
	}
	if (g_lobbyCharacterCarousel == NULL) {
		RFWnd* area = GetChild(std::wstring(L"wnd_char_select_anim_area"));
		if (area != NULL) {
			RFWnd* wnd = CreateLobbyCharacterCarousel(area);
			if (wnd != NULL) {
				static_cast<RFFormBase<RFWndEx>*>(wnd)->ShowForm(true);
			}
			area->SetHitTestVisible(false);
		}
	}
	// The pop-up menu: panel_popup takes the menu's size and sits at the
	// screen's right edge, just above wnd_underbar.
	if (g_lobbyPopupMenuWnd == NULL) {
		RFWnd* popup = GetChild(std::wstring(L"panel_popup"));
		if (popup != NULL) {
			float underbarTop = 0.0f;
			RFWnd* underbar = GetChild(std::wstring(L"wnd_underbar"));
			if (underbar != NULL) {
				underbarTop = underbar->GetScreenPos().y;
			}
			RFWnd* menu = CreateLobbyPopupMenuWnd(popup);
			if (menu != NULL) {
				popup->SetSize(menu->GetSize());
				static_cast<RFFormBase<RFWndEx>*>(menu)->ShowForm(true);
				menu->SetPos(0.0f, 0.0f);
			}
			RFSize size = popup->GetSize();
			float screenWidth = g_view != NULL ? g_view->GetWidth() : 800.0f;  // JP 0xd0562c
			popup->SetScreenPos(screenWidth - size.width, underbarTop - size.height);
			popup->ShowWindow(false);
		}
	}
	if (g_messengerBubble == NULL) {
		RFWnd* box = GetChild(std::wstring(L"panel_notice_box"));
		if (box != NULL) {
			box->SetPreDraw(false);
			RFLobbyMessengerBubble* bubble = CreateMessengerBubble(box);
			if (bubble != NULL) {
				bubble->Hide();
			}
			box->ShowWindow(true);
			box->SetHitTestVisible(false);
		}
	}
	RefreshBarButtons();
	LoadBannerBitmaps();
	RefreshOnShow_5fae60(false);
	RFWnd* wnd = GetChild(std::wstring(L"panel_popup"));
	if (wnd != NULL) {
		wnd->SetPreDraw(false);
	}
	wnd = GetChild(std::wstring(L"pic_pang_bar_frame_tree"));
	if (wnd != NULL) {
		wnd->SetPreDraw(false);
	}
	if (s_mode == 0 && g_loadingWaitingDialog == NULL) {
		RFWnd* dialog = CreateLoadingWaitingDialog(this);
		if (dialog != NULL) {
			static_cast<RFLoadingProgressDlg*>(dialog)->Open();
		}
	}
	ShowChild(L"wnd_char_flip_anim", false);
}

// 0x5ff8f0: fills s_layoutStates once (its size, 0xe78c50, is the test).
void RFLobbyDesktop::InitAfterConstruct() {
	if (s_layoutStates.size() == 0) {
		s_layoutStates[std::string("")] = 0;
		s_layoutStates[std::string("lobbymain")] = 1;
		s_layoutStates[std::string("shop|char")] = 2;
		s_layoutStates[std::string("shop|caddie")] = 3;
		s_layoutStates[std::string("shop|item")] = 4;
		s_layoutStates[std::string("myroom|char")] = 5;
		s_layoutStates[std::string("myroom|caddie")] = 6;
		s_layoutStates[std::string("myroom|item")] = 7;
		s_layoutStates[std::string("tutorial")] = 8;
	}
}

// 0x5d5520 (placed among the character-select windows' code): the
// lobby's game messages.
void RFLobbyDesktop::HandleMsg(const MsgObject& msg) {
	IActor* actor;
	switch (msg.m_msg) {
	case 0x1:
		m_inputFrozen = true;
		break;
	case 0x2:
		m_inputFrozen = false;
		break;
	case 0x53:
		m_inputFrozen = true;
		RFSceneManager::SetFreshInputSuspended(true);
		m_unknown189 = false;
		break;
	case 0x54: {
		m_inputFrozen = false;
		RFSceneManager* scene = RFSceneManager::Instance();
		RFManager* manager = scene == NULL ? NULL : scene->GetManager();  // 0x477bc0
		if (!manager->IsModalExist()) {
			RFSceneManager::SetFreshInputSuspended(false);
		}
		m_unknown189 = true;
		break;
	}
	case 0x55:
		// param1: a layout name; opens its bar state.
		if (msg.m_param1 != 0) {
			std::string name((const char*)msg.m_param1);
			EventArgs args;
			switch (GetLayoutState(name)) {
			case 2:
			case 3:
			case 4:
				OnShopClicked(args);
				break;
			case 5:
			case 6:
			case 7:
				OnMyRoomClicked(args);
				break;
			case 8:
				HandleMsg(MsgObject(NULL, 0x88, (intptr_t)"CTutorialWidget", 0, 0, 0, 0));
				break;
			}
		}
		break;
	case 0x56:
		if (IsWindowVisible(false) && g_loadingWaitingDialog == NULL) {
			RFWnd* dialog = CreateLoadingWaitingDialog(this);
			if (dialog != NULL) {
				static_cast<RFLoadingProgressDlg*>(dialog)->Open();
			}
		}
		break;
	case 0x57:
		if (g_loadingWaitingDialog != NULL) {
			g_loadingWaitingDialog->Close(static_cast<RFWNDCLOSESTATE>(1));
		}
		actor = AfxGetTask()->GetActor("LobbyBg");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x182, 0, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
		}
		break;
	case 0x58:
		if (g_jpObjectE73e10 != NULL) {
			RFSceneManager* scene = RFSceneManager::Instance();
			RFManager* manager = scene == NULL ? NULL : scene->GetManager();  // 0x477bc0
			if (!manager->IsModalExist()) {
				g_jpObjectE73e10[0xb0] = 1;
			}
		}
		break;
	case 0x59:
		SetBarSelection((int)msg.m_param1);
		break;
	case 0x88: {
		// param1: the name of the window that opened; the desktop itself
		// returns to the lobby bar state, another one is kept in m_string224.
		std::string name((const char*)msg.m_param1);
		if (name == "RFLobbyDesktop") {
			if (m_string224.size() == 0) {
				SetBarSelection(1);
				UpdateLobbyCamera();
			}
		} else {
			m_string224 = name;
			FrForm* pageForm = Task_GetWidgetForm(AfxGetTask());
			if (pageForm != NULL) {
				Form_Close(pageForm, true);
			}
			if (name == "CTutorialWidget") {
				SetBarSelection(8);
				HandleMsg(MsgObject(NULL, 0x180, 0, 0, 0, 0, 0));
			}
		}
		break;
	}
	case 0xca:
	case 0xcb:
		NotifyTopBar(msg.m_msg);
		break;
	case 0xcc:
		if (msg.m_param2 != 0) {
			SetMessengerBubble(1, std::string((const char*)msg.m_param1));
			m_messageBubbleShown = true;
		} else if (m_messageBubbleShown) {
			m_messageBubbleShown = false;
		}
		break;
	case 0xcd:
	case 0xce:
	case 0xcf:
	case 0xd1:
		NotifyMessenger(msg.m_msg);
		break;
	case 0x106:
		NotifyCarousel();
		break;
	case 0x180:
		// The bar state's camera move finished.
		switch (m_lobbyState) {
		case 2:
		case 5:
			if (m_string224.size() != 0) {
				actor = AfxGetTask()->GetActor("LobbyAvatar");
				if (actor != NULL) {
					actor << MsgObject(NULL, 0x19a, 0, 0, 0, 0, 0);
				}
			}
			// fall through
		case 3:
		case 4:
		case 6:
		case 7:
			if (!m_busy || m_string224.size() == 0) {
				break;
			}
			// fall through
		case 8:
			if (WSingleton<CTaskManager>::Instance() != NULL) {
				TaskManager_9bf6e0(WSingleton<CTaskManager>::Instance(), m_string224.c_str(), "TOPPAGE");
			}
			m_string224.clear();
			break;
		}
		actor = AfxGetTask()->GetActor("LobbyAvatar");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x18e, 0, 0, 0, 0, 0);
		}
		actor = AfxGetTask()->GetActor("LobbyCaddie");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x18e, 0, 0, 0, 0, 0);
		}
		actor = AfxGetTask()->GetActor("LobbyMascot");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x18e, 0, 0, 0, 0, 0);
		}
		actor = AfxGetTask()->GetActor("Lobby");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x1a0, 0, 0, 0, 0, 0);
		}
		m_busy = false;
		break;
	}
}

// 0x5f6410
int RFLobbyDesktop::GetSelectedCharacterTypeId() {
	return g_lobbyCharacterSelectWnd ? g_lobbyCharacterSelectWnd->m_carouselCharacter : 15;
}

// 0x5f6450 (HandleMsg 0x106)
void RFLobbyDesktop::NotifyCarousel() {
	if (g_lobbyCharacterCarousel != NULL) {
		g_lobbyCharacterCarousel->ResetCharacters(0xf);
	}
}

// 0x5f8af0: the bar state of a layout name (0 when s_layoutStates lacks it).
int RFLobbyDesktop::GetLayoutState(std::string const& name) {
	std::map<std::string, int>::iterator it = s_layoutStates.find(name);
	if (it != s_layoutStates.end()) {
		return it->second;
	}
	return 0;
}

// 0x5f7080
void RFLobbyDesktop::OnDestroy() {
	IActor* actor = AfxGetTask()->GetActor("LobbyBg");
	if (actor != NULL) {
		actor << MsgObject(NULL, 0x183, 0, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
	}
	actor = AfxGetTask()->GetActor("LobbyAvatar");
	if (actor != NULL) {
		actor << MsgObject(NULL, 0x184, 0, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
	}
	actor = AfxGetTask()->GetActor("LobbyCaddie");
	if (actor != NULL) {
		actor << MsgObject(NULL, 0x184, 0, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
	}
	actor = AfxGetTask()->GetActor("LobbyMascot");
	if (actor != NULL) {
		actor << MsgObject(NULL, 0x184, 0, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
	}
	Jp_GameState_SetLobbyCameraEnabled(true);  // JP CSharedDoc 0xe47dbc +0x10b27
	Jp_LobbyViewReset(0);
	RFFormDesktopBase<RFWndEx>::OnDestroy();
}

// 0x5ff4f0
void RFLobbyDesktop::ShowLobby(bool show) {
	if (!show) {
		m_lobbyState1f8 = 1;
		m_lobbyState = 1;
	} else {
		RefreshOnShow();
		if (g_lobbyCharacterSelectWnd != NULL) {
			g_lobbyCharacterSelectWnd->OnLobbyShown();  // 0x5d3b90
		}
		if (s_mode == 0) {
			IActor* lobby = AfxGetTask()->GetActor("Lobby");
			if (lobby != NULL) {
				lobby << MsgObject(NULL, 0x77, 0, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
			}
		}
		if (RFSceneManager::Instance() != NULL) {
			RFSceneManager::Instance()->SetInputDisabled(false);
		}
	}
	ShowDesktop(show);
}


// 0x5f5ef0 (OnUpdateDesktop's last call): the banner fades out at
// m_bannerFadeSpeed per second, down to 0.
void RFLobbyDesktop::UpdateBannerFade(float dt) {
	if (m_bannerShown) {
		m_bannerAlpha = m_bannerAlpha - m_bannerFadeSpeed * dt;
		if (m_bannerAlpha < 0.0f) {
			m_bannerAlpha = 0.0f;
		}
	}
}

// 0x5f5f40
void RFLobbyDesktop::ResetBanner() {
	m_bannerShown = false;
	m_bannerAlpha = g_lobbyDesktopFloatCfec5c;
	m_bannerFadeSpeed = g_lobbyBannerFadeSpeed;
}

// 0x5f5f60 (btn_cookie, btn_pang_cookie)
void RFLobbyDesktop::OnCookieClicked(EventArgs&) {
	if (m_busy == 0) {
		m_taskMain->OnUnderBar_CookieUp();  // JP vtable +0x4c = KR CTaskMain::OnUnderBar_CookieUp
	}
}

// 0x5f5f80
void RFLobbyDesktop::OnPangCookieEnter(EventArgs&) {
}

// 0x5f5f90
void RFLobbyDesktop::OnPangCookieLeave(EventArgs&) {
}

// 0x5f5fa0 (btn_myinfo)
void RFLobbyDesktop::OnMyInfoClicked(EventArgs&) {
	m_taskMain->OnOverBar_MyInfoUp();  // JP 0x65ce40 = KR CTaskMain::OnOverBar_MyInfoUp
}

// 0x5f5fb0 (btn_option)
void RFLobbyDesktop::OnOptionClicked(EventArgs&) {
	m_taskMain->OnOverBar_OptionUp();  // JP 0x659d30 ("option") = KR CTaskMain::OnOverBar_OptionUp
}

// 0x5f5fc0 (btn_ad)
void RFLobbyDesktop::OnAdClicked(EventArgs&) {
	m_taskMain->OnOverBar_OneLineReqBtnUp();  // JP 0x65d610 = KR CTaskMain::OnOverBar_OneLineReqBtnUp
}

// 0x5f5fd0 (btn_premium)
void RFLobbyDesktop::OnPremiumClicked(EventArgs&) {
	if (s_mode != 0) {
		return;
	}
	if (m_lobbyMain != NULL) {
		LobbyMain_OpenPremiumNotice(m_lobbyMain);  // JP only
	}
}

// 0x5f6470 (OnInitialUpdate): the banner's four bitmaps from the Fresh
// (JP 0xbe9b90 = Fresh::GetBitmap), then ResetBanner.
void RFLobbyDesktop::LoadBannerBitmaps() {
	m_bannerBitmaps[0] = const_cast<Bitmap*>(g_pFresh->GetBitmap("Premium service_on_00"));
	m_bannerBitmaps[1] = const_cast<Bitmap*>(g_pFresh->GetBitmap("Premium service_on_01"));
	m_bannerBitmaps[2] = const_cast<Bitmap*>(g_pFresh->GetBitmap("Premium service_on_02"));
	m_bannerBitmaps[3] = const_cast<Bitmap*>(g_pFresh->GetBitmap("Premium service_base"));
	ResetBanner();
}

// 0x5f64e0: the banner's three pieces side by side, centred on the screen,
// over the background strip stretched across it, at m_bannerAlpha; reset
// once it has faded out.
void RFLobbyDesktop::DrawDesktop() {
	Bitmap* left = m_bannerBitmaps[0];
	Bitmap* middle = m_bannerBitmaps[1];
	Bitmap* right = m_bannerBitmaps[2];
	Bitmap* strip = m_bannerBitmaps[3];
	if (!m_bannerShown || left == NULL || middle == NULL || right == NULL || strip == NULL) {
		return;
	}
	FrGraphicInterface* gdi = Jp_GetFreshGDI();
	uint32_t color = ((uint32_t)(unsigned char)m_bannerAlpha << 24) | 0x00ffffff;
	float x = g_view->GetWidth() * 0.5f - ((float)left->Width() + (float)middle->Width() + (float)right->Width()) * 0.5f;
	float y = g_view->GetHeight() * 0.5f - (float)left->Height() * 0.5f;
	gdi->DrawTexture(strip, WRect(0.0f, (float)(y - g_lobbyBannerStripTop), g_view->GetWidth(),
		(float)((float)left->Height() + g_lobbyBannerStripExtra)), color, 0);  // JP 0xbec640
	gdi->DrawTexture(left, WRect(x, y, left->Width(), left->Height()), color, 0);
	gdi->DrawTexture(middle, WRect((float)left->Width() + x, y, middle->Width(), middle->Height()), color, 0);
	gdi->DrawTexture(right, WRect((float)left->Width() + x + (float)middle->Width(), y, right->Width(), right->Height()), color, 0);
	if (m_bannerAlpha <= 0.0f) {
		ResetBanner();
	}
}

// 0x5f67b0
void RFLobbyDesktop::OnFinalize(EventArgs&) {
	if (RFTooltipHelper::Instance() != NULL) {
		RFTooltipHelper::Instance()->Reset();
	}
}

// 0x5f67d0
void RFLobbyDesktop::OnKeyDownDesktop(EventArgs& args) {
	RFSceneManager* scene = RFSceneManager::Instance();
	if (scene != NULL && scene->GetManager() != NULL && scene->GetManager()->IsModalExist()) {
		return;
	}
	if (Jp_IsFreshDialogActive()) {
		return;
	}
	unsigned int key = static_cast<KeyEventArgs&>(args).keyCode;
	if (key == 0x1b) {
		RFLobbyCharacterSelectAnimWnd* carousel = g_lobbyCharacterCarousel;
		if (carousel != NULL && !carousel->IsAnimating() && carousel->IsOpen()) {
			carousel->SetOpen(RFLobbyCharacterSelectAnimWnd::OpenAction_Close, true);
		}
	} else if (key == 0xd) {
		RFLobbyCharacterSelectAnimWnd* carousel = g_lobbyCharacterCarousel;
		if (carousel != NULL && !carousel->IsAnimating() && carousel->IsOpen()) {
			carousel->SetOpen(RFLobbyCharacterSelectAnimWnd::OpenAction_Close, false);
		}
	}
}

// 0x5f6870
void RFLobbyDesktop::OnServerNameLeave(EventArgs&) {
	if (RFTooltipHelper::Instance() != NULL) {
		RFTooltipHelper::Instance()->HideTooltip();
	}
}

// 0x5f6890
void RFLobbyDesktop::OnSpecialCardLeave(EventArgs&) {
	if (RFTooltipHelper::Instance() != NULL) {
		RFTooltipHelper::Instance()->HideTooltip();
	}
}

// 0x5f68b0
void RFLobbyDesktop::OnMessengerLeave(EventArgs&) {
	if (RFTooltipHelper::Instance() != NULL) {
		RFTooltipHelper::Instance()->HideTooltip();
	}
}

// 0x5f68d0
void RFLobbyDesktop::OnMyInfoLeave(EventArgs&) {
	if (RFTooltipHelper::Instance() != NULL) {
		RFTooltipHelper::Instance()->HideTooltip();
	}
}

// 0x5f68f0
void RFLobbyDesktop::OnOptionLeave(EventArgs&) {
	if (RFTooltipHelper::Instance() != NULL) {
		RFTooltipHelper::Instance()->HideTooltip();
	}
}

// 0x5f6910
void RFLobbyDesktop::OnAdEnter(EventArgs&) {
}

// 0x5f6920
void RFLobbyDesktop::OnPremiumLeave(EventArgs&) {
	if (RFTooltipHelper::Instance() != NULL) {
		RFTooltipHelper::Instance()->HideTooltip();
	}
}

// 0x5f6940
void RFLobbyDesktop::OnAdLeave(EventArgs&) {
	if (RFTooltipHelper::Instance() != NULL) {
		RFTooltipHelper::Instance()->HideTooltip();
	}
}

// 0x5f6960
void RFLobbyDesktop::OnDrawDesktop(EventArgs&) {
	DrawDesktop();
}

// 0x5f6970 (wnd_won): CProjectG 0xe47db0 is the KR WSingleton<CProjectG>.
void RFLobbyDesktop::OnDrawWon(EventArgs&) {
	ProjectG_DrawWon(WSingleton<CProjectG>::Instance());  // JP only (0xa62c60)
}

// 0x5f6980 (label_ad)
void RFLobbyDesktop::OnDrawAd(EventArgs&) {
	if (COneLineBoard::Instance() != NULL) {  // 0xe78c58
		COneLineBoard::Instance()->Display();  // 0x897c40
	}
}

// 0x5f7210 (lobby mode only): message 0x185 to LobbyAvatar, 0x1a5 to
// LobbyCaddie, 0x1aa to LobbyMascot.
void RFLobbyDesktop::InitialUpdate_5f7210() {
	if (s_mode != 0) {
		return;
	}
	IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
	if (actor != NULL) {
		actor << MsgObject(NULL, 0x185, 0, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
	}
	actor = AfxGetTask()->GetActor("LobbyCaddie");
	if (actor != NULL) {
		actor << MsgObject(NULL, 0x1a5, 0, 0, 0, 0, 0);
	}
	actor = AfxGetTask()->GetActor("LobbyMascot");
	if (actor != NULL) {
		actor << MsgObject(NULL, 0x1aa, 0, 0, 0, 0, 0);
	}
}

// 0x5f7310 (lobby mode only): in the shop and my-room character and caddie
// states, the mouse wheel zooms the LobbyBg camera in (up) or out (down);
// the caddie camera only when the player has a caddie (JP CSharedDoc
// +0xa26, the KR m_myCaddieId by place: just before m_myCharacterId, JP
// +0xa2a).
void RFLobbyDesktop::UpdateLobby_5f7310() {
	if (m_busy) {
		return;
	}
	const char* camera;
	bool zoom;
	switch (m_lobbyState) {
	case 2:
	case 5: {
		if (!m_unknown244) {
			return;
		}
		float wheel = RFSceneManager::Instance()->GetMouseWheel();
		if (wheel == 0.0f) {
			return;
		}
		zoom = wheel > 0.0f;
		if (m_shopCameraZoom == zoom) {
			return;
		}
		camera = zoom ? "ShopCamera_z" : "ShopCamera";
		break;
	}
	case 3:
	case 6: {
		if (WSingleton<CSharedDoc>::Instance()->m_myCaddieId == 0) {
			return;
		}
		if (!m_unknown245) {
			return;
		}
		float wheel = RFSceneManager::Instance()->GetMouseWheel();
		if (wheel == 0.0f) {
			return;
		}
		zoom = wheel > 0.0f;
		if (m_shopCameraZoom == zoom) {
			return;
		}
		camera = zoom ? "CaddieCamera_z" : "CaddieCamera";
		break;
	}
	default:
		return;
	}
	IActor* actor = AfxGetTask()->GetActor("LobbyBg");
	if (actor != NULL) {
		actor << MsgObject(NULL, 0x17f, (intptr_t)camera, 0, 1, 0, 0);  // vtable +0x2c, IActor::HandleMsg
	}
	m_shopCameraZoom = zoom;
	m_busy = true;
}

// 0x5f74e0 (btn_game, btn_talk)
void RFLobbyDesktop::OnGameClicked(EventArgs&) {
	if (m_busy != 0) {
		return;
	}
	RFSceneManager::Instance()->SetInputDisabled(true);
	FrForm* pageForm = Task_GetWidgetForm(AfxGetTask());
	if (pageForm != NULL) {
		Form_Close(pageForm, true);
	}
	m_taskMain->OnUnderBar_GameUp();  // JP 0x6586c0 = KR CTaskMain::OnUnderBar_GameUp
}

// 0x5f7540 (btn_practice)
void RFLobbyDesktop::OnPracticeClicked(EventArgs&) {
	if (m_busy != 0) {
		return;
	}
	FrForm* pageForm = Task_GetWidgetForm(AfxGetTask());
	if (pageForm != NULL) {
		Form_Close(pageForm, true);
	}
	RFSceneManager::Instance()->SetInputDisabled(true);
	LobbyMain_OpenSingleModeStart(m_lobbyMain);  // JP only
}

// 0x5f75a0 (btn_channel)
void RFLobbyDesktop::OnChannelClicked(EventArgs&) {
	if (m_busy != 0) {
		return;
	}
	// The task's vtable +0x30, KR CTask::SendMsgToMainActor; the KR main
	// actor's message 0x112 (CTaskMain::OnUnderBar_ServerUp's, not confirmed).
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x112, 0, 0, 0, 0, 0));
}

// 0x5f7610 (wnd_pang). The font's vtable +0x18 (view, text) measures the
// width and +0x24 the height: KR WFont::GetTextWidth / GetFontHeight.
void RFLobbyDesktop::OnDrawPang(EventArgs& args) {
	if (WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		return;
	}
	RFWnd* wnd = static_cast<WindowEventArgs_<RFWnd>&>(args).sender;
	if (wnd != NULL) {
		RFRect rect = wnd->GetScreenRect();
		float width = m_pangFont->GetTextWidth(g_view, m_pangText.c_str());
		int height = m_pangFont->GetFontHeight();
		float x = rect.left + (rect.right - rect.left) - width - g_lobbyPangOffsetX;
		float y = rect.top + (rect.bottom - rect.top) - (float)height - 1.0;
		m_pangFont->Print(g_view, x, y, m_pangText.c_str(), 0, 0xfff0bc10, NULL);
	}
}

// 0x5f7710 (wnd_cookie): the same with m_cookieText and m_cookieFont.
void RFLobbyDesktop::OnDrawCookie(EventArgs& args) {
	if (WSingleton<CProjectG>::Instance()->HidePrivacy()) {
		return;
	}
	RFWnd* wnd = static_cast<WindowEventArgs_<RFWnd>&>(args).sender;
	if (wnd != NULL) {
		RFRect rect = wnd->GetScreenRect();
		float width = m_cookieFont->GetTextWidth(g_view, m_cookieText.c_str());
		int height = m_cookieFont->GetFontHeight();
		float x = rect.left + (rect.right - rect.left) - width - 3.0;
		float y = rect.top + (rect.bottom - rect.top) - (float)height - 1.0;
		m_cookieFont->Print(g_view, x, y, m_cookieText.c_str(), 0, 0xfff0bc10, NULL);
	}
}

// 0x5f8020: in lobby mode, when the bar state changed, moves the LobbyBg
// camera to the state's place (message 0x17f with the camera's name, an
// empty string and 1) after the mascot's message, then sets m_busy.
void RFLobbyDesktop::UpdateLobbyCamera() {
	if (s_mode != 0) {
		return;
	}
	if (m_lobbyState1f8 == m_lobbyState) {
		return;
	}
	std::string empty("");
	IActor* actor;
	switch (m_lobbyState) {
	case 1:
		actor = AfxGetTask()->GetActor("LobbyMascot");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x1aa, 0, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
		}
		actor = AfxGetTask()->GetActor("LobbyBg");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x17f, (intptr_t)"DefaultCamera", (intptr_t)empty.c_str(), 1, 0, 0);
		}
		break;
	case 2:
	case 5: {
		actor = AfxGetTask()->GetActor("LobbyMascot");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x1ab, 0, 0, 0, 0, 0);
		}
		const char* camera = m_shopCameraZoom ? "ShopCamera_z" : "ShopCamera";
		actor = AfxGetTask()->GetActor("LobbyBg");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x17f, (intptr_t)camera, (intptr_t)empty.c_str(), 1, 0, 0);
		}
		break;
	}
	case 3:
	case 6:
		actor = AfxGetTask()->GetActor("LobbyBg");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x17f, (intptr_t)"CaddieCamera", (intptr_t)empty.c_str(), 1, 0, 0);
		}
		break;
	case 4:
	case 7:
		actor = AfxGetTask()->GetActor("LobbyBg");
		if (actor != NULL) {
			actor << MsgObject(NULL, 0x17f, (intptr_t)"ItemCamera", (intptr_t)empty.c_str(), 1, 0, 0);
		}
		break;
	default:
		return;
	}
	if (!m_busy) {
		m_busy = true;
	}
}

// 0x5f8380: the top bar's numbers through MakePriceStrA (JP 0x98b500 = KR
// MakePriceStrA): the pang, the cookies, and the cookies / 100.
void RFLobbyDesktop::RefreshOnShow_5f8380() {
	char buffer[0x40];
	MakePriceStrA(buffer, Jp_GameState_Pang());
	m_pangText.assign(buffer, strlen(buffer));
	MakePriceStrA(buffer, Jp_GameState_Cookies());
	m_cookieText.assign(buffer, strlen(buffer));
	char hundredths[0x40];
	MakePriceStrA(hundredths, Jp_GameState_Cookies() / 100);
	m_string1dc.assign(hundredths, strlen(hundredths));
}

// 0x5f8470 (btn_exit)
void RFLobbyDesktop::OnExitClicked(EventArgs&) {
	if (m_busy) {
		return;
	}
	if (m_avatarChatMain != NULL) {
		if (Jp_IsFreshDialogActive()) {
			return;
		}
		RFSceneManager* scene = RFSceneManager::Instance();
		if (scene != NULL && scene->GetManager() != NULL && scene->GetManager()->IsModalExist()) {
			return;
		}
	}
	if (s_mode == 0) {
		m_taskMain->OnUnderBar_ExitUp();  // JP 0x658a20 = KR CTaskMain::OnUnderBar_ExitUp
	} else if (s_mode == 1) {
		if (!Jp_IsExitMessageBoxMode()) {
			// 0xe741d0 = WSingleton<CQuitWindow>::m_pInstance (its ctor 0x6eefa0
			// stores it; the KR lounge task creates it, Avatar_Task.cpp:113).
			CQuitWindow* quitWindow = CQuitWindow::Instance();
			if (quitWindow != NULL) {
				quitWindow->Process();  // JP 0x6f0250 = KR CQuitWindow::Process
			}
		} else {
			Jp_ShowExitMessageBox(this);  // JP only
		}
	}
}

// 0x5f8580 (btn_spcard EventMouseEnter)
void RFLobbyDesktop::OnSpecialCardEnter(EventArgs& args) {
	if (RFTooltipHelper::Instance() != NULL) {
		std::string text(Jp_LookupText(g_jpTextKeySpecialCardTooltip));
		RFTooltipHelper::Instance()->ShowTooltip(static_cast<WindowEventArgs_<RFWnd>&>(args).sender, text, 1, 0xb);
	}
}

// 0x5f8640 (btn_messenger EventMouseEnter)
void RFLobbyDesktop::OnMessengerEnter(EventArgs& args) {
	if (RFTooltipHelper::Instance() != NULL) {
		std::string text(Jp_LookupText(g_jpTextKeyMessengerTooltip));
		RFTooltipHelper::Instance()->ShowTooltip(static_cast<WindowEventArgs_<RFWnd>&>(args).sender, text, 1, 0xb);
	}
}

// 0x5f8700 (btn_myinfo EventMouseEnter)
void RFLobbyDesktop::OnMyInfoEnter(EventArgs& args) {
	if (RFTooltipHelper::Instance() != NULL) {
		std::string text(Jp_LookupText(g_jpTextKeyMyInfoTooltip));
		RFTooltipHelper::Instance()->ShowTooltip(static_cast<WindowEventArgs_<RFWnd>&>(args).sender, text, 1, 0xb);
	}
}

// 0x5f87c0 (btn_option EventMouseEnter)
void RFLobbyDesktop::OnOptionEnter(EventArgs& args) {
	if (RFTooltipHelper::Instance() != NULL) {
		std::string text(Jp_LookupText(g_jpTextKeyOptionTooltip));
		RFTooltipHelper::Instance()->ShowTooltip(static_cast<WindowEventArgs_<RFWnd>&>(args).sender, text, 1, 0xb);
	}
}

// 0x5f8880 (btn_premium EventMouseEnter): the text is m_premiumText.
void RFLobbyDesktop::OnPremiumEnter(EventArgs& args) {
	if (RFTooltipHelper::Instance() != NULL) {
		std::string text(m_premiumText.c_str());
		RFTooltipHelper::Instance()->ShowTooltip(static_cast<WindowEventArgs_<RFWnd>&>(args).sender, text, 1, 0xb);
	}
}

// 0x5f8d20
void RFLobbyDesktop::OnLayoutChanged(uint32_t flags) {
	RFWnd::OnLayoutChanged(flags);
	RFWnd* area = GetChild(std::wstring(L"wnd_char_select_area"));
	if (area != NULL) {
		area->CenterWindow_Horizontal(NULL);
	}
	RFWnd* animArea = GetChild(std::wstring(L"wnd_char_select_anim_area"));
	if (animArea != NULL) {
		animArea->CenterWindow_Horizontal(NULL);
	}
}

// 0x5f8e50
void RFLobbyDesktop::FocusPopupPanel() {
	RFWnd* popup = GetChild(std::wstring(L"panel_popup"));
	if (popup != NULL) {
		RFSceneManager* scene = RFSceneManager::Instance();
		if (scene != NULL) {
			RFManager* manager = scene->GetManager();
			if (manager != NULL) {
				manager->FocusWindow(popup);
			}
		}
	}
}

// 0x5f8f20 (through the thunk j_sub_5f8f20): the ad ticker (COneLineBoard) takes
// label_ad's screen rectangle (its y moved down by g_lobbyAdTickerOffsetY)
// and the label is emptied.
void RFLobbyDesktop::RefreshOnShow_5f8f20() {
	RFLabel* label = GetChildAs<RFLabel>(L"label_ad");
	if (label == NULL) {
		return;
	}
	if (COneLineBoard::Instance() != NULL) {
		RFRect rect = label->GetScreenRect();
		COneLineBoard::Instance()->SetRect(WRect(rect.left,
			(float)(rect.top + g_lobbyAdTickerOffsetY),
			rect.right - rect.left, rect.bottom - rect.top));
	}
	label->SetLabel(std::wstring(L""));
}

// 0x5f9090
bool RFLobbyDesktop::IsPopupMenuOpen() {
	RFWnd* popup = GetChild(std::wstring(L"panel_popup"));
	if (popup == NULL) {
		return false;
	}
	return popup->IsWindowVisible(false);
}

// 0x5f9150: the Refresh input frozen with m_inputFrozen, the desktop
// disabled while m_busy (closing the carousel when it was open), then
// m_lobbyCovered: whether the pop-up menu, the carousel or a modal window
// covers the lobby (never while a Fresh layout is up); the task's TaskMain_ProcessLobbyKeys (0x65c580)
// runs only when nothing does.
void RFLobbyDesktop::Update_5f9150() {
	RFSceneManager* scene = RFSceneManager::Instance();
	RFManager* manager = scene == NULL ? NULL : scene->GetManager();
	if (manager != NULL) {
		manager->FreezeInputs(m_inputFrozen);
	}
	bool busy = m_busy;
	if (IsWindowEnabled(false) == busy) {
		EnableWindow(!busy);
		if (g_lobbyCharacterCarousel != NULL && g_lobbyCharacterCarousel->IsOpen()) {
			g_lobbyCharacterCarousel->SetOpen(RFLobbyCharacterSelectAnimWnd::OpenAction_Close, false);
		}
	}
	m_lobbyCovered = false;
	if (Jp_HasFreshLayoutWindow() && Jp_GetFreshModalFormCount() != 0) {
		m_lobbyCovered = false;
	} else {
		RFWnd* popup = GetChild(std::wstring(L"panel_popup"));
		if (popup != NULL && popup->IsWindowVisible(false)) {
			m_lobbyCovered = true;
		} else {
			if (g_lobbyCharacterCarousel != NULL) {
				m_lobbyCovered = g_lobbyCharacterCarousel->IsOpen();
			}
			if (m_lobbyCovered) {
				return;
			}
			scene = RFSceneManager::Instance();
			manager = scene == NULL ? NULL : scene->GetManager();
			if (manager != NULL) {
				m_lobbyCovered = manager->IsModalExist();
			}
		}
	}
	if (!m_lobbyCovered) {
		TaskMain_ProcessLobbyKeys(m_taskMain);
	}
}

// 0x5f92f0: btn_spcard's images by id: 0xca with the special-card alarm
// on (0xe74210 = WSingleton<CCardManager>, +0x1380 its m_spCardAlarm) → the "on" images (m_specialCardImageSet = 2); 0xcb →
// the premium images (m_specialCardImageSet = 1); otherwise the normal ones
// (m_specialCardImageSet = 0).
void RFLobbyDesktop::NotifyTopBar(int id) {
	switch (id) {
	case 0xca:
		if (Jp_IsSpecialCardActive()) {
			RFButton* button = dynamic_cast<RFButton*>(GetChild(std::wstring(L"btn_spcard")));
			if (button != NULL) {
				button->ChangeNormalButtonImage(std::wstring(L"spcard_btn_on_1.tga"));
				button->ChangeOverButtonImage(std::wstring(L"spcard_btn_on_2.tga"));
				button->ChangeDownButtonImage(std::wstring(L"spcard_btn_on_p.tga"));
			}
			m_specialCardImageSet = 2;
			return;
		}
		break;
	case 0xcb: {
		RFButton* button = dynamic_cast<RFButton*>(GetChild(std::wstring(L"btn_spcard")));
		if (button != NULL) {
			button->ChangeNormalButtonImage(std::wstring(L"premium_btn_on_1.tga"));
			button->ChangeOverButtonImage(std::wstring(L"premium_btn_on_2.tga"));
			button->ChangeDownButtonImage(std::wstring(L"premium_btn_on_p.tga"));
		}
		m_specialCardImageSet = 1;
		return;
	}
	}
	RFButton* button = dynamic_cast<RFButton*>(GetChild(std::wstring(L"btn_spcard")));
	if (button != NULL) {
		button->ChangeNormalButtonImage(std::wstring(L"spcard_btn_n.tga"));
		button->ChangeOverButtonImage(std::wstring(L"spcard_btn_o.tga"));
		button->ChangeDownButtonImage(std::wstring(L"spcard_btn_p.tga"));
	}
	m_specialCardImageSet = 0;
}

// 0x5f9610
void RFLobbyDesktop::UpdateSpecialCardBlink(float dt) {
	if (!m_specialCardImageSet) {
		return;
	}
	if (GetChildAs<RFButton>(std::wstring(L"btn_spcard")) == NULL) {
		return;
	}
	if (m_specialCardBlinkTime < 1.0f) {
		m_specialCardBlinkTime = m_specialCardBlinkTime + dt;
		return;
	}
	RFButton* button = GetChildAs<RFButton>(L"btn_spcard");
	if (button != NULL) {
		button->SetState(button->GetState() != RFButton::RFBUTTON_STATE_OVER ? RFButton::RFBUTTON_STATE_OVER
			: RFButton::RFBUTTON_STATE_NORMAL);
	}
	m_specialCardBlinkTime = 0.0f;
}

// 0x5f9780
void RFLobbyDesktop::UpdateMessengerBlink(float dt) {
	if (!m_messengerBlink) {
		return;
	}
	if (GetChildAs<RFButton>(std::wstring(L"btn_messenger")) == NULL) {
		return;
	}
	if (m_messengerBlinkTime < 1.0f) {
		m_messengerBlinkTime = m_messengerBlinkTime + dt;
		return;
	}
	RFButton* button = GetChildAs<RFButton>(L"btn_messenger");
	if (button != NULL) {
		button->SetState(button->GetState() != RFButton::RFBUTTON_STATE_OVER ? RFButton::RFBUTTON_STATE_OVER
			: RFButton::RFBUTTON_STATE_NORMAL);
	}
	m_messengerBlinkTime = 0.0f;
}

// 0x5f98f0
void RFLobbyDesktop::OnLButtonDownDesktop(EventArgs& args) {
	RFLobbyCharacterSelectAnimWnd* carousel = g_lobbyCharacterCarousel;
	if (carousel == NULL || !carousel->IsOpen()) {
		return;
	}
	RFWnd* area = GetChild(std::wstring(L"wnd_char_select_anim_area"));
	if (area != NULL) {
		RFRect rect = area->GetScreenRect();
		MouseEventArgs& mouse = static_cast<MouseEventArgs&>(args);
		if (!rect.PtInRect(mouse.pos.x, mouse.pos.y)) {
			g_lobbyCharacterCarousel->SetOpen(RFLobbyCharacterSelectAnimWnd::OpenAction_Close, true);
		}
	}
}

// 0x5f9a00 (wnd_special_card_empty EventUpdate): hides the window after
// g_specialCardEmptyTime.
void RFLobbyDesktop::OnUpdateSpecialCardEmpty(EventArgs& args) {
	RFWnd* wnd = GetChild(std::wstring(L"wnd_special_card_empty"));
	if (wnd != NULL && wnd->IsWindowVisible(false)) {
		m_specialCardEmptyTime = static_cast<UpdateEventArgs&>(args).elapsed + m_specialCardEmptyTime;
		if (m_specialCardEmptyTime > g_specialCardEmptyTime) {
			m_specialCardEmptyTime = 0.0f;
			wnd->ShowWindow(false);
		}
	}
}

// 0x5f9b00 (btn_post)
void RFLobbyDesktop::OnPostClicked(EventArgs&) {
	if (m_busy == 0) {
		if (m_postPending) {
			// 0x5f9b47: back to the plain post images.
			RFButton* button = dynamic_cast<RFButton*>(GetChild(std::wstring(L"btn_post")));
			if (button != NULL) {
				button->ChangeNormalButtonImage(std::wstring(L"post_n.tga"));
				button->ChangeOverButtonImage(std::wstring(L"post_o.tga"));
				button->ChangeDownButtonImage(std::wstring(L"post_d.tga"));
				m_postPending = false;
			}
		}
		m_taskMain->OnUnderBar_GiftUp();  // JP 0x658420 ("postbox") = KR CTaskMain::OnUnderBar_GiftUp
	}
}

// 0x5f9c40 (btn_spcard)
void RFLobbyDesktop::OnSpecialCardClicked(EventArgs& args) {
	RFWnd* button = static_cast<WindowEventArgs_<RFWnd>&>(args).sender;
	if (button != NULL) {
		RFRect rect = button->GetScreenRect();
		WRect buttonRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
		TaskMain_OnOverBar_SpecialCardUp(m_taskMain, buttonRect);  // JP 0x656be0 ("spcardbuff"), not the KR one
	}
	NotifyTopBar(0xca);
}

// 0x5fa270
void RFLobbyDesktop::ShowChild(const wchar_t* name, bool show) {
	RFWnd* wnd = GetChild(std::wstring(name));
	if (wnd != NULL) {
		wnd->ShowWindow(show);
	}
}

// 0x5fa340
void RFLobbyDesktop::EnableChild(const wchar_t* name, bool enable) {
	RFWnd* wnd = GetChild(std::wstring(name));
	if (wnd != NULL) {
		wnd->EnableWindow(enable);
	}
}

// 0x5fa410
void RFLobbyDesktop::FocusChild(const wchar_t* name) {
	RFWnd* wnd = GetChild(std::wstring(name));
	if (wnd != NULL) {
		RFSceneManager* scene = RFSceneManager::Instance();
		RFManager* manager = scene == NULL ? NULL : scene->GetManager();
		manager->FocusWindow(wnd);
	}
}

// 0x5fa7c0 (OnInitialUpdate): btn_talk / btn_game by the channel's type;
// in avatar-chat mode only my room, cookies and post stay enabled, in lobby
// mode the "Lobby" actor gets message 0x77 (1); then the character select
// window's lobby call and the tooltips' reset.
void RFLobbyDesktop::RefreshBarButtons() {
	bool talk = Jp_GetChannelType() == 0x10;
	ShowChild(L"btn_talk", talk);
	ShowChild(L"btn_game", !talk);
	switch (s_mode) {
	case 0: {
		IActor* lobby = AfxGetTask()->GetActor("Lobby");
		if (lobby != NULL) {
			lobby << MsgObject(NULL, 0x77, 1, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
		}
		break;
	}
	case 1:
		EnableChild(L"btn_game", false);
		EnableChild(L"btn_talk", false);
		EnableChild(L"btn_practice", false);
		EnableChild(L"btn_shop", false);
		EnableChild(L"btn_myroom", true);
		EnableChild(L"btn_cookie", true);
		EnableChild(L"btn_post", true);
		EnableChild(L"btn_channel", false);
		break;
	}
	if (g_lobbyCharacterSelectWnd != NULL) {
		g_lobbyCharacterSelectWnd->OnLobbyShown();  // 0x5d3b90
	}
	if (RFTooltipHelper::Instance() != NULL) {
		RFTooltipHelper::Instance()->Reset();
	}
}

// 0x5fa980: label_channel_name shows "<server> > <channel>" (the server
// alone outside a channel, "OFFLINE PLAY" offline); when the pair changed,
// the server notice dialog says so (lobby mode, no other dialog up) and
// btn_talk / btn_game follow the channel's type.
void RFLobbyDesktop::RefreshOnShow_5fa980() {
	RFLabel* label = dynamic_cast<RFLabel*>(GetChild(std::wstring(L"label_channel_name")));
	if (label == NULL) {
		return;
	}
	if (Jp_GameState_IsOfflinePlay()) {
		label->SetLabel(std::wstring(L"OFFLINE PLAY"));
		return;
	}
	const char* serverName = Jp_GameState_ServerName();
	const char* channelName = Jp_GameState_ChannelName();
	if (strlen(channelName) == 0) {
		label->SetLabel(ShiftJisToWString(std::string(serverName)));
		return;
	}
	if (m_serverName != std::string(serverName) || m_channelName != std::string(channelName)) {
		m_serverName = serverName;
		m_channelName = channelName;
		if (g_loadingWaitingDialog == NULL && g_serverNoticeDlg == NULL && s_mode == 0) {
			RFWnd* notice = CreateServerNoticeDialog(this);
			if (notice != NULL) {
				ServerNoticeDialog_SetText(notice,
					std::string(MakeStr("%s > %s", m_serverName.c_str(), m_channelName.c_str())));
				ServerNoticeDialog_Show(notice, true);
			}
		}
		bool talk = Jp_GetChannelType() == 0x10;
		ShowChild(L"btn_talk", talk);
		ShowChild(L"btn_game", !talk);
	}
	std::string text(MakeStr("%s > %s", m_serverName.c_str(), m_channelName.c_str()));
	label->SetLabel(ShiftJisToWString(std::string(text.c_str())));
}

// 0x5fae60
void RFLobbyDesktop::RefreshOnShow_5fae60(bool showBanner) {
	bool premium = Jp_GameState_IsPremium();
	if (m_premium != premium) {
		m_premium = premium;
		if (premium && showBanner) {
			m_bannerShown = true;
		}
		ShowChild(L"btn_premium", premium);
	}
	int64_t time;
	if (!Jp_GameState_FindItemTime(0x1a100002, &time)) {
		return;
	}
	SYSTEMTIME end;
	memset(&end, 0, sizeof(end));
	Jp_SystemTimeFromTime(&end, time);
	int64_t hours = Jp_GameState_HoursUntil(end);
	const char* text;
	if (hours != 0) {
		unsigned short days = (unsigned short)(hours / 24);
		unsigned short rest = (unsigned short)(hours - (int64_t)days * 24);
		if (days != 0 || rest != 0) {
			text = MakeStr(Jp_LookupText(g_jpTextKeyPremiumRemain), days, rest);
			m_premiumText.assign(text, strlen(text));
			return;
		}
	}
	text = Jp_LookupText(g_jpTextKeyPremiumNone);
	m_premiumText.assign(text, strlen(text));
}

// 0x5fafb0
void RFLobbyDesktop::ShowPopupMenu(bool show) {
	ShowChild(L"panel_popup", show);
	m_popupMenuOpen = show;
}

// 0x5fafe0: the bar icons follow the new bar state: enabled in the lobby
// (1); disabled in states 9 (btn_game back to normal) and 10-11
// (btn_practice back to normal); the other states leave them.
void RFLobbyDesktop::OnBarSelectionChanged(int id) {
	RFButton* button;
	switch (id) {
	case 1:
		EnableChild(L"group_lefticon", true);
		EnableChild(L"group_righticon", true);
		return;
	case 9:
		EnableChild(L"group_lefticon", false);
		EnableChild(L"group_righticon", false);
		button = GetChildAs<RFButton>(L"btn_game");
		break;
	case 10:
	case 11:
		EnableChild(L"group_lefticon", false);
		EnableChild(L"group_righticon", false);
		button = GetChildAs<RFButton>(L"btn_practice");
		break;
	default:
		return;
	}
	if (button != NULL) {
		button->SetState(RFButton::RFBUTTON_STATE_NORMAL);
	}
}

// 0x5fb190
void RFLobbyDesktop::SetMessengerBubble(int index, std::string const& text) {
	if (g_messengerBubble != NULL) {
		if (index == 0) {
			g_messengerBubble->Hide();
		} else {
			g_messengerBubble->SetText(std::string(text.c_str()));
		}
	}
}

// 0x5fb250
void RFLobbyDesktop::ShowSpecialCardEmpty() {
	ShowChild(L"wnd_special_card_empty", true);
}

// 0x5fb260 (panel_popup EventKeyDown)
void RFLobbyDesktop::OnKeyDownPopup(EventArgs& args) {
	if (static_cast<KeyEventArgs&>(args).keyCode == 0x1b) {
		ShowPopupMenu(false);
	}
}

// 0x5fb280 (btn_popup)
void RFLobbyDesktop::OnPopupClicked(EventArgs&) {
	if (m_busy == 0) {
		bool open = !IsPopupMenuOpen();
		ShowPopupMenu(open);
		if (open) {
			FocusChild(L"panel_popup");
		}
	}
}

// 0x5fb2d0 (panel_popup EventKillFocus): closes the pop-up menu unless the
// mouse is on it or on btn_popup.
void RFLobbyDesktop::OnKillFocusPopup(EventArgs&) {
	RFWnd* popup = GetChild(std::wstring(L"panel_popup"));
	RFWnd* button = GetChild(std::wstring(L"btn_popup"));
	if (popup != NULL && button != NULL) {
		RFRect popupRect = popup->GetScreenRect();
		RFRect buttonRect = button->GetScreenRect();
		WVector const& mouse = RFSceneManager::Instance()->GetMousePosition();
		if (!popupRect.PtInRect(mouse.x, mouse.y)) {
			if (!buttonRect.PtInRect(mouse.x, mouse.y)) {
				ShowPopupMenu(false);
			}
		}
	}
}

// The JP-only task code the pop-up menu's buttons call (names ours; not
// decompiled yet, declared only). Each ends, like the KR bar handlers, with
// CTaskMain::OnBarHoverOff(0) (JP 0x656a80).
// 0x658e70: CloseAllDialogs, the screen size 800x600, the browser
// (0xe75cb0, 0x8a9fa0) on ".../on_client/gacha/login.aspx?ID=%s&KID=%s".
void TaskMain_OpenGachaPage(CTaskMain* task);
// 0x658e20: the same with ".../on_client/entrance.aspx?ID=%s&KID=%s".
void TaskMain_OpenEntrancePage(CTaskMain* task);
// 0x659340: CloseAllDialogs, then (0x655d70) the "AlchemyDlg" form at the
// JP CTaskMain +0x1a8 unless the server closed the service.
void TaskMain_OpenAlchemyDlg(CTaskMain* task);
// 0x6582f0: connected → packet 0x157 with CSharedDoc +0x8cb and the task's
// message 1; else the 0x753a notice.
void TaskMain_RequestAchievement(CTaskMain* task);
// CTaskMain vtable +0x5c (0x659240): CloseAllDialogs, 800x600, the browser
// on ".../on_client/club/login.aspx?ID=%s&KID=%s".
void TaskMain_OpenClubPage(CTaskMain* task);
// ntAvatarChatMain's override of that slot (0x7b0dd0): the avatar's
// Stop() (0x7a3ea0), then CTaskMain's 0x659240.
void AvatarChatMain_OpenClubPage(ntAvatarChatMain* main);
// 0x593990: packet 0x151 (Send(0)), then this << MsgObject(this, 1) (the
// shape of KR CLobbyMain::OnToppage_MissionEventDown).
void LobbyMain_OpenQuest(CLobbyMain* lobby);
// ntAvatarChatMain vtable +0x68 (0x7b0d20, a JP-only new virtual): the
// avatar's Stop(), packet 0x151, HandleMsg(MsgObject(this, 1)).
void AvatarChatMain_OpenQuest(ntAvatarChatMain* main);

// 0x5fb620 (btn_scratch)
void RFLobbyDesktop::OnPopupScratchClicked(EventArgs&) {
	switch (s_mode) {
	case 0:
		TaskMain_OpenGachaPage(m_taskMain);  // JP only
		break;
	case 1:
		// JP vtable +0x50 = KR CTaskMain::OnUnderBar_GuildUp's slot; the JP
		// override 0x7b0d00 stops the avatar, then the JP CTaskMain body
		// 0x658ec0 only closes the dialogs (no web page, unlike KR).
		m_avatarChatMain->OnUnderBar_GuildUp();
		break;
	}
	ShowPopupMenu(false);
}

// 0x5fb670 (btn_bong)
void RFLobbyDesktop::OnPopupBongClicked(EventArgs&) {
	m_taskMain->OnUnderBar_BongdariShopUp();  // JP vtable +0x54 = KR CTaskMain::OnUnderBar_BongdariShopUp
	ShowPopupMenu(false);
}

// 0x5fb690 (btn_itemtransform)
void RFLobbyDesktop::OnPopupItemTransformClicked(EventArgs&) {
	TaskMain_OpenAlchemyDlg(m_taskMain);  // JP only
	ShowPopupMenu(false);
}

// 0x5fb6b0 (btn_quest)
void RFLobbyDesktop::OnPopupQuestClicked(EventArgs&) {
	switch (s_mode) {
	case 0:
		if (m_lobbyMain != NULL) {
			LobbyMain_OpenQuest(m_lobbyMain);  // JP only
		}
		break;
	case 1:
		if (m_avatarChatMain != NULL) {
			AvatarChatMain_OpenQuest(m_avatarChatMain);  // JP only (vtable +0x68)
		}
		break;
	}
	ShowPopupMenu(false);
}

// 0x5fb700 (btn_notice)
void RFLobbyDesktop::OnPopupNoticeClicked(EventArgs&) {
	if (m_taskMain != NULL) {
		m_taskMain->OnUnderBar_NoticeUp();  // JP vtable +0x60 = KR CTaskMain::OnUnderBar_NoticeUp
	}
	ShowPopupMenu(false);
}

// 0x5fb730 (btn_ranking)
void RFLobbyDesktop::OnPopupRankingClicked(EventArgs&) {
	if (m_taskMain != NULL) {
		// JP vtable +0x58 = KR CTaskMain::OnUnderBar_RankingUp (non-virtual
		// in KR, so the avatar chat's override is not reached here).
		m_taskMain->OnUnderBar_RankingUp();
	}
	ShowPopupMenu(false);
}

// 0x5fb760 (btn_achieve)
void RFLobbyDesktop::OnPopupAchieveClicked(EventArgs&) {
	if (m_taskMain != NULL) {
		TaskMain_RequestAchievement(m_taskMain);  // JP only
	}
	ShowPopupMenu(false);
}

// 0x5fb780 (btn_myguild)
void RFLobbyDesktop::OnPopupMyGuildClicked(EventArgs&) {
	switch (s_mode) {
	case 0:
		if (m_taskMain != NULL) {
			TaskMain_OpenClubPage(m_taskMain);  // JP only (vtable +0x5c)
		}
		break;
	case 1:
		if (m_avatarChatMain != NULL) {
			AvatarChatMain_OpenClubPage(m_avatarChatMain);  // JP only (vtable +0x5c)
		}
		break;
	}
	ShowPopupMenu(false);
}

// 0x5fb7e0 (btn_center)
void RFLobbyDesktop::OnPopupCenterClicked(EventArgs&) {
	if (m_taskMain != NULL) {
		TaskMain_OpenEntrancePage(m_taskMain);  // JP only
	}
	ShowPopupMenu(false);
}

// 0x5fb800 (btn_gacha)
void RFLobbyDesktop::OnPopupGachaClicked(EventArgs&) {
	if (m_taskMain != NULL) {
		TaskMain_OpenGachaPage(m_taskMain);  // JP only
	}
	ShowPopupMenu(false);
}

// 0x5fb440 (pic_server_name EventMouseEnter): the server name
// (m_serverName) with its label, and a second labelled line with
// m_channelName when that is not empty.
void RFLobbyDesktop::OnServerNameEnter(EventArgs& args) {
	if (RFTooltipHelper::Instance() != NULL) {
		std::string first(MakeStr("<font color=\"#ffaa00\">%s</font> %s",
			Jp_LookupText(g_jpTextKeyServerNameLabel), m_serverName.c_str()));
		const char* secondLine;
		if (m_channelName.size() != 0) {
			secondLine = MakeStr("\n<font color=\"#ffaa00\">%s</font> %s",
				Jp_LookupText(g_jpTextKeyServerName2Label), m_channelName.c_str());
		} else {
			secondLine = "";
		}
		std::string second(secondLine);
		std::string text = first + second;
		RFTooltipHelper::Instance()->ShowTooltip(static_cast<WindowEventArgs_<RFWnd>&>(args).sender,
			std::string(text.c_str()), 1, 0xb);
	}
}

// 0x5fbe40
void RFLobbyDesktop::RefreshOnShow() {
	RefreshOnShow_5f8380();
	RefreshOnShow_5fa980();
	RefreshOnShow_5f8f20();
	RefreshOnShow_5fae60(true);
}

// 0x5fbe40 when RFLobbyDesktop::s_instance (0xe78c38) is set (the JP
// inlines the test at each caller, CShopWidget::OnPacket 0x5e0ff0).
void JpLobbyDesktop_RefreshOnShow(void) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->RefreshOnShow();
	}
}

// 0x5fbe70 on s_instance: the JP shop and My Room pages call the member
// directly after their own s_instance test (0xa90004, 0xa8fea8, 0x61dc2f,
// 0x61dda6 ...), so this has no test of its own.
void JpLobbyDesktop_SetBarSelection(int id) {
	RFLobbyDesktop::s_instance->SetBarSelection(id);
}

// 0x5f8020 on s_instance, the same callers (0xa900ce, 0x61df1d).
void JpLobbyDesktop_UpdateLobbyCamera(void) {
	RFLobbyDesktop::s_instance->UpdateLobbyCamera();
}

// 0x5fbe70: switches the bar state (the ids of s_layoutStates): the
// avatar, caddie and mascot actors get the messages of the state left and
// entered, then the "Lobby" actor 0x1a0, and 0x5fafe0(id).
void RFLobbyDesktop::SetBarSelection(int id) {
	if (m_lobbyState == id) {
		return;
	}
	IActor* actor;
	switch (id) {
	case 1:
		if (IsShopState()) {
			if (m_avatarPending) {
				actor = AfxGetTask()->GetActor("LobbyAvatar");
				if (actor != NULL) {
					actor << MsgObject(NULL, 0x189, 0, 0, 0, 0, 0);  // vtable +0x2c, IActor::HandleMsg
				}
				actor = AfxGetTask()->GetActor("LobbyAvatar");
				if (actor != NULL) {
					actor << MsgObject(NULL, 0x85, 0, 0, 0, 0, 0);
				}
			}
			if (m_caddiePending) {
				actor = AfxGetTask()->GetActor("LobbyCaddie");
				if (actor != NULL) {
					actor << MsgObject(NULL, 0x1a9, 0, 0, 0, 0, 0);
				}
			}
			actor = AfxGetTask()->GetActor("LobbyMascot");
			if (actor != NULL) {
				actor << MsgObject(NULL, 0x1aa, 0, 0, 0, 0, 0);
			}
		} else {
			actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor != NULL) {
				actor << MsgObject(NULL, 0x1ce, 0, 0, 0, 0, 0);
			}
			actor = AfxGetTask()->GetActor("LobbyCaddie");
			if (actor != NULL) {
				actor << MsgObject(NULL, 0x1ce, 0, 0, 0, 0, 0);
			}
		}
		break;
	case 2:
	case 3:
	case 4:
		if (m_avatarPending) {
			actor = AfxGetTask()->GetActor("LobbyAvatar");
			if (actor != NULL) {
				actor << MsgObject(NULL, 0x187, 0, 1, 0, 0, 0);
			}
		}
		if (m_caddiePending) {
			actor = AfxGetTask()->GetActor("LobbyCaddie");
			if (actor != NULL) {
				actor << MsgObject(NULL, 0x1a7, 0, 0, 0, 0, 0);
			}
		}
		break;
	case 5:
	case 6:
	case 7:
		if (IsShopState()) {
			if (m_avatarPending) {
				actor = AfxGetTask()->GetActor("LobbyAvatar");
				if (actor != NULL) {
					actor << MsgObject(NULL, 0x189, 0, 0, 0, 0, 0);
				}
				actor = AfxGetTask()->GetActor("LobbyAvatar");
				if (actor != NULL) {
					actor << MsgObject(NULL, 0x85, 0, 0, 0, 0, 0);
				}
			}
			if (m_caddiePending) {
				actor = AfxGetTask()->GetActor("LobbyCaddie");
				if (actor != NULL) {
					actor << MsgObject(NULL, 0x1a8, 0, 0, 0, 0, 0);
				}
				actor = AfxGetTask()->GetActor("LobbyCaddie");
				if (actor != NULL) {
					actor << MsgObject(NULL, 0x1a9, 0, 0, 0, 0, 0);
				}
			}
			actor = AfxGetTask()->GetActor("LobbyMascot");
			if (actor != NULL) {
				actor << MsgObject(NULL, 0x1aa, 0, 0, 0, 0, 0);
			}
		}
		break;
	}
	m_lobbyState1f8 = m_lobbyState;
	m_lobbyState = id;
	actor = AfxGetTask()->GetActor("Lobby");
	if (actor != NULL) {
		actor << MsgObject(NULL, 0x1a0, 0, 0, 0, 0, 0);
	}
	m_shopCameraZoom = false;
	m_avatarPending = false;
	m_caddiePending = false;
	OnBarSelectionChanged(id);
}

// 0x5fc490
void RFLobbyDesktop::UpdateLobby_5fc490() {
	switch (m_lobbyState) {
	case 1:
	case 8:
		m_pendingTaskChange = 0;
		break;
	case 9:
		if (Jp_HasFreshLayoutWindow() && Jp_GetFreshModalFormCount() == 0) {
			m_pendingTaskChange = 0;
			SetBarSelection(1);
			break;
		}
		m_pendingTaskChange = 1;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 10:
	case 11:
	case 12:
		m_pendingTaskChange = 1;
		break;
	}
}

// 0x5fc520: btn_messenger's (and btn_post's) images by notification id;
// 0xcc does nothing.
void RFLobbyDesktop::NotifyMessenger(int id) {
	switch (id) {
	case 0xcc:
		return;
	case 0xcf: {
		RFButton* button = dynamic_cast<RFButton*>(GetChild(std::wstring(L"btn_messenger")));
		if (button != NULL) {
			button->ChangeNormalButtonImage(std::wstring(L"gift_btn_on_1.tga"));
			button->ChangeOverButtonImage(std::wstring(L"gift_btn_on_2.tga"));
			button->ChangeDownButtonImage(std::wstring(L"gift_btn_on_p.tga"));
			m_messengerBlinkTime = 0.0f;
			m_messengerBlink = true;
		}
		return;
	}
	case 0xcd:
	case 0xd1:
		if (Jp_GameState_10d81() || Jp_GameState_10d82()) {
			RFButton* button = dynamic_cast<RFButton*>(GetChild(std::wstring(L"btn_messenger")));
			if (button != NULL) {
				button->ChangeNormalButtonImage(std::wstring(L"pin_btn_on_1.tga"));
				button->ChangeOverButtonImage(std::wstring(L"pin_btn_on_2.tga"));
				button->ChangeDownButtonImage(std::wstring(L"pin_btn_on_p.tga"));
			}
			m_messengerBlinkTime = 0.0f;
			m_messengerBlink = true;
			{
				const char* bubbleText = Jp_GetTableText(0xea63);  // "" for an id the table lacks; NULL guarded anyway
				SetMessengerBubble(1, std::string(bubbleText != NULL ? bubbleText : ""));
			}
			return;
		}
		if (Jp_GameState_TreasureAlarmState() == 3) {
			RFButton* button = GetChildAs<RFButton>(std::wstring(L"btn_messenger"));
			if (button != NULL) {
				button->ChangeNormalButtonImage(std::wstring(L"pin_btn_n.tga"));
				button->ChangeOverButtonImage(std::wstring(L"pin_btn_o.tga"));
				button->ChangeDownButtonImage(std::wstring(L"pin_btn_p.tga"));
				button->SetState(RFButton::RFBUTTON_STATE_NORMAL);
			}
			m_messengerBlink = false;
			return;
		}
		// fall through: the 0xce test
	case 0xce:
		if (Jp_GameState_10d80()) {
			RFButton* button = GetChildAs<RFButton>(std::wstring(L"btn_messenger"));
			if (button != NULL) {
				button->ChangeNormalButtonImage(std::wstring(L"gift_btn_on_1.tga"));
				button->ChangeOverButtonImage(std::wstring(L"gift_btn_on_2.tga"));
				button->ChangeDownButtonImage(std::wstring(L"gift_btn_on_p.tga"));
			}
			RFButton* post = GetChildAs<RFButton>(std::wstring(L"btn_post"));
			if (post != NULL) {
				post->ChangeNormalButtonImage(std::wstring(L"n_post_btn_n.png"));
				post->ChangeOverButtonImage(std::wstring(L"n_post_btn_o.png"));
				post->ChangeDownButtonImage(std::wstring(L"n_post_btn_d.png"));
				m_postPending = true;
			}
			m_messengerBlinkTime = 0.0f;
			m_messengerBlink = true;
			{
				const char* bubbleText = Jp_GetTableText(0xea64);  // "" for an id the table lacks; NULL guarded anyway
				SetMessengerBubble(1, std::string(bubbleText != NULL ? bubbleText : ""));
			}
			return;
		}
		break;
	}
	RFButton* button = dynamic_cast<RFButton*>(GetChild(std::wstring(L"btn_messenger")));
	if (button != NULL) {
		button->ChangeNormalButtonImage(std::wstring(L"pin_btn_n.tga"));
		button->ChangeOverButtonImage(std::wstring(L"pin_btn_o.tga"));
		button->ChangeDownButtonImage(std::wstring(L"pin_btn_p.tga"));
		button->SetState(RFButton::RFBUTTON_STATE_NORMAL);
	}
	m_messengerBlink = false;
}

// 0x5fcb10 (EventUpdate)
void RFLobbyDesktop::OnUpdateDesktop(EventArgs& args) {
	UpdateEventArgs& update = static_cast<UpdateEventArgs&>(args);
	UpdateSpecialCardBlink(update.elapsed);
	UpdateMessengerBlink(update.elapsed);
	if (s_mode == 0) {
		UpdateLobby_5f7310();
		UpdateLobby_5fc490();
	}
	Update_5f9150();
	UpdateBannerFade(update.elapsed);
}

// 0x5fcb70 (btn_shop)
void RFLobbyDesktop::OnShopClicked(EventArgs&) {
	if (Task_IsLayoutOpen(AfxGetTask(), "shopmain")) {
		return;
	}
	if (s_mode != 0) {
		if (s_mode == 1 && m_avatarChatMain != NULL) {
			static_cast<CTaskMain*>(m_avatarChatMain)->OnUnderBar_ShopUp();  // JP vtable +0x44 = KR OnUnderBar_ShopUp
			m_pendingTaskChange = 1;
		}
	} else if (m_busy == 0) {
		m_taskMain->OnUnderBar_ShopUp();  // JP vtable +0x44 = KR CTaskMain::OnUnderBar_ShopUp
		SetBarSelection(2);
		UpdateLobbyCamera();
	}
}

// 0x5fcbf0 (btn_myroom)
void RFLobbyDesktop::OnMyRoomClicked(EventArgs&) {
	if (Task_IsLayoutOpen(AfxGetTask(), "realmyroom")) {
		return;
	}
	if (s_mode != 0) {
		if (s_mode == 1 && m_avatarChatMain != NULL) {
			static_cast<CTaskMain*>(m_avatarChatMain)->OnUnderBar_MyRoomUp();  // JP vtable +0x48 = KR OnUnderBar_MyRoomUp
			m_pendingTaskChange = 1;
		}
	} else if (m_busy == 0) {
		m_taskMain->OnUnderBar_MyRoomUp();  // JP vtable +0x48 = KR CTaskMain::OnUnderBar_MyRoomUp
		SetBarSelection(5);
		UpdateLobbyCamera();
	}
}

// 0x5fcc70 (btn_messenger)
void RFLobbyDesktop::OnMessengerClicked(EventArgs&) {
	SetMessengerBubble(0, std::string(""));
	m_taskMain->OnOverBar_MessengerUp();  // JP 0x65b7d0 = KR CTaskMain::OnOverBar_MessengerUp
	NotifyMessenger(0xcd);
}

// 0x5fff30 (name ours): mode 0 under parent, mode 1 (0x5fffbd) without a
// parent, mode 2 as the manager's desktop.
RFWnd* CreateLobbyDesktop(RFWnd* parent) {
	RFWnd* result = NULL;
	if (g_lobbyDesktopCreateMode == 0) {
		result = CreateWnd<RFLobbyDesktop>(&RFManager::GetInstance(), std::wstring(L"lobby_desktop.xml"), parent);
	} else if (g_lobbyDesktopCreateMode == 1) {
		result = CreateWnd<RFLobbyDesktop>(&RFManager::GetInstance(), std::wstring(L"lobby_desktop.xml"), NULL);
	} else if (g_lobbyDesktopCreateMode == 2) {
		result = CreateDesktop<RFLobbyDesktop>(&RFManager::GetInstance(), std::wstring(L"lobby_desktop.xml"));
	}
	g_lobbyDesktopParent = parent;
	return result;
}
