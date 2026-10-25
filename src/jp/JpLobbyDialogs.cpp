// The JP 983 lobby's pop-up menu and server notice dialog
// (JpLobbyDialogs.h), from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>). Their creators use RFManager.h's
// CreateWnd<T> / CreateDesktopWnd<T> (instances 0x5d5f00 / 0x5d5f90 and
// 0x5d7950 / 0x5d79e0).
#include "JpLobbyDialogs.h"

#include <string>

#include "JpLobbyGlue.h"
#include "RFButton.h"
#include "RFLobbyDesktop.h"
#include "RFManager.h"
#include "RFSceneManager.h"
#include "RFRect.h"
#include "RFSize.h"
#include "RFTextArea.h"
#include "RefreshStringUtil.h"

// 0xe781bc
RFWnd* RFLobbyPopupMenuWnd::s_loadParent = NULL;
// 0xe781cc
RFWnd* RFServerNoticeDlg::s_loadParent = NULL;
// 0xe78c40
RFWnd* RFLobbyMessengerBubble::s_loadParent = NULL;
// 0xe03260
int g_jpModalCreateMode = 0;
// 0xe781a0
RFWnd* RFLoadingProgressDlg::s_loadParent = NULL;
// 0xe781a8
int RFLoadingProgressDlg::s_clearedOnDestroy = 0;

// 0x5d5d20
RFLobbyPopupMenuWnd::RFLobbyPopupMenuWnd() {
	g_lobbyPopupMenuWnd = this;
}

// 0x5d5d40
RFLobbyPopupMenuWnd::~RFLobbyPopupMenuWnd() {
	g_lobbyPopupMenuWnd = NULL;
}

// 0x5d5d60
void RFLobbyPopupMenuWnd::OnKillFocus(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFRect rect = GetScreenRect();
		WVector const& mouse = RFSceneManager::Instance()->GetMousePosition();  // 0xe73ddc, 0x6038b0
		if (!rect.PtInRect(mouse.x, mouse.y)) {
			RFLobbyDesktop::s_instance->FocusPopupPanel();
		}
		RFLobbyDesktop::s_instance->OnKillFocusPopup(args);
	}
}

// 0x5d5dc0
void RFLobbyPopupMenuWnd::OnScratchClicked(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->OnPopupScratchClicked(args);
	}
}

// 0x5d5de0
void RFLobbyPopupMenuWnd::OnBongClicked(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->OnPopupBongClicked(args);
	}
}

// 0x5d5e00
void RFLobbyPopupMenuWnd::OnItemTransformClicked(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->OnPopupItemTransformClicked(args);
	}
}

// 0x5d5e20
void RFLobbyPopupMenuWnd::OnQuestClicked(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->OnPopupQuestClicked(args);
	}
}

// 0x5d5e40
void RFLobbyPopupMenuWnd::OnNoticeClicked(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->OnPopupNoticeClicked(args);
	}
}

// 0x5d5e60
void RFLobbyPopupMenuWnd::OnRankingClicked(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->OnPopupRankingClicked(args);
	}
}

// 0x5d5e80
void RFLobbyPopupMenuWnd::OnAchieveClicked(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->OnPopupAchieveClicked(args);
	}
}

// 0x5d5ea0
void RFLobbyPopupMenuWnd::OnMyGuildClicked(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->OnPopupMyGuildClicked(args);
	}
}

// 0x5d5ec0
void RFLobbyPopupMenuWnd::OnCenterClicked(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->OnPopupCenterClicked(args);
	}
}

// 0x5d5ee0
void RFLobbyPopupMenuWnd::OnGachaClicked(EventArgs& args) {
	if (RFLobbyDesktop::s_instance != NULL) {
		RFLobbyDesktop::s_instance->OnPopupGachaClicked(args);
	}
}

// 0x5d6170 (parent in ecx): the window from lobby_popupmenu_wnd_jp.xml,
// made as g_jpWndCreateMode (0xe73dd8) says; the parent is kept in
// s_loadParent.
RFWnd* CreateLobbyPopupMenuWnd(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFLobbyPopupMenuWnd>(&RFManager::GetInstance(), L"lobby_popupmenu_wnd_jp.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFLobbyPopupMenuWnd>(&RFManager::GetInstance(), L"lobby_popupmenu_wnd_jp.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFLobbyPopupMenuWnd>(&RFManager::GetInstance(), L"lobby_popupmenu_wnd_jp.xml");
		break;
	}
	RFLobbyPopupMenuWnd::s_loadParent = parent;
	return wnd;
}

