// JP-only game code: FrCardComposeDlg, the JP card compose dialog
// ("card_compose"). See JpCardComposeDlg.h.
#include "JpCardComposeDlg.h"

#include "ItemManager.h"
#include "JpAlchemyDlg.h"
#include "JpLobbyGlue.h"
#include "JpMasteryCard.h"
#include "JpMasterySlotAni.h"
#include "TaskMain.h"
#include "Packet.h"
#include "RFLobbyDesktop.h"
#include "SharedDoc.h"
#include "RFTextManager.h"
#include "TaskManager.h"
#include "actor.h"
#include "avatar_avatar.h"
#include "binstr.h"
#include "bitmap.h"
#include "commonutil.h"
#include "frarea.h"
#include "frbutton.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frlistbox.h"
#include "frscrollbar.h"
#include "frtextbutton.h"
#include "frviewer.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "fxsequence.h"
#include "wfont.h"
#include "woverlay.h"
#include "wresrcmng.h"
#include "wview.h"

#include <algorithm>
#include <string.h>
#include <time.h>

// lib: 0x48de70, 0x48de80 (this-adjusting thunks), 0xcc7df0 (message-map
// initializer), 0x48d460, 0x48d530 (std::map clear / erase), 0x488920 /
// 0x488740 (CardComposeImageCache's implicit constructor / destructor),
// 0x487370 (CardComposeImageSlot's implicit destructor), 0x487220
// (std::for_each with ReleaseOverlay), 0x488230, 0x488840, 0x487460,
// 0x488290 (CardComposeImageCache's std::map clear / operator[] /
// equal_range / erase), 0x48d280 (std::string::append)
// 0x48ec90, 0x48eca0 (FrCardComposeResultDlg's this-adjusting thunks),
// 0xcc84f0 (FrCardComposeResultDlg::m_RTTI's dynamic initializer), 0xcc8530
// (its message-map initializer), 0x48e9b0 / 0x48eae0 (the CPvsFxBox
// constructor / destructor copies its constructor / destructor call),
// 0x48ee50 (CreateForm<FrCardComposeResultDlg>, frform.h, called by
// OpenResultDlg)
// Not written: the static registration of Create under "FrCardComposeDlg"
// with the JP form registry. The CTaskMain factory 0x654970 is the
// CreateForm<FrCardComposeDlg> instantiation (frform.h) the opener uses.

const WRTTI FrCardComposeAboutDlg::m_RTTI("FrCardComposeAboutDlg", &FrForm::m_RTTI);
const WRTTI FrCardComposeCheckDlg::m_RTTI("FrCardComposeCheckDlg", &FrForm::m_RTTI);
const WRTTI FrCardComposeDlg::m_RTTI("FrCardComposeDlg", &FrForm::m_RTTI);
const WRTTI FrCardComposeResultDlg::m_RTTI("FrCardComposeResultDlg", &FrForm::m_RTTI);

// 0xf02430: the game camera (as JpAlchemyDlg.cpp).
extern WMatrix g_camera;

sFRESH_MSGMAP FrCardComposeAboutDlg::_MsgMap = { &FrForm::_MsgMap, &FrCardComposeAboutDlg::_MsgEntries[0] };

