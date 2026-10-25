// The JP 983 character mastery's slot group (JpMasterySlot.h), from the JP
// exe's bodies (node scripts/disasm.ts --jp 0x<va>), in address order. The
// creators use RFManager.h's CreateWnd<T> / CreateDesktopWnd<T>
// (RFCharMasteryTileDlg's instances 0x5c2850 / 0x5c28e0).
#include "JpMasterySlot.h"

#include <algorithm>
#include <string>

#include "Packet.h"
#include "SharedDoc.h"
#include "commonutil.h"

#include "JpLobbyDialogs.h"
#include "JpLobbyGlue.h"
#include "JpMasteryCard.h"
#include "JpMasteryMainDlg.h"
#include "JpMasterySlotAni.h"
#include "JpQuestForms.h"
#include "JpTooltipWnd.h"
#include "RFButton.h"
#include "RFCheck.h"
#include "RFEventArgs.h"
#include "RFManager.h"
#include "RFSize.h"
#include "RFTextManager.h"
#include "RFVector2.h"
#include "RefreshStringUtil.h"
#include "SoundManager.h"

// 0xe78120
RFCharMasteryTileDlg* RFCharMasteryTileDlg::s_instance = NULL;
// 0xe7811c
RFWnd* RFCharMasteryTileDlg::s_loadParent = NULL;
// 0xe78104
RFWnd* RFMasteryslotNoticeDlg::s_loadParent = NULL;
// 0xe78108
RFMasteryslotNoticeDlg* RFMasteryslotNoticeDlg::s_instance = NULL;
// 0xe7810c
RFWnd* RFMasteryslotConfirmDlg::s_loadParent = NULL;
// 0xe78114
RFWnd* RFMasteryTileColumn::s_loadParent = NULL;

