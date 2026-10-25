// The JP 983 character mastery cards (JpMasteryCard.h), from the JP exe's
// bodies (node scripts/disasm.ts --jp 0x<va>, jp_hlil.ts), in address
// order.
#include "JpMasteryCard.h"

#include <algorithm>
#include <string>

#include "commonutil.h"
#include "SharedDoc.h"

#include "JpCommonDialogs.h"
#include "JpLobbyGlue.h"
#include "JpMasteryTooltipWnd.h"
#include "JpTooltipWnd.h"
#include "RFManager.h"
#include "RFButton.h"
#include "RFEventArgs.h"
#include "RFLabel.h"
#include "RFListSC.h"
#include "RFPicture.h"
#include "RFTextArea.h"
#include "RFTextManager.h"
#include "RFSize.h"
#include "RefreshStringUtil.h"
#include "SoundManager.h"
#include "RFVector2.h"

// 0xe780cc
RFCharMasteryCardDlg* g_charMasteryCardDlg = NULL;
// 0xe780b4
RFCharMasteryCardSelectDlg* g_masteryCardSelectDlg = NULL;
// 0xe780bc
RFCharacterMasteryCardDetachDlg* g_masteryCardDetachDlg = NULL;
// 0xe780b0
RFWnd* RFCharMasteryCardSelectDlg::s_loadParent = NULL;
// 0xe780b8
RFWnd* RFCharacterMasteryCardDetachDlg::s_loadParent = NULL;
// 0xe780c0
RFWnd* RFCardIcon::s_loadParent = NULL;
// 0xe780c8
RFWnd* RFCharMasteryCardDlg::s_loadParent = NULL;
// 0xe780a8
RFWnd* RFCharMasteryCardSelectColumn::s_loadParent = NULL;
// 0xe780a0
RFWnd* RFCharMasteryCardNoticeDlg::s_loadParent = NULL;
// 0xe780a4
RFCharMasteryCardNoticeDlg* g_masteryCardNoticeDlg = NULL;

// lib: 0x5bb9e0 (std::for_each over the card list with
// RFCharMasteryCardSelectDlg::AddCardColumn, its operator() inlined),
// 0x5b9ca0 (std::list begin: the head's next), 0x5b9cc0 (std::list end:
// the head), 0x5b9930 (the list iterator constructor, node and owner),
// 0x5b9790, 0x5b97a0, 0x5b97b0, 0x5b97c0, 0x5b97d0, 0x5b97e0 (implicit
// ~MemberFunctionFunctor<T>: the EventFunctorBase vptr store only)

// The UI sounds the card handlers play through the game's sound manager
// (0xf023f4, slot 3; constants 0xd66d08 0.5f and 0xd00d90 200.0f).
static void PlayMasteryCardSound(const char* name) {
	g_audio->PlaySfx(name, NULL, 0, NULL, NULL, 0.5f, 200.0f);
}

// 0x5b8000 (name ours): slots 9 and 10 take the second card packet.
static bool IsMasteryCardSpecialSlot(int slot) {
	if (slot >= 9 && slot <= 10) {
		return true;
	}
	return false;
}

// 0x5b8010
bool IsMasteryCardLockableSlot(int slot) {
	return slot == 6;
}

// 0x5b8020 (name ours): a card slot (1-10) to the server's slot index;
// -1 for any other.
static int MasteryCardSlotToServerSlot(int slot) {
	switch (slot) {
	case 1:
		return 0;
	case 2:
		return 1;
	case 3:
		return 2;
	case 4:
		return 4;
	case 5:
		return 5;
	case 6:
		return 6;
	case 7:
		return 8;
	case 8:
		return 9;
	case 9:
		return 3;
	case 10:
		return 7;
	}
	return -1;
}

