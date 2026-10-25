// The JP 983 character mastery's stat detail window and its up/down and
// notice dialogs (JpMasteryStatDetail.h), from the JP exe's bodies
// (node scripts/disasm.ts --jp 0x<va>), in address order. The creators use
// RFManager.h's CreateWnd<T> / CreateDesktopWnd<T> (instances 0x5c6970 /
// 0x5c6a00 for RFMasteryStatNoticeDlg, 0x5c6a90 / 0x5c6b20 for
// RFMasteryStatUpDnDlg, 0x5c7f80 / 0x5c8010 for
// RFCharMasteryStatDetailWnd); their handlers are bound through
// MemberFunctionFunctor<T>.
// lib: 0x5c6490, 0x5c6600 (this-adjusting thunks of the RFModalRenderer
// sub-objects' destructors), 0x5c71b0, 0x5c71e0, 0x5c80a0 (scalar deleting
// destructors), 0x5c6970, 0x5c6a00, 0x5c6a90, 0x5c6b20, 0x5c7f80, 0x5c8010
// (CreateWnd<T> / CreateDesktopWnd<T>).
#include "JpMasteryStatDetail.h"

#include <string>

#include "JpLobbyDialogs.h"
#include "JpCommonDialogs.h"
#include "JpItemGlue.h"
#include "JpLobbyGlue.h"
#include "JpMasteryMainDlg.h"
#include "JpMasterySlot.h"
#include "JpTooltipWnd.h"
#include "RFButton.h"
#include "RFEventArgs.h"
#include "RFLabel.h"
#include "RFManager.h"
#include "Packet.h"
#include "RFPicture.h"
#include "RFTextArea.h"
#include "RFTextManager.h"
#include "TaskManager.h"
#include "RFSize.h"
#include "RFVector2.h"
#include "RefreshStringUtil.h"
#include "SoundManager.h"
#include "commonutil.h"

// 0xe78134
RFWnd* RFMasteryStatNoticeDlg::s_loadParent = NULL;
// 0xe78138
RFMasteryStatNoticeDlg* RFMasteryStatNoticeDlg::s_instance = NULL;
// 0xe7813c
RFWnd* RFMasteryStatUpDnDlg::s_loadParent = NULL;
// 0xe78140
RFMasteryStatUpDnDlg* RFMasteryStatUpDnDlg::s_instance = NULL;
// 0xe78144
RFWnd* RFCharMasteryStatDetailWnd::s_loadParent = NULL;

