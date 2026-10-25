// JP-only game code: the JP CRealMyRoomMain (JpRealMyRoomMain,
// JpRealMyRoomMain.h), the lobby My Room page's "realmyroom" form, from the
// JP exe (node scripts/disasm.ts --jp 0x<va>), and its registration in the
// JP form registry. What is still a port body: JpRealMyRoomMain.h, "Status".
#include "JpRealMyRoomMain.h"

#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "frgaugebar.h"
#include "frgroupbox.h"
#include "frlistbox.h"
#include "frtabbutton.h"
#include "frviewer.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "commonutil.h"
#include "ItemManager.h"
#include "JpLobbyGlue.h"
#include "JpTanLines.h"
#include "RFLobbyDesktop.h"
#include "actor.h"
#include "frscrollbar.h"
#include "s5/Utilities.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "shared/Localize.h"

// 0x604b00 (?SetSubTabImg@@YIXQAPBDH@Z): the KR SetSubTabImg
// (RealMyRoomMainUI.cpp, compiled into the port), the three images
// "stext_%d_n" of a sub tab.
void SetSubTabImg(const char* imgs[], int index);

// lib: 0x61d780 (the FrCmdTarget deleting-destructor thunk)
const WRTTI JpRealMyRoomMain::m_RTTI("CRealMyRoomMain", &FrTaskForm::m_RTTI);

// 0x604b50
const sFRESH_MSGMAP* JpRealMyRoomMain::GetMessageMap(void) const {
	return &JpRealMyRoomMain::_MsgMap;
}

sFRESH_MSGMAP JpRealMyRoomMain::_MsgMap = { &FrForm::_MsgMap, &JpRealMyRoomMain::_MsgEntries[0] };