// 0x5b8120 (name ours): a card slot's card type (slots 1-3 and 9
// character 0, 4-6 and 10 caddie 1, 7-8 NPC 5); 6 for any other.
int MasteryCardSlotToCardType(int slot) {
	switch (slot) {
	case 1:
	case 2:
	case 3:
	case 9:
		return 0;
	case 4:
	case 5:
	case 6:
	case 10:
		return 1;
	case 7:
	case 8:
		return 5;
	}
	return 6;
}

// 0x5b82c0
void RFCharMasteryCardNoticeDlg::OnOkClicked(EventArgs&) {
	PlayMasteryCardSound("ui_button_ok_click");
	Close(static_cast<RFWNDCLOSESTATE>(1));
}

// 0x5b8320
void RFCharMasteryCardSelectDlg::OnDialogKey(KeyEventArgs& args) {
	if (args.keyCode == 0x1b) {
		Close(static_cast<RFWNDCLOSESTATE>(2));
		args.handled = true;
	}
}

// 0x5b8340
void RFCharMasteryCardSelectDlg::OnCancelClicked(EventArgs&) {
	PlayMasteryCardSound("ui_button_cancel_click");
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5b8380
void RFCharacterMasteryCardDetachDlg::OnDialogKey(KeyEventArgs& args) {
	if (args.keyCode == 0x1b) {
		PlayMasteryCardSound("ui_dialog_close");
		Close(static_cast<RFWNDCLOSESTATE>(2));
		args.handled = true;
	}
}

// 0x5b83e0
void RFCharacterMasteryCardDetachDlg::OnCloseClicked(EventArgs&) {
	PlayMasteryCardSound("ui_dialog_close");
	Close(static_cast<RFWNDCLOSESTATE>(2));
}

// 0x5b8420
void RFCardIcon::SetCard(const JpCardData* card, int slotIndex) {
	m_card = card;
	m_slotIndex = slotIndex;
}

// 0x5b8440
void RFCardIcon::SetLocked(bool locked, bool hideLockInfo) {
	m_locked = locked;
	m_lockInfoHidden = locked ? hideLockInfo : true;
}

// 0x5b8790 (the picker's use of the shared copy)
void RFCharMasteryCardSelectDlg::OnKey(EventArgs& args) {
	OnDialogKey(static_cast<KeyEventArgs&>(args));
}

// 0x5b8790 (the detach dialog's use of the shared copy)
void RFCharacterMasteryCardDetachDlg::OnKey(EventArgs& args) {
	OnDialogKey(static_cast<KeyEventArgs&>(args));
}

// 0x5b87f0
RFCharMasteryCardNoticeDlg::RFCharMasteryCardNoticeDlg()
	: m_cardImg(NULL)
	, m_message(NULL) {
}

// 0x5b8880
RFCharMasteryCardNoticeDlg::~RFCharMasteryCardNoticeDlg() {
}

// 0x5b88f0
RFCharMasteryCardSelectColumn::RFCharMasteryCardSelectColumn()
	: m_card(NULL)
	, m_cardImg(NULL)
	, m_cardName(NULL) {
}

// 0x5b8920
RFCharMasteryCardSelectColumn::~RFCharMasteryCardSelectColumn() {
}

// 0x5b8930
RFCharMasteryCardSelectDlg::RFCharMasteryCardSelectDlg()
	: m_cardList(NULL)
	, m_cardTypeTitle(NULL)
	, m_cardImg(NULL)
	, m_useItemImg(NULL)
	, m_useItemText(NULL)
	, m_noticeText(NULL)
	, m_warningText(NULL)
	, m_okButton(NULL)
	, m_selectedCard(NULL)
	, m_slotIndex(-1)
	, m_selectState(false) {
	g_masteryCardSelectDlg = this;
}

// 0x5b8a00
RFCharMasteryCardSelectDlg::~RFCharMasteryCardSelectDlg() {
	g_masteryCardSelectDlg = NULL;
}

// 0x5b8a70
RFCharacterMasteryCardDetachDlg::RFCharacterMasteryCardDetachDlg()
	: m_cardOriginImg(NULL)
	, m_slotIndex(-1) {
	g_masteryCardDetachDlg = this;
}

// 0x5b8b10
RFCharacterMasteryCardDetachDlg::~RFCharacterMasteryCardDetachDlg() {
	g_masteryCardDetachDlg = NULL;
}

// 0x5b8b80
RFCardIcon::RFCardIcon()
	: m_slotIndex(-1)
	, m_card(NULL)
	, m_cardImg(NULL)
	, m_cardImgNot(NULL)
	, m_cardBlank(NULL)
	, m_locked(false)
	, m_lockInfoHidden(true) {
}

// 0x5b8bd0 (scalar deleting destructor; the destructor is inlined there)
RFCardIcon::~RFCardIcon() {
}

// 0x5b8c00
RFCharMasteryCardDlg::RFCharMasteryCardDlg()
	: m_tooltip(NULL) {
	g_charMasteryCardDlg = this;
	for (int i = 0; i < kCardSlotCount; ++i) {
		m_cardIcons[i] = NULL;
	}
}

// 0x5b8c60
RFCharMasteryCardDlg::~RFCharMasteryCardDlg() {
	g_charMasteryCardDlg = NULL;
}

// 0x5b9880
void RFCharMasteryCardSelectDlg::UpdateOkButton() {
	if (m_okButton == NULL || !IsMasteryCardSpecialSlot(m_slotIndex)) {
		return;
	}
	const sItemInfo* item = Jp_GameState_FindInventoryItem(0x1a00018f);
	if (item == NULL || item->m_count <= 0) {
		m_okButton->EnableWindow(false);
	}
}

// 0x5b9e90: the card's name is read unchecked, as in the exe.
void RFCharMasteryCardNoticeDlg::SetCard(uint32_t typeId, bool attached) {
	const JpCardData* card = Jp_FindCard(typeId);
	if (card != NULL && m_cardImg != NULL) {
		std::string name(MakeStr("%s.png", JpCardData_GetImageName(card)));
		m_cardImg->SetPicture(ShiftJisToWString(name));
	}
	if (m_message != NULL) {
		std::string text("");
		if (attached) {
			text = MakeStr(Jp_GetTableText(0x18847), JpCardData_GetName(card));
		} else {
			text = MakeStr(Jp_GetTableText(0x18848), JpCardData_GetName(card));
		}
		m_message->SetText(ShiftJisToWString(std::string(text.c_str())));
	}
}

// 0x5ba0f0
void RFCharMasteryCardSelectColumn::SetCard(const JpCardEntry* entry) {
	const JpCardData* card = Jp_FindCard(entry->m_typeId);
	if (card == NULL) {
		return;
	}
	m_card = entry;
	if (m_cardImg != NULL) {
		std::string name(MakeStr("%s.png", JpCardData_GetImageName(card)));
		m_cardImg->SetPicture(ShiftJisToWString(name));
	}
	if (m_cardName != NULL) {
		std::string text(MakeStr("x%d %s", entry->m_count, JpCardData_GetName(card)));
		m_cardName->SetLabel(ShiftJisToWString(text));
	}
}

// 0x5ba2c0: JP +0x49 is taken as the KR sItem::m_icon (the JP name field
// is 0x18 bytes longer), as in JpCommonDialogs.cpp.
void RFCharMasteryCardSelectDlg::UpdateUseItem() {
	IFF_STRUCT::sItem* record = ItemManager()->FindItem(0x1a00018f);
	if (record == NULL) {
		return;
	}
	if (m_useItemImg != NULL) {
		std::string name(MakeStr("%s.tga", record->m_icon));
		m_useItemImg->SetPicture(ShiftJisToWString(name));
	}
	const sItemInfo* item = Jp_GameState_FindInventoryItem(record->m_typeId);
	if (m_useItemText != NULL) {
		std::string text(MakeStr("%s x%d", record->m_name, item != NULL ? item->m_count : 0));
		m_useItemText->SetText(ShiftJisToWString(text));
	}
	if (m_warningText != NULL) {
		std::string text("");
		if (IsMasteryCardSpecialSlot(m_slotIndex)) {
			text = Jp_GetTableText(0x18849);
		} else {
			text = Jp_GetTableText(0x1884a);
		}
		m_warningText->SetText(ShiftJisToWString(text));
		m_warningText->ShowWindow(true);
	}
}

// 0x5ba570
void RFCharacterMasteryCardDetachDlg::SetCard(const JpCardData* card, int slotIndex) {
	if (card == NULL) {
		return;
	}
	if (m_cardOriginImg != NULL) {
		std::string name(MakeStr("%s.png", JpCardData_GetImageName(card)));
		m_cardOriginImg->SetPicture(ShiftJisToWString(name));
	}
	m_slotIndex = slotIndex;
}

// 0x5ba670
void RFCardIcon::UpdateView() {
	if (m_cardImg != NULL) {
		std::string name(m_card != NULL ? MakeStr("%s.png", JpCardData_GetImageName(m_card)) : "");
		m_cardImg->SetPicture(ShiftJisToWString(name.c_str()));
	}
	if (m_cardImgNot != NULL) {
		m_cardImgNot->ShowWindow(!m_lockInfoHidden && m_card != NULL);
	}
	if (m_cardBlank != NULL) {
		m_cardBlank->ShowWindow(m_lockInfoHidden && m_card == NULL);
	}
}

// 0x5ba830
void RFCharMasteryCardDlg::ShowCardTooltip(bool show, const char* text) {
	if (m_tooltip == NULL) {
		return;
	}
	if (show) {
		m_tooltip->SetText(text);
		WVector const& mouse = RFSceneManager::Instance()->GetMousePosition();
		RFSize size = m_tooltip->GetSize();
		m_tooltip->SetPosition(mouse.x - size.width * 0.5, mouse.y - size.height);
	}
	m_tooltip->Show(show);
}

// 0x5ba8e0: the type names are the game's texts for the Korean
// "\xc4\xb3\xb8\xaf\xc5\xcd" (character) and "\xc4\xb3\xb5\xf0" (caddie).
void RFCharMasteryCardSelectDlg::SetNoticeText(int cardType) {
	if (m_noticeText == NULL) {
		return;
	}
	std::string typeName("");
	typeName += IsMasteryCardSpecialSlot(m_slotIndex) ? Jp_GetTableText(0x1884b) : "";
	switch (cardType) {
	case 0:
		typeName += "'";
		typeName += Jp_LookupText("\xc4\xb3\xb8\xaf\xc5\xcd");
		typeName += "'";
		break;
	case 1:
		typeName += "'";
		typeName += Jp_LookupText("\xc4\xb3\xb5\xf0");
		typeName += "'";
		break;
	case 5:
		typeName += "'NPC'";
		break;
	}
	std::string text(MakeStr(Jp_GetTableText(0x1884c), typeName.c_str()));
	m_noticeText->SetText(ShiftJisToWString(text));
}

// 0x5baad0
void RFCardIcon::OnMouseEnter(EventArgs&) {
	if (m_locked && !m_lockInfoHidden && g_charMasteryCardDlg != NULL) {
		std::string text(Jp_GetTableText(0x18850));
		g_charMasteryCardDlg->ShowCardTooltip(true, text.c_str());
	}
}

// 0x5baba0
void RFCardIcon::OnMouseLeave(EventArgs&) {
	if (!m_locked || m_lockInfoHidden) {
		return;
	}
	if (g_charMasteryCardDlg != NULL) {
		g_charMasteryCardDlg->ShowCardTooltip(false, NULL);
	}
}

// 0x5babd0
RFCharMasteryCardNoticeDlg* CreateMasteryCardNoticeDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharMasteryCardNoticeDlg>(&RFManager::GetInstance(), L"mastery_card_notice_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharMasteryCardNoticeDlg>(&RFManager::GetInstance(), L"mastery_card_notice_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharMasteryCardNoticeDlg>(&RFManager::GetInstance(), L"mastery_card_notice_dialog.xml");
		break;
	}
	RFCharMasteryCardNoticeDlg::s_loadParent = parent;
	return static_cast<RFCharMasteryCardNoticeDlg*>(wnd);
}

