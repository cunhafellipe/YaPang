// JP-only game code: the JP CRealMyRoomMain (JpRealMyRoomMain,
// JpRealMyRoomMain.h), the lobby My Room page's "realmyroom" form, from the
// JP exe (node scripts/disasm.ts --jp 0x<va>), and its registration in the
// JP form registry. What is still a port body: JpRealMyRoomMain.h, "Status".
#include "JpRealMyRoomMain.h"

#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "frgroupbox.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "frtabbutton.h"
#include "frviewer.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "commonutil.h"
#include "UccManager.h"
#include "cardsystem.h"
#include "CodeTinker.h"
#include "exhibition.h"
#include "ItemManager.h"
#include "ItemStorage.h"
#include "JpLobbyGlue.h"
#include "JpMasterySlot.h"
#include "JpTanLines.h"
#include "RFLobbyDesktop.h"
#include "actor.h"
#include "avatar_avatar.h"
#include "frscrollbar.h"
#include "s5/Utilities.h"
#include "Packet.h"
#include "PangGiftDlg.h"
#include "RankingDlg.h"
#include "RealMyRoomData.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "user_info.h"
#include "wresrcmng.h"
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
	{ "middle_ui.pet", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnPetOwnerDraw },
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
	{ "stat_group.arrow_m", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&JpRealMyRoomMain::OnStatArrowInit },
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
	m_statModeWnd = NULL;
	m_mainTab = NULL;
	m_subTab = NULL;
	m_itemList = NULL;
	m_tabText[0] = NULL;
	m_tabText[1] = NULL;
	m_petArea = NULL;
	m_statGroup = NULL;
	for (int i = 0; i < 5; i++) {
		m_statValue[i] = NULL;
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
	m_caddieTypeId = 0;
	m_caddieSerial = 0;
	m_caddiePreviewTypeId = 0;
	m_mascotTypeId = 0;
	m_mascotSerial = 0;
	m_clubSerial = 0;
	m_ballTypeId = 0;
	m_statSource = 0;
	m_mainTabIndex = 0;
	m_subTabIndex = 0x16;
	m_subTabDirty = false;
	m_uiState = 0;
	m_itemDescLeft = 0.0f;
	m_itemDescBottom = 0.0f;
	m_itemHideDescLeft2 = 0.0f;
	m_itemHideDescBottom2 = 0.0f;
	m_furnitureTabIndex = 0;
	m_petExhibition = NULL;
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

// Port body (JP 0x61f500, the body 0x61d590): "AvatarChat" gets 0x58 (the
// desktop's: the avatar manager, which MakeRealMyRoom stopped in the avatar
// chat, is active again) and, in the avatar chat (RFLobbyDesktop::s_mode
// 1), the desktop's m_pendingTaskChange (+0x184) is cleared (else its
// SyncInputState keeps the bars' Refresh input off) and the player's
// avatar drops state 2 (ntAvatar::SetState 0x7a4820) that OnInit gave it.
// Left out: the members' destruction the port does not model; the card
// book is deleted here.
JpRealMyRoomMain::~JpRealMyRoomMain(void) {
	if (CTaskManager::Instance() != NULL) {
		AfxPostMsg(NULL, "AvatarChat", 0x58, 0, 0, 0, 0);
	}

	if (RFLobbyDesktop::s_mode == 1) {
		if (RFLobbyDesktop::s_instance != NULL) {
			RFLobbyDesktop::s_instance->ClearPendingTaskChange();
		}
		ntAvatarManager* manager = WSingleton<ntAvatarManager>::Instance();
		if (manager != NULL && manager->m_pMyAvatar != NULL) {
			manager->m_pMyAvatar->SubState(2);
		}
	}

	delete m_cardBook;
	m_cardBook = NULL;
}

// Port body (JP 0x620c30). Left out: the card book's slot reset 0x7f3860
// (its card page is not ported), the shared doc's +0x3704 character map
// and the 0x74 bytes at +0xa26 copied to +0x710 / +0x720,
// 0xbe7870 and the 0xe7688c
// tooltip rects under local content 0x70. Kept: the tab state, the main
// tab (0), the sub tab (0x16, SetSubTab a port body), the avatar chat's
// state 2 on the player's avatar (0xe73e10 +0xa4, ntAvatar::SetState
// 0x7a4820 | 2; dropped by the destructor), the lounge mode's
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
		ntAvatarManager* manager = WSingleton<ntAvatarManager>::Instance();
		if (manager != NULL && manager->m_pMyAvatar != NULL) {
			manager->m_pMyAvatar->AddState(2);
		}
	}

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