// The UI sounds the mastery handlers play through the game's sound manager
// (0xf023f4, slot 3; constants 0xd66d08 0.5f and 0xd00d90 200.0f).
static void PlayMasteryStatSound(const char* name) {
	g_audio->PlaySfx(name, NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x5c5b90
void RFMasteryStatNoticeDlg::OnOkClicked(EventArgs& args) {
	PlayMasteryStatSound("ui_button_ok_click");
	Close(static_cast<RFWNDCLOSESTATE>(1));
}

// 0x5c5bd0
void RFMasteryStatUpDnDlg::OnCancelClicked(EventArgs& args) {
	PlayMasteryStatSound("ui_button_cancel_click");
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5c5c10
void RFMasteryStatUpDnDlg::UpdateNumbers() {
	if (m_direction == 0) {
		return;
	}
	JpMasteryStatInfo info;
	int32_t total = info.GetStatValue(m_statType) + (m_direction != -1 ? 1 : -1);
	int32_t before = info.GetFreeMasteryPoints(m_statType);
	int32_t after;
	if (m_direction == 1) {
		after = before - 1;
	} else if (info.GetExcessMasteryPoints(m_statType) != 0) {
		after = 0;
	} else {
		after = before + 1;
	}
	if (m_totalStatLabel != NULL) {
		m_totalStatLabel->SetLabel(total, false);
	}
	if (m_beforeSlotLabel != NULL) {
		m_beforeSlotLabel->SetLabel(before, false);
	}
	if (m_afterSlotLabel != NULL) {
		m_afterSlotLabel->SetLabel(after, false);
	}
}

// 0x5c6340 (one copy, shared by both dialogs)
void RFMasteryStatNoticeDlg::OnKey(EventArgs& args) {
	OnDialogKey(static_cast<KeyEventArgs&>(args));
}

// 0x5c6340 (the up/down dialog's use of the shared copy)
void RFMasteryStatUpDnDlg::OnKey(EventArgs& args) {
	OnDialogKey(static_cast<KeyEventArgs&>(args));
}

// 0x5c6400
RFMasteryStatNoticeDlg::RFMasteryStatNoticeDlg()
	: m_statImg1(NULL), m_statImg2(NULL), m_arrowImg(NULL), m_statLabel(NULL), m_message(NULL) {
	s_instance = this;
}

// 0x5c64a0
RFMasteryStatNoticeDlg::~RFMasteryStatNoticeDlg() {
	s_instance = NULL;
}

// 0x5c6510
void RFMasteryStatNoticeDlg::OnDialogKey(KeyEventArgs& args) {
	if (args.keyCode == 0xd || args.keyCode == 0x1b) {
		OnOkClicked(args);
		args.handled = true;
	}
}

// 0x5c6540
RFMasteryStatUpDnDlg::RFMasteryStatUpDnDlg()
	: m_arrowImg(NULL), m_titleImg(NULL), m_statBg1Img(NULL), m_statBg2Img(NULL),
	  m_totalStatLabel(NULL), m_beforeSlotLabel(NULL), m_afterSlotLabel(NULL),
	  m_message(NULL), m_warningText(NULL), m_direction(0), m_statType(5), m_requestSent(false) {
	s_instance = this;
}

// 0x5c6610
RFMasteryStatUpDnDlg::~RFMasteryStatUpDnDlg() {
	s_instance = NULL;
}

// 0x5c7a70
RFCharMasteryStatDetailWnd::RFCharMasteryStatDetailWnd()
	: m_statType(5), m_statUpButton(NULL), m_statDownButton(NULL), m_tooltip(NULL), m_detailOpen(0),
	  m_caddie(NULL), m_mascot(NULL), m_clubSet(NULL), m_club(NULL) {
	for (int32_t i = 0; i < 6; ++i) {
		m_statLabels[i] = NULL;
	}
}

// 0x5c7b10
RFCharMasteryStatDetailWnd::~RFCharMasteryStatDetailWnd() {
}

// 0x5c8400 (parent in ecx)
RFWnd* CreateMasteryStatNoticeDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RFMasteryStatNoticeDlg>(&RFManager::GetInstance(), L"mastery_stat_notice_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFMasteryStatNoticeDlg>(&RFManager::GetInstance(), L"mastery_stat_notice_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFMasteryStatNoticeDlg>(&RFManager::GetInstance(), L"mastery_stat_notice_dialog.xml");
		break;
	}
	RFMasteryStatNoticeDlg::s_loadParent = parent;
	return wnd;
}

// 0x5c8870
void RFMasteryStatNoticeDlg::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventKeyDown, Subscriber(&RFMasteryStatNoticeDlg::OnKey, this));
	RegisterTo(L"ok_btn", RFButton::EventClicked, Subscriber(&RFMasteryStatNoticeDlg::OnOkClicked, this));
	m_statImg1 = dynamic_cast<RFPicture*>(GetChild(L"stat_img_01"));
	m_statImg2 = dynamic_cast<RFPicture*>(GetChild(L"stat_img_02"));
	m_arrowImg = dynamic_cast<RFPicture*>(GetChild(L"arrow_img"));
	m_statLabel = dynamic_cast<RFLabel*>(GetChild(L"stat_label"));
	m_message = dynamic_cast<RFTextArea*>(GetChild(L"message"));
	PlayMasteryStatSound("ui_dialog_open");
}