// 0x5bad10
void RFCharMasteryCardNoticeDlg::OnInitialUpdate() {
	RegisterTo(L"ok_btn", RFButton::EventClicked, Subscriber(&RFCharMasteryCardNoticeDlg::OnOkClicked, this));
	m_cardImg = dynamic_cast<RFPicture*>(GetChild(L"card_img"));
	m_message = dynamic_cast<RFTextArea*>(GetChild(L"message"));
	PlayMasteryCardSound("ui_dialog_open");
}

// 0x5baf20
RFCharMasteryCardSelectColumn* CreateMasteryCardSelectColumn(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharMasteryCardSelectColumn>(&RFManager::GetInstance(), L"kam_mastery_card_select_column.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharMasteryCardSelectColumn>(&RFManager::GetInstance(), L"kam_mastery_card_select_column.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharMasteryCardSelectColumn>(&RFManager::GetInstance(), L"kam_mastery_card_select_column.xml");
		break;
	}
	RFCharMasteryCardSelectColumn::s_loadParent = parent;
	return static_cast<RFCharMasteryCardSelectColumn*>(wnd);
}

// 0x5bb060
RFCharMasteryCardSelectDlg* CreateMasteryCardSelectDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharMasteryCardSelectDlg>(&RFManager::GetInstance(), L"kam_mastery_card_select_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharMasteryCardSelectDlg>(&RFManager::GetInstance(), L"kam_mastery_card_select_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharMasteryCardSelectDlg>(&RFManager::GetInstance(), L"kam_mastery_card_select_dialog.xml");
		break;
	}
	RFCharMasteryCardSelectDlg::s_loadParent = parent;
	return static_cast<RFCharMasteryCardSelectDlg*>(wnd);
}