// Port body (JP 0x6165d0): the page sends what was changed (the equip
// senders, then CSharedDoc::SendEquipItemSlot 0x63de40), drops the preview,
// stops its loader (the resource manager's locks of the loader's thread,
// 0xb55a00, then 0x98af10) and ends as FrTaskForm::Shutdown (0x523610).
// Left out: the JP-only C->S 0x20 kind 10 (0x60e6c0: 8 bytes of the shared
// doc's +0xa92, which the port's CSharedDoc has no field for), the card
// book's +0xa78 object, 0x614530, 0x614410, the 0xe741e4 singleton and the
// RFWnd 0xe780e4 closed last.
bool JpRealMyRoomMain::Shutdown(void) {
	Net_EquipDresserParts();
	Net_EquipCharacter();
	Net_EquipCaddie();
	Net_EquipItem();
	Net_EquipGear();
	Net_EquipSkin();
	Net_EquipMascot();
	Net_SendEquipKind9();
	WSingleton<CSharedDoc>::Instance()->SendEquipItemSlot();
	ReleasePreview(true);
	g_resrcmng->UnlockAllByThread(m_petLoader.GetThreadID());
	m_petLoader.BreakWorkingThread();
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

// 0x611f50 (reached through CHandOwner at +0x120; pData is the type id's
// address LoadPreview queued)
void JpRealMyRoomMain::OnAnotherHand(int nWork, void* pData) {
	if (nWork == 0) {
		ShowPet(*(const uint32_t*)pData);
	}
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

// 0x607f40: the shared stat box's arrow (the shop's before / after mark) is
// hidden in My Room.
void JpRealMyRoomMain::OnStatArrowInit(intptr_t param) {
	FrArea* arrow = DYNAMIC_CAST(FrArea, param);
	if (arrow != NULL) {
		arrow->SetVisible(false);
	}
}

// 0x607fa0
void JpRealMyRoomMain::OnPowerBarInit(intptr_t param) {
	m_statValue[0] = DYNAMIC_CAST(FrStatic, param);
}

// 0x608010
void JpRealMyRoomMain::OnControlBarInit(intptr_t param) {
	m_statValue[1] = DYNAMIC_CAST(FrStatic, param);
}

// 0x608080
void JpRealMyRoomMain::OnImpactBarInit(intptr_t param) {
	m_statValue[2] = DYNAMIC_CAST(FrStatic, param);
}

// 0x6080f0
void JpRealMyRoomMain::OnSpinBarInit(intptr_t param) {
	m_statValue[3] = DYNAMIC_CAST(FrStatic, param);
}

// 0x608160
void JpRealMyRoomMain::OnCurveBarInit(intptr_t param) {
	m_statValue[4] = DYNAMIC_CAST(FrStatic, param);
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

// 0x60a420: the item storage's front controls are hidden and registered
// with the storage (0xe7688c, CItemStorage::AddFrontUI 0x7fe140), which
// shows them while it is open.
void JpRealMyRoomMain::OnItemStoragePangInit(intptr_t param) {
	m_itemStoragePang = DYNAMIC_CAST(FrArea, param);
	if (CItemStorage::IsInstantiated()) {
		CItemStorage::Instance()->AddFrontUI(m_itemStoragePang);
	}
}

// Port body (JP 0x60a4a0): left out the +0x104 / +0x108 floats.
void JpRealMyRoomMain::OnPangMinusInit(intptr_t param) {
	m_pangMinus = DYNAMIC_CAST(FrButton, param);
	if (m_pangMinus != NULL && CItemStorage::IsInstantiated()) {
		CItemStorage::Instance()->AddFrontUI(m_pangMinus);
	}
}

// Port body (JP 0x60a560): as OnPangMinusInit.
void JpRealMyRoomMain::OnPangPlusInit(intptr_t param) {
	m_pangPlus = DYNAMIC_CAST(FrButton, param);
	if (m_pangPlus != NULL && CItemStorage::IsInstantiated()) {
		CItemStorage::Instance()->AddFrontUI(m_pangPlus);
	}
}

// Port body (JP 0x60a620): left out the style bit 0x200 cleared at +0x44.
void JpRealMyRoomMain::OnItemStorageListInit(intptr_t param) {
	m_itemStorageList = DYNAMIC_CAST(FrListBox, param);
	if (m_itemStorageList != NULL) {
		m_itemStorageList->UseRightButton(true);
	}
}

// 0x60a850 (the storage registration as OnItemStoragePangInit)
void JpRealMyRoomMain::OnItemStorageIndexInit(intptr_t param) {
	m_itemStorageIndex = DYNAMIC_CAST(FrListBox, param);
	if (CItemStorage::IsInstantiated()) {
		CItemStorage::Instance()->AddFrontUI(m_itemStorageIndex);
	}
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

// Port body (JP 0x60aac0): left out the +0x104 / +0x108 floats.
void JpRealMyRoomMain::OnItemStorageCloseInit(intptr_t param) {
	m_itemStorageClose = DYNAMIC_CAST(FrButton, param);
	if (CItemStorage::IsInstantiated()) {
		CItemStorage::Instance()->AddFrontUI(m_itemStorageClose);
	}
	JpTanLines* tanLines = JpTanLines::Instance();  // 0x542f00
	if (tanLines != NULL) {  // port: the JP stores without a test
		tanLines->m_flag7a = true;
	}
}

// 0x60ab70 (the storage registration as OnItemStoragePangInit)
void JpRealMyRoomMain::OnItemStorageDragDropInit(intptr_t param) {
	m_itemStorageDragDrop = DYNAMIC_CAST(FrArea, param);
	if (CItemStorage::IsInstantiated()) {
		CItemStorage::Instance()->AddFrontUI(m_itemStorageDragDrop);
	}
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
	if (m_petExhibition == NULL) {
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

// 0x605530 (the KR HidePet; the JP deletes the preview through its virtual
// destructor)
void JpRealMyRoomMain::ReleasePreview(bool wait) {
	if (wait) {
		m_petLoader.WaitUntilWorkDone(9999);
	}

	if (m_petExhibition != NULL) {
		delete m_petExhibition;
		m_petExhibition = NULL;
	}
}

// 0x605640 (the JP exhibition's +0x55c is the KR GetTypeId)
void JpRealMyRoomMain::LoadPreview(const uint32_t* typeId, bool force) {
	m_petLoader.WaitUntilWorkDone(9999);
	if (force || m_petExhibition == NULL || m_petExhibition->GetTypeId() != *typeId) {
		m_petLoader.Work(this, 0, const_cast<uint32_t*>(typeId), true, -1);
	}
}

// 0x604ed0
void JpRealMyRoomMain::SetPetAreaRect(void) {
	switch (m_subTabIndex) {
	case 0xe:
	case 0xf:
	case 0x10:
		m_petArea = m_skinPet[0];
		break;
	case 0x11:
	case 0x23:
		m_petArea = m_skinPet[1];
		break;
	default:
		m_petArea = m_pet;
		break;
	}
}

// Port body (JP 0x60e8b0, the KR ShowPet with the JP sub tab numbers, KR +
// 1, and local content 0x57, KR 0x56 + 1). The JP builds a character in
// its own exhibition class (new 0x64c, ctor 0x604750, SetModel its vtable
// [2]) and the other kinds in the JP CExhibition (ctor 0x4ff080, its last
// argument -1); the port uses the KR CExhibition for both, with the JP
// placements and SetControlFlag(0x1f) (JP +0x564). Left out: a character
// in the lobby (RFLobbyDesktop::s_mode 0) outside the skin sub tabs, which
// the JP builds only while a UCC is drawn (0x605e70: local content 0x58 and
// the UCC draw dialog +0x228, not ported), sized after the lobby's window
// (0xe73ddc +0x28 +0x4c) and main_bg3; the UCC texture of the drawn part.
void JpRealMyRoomMain::ShowPet(unsigned long typeId) {
	if (typeId == 0) {
		return;
	}

	m_petLoader.Enter();
	if (m_petArea == NULL) {
		m_petLoader.Leave();
		return;
	}

	ReleasePreview(false);
	WRect rect = m_petArea->GetRect();

	CExhibition* pet = NULL;
	bool loaded = false;
	switch (typeId >> 26) {
	case 1: {
		bool skinTab = m_subTabIndex == 0xe || m_subTabIndex == 0xf || m_subTabIndex == 0x10;
		if (RFLobbyDesktop::s_mode == 0 && !skinTab) {
			m_petLoader.Leave();
			return;
		}
		if (skinTab) {
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.35f, true);
			pet->SetArea(rect.w, rect.h);
			pet->SetDistanceRange(16.0f, 20.0f);
			pet->SetDistance(18.0f, true, false);
		} else {
			pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
			pet->SetArea(rect.w, rect.h);
			pet->SetDistanceRange(8.0f, 12.0f);
			pet->SetDistance(10.0f, true, false);
		}
		CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
		loaded = pet->SetModel(pDoc->GetMyPartTidList(), pDoc->GetMyAuxPartTidList(), NULL, 3.0f, 0);
		break;
	}

	case 4:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h / 3.0f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistance(7.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 5:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistance(1.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 6:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistance(6.0f, true, false);
		pet->SetDistanceRange(5.0f, 7.0f);
		loaded = pet->SetModel(typeId);
		break;

	case 7:
	case 8:
		// Only in the avatar chat (the lobby shows them on its own actors).
		if (RFLobbyDesktop::s_mode == 0) {
			m_petLoader.Leave();
			return;
		}
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistanceRange(9.0f, 13.0f);
		pet->SetDistance(11.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 11:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistance(18.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 16:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f + 15.0f, 0.35f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistanceRange(4.0f, 8.0f);
		pet->SetDistance(6.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 18:
		if (!IsLocalContent((localContentType_t)0x57)) {
			m_petLoader.Leave();
			return;
		}
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w * 0.5f, rect.h * 0.5f);
		pet->SetDistance(15.0f, true, false);
		loaded = pet->SetModel(typeId);
		break;

	case 31:
		pet = new CExhibition(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, 0.0f, true);
		pet->SetArea(rect.w, rect.h);
		pet->SetDistanceRange(5.0f, 7.0f);
		if ((typeId & 0x3C00000) == 0xC00000) {
			pet->SetDistance(5.0f, true, false);
		} else {
			pet->SetDistance(6.0f, true, false);
		}
		loaded = pet->SetModel(typeId);
		break;

	default:
		m_petLoader.Leave();
		return;
	}

	if (!loaded) {
		delete pet;
	} else {
		pet->SetControlFlag(0x1f);
		m_petExhibition = pet;
	}

	m_petLoader.Leave();
}

// Port body (JP 0x60ca50, FrForm [9]; the KR OnProcess's preview part): the
// preview turns with the mouse except on the main tabs 1 and 4. Left out:
// ESCAPE closing the messenger (0xe73de8 0x905710), the skin pages' side
// slides (+0x97c / +0x984) and the 0x899640 tooltip statics.
void JpRealMyRoomMain::OnProc(const float dt) {
	if (m_petLoader.IsWorking(0)) {
		Sleep(10);
	} else if (m_petExhibition != NULL) {
		m_petLoader.Enter();

		bool rotate;
		switch (m_mainTabIndex) {
		case 1:
		case 4:
			rotate = false;
			break;
		default:
			rotate = true;
			break;
		}

		m_petExhibition->Process(dt, rotate);
		m_petLoader.Leave();
	}
}

// 0x61a750 (the KR SetLevelBar on the JP stat box: FrStatic captions in place
// of the KR gauges; 0x5e48e0 GetStatValue, 0x5e48a0 GetFreeMasteryPoints)
void JpRealMyRoomMain::SetLevelBar(void) {
	JpMasteryStatInfo statInfo;
	for (int i = 0; i < 5; i++) {
		int value = statInfo.GetStatValue(i);
		int max = statInfo.GetFreeMasteryPoints(i) + value;
		if (m_statValue[i] != NULL) {
			m_statValue[i]->SetCaption11(MakeStr("%d/%d", value, max));
		}
	}
}

// 0x613f70 (the KR IsInItemList)
bool JpRealMyRoomMain::IsInItemList(unsigned long typeId) {
	for (FrListBox::ITEM_LIST::iterator it = m_itemList->m_itemList.begin(); it != m_itemList->m_itemList.end(); ++it) {
		sItemInfo* pInfo = (sItemInfo*)(*it)->pData;
		if (pInfo == NULL) {
			continue;
		}
		if (pInfo->m_typeId == typeId) {
			return true;
		}
	}
	return false;
}

// 0x615ab0
void JpRealMyRoomMain::UnequipItem(DWORD* pItemId) {
	std::list<sItemInfo>& items = WSingleton<CSharedDoc>::Instance()->m_itemInfoList;
	for (std::list<sItemInfo>::iterator it = items.begin(); it != items.end(); ++it) {
		if (it->m_typeId == *pItemId) {
			if (!IsInItemList(*pItemId)) {
				m_itemList->AddItem(&*it);
				m_itemList->SortItem(MyRoomEmptyCompare);
			}
		}
	}

	*pItemId = 0;
}

// The worn parts of the shown character selected in item_list: by type id,
// and by item id too when the character record has one for that slot (the
// JP's +0x3704 map find of +0xa2a); byItemId false (local content 0x52
// off) compares the type id only. Inlined in each parts case of the JP
// BuildDisplayStand (name ours).
static void SelectWornParts(FrListBox* pList, const DWORD* pCharParts, const uint32_t* pCharPartIds, bool byItemId) {
	if (pCharParts == NULL) {
		return;
	}

	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	for (int i = 0; i < 24; i++) {
		for (FrListBox::ITEM_LIST::iterator it = pList->m_itemList.begin(); it != pList->m_itemList.end(); ++it) {
			FrListItem* pItem = *it;
			sItemInfo* pInfo = (sItemInfo*)pItem->pData;
			if (pInfo == NULL) {
				continue;
			}
			if (byItemId) {
				std::map<unsigned int, sCharacterInfo>::iterator cit = pDoc->m_characterMap.find(pDoc->m_myCharacterId);
				if (cit == pDoc->m_characterMap.end()) {
					continue;
				}
				int slot = (pInfo->m_typeId >> 13) & 0x1F;
				if (cit->second.partIds[slot] == 0) {
					if (pInfo->m_typeId != pCharParts[i]) {
						continue;
					}
				} else if (pInfo->m_typeId != pCharParts[i] || pInfo->m_serial != pCharPartIds[i]) {
					continue;
				}
			} else if (pInfo->m_typeId != pCharParts[i]) {
				continue;
			}
			pList->SelectItem(pItem, false);
			break;
		}
	}
}

// A part's card slot counts copied into its item record from the IFF part
// (the JP also copies a third short, part +0x22c to the JP record's +0xa8,
// which the KR sItemInfo lacks). Inlined in the JP (name ours).
static void CopyCardSlotCounts(sItemInfo* pInfo) {
	IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pInfo->m_typeId);
	if (pPart != NULL) {
		pInfo->m_charCardSlotCount = pPart->m_charCardSlots;
		pInfo->m_caddieCardSlotCount = pPart->m_caddieCardSlots;
	}
}

// True when one of the owned characters is the one a part is made for (the
// sub tab 0x17 rows are the parts no owned character can wear). Inlined in
// the JP (name ours).
static bool IsPartOfOwnedCharacter(unsigned long typeId) {
	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	for (std::map<unsigned int, sCharacterInfo>::const_iterator cit = pDoc->m_characterMap.begin(); cit != pDoc->m_characterMap.end(); ++cit) {
		if (((typeId >> 18) & 0xFF) == (cit->second.typeId & 0x3FFFFFF)) {
			return true;
		}
	}
	return false;
}

// Port body (JP 0x61af80, the KR BuildDisplayStand; jump table 0x61c82c over
// the byte table 0x61c88c, sub tab - 2): the KR cases on the JP sub tab
// numbers (KR + 1) and local content numbers (0x52, 0x57, 0x58: KR + 1;
// the item slots' 0x81 is the KR 0x84). JP additions kept: the parts in the
// trade list are left out (0x636850). Left out: the ball tab's
// 0x140000d8 first row under the doc's +0x812 bit 14, the item tab's
// 0x6336a0 / IFF flag (+0xc0 & 6) filters (the KR IsNonVisibleItem in
// their place), the cut-in tab's selection by 0x63f450 (the KR equipped
// skin test in its place), the trophy tab's fourth list (+0x3828), the
// card tab's pack list (0x6141b0) and card book page, and the sub tab
// 0x26 (the item set list +0xac0, 0x478f40).
void JpRealMyRoomMain::BuildDisplayStand(int subTab) {
	if (m_itemList == NULL) {
		return;
	}

	m_itemList->ClearItem();
	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	switch (subTab) {
	case 2:
	case 3:
	case 4: {
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
			CopyCardSlotCounts(pInfo);
			m_itemList->AddItem(pInfo);
		}
		SelectWornParts(m_itemList, m_pCharParts, m_charPartIds, true);
		break;
	}

	case 5: {
		unsigned long category = MyRoomCategoryFlag(subTab);
		for (std::list<sItemInfo>::iterator it = pDoc->m_myPartsList.begin(); it != pDoc->m_myPartsList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (!S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
				continue;
			}
			if (IsInTradeList(pInfo->m_serial)) {
				continue;
			}
			if (IsAngelWing(pInfo->m_typeId)) {
				if (!IsMannerPlayer() || pDoc->m_myLevel < 0x15) {
					continue;
				}
			}
			CopyCardSlotCounts(pInfo);
			m_itemList->AddItem(pInfo);
		}
		SelectWornParts(m_itemList, m_pCharParts, m_charPartIds, true);

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
				if (pInfo != NULL && pInfo->m_typeId == m_pCharCards[i]) {
					m_itemList->SelectItem(*it, false);
					break;
				}
			}
		}
		break;
	}

	case 0x1b: {
		unsigned long category = MyRoomCategoryFlag(subTab);
		// JP +0x3878 (the port's m_myItemList, JpPetFrameGlue.cpp).
		for (std::list<sItemInfo>::iterator it = pDoc->m_myItemList.begin(); it != pDoc->m_myItemList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (!S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
				continue;
			}
			if (IsLocalContent((localContentType_t)0x52)) {
				CopyCardSlotCounts(pInfo);
			}
			m_itemList->AddItem(pInfo);
		}
		SelectWornParts(m_itemList, m_pCharParts, m_charPartIds, IsLocalContent((localContentType_t)0x52));
		break;
	}

	case 0x16: {
		unsigned long category = MyRoomCategoryFlag(subTab);
		for (std::list<sItemInfo>::iterator it = pDoc->m_myPartsList.begin(); it != pDoc->m_myPartsList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (IsInTradeList(pInfo->m_serial)) {
				continue;
			}
			if (!S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
				continue;
			}
			if (IsLocalContent((localContentType_t)0x52)) {
				CopyCardSlotCounts(pInfo);
			}
			m_itemList->AddItem(pInfo);
		}
		SelectWornParts(m_itemList, m_pCharParts, m_charPartIds, true);

		for (std::list<sItemInfo>::iterator it = pDoc->m_myExtraItemList3.begin(); it != pDoc->m_myExtraItemList3.end(); ++it) {
			m_itemList->AddItem(&*it);
		}

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

		for (std::list<sItemInfo>::iterator it = pDoc->m_myItemList.begin(); it != pDoc->m_myItemList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (!S5::UnderCategory(m_charTypeId, pInfo->m_typeId, category)) {
				continue;
			}
			if (IsLocalContent((localContentType_t)0x52)) {
				CopyCardSlotCounts(pInfo);
			}
			m_itemList->AddItem(pInfo);
		}
		SelectWornParts(m_itemList, m_pCharParts, m_charPartIds, IsLocalContent((localContentType_t)0x52));
		break;
	}

	case 0x17:
		// The parts no owned character wears.
		for (std::list<sItemInfo>::iterator it = pDoc->m_myPartsList.begin(); it != pDoc->m_myPartsList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (pInfo->m_typeId & 0x600) {
				continue;
			}
			if (IsPartOfOwnedCharacter(pInfo->m_typeId)) {
				continue;
			}
			if (IsLocalContent((localContentType_t)0x52)) {
				CopyCardSlotCounts(pInfo);
			}
			m_itemList->AddItem(pInfo);
		}

		if (IsLocalContent((localContentType_t)0x58)) {
			for (std::list<sItemInfo>::iterator it = pDoc->m_myItemList.begin(); it != pDoc->m_myItemList.end(); ++it) {
				sItemInfo* pInfo = &*it;
				if (pInfo->m_typeId & 0x600) {
					continue;
				}
				if (IsPartOfOwnedCharacter(pInfo->m_typeId)) {
					continue;
				}
				m_itemList->AddItem(pInfo);
			}
		}
		break;

	case 7:
		// Club sets (JP +0x3744, KR m_myItemMap); the shown one is selected.
		for (std::map<unsigned int, sItemInfo>::iterator it = pDoc->m_myItemMap.begin(); it != pDoc->m_myItemMap.end(); ++it) {
			FrListItem* pItem = m_itemList->AddItem(&it->second);
			if (m_clubSerial == 0) {
				if (pDoc->m_myClubId == it->second.m_serial) {
					m_clubSerial = it->second.m_serial;
					m_itemList->SelectItem(pItem, true);
				}
			} else if (m_clubSerial == it->second.m_serial) {
				m_itemList->SelectItem(pItem, true);
			}
		}
		break;

	case 8:
		// Balls (JP +0x3750, KR m_myGearList).
		for (std::list<sItemInfo>::iterator it = pDoc->m_myGearList.begin(); it != pDoc->m_myGearList.end(); ++it) {
			FrListItem* pItem = m_itemList->AddItem(&*it);
			if (pDoc->m_myBallId == it->m_typeId) {
				m_ballTypeId = (uint32_t)it->m_typeId;
				m_itemList->SelectItem(pItem, true);
			}
		}
		break;

	case 9: {
		// Usable items (JP +0x375c, KR m_itemInfoList), less the equipped
		// ones; then the item slots.
		for (std::list<sItemInfo>::iterator it = pDoc->m_itemInfoList.begin(); it != pDoc->m_itemInfoList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (ItemManager()->GetCouponKind(pInfo->m_typeId)) {
				continue;
			}
			if (pInfo->m_typeId & 0x2000000) {
				continue;
			}

			int equipped = 0;
			for (int k = 0; k < 10; k++) {
				if (pInfo->m_typeId == pDoc->m_equipItem[k]) {
					equipped++;
				}
			}
			if (pInfo->m_count - equipped > 0) {
				FrListItem* pItem = m_itemList->AddItem(pInfo);
				pItem->AddNoRBtnIdx(1);
				IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pInfo->m_typeId);
				if (pCommon != NULL && !(pCommon->m_flags0 & 1)) {
					pItem->AddNoRBtnIdx(2);
				}
			}
		}

		if (m_itemSlot == NULL) {
			break;
		}

		m_itemSlot->ClearItem();
		int slotCount = pDoc->GetNumAddItemSlotByMascot() + 8;
		// Port: the JP port has no card manager (its instance is a missing
		// symbol's zero stub): no card period slots without it.
		CCardManager* pCards = CCardManager::Instance();
		if (IsLocalContent((localContentType_t)0x81)) {
			if (IsLocalContent((localContentType_t)0x52) && pCards != NULL) {
				CCardManager::Instance()->SetPlayerIndex(0xFF);
				CCardManager::Instance()->CalcCardPeriodAndStatus();
				int cardSlots = CCardManager::Instance()->GetCardPeriodSlot();
				if (cardSlots > 0) {
					slotCount += cardSlots;
				}
			}
			for (int k = 0; k < 10; k++) {
				DWORD* pEquip = &pDoc->m_equipItem[k];
				if (k < slotCount) {
					m_itemSlot->AddItem(pEquip);
				} else {
					UnequipItem(pEquip);
				}
			}
		} else {
			if (IsLocalContent((localContentType_t)0x52) && pCards != NULL) {
				CCardManager::Instance()->SetPlayerIndex(0xFF);
				CCardManager::Instance()->CalcCardPeriodAndStatus();
				slotCount += CCardManager::Instance()->GetCardPeriodSlot();
			}
			for (int k = 0; k < slotCount; k++) {
				m_itemSlot->AddItem(&pDoc->m_equipItem[k]);
			}
		}
		for (int n = m_itemSlot->GetCurrentItemSize(true); n < 10; n++) {
			m_itemSlot->AddItem(NULL);
		}
		break;
	}

	case 10:
		// The other items (type id bit 0x2000000), but the sub tab 0x19's.
		for (std::list<sItemInfo>::iterator it = pDoc->m_itemInfoList.begin(); it != pDoc->m_itemInfoList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (ItemManager()->GetCouponKind(pInfo->m_typeId)) {
				continue;
			}
			if (pInfo->m_typeId == 0x1A000042) {
				continue;
			}
			if (ItemManager()->IsNonVisibleItem(pInfo->m_typeId) == true) {
				continue;
			}
			if (pInfo->m_typeId & 0x2000000) {
				FrListItem* pItem = m_itemList->AddItem(pInfo);
				if (CCodeTinker().IsNeedOpenMiniButton(pInfo->m_typeId)) {
					pItem->AddNoRBtnIdx(0);
				}
			}
		}
		break;

	case 0x19:
		if (IsLocalContent((localContentType_t)0x14)) {
			for (std::list<sItemInfo>::iterator it = pDoc->m_itemInfoList.begin(); it != pDoc->m_itemInfoList.end(); ++it) {
				sItemInfo* pInfo = &*it;
				if ((pInfo->m_typeId & 0x2000000) && pInfo->m_typeId == 0x1A000042) {
					FrListItem* pItem = m_itemList->AddItem(pInfo);
					pItem->AddNoRBtnIdx(0);
				}
			}
		}
		break;

	case 0xe:
	case 0xf:
	case 0x10:
	case 0x11:
	case 0x23: {
		// Skins (JP +0x3798, KR m_mySkinList) of the sub tab's kind; the
		// equipped ones are selected.
		unsigned long category = MyRoomCategoryFlag(subTab);
		for (std::list<sItemInfo>::iterator it = pDoc->m_mySkinList.begin(); it != pDoc->m_mySkinList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (category == 5 || ((pInfo->m_typeId >> 22) & 0xF) == category) {
				FrListItem* pItem = m_itemList->AddItem(pInfo);
				eEquipSkin skin;
				if (S5::UTIL::GetEquipSkinIndex(skin, pInfo->m_typeId)) {
					if (pDoc->m_equipSkin[skin] == pInfo->m_typeId) {
						m_itemList->SelectItem(pItem, true);
					}
				}
			}
		}
		break;
	}

	case 0x22:
		// Cut-ins of the shown character (or for everyone).
		for (std::list<sItemInfo>::iterator it = pDoc->m_mySkinList.begin(); it != pDoc->m_mySkinList.end(); ++it) {
			sItemInfo* pInfo = &*it;
			if (!S5::ISCutinItem(pInfo->m_typeId)) {
				continue;
			}
			IFF_STRUCT::S5::sCutinInformation* pCutin = ItemManager()->FindCutinInformation(pInfo->m_typeId);
			if (pCutin == NULL) {
				continue;
			}
			if ((m_charTypeId & 0x3FFFFFF) == pCutin->m_charSerial || pCutin->m_charSerial >= 100) {
				FrListItem* pItem = m_itemList->AddItem(pInfo);
				eEquipSkin skin;
				if (S5::UTIL::GetEquipSkinIndex(skin, pInfo->m_typeId)) {
					if (pDoc->m_equipSkin[skin] == pInfo->m_typeId) {
						m_itemList->SelectItem(pItem, true);
					}
				}
			}
		}
		break;

	case 0xb:
		// Caddies (JP +0x3718, KR m_caddieMap); the shown one is selected.
		for (std::map<unsigned int, sCaddieInfo>::iterator it = pDoc->m_caddieMap.begin(); it != pDoc->m_caddieMap.end(); ++it) {
			FrListItem* pItem = m_itemList->AddItem(&it->second);
			IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(it->second.typeId);
			if ((it->second.flags & 2) && it->second.days <= 0 && it->second.cadItemTypeId == 0 && pCaddie != NULL && pCaddie->fee) {
				pItem->AddNoRBtnIdx(3);
			}
			if (m_caddieSerial == 0) {
				if (pDoc->m_myCaddieId == it->second.id) {
					m_caddieSerial = it->second.id;
					m_itemList->SelectItem(pItem, true);
				}
			} else if (m_caddieSerial == it->second.id) {
				m_itemList->SelectItem(pItem, true);
			}
		}
		break;

	case 0xd:
		// Mascots (JP +0x3728, KR m_myMascotMap).
		for (std::map<unsigned int, sMascotInfo>::iterator it = pDoc->m_myMascotMap.begin(); it != pDoc->m_myMascotMap.end(); ++it) {
			FrListItem* pItem = m_itemList->AddItem(&it->second);
			if (pDoc->m_myMascotId == it->second.id) {
				m_itemList->SelectItem(pItem, true);
			}
		}
		break;

	case 0x13: {
		// Trophies: the course medals of this season and the last, then the
		// special trophies, newest first.
		m_trophyList.clear();
		sTrophyStatistics& trophy = pDoc->m_myTrophy;
		sTrophyStatistics& oldTrophy = pDoc->m_oldSeasonTrophy;
		for (int course = 0; course <= 12; course++) {
			for (int medal = 2; medal >= 0; medal--) {
				if (trophy.m_medals[course][medal] > 0 || oldTrophy.m_medals[course][medal] > 0) {
					sSpecialTrophy special;
					special.m_count = trophy.m_medals[course][medal] + oldTrophy.m_medals[course][medal];
					special.m_typeId = ((course | 0x2C00) << 16) | medal;
					special.m_id = 0;
					m_trophyList.push_back(special);
				}
			}
		}
		for (std::list<sSpecialTrophy>::iterator it = pDoc->m_mySpecialTrophies.begin(); it != pDoc->m_mySpecialTrophies.end(); ++it) {
			m_trophyList.push_back(*it);
		}
		for (std::list<sSpecialTrophy>::iterator it = pDoc->m_specialTrophies.begin(); it != pDoc->m_specialTrophies.end(); ++it) {
			m_trophyList.push_back(*it);
		}
		for (std::list<sSpecialTrophy>::reverse_iterator rit = m_trophyList.rbegin(); rit != m_trophyList.rend(); ++rit) {
			m_itemList->AddItem(&*rit);
		}
		break;
	}

	case 0x14:
		// Coupons.
		for (std::list<sItemInfo>::iterator it = pDoc->m_itemInfoList.begin(); it != pDoc->m_itemInfoList.end(); ++it) {
			if (ItemManager()->GetCouponKind(it->m_typeId)) {
				m_itemList->AddItem(&*it);
			}
		}
		break;

	case 0x15:
		// Quests (JP +0x3834).
		for (std::list<sQuest>::iterator it = pDoc->m_questList.begin(); it != pDoc->m_questList.end(); ++it) {
			m_itemList->AddItem(&*it);
		}
		break;

	case 0x18:
		// Replays (JP +0x3768).
		if (IsLocalContent((localContentType_t)0x50)) {
			pDoc->ReplaySavFileCheck();
			for (std::list<sReplayFileInfo>::iterator it = pDoc->m_replayFileList.begin(); it != pDoc->m_replayFileList.end(); ++it) {
				m_itemList->AddItem(&*it);
			}
		}
		break;

	case 0x1c:
		// Cards (JP +0x37e0).
		if (IsLocalContent((localContentType_t)0x52)) {
			for (std::list<sCards>::iterator it = pDoc->m_myCardList.begin(); it != pDoc->m_myCardList.end(); ++it) {
				m_itemList->AddItem(&*it);
			}
		}
		break;

	case 0x21:
		// Furniture (JP +0x37d4).
		for (std::list<sFurniture_List>::iterator it = pDoc->m_furnitureList.begin(); it != pDoc->m_furnitureList.end(); ++it) {
			m_itemList->AddItem(&*it);
		}
		break;

	case 0x1d:
	case 0x1e:
	case 0x1f:
		if (IsLocalContent((localContentType_t)0x57)) {
			unsigned long category = MyRoomCategoryFlag(subTab);
			for (std::list<sFurniture_List>::iterator it = pDoc->m_furnitureList.begin(); it != pDoc->m_furnitureList.end(); ++it) {
				if (((it->m_typeId >> 11) & 0x7FF) == category) {
					m_itemList->AddItem(&*it);
				}
			}
		}
		break;

	case 0x20:
		for (std::list<sFurniture_List>::iterator it = pDoc->m_furnitureList.begin(); it != pDoc->m_furnitureList.end(); ++it) {
			int kind = (it->m_typeId >> 11) & 0x7FF;
			if ((kind >= 0 && kind < 7) || kind >= 10) {
				m_itemList->AddItem(&*it);
			}
		}
		break;
	}

	m_itemList->ResetTopRow();
	for (int n = m_itemList->GetCurrentItemSize(true); n < 8; n++) {
		m_itemList->AddItem(NULL);
	}
	if (subTab != 0x13) {
		m_itemList->SortItem(MyRoomEmptyCompare);
	}
}

// 0x61aa80
void JpRealMyRoomMain::ShowMainModel(void) {
	if (m_statModeWnd != NULL) {
		m_statModeWnd->Enable(true);
	}
	m_statSource = 0;
	SetLevelBar();
}

// 0x61aab0
void JpRealMyRoomMain::ShowItemModel(void) {
	if (m_statModeWnd != NULL) {
		m_statModeWnd->Enable(false);
	}
	m_statSource = 1;
	SetLevelBar();
}

// Port body (JP 0x61c960, the KR ChangeCaddie). Left out: the shared doc's
// +0x1103c copy of the id, and in the lobby (RFLobbyDesktop::s_mode 0) the
// doc's 0x643f40 with the preview type id before the "LobbyCaddie" message
// 0x1a9.
bool JpRealMyRoomMain::ChangeCaddie(uint32_t id) {
	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	bool changed = false;
	std::map<unsigned int, sCaddieInfo>::iterator it = pDoc->m_caddieMap.find(id);
	if (it == pDoc->m_caddieMap.end()) {
		m_caddieTypeId = 0;
		m_caddieSerial = 0;
		pDoc->m_myCaddieId = 0;
		if (m_subTabIndex == 0xb) {
			ReleasePreview(true);
			m_itemList->UnselectAllItem();
			ShowMainModel();
			IActor* caddie = AfxGetTask()->GetActor("LobbyCaddie");
			if (caddie != NULL) {
				caddie << MsgObject(0, 0x1a6, 0, 0, 0, 0, 0);
			}
		}
		return changed;
	}

	m_caddieSerial = (uint32_t)it->second.id;
	m_caddieTypeId = (uint32_t)it->second.typeId;
	m_caddiePreviewTypeId = (uint32_t)(it->second.cadItemTypeId != 0 ? it->second.cadItemTypeId : it->second.typeId);
	IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(m_caddieTypeId);
	if (pCaddie != NULL && (pCaddie->m_level & 0x7f) <= pDoc->m_myLevel) {
		if (it->second.id != pDoc->m_myCaddieId) {
			changed = true;
		}
		pDoc->m_myCaddieId = it->second.id;
	}

	if (m_subTabIndex != 0xb) {
		return changed;
	}

	BuildDisplayStand(0xb);
	if (RFLobbyDesktop::s_mode != 0) {
		LoadPreview(&m_caddiePreviewTypeId, false);
	} else if (changed) {
		IActor* caddie = AfxGetTask()->GetActor("LobbyCaddie");
		if (caddie != NULL) {
			caddie << MsgObject(0, 0x1a9, 0, 0, 0, 0, 0);
		}
	}
	ShowMainModel();
	return changed;
}

// Port body (JP 0x61cb70, the KR ChangeMascot). Left out: the shared doc's
// +0x110a4 copy of the id.
void JpRealMyRoomMain::ChangeMascot(uint32_t id) {
	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	std::map<unsigned int, sMascotInfo>::iterator it = pDoc->m_myMascotMap.find(id);
	if (it == pDoc->m_myMascotMap.end()) {
		m_mascotTypeId = 0;
		m_mascotSerial = 0;
		pDoc->m_myMascotId = 0;
		if (m_subTabIndex == 0xd) {
			ReleasePreview(true);
			m_itemList->UnselectAllItem();
		}
	} else {
		m_mascotTypeId = (uint32_t)it->second.typeId;
		m_mascotSerial = (uint32_t)it->second.id;
		pDoc->m_myMascotId = it->second.id;
		if (m_subTabIndex == 0xd) {
			BuildDisplayStand(0xd);
			LoadPreview(&m_mascotTypeId, false);
			ShowItemModel();
		}
	}

	if (IsLocalContent((localContentType_t)0x52)) {
		pDoc->SendEquipItemSlot();
	} else {
		pDoc->SendEquipItemSlotByMascot();
	}
}

// 0x61eaa0 (the KR ChangeCharacter). Left out: the note dialog result
// (+0x124 == 1, OnNoteDlgResult) and, in ShowMainModel, the +0x12c control.
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
		ShowMainModel();
	} else if (m_mainTabIndex == 4) {
		pDoc->BuildMyPartTidList(info);
		BuildDisplayStand(m_subTabIndex);
		LoadPreview(&m_charTypeId, false);
	} else if (m_statSource == 1) {
		SetLevelBar();
	}

	// 0x6091a0: item_list scrolled to its top.
	if (m_itemList != NULL && m_itemList->GetScrollBar() != NULL) {
		m_itemList->GetScrollBar()->SetCurTopRow(0);
	}
}

// 0x61d7a0. Left out: the JP SetItemDesc, the cases' club / aztec / gift /
// card helpers (0x61cc80 with +0x514, 0x61ce00 with +0x524, 0x61aab0, the
// "st_cutin" caption, the sub tab 0x18 shirt colour copy of the doc
// +0xa76..+0xa8a, 0x605720's card book part) and 0x604f40 after
// SetPetAreaRect.
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
	SetLevelBar();
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

	SetPetAreaRect();

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

// 0x61f530: while the preview's job runs, tab_main goes back to the
// current tab.
void JpRealMyRoomMain::OnTabMainUp(void) {
	TButtonItem* pItem = m_mainTab->GetSelected();
	if (pItem == NULL) {
		return;
	}

	int tab = pItem->index;
	if (m_petLoader.IsWorking(0)) {
		m_mainTab->Select(m_mainTabIndex);
		return;
	}

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
		ChangeCaddie(WSingleton<CSharedDoc>::Instance()->m_myCaddieId);
		break;
	case 3:
		SetSubTab(0xd, false);
		ChangeMascot(WSingleton<CSharedDoc>::Instance()->m_myMascotId);
		break;
	case 4:
		SetSubTab(0xe, false);
		ShowItemModel();
		break;
	case 5:
		SetSubTab(0x13, false);
		ShowItemModel();
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