// 0x5c8c50 (parent in ecx)
RFWnd* CreateMasteryStatUpDnDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RFMasteryStatUpDnDlg>(&RFManager::GetInstance(), L"kam_status_updown_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFMasteryStatUpDnDlg>(&RFManager::GetInstance(), L"kam_status_updown_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFMasteryStatUpDnDlg>(&RFManager::GetInstance(), L"kam_status_updown_dialog.xml");
		break;
	}
	RFMasteryStatUpDnDlg::s_loadParent = parent;
	return wnd;
}

// 0x5c8d90
void RFMasteryStatUpDnDlg::UpdateImages() {
	if (m_direction == 0) {
		return;
	}
	if (m_arrowImg != NULL) {
		m_arrowImg->SetPicture(m_direction != -1 ? L"mastery_stat_arrow_up.tga" : L"mastery_stat_arrow_down.tga");
	}
	if (m_titleImg != NULL) {
		m_titleImg->SetPicture(m_direction != -1 ? L"title_status_up.tga" : L"title_status_down.tga");
	}
	if (m_statBg1Img != NULL) {
		std::string name(MakeStr("button_mastery_stat_%d_n.tga", m_statType));
		m_statBg1Img->SetPicture(ShiftJisToWString(name));
	}
	if (m_statBg2Img != NULL) {
		std::string name(MakeStr("button_mastery_stat_%d_cap.tga", m_statType));
		m_statBg2Img->SetPicture(ShiftJisToWString(name));
	}
}

// 0x5c9020 (parent in ecx)
RFWnd* CreateMasteryStatDetailWnd(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharMasteryStatDetailWnd>(&RFManager::GetInstance(), L"kam_status_detail_wnd.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharMasteryStatDetailWnd>(&RFManager::GetInstance(), L"kam_status_detail_wnd.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharMasteryStatDetailWnd>(&RFManager::GetInstance(), L"kam_status_detail_wnd.xml");
		break;
	}
	RFCharMasteryStatDetailWnd::s_loadParent = parent;
	return wnd;
}

// 0x5c9160
void RFCharMasteryStatDetailWnd::SetStat(int32_t index) {
	RFPicture* cap = dynamic_cast<RFPicture*>(GetChild(L"cap_img"));
	if (cap != NULL) {
		std::string name(MakeStr("button_mastery_stat_%d_cap.tga", index));
		cap->SetPicture(ShiftJisToWString(name));
	}
}

// 0x5c92a0
void RFCharMasteryStatDetailWnd::OnCapClicked(EventArgs& args) {
	if (m_detailOpen == 0) {
		m_detailOpen = 1;
		PlayAnimation(L"_push_status_detail_");
		PlayMasteryStatSound("mastery_stat_open");
	} else {
		m_detailOpen = 0;
		PlayAnimation(L"_close_status_detail_");
		PlayMasteryStatSound("mastery_stat_closed");
	}
}

// 0x5c9400
void RFCharMasteryStatDetailWnd::ShowTooltip(bool show, char const* text, int32_t labelIndex, uint32_t color) {
	if (m_tooltip == NULL) {
		return;
	}
	if (!show) {
		m_tooltip->Show(false);
		return;
	}
	std::wstring wideText = ShiftJisToWString(std::string(text));
	m_tooltip->SetTextColor(color);
	m_tooltip->SetText(wideText);
	RFLabel* label = m_statLabels[labelIndex];
	double width = label->GetSize().width;
	RFVector2 pos = label->GetScreenPos();
	float x = static_cast<float>(pos.x + width);
	m_tooltip->SetPosition(x, label->GetScreenPos().y);
	m_tooltip->Show(show);
}