// 0x5bb200
void RFCharMasteryCardSelectDlg::SelectCard(const JpCardEntry* card) {
	m_selectedCard = card;
	if (m_cardImg != NULL && card != NULL) {
		std::string name(MakeStr("%s.png", JpCardData_GetImageName(Jp_FindCard(card->m_typeId))));
		m_cardImg->SetPicture(ShiftJisToWString(name));
		PlayAnimation(L"_open_card_img_");
	}
}

// 0x5bb360
void RFCharMasteryCardSelectDlg::SetCardTypeTitle(int cardType) {
	if (m_cardTypeTitle == NULL) {
		return;
	}
	if (cardType == 0) {
		m_cardTypeTitle->SetPicture(L"title_charater_card.tga");
	} else if (cardType == 1) {
		m_cardTypeTitle->SetPicture(L"title_caddie_card.tga");
	} else if (cardType == 5) {
		m_cardTypeTitle->SetPicture(L"title_npc_card.tga");
	}
}

// 0x5bb460
RFCharacterMasteryCardDetachDlg* CreateMasteryCardDetachDialog(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharacterMasteryCardDetachDlg>(&RFManager::GetInstance(), L"kam_card_deselect_dialog.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharacterMasteryCardDetachDlg>(&RFManager::GetInstance(), L"kam_card_deselect_dialog.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharacterMasteryCardDetachDlg>(&RFManager::GetInstance(), L"kam_card_deselect_dialog.xml");
		break;
	}
	RFCharacterMasteryCardDetachDlg::s_loadParent = parent;
	return static_cast<RFCharacterMasteryCardDetachDlg*>(wnd);
}

