// The JP 983 character mastery screen's root (JpMasteryMainDlg.h):
// RFCharMasteryAboutDlg, RFCharImgWnd and RFCharMasteryMainDlg, from the JP
// exe's bodies (node scripts/disasm.ts --jp 0x<va>), in address order.
// The creators use RFManager.h's CreateWnd<T> / CreateDesktopWnd<T>
// (instances 0x5bd690 / 0x5bd720 about, 0x5bd7b0 / 0x5bd840 picture,
// 0x5bd8d0 / 0x5bd960 main).
// lib: 0x5bd4b0, 0x5bd610 (the RFModalRenderer sub-objects' [0] thunks),
// 0x5bdb10, 0x5bdb40, 0x5bdb70 (scalar deleting destructors), 0x5bdba0,
// 0x5bdc00 (Subscriber constructors), 0x5bd9f0, 0x5bda20, 0x5bda40,
// 0x5bda50, 0x5bda60, 0x5bda70, 0x5bdaa0, 0x5bdac0, 0x5bdad0, 0x5bdae0,
// 0x5bdc60, 0x5bdc90 (MemberFunctionFunctor<RFCharMasteryAboutDlg> /
// <RFCharMasteryMainDlg>), 0x5bdaf0, 0x5bdb00 (~EventFunctorBase)
#include "JpMasteryMainDlg.h"

#include <string>

#include "JpLobbyDialogs.h"
#include "JpMasteryCard.h"
#include "JpMasterySlot.h"
#include "JpMasteryStatus.h"
#include "JpTooltipWnd.h"
#include "RFButton.h"
#include "RFCheck.h"
#include "RFLabelButton.h"
#include "RFManager.h"
#include "RFPicture.h"
#include "RFTextManager.h"
#include "RefreshStringUtil.h"
#include "SoundManager.h"
#include "commonutil.h"
#include "globalgamedefine.h"

// 0xe780e4
RFCharMasteryMainDlg* g_charMasteryMainDlg = NULL;
// 0xe780d4
RFCharMasteryAboutDlg* g_charMasteryAboutDlg = NULL;
// 0xe780e0
RFWnd* RFCharMasteryMainDlg::s_loadParent = NULL;
// 0xe780d0
RFWnd* RFCharMasteryAboutDlg::s_loadParent = NULL;
// 0xe780d8
RFWnd* RFCharImgWnd::s_loadParent = NULL;