// The UI sounds every mastery handler plays through the game's sound
// manager (0xf023f4, slot 3; constants 0xd66d08 0.5f and 0xd00d90 200.0f).
static void PlayMasterySound(const char* name) {
	g_audio->PlaySfx(name, NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x5c0260
RFMasteryslotNoticeDlg* RFMasteryslotNoticeDlg::GetInstance() {
	return s_instance;
}

// 0x5c0270
void RFMasteryslotNoticeDlg::SetInstance(RFMasteryslotNoticeDlg* instance) {
	s_instance = instance;
}

// 0x5c0280
void RFMasteryslotNoticeDlg::ClearInstance() {
	s_instance = NULL;
}

// 0x5c0290
const JpMasterySlotRecord* RFMasteryTileColumn::GetColumnData() {
	return m_columnData;
}

// 0x5c02a0
RFCharMasteryTileDlg* RFCharMasteryTileDlg::GetInstance() {
	return s_instance;
}

// 0x5c02b0
void RFCharMasteryTileDlg::SetInstance(RFCharMasteryTileDlg* instance) {
	s_instance = instance;
}

// 0x5c02c0
void RFCharMasteryTileDlg::ClearInstance() {
	s_instance = NULL;
}

// 0x5c02d0
bool JpMasteryStatInfo::IsValidStat(uint32_t stat) {
	return stat <= 4;
}

// 0x5c0360
void RFMasteryslotNoticeDlg::OnOkClicked(EventArgs& args) {
	PlayMasterySound("ui_button_ok_click");
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5c03a0
void RFMasteryslotConfirmDlg::OnOkClicked(EventArgs& args) {
	PlayMasterySound("ui_button_ok_click");
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5c03e0
void RFMasteryslotConfirmDlg::OnCancelClicked(EventArgs& args) {
	PlayMasterySound("ui_button_cancel_click");
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5c0420
void RFMasteryTileColumn::SetColumnData(const JpMasterySlotRecord* data) {
	m_columnData = data;
}

// 0x5c0430
void RFMasteryTileColumn::ShowSelected(bool show) {
	if (m_selectedImg != NULL) {
		m_selectedImg->ShowWindow(show);
	}
}

// 0x5c0450
void RFCharMasteryTileDlg::UpdateSlotLabels() {
	JpMasteryStatInfo info;
	if (m_powerSlotLabel != NULL) {
		m_powerSlotLabel->SetLabel(info.GetOpenSlotCount(0, -1), false);
	}
	if (m_penaltyPowerLabel != NULL) {
		m_penaltyPowerLabel->SetLabel(info.GetPenaltyValue() + 1, false);
	}
}

// 0x5c0510
int RFCharMasteryTileDlg::GetStatCountIndex(int32_t stat) {
	switch (stat) {
	case 1:
		return 0;
	case 2:
		return 1;
	case 4:
		return 2;
	case 5:
		return 3;
	}
	return stat - 1;
}

// 0x5c0610
void RFCharMasteryTileDlg::OnTooltipMouseLeave(EventArgs& args) {
	if (m_tooltip != NULL) {
		m_tooltip->Show(false);
	}
}

// 0x5c06d0
bool Jp_FindMasteryData(int id, IContentsDataContainer** data) {
	IContentsDataContainer* found = JpMastery_FindData(id);
	if (found != NULL) {
		*data = found;
		return true;
	}
	return false;
}

// 0x5c0700
RFList* RFCharMasteryTileDlg::GetChildList(const std::wstring& name) {
	return dynamic_cast<RFList*>(GetChild(name));
}

// 0x5c0820 (one copy, shared by both dialogs: a jump to [34])
void RFMasteryslotNoticeDlg::OnKey(EventArgs& args) {
	OnDialogKey(static_cast<KeyEventArgs&>(args));
}

// 0x5c0820 (the confirm dialog's use of the shared copy)
void RFMasteryslotConfirmDlg::OnKey(EventArgs& args) {
	OnDialogKey(static_cast<KeyEventArgs&>(args));
}

// 0x5c0830
bool RFMasteryslotNoticeDlg::HasInstance() {
	return s_instance != NULL;
}

// 0x5c0840
const char* JpMasteryStatInfo::GetStatName(uint32_t stat) {
	if (stat > 4) {
		return "";
	}
	const char* names[5];
	names[0] = Jp_LookupText("\xc6\xc4\xbf\xf6");          // 0xd01934 power
	names[1] = Jp_LookupText("\xc4\xc1\xc6\xae\xb7\xd1");  // 0xd01924 control
	names[2] = Jp_LookupText("\xc1\xa4\xc8\xae\xb5\xb5");  // 0xd01944 accuracy
	names[3] = Jp_LookupText("\xbd\xba\xc7\xc9");          // 0xd0192c spin
	names[4] = Jp_LookupText("\xc4\xbf\xba\xea");          // 0xd0193c curve
	return names[stat];
}

// 0x5c0d20
RFMasteryslotNoticeDlg::RFMasteryslotNoticeDlg()
	: m_slotImg(NULL), m_message(NULL), m_slotRecord(NULL) {
	s_instance = this;
}

// 0x5c0e30
void RFMasteryslotNoticeDlg::OnDialogKey(KeyEventArgs& args) {
	if (args.keyCode == 0xd || args.keyCode == 0x1b) {
		OnOkClicked(args);
		args.handled = true;
	}
}

// 0x5c0e60
RFMasteryslotConfirmDlg::RFMasteryslotConfirmDlg()
	: m_message(NULL), m_slotImg(NULL), m_slotRecord(NULL) {
}

// 0x5c0f60
RFMasteryTileColumn::RFMasteryTileColumn()
	: m_slotImg(NULL), m_lockedImg(NULL), m_selectedImg(NULL), m_columnData(NULL), m_state(3) {
}

// 0x5c1630 (scalar deleting destructor; the destructor itself is the
// inlined base chain)
RFMasteryslotNoticeDlg::~RFMasteryslotNoticeDlg() {
}

// 0x5c1660 (scalar deleting destructor)
RFMasteryslotConfirmDlg::~RFMasteryslotConfirmDlg() {
}

// 0x5c1690 (scalar deleting destructor)
RFMasteryTileColumn::~RFMasteryTileColumn() {
}

// 0x5c16c0
// The current task's wait message, then packet 0x187 with the character's
// first two dwords (type id and id: the JP and KR records agree there).
void RFCharMasteryTileDlg::RequestSelectedSlot(EventArgs&) {
	JpMastery_PostTaskWait();
	WSendPacket packet(0x187);
	packet.Encode4((uint32_t)m_character->typeId);
	packet.Encode4((uint32_t)m_character->id);
	packet.Send(SEND_TO_DEFAULT);
}

// 0x5c17a0
void RFCharMasteryTileDlg::ClearSelectedMarks() {
	if (m_slotList1 != NULL) {
		for (int32_t i = 0; i < m_slotList1->GetItemCount(); ++i) {
			RFMasteryTileColumn* column = static_cast<RFMasteryTileColumn*>(m_slotList1->GetItemData(i));
			if (column != NULL) {
				column->ShowSelected(false);
			}
		}
	}
	if (m_slotList2 != NULL) {
		for (int32_t i = 0; i < m_slotList2->GetItemCount(); ++i) {
			RFMasteryTileColumn* column = static_cast<RFMasteryTileColumn*>(m_slotList2->GetItemData(i));
			if (column != NULL) {
				column->ShowSelected(false);
			}
		}
	}
}

// 0x5c1840
const JpMasterySlotRecord* RFCharMasteryTileDlg::GetSlotRecord(int number) {
	if (m_slotRecords.empty()) {
		return NULL;
	}
	int index = number - 1;
	int last = static_cast<int>(m_slotRecords.size()) - 1;
	if (index < 0) {
		index = 0;
	} else if (index > last) {
		index = last;
	}
	return m_slotRecords.at(index);
}

// 0x5c1b70
void RFMasteryslotConfirmDlg::OnDialogKey(KeyEventArgs& args) {
	if (args.keyCode == 0xd) {
		if (RFCharMasteryTileDlg::s_instance != NULL) {
			RFCharMasteryTileDlg::s_instance->RequestSelectedSlot(args);
		}
		args.handled = true;
		Close(static_cast<RFWNDCLOSESTATE>(1));
	} else if (args.keyCode == 0x1b) {
		Close(static_cast<RFWNDCLOSESTATE>(2));
		args.handled = true;
	}
}

// 0x5c1c70
bool RFCharMasteryTileDlg::MeetsRequirement(int32_t requirement) {
	if (m_masteryData == NULL) {
		return false;
	}
	const std::map<unsigned long, _gateway::_questoneday::JpAchievementRecord>& achievements = m_masteryData->GetAchievements();
	for (std::map<unsigned long, _gateway::_questoneday::JpAchievementRecord>::const_iterator it = achievements.begin();
		 it != achievements.end(); ++it) {
		for (uint32_t i = 0; i < 10; ++i) {
			if (it->second.GetCounterId(i) == static_cast<uint32_t>(requirement) && *it->second.GetCounterCount(i) != 0) {
				return true;
			}
		}
	}
	return false;
}

// 0x5c1dc0
void RFMasteryslotNoticeDlg::SetSlotRecord(const JpMasterySlotRecord* record) {
	m_slotRecord = record;
	if (record != NULL && m_slotImg != NULL) {
		std::string name(MakeStr("slot_image_%d_1.tga", record->stat - 1));
		m_slotImg->SetPicture(ShiftJisToWString(name));
	}
}

// 0x5c1ec0
void RFMasteryslotConfirmDlg::SetSlotRecord(const JpMasterySlotRecord* record) {
	m_slotRecord = record;
	if (record != NULL && m_slotImg != NULL) {
		std::string name(MakeStr("slot_image_%d_0.tga", record->stat - 1));
		m_slotImg->SetPicture(ShiftJisToWString(name));
	}
}

// 0x5c1fc0
void RFMasteryTileColumn::UpdateSlotState(int32_t selectedSlotNumber) {
	if (m_columnData == NULL) {
		return;
	}
	m_state = 0;
	if (selectedSlotNumber >= m_columnData->slotNumber) {
		m_state = 2;
		return;
	}
	if (m_columnData->slotNumber == selectedSlotNumber + 1) {
		bool requirementMet = true;
		bool levelMet = true;
		if (m_columnData->requirement != 0) {
			requirementMet = RFCharMasteryTileDlg::s_instance->MeetsRequirement(m_columnData->requirement);
		}
		if (m_columnData->requiredLevel != 0) {
			levelMet = Jp_GameState_MyLevel() >= m_columnData->requiredLevel;
		}
		if (requirementMet && levelMet) {
			m_state = 1;
		}
	}
}

// 0x5c2050
void RFMasteryTileColumn::UpdateSlotImage() {
	if (m_columnData == NULL) {
		return;
	}
	if (m_slotImg != NULL) {
		std::string name(MakeStr("slot_image_%d_%d.tga", m_columnData->stat - 1, m_state == 2));
		m_slotImg->SetPicture(ShiftJisToWString(name));
	}
	if (m_lockedImg != NULL) {
		m_lockedImg->ShowWindow(m_state == 0);
	}
}

// 0x5c2190
RFCharMasteryTileDlg::RFCharMasteryTileDlg() {
	s_instance = this;
	// The original zeroes 0x184-0x1d7 with one memset.
	m_currentColumn = NULL;
	m_rankImg = NULL;
	m_powerSlotLabel = NULL;
	m_penaltyPowerLabel = NULL;
	m_slotList1 = NULL;
	m_slotList2 = NULL;
	for (int i = 0; i < 4; ++i) {
		m_statCountLabels[i] = NULL;
	}
	m_needTpLabel = NULL;
	m_currentTpLabel = NULL;
	m_levelLimitLabel = NULL;
	m_achievementClearLabel = NULL;
	m_levelLimitCheck = NULL;
	m_achievementClearCheck = NULL;
	m_slotExpandedImg = NULL;
	m_expandButton = NULL;
	m_slotRecords.clear();
	m_character = NULL;
	m_tooltip = NULL;
	m_masteryData = NULL;
}

// 0x5c2280
RFCharMasteryTileDlg::~RFCharMasteryTileDlg() {
	s_instance = NULL;
}

// 0x5c2320
void RFCharMasteryTileDlg::UpdateStatCounts() {
	int32_t counts[4] = { 0, 0, 0, 0 };
	std::for_each(m_slotRecords.begin(), m_slotRecords.end(), [this, &counts](const JpMasterySlotRecord* record) {
		int index = record->stat - 1;
		switch (index) {
		case 0:
			index = 0;
			break;
		case 1:
			index = 1;
			break;
		case 3:
			index = 2;
			break;
		case 4:
			index = 3;
			break;
		}
		if (index == -1) {
			return;
		}
		if (JpCharacter_GetMasterySlotNumber(m_character) >= record->slotNumber) {
			++counts[index];
		}
	});
	for (int i = 0; i < 4; ++i) {
		if (m_statCountLabels[i] != NULL) {
			std::string text(MakeStr("x%d", counts[i]));
			m_statCountLabels[i]->SetLabel(ShiftJisToWString(text));
		}
	}
}

// 0x5c2460
void RFCharMasteryTileDlg::OnRankMouseEnter(EventArgs& args) {
	if (m_tooltip != NULL) {
		std::string text(Jp_GetTableText(0x18843));
		m_tooltip->SetText(text.c_str());
		if (m_rankImg != NULL) {
			RFVector2 pos = m_rankImg->GetScreenPos();
			RFSize size = m_rankImg->GetSize();
			m_tooltip->SetPosition(size.width + pos.x, pos.y);
		}
		m_tooltip->Show(true);
	}
}

// 0x5c2580
void RFCharMasteryTileDlg::OnPowerSlotMouseEnter(EventArgs& args) {
	if (m_tooltip != NULL) {
		JpMasteryStatInfo info;
		// The next level that opens one more power slot.
		int openSlots = info.GetOpenSlotCount(0, -1);
		int nextLevel = -1;
		for (int level = Jp_GameState_MyLevel() + 1; level < 0x47; ++level) {
			if (openSlots < info.GetOpenSlotCount(0, level)) {
				nextLevel = level;
				break;
			}
		}
		const char* levelName;
		if (nextLevel == -1) {
			levelName = "-";  // 0xd02760
		} else {
			levelName = Jp_LookupText(JpLevelTable_Find(nextLevel).name);
		}
		std::string text(MakeStr(Jp_GetTableText(0x18844), levelName));
		m_tooltip->SetText(text.c_str());
		if (m_powerSlotLabel != NULL) {
			RFVector2 pos = m_powerSlotLabel->GetScreenPos();
			RFSize size = m_powerSlotLabel->GetSize();
			m_tooltip->SetPosition(size.width + pos.x, pos.y);
		}
		m_tooltip->Show(true);
	}
}

// 0x5c2730
void RFCharMasteryTileDlg::OnPenaltyPowerMouseEnter(EventArgs& args) {
	if (m_tooltip != NULL) {
		std::string text(Jp_GetTableText(0x18845));
		m_tooltip->SetText(text.c_str());
		if (m_penaltyPowerLabel != NULL) {
			RFVector2 pos = m_penaltyPowerLabel->GetScreenPos();
			RFSize size = m_penaltyPowerLabel->GetSize();
			m_tooltip->SetPosition(size.width + pos.x, pos.y);
		}
		m_tooltip->Show(true);
	}
}

// 0x5c2970
void RFMasteryTileColumn::Refresh(int32_t selectedSlotNumber) {
	UpdateSlotState(selectedSlotNumber);
	UpdateSlotImage();
}

// 0x5c2a40
void RFCharMasteryTileDlg::SelectColumn(const JpMasterySlotRecord* record) {
	if (record == NULL) {
		return;
	}
	if (m_currentColumn != NULL) {
		m_currentColumn->SetColumnData(record);
		m_currentColumn->Refresh(JpCharacter_GetMasterySlotNumber(m_character));
	}
	int32_t needTp = 0;
	if (m_needTpLabel != NULL) {
		m_needTpLabel->ClearText();
		needTp = record->needTp;
		std::string text(MakeStr(Jp_GetTableText(0x1883e), needTp));
		m_needTpLabel->SetText(ShiftJisToWString(std::string(text.c_str())));
	}
	int32_t currentTp = 0;
	if (m_currentTpLabel != NULL) {
		m_currentTpLabel->ClearText();
		const sItemInfo* item = Jp_GameState_FindInventoryItem(0x1a0002a6);
		if (item != NULL) {
			currentTp = item->m_count;
		}
		std::string text(MakeStr(Jp_GetTableText(0x1883f), currentTp));
		// Black when the TP covers the cost, red otherwise.
		m_currentTpLabel->SetColor(needTp <= currentTp ? 0xff000000u : 0xffff0000u);
		m_currentTpLabel->SetText(ShiftJisToWString(std::string(text.c_str())));
	}
	if (m_levelLimitLabel != NULL) {
		m_levelLimitLabel->ClearText();
		if (record->requiredLevel != 0) {
			const char* levelName = Jp_LookupText(JpLevelTable_Find(record->requiredLevel).name);
			std::string text(MakeStr(Jp_GetTableText(0x18840), levelName));
			m_levelLimitLabel->SetText(ShiftJisToWString(std::string(text.c_str())));
		}
	}
	if (m_levelLimitCheck != NULL) {
		m_levelLimitCheck->ShowWindow(record->requiredLevel != 0);
		m_levelLimitCheck->SetCheck(Jp_GameState_MyLevel() >= static_cast<uint32_t>(record->requiredLevel));
	}
	int32_t requirement = record->requirement;
	if (m_achievementClearLabel != NULL) {
		m_achievementClearLabel->ClearText();
		const char* achievementName = Jp_FindAchievementName(requirement);
		if (achievementName != NULL) {
			std::string text(MakeStr(Jp_GetTableText(0x18841), achievementName));
			m_achievementClearLabel->SetText(ShiftJisToWString(std::string(text.c_str())));
		}
	}
	if (m_achievementClearCheck != NULL) {
		m_achievementClearCheck->ShowWindow(requirement != 0);
		m_achievementClearCheck->SetCheck(MeetsRequirement(requirement));
	}
	if (m_slotExpandedImg != NULL) {
		m_slotExpandedImg->ShowWindow(JpCharacter_GetMasterySlotNumber(m_character) >= record->slotNumber);
	}
	if (m_expandButton != NULL) {
		bool canOpen = true;
		if (JpCharacter_GetMasterySlotNumber(m_character) + 1 != record->slotNumber) {
			canOpen = false;
		}
		if (currentTp < record->needTp) {
			canOpen = false;
		}
		if (m_levelLimitCheck->IsWindowVisible(true) && !m_levelLimitCheck->GetCheck()) {
			canOpen = false;
		}
		bool enable;
		if (m_achievementClearCheck->IsWindowVisible(true)) {
			enable = m_achievementClearCheck->GetCheck() && canOpen;
		} else {
			enable = canOpen;
		}
		m_expandButton->EnableWindow(enable);
	}
}

// 0x5c30c0
RFWnd* CreateMasteryslotNoticeDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RFMasteryslotNoticeDlg>(&RFManager::GetInstance(), L"mastery_slot_notice_dlg.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFMasteryslotNoticeDlg>(&RFManager::GetInstance(), L"mastery_slot_notice_dlg.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFMasteryslotNoticeDlg>(&RFManager::GetInstance(), L"mastery_slot_notice_dlg.xml");
		break;
	}
	RFMasteryslotNoticeDlg::s_loadParent = parent;
	return wnd;
}

// 0x5c3200
void RFMasteryslotNoticeDlg::SetMessage(const char* text) {
	if (m_message != NULL) {
		std::string message(text);
		m_message->SetText(ShiftJisToWString(message));
	}
}

// 0x5c3340
void RFMasteryslotNoticeDlg::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventKeyDown, Subscriber(&RFMasteryslotNoticeDlg::OnKey, this));
	RegisterTo(L"ok_btn", RFButton::EventClicked, Subscriber(&RFMasteryslotNoticeDlg::OnOkClicked, this));
	m_slotImg = dynamic_cast<RFPicture*>(GetChild(L"slot_img"));
	m_message = dynamic_cast<RFTextArea*>(GetChild(L"message"));
	PlayMasterySound("ui_dialog_open");
}

// 0x5c3600
RFWnd* CreateMasteryslotConfirmDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpModalCreateMode) {
	case 0:
		wnd = CreateWnd<RFMasteryslotConfirmDlg>(&RFManager::GetInstance(), L"mastery_slot_confirm_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFMasteryslotConfirmDlg>(&RFManager::GetInstance(), L"mastery_slot_confirm_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFMasteryslotConfirmDlg>(&RFManager::GetInstance(), L"mastery_slot_confirm_dialog.xml");
		break;
	}
	RFMasteryslotConfirmDlg::s_loadParent = parent;
	return wnd;
}

// 0x5c3740
void RFMasteryslotConfirmDlg::SetMessage(const char* text) {
	if (m_message != NULL) {
		std::string message(text);
		m_message->SetText(ShiftJisToWString(message));
	}
}

// 0x5c3880
void RFMasteryslotConfirmDlg::OnInitialUpdate() {
	m_message = dynamic_cast<RFTextArea*>(GetChild(L"message"));
	m_slotImg = dynamic_cast<RFPicture*>(GetChild(L"slot_img"));
	RegisterTo(L"", RFWnd::EventKeyDown, Subscriber(&RFMasteryslotConfirmDlg::OnKey, this));
	RegisterTo(L"ok_btn", RFButton::EventClicked, Subscriber(&RFMasteryslotConfirmDlg::OnOkClicked, this));
	RegisterTo(L"cancel_btn", RFButton::EventClicked, Subscriber(&RFMasteryslotConfirmDlg::OnCancelClicked, this));
	PlayMasterySound("ui_dialog_open");
}

// 0x5c3c00
RFWnd* CreateMasteryTileColumn(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFMasteryTileColumn>(&RFManager::GetInstance(), L"slot_mastery_column.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFMasteryTileColumn>(&RFManager::GetInstance(), L"slot_mastery_column.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFMasteryTileColumn>(&RFManager::GetInstance(), L"slot_mastery_column.xml");
		break;
	}
	RFMasteryTileColumn::s_loadParent = parent;
	return wnd;
}

// 0x5c3d40
void RFMasteryTileColumn::OnColumnClicked(EventArgs& args) {
	if (RFCharMasteryTileDlg::s_instance != NULL) {
		RFCharMasteryTileDlg::s_instance->SelectColumn(m_columnData);
		RFCharMasteryTileDlg::s_instance->ClearSelectedMarks();
	}
	if (m_selectedImg != NULL) {
		m_selectedImg->ShowWindow(true);
	}
	PlayMasterySound("ui_icon_click");
}

// 0x5c3db0
RFWnd* CreateMasteryTileDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharMasteryTileDlg>(&RFManager::GetInstance(), L"slot_mastery_wnd.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharMasteryTileDlg>(&RFManager::GetInstance(), L"slot_mastery_wnd.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharMasteryTileDlg>(&RFManager::GetInstance(), L"slot_mastery_wnd.xml");
		break;
	}
	RFCharMasteryTileDlg::s_loadParent = parent;
	return wnd;
}