// 0x5bb5a0
RFCardIcon* CreateMasteryCardIcon(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCardIcon>(&RFManager::GetInstance(), L"kam_mastery_card_thumb.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCardIcon>(&RFManager::GetInstance(), L"kam_mastery_card_thumb.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCardIcon>(&RFManager::GetInstance(), L"kam_mastery_card_thumb.xml");
		break;
	}
	RFCardIcon::s_loadParent = parent;
	return static_cast<RFCardIcon*>(wnd);
}

// 0x5bb6e0
RFWnd* CreateCharMasteryCardDlg(RFWnd* parent) {
	RFWnd* wnd = NULL;
	switch (g_jpWndCreateMode) {
	case 0:
		wnd = CreateWnd<RFCharMasteryCardDlg>(&RFManager::GetInstance(), L"kam_mastery_card.xml", parent);
		break;
	case 1:
		wnd = CreateWnd<RFCharMasteryCardDlg>(&RFManager::GetInstance(), L"kam_mastery_card.xml", NULL);
		break;
	case 2:
		wnd = CreateDesktopWnd<RFCharMasteryCardDlg>(&RFManager::GetInstance(), L"kam_mastery_card.xml");
		break;
	}
	RFCharMasteryCardDlg::s_loadParent = parent;
	return wnd;
}