// 0x5c9bc0: an up costs the pang of the stat's next mastery step (a down
// is free); short of it, the notice "not enough pang for the upgrade"
// (Jp_LookupText of the Korean key); else the main actor gets message 1,
// and packet 0x188 (up) / 0x189 (down) sends the stat and the selected
// character's record.
void RFMasteryStatUpDnDlg::OnConfirmClicked(EventArgs& args) {
	if (m_requestSent) {
		return;
	}
	PlayMasteryStatSound("ui_button_ok_click");
	if (m_direction == 0) {
		return;
	}
	JpMasteryStatInfo info;
	const JpIffEnchant* step = JpIff_FindEnchant(
		static_cast<uint32_t>(info.GetMasteryPoints(m_statType, true, NULL)) | ((static_cast<uint32_t>(m_statType) | 0x340) << 20));
	uint32_t cost = m_direction == -1 ? 0 : JpIffEnchant_GetValue(step);
	int64_t pang = Jp_GameState_Pang();
	if (m_direction == 1 && static_cast<int64_t>(cost) > pang) {
		RFNoticeDlg* dlg = static_cast<RFNoticeDlg*>(CreateNoticeDialog(this));
		if (dlg != NULL) {
			// "\xbe\xf7\xb1\xd7..." (EUC-KR): "not enough pang for the upgrade."
			std::string text(Jp_LookupText("\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xc0\xa7\xc7\xd1 "
				"\xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9."));
			dlg->SetMessage(text.c_str());
			dlg->ShowModal();
		}
		return;
	}
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	m_requestSent = true;
	const sCharacterInfo* character = Jp_GameState_EquippedCharacterData();
	WSendPacket packet(0x188 + (m_direction == -1 ? 1 : 0));
	packet.Encode4(static_cast<unsigned long>(m_statType));
	JpCharacter_EncodeRecord(packet, character);
	packet.Send(SEND_TO_DEFAULT);
}

// 0x5c9df0: the message: down, table text 0x18834 with the stat name; up
// (with a selected character and a mastery step record), table text
// 0x18833 with the stat name and the step's pang cost. The warning: for
// a power (stat 0) up, table text 0x18835 with GetPenaltyValue() + 1.
void RFMasteryStatUpDnDlg::UpdateMessage() {
	if (m_direction == 0) {
		return;
	}
	JpMasteryStatInfo info;
	if (m_message != NULL) {
		m_message->ClearText();
		std::string text;
		if (m_direction == -1) {
			text = MakeStr(Jp_GetTableText(0x18834), info.GetStatName(m_statType));
		} else if (Jp_GameState_EquippedCharacterData() != NULL) {
			const JpIffEnchant* step = JpIff_FindEnchant(
				static_cast<uint32_t>(info.GetMasteryPoints(m_statType, true, NULL)) | ((static_cast<uint32_t>(m_statType) | 0x340) << 20));
			if (step != NULL) {
				text += MakeStr(Jp_GetTableText(0x18833), info.GetStatName(m_statType), JpIffEnchant_GetValue(step));
			}
		}
		m_message->SetText(ShiftJisToWString(text));
	}
	if (m_warningText != NULL) {
		m_warningText->ClearText();
		if (m_statType == 0 && m_direction == 1) {
			std::string text(MakeStr(Jp_GetTableText(0x18835), info.GetPenaltyValue() + 1));
			m_warningText->SetText(ShiftJisToWString(text));
		}
	}
}

// 0x5ca030
void RFCharMasteryStatDetailWnd::OnEgg1MouseEnter(EventArgs& args) {
	std::string text(Jp_GetTableText(0x18836));
	ShowTooltip(true, text.c_str(), 0, 0xff000000);
}