// 0x5c3ef0
void RFCharMasteryTileDlg::InitRankWidgets() {
	m_rankImg = dynamic_cast<RFPicture*>(GetChild(L"rank_img"));
	if (m_rankImg != NULL) {
		std::string name(MakeStr("level_%03d.tga", Jp_GameState_MyLevel() + 1));
		m_rankImg->SetPicture(ShiftJisToWString(name));
		m_rankImg->SetSize(m_rankImg->GetTextureSize());
	}
	m_powerSlotLabel = dynamic_cast<RFLabel*>(GetChild(L"rank_power_slot_num_label"));
	m_penaltyPowerLabel = dynamic_cast<RFLabel*>(GetChild(L"rank_penalty_power_num_label"));
}

// 0x5c4190
void RFCharMasteryTileDlg::InitStatCountLabels() {
	m_statCountLabels[0] = dynamic_cast<RFLabel*>(GetChild(L"total_power_slot_num_label"));
	m_statCountLabels[1] = dynamic_cast<RFLabel*>(GetChild(L"total_control_slot_num_label"));
	m_statCountLabels[2] = dynamic_cast<RFLabel*>(GetChild(L"total_spin_slot_num_label"));
	m_statCountLabels[3] = dynamic_cast<RFLabel*>(GetChild(L"total_curve_slot_num_label"));
}