// 0x5bb820
void ShowMasteryCardNotice(RFWnd* parent, uint32_t typeId, bool attached) {
	if (g_masteryCardDetachDlg != NULL) {
		g_masteryCardDetachDlg->Close(static_cast<RFWNDCLOSESTATE>(2));
	}
	if (g_masteryCardSelectDlg != NULL) {
		g_masteryCardSelectDlg->Close(static_cast<RFWNDCLOSESTATE>(2));
	}
	if (g_masteryCardNoticeDlg != NULL) {
		return;
	}
	RFCharMasteryCardNoticeDlg* dlg = CreateMasteryCardNoticeDialog(parent);
	if (dlg != NULL) {
		dlg->SetCard(typeId, attached);
		dlg->ShowModal();
	}
}

// 0x5bb880
void RFCharMasteryCardDlg::CreateCardSlots() {
	for (int i = 0; i < kCardSlotCount; ++i) {
		std::string narrow(MakeStr("slot_wnd%d", i + 1));
		RFWnd* placeholder = GetChild(ShiftJisToWString(narrow));
		if (placeholder != NULL) {
			m_cardIcons[i] = CreateMasteryCardIcon(this);
			RFVector2 pos = placeholder->GetPos();
			m_cardIcons[i]->SetPos(pos.x, pos.y);
			m_cardIcons[i]->ShowForm(true);
			DeleteChild(placeholder);
		}
	}
}

// Inlined into std::for_each 0x5bb9e0 (SetSlot's list walk).
void RFCharMasteryCardSelectDlg::AddCardColumn::operator()(const JpCardEntry& entry) const {
	if (entry.m_typeId != 0 && entry.m_count > 0 && *cardType == static_cast<int>((entry.m_typeId >> 22) & 0xf)) {
		RFCharMasteryCardSelectColumn* column = CreateMasteryCardSelectColumn(dlg->m_cardList);
		column->SetCard(&entry);
		column->ShowForm(true);
		dlg->m_cardList->InsertWindowItem(column);
	}
}

// 0x5bba60
void RFCharMasteryCardSelectColumn::OnClicked(EventArgs&) {
	if (g_masteryCardSelectDlg != NULL) {
		g_masteryCardSelectDlg->SelectCard(m_card);
	}
	PlayMasteryCardSound("ui_icon_click");
}

// 0x5bbab0
void RFCharMasteryCardSelectDlg::OnOkClicked(EventArgs&) {
	if (m_selectState) {
		return;
	}
	if (m_selectedCard != NULL) {
		JpMastery_PostTaskWait();
		m_selectState = true;
		JpMasteryCard_SendCardPacket(IsMasteryCardSpecialSlot(m_slotIndex) ? 0x18b : 0x18a,
			m_selectedCard->m_typeId, m_selectedCard->m_guid,
			MasteryCardSlotToServerSlot(m_slotIndex) + 1);
	}
	PlayMasteryCardSound("ui_button_ok_click");
}