// 0x5d62b0
void RFLobbyPopupMenuWnd::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_scratch", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_bong", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_itemtransform", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_quest", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_notice", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_ranking", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_achieve", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_myguild", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_center", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_gacha", RFWnd::EventKillFocus, Subscriber(&RFLobbyPopupMenuWnd::OnKillFocus, this));
	RegisterTo(L"btn_scratch", RFButton::EventClicked, Subscriber(&RFLobbyPopupMenuWnd::OnScratchClicked, this));
	RegisterTo(L"btn_bong", RFButton::EventClicked, Subscriber(&RFLobbyPopupMenuWnd::OnBongClicked, this));
	RegisterTo(L"btn_itemtransform", RFButton::EventClicked, Subscriber(&RFLobbyPopupMenuWnd::OnItemTransformClicked, this));
	RegisterTo(L"btn_quest", RFButton::EventClicked, Subscriber(&RFLobbyPopupMenuWnd::OnQuestClicked, this));
	RegisterTo(L"btn_notice", RFButton::EventClicked, Subscriber(&RFLobbyPopupMenuWnd::OnNoticeClicked, this));
	RegisterTo(L"btn_ranking", RFButton::EventClicked, Subscriber(&RFLobbyPopupMenuWnd::OnRankingClicked, this));
	RegisterTo(L"btn_achieve", RFButton::EventClicked, Subscriber(&RFLobbyPopupMenuWnd::OnAchieveClicked, this));
	RegisterTo(L"btn_myguild", RFButton::EventClicked, Subscriber(&RFLobbyPopupMenuWnd::OnMyGuildClicked, this));
	RegisterTo(L"btn_center", RFButton::EventClicked, Subscriber(&RFLobbyPopupMenuWnd::OnCenterClicked, this));
	RegisterTo(L"btn_gacha", RFButton::EventClicked, Subscriber(&RFLobbyPopupMenuWnd::OnGachaClicked, this));
	SetPreDraw(false);
}

// 0x5d78f0
void RFServerNoticeDlg::SetInstance(RFServerNoticeDlg* dialog) {
	g_serverNoticeDlg = dialog;
}

// 0x5d7900
void RFServerNoticeDlg::ClearInstance() {
	g_serverNoticeDlg = NULL;
}

// 0x5d7910
RFServerNoticeDlg::RFServerNoticeDlg()
	: m_closeDelay(1.0f) {
	g_serverNoticeDlg = this;
}

// 0x5d7930
RFServerNoticeDlg::~RFServerNoticeDlg() {
	g_serverNoticeDlg = NULL;
}

// 0x5d7bc0
void RFServerNoticeDlg::Show(bool show) {
	ShowForm(show);
}

// 0x5d7bd0 (parent in ecx): the dialog from server_notice_dialog.xml, made
// as g_jpWndCreateMode (0xe73dd8) says; the parent is kept in
// s_loadParent.
RFWnd* CreateServerNoticeDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFServerNoticeDlg>(&RFManager::GetInstance(), L"server_notice_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFServerNoticeDlg>(&RFManager::GetInstance(), L"server_notice_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFServerNoticeDlg>(&RFManager::GetInstance(), L"server_notice_dialog.xml");
		break;
	}
	RFServerNoticeDlg::s_loadParent = parent;
	return wnd;
}

// 0x5d7d10
void RFServerNoticeDlg::OnUpdateNotice(EventArgs& args) {
	if (m_closeDelay > 0.0f) {
		m_closeDelay -= static_cast<UpdateEventArgs&>(args).elapsed;
		if (m_closeDelay <= 0.0f) {
			m_closeDelay = 0.0f;
			PlayAnimation(L"_Closing_");
		}
	}
	if (m_closeDelay == 0.0f && IsWindowVisible(false) && !IsAnimationPlaying()) {
		Close(static_cast<RFWNDCLOSESTATE>(1));
	}
}

// 0x5d7f70
void RFServerNoticeDlg::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventUpdate, Subscriber(&RFServerNoticeDlg::OnUpdateNotice, this));
}

// What RFLobbyDesktop calls on the created dialog: its [34] Show.
void ServerNoticeDialog_Show(RFWnd* dialog, bool show) {
	static_cast<RFServerNoticeDlg*>(dialog)->Show(show);
}

// 0x5d15a0
void RFLoadingProgressDlg::OnInitialUpdate() {
	SetPreDraw(false);
}

// 0x5d15b0
RFLoadingProgressDlg::RFLoadingProgressDlg() {
	g_loadingWaitingDialog = this;
}

