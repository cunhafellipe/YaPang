// The JP 983 2014 Arin birthday event screen (JpArinEvent.h): the main
// dialog RF2014ArinBirthdayDlg, the two stage windows, the notify popup
// and the two mission popups, from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>), in address order.
// The creators use RFManager.h's CreateWnd<T> / CreateDesktopWnd<T>
// (instances 0x5dde30 / 0x5ddec0, 0x5d89c0 / 0x5d8a50, 0x5dbfd0 / 0x5dc060).
// lib: 0x5ddd00 (the RFModalRenderer sub-object's [0] thunk), 0x5de090
// (scalar deleting destructor), 0x449190 (std::vector<std::string>::_Tidy),
// the implicit ~RF2014ArinStage2Wnd 0x5dad10, 0x5dd950
// (std::vector<ArinBirthday2014MissionText>::_Tidy), 0x5dddb0 (its
// push_back), 0x5d9880 (~ArinBirthday2014MissionText), 0x5de470 / 0x5de500
// and the map popup's CreateWnd / CreateDesktopWnd instances, 0x5dbee0
// (std::vector<ArinBirthday2014Mission>::operator=), 0x5d8ae0 / 0x5d8b70
// (the notify popup's CreateWnd / CreateDesktopWnd instances)
#include "JpArinEvent.h"

#include <string>

#include "JpLobbyDialogs.h"
#include "JpLobbyGlue.h"
#include "JpMasteryCard.h"
#include "JpTooltipWnd.h"
#include "Packet.h"
#include "RFButton.h"
#include "RFEventArgs.h"
#include "RFLabel.h"
#include "RFManager.h"
#include "RFPicture.h"
#include "RFTextArea.h"
#include "RFTextManager.h"
#include "RefreshStringUtil.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "actor.h"
#include "commonutil.h"