// 0x5c4370
void RFCharMasteryTileDlg::OnExpandClicked(EventArgs&) {
	RFMasteryslotConfirmDlg* confirm = static_cast<RFMasteryslotConfirmDlg*>(CreateMasteryslotConfirmDialog(this));
	if (confirm != NULL) {
		const JpMasterySlotRecord* record = m_currentColumn->m_columnData;
		std::string text(MakeStr(Jp_GetTableText(0x18842), record->needTp));
		confirm->SetSlotRecord(record);
		confirm->SetMessage(text.c_str());
		confirm->RegisterTo(L"ok_btn", RFButton::EventClicked, Subscriber(&RFCharMasteryTileDlg::RequestSelectedSlot, this));
		confirm->ShowModal();
	}
}

// 0x5c45c0
void RFMasteryTileColumn::OnInitialUpdate() {
	RegisterTo(L"slot_btn", RFButton::EventClicked, Subscriber(&RFMasteryTileColumn::OnColumnClicked, this));
	m_slotImg = dynamic_cast<RFPicture*>(GetChild(L"slot_img"));
	m_lockedImg = dynamic_cast<RFPicture*>(GetChild(L"locked_img"));
	m_selectedImg = dynamic_cast<RFPicture*>(GetChild(L"selected_img"));
	if (m_lockedImg != NULL) {
		m_lockedImg->ShowWindow(false);
	}
	if (m_selectedImg != NULL) {
		m_selectedImg->ShowWindow(false);
	}
}