// 0x5bbc50
void RFCharacterMasteryCardDetachDlg::OnUseItemOkClicked(EventArgs&) {
	const sItemInfo* item = Jp_GameState_FindInventoryItem(0x1a0000c2);
	if (JpMasteryCard_StateListNotEmpty() && item != NULL) {
		JpMastery_PostTaskWait();
		JpMasteryCard_SendCardPacket(0x18c, item->m_typeId, item->m_serial,
			MasteryCardSlotToServerSlot(m_slotIndex) + 1);
	}
}

// 0x5bbdb0
void RFCharMasteryCardDlg::SetCharacterUid(uint32_t uid) {
	if (uid == 0xffffffffu) {
		return;
	}
	const sCharacterInfo* data = Jp_GameState_CharacterData(uid);
	if (data == NULL) {
		return;
	}
	bool hideLockInfo = Jp_GameState_CharacterHasMasteryPart(data);
	for (int slot = 1; slot <= kCardSlotCount; ++slot) {
		RFCardIcon* icon = m_cardIcons[slot - 1];
		if (icon != NULL) {
			icon->SetCard(Jp_FindCard(JpCharacterData_GetCardTypeId(data, MasteryCardSlotToServerSlot(slot))), slot);
			icon->SetLocked(IsMasteryCardLockableSlot(slot), hideLockInfo);
			icon->UpdateView();
		}
	}
}

// 0x5bbe70
void RFCharMasteryCardDlg::OnInitialUpdate() {
	CreateCardSlots();
	m_tooltip = RFMasteryToolTipWnd::Load(GetSuper(false));
}

// 0x5bbec0 (body 0x5bbf08)
void RFCharMasteryCardSelectColumn::OnInitialUpdate() {
	RegisterTo(L"column_body", RFButton::EventClicked, Subscriber(&RFCharMasteryCardSelectColumn::OnClicked, this));
	m_cardImg = dynamic_cast<RFPicture*>(GetChild(L"card_img"));
	m_cardName = dynamic_cast<RFLabel*>(GetChild(L"card_name"));
}

// 0x5bc080
void RFCharMasteryCardSelectDlg::SetSlot(int slotIndex) {
	m_slotIndex = slotIndex;
	int cardType = MasteryCardSlotToCardType(slotIndex);
	std::list<JpCardEntry>& cards = Jp_GameState_CardList();
	AddCardColumn adder = { &cardType, this };
	std::for_each(cards.begin(), cards.end(), adder);
	SetCardTypeTitle(cardType);
	UpdateUseItem();
	SetNoticeText(cardType);
	UpdateOkButton();
}

// 0x5bc0f0
void RFCharMasteryCardSelectDlg::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventKeyDown, Subscriber(&RFCharMasteryCardSelectDlg::OnKey, this));
	RegisterTo(L"ok_btn", RFButton::EventClicked, Subscriber(&RFCharMasteryCardSelectDlg::OnOkClicked, this));
	RegisterTo(L"cancel_btn", RFButton::EventClicked, Subscriber(&RFCharMasteryCardSelectDlg::OnCancelClicked, this));
	m_cardList = dynamic_cast<RFListSC*>(GetChild(L"card_list"));
	m_cardTypeTitle = dynamic_cast<RFPicture*>(GetChild(L"title_card_type_img"));
	m_cardImg = dynamic_cast<RFPicture*>(GetChild(L"card_img"));
	m_useItemImg = dynamic_cast<RFPicture*>(GetChild(L"use_item_img"));
	m_useItemText = dynamic_cast<RFTextArea*>(GetChild(L"use_item_text"));
	m_noticeText = dynamic_cast<RFTextArea*>(GetChild(L"notice_text"));
	m_warningText = dynamic_cast<RFTextArea*>(GetChild(L"warnning_text"));
	m_okButton = dynamic_cast<RFButton*>(GetChild(L"ok_btn"));
	PlayMasteryCardSound("ui_dialog_open");
}