// 0x5d1640
RFLoadingProgressDlg::~RFLoadingProgressDlg() {
	g_loadingWaitingDialog = NULL;
}

// 0x5d1800
void RFLoadingProgressDlg::OnDestroy() {
	s_clearedOnDestroy = 0;
	RFFormModalBase<RFWndEx>::OnDestroy();
	if (RFSceneManager::Instance() != NULL) {
		RFSceneManager::SetFreshInputSuspended(false);
	}
}

// 0x5d1870 (parent in ecx): the dialog from
// common_loading_waiting_dialog.xml, made as g_jpModalCreateMode says;
// the parent is kept in s_loadParent. The constructor stores the instance
// in g_loadingWaitingDialog. The creators are RFManager.h's CreateWnd<T>
// (instance 0x5d16b0) and CreateDesktopWnd<T> (instance 0x5d1740).
RFWnd* CreateLoadingWaitingDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RFLoadingProgressDlg>(&RFManager::GetInstance(), L"common_loading_waiting_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFLoadingProgressDlg>(&RFManager::GetInstance(), L"common_loading_waiting_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFLoadingProgressDlg>(&RFManager::GetInstance(), L"common_loading_waiting_dialog.xml");
		break;
	}
	RFLoadingProgressDlg::s_loadParent = parent;
	return wnd;
}

// 0x5d19b0
void RFLoadingProgressDlg::Open() {
	ShowModal();
	PlayAnimation(L"_Opening_");
	if (RFSceneManager::Instance() != NULL) {
		RFSceneManager::SetFreshInputSuspended(true);
	}
}

// 0x5f69a0
RFLobbyMessengerBubble::RFLobbyMessengerBubble()
	: m_displayTime(0.0f) {
	g_messengerBubble = this;
}

// 0x5f69c0
RFLobbyMessengerBubble::~RFLobbyMessengerBubble() {
	g_messengerBubble = NULL;
}

// 0x5f7f10
void RFLobbyMessengerBubble::Hide() {
	ShowForm(false);
	m_displayTime = 0.0f;
}

// 0x5f7f30
void RFLobbyMessengerBubble::OnUpdateBubble(EventArgs& args) {
	if (IsWindowVisible(false) && m_displayTime > 0.0f) {
		m_displayTime -= static_cast<UpdateEventArgs&>(args).elapsed;
		if (m_displayTime <= 0.0f) {
			Hide();
		}
	}
}

// 0x5f9cb0 (parent in ecx): the bubble from lobby_messenger_bubble_wnd.xml,
// made as g_jpWndCreateMode (0xe73dd8) says; the parent is kept in
// s_loadParent. The constructor stores the instance in g_messengerBubble.
// The creators are RFManager.h's CreateWnd<T> (instance 0x5f6c90) and
// CreateDesktopWnd<T> (instance 0x5f6d20).
RFLobbyMessengerBubble* CreateMessengerBubble(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFLobbyMessengerBubble>(&RFManager::GetInstance(), L"lobby_messenger_bubble_wnd.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFLobbyMessengerBubble>(&RFManager::GetInstance(), L"lobby_messenger_bubble_wnd.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFLobbyMessengerBubble>(&RFManager::GetInstance(), L"lobby_messenger_bubble_wnd.xml");
		break;
	}
	RFLobbyMessengerBubble::s_loadParent = parent;
	return static_cast<RFLobbyMessengerBubble*>(wnd);
}

// 0x5f9df0
void RFLobbyMessengerBubble::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventUpdate, Subscriber(&RFLobbyMessengerBubble::OnUpdateBubble, this));
	SetPreDraw(false);
}

// 0x5f9f10: the box's bottom is the text's height + 14 (0xd04fc0), not its
// top + height, as in the exe.
void RFLobbyMessengerBubble::SetText(std::string const& text) {
	Hide();
	RFTextArea* textArea = dynamic_cast<RFTextArea*>(GetChild(L"notice_box_text"));
	if (textArea != NULL) {
		textArea->SetText(ShiftJisToWString(std::string(text.c_str())));
		RFWnd* box = GetChild(L"wnd_notice_box");
		if (box != NULL) {
			RFRect rect = box->GetRect();
			box->SetRect(RFRect(rect.left, rect.top, rect.right, (float)(textArea->GetSize().height + 14.0)));
			SetRect(box->GetRect());
			s_loadParent->SetRect(box->GetRect());
		}
	}
	ShowForm(true);
	PlayAnimation(L"_Opening_");
	// 5.0f (0xcfea7c)
	m_displayTime = 5.0f;
}