// 0x5c4810
void RFCharMasteryTileDlg::BuildSlotColumns() {
	if (m_slotRecords.empty()) {
		return;
	}
	ClearSelectedMarks();
	int index = 0;
	std::for_each(m_slotRecords.begin(), m_slotRecords.end(), [this, &index](const JpMasterySlotRecord* record) {
		RFList* list = index > 4 ? m_slotList2 : m_slotList1;
		RFMasteryTileColumn* column = static_cast<RFMasteryTileColumn*>(list->GetItemData(index % 5));
		if (column != NULL) {
			column->Refresh(JpCharacter_GetMasterySlotNumber(m_character));
			if (JpCharacter_GetMasterySlotNumber(m_character) + 1 == record->slotNumber) {
				column->ShowSelected(true);
			}
		}
		++index;
	});
}

// 0x5c4860
void RFCharMasteryTileDlg::LoadSlotRecords() {
	JpMasteryStatInfo info;
	info.GetSlotRecords(m_slotRecords, 0, -1);
	m_character = Jp_GameState_EquippedCharacterData();
}

// 0x5c48f0
void RFCharMasteryTileDlg::InitExpandPanel() {
	RegisterTo(L"button_slot_expand", RFButton::EventClicked, Subscriber(&RFCharMasteryTileDlg::OnExpandClicked, this));
	RFWndEx* placeholder = dynamic_cast<RFWndEx*>(GetChild(L"selected_slot_wnd"));
	RFWndEx* popup = dynamic_cast<RFWndEx*>(GetChild(L"slot_expand_pop"));
	if (placeholder != NULL && popup != NULL) {
		m_currentColumn = static_cast<RFMasteryTileColumn*>(CreateMasteryTileColumn(popup));
		if (m_currentColumn != NULL) {
			m_currentColumn->SetPos(placeholder->GetPos().x, placeholder->GetPos().y);
			m_currentColumn->ShowSelected(false);
			m_currentColumn->ShowForm(true);
		}
		DeleteChild(placeholder);
	}
	m_needTpLabel = dynamic_cast<RFTextArea*>(GetChild(L"need_tp_label"));
	m_currentTpLabel = dynamic_cast<RFTextArea*>(GetChild(L"current_tp_label"));
	m_levelLimitLabel = dynamic_cast<RFTextArea*>(GetChild(L"level_limit_label"));
	m_achievementClearLabel = dynamic_cast<RFTextArea*>(GetChild(L"achivement_clear_label"));
	m_levelLimitCheck = dynamic_cast<RFCheck*>(GetChild(L"level_limit_check"));
	m_achievementClearCheck = dynamic_cast<RFCheck*>(GetChild(L"achivement_clear_check"));
	m_slotExpandedImg = dynamic_cast<RFPicture*>(GetChild(L"slot_expanded_img"));
	m_expandButton = dynamic_cast<RFButton*>(GetChild(L"button_slot_expand"));
}