// 0x5bc6b0
void RFCharacterMasteryCardDetachDlg::OnDetachClicked(EventArgs&) {
	if (m_slotIndex == -1 || m_cardOriginImg == NULL) {
		return;
	}
	IFF_STRUCT::sItem* record = ItemManager()->FindItem(0x1a0000c2);
	if (record == NULL) {
		return;
	}
	const sItemInfo* item = Jp_GameState_FindInventoryItem(record->m_typeId);
	if (item != NULL && item->m_count >= 1) {
		RFUseItemDlg* dlg = static_cast<RFUseItemDlg*>(CreateUseItemDialog(this));
		std::string text(MakeStr(Jp_GetTableText(0x1884d), record->m_name));
		dlg->SetItem(item);
		dlg->RegisterTo(L"ok_btn", RFButton::EventClicked, Subscriber(&RFCharacterMasteryCardDetachDlg::OnUseItemOkClicked, this));
		dlg->SetMessage(text.c_str());
		dlg->ShowModal();
	} else {
		RFNoticeDlg* dlg = static_cast<RFNoticeDlg*>(CreateNoticeDialog(this));
		std::string text(MakeStr(Jp_GetTableText(0x1884e), record->m_name));
		dlg->SetMessage(text.c_str());
		dlg->ShowModal();
	}
}

// 0x5bc880
void RFCardIcon::OnClicked(EventArgs&) {
	if (m_card == NULL) {
		if (m_locked && !m_lockInfoHidden) {
			RFNoticeDlg* dlg = static_cast<RFNoticeDlg*>(CreateNoticeDialog(this));
			if (dlg != NULL) {
				std::string text(Jp_GetTableText(0x1884f));
				dlg->SetMessage(text.c_str());
				dlg->ShowModal();
			}
		} else if (g_masteryCardSelectDlg == NULL) {
			RFCharMasteryCardSelectDlg* dlg = CreateMasteryCardSelectDialog(this);
			if (dlg != NULL) {
				dlg->SetSlot(m_slotIndex);
				dlg->ShowModal();
			}
		}
	} else if (g_masteryCardDetachDlg == NULL) {
		RFCharacterMasteryCardDetachDlg* dlg =
			CreateMasteryCardDetachDialog(s_loadParent->GetSuper(false));
		if (dlg != NULL) {
			dlg->SetCard(m_card, m_slotIndex);
			dlg->ShowModal();
		}
	}
}

// 0x5bc9a0
void RFCharacterMasteryCardDetachDlg::OnInitialUpdate() {
	RegisterTo(L"", RFWnd::EventKeyDown, Subscriber(&RFCharacterMasteryCardDetachDlg::OnKey, this));
	RegisterTo(L"detach_btn", RFButton::EventClicked, Subscriber(&RFCharacterMasteryCardDetachDlg::OnDetachClicked, this));
	RegisterTo(L"close_btn", RFButton::EventClicked, Subscriber(&RFCharacterMasteryCardDetachDlg::OnCloseClicked, this));
	m_cardOriginImg = dynamic_cast<RFPicture*>(GetChild(L"card_origin_img"));
	PlayMasteryCardSound("ui_dialog_open");
}

// 0x5bccc0
void RFCardIcon::OnInitialUpdate() {
	RegisterTo(L"frame_btn", RFButton::EventClicked, Subscriber(&RFCardIcon::OnClicked, this));
	RegisterTo(L"frame_btn", RFWnd::EventMouseEnter, Subscriber(&RFCardIcon::OnMouseEnter, this));
	RegisterTo(L"frame_btn", RFWnd::EventMouseLeave, Subscriber(&RFCardIcon::OnMouseLeave, this));
	m_cardImg = dynamic_cast<RFPicture*>(GetChild(L"card_img"));
	m_cardImgNot = dynamic_cast<RFPicture*>(GetChild(L"card_img_not"));
	m_cardBlank = dynamic_cast<RFPicture*>(GetChild(L"card_blank"));
}