sFRESH_ENTRY FrCardComposeAboutDlg::_MsgEntries[] = {
	{ "about_viewer", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeAboutDlg::OnAboutViewerInit },
	{ "index01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeAboutDlg::OnIndex01Init },
	{ "index01", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeAboutDlg::OnIndex01LBtnUp },
	{ "index02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeAboutDlg::OnIndex02Init },
	{ "index02", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeAboutDlg::OnIndex02LBtnUp },
	{ "index03", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeAboutDlg::OnIndex03Init },
	{ "index03", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeAboutDlg::OnIndex03LBtnUp },
	{ "index04", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeAboutDlg::OnIndex04Init },
	{ "index04", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeAboutDlg::OnIndex04LBtnUp },
	{ "index05", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeAboutDlg::OnIndex05Init },
	{ "index05", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeAboutDlg::OnIndex05LBtnUp },
	{ "index06", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeAboutDlg::OnIndex06Init },
	{ "index06", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeAboutDlg::OnIndex06LBtnUp },
	{ "backward", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeAboutDlg::OnBackwardInit },
	{ "backward", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeAboutDlg::OnBackwardLBtnUp },
	{ "forward", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeAboutDlg::OnForwardInit },
	{ "forward", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeAboutDlg::OnForwardLBtnUp },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

sFRESH_MSGMAP FrCardComposeCheckDlg::_MsgMap = { &FrForm::_MsgMap, &FrCardComposeCheckDlg::_MsgEntries[0] };

sFRESH_ENTRY FrCardComposeCheckDlg::_MsgEntries[] = {
	{ "card_compose_viewer1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeCheckDlg::OnViewer1Init },
	{ "card_compose_viewer2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeCheckDlg::OnViewer2Init },
	{ "card_compose_viewer3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeCheckDlg::OnViewer3Init },
	{ "accept_compose", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeCheckDlg::OnAcceptComposeLBtnUp },
	{ "cancel_compose", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeCheckDlg::OnCancelComposeLBtnUp },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

sFRESH_MSGMAP FrCardComposeResultDlg::_MsgMap = { &FrForm::_MsgMap, &FrCardComposeResultDlg::_MsgEntries[0] };

sFRESH_ENTRY FrCardComposeResultDlg::_MsgEntries[] = {
	{ "synthesized_card", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeResultDlg::OnSynthesizedCardInit },
	{ "fx_player", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeResultDlg::OnFxPlayerInit },
	{ "fx_player", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrCardComposeResultDlg::OnFxPlayerOwnerDraw },
	{ "o", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeResultDlg::OnOInit },
	{ "o", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeResultDlg::OnOLBtnUp },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x486720
bool FrCardComposeDlg::AreSlotsFull(void) const {
	return m_slotCount == 3;
}

// The compose checks of CheckComposePrice (0x487960). The original's are
// members of the slot-state sub-object; they read only their arguments and
// the JP CSharedDoc.

// 0x486730
// Tests the array pointer itself, not the ids (the original's).
static bool HasSlotCardIds(const unsigned int* cardIds) {
	for (int i = 0; i < 3; i++) {
		if (cardIds == 0) {
			return false;
		}
	}
	return true;
}

// 0x486750
// No slot card of kind 3 (type id bits 22-25).
static bool HasNoKind3Card(const unsigned int* cardIds) {
	for (int i = 0; i < 3; i++) {
		if ((cardIds[i] & 0x3c00000) == 0xc00000) {
			return false;
		}
	}
	return true;
}

// 0x486790
// No slot card of kind 4 (type id bits 22-25).
static bool HasNoKind4Card(const unsigned int* cardIds) {
	for (int i = 0; i < 3; i++) {
		if ((cardIds[i] & 0x3c00000) == 0x1000000) {
			return false;
		}
	}
	return true;
}

// 0x486f70
void FrCardComposeDlg::SendComposeRequest(unsigned long long price, const unsigned int* cardIds) {
	WSendPacket packet(0x155);
	packet.Encode8(price);
	for (int i = 0; i < 3; i++) {
		packet.Encode4(cardIds[i]);
	}
	packet.Send(SEND_TO_DEFAULT);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
}

// 0x487050
// The player's pang (JP CSharedDoc +0x91e) covers `price`.
static bool HasPangFor(const long long* price) {
	if (Jp_GameState_Pang() >= *price) {
		return true;
	}
	return false;
}

// 0x487080
// No slot card of rarity 3 (the card record's +0xc0, KR m_grade).
static bool HasNoRarity3Card(const unsigned int* cardIds) {
	for (int i = 0; i < 3; i++) {
		if (ItemManager()->FindCard(cardIds[i])->m_grade == 3) {
			return false;
		}
	}
	return true;
}

// 0x4870d0
long long FrCardComposeDlg::GetComposePrice(void) const {
	unsigned long long price = 0;
	for (int i = 0; i < 3; i++) {
		if (m_slotCardId[i] == 0) {
			continue;
		}
		switch (ItemManager()->FindCard(m_slotCardId[i])->m_grade) {
		case 0:
			price += 1000;
			break;
		case 1:
			price += 2000;
			break;
		case 2:
			price += 5000;
			break;
		}
	}
	return price;
}

// 0x4877f0
// Each slot card is held (JP CSharedDoc +0x37e0) at least as many times as
// the slots name it.
static bool HasHeldSlotCards(const unsigned int* cardIds) {
	int needed[3] = { 0, 0, 0 };
	for (int i = 0; i < 3; i++) {
		if (needed[i] == -1) {
			continue;
		}
		for (int j = i + 1; j < 3; j++) {
			if (cardIds[i] == cardIds[j]) {
				needed[j] = -1;
				needed[i]++;
			}
		}
		std::list<JpCardEntry>& cards = Jp_GameState_CardList();
		for (std::list<JpCardEntry>::iterator it = cards.begin(); it != cards.end(); ++it) {
			if (it->m_typeId == cardIds[i]) {
				needed[i]++;
				if (it->m_count < needed[i]) {
					return false;
				}
			}
		}
	}
	return true;
}

// 0x487960
bool FrCardComposeDlg::CheckComposePrice(long long& price) {
	if (!AreSlotsFull()) {
		return false;
	}
	if (!HasPangFor(&price)) {
		return false;
	}
	if (!HasSlotCardIds(m_slotCardId)) {
		return false;
	}
	if (!HasNoRarity3Card(m_slotCardId)) {
		return false;
	}
	if (!HasNoKind3Card(m_slotCardId)) {
		return false;
	}
	if (!HasNoKind4Card(m_slotCardId)) {
		return false;
	}
	if (!HasHeldSlotCards(m_slotCardId)) {
		return false;
	}
	return true;
}

// CardComposeImageCache::Clear's std::for_each functor (inlined into
// 0x487220).
struct ReleaseOverlay {
	void operator()(std::pair<const std::string, WOverlay*>& entry) const {
		if (g_resrcmng != NULL && entry.second != NULL) {
			g_resrcmng->Release(entry.second);
			entry.second = NULL;
		}
	}
};

// 0x4885d0
void CardComposeImageCache::Clear(void) {
	std::for_each(m_overlays.begin(), m_overlays.end(), ReleaseOverlay());
	m_overlays.clear();
}

// 0x4889f0
void CardComposeImageCache::Add(const char* name, WOverlay* overlay) {
	int oldest = 0;
	int64_t oldestTime = time(NULL);
	for (int i = 0; i < 20; i++) {
		if (m_slots[i].m_lastUse == 0) {
			oldest = i;
			break;
		}
		if (m_slots[i].m_lastUse < oldestTime) {
			oldest = i;
			oldestTime = m_slots[i].m_lastUse;
		}
	}
	CardComposeImageSlot& slot = m_slots[oldest];
	if (slot.m_lastUse != 0) {
		delete m_overlays[slot.m_name];
		m_overlays.erase(slot.m_name);
	}
	slot.m_name = name;
	slot.m_lastUse = time(NULL);
	m_overlays[std::string(name)] = overlay;
}

// 0x488b60
WOverlay* CardComposeImageCache::Load(const char* name) {
	WOverlay* overlay = m_overlays[std::string(name)];
	if (overlay == NULL) {
		overlay = g_resrcmng->GetOverlay(name, 0);
		if (overlay != NULL) {
			Add(name, overlay);
		}
		return overlay;
	}
	for (int i = 0; i < 20; i++) {
		if (m_slots[i].m_name == name) {
			m_slots[i].m_lastUse = time(NULL);
			break;
		}
	}
	return overlay;
}

// 0x488d60
const sFRESH_MSGMAP* FrCardComposeDlg::GetMessageMap(void) const {
	return &FrCardComposeDlg::_MsgMap;
}

sFRESH_MSGMAP FrCardComposeDlg::_MsgMap = { &FrForm::_MsgMap, &FrCardComposeDlg::_MsgEntries[0] };

sFRESH_ENTRY FrCardComposeDlg::_MsgEntries[] = {
	{ "my_card_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnMyCardListInit },
	{ "my_card_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnMyCardListOwnerDraw },
	{ "my_card_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnMyCardListLBtnDown },
	{ "compose_card", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnComposeCardInit },
	{ "compose_card", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnComposeCardLBtnDown },
	{ "compose_card_btn1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnComposeCardBtn1Init },
	{ "compose_card_btn1", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnComposeCardBtn1LBtnUp },
	{ "compose_card_btn1", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnComposeCardBtn1OwnerDraw },
	{ "compose_card_btn2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnComposeCardBtn2Init },
	{ "compose_card_btn2", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnComposeCardBtn2LBtnUp },
	{ "compose_card_btn2", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnComposeCardBtn2OwnerDraw },
	{ "compose_card_btn3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnComposeCardBtn3Init },
	{ "compose_card_btn3", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnComposeCardBtn3LBtnUp },
	{ "compose_card_btn3", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnComposeCardBtn3OwnerDraw },
	{ "about", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnAboutInit },
	{ "about", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnAboutLBtnDown },
	{ "whole_card_button", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnWholeCardButtonInit },
	{ "whole_card_button", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnWholeCardButtonLBtnDown },
	{ "normal_card_button", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnNormalCardButtonInit },
	{ "normal_card_button", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnNormalCardButtonLBtnDown },
	{ "rare_card_button", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnRareCardButtonInit },
	{ "rare_card_button", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnRareCardButtonLBtnDown },
	{ "super_rare_card_button", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnSuperRareCardButtonInit },
	{ "super_rare_card_button", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnSuperRareCardButtonLBtnDown },
	{ "print_pang_area", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnPrintPangAreaInit },
	{ "print_pang_area", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnPrintPangAreaOwnerDraw },
	{ "MagicBox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnMagicBoxInit },
	{ "MagicBox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnMagicBoxLBtnUp },
	{ "CardCompose", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnCardComposeInit },
	{ "CardCompose", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnCardComposeLBtnUp },
	{ "Exchange", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrCardComposeDlg::OnExchangeInit },
	{ "Exchange", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrCardComposeDlg::OnExchangeLBtnUp },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x488d70
int FrCardComposeDlg::GetCardKindOrder(const IFF_STRUCT::sCard* card) {
	int kind = (card->m_typeId >> 22) & 0xf;
	if (kind == 2) {
		return 0;
	}
	if (kind == 0) {
		return 1;
	}
	if (kind == 1) {
		return 2;
	}
	if (kind == 5) {
		return 3;
	}
	return -1;
}

// 0x488db0
bool FrCardComposeDlg::CompareCardItems(const void* a, const void* b) {
	const CardComposeCard* cardA = (const CardComposeCard*)a;
	const CardComposeCard* cardB = (const CardComposeCard*)b;
	if (cardA->m_iff.m_grade > cardB->m_iff.m_grade) {
		return false;
	}
	if (cardA->m_iff.m_grade < cardB->m_iff.m_grade) {
		return true;
	}
	int orderA = GetCardKindOrder(&cardA->m_iff);
	int orderB = GetCardKindOrder(&cardB->m_iff);
	if (orderA > orderB) {
		return false;
	}
	if (orderA < orderB) {
		return true;
	}
	return cardA->m_card.m_typeId <= cardB->m_card.m_typeId;
}

// 0x488e20
void FrCardComposeDlg::LoadBitmaps(void) {
	m_pCardIcon = g_pFresh->GetBitmap("cardmix_btn_list_my_o");
	m_pSlotIcon = g_pFresh->GetBitmap("cardmix_btn_slot_my_o");
	m_pSlotIconDown = g_pFresh->GetBitmap("cardmix_btn_slot_my_d");
}

// 0x488e70
void FrCardComposeDlg::InitCardImageSize(void) {
	m_cardImageRect.w = 21.5f;
	m_cardImageRect.h = 26.5f;
}

// 0x488e90
void FrCardComposeDlg::OnCardComposeLBtnUp(void) {
}

// 0x488ea0
int FrCardComposeDlg::FindFreeSlot(const CardComposeCard* card) {
	int count = card->m_card.m_count;
	if (count < 1) {
		return -1;
	}
	unsigned long typeId = card->m_card.m_typeId;
	int slot = -1;
	int used = 0;
	for (int i = 2; i >= 0; i--) {
		if (m_slotCardId[i] == typeId) {
			used++;
		}
		if (m_slotCardId[i] == 0) {
			slot = i;
		}
	}
	if (slot == -1 || count <= used) {
		return -1;
	}
	m_slotCardId[slot] = typeId;
	m_slotCount++;
	return slot;
}

// 0x488f30
bool FrCardComposeDlg::RemoveSlotCardData(int slot) {
	if (m_slotCardId[slot] == 0) {
		return false;
	}
	m_slotCardId[slot] = 0;
	m_slotCount--;
	return true;
}

// 0x488f60
int FrCardComposeDlg::CountSlotCard(const CardComposeCard* card) const {
	unsigned long typeId = card->m_card.m_typeId;
	int used = 0;
	if (m_slotCardId[0] == typeId) {
		used = 1;
	}
	if (m_slotCardId[1] == typeId) {
		used++;
	}
	if (m_slotCardId[2] == typeId) {
		used++;
	}
	return card->m_card.m_count - used;
}

// 0x488fa0
bool FrCardComposeDlg::SetResultCard(const unsigned int& cardId) {
	if (cardId == 0) {
		return false;
	}
	m_resultCardId = cardId;
	return true;
}

// 0x488fc0
void FrCardComposeDlg::RemoveSlotCard(int slot) {
	if (!RemoveSlotCardData(slot)) {
		return;
	}
	if (!AreSlotsFull()) {
		m_pComposeCard->Enable(false);
	}
	MakePriceStrA(m_priceText, GetComposePrice());
	m_slotCardView[slot] = 0;
	m_pSlotBtn[slot]->SetVisible(false);
}

// 0x489040
bool FrCardComposeDlg::OnCheckResult(int ret, FrForm* form) {
	if (ret == FrYES) {
		long long price = GetComposePrice();
		if (CheckComposePrice(price)) {
			SendComposeRequest(price, m_slotCardId);
			m_pComposeCard->Enable(false);
		}
	}
	m_pCheckDlg = 0;
	return true;
}

// 0x4890d0
const sFRESH_MSGMAP* FrCardComposeCheckDlg::GetMessageMap(void) const {
	return &FrCardComposeCheckDlg::_MsgMap;
}

// 0x4890e0
void FrCardComposeCheckDlg::OnAcceptComposeLBtnUp(void) {
	Close(FrYES, true);
}

// 0x4890f0
void FrCardComposeCheckDlg::OnCancelComposeLBtnUp(void) {
	Close(FrCANCEL, true);
}

// 0x489100
const sFRESH_MSGMAP* FrCardComposeAboutDlg::GetMessageMap(void) const {
	return &FrCardComposeAboutDlg::_MsgMap;
}

// 0x489110
FrCardComposeAboutDlg::FrCardComposeAboutDlg(void) {
	m_pIndex[0] = 0;
	m_pIndex[1] = 0;
	m_pIndex[2] = 0;
	m_pIndex[3] = 0;
	m_pIndex[4] = 0;
	m_pIndex[5] = 0;
	m_page = 0;
	m_pBackward = 0;
	m_pForward = 0;
}

// 0x489180
FrCardComposeAboutDlg::~FrCardComposeAboutDlg(void) {
}

// 0x489250
const sFRESH_MSGMAP* FrCardComposeResultDlg::GetMessageMap(void) const {
	return &FrCardComposeResultDlg::_MsgMap;
}

// 0x489260
void FrCardComposeResultDlg::SetOwnerDlg(FrCardComposeDlg* dlg) {
	m_pOwnerDlg = dlg;
}

// 0x489270
void FrCardComposeResultDlg::OnOLBtnUp(void) {
	OnFreshOkay();
}

// 0x489d30
void FrCardComposeDlg::InitCardIconSize(void) {
	if (m_pCardIcon) {
		m_cardIconRect.w = (float)m_pCardIcon->Width();
		m_cardIconRect.h = (float)m_pCardIcon->Height();
	}
}

// 0x489d80
void FrCardComposeDlg::InitSlotIconSize(void) {
	if (m_pSlotIcon) {
		m_slotIconRect.w = (float)m_pSlotIcon->Width();
		m_slotIconRect.h = (float)m_pSlotIcon->Height();
	}
}

// 0x489dd0
void FrCardComposeDlg::OnMyCardListInit(intptr_t param) {
	m_pMyCardList = DYNAMIC_CAST(FrListBox, param);
	m_pMyCardList->UseDblClick(false);
}

// 0x4891a0
void FrCardComposeAboutDlg::ShowPage(int page) {
	if ((unsigned int)page > 5) {
		return;
	}
	if (page == 0) {
		m_pBackward->Enable(false);
		m_pForward->Enable(true);
	} else if (page == 5) {
		m_pForward->Enable(false);
		m_pBackward->Enable(true);
	} else {
		m_pBackward->Enable(true);
		m_pForward->Enable(true);
	}
	for (int i = 0; i < 6; i++) {
		m_pIndex[i]->Unselect(0);
	}
	m_pIndex[page]->Select(0);
	if (m_pViewer) {
		m_pViewer->Open(MakeStr("help_cardmix_%02d.jpg", m_page + 1));
	}
}

// 0x489e50
void FrCardComposeDlg::OnPrintPangAreaInit(intptr_t param) {
	m_pPrintPangArea = DYNAMIC_CAST(FrArea, param);
}

// 0x489ec0
void FrCardComposeDlg::OnMagicBoxInit(intptr_t param) {
	m_pMagicBox = DYNAMIC_CAST(FrButton, param);
	if (m_pMagicBox) {
		m_pMagicBox->SetVisible(true);
	}
	if (m_pMagicBox) {
		m_pMagicBox->SetPushDelay(0.01f);
	}
	if (m_pMagicBox) {
		m_pMagicBox->EnableHover(true, 0.0f);
	}
}

// 0x489f70
void FrCardComposeDlg::OnCardComposeInit(intptr_t param) {
	m_pCardCompose = DYNAMIC_CAST(FrButton, param);
	if (m_pCardCompose) {
		m_pCardCompose->SetVisible(true);
	}
	if (m_pCardCompose) {
		m_pCardCompose->SetPushDelay(0.01f);
	}
	if (m_pCardCompose) {
		m_pCardCompose->EnableHover(true, 0.0f);
	}
}

// 0x48a020
void FrCardComposeDlg::OnExchangeInit(intptr_t param) {
	m_pExchange = DYNAMIC_CAST(FrButton, param);
	if (m_pExchange) {
		m_pExchange->SetVisible(true);
	}
	if (m_pExchange) {
		m_pExchange->SetPushDelay(0.01f);
	}
	if (m_pExchange) {
		m_pExchange->EnableHover(true, 0.0f);
	}
}

// 0x48a0d0
void FrCardComposeDlg::DrawOwnPang(FrGraphicInterface* gdi) {
	size_t len = strlen(m_ownPangText);
	const WRect& rect = m_pPrintPangArea->GetRect();
	float y = rect.y + 5.0;
	float width = (float)((m_pPriceFont->GetFontInfo()->fontw - 1.0) * (len - 1));
	float x = rect.x + 155.0 - width;
	m_pPriceFont->Print(g_view, x, y, m_ownPangText, 0, 0xffffffff, NULL);
}

// 0x48a160
void FrCardComposeDlg::DrawComposePrice(FrGraphicInterface* gdi) {
	size_t len = strlen(m_priceText);
	const WRect& rect = m_pPrintPangArea->GetRect();
	float y = rect.y + 31.0;
	float width = (float)((m_pPriceFont->GetFontInfo()->fontw - 1.0) * (len - 1));
	float x = rect.x + 155.0 - width;
	m_pPriceFont->Print(g_view, x, y, m_priceText, 0, 0xffffffff, NULL);
}

// 0x48a1f0
void FrCardComposeDlg::OnComposeCardInit(intptr_t param) {
	m_pComposeCard = DYNAMIC_CAST(FrButton, param);
	m_pComposeCard->Enable(false);
}

// 0x48a260
void FrCardComposeDlg::DrawCardImage(FrListItem* item, FrGraphicInterface* gdi, const CardComposeCard* card) {
	const Bitmap* bitmap = g_pFresh->GetBitmap(card->m_iff.m_icon);
	if (bitmap) {
		m_cardImageRect.x = m_pMyCardList->GetRect().x + 10.0;
		m_cardImageRect.y = item->pos.y + 2.5;
		gdi->DrawTexture(bitmap, m_cardImageRect, 0xffffffff, 0);
	}
}

// 0x48a2c0
void FrCardComposeDlg::DrawCardName(FrListItem* item, const CardComposeCard* card) {
	WPoint pos;
	pos.x = item->pos.x + 35.0;
	pos.y = item->pos.y + 10.0;
	g_pFresh->GetManager()->PrintText(pos, 0, card->m_iff.m_name, 102.0f, 0xffffffff);
}

// 0x48a310
void FrCardComposeDlg::DrawCardCount(FrListItem* item, int count) {
	WPoint pos;
	pos.x = item->pos.x + 161.0;
	pos.y = item->pos.y + 10.0;
	g_pFresh->GetManager()->PrintText(pos, 2, MakeStr("x%d", count), -1.0f, 0xffffffff);
}

// 0x48a370
void FrCardComposeDlg::OnComposeCardBtn1Init(intptr_t param) {
	m_pSlotBtn[0] = DYNAMIC_CAST(FrButton, param);
	m_pSlotBtn[0]->SetVisible(false);
}

// 0x48a3e0
void FrCardComposeDlg::OnComposeCardBtn1LBtnUp(void) {
	RemoveSlotCard(0);
}

// 0x48a3f0
void FrCardComposeDlg::OnComposeCardBtn2Init(intptr_t param) {
	m_pSlotBtn[1] = DYNAMIC_CAST(FrButton, param);
	m_pSlotBtn[1]->SetVisible(false);
}

// 0x48a460
void FrCardComposeDlg::OnComposeCardBtn2LBtnUp(void) {
	RemoveSlotCard(1);
}

// 0x48a470
void FrCardComposeDlg::OnComposeCardBtn3Init(intptr_t param) {
	m_pSlotBtn[2] = DYNAMIC_CAST(FrButton, param);
	m_pSlotBtn[2]->SetVisible(false);
}

// 0x48a4e0
void FrCardComposeDlg::OnComposeCardBtn3LBtnUp(void) {
	RemoveSlotCard(2);
}

// 0xe428fc: the whole texture.
static const _WRECT s_wholeTexture = { 0.0f, 0.0f, 1.0f, 1.0f };

// 0x48a4f0
void FrCardComposeDlg::DrawSlotCardView(int slot) {
	FrButton* btn = m_pSlotBtn[slot];
	if (btn == 0) {
		return;
	}
	_WRECT dest;
	dest.x = btn->GetRect().x;
	dest.y = btn->GetRect().y;
	dest.x = dest.x + 1.5;
	dest.y = dest.y + 1.5;
	dest.w = 173.0f;
	dest.h = 352.0f;
	m_slotCardView[slot]->Render(g_view, s_wholeTexture, dest, 0, 0xffffffff, 0.0f, 0);
}

// 0x48a580
void FrCardComposeDlg::DrawSlotIcon(int slot, FrButton* btn, FrGraphicInterface* gdi) {
	m_slotIconRect.x = m_pSlotBtn[slot]->GetRect().x;
	m_slotIconRect.y = m_pSlotBtn[slot]->GetRect().y;
	if (btn->GetStatus() == FrButton::OVER) {
		gdi->DrawTexture(m_pSlotIcon, m_slotIconRect, 0xffffffff, 0);
	} else if (btn->GetStatus() == FrButton::PRESSED) {
		gdi->DrawTexture(m_pSlotIconDown, m_slotIconRect, 0xffffffff, 0);
	}
}

// 0x48a5f0
void FrCardComposeDlg::DrawSlotCard(int slot) {
	FrButton* btn = m_pSlotBtn[slot];
	if (btn == 0) {
		return;
	}
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0) {
		return;
	}
	if (m_slotCardView[slot] == 0) {
		return;
	}
	DrawSlotCardView(slot);
	if (btn->GetStatus() != FrButton::NORMAL) {
		DrawSlotIcon(slot, btn, gdi);
	}
}

// 0x48a650
void FrCardComposeDlg::OnAboutInit(intptr_t param) {
	m_pAbout = DYNAMIC_CAST(FrButton, param);
}

// 0x48a6c0
void FrCardComposeDlg::OnWholeCardButtonInit(intptr_t param) {
	m_pFilterBtn[3] = DYNAMIC_CAST(FrButton, param);
	m_pFilterBtn[3]->SetStatus(FrButton::PRESSED);
}

// 0x48a740
void FrCardComposeDlg::OnNormalCardButtonInit(intptr_t param) {
	m_pFilterBtn[0] = DYNAMIC_CAST(FrButton, param);
}

// 0x48a7b0
void FrCardComposeDlg::OnRareCardButtonInit(intptr_t param) {
	m_pFilterBtn[1] = DYNAMIC_CAST(FrButton, param);
}

// 0x48a820
void FrCardComposeDlg::OnSuperRareCardButtonInit(intptr_t param) {
	m_pFilterBtn[2] = DYNAMIC_CAST(FrButton, param);
}

// 0x48a890
void FrCardComposeCheckDlg::OnViewer1Init(intptr_t param) {
	m_pViewer[0] = DYNAMIC_CAST(FrViewer, param);
}

// 0x48a900
void FrCardComposeCheckDlg::OnViewer2Init(intptr_t param) {
	m_pViewer[1] = DYNAMIC_CAST(FrViewer, param);
}

// 0x48a970
void FrCardComposeCheckDlg::OnViewer3Init(intptr_t param) {
	m_pViewer[2] = DYNAMIC_CAST(FrViewer, param);
}

// 0x48a9e0
FrForm* FrCardComposeAboutDlg::Create(void) {
	return new FrCardComposeAboutDlg;
}

// 0x48aaa0
void FrCardComposeAboutDlg::OnAboutViewerInit(intptr_t param) {
	m_pViewer = DYNAMIC_CAST(FrViewer, param);
}

// 0x48ab10
void FrCardComposeAboutDlg::OnIndex01Init(intptr_t param) {
	m_pIndex[0] = DYNAMIC_CAST(FrTextButton, param);
}

// 0x48ab80
void FrCardComposeAboutDlg::OnIndex01LBtnUp(void) {
	m_page = 0;
	ShowPage(0);
}

// 0x48aba0
void FrCardComposeAboutDlg::OnIndex02Init(intptr_t param) {
	m_pIndex[1] = DYNAMIC_CAST(FrTextButton, param);
}

// 0x48ac10
void FrCardComposeAboutDlg::OnIndex02LBtnUp(void) {
	m_page = 1;
	ShowPage(1);
}

// 0x48ac30
void FrCardComposeAboutDlg::OnIndex03Init(intptr_t param) {
	m_pIndex[2] = DYNAMIC_CAST(FrTextButton, param);
}

// 0x48aca0
void FrCardComposeAboutDlg::OnIndex03LBtnUp(void) {
	m_page = 2;
	ShowPage(2);
}

// 0x48acc0
void FrCardComposeAboutDlg::OnIndex04Init(intptr_t param) {
	m_pIndex[3] = DYNAMIC_CAST(FrTextButton, param);
}

// 0x48ad30
void FrCardComposeAboutDlg::OnIndex04LBtnUp(void) {
	m_page = 3;
	ShowPage(3);
}

// 0x48ad50
void FrCardComposeAboutDlg::OnIndex05Init(intptr_t param) {
	m_pIndex[4] = DYNAMIC_CAST(FrTextButton, param);
}

// 0x48adc0
void FrCardComposeAboutDlg::OnIndex05LBtnUp(void) {
	m_page = 4;
	ShowPage(4);
}

// 0x48ade0
void FrCardComposeAboutDlg::OnIndex06Init(intptr_t param) {
	m_pIndex[5] = DYNAMIC_CAST(FrTextButton, param);
}

// 0x48ae50
void FrCardComposeAboutDlg::OnIndex06LBtnUp(void) {
	m_page = 5;
	ShowPage(5);
}

// 0x48ae70
void FrCardComposeAboutDlg::OnBackwardInit(intptr_t param) {
	m_pBackward = DYNAMIC_CAST(FrButton, param);
}

// 0x48aee0
void FrCardComposeAboutDlg::OnBackwardLBtnUp(void) {
	m_page--;
	ShowPage(m_page);
}

// 0x48af00
void FrCardComposeAboutDlg::OnForwardInit(intptr_t param) {
	m_pForward = DYNAMIC_CAST(FrButton, param);
}

// 0x48af70
void FrCardComposeAboutDlg::OnForwardLBtnUp(void) {
	m_page++;
	ShowPage(m_page);
}

// 0x48af90
bool FrCardComposeAboutDlg::OnInit(void) {
	ShowPage(0);
	ntAvatarManager* manager = WSingleton<ntAvatarManager>::Instance();
	if (manager) {
		manager->SetActive(false);
	}
	return FrForm::OnInit();
}

// 0x48afc0
void FrCardComposeResultDlg::OnSynthesizedCardInit(intptr_t param) {
	m_pSynthesizedCard = DYNAMIC_CAST(FrViewer, param);
	m_pSynthesizedCard->SetVisible(false);
}

// 0x48b030: the area is not NULL-tested (the original's).
void FrCardComposeResultDlg::OnFxPlayerInit(intptr_t param) {
	FrArea* area = DYNAMIC_CAST(FrArea, param);
	m_fxRect = area->GetRect();
	m_effect.SetScreenCenter(m_fxRect.x, m_fxRect.y);
}

// 0x48b0e0: 2.0f 0xcfea84, 4.0f 0xcfea80.
void FrCardComposeResultDlg::OnProc(const float dt) {
	FrForm::OnProc(dt);
	if (m_effect.Finished()) {
		return;
	}
	float ms = (float)(dt * 1000.0);
	m_effect.ProcessAttacher(ms);
	m_effect.Process(ms);
	if (m_showTime > 0.0f) {
		m_showTime += dt;
	}
	if (!m_pSynthesizedCard->IsVisible() && m_showTime > 2.0f) {
		m_pSynthesizedCard->SetVisible(true);
	}
	if (m_showTime > 4.0f) {
		m_pOk->Enable(true);
		m_showTime = -1.0f;
	}
}

// 0x48b1c0: FrAlchemyMPBreakDlg::OnExchangeOnEffectDraw 0x4f56a0's
// sequence with the clipping area set to fx_player's rect.
void FrCardComposeResultDlg::OnFxPlayerOwnerDraw(void) {
	if (m_effect.Finished()) {
		return;
	}
	bool fog = g_view->SetFogEnable(false);
	g_view->SetCamera(m_effectCamera);
	float clipNear = g_view->GetClipNearValue();
	float clipFar = g_view->GetClipFarValue();
	WView::PROJECTION_MODE mode = g_view->GetProjectionMode();
	// 0xb4f950: the JP GetFOV (out of line).
	float fov = g_view->GetFOV();
	g_view->SetProjectionMode(WView::PERSPECTIVE);
	g_view->SetFOV(1.5707964f);         // 0xcff0ac
	g_view->SetClip(0.5f, 50.0f, false);  // 0xcff0a8
	g_view->SetClippingArea(m_fxRect);  // 0xb52890
	g_view->UpdateCamera();
	m_effect.Display(g_view);
	// JP [4] takes a second argument (1 here) that the KR WView::Flush
	// does not have.
	g_view->Flush(1);
	g_view->SetProjectionMode(mode);
	g_view->SetClip(clipNear, clipFar, false);
	g_view->SetFOV(fov);
	g_view->ResetClippingArea();     // 0xb52890 on (0, 0, width, height)
	g_view->ResetScreenCenter();     // 0xb4fcf0 on (width / 2, height / 2)
	g_view->UpdateCamera();
	g_view->SetFogEnable(fog);
}

// 0x48b380: LoadSequence 0x9705a0, AddSequence 0x971340 (as
// FrAlchemyMPBreakDlg::StartBreakSequence 0x4f7d50).
void FrCardComposeResultDlg::PlaySequence(const char* name) {
	CFxSequence* sequence = m_effect.LoadSequence(name, true, NULL);
	if (sequence == NULL) {
		return;
	}
	FrViewer* viewer = m_pSynthesizedCard;
	if (viewer == NULL) {
		return;
	}
	float x = viewer->GetRect().w * 0.5 + viewer->GetRect().x;
	float y = viewer->GetRect().h * 0.5 + viewer->GetRect().y;
	Jp_FxSequence_SetScreenPos(sequence, (float)(int)x, (float)(int)y);
	Jp_FxSequence_SetPos(sequence, g_jpSlotAniSequencePos);
	m_effect.AddSequence(sequence);
}

// 0x48b420
void FrCardComposeResultDlg::OnOInit(intptr_t param) {
	m_pOk = DYNAMIC_CAST(FrButton, param);
	m_pOk->Enable(false);
}

// 0xe0416c: the price font's page.
static char* s_priceFontFiles[] = { const_cast<char*>("[font_bong.jpg") };

// 0x48bb90
void FrCardComposeDlg::InitPriceFont(void) {
	tagWTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = s_priceFontFiles;
	font.fontw = 9;
	font.fonth = 0x10;
	font.numPages = 1;
	font.texw = 0x80;
	font.texh = 0x20;
	font.pCharSet = const_cast<char*>("1234567890,-");
	m_pPriceFont = g_resrcmng->GetTitleFont();
	m_pPriceFont->Create(&font);
	MakePriceStrA(m_ownPangText, Jp_GameState_Pang());
	MakePriceStrA(m_priceText, 0);
}

// 0x48bc40
void FrCardComposeDlg::OnPrintPangAreaOwnerDraw(intptr_t param) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi && m_pPrintPangArea) {
		DrawOwnPang(gdi);
		DrawComposePrice(gdi);
	}
}

// 0x48bc80
void FrCardComposeDlg::OnMagicBoxLBtnUp(void) {
	OnFreshCancel();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x338, 0, 0, 0, 0, 0));
}

// 0x48bce0
void FrCardComposeDlg::OnExchangeLBtnUp(void) {
	OnFreshCancel();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2e3, 0, 0, 0, 0, 0));
}

// 0x48bd40
void FrCardComposeDlg::OnMyCardListOwnerDraw(intptr_t param) {
	FrListItem* item = (FrListItem*)param;
	if (item == 0) {
		return;
	}
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0) {
		return;
	}
	const CardComposeCard* card = (const CardComposeCard*)item->pData;
	if (card == 0) {
		return;
	}
	if (item->underCursor && m_pCardIcon) {
		m_cardIconRect.x = m_pMyCardList->GetRect().x + 2.0;
		m_cardIconRect.y = item->pos.y + 1.0;
		gdi->DrawTexture(m_pCardIcon, m_cardIconRect, 0xffffffff, 0);
	}
	DrawCardImage(item, gdi, card);
	DrawCardName(item, card);
	DrawCardCount(item, CountSlotCard(card));
}

// 0x48bde0
void FrCardComposeDlg::OnComposeCardBtn1OwnerDraw(intptr_t param) {
	DrawSlotCard(0);
}

// 0x48bdf0
void FrCardComposeDlg::OnComposeCardBtn2OwnerDraw(intptr_t param) {
	DrawSlotCard(1);
}

// 0x48be00
void FrCardComposeDlg::OnComposeCardBtn3OwnerDraw(intptr_t param) {
	DrawSlotCard(2);
}

// 0x48be10
void FrCardComposeDlg::OnAboutLBtnDown(void) {
	FrCardComposeAboutDlg* dlg = CreateForm<FrCardComposeAboutDlg>(g_pFresh->GetManager(), NULL, "card_compose_about", NULL);
	if (dlg) {
		dlg->Open(NULL, 3);
	}
}

// 0x48be60
void FrCardComposeCheckDlg::ShowCards(void) {
	for (int i = 0; i < 3; i++) {
		if (m_pViewer[i] == 0) {
			return;
		}
		m_pViewer[i]->Open(m_cardNames[i].c_str());
	}
}

// 0x48bea0
void FrCardComposeResultDlg::ShowCard(void) {
	if (m_pSynthesizedCard && m_pSynthesizedCard->Open(m_cardImage.c_str())) {
		m_showTime = 1.0f;
	}
}

// 0x48c500
FrCardComposeCheckDlg::FrCardComposeCheckDlg(void) {
}

// 0x48c5b0
FrCardComposeCheckDlg::~FrCardComposeCheckDlg(void) {
}

// 0x48d3a0
FrForm* FrCardComposeCheckDlg::Create(void) {
	return new FrCardComposeCheckDlg;
}

// 0x48d6f0
void FrCardComposeDlg::SetSlotCard(const CardComposeCard* card, int slot) {
	std::string name(card->m_iff.m_image);
	name.append(".png", 4);
	m_slotCardView[slot] = m_cardImages.Load(name.c_str());
}

// 0x48d7c0: std::string::assign(name, length) per grade; -1 and the
// grades above 3 leave the string.
void FrCardComposeDlg::SetResultSeq(const int& grade) {
	switch (grade) {
	case 0:
		m_resultSeq.assign("card_compose_N.seq", 0x12);
		break;
	case 1:
		m_resultSeq.assign("card_compose_R.seq", 0x12);
		break;
	case 2:
		m_resultSeq.assign("card_compose_SR.seq", 0x13);
		break;
	case 3:
		m_resultSeq.assign("card_compose_SC.seq", 0x13);
		break;
	}
}

// 0x48d850
void FrCardComposeResultDlg::SetCardImage(const unsigned int& cardId) {
	IFF_STRUCT::sCard* card = ItemManager()->FindCard(cardId);
	if (card == 0) {
		return;
	}
	m_cardImage.assign(card->m_image, strlen(card->m_image));
	m_cardImage.append(".png", 4);
}

// 0x48da90
void FrCardComposeDlg::OnMyCardListLBtnDown(void) {
	FrListItem* item = m_pMyCardList->GetItemUnderCursor();
	if (item == 0) {
		return;
	}
	const CardComposeCard* card = (const CardComposeCard*)item->pData;
	if (card == 0) {
		return;
	}
	if (AreSlotsFull()) {
		FrForm* dlg = CreateForm<FrForm>(g_pFresh->GetManager(), NULL, "notify", NULL);
		if (dlg) {
			// "No empty slot. Press the card compose button."
			dlg->SetMessage(Jp_LookupText("\xba\xf3\x20\xbd\xbd\xb7\xd4\xc0\xcc\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9\x2e\x20\xc4\xab\xb5\xe5\x20\xc7\xd5\xbc\xba\x20\xb9\xf6\xc6\xb0\xc0\xbb\x20\xb4\xad\xb7\xaf\xc1\xd6\xbc\xbc\xbf\xe4\x2e"), false);
			dlg->Open(NULL, 3);
		}
		return;
	}
	int slot = FindFreeSlot(card);
	if (slot != -1) {
		SetSlotCard(card, slot);
		m_pSlotBtn[slot]->SetVisible(true);
		MakePriceStrA(m_priceText, GetComposePrice());
		if (AreSlotsFull()) {
			m_pComposeCard->Enable(true);
		}
	}
}

// 0x48dbb0
void FrCardComposeCheckDlg::SetCardNames(const std::string* names) {
	for (int i = 0; i < 3; i++) {
		m_cardNames[i] = names[i];
		m_cardNames[i].append(".png", 4);
	}
}

// 0x48dd20
FrCardComposeDlg::~FrCardComposeDlg(void) {
	m_cardMap.clear();
	if (g_resrcmng != NULL && m_pResource != NULL) {
		g_resrcmng->Release(m_pResource);
		m_pResource = 0;
	}
	m_cardImages.Clear();
	if (g_resrcmng != NULL && m_pPriceFont != NULL) {
		g_resrcmng->Release(m_pPriceFont);
		m_pPriceFont = 0;
	}
}

// 0x48de90
bool FrCardComposeDlg::FillCardList(std::list<sCards>& cards, int filter) {
	for (std::list<sCards>::iterator it = cards.begin(); it != cards.end(); ++it) {
		sCards* entry = &*it;
		if (entry == 0) {
			continue;
		}
		IFF_STRUCT::sCard* iff = ItemManager()->FindCard(entry->m_typeId);
		if (iff == 0) {
			continue;
		}
		CardComposeCard* data = 0;
		std::map<unsigned int, CardComposeCard>::iterator found = m_cardMap.find(entry->m_typeId);
		if (found == m_cardMap.end()) {
			CardComposeCard value;
			value.m_card = *entry;
			value.m_iff = *iff;
			std::pair<std::map<unsigned int, CardComposeCard>::iterator, bool> inserted = m_cardMap.insert(std::make_pair((unsigned int)entry->m_typeId, value));
			if (inserted.second) {
				data = &inserted.first->second;
			}
		} else {
			found->second.m_card.m_count = entry->m_count;
			data = &found->second;
		}
		if (entry->m_typeId == 0) {
			continue;
		}
		if (iff->m_grade == 3) {
			continue;
		}
		int kind = (entry->m_typeId >> 22) & 0xf;
		if (kind == 3 || kind == 4) {
			continue;
		}
		if (filter == 3 || iff->m_grade == filter) {
			m_pMyCardList->AddItem(data);
		}
	}
	return m_pMyCardList->GetCurrentItemSize(true) > 0;
}

// 0x48e000
void FrCardComposeDlg::RequestCompose(void) {
	m_pCheckDlg = CreateForm<FrCardComposeCheckDlg>(g_pFresh->GetManager(), this, "card_compose_check", NULL);
	if (m_pCheckDlg == 0) {
		return;
	}
	m_pCheckDlg->Open((FRESH_RESULT_CALLBACK)&FrCardComposeDlg::OnCheckResult, 1);
	std::string names[3];
	for (int i = 0; i < 3; i++) {
		IFF_STRUCT::sCard* card = ItemManager()->FindCard(m_slotCardId[i]);
		if (card == 0) {
			return;
		}
		names[i] = card->m_image;
	}
	m_pCheckDlg->SetCardNames(names);
	m_pCheckDlg->ShowCards();
}

// 0x48e160: MakePriceStrA 0x98b500, ClearItem 0xbf0e50, SortItem
// 0xbf19d0, SetCurTopRow 0xbf6950 (the JP list's scroll bar at +0x90).
void FrCardComposeDlg::ResetCompose(void) {
	for (int i = 0; i < 3; i++) {
		m_slotCardId[i] = 0;
		m_pSlotBtn[i]->SetVisible(false);
	}
	MakePriceStrA(m_ownPangText, Jp_GameState_Pang());
	MakePriceStrA(m_priceText, 0);
	m_pMyCardList->ClearItem();
	if (FillCardList(Jp_GameState_CardList(), m_cardFilter)) {
		m_pMyCardList->SortItem(&FrCardComposeDlg::CompareCardItems);
	}
	m_slotCount = 0;
	// JP: also stores -1 at the JP FrListBox +0xec, a field the KR
	// FrListBox layout does not identify (KR +0xec is m_colCapacity).
	m_pMyCardList->GetScrollBar()->SetCurTopRow(m_listScrollPos);
}

// 0x48e230
bool FrCardComposeDlg::OnResultDlgResult(int ret, FrForm* form) {
	if (ret == FrOK || ret == FrCANCEL) {
		ResetCompose();
	}
	m_pResultDlg = 0;
	return true;
}

// 0x48e260
void FrCardComposeDlg::SetCardFilter(int filter) {
	if (m_cardFilter == filter) {
		return;
	}
	m_pFilterBtn[m_cardFilter]->SetStatus(FrButton::NORMAL);
	m_cardFilter = filter;
	m_pMyCardList->ClearItem();
	if (FillCardList(Jp_GameState_CardList(), filter)) {
		m_pMyCardList->SortItem(&FrCardComposeDlg::CompareCardItems);
	}
}

// 0x48e400
FrCardComposeDlg::FrCardComposeDlg(void) {
	// Also zeroes +0x15c and constructs the sub-object at +0x168
	// (0x486700): their types are not recovered (UNKNOWN in the layout).
	// m_cardImages, m_cardMap and m_resultSeq are constructed implicitly.
	m_selectedIndex = -1;
	m_resultCardId = 0;
	m_pResource = 0;
	m_slotCount = 0;
	m_cardFilter = 3;
	m_listScrollPos = 0;
	m_slotCardId[0] = 0;
	m_slotCardView[0] = 0;
	m_slotCardId[1] = 0;
	m_slotCardView[1] = 0;
	m_slotCardId[2] = 0;
	m_slotCardView[2] = 0;
	InitPriceFont();
	m_resultSeq = "";
	m_pMagicBox = 0;
	m_pCardCompose = 0;
	m_pExchange = 0;
}

// 0x48e5c0
bool FrCardComposeDlg::OnInit(void) {
	LoadBitmaps();
	InitCardImageSize();
	InitCardIconSize();
	InitSlotIconSize();
	if (FillCardList(Jp_GameState_CardList(), 3)) {
		m_pMyCardList->SortItem(&FrCardComposeDlg::CompareCardItems);
	}
	m_pCardCompose->SetStatus(FrButton::PRESSED);
	if (RFLobbyDesktop::s_mode == 1) {
		ntAvatarManager* manager = WSingleton<ntAvatarManager>::Instance();
		if (manager) {
			if (manager->m_pMyAvatar) {
				manager->m_pMyAvatar->AddState(0x20);
			}
			manager->SetActive(false);
		}
	}
	return true;
}

// 0x48e650
void FrCardComposeDlg::OnComposeCardLBtnDown(void) {
	if (!AreSlotsFull()) {
		return;
	}
	if (Jp_GameState_Pang() >= GetComposePrice()) {
		m_listScrollPos = m_pMyCardList->GetScrollBar()->GetCurTopRow_Int();
		RequestCompose();
		return;
	}
	FrForm* dlg = CreateForm<FrForm>(g_pFresh->GetManager(), NULL, "notify", NULL);
	if (dlg) {
		// "Not enough pang."
		dlg->SetMessage(Jp_LookupText("\xba\xb8\xc0\xaf\xc7\xcf\xb0\xed\x20\xc0\xd6\xb4\xc2\x20\xc6\xce\xc0\xcc\x20\xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9"), false);
		dlg->Open(NULL, 3);
	}
}

// 0x48e740
void FrCardComposeDlg::OnWholeCardButtonLBtnDown(void) {
	SetCardFilter(3);
}

// 0x48e750
void FrCardComposeDlg::OnNormalCardButtonLBtnDown(void) {
	SetCardFilter(0);
}

// 0x48e760
void FrCardComposeDlg::OnRareCardButtonLBtnDown(void) {
	SetCardFilter(1);
}

// 0x48e770
void FrCardComposeDlg::OnSuperRareCardButtonLBtnDown(void) {
	SetCardFilter(2);
}

// 0x48e920
FrForm* FrCardComposeDlg::Create(void) {
	return new FrCardComposeDlg;
}

// 0x48ebc0
FrCardComposeResultDlg::~FrCardComposeResultDlg(void) {
	m_effect.ClearAll(false);
}

// 0x48ecb0: WMatrix::IDENTITY is 0xe42504, 600.0 0xcff418, -30.0 0xcff410.
FrCardComposeResultDlg::FrCardComposeResultDlg(void)
	: m_effectCamera(WMatrix::IDENTITY), m_fxRect(0.0f, 0.0f, 1.0f, 1.0f), m_showTime(-1.0f) {
	m_effect.Init(g_resrcmng);
	m_effect.SetCamera(&g_camera);
	// JP: the effect's frame time (+0x4, the KR protected
	// CFxBox::m_frameTime) set to 33.333f (0xcff420), the KR constructor's
	// value; the KR CFxBox has no setter for it.
	float scale = (float)(g_view->GetHeight() / 600.0);
	scale = (float)(scale * -30.0);
	m_effectCamera.pivot = m_effectCamera.za * scale;
}

// 0x48ef20
void FrCardComposeDlg::OpenResultDlg(void) {
	m_pResultDlg = CreateForm<FrCardComposeResultDlg>(g_pFresh->GetManager(), this, "result_card_compose", NULL);
	if (m_pResultDlg == 0) {
		return;
	}
	m_pResultDlg->SetCardImage(m_resultCardId);
	m_pResultDlg->PlaySequence(m_resultSeq.c_str());
	m_pResultDlg->SetOwnerDlg(this);
	m_pResultDlg->ShowCard();
	m_pResultDlg->Open((FRESH_RESULT_CALLBACK)&FrCardComposeDlg::OnResultDlgResult, 1);
}

// 0x48efd0
FrForm* FrCardComposeResultDlg::Create(void) {
	return new FrCardComposeResultDlg;
}

// The JP service bit the card compose dialog needs (bit 0 of the high
// dword; 0x659490 pushes 1, 0).
static const uint64_t kCardComposeService = (uint64_t)1 << 32;

// JP CTaskMain +0x18c, the open card compose dialog: the KR CTaskMain has
// no such member, so the port keeps it here with the task that opened it
// (as JpAlchemyDlg.cpp keeps +0x1a8).
static CTaskMain* s_cardComposeDlgTask = NULL;
static FrCardComposeDlg* s_cardComposeDlg = NULL;

static void SetCardComposeDlg(CTaskMain* task, FrCardComposeDlg* dlg) {
	s_cardComposeDlgTask = task;
	s_cardComposeDlg = dlg;
}

// The JP CTaskMain +0x18c read (inline in the JP).
FrCardComposeDlg* TaskMain_GetCardComposeDlg(const CTaskMain* task) {
	return s_cardComposeDlgTask == task ? s_cardComposeDlg : NULL;
}

// The JP CTaskMain +0x6c read (inline in the JP): KR m_barButton[0x13].
FrWnd* TaskMain_GetCardComposeIconWnd(const CTaskMain* task) {
	return task->m_barButton[0x13];
}

// 0x65f357 (inline in the JP CTaskMain::HandleMsgCommon 0x65d8e0, case
// 0x325).
void TaskMain_SetCardComposeResultSeq(CTaskMain* task, int grade) {
	FrCardComposeDlg* dlg = TaskMain_GetCardComposeDlg(task);
	if (dlg) {
		dlg->SetResultSeq(grade);
	}
}

// 0x65f383 (inline in the JP HandleMsgCommon, case 0x326): the JP reads the
// dialog again (JP CTaskMain +0x18c) for OpenResultDlg.
void TaskMain_OnCardComposeResult(CTaskMain* task, unsigned int cardId) {
	if (TaskMain_GetCardComposeDlg(task) && TaskMain_GetCardComposeDlg(task)->SetResultCard(cardId)) {
		TaskMain_GetCardComposeDlg(task)->OpenResultDlg();
	}
}

// 0x653ae0: the JP CTaskMain's card compose result handler (ret 8, always
// true), the twin of OnAlchemyDlgResult (0x6525a0): in avatar-chat mode
// (RFLobbyDesktop::s_mode, JP 0xe78c3c) with neither the alchemy dialog (JP
// +0x1a8) nor the recycle dialog (JP +0x164, the KR m_recycleDlg) open, the
// player's avatar drops state 0x20 (JP ntAvatar::SetState 0x7a4820, the KR
// SubState) and the avatar manager (JP 0xe73e10 +0xb0) is active again.
bool CTaskMain::OnCardComposeDlgResult(int, FrForm*) {
	SetCardComposeDlg(this, NULL);

	if (RFLobbyDesktop::s_mode == 1 && TaskMain_GetAlchemyDlg(this) == NULL && !IsActiveRecycleDlg()) {
		ntAvatarManager* manager = WSingleton<ntAvatarManager>::Instance();
		if (manager) {
			ntAvatar* myAvatar = manager->m_pMyAvatar;
			if (myAvatar) {
				myAvatar->SubState(0x20);
			}
			WSingleton<ntAvatarManager>::Instance()->SetActive(true);
		}
	}
	return true;
}

// 0x659490: the JP CTaskMain vtable +0x3c is CloseAllDialogs (as in
// 0x659340); the form's JP [30] (+0x78) EnableDrag, [25] (+0x64)
// Open(cb, flag) and [22] (+0x58, 0xbe6100) SetIconRect are the KR FrForm
// slots 29, 24 and 22 (the JP has one more slot between SetIconRect and
// Open; 0xbe6100 is the KR FrForm::SetIconRect's body); OnBarHoverOff is
// JP 0x656a80. Unlike the alchemy opener there is no "already open" test,
// no avatar manager change and no CSharedDoc +0x10b25 store (the dialog's
// OnInit 0x48e5c0 makes the avatar manager inactive itself).
void TaskMain_OpenCardComposeDlg(CTaskMain* task) {
	if (SharedDoc_IsControlServerService64(CSharedDoc::Instance(), kCardComposeService)) {
		Jp_NotifyServerServiceClosed(kCardComposeService);
		return;
	}

	task->CloseAllDialogs();
	FrCardComposeDlg* dlg = CreateForm<FrCardComposeDlg>(g_pFresh->GetManager(), task, "card_compose", 0);
	SetCardComposeDlg(task, dlg);
	dlg->EnableDrag(false);
	TaskMain_GetCardComposeDlg(task)->Open((FRESH_RESULT_CALLBACK)&CTaskMain::OnCardComposeDlgResult, 0x11);

	FrWnd* iconWnd = TaskMain_GetCardComposeIconWnd(task);
	if (iconWnd && TaskMain_GetCardComposeDlg(task)) {
		TaskMain_GetCardComposeDlg(task)->SetIconRect(iconWnd->GetRect());
	}
	task->OnBarHoverOff(0);
}