namespace _gateway {

// 0xd11768
const uint32_t g_arinTicketItemTypeIds[5] = {
	0x1a0002bb, 0x1a0002bc, 0x1a0002bd, 0x1a0002be, 0x1a0002bf
};
// 0xe781d8
RF2014ArinBirthdayDlg* g_arinBirthday2014Dlg = NULL;
// 0xe781d4
RFWnd* RF2014ArinBirthdayDlg::s_loadParent = NULL;
// 0xe781dc
RFWnd* RF2014ArinStage1Wnd::s_loadParent = NULL;
// 0xe781e4
RFWnd* RF2014ArinStage2Wnd::s_loadParent = NULL;
// 0xe781ec
RFWnd* RF2014ArinNotifyDlg::s_loadParent = NULL;
// 0xe781f0
RF2014ArinNotifyDlg* g_arinNotifyDlg = NULL;
// 0xe781f4
RFWnd* RF2014ArinMissionInfoDlg::s_loadParent = NULL;
// 0xe781f8
RF2014ArinMissionInfoDlg* g_arinMissionInfoDlg = NULL;
// 0xe781fc
RFWnd* RF2014ArinMapMissionInfoDlg::s_loadParent = NULL;
// 0xe78200
RF2014ArinMapMissionInfoDlg* g_arinMapMissionInfoDlg = NULL;

// 0x5d81f0
void RF2014ArinBirthdayDlg::SetEventState(CArinBirthDay2014Doc* state) {
	m_eventState = state;
}

// 0x5d8200
void RF2014ArinBirthdayDlg::SetStage(int stage) {
	m_stage = stage;
}

// 0x5d8210
void RF2014ArinBirthdayDlg::OnCloseClicked(EventArgs&) {
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5d8220
void RF2014ArinStage1Wnd::OnTicket1DblClick(EventArgs&) {
}

// 0x5d8230
void RF2014ArinStage1Wnd::OnTicket2DblClick(EventArgs&) {
}

// 0x5d8240
void RF2014ArinStage1Wnd::OnTicket3DblClick(EventArgs&) {
}

// 0x5d8250
void RF2014ArinStage1Wnd::OnTicket4DblClick(EventArgs&) {
}

// 0x5d8260
void RF2014ArinStage1Wnd::OnTicket5DblClick(EventArgs&) {
}

// 0x5d8270
bool RF2014ArinStage1Wnd::HasTicket(int index) {
	return m_ticketCounts[index] > 0;
}

// 0x5d8290
void RF2014ArinStage1Wnd::ConsumeTicket(int index) {
	if (m_ticketCounts[index] != 0) {
		--m_ticketCounts[index];
		m_ticketUsed[index] = true;
	}
}

// 0x5d82c0
void RF2014ArinStage2Wnd::SetStage(int stage) {
	m_stage = stage;
}

// 0x5d82d0
int RF2014ArinStage2Wnd::CountRewarded() {
	int count = 0;
	if (m_rewarded[0]) {
		count = 1;
	}
	if (m_rewarded[1]) {
		++count;
	}
	if (m_rewarded[2]) {
		++count;
	}
	if (m_rewarded[3]) {
		++count;
	}
	if (m_rewarded[4]) {
		++count;
	}
	return count;
}

// 0x5d8310
void RF2014ArinNotifyDlg::OnCheckClicked(EventArgs&) {
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5d8320
void RF2014ArinMissionInfoDlg::OnCheckClicked(EventArgs&) {
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5d8330
void RF2014ArinMapMissionInfoDlg::OnCheckClicked(EventArgs&) {
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5d86e0
void RF2014ArinBirthdayDlg::OnKeyDownForm(EventArgs& args) {
	if (static_cast<KeyEventArgs&>(args).keyCode == 0x1b) {
		OnCloseClicked(args);
		args.handled = true;
	}
}

// 0x5d8700
RF2014ArinStage1Wnd::RF2014ArinStage1Wnd() {
	for (int i = 0; i < 5; ++i) {
		m_ticketCounts[i] = 0;
		m_ticketUsed[i] = false;
	}
}

// 0x5d8750
RF2014ArinNotifyDlg::RF2014ArinNotifyDlg() {
	g_arinNotifyDlg = this;
}

// 0x5d9020
void SendArinBirthday2014Request() {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	WSendPacket packet(0x190);
	packet.Send(SEND_TO_DEFAULT);
}

// 0x5d9100
void RF2014ArinStage1Wnd::OnAllTicketsUsed() {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	WSendPacket packet(0x191);
	packet.Send(SEND_TO_DEFAULT);
}

// 0x5d91b0
void RF2014ArinStage2Wnd::RequestReward(int index) {
	if (m_missions[index].m_completed && m_missions[index].m_rewarded != true) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
		WSendPacket packet(0x193);
		packet.Encode4(index + 0x15);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// 0x5d9290
void RF2014ArinStage2Wnd::RequestFinalReward() {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	WSendPacket packet(0x194);
	packet.Send(SEND_TO_DEFAULT);
}

// 0x5d97f0
void RF2014ArinStage2Wnd::OnReward1Clicked(EventArgs&) {
	RequestReward(0);
}

// 0x5d9800
void RF2014ArinStage2Wnd::OnReward2Clicked(EventArgs&) {
	RequestReward(1);
}

// 0x5d9810
void RF2014ArinStage2Wnd::OnReward3Clicked(EventArgs&) {
	RequestReward(2);
}

// 0x5d9820
void RF2014ArinStage2Wnd::OnReward4Clicked(EventArgs&) {
	RequestReward(3);
}

// 0x5d9830
void RF2014ArinStage2Wnd::OnReward5Clicked(EventArgs&) {
	RequestReward(4);
}

// 0x5d9840
void RF2014ArinStage2Wnd::OnFinalClicked(EventArgs&) {
	RequestFinalReward();
}

// 0x5d99b0
void RF2014ArinStage1Wnd::SetTicketCount(int index, int count) {
	RFLabel* label = dynamic_cast<RFLabel*>(GetChild(ShiftJisToWString(std::string(Jp_LookupText(MakeStr("%s_%02d", "label_ticket", index + 1))))));
	m_ticketCounts[index] = count;
	label->SetLabel(count, false);
}

// 0x5d9ae0
void RF2014ArinStage1Wnd::ShowTicketPicture(int index, bool show) {
	RFPicture* picture = dynamic_cast<RFPicture*>(GetChild(ShiftJisToWString(std::string(Jp_LookupText(MakeStr("%s%d", "pic_event_ticket", index + 1))))));
	picture->ShowWindow(show);
}

// 0x5d9c00
void RF2014ArinStage2Wnd::ShowMissionButton(int index, bool show) {
	RFButton* button = dynamic_cast<RFButton*>(GetChild(ShiftJisToWString(std::string(Jp_LookupText(MakeStr("%s_%02d", "btn_mission", index + 1))))));
	button->ShowWindow(show);
}

// 0x5d9d20
void RF2014ArinStage2Wnd::ShowRewardButton(int index, bool show) {
	RFButton* button = dynamic_cast<RFButton*>(GetChild(ShiftJisToWString(std::string(Jp_LookupText(MakeStr("%s_%02d", "btn_reward", index + 1))))));
	button->ShowWindow(show);
}

// 0x5d9f80
void RF2014ArinStage2Wnd::SetRewardState(int index, RFButton::RFBUTTON_STATE state) {
	RFButton* button = dynamic_cast<RFButton*>(GetChild(ShiftJisToWString(std::string(Jp_LookupText(MakeStr("%s_%02d", "btn_reward", index + 1))))));
	button->SetState(state);
}

// 0x5da3f0
void RF2014ArinBirthdayDlg::ShowText(int index) {
	RFTextArea* text = dynamic_cast<RFTextArea*>(GetChild(L"text_area"));
	text->SetText(ShiftJisToWString(std::string(Jp_LookupText(m_texts[index].c_str()))));
}

// 0x5da550
void RF2014ArinBirthdayDlg::ShowOkButton(bool show) {
	RFButton* ok = dynamic_cast<RFButton*>(GetChild(L"btn_ok"));
	ok->ShowWindow(show);
}

// 0x5da620
void RF2014ArinBirthdayDlg::SetSecondBackground() {
	RFPicture* background = dynamic_cast<RFPicture*>(GetChild(L"pic_arinbirth_bg"));
	background->SetPicture(L"pic_arinbirth_bg_02.png");
}

// 0x5da720
void RF2014ArinBirthdayDlg::SetFirstBackground() {
	RFPicture* background = dynamic_cast<RFPicture*>(GetChild(L"pic_arinbirth_bg"));
	background->SetPicture(L"pic_arinbirth_bg_01.png");
}

// 0x5da820
void RF2014ArinBirthdayDlg::SetTikiPicture(int index) {
	RFPicture* character = dynamic_cast<RFPicture*>(GetChild(L"pic_char"));
	character->SetPicture(ShiftJisToWString(std::string(MakeStr("pic_tiki_%d.png", index + 1))));
}

// 0x5da970
void RF2014ArinBirthdayDlg::SetArinPicture(int index) {
	RFPicture* character = dynamic_cast<RFPicture*>(GetChild(L"pic_char"));
	character->SetPicture(ShiftJisToWString(std::string(MakeStr("pic_arin_%d.png", index + 1))));
}

// 0x5daac0
RF2014ArinStage1Wnd* CreateArinStage1Wnd(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RF2014ArinStage1Wnd>(&RFManager::GetInstance(), L"event_arinbirth_ticket_wnd.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RF2014ArinStage1Wnd>(&RFManager::GetInstance(), L"event_arinbirth_ticket_wnd.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RF2014ArinStage1Wnd>(&RFManager::GetInstance(), L"event_arinbirth_ticket_wnd.xml");
		break;
	}
	RF2014ArinStage1Wnd::s_loadParent = parent;
	return static_cast<RF2014ArinStage1Wnd*>(wnd);
}

// 0x5dac00
void RF2014ArinStage1Wnd::ShowTicketBase(bool show) {
	RFPicture* picture = dynamic_cast<RFPicture*>(GetChild(L"pic_event_ticket"));
	picture->ShowWindow(show);
}

// 0x5dacd0
RF2014ArinStage2Wnd::RF2014ArinStage2Wnd() {
	m_stage = 0;
	m_clearedByCtor = false;
	m_rewarded[0] = false;
	m_rewarded[1] = false;
	m_rewarded[2] = false;
	m_rewarded[3] = false;
	m_rewarded[4] = false;
}

// 0x5dadb0
void RF2014ArinStage2Wnd::SetMagicBackground(int count) {
	RFPicture* background = dynamic_cast<RFPicture*>(GetChild(L"pic_event_magic_bg"));
	background->SetPicture(ShiftJisToWString(std::string(MakeStr("pic_event_magic_bg_%02d.png", count))));
}

// 0x5daf00
void RF2014ArinStage2Wnd::ShowFinalButton(bool show) {
	RFButton* button = dynamic_cast<RFButton*>(GetChild(L"btn_final"));
	button->ShowWindow(show);
}

// 0x5dafd0
void RF2014ArinStage2Wnd::ShowFinalPictures(bool show) {
	RFPicture* first = dynamic_cast<RFPicture*>(GetChild(L"pic_final_1"));
	first->ShowWindow(show);
	RFPicture* second = dynamic_cast<RFPicture*>(GetChild(L"pic_final_2"));
	second->ShowWindow(show);
}

// 0x5db110
bool RF2014ArinStage2Wnd::PlayFinalIfAllRewarded() {
	if (m_stage == 0xb) {
		return false;
	}
	for (int i = 0; i < 5; ++i) {
		if (!m_rewarded[i]) {
			return false;
		}
	}
	PlayAnimation(L"_Final_");
	return true;
}

// 0x5db1d0
RFWnd* CreateArinNotifyDlg(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RF2014ArinNotifyDlg>(&RFManager::GetInstance(), L"event_arinbirth_popup_1_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RF2014ArinNotifyDlg>(&RFManager::GetInstance(), L"event_arinbirth_popup_1_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RF2014ArinNotifyDlg>(&RFManager::GetInstance(), L"event_arinbirth_popup_1_dialog.xml");
		break;
	}
	RF2014ArinNotifyDlg::s_loadParent = parent;
	return wnd;
}

// 0x5db310
void RF2014ArinNotifyDlg::SetText(std::string text) {
	RFTextArea* area = dynamic_cast<RFTextArea*>(GetChild(L"TextArea"));
	if (area != NULL) {
		area->SetText(ShiftJisToWString(std::string(Jp_LookupText(text.c_str()))));
	}
}

// 0x5db470
void RF2014ArinNotifyDlg::OnInitialUpdate() {
	RegisterTo(L"btn_evert_arinbirth_ckeck", RFButton::EventClicked, Subscriber(&RF2014ArinNotifyDlg::OnCheckClicked, this));
}

// 0x5db570
void RF2014ArinMissionInfoDlg::SetMission(int index, const ArinBirthday2014Mission* mission) {
	RFLabel* label = dynamic_cast<RFLabel*>(GetChild(L"Label"));
	if (label != NULL) {
		label->SetLabel(ShiftJisToWString(std::string(Jp_LookupText(MakeStr(m_texts[index].m_title.c_str(), mission->m_goal)))));
	}
	RFTextArea* text = dynamic_cast<RFTextArea*>(GetChild(L"TextArea"));
	if (text != NULL) {
		text->SetText(ShiftJisToWString(std::string(Jp_LookupText(MakeStr(m_texts[index].m_body.c_str(), mission->m_progress, mission->m_goal)))));
	}
}

// 0x5db850
void RF2014ArinMissionInfoDlg::OnInitialUpdate() {
	RegisterTo(L"btn_evert_arinbirth_ckeck", RFButton::EventClicked, Subscriber(&RF2014ArinMissionInfoDlg::OnCheckClicked, this));
}

// 0x5db950
void RF2014ArinMapMissionInfoDlg::SetMission(const ArinBirthday2014Mission* mission) {
	RFLabel* label = dynamic_cast<RFLabel*>(GetChild(L"Label"));
	if (label != NULL) {
		label->SetLabel(ShiftJisToWString(std::string(Jp_LookupText(m_texts[0].m_title.c_str()))));
	}
	if (mission->m_progress & 0x20) {
		SetCoursePicture(L"pic_westwiz", L"pic_westwiz_o.png");
	}
	if (mission->m_progress & 0x40000) {
		SetCoursePicture(L"pic_wizcity", L"pic_wizcity_o.png");
	}
	if (mission->m_progress & 0x200) {
		SetCoursePicture(L"pic_whitewiz", L"pic_whitewiz_o.png");
	}
	if (mission->m_progress & 0x10) {
		SetCoursePicture(L"pic_wizwiz", L"pic_wizwiz_o.png");
	}
}

// (inlined four times in 0x5db950)
void RF2014ArinMapMissionInfoDlg::SetCoursePicture(const wchar_t* name, const wchar_t* picture) {
	RFPicture* course = dynamic_cast<RFPicture*>(GetChild(name));
	if (course != NULL) {
		course->SetPicture(picture);
	}
}

// 0x5dbde0
void RF2014ArinMapMissionInfoDlg::OnInitialUpdate() {
	RegisterTo(L"btn_evert_arinbirth_ckeck", RFButton::EventClicked, Subscriber(&RF2014ArinMapMissionInfoDlg::OnCheckClicked, this));
}

// 0x5dc280
void RF2014ArinBirthdayDlg::ShowRewardNotify() {
	RF2014ArinNotifyDlg* notify = static_cast<RF2014ArinNotifyDlg*>(CreateArinNotifyDlg(NULL));
	if (notify != NULL) {
		notify->SetText(std::string(Jp_GetTableText(0x30e1e)));
		notify->CenterWindow(NULL);
		notify->ShowModal();
	}
}

// 0x5dc2f0
void RF2014ArinBirthdayDlg::ShowFinalNotify() {
	RF2014ArinNotifyDlg* notify = static_cast<RF2014ArinNotifyDlg*>(CreateArinNotifyDlg(NULL));
	if (notify != NULL) {
		notify->SetText(std::string(Jp_GetTableText(0x30e1f)));
		notify->CenterWindow(NULL);
		notify->ShowModal();
	}
}

// 0x5dc360
void RF2014ArinStage1Wnd::CheckAllTicketsUsed() {
	for (int i = 0; i < 5; ++i) {
		if (!m_ticketUsed[i]) {
			return;
		}
	}
	ShowTicketBase(true);
	OnAllTicketsUsed();
}

// 0x5dc390
RF2014ArinStage2Wnd* CreateArinStage2Wnd(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RF2014ArinStage2Wnd>(&RFManager::GetInstance(), L"event_arinbirth_magic_wnd.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RF2014ArinStage2Wnd>(&RFManager::GetInstance(), L"event_arinbirth_magic_wnd.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RF2014ArinStage2Wnd>(&RFManager::GetInstance(), L"event_arinbirth_magic_wnd.xml");
		break;
	}
	RF2014ArinStage2Wnd::s_loadParent = parent;
	return static_cast<RF2014ArinStage2Wnd*>(wnd);
}

// 0x5dc500
void RF2014ArinStage2Wnd::ShowAll(bool show) {
	for (int i = 0; i < 5; ++i) {
		ShowMissionButton(i, show);
		ShowRewardButton(i, show);
	}
	ShowFinalButton(show);
	ShowFinalPictures(show);
}

// 0x5dc540
void RF2014ArinStage2Wnd::RefreshMissions() {
	ShowAll(false);
	for (size_t i = 0; i < m_missions.size(); ++i) {
		if (m_missions[i].m_completed) {
			ShowMissionButton(static_cast<int>(i), false);
			if (m_missions[i].m_rewarded) {
				m_rewarded[i] = true;
				ShowRewardButton(static_cast<int>(i), true);
				SetRewardState(static_cast<int>(i), RFButton::RFBUTTON_STATE_DISABLE);
			} else {
				SetRewardState(static_cast<int>(i), RFButton::RFBUTTON_STATE_NORMAL);
				ShowRewardButton(static_cast<int>(i), true);
			}
		} else {
			ShowMissionButton(static_cast<int>(i), true);
		}
	}
	ShowFinalButton(PlayFinalIfAllRewarded());
	ShowFinalPictures(PlayFinalIfAllRewarded());
}

// 0x5dc600
void RF2014ArinStage2Wnd::MarkRewarded(int index) {
	m_rewarded[index] = true;
	SetRewardState(index, RFButton::RFBUTTON_STATE_DISABLE);
	ShowFinalButton(PlayFinalIfAllRewarded());
	ShowFinalPictures(PlayFinalIfAllRewarded());
	SetMagicBackground(CountRewarded());
}

// 0x5dc650
void RF2014ArinStage2Wnd::ApplyEventEnded() {
	if (m_stage == 0xb) {
		for (int i = 0; i < 5; ++i) {
			ShowMissionButton(i, false);
		}
		ShowFinalButton(false);
		ShowFinalPictures(false);
		RFPicture* background = dynamic_cast<RFPicture*>(GetChild(L"pic_event_magic_bg"));
		background->SetPicture(L"pic_event_magic_bg_05.png");
		StopAnimation();
	}
}

// 0x5dc850
void RF2014ArinBirthdayDlg::ApplyReward(int reward) {
	RFWnd* eventCtrl = GetChild(L"wnd_event_ctrl");
	RF2014ArinStage2Wnd* magic = dynamic_cast<RF2014ArinStage2Wnd*>(eventCtrl->GetChild(L"wnd_event_magic_ctrl"));
	if (magic != NULL) {
		magic->MarkRewarded(reward);
	}
}

// 0x5dc970
void RF2014ArinBirthdayDlg::ApplyFinalReward() {
	RFWnd* eventCtrl = GetChild(L"wnd_event_ctrl");
	RF2014ArinStage2Wnd* magic = dynamic_cast<RF2014ArinStage2Wnd*>(eventCtrl->GetChild(L"wnd_event_magic_ctrl"));
	if (magic != NULL) {
		magic->SetStage(0xb);
		magic->ApplyEventEnded();
	}
}

// 0x5dca90
void RF2014ArinStage1Wnd::UseTicket(int index) {
	if (HasTicket(index)) {
		ConsumeTicket(index);
		SetTicketCount(index, m_ticketCounts[index]);
		ShowTicketPicture(index, true);
		CheckAllTicketsUsed();
	}
}

// 0x5dcad0
void RF2014ArinStage1Wnd::UseHeldTickets() {
	for (int i = 0; i < 5; ++i) {
		uint32_t typeId = g_arinTicketItemTypeIds[i];
		RFButton* button = dynamic_cast<RFButton*>(GetChild(ShiftJisToWString(std::string(Jp_LookupText(MakeStr("%s_%02d", "btn_ticket", i + 1))))));
		// The original casts label_ticket_<i + 1> and drops the result.
		(void)dynamic_cast<RFLabel*>(GetChild(ShiftJisToWString(std::string(Jp_LookupText(MakeStr("%s_%02d", "label_ticket", i + 1))))));
		if (Jp_GameState_FindInventoryItem(typeId) != NULL) {
			UseTicket(i);
			if (m_ticketCounts[i] < 1) {
				button->EnableWindow(false);
			}
		}
	}
}

// 0x5dcd40
void RF2014ArinStage2Wnd::SetMissions(std::vector<ArinBirthday2014Mission> const* missions) {
	m_missions = *missions;
	RefreshMissions();
	SetMagicBackground(CountRewarded());
	ApplyEventEnded();
}

// 0x5dcdd0
void RF2014ArinBirthdayDlg::UpdateStage() {
	// The original casts pic_arinbirth_bg and drops the result.
	(void)dynamic_cast<RFPicture*>(GetChild(L"pic_arinbirth_bg"));
	RFWnd* eventCtrl = GetChild(L"wnd_event_ctrl");
	RFPicture* character = dynamic_cast<RFPicture*>(GetChild(L"pic_char"));
	switch (m_stage) {
	case 0:
		SetFirstBackground();
		ShowText(3);
		SetTikiPicture(2);
		break;
	case 1:
		ShowText(4);
		SetTikiPicture(3);
		break;
	case 2: {
		ShowText(5);
		SetTikiPicture(2);
		RF2014ArinStage1Wnd* tickets = CreateArinStage1Wnd(eventCtrl);
		tickets->CenterWindow(eventCtrl);
		tickets->ShowForm(true);
		ShowOkButton(false);
		break;
	}
	case 3:
		ShowText(6);
		SetTikiPicture(1);
		ShowOkButton(true);
		break;
	case 4:
		SetFirstBackground();
		ShowText(0);
		SetTikiPicture(0);
		break;
	case 5:
		ShowText(1);
		SetTikiPicture(1);
		break;
	case 6:
		ShowText(2);
		SetTikiPicture(0);
		break;
	case 7: {
		RFWnd* ticketCtrl = eventCtrl->GetChild(L"wnd_event_ticket_ctrl");
		if (ticketCtrl != NULL) {
			ticketCtrl->ShowWindow(false);
		}
		ShowText(7);
		SetArinPicture(0);
		ShowOkButton(true);
		break;
	}
	case 8:
		ShowText(8);
		SetArinPicture(1);
		break;
	case 9: {
		SetSecondBackground();
		RFWnd* ticketCtrl = eventCtrl->GetChild(L"wnd_event_ticket_ctrl");
		if (ticketCtrl != NULL) {
			ticketCtrl->ShowWindow(false);
		}
		ShowText(9);
		SetArinPicture(2);
		RF2014ArinStage2Wnd* magic = CreateArinStage2Wnd(eventCtrl);
		magic->SetStage(9);
		magic->SetMissions(&m_eventState->m_missions);
		magic->CenterWindow(eventCtrl);
		magic->ShowForm(true);
		character->SetPicture(L"pic_arin_1.png");
		ShowOkButton(false);
		break;
	}
	case 10:
		SetSecondBackground();
		ShowText(11);
		SetArinPicture(3);
		break;
	case 11: {
		SetSecondBackground();
		ShowText(12);
		SetArinPicture(0);
		eventCtrl->ShowWindow(true);
		RF2014ArinStage2Wnd* magic = CreateArinStage2Wnd(eventCtrl);
		magic->SetStage(0xb);
		magic->SetMissions(&m_eventState->m_missions);
		magic->CenterWindow(eventCtrl);
		magic->ShowForm(true);
		ShowOkButton(false);
		break;
	}
	}
}

// 0x5dd210
void RF2014ArinBirthdayDlg::OnOkClicked(EventArgs&) {
	switch (m_stage) {
	case 0:
		SetStage(1);
		break;
	case 1:
		SetStage(2);
		break;
	case 2:
		SetStage(3);
		break;
	case 3:
	case 6:
		SetStage(7);
		break;
	case 4:
		SendArinBirthday2014Request();
		break;
	case 5:
		SetStage(6);
		break;
	case 7:
		SetStage(8);
		break;
	case 8:
		SetStage(9);
		break;
	}
	UpdateStage();
}

// 0x5dd2e0
void RF2014ArinStage1Wnd::OnInitialUpdate() {
	RegisterTo(L"btn_ticket_01", RFWnd::EventLButtonDblClick, Subscriber(&RF2014ArinStage1Wnd::OnTicket1DblClick, this));
	RegisterTo(L"btn_ticket_02", RFWnd::EventLButtonDblClick, Subscriber(&RF2014ArinStage1Wnd::OnTicket2DblClick, this));
	RegisterTo(L"btn_ticket_03", RFWnd::EventLButtonDblClick, Subscriber(&RF2014ArinStage1Wnd::OnTicket3DblClick, this));
	RegisterTo(L"btn_ticket_04", RFWnd::EventLButtonDblClick, Subscriber(&RF2014ArinStage1Wnd::OnTicket4DblClick, this));
	RegisterTo(L"btn_ticket_05", RFWnd::EventLButtonDblClick, Subscriber(&RF2014ArinStage1Wnd::OnTicket5DblClick, this));
	ShowTicketBase(false);
	for (int i = 0; i < 5; ++i) {
		ShowTicketPicture(i, false);
		uint32_t typeId = g_arinTicketItemTypeIds[i];
		RFButton* button = dynamic_cast<RFButton*>(GetChild(ShiftJisToWString(std::string(Jp_LookupText(MakeStr("%s_%02d", "btn_ticket", i + 1))))));
		// The original casts label_ticket_<i + 1> and drops the result.
		(void)dynamic_cast<RFLabel*>(GetChild(ShiftJisToWString(std::string(Jp_LookupText(MakeStr("%s_%02d", "label_ticket", i + 1))))));
		const sItemInfo* item = Jp_GameState_FindInventoryItem(typeId);
		if (item != NULL) {
			button->EnableWindow(true);
			m_ticketCounts[i] = item->m_count;
			SetTicketCount(i, item->m_count);
		} else {
			button->EnableWindow(false);
			SetTicketCount(i, 0);
		}
	}
	UseHeldTickets();
}

// 0x5ddb40
RF2014ArinBirthdayDlg::RF2014ArinBirthdayDlg() {
	m_stage = 0;
	m_eventState = NULL;
	g_arinBirthday2014Dlg = this;
}

// 0x5ddbe0
RF2014ArinBirthdayDlg::~RF2014ArinBirthdayDlg() {
	g_arinBirthday2014Dlg = NULL;
}

// 0x5ddc70
RF2014ArinMissionInfoDlg::~RF2014ArinMissionInfoDlg() {
	g_arinMissionInfoDlg = NULL;
}

// 0x5ddd10
RF2014ArinMapMissionInfoDlg::~RF2014ArinMapMissionInfoDlg() {
	g_arinMapMissionInfoDlg = NULL;
}

// 0x5ddf50
RFWnd* CreateArinBirthday2014Dlg(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RF2014ArinBirthdayDlg>(&RFManager::GetInstance(), L"event_arinbirth_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RF2014ArinBirthdayDlg>(&RFManager::GetInstance(), L"event_arinbirth_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RF2014ArinBirthdayDlg>(&RFManager::GetInstance(), L"event_arinbirth_dialog.xml");
		break;
	}
	RF2014ArinBirthdayDlg::s_loadParent = parent;
	return wnd;
}

// 0x5de0c0
RF2014ArinMissionInfoDlg::RF2014ArinMissionInfoDlg() {
	g_arinMissionInfoDlg = this;
	ArinBirthday2014MissionText text;
	text.m_title = Jp_GetTableText(0x30e15);
	text.m_body = Jp_GetTableText(0x30e16);
	m_texts.push_back(text);
	text.m_title = Jp_GetTableText(0x30e17);
	text.m_body = Jp_GetTableText(0x30e18);
	m_texts.push_back(text);
	text.m_title = Jp_GetTableText(0x30e19);
	text.m_body = Jp_GetTableText(0x30e1a);
	m_texts.push_back(text);
	text.m_title = Jp_GetTableText(0x30e1b);
	text.m_body = Jp_GetTableText(0x30e1c);
	m_texts.push_back(text);
}

// 0x5de330
RF2014ArinMapMissionInfoDlg::RF2014ArinMapMissionInfoDlg() {
	g_arinMapMissionInfoDlg = this;
	ArinBirthday2014MissionText text;
	text.m_title = Jp_GetTableText(0x30e1d);
	text.m_body = "";
	m_texts.push_back(text);
}

// 0x5de6b0
void RF2014ArinBirthdayDlg::LoadTexts() {
	m_texts.clear();
	m_texts.push_back(std::string(Jp_GetTableText(0x30e08)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e09)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e0a)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e0b)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e0c)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e0d)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e0e)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e0f)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e10)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e11)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e12)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e13)));
	m_texts.push_back(std::string(Jp_GetTableText(0x30e14)));
}