// 0x5c4e70
void RFCharMasteryTileDlg::Setup() {
	UpdateSlotLabels();
	BuildSlotColumns();
	UpdateStatCounts();
	SelectColumn(GetSlotRecord(JpCharacter_GetMasterySlotNumber(m_character) + 1));
}

// 0x5c4eb0
void RFCharMasteryTileDlg::InitSlotLists() {
	if (m_slotRecords.empty()) {
		return;
	}
	m_slotList1 = dynamic_cast<RFList*>(GetChild(L"mastery_table1"));
	m_slotList2 = dynamic_cast<RFList*>(GetChild(L"mastery_table2"));
	if (m_slotList1 != NULL && m_slotList2 != NULL) {
		// 0x5c4540: std::for_each with this lambda.
		int index = 0;
		std::for_each(m_slotRecords.begin(), m_slotRecords.end(), [this, &index](const JpMasterySlotRecord* record) {
			RFList* list = index > 4 ? m_slotList2 : m_slotList1;
			RFMasteryTileColumn* column = static_cast<RFMasteryTileColumn*>(CreateMasteryTileColumn(list));
			if (column != NULL) {
				column->SetColumnData(record);
				column->ShowForm(true);
				list->InsertWindowItem(column);
			}
			++index;
		});
	}
}