// 0x5ca0f0: the lines are joined by '\n' (count: the lines so far); the
// part and aux part lines come from std::for_each functors (0x5c81d0 /
// 0x5c8280, which take this, &count and &text).
void RFCharMasteryStatDetailWnd::OnEgg2MouseEnter(EventArgs& args) {
	int32_t count = 0;
	std::string text;
	for (std::vector<const JpIffPart*>::const_iterator it = m_parts.begin(); it != m_parts.end(); ++it) {
		if (JpIffPart_GetBaseStat(*it, m_statType) > 0) {
			if (count != 0) {
				text.append(1, '\n');
			}
			text += MakeStr("%s : %d", JpIffPart_GetName(*it), JpIffPart_GetBaseStat(*it, m_statType));
			++count;
		}
	}
	for (std::vector<const JpIffAuxPart*>::const_iterator it = m_auxParts.begin(); it != m_auxParts.end(); ++it) {
		if (JpIffAuxPart_GetLevel(*it, m_statType) > 0) {
			if (count != 0) {
				text.append(1, '\n');
			}
			text += MakeStr("%s : %d", JpIffAuxPart_GetName(*it), JpIffAuxPart_GetLevel(*it, m_statType));
			++count;
		}
	}
	if (m_caddie != NULL && JpIffCaddie_GetStat(m_caddie, m_statType) > 0) {
		if (count != 0) {
			text.append(1, '\n');
		}
		text += MakeStr("%s : %d", JpIffCaddie_GetName(m_caddie), JpIffCaddie_GetStat(m_caddie, m_statType));
		count += 1;
	}
	if (m_mascot != NULL && JpIffMascot_GetStat(m_mascot, m_statType) > 0) {
		if (count != 0) {
			text.append(1, '\n');
		}
		text += MakeStr("%s : %d", JpIffMascot_GetName(m_mascot), JpIffMascot_GetStat(m_mascot, m_statType));
		count += 1;
	}
	if (m_clubSet != NULL && JpIffClubSet_GetBaseStat(m_clubSet, m_statType) > 0) {
		if (count != 0) {
			text.append(1, '\n');
		}
		uint8_t value = Jp_GetClubSetStat(m_statType, m_clubSet, JpItemData_GetClubUpgrades(m_club), JpItemData_GetClubExtra(m_club));
		text += MakeStr("%s : %d", JpIffClubSet_GetName(m_clubSet), value);
		count += 1;
	}
	int32_t bonus = JpCardManager_GetStatBonus(m_statType);
	if (bonus > 0) {
		if (count != 0) {
			text.append(1, '\n');
		}
		text += MakeStr(Jp_GetTableText(0x18837), bonus);
		count += 1;
	}
	int32_t effect = JpCardManager_GetEffectStatBonus(m_statType);
	if (effect > 0) {
		if (count != 0) {
			text.append(1, '\n');
		}
		text += MakeStr(Jp_GetTableText(0x18838), effect);
		count += 1;
	}
	if (text.empty()) {
		return;
	}
	ShowTooltip(true, text.c_str(), 1, 0xff000000);
}

// 0x5ca410
void RFCharMasteryStatDetailWnd::OnEgg3MouseEnter(EventArgs& args) {
	std::string text;
	JpMasteryStatInfo info;
	int32_t points = info.GetMasteryPoints(m_statType, true, NULL);
	int32_t maxPoints = info.GetMaxMasteryPoints(m_statType);
	uint32_t color = points <= maxPoints ? 0xff000000 : 0xffff0000;
	text += MakeStr("[%s : %d/%d]", info.GetStatName(m_statType), points, maxPoints);
	int32_t slots = info.GetSlotPoints(m_statType, NULL) + info.GetOpenSlotCount(m_statType, -1);
	text += slots > 0 ? MakeStr(Jp_GetTableText(0x18839), slots) : "";
	int32_t parts = info.GetPartsStat(m_statType);
	text += parts > 0 ? MakeStr(Jp_GetTableText(0x1883a), parts) : "";
	int32_t cards = info.GetCardStat(m_statType);
	text += cards > 0 ? MakeStr(Jp_GetTableText(0x1883b), cards) : "";
	if (text.empty()) {
		return;
	}
	ShowTooltip(true, text.c_str(), 2, color);
}