// 0x5debf0
RF2014ArinMissionInfoDlg* CreateArinMissionInfoDlg(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RF2014ArinMissionInfoDlg>(&RFManager::GetInstance(), L"event_arinbirth_popup_2_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RF2014ArinMissionInfoDlg>(&RFManager::GetInstance(), L"event_arinbirth_popup_2_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RF2014ArinMissionInfoDlg>(&RFManager::GetInstance(), L"event_arinbirth_popup_2_dialog.xml");
		break;
	}
	RF2014ArinMissionInfoDlg::s_loadParent = parent;
	return static_cast<RF2014ArinMissionInfoDlg*>(wnd);
}

// 0x5ded30
RF2014ArinMapMissionInfoDlg* CreateArinMapMissionInfoDlg(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RF2014ArinMapMissionInfoDlg>(&RFManager::GetInstance(), L"event_arinbirth_popup_3_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RF2014ArinMapMissionInfoDlg>(&RFManager::GetInstance(), L"event_arinbirth_popup_3_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RF2014ArinMapMissionInfoDlg>(&RFManager::GetInstance(), L"event_arinbirth_popup_3_dialog.xml");
		break;
	}
	RF2014ArinMapMissionInfoDlg::s_loadParent = parent;
	return static_cast<RF2014ArinMapMissionInfoDlg*>(wnd);
}