// The sound the game's handlers play (constants 0xd66d08 0.5f and 0xd00d90
// 200.0f).
static void PlayUiSound(const char* name) {
	g_audio->PlaySfx(name, NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x5bd250
void RFCharMasteryAboutDlg::OnConfirmClicked(EventArgs&) {
	PlayUiSound("ui_dialog_close");
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5bd290
void RFCharMasteryMainDlg::OnKeyDownEscape(EventArgs& args) {
	if (static_cast<KeyEventArgs&>(args).keyCode == 0x1b) {
		PlayUiSound("ui_dialog_close");
		Close(static_cast<RFWNDCLOSESTATE>(2));
		args.handled = true;
	}
}

// 0x5bd2f0
void RFCharMasteryMainDlg::OnExitClicked(EventArgs&) {
	PlayUiSound("ui_dialog_close");
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5bd3c0
void RFCharMasteryMainDlg::OnKeyDownForm(EventArgs& args) {
	OnKeyDownEscape(args);
}

// 0x5bd3f0
RFCharMasteryAboutDlg::RFCharMasteryAboutDlg() {
	g_charMasteryAboutDlg = this;
	for (int i = 0; i < PageCount; ++i) {
		m_pageButtons[i] = NULL;
	}
	m_page = 1;
}

// 0x5bd4c0
RFCharMasteryAboutDlg::~RFCharMasteryAboutDlg() {
	g_charMasteryAboutDlg = NULL;
}

// 0x5bd530
RFCharImgWnd::RFCharImgWnd()
	: m_charImg(NULL) {
}

// 0x5bd550
RFCharImgWnd::~RFCharImgWnd() {
}

// 0x5bd560
RFCharMasteryMainDlg::RFCharMasteryMainDlg() {
	g_charMasteryMainDlg = this;
	m_statusTab = NULL;
	m_masteryTab = NULL;
	m_charImgWnd = NULL;
	m_statusDlg = NULL;
	m_cardDlg = NULL;
	m_tileDlg = NULL;
	m_character = NULL;
	m_tab = -1;
}

// 0x5bd620
RFCharMasteryMainDlg::~RFCharMasteryMainDlg() {
	g_charMasteryMainDlg = NULL;
}

// 0x5bdcc0
void RFCharMasteryMainDlg::SelectTab(int tab) {
	if (m_tab != tab) {
		if (m_statusTab != NULL) {
			m_statusTab->SetCheck(tab == 0);
		}
		if (m_masteryTab != NULL) {
			m_masteryTab->SetCheck(tab != 0);
		}
		if (m_statusDlg != NULL) {
			m_statusDlg->ShowForm(tab == 0);
		}
		if (m_cardDlg != NULL) {
			m_cardDlg->ShowForm(tab == 0);
		}
		if (m_tileDlg != NULL) {
			m_tileDlg->ShowForm(tab != 0);
		}
	}
}

// 0x5bdd50
void RFCharImgWnd::SetCharacter(uint32_t typeId) {
	if (m_charImg != NULL && typeId != 0) {
		std::string name(MakeStr("mastery_char_img_%d.png", typeId & 0x3ffffff));
		m_charImg->SetPicture(ShiftJisToWString(name));
	}
}

// 0x5bde50
void RFCharMasteryMainDlg::OnStatusTabChanged(EventArgs&) {
	SelectTab(0);
	PlayUiSound("ui_icon_click");
}

// 0x5bde90
void RFCharMasteryMainDlg::OnMasteryTabChanged(EventArgs&) {
	SelectTab(1);
	PlayUiSound("ui_icon_click");
}

// 0x5bded0
RFCharMasteryAboutDlg* CreateCharMasteryAboutDlg(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharMasteryAboutDlg>(&RFManager::GetInstance(), L"kam_mastery_about_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharMasteryAboutDlg>(&RFManager::GetInstance(), L"kam_mastery_about_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharMasteryAboutDlg>(&RFManager::GetInstance(), L"kam_mastery_about_dialog.xml");
		break;
	}
	RFCharMasteryAboutDlg::s_loadParent = parent;
	return static_cast<RFCharMasteryAboutDlg*>(wnd);
}

// 0x5be010
void RFCharMasteryAboutDlg::ShowPage(int page) {
	for (int i = 0; i < PageCount; ++i) {
		if (m_pageButtons[i] != NULL) {
			m_pageButtons[i]->EnableWindow(true);
		}
	}
	m_pageButtons[page - 1]->EnableWindow(false);
	m_page = page;
	RFPicture* picture = dynamic_cast<RFPicture*>(GetChild(std::wstring(L"pic_help_mastery_01")));
	std::string name(MakeStr("char_master_help_%d.jpg", page));
	picture->SetPicture(ShiftJisToWString(name));
}

// 0x5be1a0
void RFCharMasteryAboutDlg::PreviousPage() {
	--m_page;
	if (m_page < 1) {
		m_page = 1;
	}
	ShowPage(m_page);
	PlayUiSound("ui_icon_click");
}

// 0x5be200
void RFCharMasteryAboutDlg::NextPage() {
	++m_page;
	if (m_page > PageCount) {
		m_page = PageCount;
	}
	ShowPage(m_page);
	PlayUiSound("ui_icon_click");
}

// 0x5be260
RFCharImgWnd* CreateCharImgWnd(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharImgWnd>(&RFManager::GetInstance(), L"kam_char_img_wnd.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharImgWnd>(&RFManager::GetInstance(), L"kam_char_img_wnd.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharImgWnd>(&RFManager::GetInstance(), L"kam_char_img_wnd.xml");
		break;
	}
	RFCharImgWnd::s_loadParent = parent;
	return static_cast<RFCharImgWnd*>(wnd);
}

// 0x5be3a0
void RFCharImgWnd::OnInitialUpdate() {
	m_charImg = dynamic_cast<RFPicture*>(GetChild(std::wstring(L"char_img")));
}

// 0x5be450
RFWnd* CreateCharMasteryMainDlg(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharMasteryMainDlg>(&RFManager::GetInstance(), L"kam_mastery_main_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharMasteryMainDlg>(&RFManager::GetInstance(), L"kam_mastery_main_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharMasteryMainDlg>(&RFManager::GetInstance(), L"kam_mastery_main_dialog.xml");
		break;
	}
	RFCharMasteryMainDlg::s_loadParent = parent;
	return wnd;
}

// 0x5be590
void RFCharMasteryMainDlg::CreateCharImg() {
	RFWndEx* placeholder = dynamic_cast<RFWndEx*>(GetChild(std::wstring(L"char_img_wnd")));
	if (placeholder != NULL) {
		m_charImgWnd = CreateCharImgWnd(this);
		if (m_charImgWnd != NULL) {
			RFVector2 pos = placeholder->GetPos();
			m_charImgWnd->SetPos(pos.x, placeholder->GetPos().y);
			m_charImgWnd->SetCharacter((uint32_t)m_character->typeId);
			m_charImgWnd->ShowForm(true);
		}
		DeleteChild(placeholder);
	}
}

// 0x5be6c0
void RFCharMasteryMainDlg::CreateStatusDlg() {
	if (g_charMasteryStatusDlg == NULL) {
		RFWndEx* placeholder = dynamic_cast<RFWndEx*>(GetChild(std::wstring(L"status_wnd")));
		if (placeholder != NULL) {
			m_statusDlg = static_cast<RFCharMasteryStatusDlg*>(CreateCharMasteryStatusDlg(this));
			if (m_statusDlg != NULL) {
				RFVector2 pos = placeholder->GetPos();
				m_statusDlg->SetPos(pos.x, placeholder->GetPos().y);
				m_statusDlg->ShowForm(true);
			}
			DeleteChild(placeholder);
		}
	}
}

// 0x5be7e0
void RFCharMasteryMainDlg::CreateCardDlg() {
	if (g_charMasteryCardDlg == NULL) {
		RFWndEx* placeholder = dynamic_cast<RFWndEx*>(GetChild(std::wstring(L"card_wnd")));
		if (placeholder != NULL) {
			m_cardDlg = static_cast<RFCharMasteryCardDlg*>(CreateCharMasteryCardDlg(this));
			if (m_cardDlg != NULL) {
				RFVector2 pos = placeholder->GetPos();
				m_cardDlg->SetPos(pos.x, placeholder->GetPos().y);
				m_cardDlg->SetCharacterUid((uint32_t)m_character->id);
				m_cardDlg->ShowForm(true);
			}
			DeleteChild(placeholder);
		}
	}
}

// 0x5be920
void RFCharMasteryMainDlg::CreateTileDlg() {
	if (RFCharMasteryTileDlg::s_instance == NULL) {
		RFWndEx* placeholder = dynamic_cast<RFWndEx*>(GetChild(std::wstring(L"slot_mastery_wnd")));
		if (placeholder != NULL) {
			m_tileDlg = static_cast<RFCharMasteryTileDlg*>(CreateMasteryTileDialog(this));
			if (m_tileDlg != NULL) {
				m_tileDlg->Setup();
				RFVector2 pos = placeholder->GetPos();
				m_tileDlg->SetPos(pos.x, placeholder->GetPos().y);
				m_tileDlg->ShowForm(true);
			}
			DeleteChild(placeholder);
		}
	}
}

// 0x5bea50
void RFCharMasteryMainDlg::OnHelpClicked(EventArgs&) {
	RFCharMasteryAboutDlg* about = CreateCharMasteryAboutDlg(this);
	if (about != NULL) {
		about->ShowModal();
	}
}

// 0x5bea70
void RFCharMasteryAboutDlg::SetButtonLabel(const wchar_t* name, const std::wstring& text) {
	RFLabelButton* button = dynamic_cast<RFLabelButton*>(GetChild(std::wstring(name)));
	if (button != NULL) {
		button->SetLabel(std::wstring(text.c_str()));
	}
}

// 0x5bebb0
void RFCharMasteryAboutDlg::OnPage1Clicked(EventArgs&) {
	ShowPage(1);
}

// 0x5bebc0
void RFCharMasteryAboutDlg::OnPage2Clicked(EventArgs&) {
	ShowPage(2);
}

// 0x5bebd0
void RFCharMasteryAboutDlg::OnPage3Clicked(EventArgs&) {
	ShowPage(3);
}

// 0x5bebe0
void RFCharMasteryAboutDlg::OnPage4Clicked(EventArgs&) {
	ShowPage(4);
}

// 0x5bebf0
void RFCharMasteryAboutDlg::OnPage5Clicked(EventArgs&) {
	ShowPage(5);
}

// 0x5bec00
void RFCharMasteryAboutDlg::OnPage6Clicked(EventArgs&) {
	ShowPage(6);
}

// 0x5bec10
void RFCharMasteryAboutDlg::OnPage7Clicked(EventArgs&) {
	ShowPage(7);
}

// 0x5bec20
void RFCharMasteryAboutDlg::OnPage8Clicked(EventArgs&) {
	ShowPage(8);
}

// 0x5bec30
void RFCharMasteryAboutDlg::OnPage9Clicked(EventArgs&) {
	ShowPage(9);
}

// 0x5bec40
void RFCharMasteryAboutDlg::OnForwardClicked(EventArgs&) {
	NextPage();
}

// 0x5bec50
void RFCharMasteryAboutDlg::OnBackwardClicked(EventArgs&) {
	PreviousPage();
}

// 0x5bec60
void RFCharMasteryMainDlg::SetupHandlers() {
	RegisterTo(L"", RFWnd::EventKeyDown, Subscriber(&RFCharMasteryMainDlg::OnKeyDownForm, this));
	RegisterTo(L"exit_button", RFButton::EventClicked, Subscriber(&RFCharMasteryMainDlg::OnExitClicked, this));
	RegisterTo(L"help_button", RFButton::EventClicked, Subscriber(&RFCharMasteryMainDlg::OnHelpClicked, this));
	RegisterTo(L"status_mgr_tap_check", RFCheck::EventChanged, Subscriber(&RFCharMasteryMainDlg::OnStatusTabChanged, this));
	RegisterTo(L"mastery_mgr_tap_check", RFCheck::EventChanged, Subscriber(&RFCharMasteryMainDlg::OnMasteryTabChanged, this));
	m_statusTab = dynamic_cast<RFCheck*>(GetChild(std::wstring(L"status_mgr_tap_check")));
	m_masteryTab = dynamic_cast<RFCheck*>(GetChild(std::wstring(L"mastery_mgr_tap_check")));
	m_character = Jp_GameState_EquippedCharacterData();
}

// 0x5bf150
void RFCharMasteryAboutDlg::OnInitialUpdate() {
	RegisterTo(L"label_btn_about_01", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnPage1Clicked, this));
	RegisterTo(L"label_btn_about_02", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnPage2Clicked, this));
	RegisterTo(L"label_btn_about_03", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnPage3Clicked, this));
	RegisterTo(L"label_btn_about_04", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnPage4Clicked, this));
	RegisterTo(L"label_btn_about_05", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnPage5Clicked, this));
	RegisterTo(L"label_btn_about_06", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnPage6Clicked, this));
	RegisterTo(L"label_btn_about_07", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnPage7Clicked, this));
	RegisterTo(L"label_btn_about_08", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnPage8Clicked, this));
	RegisterTo(L"label_btn_about_09", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnPage9Clicked, this));
	RegisterTo(L"btn_forward", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnForwardClicked, this));
	RegisterTo(L"btn_backward", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnBackwardClicked, this));
	RegisterTo(L"btn_confirm", RFButton::EventClicked, Subscriber(&RFCharMasteryAboutDlg::OnConfirmClicked, this));

	for (int page = 1; page <= PageCount; ++page) {
		std::string name(MakeStr("label_btn_about_%02d", page));
		m_pageButtons[page - 1] = dynamic_cast<RFLabelButton*>(GetChild(ShiftJisToWString(name)));
	}

	// The Korean keys (EUC-KR bytes, 0xd0f1a0 ... 0xd0f110) of the JP texts.
	SetButtonLabel(L"label_btn_about_01", ShiftJisToWString(std::string(Jp_LookupText("1. \xc4\xb3\xb8\xaf\xc5\xcd \xb8\xb6\xbd\xba\xc5\xcd\xb8\xae"))));
	SetButtonLabel(L"label_btn_about_02", ShiftJisToWString(std::string(Jp_LookupText("2. \xbd\xba\xc5\xdd\xc0\xc7 \xc1\xbe\xb7\xf9"))));
	SetButtonLabel(L"label_btn_about_03", ShiftJisToWString(std::string(Jp_LookupText("3. \xbd\xba\xc5\xdd \xb0\xfc\xb8\xae"))));
	SetButtonLabel(L"label_btn_about_04", ShiftJisToWString(std::string(Jp_LookupText("4. \xbd\xba\xc5\xdd\xb9\xd9 \xbc\xbc\xba\xce\xbc\xb3\xb8\xed"))));
	SetButtonLabel(L"label_btn_about_05", ShiftJisToWString(std::string(Jp_LookupText("5. \xbe\xf7/\xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5"))));
	SetButtonLabel(L"label_btn_about_06", ShiftJisToWString(std::string(Jp_LookupText("6. \xc4\xab\xb5\xe5 \xc0\xe5\xba\xf1/\xc7\xd8\xc1\xa6"))));
	SetButtonLabel(L"label_btn_about_07", ShiftJisToWString(std::string(Jp_LookupText("7. \xbd\xbd\xb7\xd4 \xb0\xfc\xb8\xae"))));
	SetButtonLabel(L"label_btn_about_08", ShiftJisToWString(std::string(Jp_LookupText("8. \xba\xb8\xb3\xca\xbd\xba\xbf\xcd \xc6\xe4\xb3\xce\xc6\xbc"))));
	SetButtonLabel(L"label_btn_about_09", ShiftJisToWString(std::string(Jp_LookupText("9. \xbd\xbd\xb7\xd4 \xc8\xae\xc0\xe5"))));

	CenterWindow(NULL);
	ShowPage(1);
	PlayUiSound("ui_dialog_open");
}

// 0x5c0160
void RFCharMasteryMainDlg::OnInitialUpdate() {
	SetupHandlers();
	CreateCharImg();
	CreateCardDlg();
	CreateStatusDlg();
	CreateTileDlg();
	RFPicture* title = dynamic_cast<RFPicture*>(GetChild(std::wstring(L"title_mastery")));
	if (title != NULL) {
		BringChildWndToTop(title);
	}
	SelectTab(0);
}