// 0x5ca640
void RFCharMasteryStatDetailWnd::OnEgg4MouseEnter(EventArgs& args) {
	std::string text;
	JpMasteryStatInfo info;
	int32_t penalty = info.GetPowerPenalty(m_statType);
	if (penalty <= 0) {
		return;
	}
	text += MakeStr(Jp_GetTableText(0x1883c), info.GetStatValue(0), penalty);
	if (text.empty()) {
		return;
	}
	ShowTooltip(true, text.c_str(), 3, 0xff000000);
}

// 0x5ca770
void RFCharMasteryStatDetailWnd::OnEggMouseLeave(EventArgs& args) {
	ShowTooltip(false, NULL, 0, 0xff000000);
}

// 0x5ca790
void RFCharMasteryStatDetailWnd::OnCapMouseEnter(EventArgs& args) {
	if (m_detailOpen == 0) {
		PlayAnimation(L"_over_status_detail_");
	}
	std::string text;
	JpMasteryStatInfo info;
	int32_t excess = info.GetExcessMasteryPoints(m_statType);
	if (excess > 0) {
		text += MakeStr(Jp_GetTableText(0x1883d), excess);
		ShowTooltip(true, text.c_str(), 4, 0xff000000);
	}
}

// 0x5ca8e0
void RFCharMasteryStatDetailWnd::OnCapMouseLeave(EventArgs& args) {
	PlayAnimation(L"_out_status_detail_");
	ShowTooltip(false, NULL, 0, 0xff000000);
}

// 0x5ca9b0
void RFCharMasteryStatDetailWnd::UpdateEntries() {
	const sCharacterInfo* character = Jp_GameState_EquippedCharacterData();
	if (character == NULL) {
		return;
	}
	m_parts.clear();
	const uint32_t* partTids = JpCharacter_GetPartTypeIds(character);
	for (int32_t i = 0; i < 24; ++i) {
		if (partTids[i] != 0) {
			const JpIffPart* part = reinterpret_cast<const JpIffPart*>(JpItemManager_FindPart(partTids[i]));
			if (part != NULL && JpIffPart_GetBaseStat(part, m_statType) > 0) {
				m_parts.push_back(part);
			}
		}
	}
	m_auxParts.clear();
	const uint32_t* auxPartTids = JpCharacter_GetAuxPartTypeIds(character);
	for (int32_t i = 0; i < 5; ++i) {
		if (auxPartTids[i] != 0) {
			const JpIffAuxPart* auxPart = JpIff_FindAuxPart(auxPartTids[i]);
			if (auxPart != NULL && JpIffAuxPart_GetLevel(auxPart, m_statType) > 0) {
				m_auxParts.push_back(auxPart);
			}
		}
	}
	const JpCaddieData* caddie = Jp_GameState_EquippedCaddie();
	m_caddie = caddie != NULL ? JpIff_FindCaddie(JpCaddieData_GetTypeId(caddie)) : NULL;
	const JpMascotData* mascot = Jp_GameState_EquippedMascot();
	m_mascot = mascot != NULL ? JpIff_FindMascot(JpMascotData_GetTypeId(mascot)) : NULL;
	const JpItemData* club = Jp_GameState_EquippedClub();
	m_clubSet = club != NULL ? reinterpret_cast<const JpIffClubSet*>(JpItemManager_FindClubSet(JpItemData_GetTypeId(club))) : NULL;
	m_club = club;
}

// 0x5cb260
void RFMasteryStatUpDnDlg::SetUp(uint32_t statType, int32_t direction) {
	JpMasteryStatInfo info;
	if (statType <= 4) {
		m_statType = statType;
		m_direction = direction;
		UpdateImages();
		UpdateNumbers();
		UpdateMessage();
	}
}