// 0x5dee70
void RF2014ArinBirthdayDlg::OnInitialUpdate() {
	RegisterTo(L"btn_event_page_out_n", RFButton::EventClicked, Subscriber(&RF2014ArinBirthdayDlg::OnCloseClicked, this));
	RegisterTo(L"btn_ok", RFButton::EventClicked, Subscriber(&RF2014ArinBirthdayDlg::OnOkClicked, this));
	RegisterTo(L"", RFWnd::EventKeyDown, Subscriber(&RF2014ArinBirthdayDlg::OnKeyDownForm, this));
	LoadTexts();
}

// 0x5df110
void RF2014ArinStage2Wnd::ShowMissionInfo(int index) {
	if (m_missions.size() < 5) {
		return;
	}
	if (m_missions[index].m_completed == true || m_missions[index].m_rewarded == true) {
		return;
	}
	RF2014ArinMissionInfoDlg* info = CreateArinMissionInfoDlg(NULL);
	if (info != NULL) {
		info->SetMission(index, &m_missions[index]);
		info->CenterWindow(NULL);
		info->ShowModal();
	}
}

// 0x5df180
void RF2014ArinStage2Wnd::ShowMapMissionInfo(int index) {
	if (m_missions.size() < 5) {
		return;
	}
	RF2014ArinMapMissionInfoDlg* info = CreateArinMapMissionInfoDlg(NULL);
	if (info != NULL) {
		info->SetMission(&m_missions[index]);
		info->CenterWindow(NULL);
		info->ShowModal();
	}
}