// The rows of 0xe13488 written so far, in the JP row order (handlers from
// the static init 0xcd2240; the full list with every handler VA:
// temps/orchestrator/refresh/R990_rows.txt). The rows not here (owner
// draws, the item list's clicks, the card / storage clicks ...) get
// FrForm's default handling.
sFRESH_ENTRY JpRealMyRoomMain::_MsgEntries[] = {
	{ "chat_bg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnChatBgInit },
	{ "middle_ui", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnMiddleUiInit },
	{ "middle_ui.main_bg1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnMainBg1Init },
	{ "middle_ui.main_bg2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnMainBg2Init },
	{ "middle_ui.main_bg3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnMainBg3Init },
	{ "middle_ui.main_bg4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnMainBg4Init },
	{ "middle_ui.item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemListInit },
	{ "middle_ui.item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemListOwnerDraw },
	{ "middle_ui.item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&JpRealMyRoomMain::OnItemListBtnDown },
	{ "middle_ui.tab_main", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnTabMainInit },
	{ "middle_ui.tab_main", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpRealMyRoomMain::OnTabMainUp },
	{ "middle_ui.tab_sub", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnTabSubInit },
	{ "middle_ui.tab_sub", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpRealMyRoomMain::OnTabSubUp },
	{ "item_search_btn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemSearchBtnInit },
	{ "close_wnd", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpRealMyRoomMain::OnCloseWndUp },
	{ "middle_ui.storage_open_btn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&JpRealMyRoomMain::OnStorageOpenBtnUp },
	{ "middle_ui.tabtxt2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnTabTxt2Init },
	{ "middle_ui.tabtxt2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnTabTxt3Init },
	{ "middle_ui.pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnPetInit },
	{ "middle_ui.skin_pet", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnSkinPetInit },
	{ "middle_ui.skin_pet2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnSkinPet2Init },
	{ "middle_ui.item_desc_bg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemDescBgInit },
	{ "middle_ui.item_desc_bg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemDescBgOwnerDraw },
	{ "middle_ui.item_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemDescInit },
	{ "middle_ui.sbox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemSlotBoxInit },
	{ "middle_ui.sbox.equip_cancel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnEquipCancelInit },
	{ "middle_ui.sbox.equip_reset", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnEquipResetInit },
	{ "middle_ui.sbox.item_slot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemSlotInit },
	{ "middle_ui.sbox.item_slot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemSlotOwnerDraw },
	{ "middle_ui.gift_bg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnGiftBgInit },
	{ "middle_ui.gift_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnGiftDescInit },
	{ "stat_group", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnStatGroupInit },
	{ "stat_group.stat_text", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnStatTextInit },
	{ "stat_group.epower", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnPowerBarInit },
	{ "stat_group.econtrol", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnControlBarInit },
	{ "stat_group.eimpact", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnImpactBarInit },
	{ "stat_group.espin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnSpinBarInit },
	{ "stat_group.ecurve", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCurveBarInit },
	{ "updown_group", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnUpDownGroupInit },
	{ "updown_group.upgrade", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnUpgradeInit },
	{ "updown_group.downgrade", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnDowngradeInit },
	{ "updown_group.mastery_m", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnMasteryInit },
	{ "updown_group.clubwork_m", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnClubWorkInit },
	{ "ftab_main", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnFurnitureTabMainInit },
	{ "fitem_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnFurnitureListInit },
	{ "item_hide_desc_bg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemHideDescBgInit },
	{ "item_hide_desc_bg", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemHideDescBgOwnerDraw },
	{ "item_hide_desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemHideDescInit },
	{ "item_hide_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemHideNameInit },
	{ "card_book", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardBookInit },
	{ "cardpacklist", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPackListInit },
	{ "cardpage_index", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPageIndexInit },
	{ "card_property", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPropertyInit },
	{ "card_pack", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPackInit },
	{ "card_use", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardUseInit },
	{ "card_volume1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardVolume1Init },
	{ "card_volume2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardVolume2Init },
	{ "card_volume3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardVolume3Init },
	{ "card_volume4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardVolume4Init },
	{ "card_volume5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardVolume5Init },
	{ "card_sp", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSpInit },
	{ "cardpack_slot1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPackSlot1Init },
	{ "cardpack_slot2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPackSlot2Init },
	{ "cardpack_slot3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPackSlot3Init },
	{ "cardpack_slot4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPackSlot4Init },
	{ "cardpack_slot5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPackSlot5Init },
	{ "cardpack_slot6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPackSlot6Init },
	{ "cardpack_slot7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPackSlot7Init },
	{ "cardpack_slot8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardPackSlot8Init },
	{ "card_slot1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot1Init },
	{ "card_slot2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot2Init },
	{ "card_slot3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot3Init },
	{ "card_slot4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot4Init },
	{ "card_slot5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot5Init },
	{ "card_slot6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot6Init },
	{ "card_slot7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot7Init },
	{ "card_slot8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot8Init },
	{ "card_slot9", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot9Init },
	{ "card_slot10", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot10Init },
	{ "card_slot11", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot11Init },
	{ "card_slot12", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnCardSlot12Init },
	{ "item_storage_pang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemStoragePangInit },
	{ "pang_minus", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnPangMinusInit },
	{ "pang_plus", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnPangPlusInit },
	{ "item_storage_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemStorageListInit },
	{ "item_storage_index", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemStorageIndexInit },
	{ "item_storage_page_next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemStoragePageNextInit },
	{ "item_storage_page_prev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemStoragePagePrevInit },
	{ "item_storage_close", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemStorageCloseInit },
	{ "item_storage_dragdrop", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnItemStorageDragDropInit },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// Port body (JP 0x61f1f0, see JpRealMyRoomMain.h): the JP builds the
// three std::maps, the +0x588 object, the shared-doc map copy, the list and
// the +0xac0 object (not modelled: they stay in the padding) and news the
// card book; the controls the INIT handlers store start NULL (the JP
// relies on its zeroing new for them).
JpRealMyRoomMain::JpRealMyRoomMain(void) {
	m_middleUi = NULL;
	m_mainTab = NULL;
	m_subTab = NULL;
	m_itemList = NULL;
	m_tabText[0] = NULL;
	m_tabText[1] = NULL;
	m_statGroup = NULL;
	for (int i = 0; i < 5; i++) {
		m_statBar[i] = NULL;
	}
	m_itemDescBg = NULL;
	m_itemDesc = NULL;
	m_itemHideDescBg = NULL;
	m_itemHideDesc = NULL;
	m_itemHideName = NULL;
	m_skinPet[0] = NULL;
	m_skinPet[1] = NULL;
	m_pet = NULL;
	m_itemSlot = NULL;
	m_slotBox = NULL;
	m_upDownGroup = NULL;
	m_equipCancel = NULL;
	m_equipReset = NULL;
	m_upgradeButton = NULL;
	m_downgradeButton = NULL;
	m_clubWorkButton = NULL;
	m_masteryButton = NULL;
	m_giftBg = NULL;
	m_giftDesc = NULL;
	m_statText = NULL;
	m_furnitureTab = NULL;
	m_furnitureList = NULL;
	for (int i = 0; i < 4; i++) {
		m_mainBg[i] = NULL;
	}
	m_chatBg = NULL;
	m_itemSearchButton = NULL;
	m_itemHideDescLeft = 0.0f;
	m_itemHideDescBottom = 0.0f;
	m_pCharLevels = NULL;
	m_pCharParts = NULL;
	for (int i = 0; i < 24; i++) {
		m_charPartIds[i] = 0;
	}
	m_pCharCards = NULL;
	m_charTypeId = 0;
	m_charSerial = 0;
	m_mainTabIndex = 0;
	m_subTabIndex = 0x16;
	m_subTabDirty = false;
	m_uiState = 0;
	m_itemDescLeft = 0.0f;
	m_itemDescBottom = 0.0f;
	m_itemHideDescLeft2 = 0.0f;
	m_itemHideDescBottom2 = 0.0f;
	m_furnitureTabIndex = 0;
	m_preview = NULL;
	m_itemStorageOpen = false;
	m_itemStorageBg = NULL;
	m_itemStoragePang = NULL;
	m_pangMinus = NULL;
	m_pangPlus = NULL;
	m_itemStorageList = NULL;
	m_itemStorageIndex = NULL;
	m_itemStorageNext = NULL;
	m_itemStoragePrev = NULL;
	m_itemStorageClose = NULL;
	m_itemStorageDragDrop = NULL;
	m_cardPackList = NULL;
	// The item list areas are filled by LoadMyRoomImages (Start), after
	// the INIT handlers have read them: they start empty.
	for (int i = 0; i < 16; i++) {
		m_itemListArea[i].rect = WRect(0.0f, 0.0f, 0.0f, 0.0f);
		m_itemListArea[i].normal = NULL;
		m_itemListArea[i].over = NULL;
	}
	m_itemDescPos.x = 0.0f;
	m_itemDescPos.y = 0.0f;
	m_cardBook = new JpCardBook();
}

// Port body (JP 0x61f500).
JpRealMyRoomMain::~JpRealMyRoomMain(void) {
	delete m_cardBook;
	m_cardBook = NULL;
}

// Port body (JP 0x620c30). Left out: the card book's slot reset 0x7f3860
// (its card page is not ported), the shared doc's +0x3704 character map
// and the 0x74 bytes at +0xa26 copied to +0x710 / +0x720,
// 0xbe7870, the lounge
// avatar's flag (0xe73e10 +0xa4, 0x7a4820 | 2), and the 0xe7688c
// tooltip rects under local content 0x70. Kept: the tab state, the main
// tab (0), the sub tab (0x16, SetSubTab a port body), the lounge mode's
// backgrounds and FrForm::OnInit; the map and equipment copies are done
// member by member.
bool JpRealMyRoomMain::OnInit(void) {
	// The JP copies the doc's +0x3704 map and the 0x74 bytes at +0xa26 (the
	// KR m_characterMap and m_myEquip): field by field here (LP64).
	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	if (&m_characterMap != &pDoc->m_characterMap) {
		m_characterMap = pDoc->m_characterMap;
	}
	m_caddieId = (uint32_t)pDoc->m_myCaddieId;
	m_characterId = (uint32_t)pDoc->m_myCharacterId;
	m_clubId = (uint32_t)pDoc->m_myClubId;
	m_ballId = (uint32_t)pDoc->m_myBallId;
	for (int i = 0; i < 10; i++) {
		m_equipItem[i] = (uint32_t)pDoc->m_equipItem[i];
	}
	for (int i = 0; i < 6; i++) {
		m_equipSkin[i] = (uint32_t)pDoc->m_equipSkin[i];
	}
	m_mascotId = (uint32_t)pDoc->m_myMascotId;

	m_mainTabIndex = 0;
	m_subTabIndex = 0x16;
	RefreshCharacter(pDoc->m_myCharacterId, false);
	SetMainTab(0, true);
	SetSubTab(0x16, true);

	if (RFLobbyDesktop::s_mode == 1) {
		for (int i = 0; i < 4; i++) {
			m_mainBg[i]->SetVisible(false);
		}
		m_chatBg->SetVisible(true);
	}

	return FrForm::OnInit();
}

// 0x60ca40
bool JpRealMyRoomMain::Start(void) {
	LoadMyRoomImages();
	return true;
}

// Port body (JP 0x6165d0): only FrTaskForm::Shutdown (0x523610) and true.
bool JpRealMyRoomMain::Shutdown(void) {
	FrTaskForm::Shutdown();
	return true;
}

// Port body (JP 0x623500, 438 bytes plus two jump tables: 0x49..0x109
// over 0x6266b8 / 0x626640 and 0x279..0x320 over 0x626820 / 0x62677c).
// Kept: message 3 is taken (true). Left out: 0x48 (the +0x588 timer
// 0x98ad40(9999) in the lounge, RefreshCharacter), 0x278 (the main actor's
// 2, then the "fortune_open_new" FrPangGiftDlg at +0x210) and both tables.
bool JpRealMyRoomMain::HandleMsg(const MsgObject& msg) {
	if (msg.m_msg == 3) {
		return true;
	}

	return false;
}

// Port body (JP 0x611f50).
void JpRealMyRoomMain::OnAnotherHand(int nWork, void* pData) {
}

// 0x622630 (only case 9 written; the JP cases 0, 2, 6 and 8 are the KR
// cases 0, 2, 8 (authority dialog) and 9/10 (furniture-mode notify); 1, 3,
// 4, 5 and 7 do nothing). Case 9 is the KR case 11 under JP local content
// 0x70 (KR 0x71) / 0x52 (KR 0x51), with the leave packet 0xd3 (KR 0xd5),
// the main actor's 0x2af (KR: RealMyRoom actor 0x234) when the item storage
// is open, and the JP's eighth equip sender.
void JpRealMyRoomMain::SelectMenu(unsigned char menu) {
	switch (menu) {
	case 9:
		if (!IsLocalContent((localContentType_t)0x70)) {
			break;
		}

		if (!m_itemStorageOpen) {
			WSendPacket packet(0xd3);
			packet.Send(SEND_TO_DEFAULT);

			Net_EquipDresserParts();
			Net_EquipCharacter();
			Net_EquipCaddie();
			Net_EquipItem();
			Net_EquipGear();
			Net_EquipSkin();
			Net_EquipMascot();
			if (IsLocalContent((localContentType_t)0x52)) {
				WSingleton<CSharedDoc>::Instance()->SendEquipItemSlot();
			} else {
				WSingleton<CSharedDoc>::Instance()->SendEquipItemSlotByMascot();
			}
			Net_SendEquipKind9();
		} else {
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2af, 0, 0, 0, 0, 0));
		}
		break;
	}
}

// 0x6229b0
void JpRealMyRoomMain::OnStorageOpenBtnUp(void) {
	SelectMenu(9);
}

// 0x604eb0: Close is FrTaskForm's [2] 0x523730 (it ends the page's widget
// and hands the lobby back to the desktop).
void JpRealMyRoomMain::OnCloseWndUp(void) {
	Close(true);
	JpTanLines* tanLines = JpTanLines::Instance();  // 0x542f00
	if (tanLines != NULL) {  // port: the JP stores without a test
		tanLines->m_flag7a = false;
	}
}

// 0x6059a0
void JpRealMyRoomMain::HideCardPackSlot(FrButton* pButton, int slot) {
	if (pButton == NULL) {
		return;
	}

	pButton->SetVisible(false);
	pButton->Enable(false);
}

// 0x605010
void JpRealMyRoomMain::ShowUpgradeButtons(bool showMastery, bool showClubWork) {
	m_masteryButton->SetVisible(showMastery);
	m_clubWorkButton->SetVisible(showClubWork);
	m_upgradeButton->SetVisible(showClubWork);
	m_downgradeButton->SetVisible(showClubWork);
}

// Port body (JP 0x60a330): the JP binds the button to the card page's
// slot (CCardSlot::BoundControl, which hides it) and registers it with the
// 0xe7688c tooltips under local content 0x70; the card page is not ported,
// so only the hiding is kept.
void JpRealMyRoomMain::BindCardSlot(FrButton* pButton, unsigned char slot) {
	if (pButton == NULL) {
		return;
	}

	if (m_cardBook == NULL) {
		return;
	}

	pButton->SetVisible(false);
}

// 0x606e90
void JpRealMyRoomMain::OnItemListInit(intptr_t param) {
	m_itemList = DYNAMIC_CAST(FrListBox, param);
	if (m_itemList == NULL) {
		return;
	}

	m_itemList->UseRightButton(true);
	m_itemList->UseDummy(true);
	m_itemList->SetPushSound(0);
	m_itemList->AddNoRButtonRect(m_itemListArea[0].rect, 0);
	m_itemList->AddNoRButtonRect(m_itemListArea[4].rect, 4);
	m_itemList->AddNoRButtonRect(m_itemListArea[1].rect, 1);
	m_itemList->AddNoRButtonRect(m_itemListArea[2].rect, 2);
	m_itemList->AddNoRButtonRect(m_itemListArea[3].rect, 3);
	if (IsLocalContent((localContentType_t)0x50)) {
		m_itemList->AddNoRButtonRect(m_itemListArea[7].rect, 7);
		m_itemList->AddNoRButtonRect(m_itemListArea[8].rect, 8);
	}
	if (IsLocalContent((localContentType_t)0x52)) {
		m_itemList->AddNoRButtonRect(m_itemListArea[9].rect, 9);
	}
	if (IsLocalContent((localContentType_t)0x57)) {
		m_itemList->AddNoRButtonRect(m_itemListArea[10].rect, 10);
	}
}

// 0x607010 is the item list's MOUSEMOVE row (not written).

// 0x607080
void JpRealMyRoomMain::OnTabMainInit(intptr_t param) {
	m_mainTab = DYNAMIC_CAST(FrTabButton, param);
	if (m_mainTab == NULL) {
		return;
	}

	const char* names[3] = { NULL };
	names[0] = MakeStr("stab_%d_n", 7);
	names[1] = MakeStr("stab_%d_d", 7);
	names[2] = MakeStr("stab_%d_d", 7);
	m_mainTab->AddTab(names, 0, NULL);

	for (int i = 2; i <= 6; i++) {
		names[0] = MakeStr("stab_%d_n", i);
		names[1] = MakeStr("stab_%d_d", i);
		names[2] = MakeStr("stab_%d_d", i);
		m_mainTab->AddTab(names, i - 1, NULL);
	}
}

// 0x607190
void JpRealMyRoomMain::OnTabSubInit(intptr_t param) {
	m_subTab = DYNAMIC_CAST(FrTabButton, param);
}

// 0x607200
void JpRealMyRoomMain::OnItemSearchBtnInit(intptr_t param) {
	m_itemSearchButton = DYNAMIC_CAST(FrButton, param);
}

// 0x607270
void JpRealMyRoomMain::OnChatBgInit(intptr_t param) {
	m_chatBg = DYNAMIC_CAST(FrViewer, param);
	if (m_chatBg != NULL) {
		m_chatBg->SetVisible(false);
	}
}

// 0x6072f0
void JpRealMyRoomMain::OnMainBg1Init(intptr_t param) {
	m_mainBg[0] = DYNAMIC_CAST(FrArea, param);
}

// 0x607360
void JpRealMyRoomMain::OnMainBg2Init(intptr_t param) {
	m_mainBg[1] = DYNAMIC_CAST(FrArea, param);
}

// 0x6073d0
void JpRealMyRoomMain::OnMainBg3Init(intptr_t param) {
	m_mainBg[2] = DYNAMIC_CAST(FrArea, param);
}

// 0x607440
void JpRealMyRoomMain::OnMainBg4Init(intptr_t param) {
	m_mainBg[3] = DYNAMIC_CAST(FrArea, param);
}

// 0x6074b0
void JpRealMyRoomMain::OnMiddleUiInit(intptr_t param) {
	m_middleUi = DYNAMIC_CAST(FrGroupBox, param);
}

// 0x607520
void JpRealMyRoomMain::OnTabTxt2Init(intptr_t param) {
	m_tabText[0] = DYNAMIC_CAST(FrArea, param);
}

// 0x607590
void JpRealMyRoomMain::OnTabTxt3Init(intptr_t param) {
	m_tabText[1] = DYNAMIC_CAST(FrArea, param);
}

// 0x607600
void JpRealMyRoomMain::OnPetInit(intptr_t param) {
	m_pet = DYNAMIC_CAST(FrArea, param);
}

// 0x607670
void JpRealMyRoomMain::OnSkinPetInit(intptr_t param) {
	m_skinPet[0] = DYNAMIC_CAST(FrArea, param);
}

// 0x6076e0
void JpRealMyRoomMain::OnSkinPet2Init(intptr_t param) {
	m_skinPet[1] = DYNAMIC_CAST(FrArea, param);
}

// 0x607750
void JpRealMyRoomMain::OnItemDescBgInit(intptr_t param) {
	m_itemDescBg = DYNAMIC_CAST(FrArea, param);
}

// 0x6078f0
void JpRealMyRoomMain::OnItemDescInit(intptr_t param) {
	m_itemDesc = DYNAMIC_CAST(FrEdit, param);
	if (m_itemDesc != NULL) {
		m_itemDesc->SetVisible(false);
		m_itemDescLeft = m_itemDesc->GetRect().x;
		m_itemDescBottom = m_itemDesc->GetRect().y + m_itemDesc->GetRect().h;
		// The KR m_itemDescPos (JpRealMyRoomMainDraw.cpp's
		// OnItemDescBgOwnerDraw reads it).
		m_itemDescPos.x = m_itemDescLeft;
		m_itemDescPos.y = m_itemDescBottom;
	}
}

// 0x607990
void JpRealMyRoomMain::OnItemHideDescInit(intptr_t param) {
	m_itemHideDesc = DYNAMIC_CAST(FrEdit, param);
	if (m_itemHideDesc != NULL) {
		m_itemHideDesc->SetVisible(false);
		float left = m_itemHideDesc->GetRect().x;
		float bottom = m_itemHideDesc->GetRect().y + m_itemHideDesc->GetRect().h;
		m_itemHideDescLeft2 = left;
		m_itemHideDescLeft = left;
		m_itemHideDescBottom2 = bottom;
		m_itemHideDescBottom = bottom;
	}
}

// 0x607a40
void JpRealMyRoomMain::OnItemHideNameInit(intptr_t param) {
	m_itemHideName = DYNAMIC_CAST(FrEdit, param);
	if (m_itemHideName != NULL) {
		m_itemHideName->SetVisible(false);
	}
}

// Port body (JP 0x607ac0): left out the 0xe7688c tooltip registration
// (0x7fe1a0) under local content 0x70.
void JpRealMyRoomMain::OnItemSlotBoxInit(intptr_t param) {
	m_slotBox = DYNAMIC_CAST(FrGroupBox, param);
	if (m_slotBox != NULL) {
		m_slotBox->SetVisible(false);
	}
}

// Port body (JP 0x607b60): left out the button's +0x104 / +0x108 floats,
// 0xbe7a40(1, 0.0f) and the 0xe7688c tooltip registration.
void JpRealMyRoomMain::OnEquipCancelInit(intptr_t param) {
	m_equipCancel = DYNAMIC_CAST(FrButton, param);
}

// Port body (JP 0x607c10): as OnEquipCancelInit.
void JpRealMyRoomMain::OnEquipResetInit(intptr_t param) {
	m_equipReset = DYNAMIC_CAST(FrButton, param);
}

// 0x607cc0
void JpRealMyRoomMain::OnItemSlotInit(intptr_t param) {
	m_itemSlot = DYNAMIC_CAST(FrListBox, param);
}

// 0x607d40
void JpRealMyRoomMain::OnGiftBgInit(intptr_t param) {
	m_giftBg = DYNAMIC_CAST(FrViewer, param);
	if (m_giftBg != NULL) {
		m_giftBg->SetVisible(false);
		m_giftBg->SetMouseEvent(false);
	}
}

// 0x607dc0
void JpRealMyRoomMain::OnGiftDescInit(intptr_t param) {
	m_giftDesc = DYNAMIC_CAST(FrEdit, param);
	if (m_giftDesc != NULL) {
		m_giftDesc->SetVisible(false);
	}
}

// 0x607e40
void JpRealMyRoomMain::OnStatGroupInit(intptr_t param) {
	m_statGroup = DYNAMIC_CAST(FrGroupBox, param);
	if (m_statGroup != NULL) {
		m_statGroup->SetVisible(false);
	}
}

// 0x607ed0
void JpRealMyRoomMain::OnStatTextInit(intptr_t param) {
	m_statText = DYNAMIC_CAST(FrArea, param);
}

// 0x607fa0
void JpRealMyRoomMain::OnPowerBarInit(intptr_t param) {
	m_statBar[0] = DYNAMIC_CAST(FrGaugeBar, param);
}

// 0x608010
void JpRealMyRoomMain::OnControlBarInit(intptr_t param) {
	m_statBar[1] = DYNAMIC_CAST(FrGaugeBar, param);
}

// 0x608080
void JpRealMyRoomMain::OnImpactBarInit(intptr_t param) {
	m_statBar[2] = DYNAMIC_CAST(FrGaugeBar, param);
}

// 0x6080f0
void JpRealMyRoomMain::OnSpinBarInit(intptr_t param) {
	m_statBar[3] = DYNAMIC_CAST(FrGaugeBar, param);
}

// 0x608160
void JpRealMyRoomMain::OnCurveBarInit(intptr_t param) {
	m_statBar[4] = DYNAMIC_CAST(FrGaugeBar, param);
}

// 0x6081d0
void JpRealMyRoomMain::OnUpDownGroupInit(intptr_t param) {
	m_upDownGroup = DYNAMIC_CAST(FrGroupBox, param);
}

// Port body (JP 0x608240): left out 0xbe7a40(1, 0.0f).
void JpRealMyRoomMain::OnUpgradeInit(intptr_t param) {
	m_upgradeButton = DYNAMIC_CAST(FrButton, param);
}

// Port body (JP 0x6082c0): left out 0xbe7a40(1, 0.0f).
void JpRealMyRoomMain::OnDowngradeInit(intptr_t param) {
	m_downgradeButton = DYNAMIC_CAST(FrButton, param);
}

// Port body (JP 0x608340): left out 0xbe7a40(1, 0.0f).
void JpRealMyRoomMain::OnClubWorkInit(intptr_t param) {
	m_clubWorkButton = DYNAMIC_CAST(FrButton, param);
}

// 0x6083c0
void JpRealMyRoomMain::OnMasteryInit(intptr_t param) {
	m_masteryButton = DYNAMIC_CAST(FrButton, param);
}

// 0x6091c0
void JpRealMyRoomMain::OnFurnitureTabMainInit(intptr_t param) {
	m_furnitureTab = DYNAMIC_CAST(FrTabButton, param);
	if (m_furnitureTab == NULL) {
		return;
	}

	m_furnitureTab->ClearTab();
	for (int i = 0; i < 5; i++) {
		m_furnitureTab->AddTab(MakeStr("rmr_arg_tab%d", i + 1), i, NULL);
	}
	m_furnitureTab->Select(m_furnitureTabIndex);
	m_furnitureTab->SetVisible(false);
}

// Port body (JP 0x6092c0): left out the list's +0xf8 = 1.
void JpRealMyRoomMain::OnFurnitureListInit(intptr_t param) {
	m_furnitureList = DYNAMIC_CAST(FrListBox, param);
	if (m_furnitureList != NULL) {
		m_furnitureList->SetVisible(false);
		m_furnitureList->UseDummy(true);
	}
}

// 0x609350
void JpRealMyRoomMain::OnItemHideDescBgInit(intptr_t param) {
	m_itemHideDescBg = DYNAMIC_CAST(FrViewer, param);
	if (m_itemHideDescBg != NULL) {
		m_itemHideDescBg->SetVisible(false);
	}
}

// 0x6098a0
void JpRealMyRoomMain::OnCardBookInit(intptr_t param) {
	FrArea* wnd = DYNAMIC_CAST(FrArea, param);
	if (wnd != NULL && m_cardBook != NULL) {
		m_cardBook->m_bookWnd = wnd;
		wnd->SetVisible(false);
	}
}

// 0x609910
void JpRealMyRoomMain::OnCardPageIndexInit(intptr_t param) {
	FrListBox* wnd = DYNAMIC_CAST(FrListBox, param);
	if (wnd != NULL && m_cardBook != NULL) {
		m_cardBook->m_pageIndex = wnd;
		wnd->SetVisible(false);
	}
}

// 0x6099d0
void JpRealMyRoomMain::OnCardPropertyInit(intptr_t param) {
	FrArea* wnd = DYNAMIC_CAST(FrArea, param);
	if (wnd != NULL && m_cardBook != NULL) {
		m_cardBook->m_propertyWnd = wnd;
		wnd->SetVisible(false);
	}
}

// Port body (JP 0x609a40, 0x609b50 ... 0x609e20, 0x609d90: the seven card
// tabs): left out the button's +0x104 / +0x108 floats.
static void BindCardTab(JpCardBook* pBook, FrButton* pButton, int tab) {
	if (pButton != NULL && pBook != NULL) {
		pBook->m_tabButtons[tab] = pButton;
		pButton->SetVisible(false);
	}
}

void JpRealMyRoomMain::OnCardPackInit(intptr_t param) {
	BindCardTab(m_cardBook, DYNAMIC_CAST(FrButton, param), 0);
}

void JpRealMyRoomMain::OnCardVolume1Init(intptr_t param) {
	BindCardTab(m_cardBook, DYNAMIC_CAST(FrButton, param), 1);
}

void JpRealMyRoomMain::OnCardVolume2Init(intptr_t param) {
	BindCardTab(m_cardBook, DYNAMIC_CAST(FrButton, param), 2);
}

void JpRealMyRoomMain::OnCardVolume3Init(intptr_t param) {
	BindCardTab(m_cardBook, DYNAMIC_CAST(FrButton, param), 3);
}

void JpRealMyRoomMain::OnCardVolume4Init(intptr_t param) {
	BindCardTab(m_cardBook, DYNAMIC_CAST(FrButton, param), 4);
}

void JpRealMyRoomMain::OnCardVolume5Init(intptr_t param) {
	BindCardTab(m_cardBook, DYNAMIC_CAST(FrButton, param), 5);
}

void JpRealMyRoomMain::OnCardSpInit(intptr_t param) {
	BindCardTab(m_cardBook, DYNAMIC_CAST(FrButton, param), 6);
}

// 0x609ad0
void JpRealMyRoomMain::OnCardUseInit(intptr_t param) {
	FrButton* wnd = DYNAMIC_CAST(FrButton, param);
	if (wnd != NULL && m_cardBook != NULL) {
		m_cardBook->m_useButton = wnd;
		wnd->SetVisible(false);
	}
}

// 0x609eb0
void JpRealMyRoomMain::OnCardPackSlot1Init(intptr_t param) { HideCardPackSlot(DYNAMIC_CAST(FrButton, param), 0); }
// 0x609f20
void JpRealMyRoomMain::OnCardPackSlot2Init(intptr_t param) { HideCardPackSlot(DYNAMIC_CAST(FrButton, param), 1); }
// 0x609f90
void JpRealMyRoomMain::OnCardPackSlot3Init(intptr_t param) { HideCardPackSlot(DYNAMIC_CAST(FrButton, param), 2); }
// 0x60a000
void JpRealMyRoomMain::OnCardPackSlot4Init(intptr_t param) { HideCardPackSlot(DYNAMIC_CAST(FrButton, param), 3); }
// 0x60a070
void JpRealMyRoomMain::OnCardPackSlot5Init(intptr_t param) { HideCardPackSlot(DYNAMIC_CAST(FrButton, param), 4); }
// 0x60a0e0
void JpRealMyRoomMain::OnCardPackSlot6Init(intptr_t param) { HideCardPackSlot(DYNAMIC_CAST(FrButton, param), 5); }
// 0x60a150
void JpRealMyRoomMain::OnCardPackSlot7Init(intptr_t param) { HideCardPackSlot(DYNAMIC_CAST(FrButton, param), 6); }
// 0x60a1c0
void JpRealMyRoomMain::OnCardPackSlot8Init(intptr_t param) { HideCardPackSlot(DYNAMIC_CAST(FrButton, param), 7); }

// 0x6102b0
void JpRealMyRoomMain::OnCardSlot1Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 0); }
// 0x610320
void JpRealMyRoomMain::OnCardSlot2Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 1); }
// 0x610390
void JpRealMyRoomMain::OnCardSlot3Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 2); }
// 0x610400
void JpRealMyRoomMain::OnCardSlot4Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 3); }
// 0x610470
void JpRealMyRoomMain::OnCardSlot5Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 4); }
// 0x6104e0
void JpRealMyRoomMain::OnCardSlot6Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 5); }
// 0x610550
void JpRealMyRoomMain::OnCardSlot7Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 6); }
// 0x6105c0
void JpRealMyRoomMain::OnCardSlot8Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 7); }
// 0x610630
void JpRealMyRoomMain::OnCardSlot9Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 8); }
// 0x6106a0
void JpRealMyRoomMain::OnCardSlot10Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 9); }
// 0x610710
void JpRealMyRoomMain::OnCardSlot11Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 10); }
// 0x610780
void JpRealMyRoomMain::OnCardSlot12Init(intptr_t param) { BindCardSlot(DYNAMIC_CAST(FrButton, param), 11); }

// Port body (JP 0x614260): left out the list's fill 0x6141b0 (the shared
// doc's card packs, +0x37e0).
void JpRealMyRoomMain::OnCardPackListInit(intptr_t param) {
	m_cardPackList = DYNAMIC_CAST(FrListBox, param);
	if (m_cardPackList != NULL) {
		m_cardPackList->SetVisible(false);
	}
}

// Port body (JP 0x60a420): left out the 0xe7688c registration (0x7fe140).
void JpRealMyRoomMain::OnItemStoragePangInit(intptr_t param) {
	m_itemStoragePang = DYNAMIC_CAST(FrArea, param);
}

// Port body (JP 0x60a4a0): left out the +0x104 / +0x108 floats and the
// 0xe7688c registration.
void JpRealMyRoomMain::OnPangMinusInit(intptr_t param) {
	m_pangMinus = DYNAMIC_CAST(FrButton, param);
}

// Port body (JP 0x60a560): as OnPangMinusInit.
void JpRealMyRoomMain::OnPangPlusInit(intptr_t param) {
	m_pangPlus = DYNAMIC_CAST(FrButton, param);
}

// Port body (JP 0x60a620): left out the style bit 0x200 cleared at +0x44.
void JpRealMyRoomMain::OnItemStorageListInit(intptr_t param) {
	m_itemStorageList = DYNAMIC_CAST(FrListBox, param);
	if (m_itemStorageList != NULL) {
		m_itemStorageList->UseRightButton(true);
	}
}

// Port body (JP 0x60a850): left out the 0xe7688c registration.
void JpRealMyRoomMain::OnItemStorageIndexInit(intptr_t param) {
	m_itemStorageIndex = DYNAMIC_CAST(FrListBox, param);
}

// Port body (JP 0x60a940): left out the +0x104 / +0x108 floats.
void JpRealMyRoomMain::OnItemStoragePageNextInit(intptr_t param) {
	m_itemStorageNext = DYNAMIC_CAST(FrButton, param);
	if (m_itemStorageNext != NULL) {
		m_itemStorageNext->SetVisible(false);
	}
}

// Port body (JP 0x60aa00): as OnItemStoragePageNextInit.
void JpRealMyRoomMain::OnItemStoragePagePrevInit(intptr_t param) {
	m_itemStoragePrev = DYNAMIC_CAST(FrButton, param);
	if (m_itemStoragePrev != NULL) {
		m_itemStoragePrev->SetVisible(false);
	}
}

// Port body (JP 0x60aac0): left out the +0x104 / +0x108 floats, the
// 0xe7688c registration and the "TanLines" byte +0x7a = 1.
void JpRealMyRoomMain::OnItemStorageCloseInit(intptr_t param) {
	m_itemStorageClose = DYNAMIC_CAST(FrButton, param);
}

// Port body (JP 0x60ab70): left out the 0xe7688c registration.
void JpRealMyRoomMain::OnItemStorageDragDropInit(intptr_t param) {
	m_itemStorageDragDrop = DYNAMIC_CAST(FrArea, param);
}

// 0x61e4e0
void JpRealMyRoomMain::SetMainTab(int tab, bool force) {
	if (force) {
		m_mainTab->Select(m_mainTabIndex);
	} else if (m_mainTabIndex == tab) {
		return;
	}

	m_mainTabIndex = tab;
	m_subTab->ClearTab();

	const char* names[3] = { NULL };
	switch (m_mainTabIndex) {
	case 0:
		SetSubTabImg(names, 0x16);
		m_subTab->AddTab(names, 0x16, NULL);
		for (int i = 2; i <= 5; i++) {
			SetSubTabImg(names, i);
			m_subTab->AddTab(names, i, NULL);
		}
		if (IsLocalContent((localContentType_t)0x58)) {
			SetSubTabImg(names, 0x1b);
			m_subTab->AddTab(names, 0x1b, NULL);
		}
		SetSubTabImg(names, 0x17);
		m_subTab->AddTab(names, 0x17, NULL);
		m_upDownGroup->SetVisible(true);
		ShowUpgradeButtons(true, false);
		break;
	case 1:
		for (int i = 7; i <= 10; i++) {
			SetSubTabImg(names, i);
			m_subTab->AddTab(names, i, NULL);
		}
		if (IsLocalContent((localContentType_t)0x52)) {
			SetSubTabImg(names, 0x1c);
			m_subTab->AddTab(names, 0x1c, NULL);
		}
		break;
	case 2:
		SetSubTabImg(names, 0xc);
		m_subTab->AddTab(names, 0xb, NULL);
		m_upDownGroup->SetVisible(true);
		ShowUpgradeButtons(true, false);
		break;
	case 3:
		SetSubTabImg(names, 0xd);
		m_subTab->AddTab(names, 0xd, NULL);
		m_upDownGroup->SetVisible(true);
		ShowUpgradeButtons(true, false);
		break;
	case 4:
		for (int i = 0xe; i <= 0x11; i++) {
			SetSubTabImg(names, i);
			m_subTab->AddTab(names, i, NULL);
		}
		SetSubTabImg(names, 0x22);
		m_subTab->AddTab(names, 0x22, NULL);
		SetSubTabImg(names, 0x23);
		m_subTab->AddTab(names, 0x23, NULL);
		SetSubTabImg(names, 0x20);
		m_subTab->AddTab(names, 0x20, NULL);
		m_upDownGroup->SetVisible(false);
		ShowUpgradeButtons(false, false);
		break;
	case 5:
		for (int i = 0x13; i <= 0x15; i++) {
			SetSubTabImg(names, i);
			m_subTab->AddTab(names, i, NULL);
		}
		if (IsLocalContent((localContentType_t)0x14)) {
			SetSubTabImg(names, 0x19);
			m_subTab->AddTab(names, 0x19, NULL);
		}
		if (IsLocalContent((localContentType_t)0x50)) {
			SetSubTabImg(names, 0x18);
			m_subTab->AddTab(names, 0x18, NULL);
		}
		SetSubTabImg(names, 0x26);
		m_subTab->AddTab(names, 0x26, NULL);
		m_upDownGroup->SetVisible(false);
		ShowUpgradeButtons(false, false);
		break;
	}

	m_itemDesc->SetVisible(false);
	m_itemDescBg->SetVisible(false);
	if (m_preview == NULL) {
		SetSubTab(m_subTabIndex, true);
	}
}

// The JP 0xb2e230 (fastcall, the sub tab in ecx; the KR
// CTaskMain::GetCategoryFlag with the JP sub tab numbers): the same table
// as JpShopMainBuild.cpp's file-local copy (one shared declaration is a
// REQUEST); its VA marker stays there.
static unsigned long MyRoomCategoryFlag(int tab) {
	switch (tab) {
	case 2:
	case 15:
		return 1;
	case 3:
	case 16:
		return 2;
	case 4:
		return 0x10;
	case 5:
		return 0x2C;
	case 6:
		return 0x40;
	case 17:
		return 3;
	case 22:
		return 0x3BF;
	case 27:
		return 0x380;
	case 29:
		return 7;
	case 30:
		return 9;
	case 31:
		return 8;
	case 35:
		return 6;
	}
	return 0;
}

// 0x636850 (thiscall on the JP CSharedDoc 0xe47dbc, name ours): true when
// the item id is in the trade list +0x111d8 (KR m_listTradeItem; the node's
// guid).
static bool IsInTradeList(unsigned long id) {
	std::list<sTradeItem>* trade = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
	for (std::list<sTradeItem>::iterator it = trade->begin(); it != trade->end(); ++it) {
		if (it->guid == id) {
			return true;
		}
	}
	return false;
}

// 0x604b40 (the KR EmptyCompares): the empty rows sort last.
static bool MyRoomEmptyCompare(const void* p1, const void* p2) {
	if (p1 != NULL && p2 == NULL) {
		return true;
	}
	return false;
}

// Port body (JP 0x605530): the JP waits first, when wait, for the +0x588
// another-hand job (0x98ad40(9999)), which the port does not model, then
// deletes the preview through its vtable [0] (deleting destructor, 1);
// the port never builds one (LoadPreview).
void JpRealMyRoomMain::ReleasePreview(bool wait) {
	(void)wait;
	m_preview = NULL;
}

// Port body (JP 0x605640): the JP waits for the +0x588 job, returns when
// the preview already shows *typeId (its +0x55c) unless force, then queues
// CAnotherHand::Work(+0x588, the CHandOwner at +0x120, 0, typeId, 1, -1).
// The +0x588 job and the preview (the character model drawn by the "pet"
// owner draw) are not ported: nothing is queued.
void JpRealMyRoomMain::LoadPreview(const uint32_t* typeId, bool force) {
	(void)typeId;
	(void)force;
}

// Port body (JP 0x61af80, the KR BuildDisplayStand). Written: the list cleared, the sub tab
// 0x16 case (jump table index 12, 0x61b5a4: the character's parts, the
// extra items and, under local content 0x58, the item list, with the
// shown character's parts and cards selected) and the tail (top row
// reset, padded to 8 rows, empty rows last unless sub tab 0x13). The other
// 23 cases (0x61afdd ... 0x61c7c9) are not written: their sub tabs show
// an empty list.
void JpRealMyRoomMain::BuildDisplayStand(int subTab) {
	if (m_itemList == NULL) {
		return;
	}

	m_itemList->ClearItem();
	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	switch (subTab) {
	case 0x16: {
		unsigned long category = MyRoomCategoryFlag(subTab);
		// JP +0x3734 (KR m_myPartsList).
		for (std::list<sItemInfo>::iterator it = pDoc->m_myPartsList.begin(); it != pDoc->m_myPartsList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (IsInTradeList(pInfo->m_serial)) {
				continue;
			}
			if (!S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
				continue;
			}
			if (IsLocalContent((localContentType_t)0x52)) {
				IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->m_typeId);
				if (pPart != NULL) {
					// The JP also copies a third short (part +0x22c to the
					// JP record's +0xa8), which the KR sItemInfo lacks.
					pInfo->m_charCardSlotCount = pPart->m_charCardSlots;
					pInfo->m_caddieCardSlotCount = pPart->m_caddieCardSlots;
				}
			}
			m_itemList->AddItem(pInfo);
		}

		// The worn parts: by type id, and by item id too when the record
		// has one for that slot (JP +0x3704 map find of +0xa2a).
		for (int i = 0; i < 24; i++) {
			if (m_pCharParts == NULL) {
				break;
			}
			for (FrListBox::ITEM_LIST::iterator it = m_itemList->m_itemList.begin(); it != m_itemList->m_itemList.end(); ++it) {
				FrListItem* pItem = *it;
				sItemInfo* pInfo = (sItemInfo*)pItem->pData;
				std::map<unsigned int, sCharacterInfo>::iterator cit = pDoc->m_characterMap.find(pDoc->m_myCharacterId);
				if (cit == pDoc->m_characterMap.end()) {
					continue;
				}
				int slot = (pInfo->m_typeId >> 13) & 0x1F;
				if (cit->second.partIds[slot] == 0) {
					if (pInfo->m_typeId != m_pCharParts[i]) {
						continue;
					}
				} else if (pInfo->m_typeId != m_pCharParts[i] || pInfo->m_serial != m_charPartIds[i]) {
					continue;
				}
				m_itemList->SelectItem(pItem, false);
				break;
			}
		}

		// JP +0x37bc (KR m_myExtraItemList3).
		for (std::list<sItemInfo>::iterator it = pDoc->m_myExtraItemList3.begin(); it != pDoc->m_myExtraItemList3.end(); ++it) {
			m_itemList->AddItem(&*it);
		}

		// The worn cards.
		for (int i = 0; i < 5; i++) {
			if (m_pCharCards == NULL) {
				break;
			}
			for (FrListBox::ITEM_LIST::iterator it = m_itemList->m_itemList.begin(); it != m_itemList->m_itemList.end(); ++it) {
				sItemInfo* pInfo = (sItemInfo*)(*it)->pData;
				if (pInfo != NULL && pInfo->m_serial != 0 && pInfo->m_typeId == m_pCharCards[i]) {
					m_itemList->SelectItem(*it, false);
					break;
				}
			}
		}

		if (!IsLocalContent((localContentType_t)0x58)) {
			break;
		}

		// JP +0x3878 (the port's m_myItemList, JpPetFrameGlue.cpp).
		for (std::list<sItemInfo>::iterator it = pDoc->m_myItemList.begin(); it != pDoc->m_myItemList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (!S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
				continue;
			}
			if (IsLocalContent((localContentType_t)0x52)) {
				IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->m_typeId);
				if (pPart != NULL) {
					pInfo->m_charCardSlotCount = pPart->m_charCardSlots;
					pInfo->m_caddieCardSlotCount = pPart->m_caddieCardSlots;
				}
			}
			m_itemList->AddItem(pInfo);
		}

		// Its worn parts again (the JP's lower_bound of +0xa2a; under local
		// content 0x52 the item id test of the record's slot, else the
		// type id only).
		for (int i = 0; i < 24; i++) {
			if (m_pCharParts == NULL) {
				break;
			}
			for (FrListBox::ITEM_LIST::iterator it = m_itemList->m_itemList.begin(); it != m_itemList->m_itemList.end(); ++it) {
				FrListItem* pItem = *it;
				sItemInfo* pInfo = (sItemInfo*)pItem->pData;
				if (pInfo == NULL) {
					continue;
				}
				if (IsLocalContent((localContentType_t)0x52)) {
					std::map<unsigned int, sCharacterInfo>::iterator cit = pDoc->m_characterMap.find(pDoc->m_myCharacterId);
					if (cit == pDoc->m_characterMap.end()) {
						continue;
					}
					int slot = (pInfo->m_typeId >> 13) & 0x1F;
					if (cit->second.partIds[slot] == 0) {
						if (pInfo->m_typeId != m_pCharParts[i]) {
							continue;
						}
					} else if (pInfo->m_typeId != m_pCharParts[i] || pInfo->m_serial != m_charPartIds[i]) {
						continue;
					}
				} else if (pInfo->m_typeId != m_pCharParts[i]) {
					continue;
				}
				m_itemList->SelectItem(pItem, false);
				break;
			}
		}
		break;
	}
	}

	m_itemList->ResetTopRow();
	for (int n = m_itemList->GetCurrentItemSize(true); n < 8; n++) {
		m_itemList->AddItem(NULL);
	}
	if (subTab != 0x13) {
		m_itemList->SortItem(MyRoomEmptyCompare);
	}
}

// 0x61eaa0 (the KR ChangeCharacter). Left out: the note dialog result
// (+0x124 == 1, OnNoteDlgResult) and 0x61aa80 (the +0x12c control's
// vtable +0x18 with 1, +0x528 = 0, SetLevelBar) after the main tab 0
// rebuild.
void JpRealMyRoomMain::RefreshCharacter(unsigned long id, bool onlyCurrent) {
	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	if (onlyCurrent) {
		pDoc->m_characterMap[pDoc->m_myCharacterId];
		if (pDoc->m_myCharacterId != id) {
			return;
		}
	}

	std::map<unsigned int, sCharacterInfo>::iterator it = pDoc->m_characterMap.find(id);
	if (it == pDoc->m_characterMap.end()) {
		return;
	}

	sCharacterInfo& info = it->second;
	m_charTypeId = info.typeId;
	m_pCharParts = info.partTypeIds;
	if (IsLocalContent((localContentType_t)0x58)) {
		memcpy(m_charPartIds, info.partIds, sizeof(m_charPartIds));
	}
	m_pCharCards = info.auxParts.tids;
	m_pCharLevels = info.unknown6A.level;
	m_charSerial = info.id;
	pDoc->m_myCharacterId = info.id;

	if (MyUID() != pDoc->m_myRoomOwnerUID) {
		if (m_mainTabIndex == 0) {
			pDoc->BuildMyPartTidList(info);
		}
		return;
	}

	if (m_mainTabIndex == 0) {
		pDoc->BuildMyPartTidList(info);
		BuildDisplayStand(m_subTabIndex);
		LoadPreview(&m_charTypeId, false);
		pDoc->m_characterMap[pDoc->m_myCharacterId];
	} else if (m_mainTabIndex == 4) {
		pDoc->BuildMyPartTidList(info);
		BuildDisplayStand(m_subTabIndex);
		LoadPreview(&m_charTypeId, false);
	}

	// 0x6091a0: item_list scrolled to its top.
	if (m_itemList != NULL && m_itemList->GetScrollBar() != NULL) {
		m_itemList->GetScrollBar()->SetCurTopRow(0);
	}
}

// 0x61d7a0. Left out: SetLevelBar 0x61a750 (not written), the JP
// SetItemDesc, the cases' club / aztec / gift / card helpers (0x61cc80 with
// +0x514, 0x61ce00 with +0x524, 0x61aab0, the "st_cutin" caption, the
// sub tab 0x18 shirt colour copy of the doc +0xa76..+0xa8a, 0x605720's card
// book part).
void JpRealMyRoomMain::SetSubTab(int subTab, bool force) {
	if (!force && m_subTabIndex == subTab && !m_subTabDirty) {
		return;
	}

	int prev = m_subTabIndex;
	m_subTabDirty = false;
	// Leaving the character pages for the pet pages 0xe-0x10 (or back)
	// reloads the preview (the KR bReloadPet).
	bool reload = false;
	if (prev == 0x16 || prev <= 5 || prev == 0x17 || prev == 0x1b || prev == 0x11) {
		if (subTab == 0xe || subTab == 0xf || subTab == 0x10) {
			reload = true;
		}
	} else if (prev == 0x22 && (subTab == 0xe || subTab == 0xf || subTab == 0x10)) {
		reload = true;
	}
	if (!reload) {
		if (prev == 0xe || prev == 0xf || prev == 0x10 || prev == 0x22) {
			if (m_mainTabIndex == 0) {
				reload = true;
			}
		} else if (m_mainTabIndex == 0 && prev == 0x11) {
			reload = true;
		}
	}

	if (subTab == 6) {
		subTab = 2;
	}

	m_subTabIndex = subTab;
	BuildDisplayStand(subTab);
	m_itemDesc->ClearLine();
	m_subTab->Select(m_subTabIndex);
	if (m_giftBg != NULL) {
		m_giftBg->SetVisible(false);
	}
	if (m_giftDesc != NULL) {
		m_giftDesc->SetVisible(false);
	}

	HideStatGroup();
	if (!m_itemList->IsVisible()) {
		m_itemList->SetVisible(true);
		// 0x605720(false), of which only the card pack list is kept.
		if (m_cardPackList != NULL) {
			m_cardPackList->SetVisible(false);
		}
	}

	// The jump table 0x61df40 over 0x61df80 (sub tab - 2).
	switch (m_subTabIndex) {
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 0x16:
	case 0x17:
	case 0x1b:
		m_itemList->SetMultiSelect(true);
		if (reload || m_uiState != 1) {
			if (RFLobbyDesktop::s_mode == 0) {
				ReleasePreview(true);
			} else {
				LoadPreview(&m_charTypeId, reload);
			}
		}
		ShowStatGroup();
		m_upDownGroup->SetVisible(true);
		break;
	case 7:
		m_itemList->SetMultiSelect(false);
		ShowStatGroup();
		m_upDownGroup->SetVisible(true);
		ShowUpgradeButtons(false, true);
		break;
	case 8:
		m_itemList->SetMultiSelect(false);
		m_upDownGroup->SetVisible(false);
		break;
	case 9:
	case 10:
		m_itemList->SetMultiSelect(false);
		ReleasePreview(true);
		m_upDownGroup->SetVisible(false);
		break;
	case 11:
		m_itemList->SetMultiSelect(false);
		ShowStatGroup();
		ReleasePreview(true);
		break;
	case 13:
		m_itemList->SetMultiSelect(false);
		ShowStatGroup();
		break;
	case 14:
	case 15:
	case 16:
	case 17:
		m_itemList->SetMultiSelect(false);
		LoadPreview(&m_charTypeId, reload);
		m_upDownGroup->SetVisible(false);
		break;
	case 0x13:
	case 0x14:
	case 0x15:
		m_itemList->SetMultiSelect(false);
		break;
	case 0x18:
		if (IsLocalContent((localContentType_t)0x50)) {
			m_itemList->SetMultiSelect(false);
			ReleasePreview(true);
		}
		break;
	case 0x1c:
		if (IsLocalContent((localContentType_t)0x52)) {
			m_itemList->SetMultiSelect(false);
			m_itemList->SetVisible(false);
			ReleasePreview(true);
			if (m_cardPackList != NULL) {
				m_cardPackList->SetVisible(true);
			}
		}
		m_upDownGroup->SetVisible(false);
		break;
	case 0x20:
		m_itemList->SetMultiSelect(true);
		m_upDownGroup->SetVisible(false);
		ReleasePreview(true);
		break;
	case 0x22:
	case 0x23:
		LoadPreview(&m_charTypeId, reload);
		break;
	}

	if (m_subTabIndex == 9) {
		if (m_slotBox != NULL) {
			m_slotBox->SetVisible(true);
		}
	} else if (m_slotBox != NULL) {
		m_slotBox->SetVisible(false);
	}

	if (RFLobbyDesktop::s_instance != NULL) {
		int avatarMsg;
		int caddieMsg;
		if (m_mainTabIndex == 0) {
			JpLobbyDesktop_SetBarSelection(5);
			avatarMsg = 0x1ce;
			caddieMsg = 0x1cf;
		} else if (m_mainTabIndex == 2) {
			JpLobbyDesktop_SetBarSelection(6);
			avatarMsg = 0x1cf;
			caddieMsg = 0x1ce;
		} else if (m_mainTabIndex == 4 && m_subTabIndex == 0x20) {
			JpLobbyDesktop_SetBarSelection(5);
			avatarMsg = 0x1ce;
			caddieMsg = 0x1cf;
		} else {
			JpLobbyDesktop_SetBarSelection(7);
			avatarMsg = 0x1cf;
			caddieMsg = 0x1cf;
		}

		IActor* avatar = AfxGetTask()->GetActor("LobbyAvatar");
		if (avatar != NULL) {
			avatar << MsgObject(0, avatarMsg, 1, 0, 0, 0, 0);
		}
		IActor* caddie = AfxGetTask()->GetActor("LobbyCaddie");
		if (caddie != NULL) {
			caddie << MsgObject(0, caddieMsg, 1, 0, 0, 0, 0);
		}
		JpLobbyDesktop_UpdateLobbyCamera();
	}

	// 0x6091a0: item_list scrolled to its top.
	if (m_itemList->GetScrollBar() != NULL) {
		m_itemList->GetScrollBar()->SetCurTopRow(0);
	}
}

// 0x6056a0
void JpRealMyRoomMain::ShowStatGroup(void) {
	if (m_statGroup != NULL) {
		m_statGroup->SetVisible(true);
	}
}

// 0x6056c0
void JpRealMyRoomMain::HideStatGroup(void) {
	if (m_statGroup != NULL) {
		m_statGroup->SetVisible(false);
	}
}

// 0x61f530. Left out: the test, under the +0x5a0 critical section, that
// the +0x588 another-hand job (+0x598 / +0x59c, 0x14-byte records) is
// done, else tab_main reselects the current tab (the job is not ported);
// ChangeCaddie 0x61c960 (the doc's +0xa26) on tab 2, ChangeMascot
// 0x61cb70 (+0xa8e) on tab 3 and 0x61aab0 on tabs 4 and 5 (not written).
void JpRealMyRoomMain::OnTabMainUp(void) {
	TButtonItem* pItem = m_mainTab->GetSelected();
	if (pItem == NULL) {
		return;
	}

	int tab = pItem->index;
	SetMainTab(tab, false);
	switch (tab) {
	case 0:
		SetSubTab(0x16, false);
		RefreshCharacter(WSingleton<CSharedDoc>::Instance()->m_myCharacterId, false);
		break;
	case 1:
		SetSubTab(7, false);
		break;
	case 2:
		SetSubTab(0xb, false);
		break;
	case 3:
		SetSubTab(0xd, false);
		break;
	case 4:
		SetSubTab(0xe, false);
		break;
	case 5:
		SetSubTab(0x13, false);
		ReleasePreview(true);
		break;
	}

	// Every tab ends with the parts sender.
	Net_EquipDresserParts();
}

// 0x61f6b0
void JpRealMyRoomMain::OnTabSubUp(void) {
	TButtonItem* pItem = m_subTab->GetSelected();
	if (pItem == NULL) {
		return;
	}

	SetSubTab(pItem->index, false);
}

// 0x620b90 (__fastcall in the JP: CreateForm<CRealMyRoomMain> 0x620a00,
// which also keeps the form in the thread-local +0xb4 slot)
FrWnd* JpRealMyRoomMain::Create(const char* name, FrWndManager* pManager) {
	return CreateForm<JpRealMyRoomMain>(pManager, NULL, name, NULL);
}

// 0xcd2220 (the static init: `s_registered =
// JpFrFormRegistry::Instance()->Register("realmyroom", creator 0x620b90)`)
static bool s_realMyRoomRegistered =
	JpFrFormRegistry::Instance()->Register("realmyroom", &JpRealMyRoomMain::Create);

// Port body (JP 0x7f6970).
JpCardBook::JpCardBook(void) {
	m_cardPage = NULL;
	m_bookWnd = NULL;
	m_propertyWnd = NULL;
	for (int i = 0; i < 7; i++) {
		m_tabButtons[i] = NULL;
	}
	m_useButton = NULL;
	m_pageIndex = NULL;
}

JpCardBook::~JpCardBook(void) {
}

// Port only (G11, JpRealMyRoomMain.h).
FrWnd* JpRealMyRoomMain_Create(const char* name, FrWndManager* pManager) {
	return JpRealMyRoomMain::Create(name, pManager);
}