// 0x5cb2f0
void RFMasteryStatUpDnDlg::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventKeyDown, Subscriber(&RFMasteryStatUpDnDlg::OnKey, this));
	RegisterTo(L"button_confirm", RFButton::EventClicked, Subscriber(&RFMasteryStatUpDnDlg::OnConfirmClicked, this));
	RegisterTo(L"button_cancel", RFButton::EventClicked, Subscriber(&RFMasteryStatUpDnDlg::OnCancelClicked, this));
	m_arrowImg = dynamic_cast<RFPicture*>(GetChild(L"arrow_img"));
	m_titleImg = dynamic_cast<RFPicture*>(GetChild(L"title_img"));
	m_statBg1Img = dynamic_cast<RFPicture*>(GetChild(L"stat_bg1_img"));
	m_statBg2Img = dynamic_cast<RFPicture*>(GetChild(L"stat_bg2_img"));
	m_totalStatLabel = dynamic_cast<RFLabel*>(GetChild(L"total_stat_num"));
	m_beforeSlotLabel = dynamic_cast<RFLabel*>(GetChild(L"before_slot_num"));
	m_afterSlotLabel = dynamic_cast<RFLabel*>(GetChild(L"after_slot_num"));
	m_message = dynamic_cast<RFTextArea*>(GetChild(L"message"));
	m_warningText = dynamic_cast<RFTextArea*>(GetChild(L"warnning_text"));
	PlayMasteryStatSound("ui_dialog_open");
}

// 0x5cb910
void RFMasteryStatUpDnDlg::OnDialogKey(KeyEventArgs& args) {
	if (args.keyCode == 0xd) {
		OnConfirmClicked(args);
		args.handled = true;
	} else if (args.keyCode == 0x1b) {
		Close(static_cast<RFWNDCLOSESTATE>(2));
		args.handled = true;
	}
}

// 0x5cb950
void RFCharMasteryStatDetailWnd::UpdateValues(int32_t statType) {
	m_statType = statType;
	JpMasteryStatInfo info;
	if (static_cast<uint32_t>(m_statType) > 4) {
		return;
	}
	int32_t values[5] = { 0 };
	values[0] = info.GetCharacterBaseStat(statType);
	values[1] = info.GetEquipmentStat(statType);
	values[2] = info.GetMasteryPoints(statType, false, NULL);
	values[3] = info.GetPowerPenalty(statType);
	values[4] = info.GetStatValue(statType);
	int32_t freePoints = info.GetFreeMasteryPoints(m_statType);
	for (int32_t i = 0; i < 5; ++i) {
		if (m_statLabels[i] == NULL) {
			continue;
		}
		std::string text(MakeStr("%d", values[i]));
		if (i > 0) {
			if (i <= 2) {
				text = MakeStr("+%d", values[i]);
			} else if (i == 3) {
				text = MakeStr("-%d", values[3]);
			}
		}
		m_statLabels[i]->SetLabel(ShiftJisToWString(std::string(text.c_str())));
	}
	m_statLabels[4]->SetColor(info.GetExcessMasteryPoints(m_statType) != 0 ? 0xff000000 : 0xffffffff);
	if (m_statLabels[5] != NULL) {
		m_statLabels[5]->SetLabel(freePoints, false);
	}
	if (m_statUpButton != NULL) {
		m_statUpButton->EnableWindow(freePoints > 0);
	}
	if (m_statDownButton != NULL) {
		m_statDownButton->EnableWindow(values[2] > 0);
	}
	UpdateEntries();
}

// 0x5cbc30
void RFCharMasteryStatDetailWnd::OnStatUpClicked(EventArgs& args) {
	if (RFMasteryStatUpDnDlg::s_instance != NULL) {
		return;
	}
	RFMasteryStatUpDnDlg* dlg = static_cast<RFMasteryStatUpDnDlg*>(CreateMasteryStatUpDnDialog(this));
	if (dlg != NULL) {
		dlg->SetUp(m_statType, 1);
		dlg->ShowModal();
	}
}