// 0x5df1e0
void RF2014ArinStage2Wnd::OnMission1Clicked(EventArgs&) {
	ShowMissionInfo(0);
}

// 0x5df1f0
void RF2014ArinStage2Wnd::OnMission2Clicked(EventArgs&) {
	ShowMissionInfo(1);
}

// 0x5df200
void RF2014ArinStage2Wnd::OnMission3Clicked(EventArgs&) {
	ShowMissionInfo(2);
}

// 0x5df210
void RF2014ArinStage2Wnd::OnMission4Clicked(EventArgs&) {
	ShowMissionInfo(3);
}

// 0x5df220
void RF2014ArinStage2Wnd::OnMission5Clicked(EventArgs&) {
	ShowMapMissionInfo(4);
}

// 0x5df230
void RF2014ArinStage2Wnd::OnInitialUpdate() {
	RegisterTo(L"btn_mission_01", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnMission1Clicked, this));
	RegisterTo(L"btn_mission_02", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnMission2Clicked, this));
	RegisterTo(L"btn_mission_03", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnMission3Clicked, this));
	RegisterTo(L"btn_mission_04", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnMission4Clicked, this));
	RegisterTo(L"btn_mission_05", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnMission5Clicked, this));
	RegisterTo(L"btn_reward_01", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnReward1Clicked, this));
	RegisterTo(L"btn_reward_02", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnReward2Clicked, this));
	RegisterTo(L"btn_reward_03", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnReward3Clicked, this));
	RegisterTo(L"btn_reward_04", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnReward4Clicked, this));
	RegisterTo(L"btn_reward_05", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnReward5Clicked, this));
	RegisterTo(L"btn_final", RFButton::EventClicked, Subscriber(&RF2014ArinStage2Wnd::OnFinalClicked, this));
}

} // namespace _gateway