// 0x5c5000
void RFCharMasteryTileDlg::OnSlotAnimationDone() {
	if (RFMasteryslotNoticeDlg::s_instance != NULL) {
		return;
	}
	RFMasteryslotNoticeDlg* notice = static_cast<RFMasteryslotNoticeDlg*>(CreateMasteryslotNoticeDialog(this));
	if (notice != NULL) {
		JpMasteryStatInfo info;
		const JpMasterySlotRecord* record = m_currentColumn->m_columnData;
		std::string text(MakeStr(Jp_GetTableText(0x18846), info.GetStatName(record->stat - 1)));
		notice->SetSlotRecord(record);
		notice->SetMessage(text.c_str());
		notice->ShowModal();
	}
	Setup();
}

// 0x5c5270
void RFCharMasteryTileDlg::OnInitialUpdate() {
	RegisterTo(L"bg_rank", RFWnd::EventMouseEnter, Subscriber(&RFCharMasteryTileDlg::OnRankMouseEnter, this));
	RegisterTo(L"bg_rank", RFWnd::EventMouseLeave, Subscriber(&RFCharMasteryTileDlg::OnTooltipMouseLeave, this));
	RegisterTo(L"bg_power_slot", RFWnd::EventMouseEnter, Subscriber(&RFCharMasteryTileDlg::OnPowerSlotMouseEnter, this));
	RegisterTo(L"bg_power_slot", RFWnd::EventMouseLeave, Subscriber(&RFCharMasteryTileDlg::OnTooltipMouseLeave, this));
	RegisterTo(L"bg_penalty_power", RFWnd::EventMouseEnter, Subscriber(&RFCharMasteryTileDlg::OnPenaltyPowerMouseEnter, this));
	RegisterTo(L"bg_penalty_power", RFWnd::EventMouseLeave, Subscriber(&RFCharMasteryTileDlg::OnTooltipMouseLeave, this));
	LoadSlotRecords();
	InitRankWidgets();
	InitSlotLists();
	InitStatCountLabels();
	InitExpandPanel();
	m_tooltip = RFMasteryToolTipWnd::Load(GetSuper(false));
	IContentsDataContainer* data = JpMastery_FindData(0xac);
	if (data != NULL) {
		m_masteryData = static_cast<_gateway::_questoneday::CQuestDocument*>(data);
	}
}

// 0x5c59e0
void RFCharMasteryTileDlg::PlaySlotAnimation() {
	RFSoltAniWnd* wnd = static_cast<RFSoltAniWnd*>(CreateSoltAniWnd(NULL));
	if (wnd != NULL) {
		int stat = m_currentColumn->m_columnData->stat - 1;
		wnd->ShowModal();
		wnd->PlayStatEffect(stat);
	}
}