// 0x5cbc70
void RFCharMasteryStatDetailWnd::OnStatDownClicked(EventArgs& args) {
	if (RFMasteryStatUpDnDlg::s_instance != NULL) {
		return;
	}
	RFMasteryStatUpDnDlg* dlg = static_cast<RFMasteryStatUpDnDlg*>(CreateMasteryStatUpDnDialog(this));
	if (dlg != NULL) {
		dlg->SetUp(m_statType, -1);
		dlg->ShowModal();
	}
}

// 0x5cbcd0
void RFCharMasteryStatDetailWnd::OnInitialUpdate() {
	RegisterTo(L"cap_btn", RFButton::EventClicked, Subscriber(&RFCharMasteryStatDetailWnd::OnCapClicked, this));
	RegisterTo(L"button_stat_up", RFButton::EventClicked, Subscriber(&RFCharMasteryStatDetailWnd::OnStatUpClicked, this));
	RegisterTo(L"button_stat_dn", RFButton::EventClicked, Subscriber(&RFCharMasteryStatDetailWnd::OnStatDownClicked, this));
	RegisterTo(L"egg1", RFWnd::EventMouseEnter, Subscriber(&RFCharMasteryStatDetailWnd::OnEgg1MouseEnter, this));
	RegisterTo(L"egg1", RFWnd::EventMouseLeave, Subscriber(&RFCharMasteryStatDetailWnd::OnEggMouseLeave, this));
	RegisterTo(L"egg2", RFWnd::EventMouseEnter, Subscriber(&RFCharMasteryStatDetailWnd::OnEgg2MouseEnter, this));
	RegisterTo(L"egg2", RFWnd::EventMouseLeave, Subscriber(&RFCharMasteryStatDetailWnd::OnEggMouseLeave, this));
	RegisterTo(L"egg3", RFWnd::EventMouseEnter, Subscriber(&RFCharMasteryStatDetailWnd::OnEgg3MouseEnter, this));
	RegisterTo(L"egg3", RFWnd::EventMouseLeave, Subscriber(&RFCharMasteryStatDetailWnd::OnEggMouseLeave, this));
	RegisterTo(L"egg4", RFWnd::EventMouseEnter, Subscriber(&RFCharMasteryStatDetailWnd::OnEgg4MouseEnter, this));
	RegisterTo(L"egg4", RFWnd::EventMouseLeave, Subscriber(&RFCharMasteryStatDetailWnd::OnEggMouseLeave, this));
	RegisterTo(L"cap_btn", RFWnd::EventMouseEnter, Subscriber(&RFCharMasteryStatDetailWnd::OnCapMouseEnter, this));
	RegisterTo(L"cap_btn", RFWnd::EventMouseLeave, Subscriber(&RFCharMasteryStatDetailWnd::OnCapMouseLeave, this));
	m_statLabels[0] = dynamic_cast<RFLabel*>(GetChild(L"char_stat_num_label"));
	m_statLabels[1] = dynamic_cast<RFLabel*>(GetChild(L"parts_stat_num_label"));
	m_statLabels[2] = dynamic_cast<RFLabel*>(GetChild(L"slot_stat_num_label"));
	m_statLabels[3] = dynamic_cast<RFLabel*>(GetChild(L"penalty_stat_num_label"));
	m_statLabels[4] = dynamic_cast<RFLabel*>(GetChild(L"total_stat_num_label"));
	m_statLabels[5] = dynamic_cast<RFLabel*>(GetChild(L"extra_slot_num_label"));
	m_statUpButton = dynamic_cast<RFButton*>(GetChild(L"button_stat_up"));
	m_statDownButton = dynamic_cast<RFButton*>(GetChild(L"button_stat_dn"));
	m_tooltip = RFMasteryToolTipWnd::Load(GetSuper(false)->GetSuper(false)->GetSuper(false));
}
