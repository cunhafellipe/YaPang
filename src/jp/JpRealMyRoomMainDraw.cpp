// JP-only game code: the JP CRealMyRoomMain's (JpRealMyRoomMain,
// JpRealMyRoomMain.h) pictures, owner draws and equip senders, from the JP
// exe (node scripts/disasm.ts --jp 0x<va>), over the KR
// RealMyRoomMainUI.cpp bodies. The members they use sit at the JP offsets
// the functions read (the KR ones minus 0x230 for the pictures, minus 0x24c
// for the edited equipment and the item list areas).
//
// Not written yet (owner draws of the 0xe13488 map): item_list 0x6166b0
// (3839 bytes, the KR OnItemListOwnerDraw), pet 0x61f6d0, fitem_list
// 0x60fb50, cardpacklist 0x60ff10 (the JP tests the sub tab +0x530 == 0x1c
// and ends in the card book's 0x7f4540), card_book_back_bg 0x6056e0 /
// card_book 0x605700 (JpCardBook 0x7f5xxx draws), item_storage_bgi 0x60a3e0,
// item_storage_list 0x60a6b0, item_storage_index 0x60a8d0,
// item_storage_dragdrop 0x60abf0 (the JP CItemStorage 0xe7688c).
#include "JpRealMyRoomMain.h"

#include <map>

#include "JpMasterySlot.h"
#include "RFLobbyDesktop.h"
#include "ItemManager.h"
#include "MascotInfo.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "UccManager.h"
#include "SoundManager.h"
#include "TaskManager.h"
#include "actor.h"
#include "PartTidList.h"
#include "bitmap.h"
#include "commonutil.h"
#include "fredit.h"
#include "fresh.h"
#include "frgraphicinterface.h"
#include "frlistbox.h"
#include "frviewer.h"
#include "frwndmanager.h"
#include "s5/Utilities.h"
#include "shared/Localize.h"

// 0xe348e0..0xe34908: the stat dots' pictures, by stat, dot then slot
// ("ab_power", "ab_power_slot" ... "ab_curve_slot"): the same table as the
// KR dot_files, defined in ShopMain.cpp.
extern const char* dot_files[5][2];

// The JP character record (0x201 bytes) of `record`'s fields with the JP
// tail (mastery points, slot number, cards) the port keeps for `tailOwner`
// (requested from JpMasteryGlue.cpp's owner, next to
// JpCharacter_EncodeRecord: Net_EquipDresserParts sends an edited copy of
// a character map value, whose tail the port keys by the original).
// 0x6049d0 (?ReviseItemIconPoint@@YIXKAAVWPoint@@@Z, the KR one in
// RealMyRoomMainUI.cpp, compiled into the port): an item icon's offset in
// a list row by its type class.
void ReviseItemIconPoint(unsigned long typeId, WPoint& pt);
void JpCharacter_EncodeRecordWithTailOf(WSendPacket& packet, const sCharacterInfo* record, const sCharacterInfo* tailOwner);

// 0x608430 (the KR LoadMyRoomImages; the JP has no gift page arrows, no
// furniture menu icons / texts, no "rmr_arrange_btn" area; it adds the
// "poster_btn", "ucc_exhibit_btn" and "state_view" areas, "mark_begin" and
// "SelectItem_Mark", and tests the local contents 0x50 / 0x52 / 0x58 where
// the KR tests 0x4f / 0x51 / 0x57)
void JpRealMyRoomMain::LoadMyRoomImages(void) {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			m_pStatPointBitmap[i][j] = g_pFresh->RegisterBitmap(dot_files[i][j]);
		}
	}

	m_pListBgBitmap[0] = g_pFresh->RegisterBitmap("select_up");
	m_pListBgBitmap[1] = g_pFresh->RegisterBitmap("select_mid");
	m_pListBgBitmap[2] = g_pFresh->RegisterBitmap("select_low");
	m_pListEmptyBitmap = g_pFresh->RegisterBitmap("select_empty");

	for (int k = 0; k < 5; k++) {
		m_selectIcons[k].normal = g_pFresh->RegisterBitmap(MakeStr("select_%d_n", k + 1));
		m_selectIcons[k].over = g_pFresh->RegisterBitmap(MakeStr("select_%d_o", k + 1));
	}
	m_pSelectDivide = g_pFresh->RegisterBitmap(MakeStr("select_divide"));

	m_pListSelBitmap = g_pFresh->RegisterBitmap("select_cursor1");
	m_pListOverBitmap = g_pFresh->RegisterBitmap("select_cursor0");

	m_pStatPartBitmap[0][0] = g_pFresh->RegisterBitmap("stat_char_n");
	m_pStatPartBitmap[0][1] = g_pFresh->RegisterBitmap("stat_char_o");
	m_pStatPartBitmap[0][2] = g_pFresh->RegisterBitmap("stat_char_d");
	m_pStatPartBitmap[1][0] = g_pFresh->RegisterBitmap("stat_club_n");
	m_pStatPartBitmap[1][1] = g_pFresh->RegisterBitmap("stat_club_o");
	m_pStatPartBitmap[1][2] = g_pFresh->RegisterBitmap("stat_club_d");
	m_pStatPartBitmap[2][0] = g_pFresh->RegisterBitmap("stat_caddie_n");
	m_pStatPartBitmap[2][1] = g_pFresh->RegisterBitmap("stat_caddie_o");
	m_pStatPartBitmap[2][2] = g_pFresh->RegisterBitmap("stat_caddie_d");

	m_pStatAllBitmap[0] = g_pFresh->RegisterBitmap("stat_all_n");
	m_pStatAllBitmap[1] = g_pFresh->RegisterBitmap("stat_all_o");
	m_pStatAllBitmap[2] = g_pFresh->RegisterBitmap("stat_all_d");

	m_cardPackItemBmp[0] = g_pFresh->RegisterBitmap("item_base_n");
	m_cardPackItemBmp[1] = g_pFresh->RegisterBitmap("item_base_o");
	m_cardPackItemBmp[2] = g_pFresh->RegisterBitmap("item_base_d");
	m_cardPackItemBmp[3] = g_pFresh->RegisterBitmap("item_base_empty");

	m_itemListArea[0].normal = g_pFresh->RegisterBitmap("giftmove_btn_n");
	m_itemListArea[0].over = g_pFresh->RegisterBitmap("giftmove_btn_o");
	m_itemListArea[0].rect = WRect(103.0f, 66.0f, m_itemListArea[0].normal->Width(), m_itemListArea[0].normal->Height());

	m_itemListArea[4].normal = g_pFresh->RegisterBitmap("answer_btn_n");
	m_itemListArea[4].over = g_pFresh->RegisterBitmap("answer_btn_o");
	m_itemListArea[4].rect = WRect(144.0f, 66.0f, m_itemListArea[4].normal->Width(), m_itemListArea[4].normal->Height());

	m_itemListArea[1].normal = g_pFresh->RegisterBitmap("equip_btn_n");
	m_itemListArea[1].over = g_pFresh->RegisterBitmap("equip_btn_o");
	m_itemListArea[1].rect = WRect(103.0f, 66.0f, m_itemListArea[1].normal->Width(), m_itemListArea[1].normal->Height());

	m_itemListArea[2].normal = g_pFresh->RegisterBitmap("delete_btn_n");
	m_itemListArea[2].over = g_pFresh->RegisterBitmap("delete_btn_o");
	m_itemListArea[2].rect = WRect(144.0f, 66.0f, m_itemListArea[2].normal->Width(), m_itemListArea[2].normal->Height());

	m_itemListArea[3].normal = g_pFresh->RegisterBitmap("reemploy_btn_n");
	m_itemListArea[3].over = g_pFresh->RegisterBitmap("reemploy_btn_o");
	m_itemListArea[3].rect = WRect(144.0f, 66.0f, m_itemListArea[3].normal->Width(), m_itemListArea[3].normal->Height());

	m_itemListArea[5].normal = g_pFresh->RegisterBitmap("clock");
	m_itemListArea[5].over = g_pFresh->RegisterBitmap("clock");
	m_itemListArea[5].rect = WRect(32.0f, 30.0f, m_itemListArea[5].normal->Width(), m_itemListArea[5].normal->Height());

	if (IsLocalContent((localContentType_t)0x50)) {
		m_itemListArea[7].normal = g_pFresh->RegisterBitmap("replaymode_btn_n");
		m_itemListArea[7].over = g_pFresh->RegisterBitmap("replaymode_btn_o");
		m_itemListArea[7].rect = WRect(144.0f, 66.0f, m_itemListArea[7].normal->Width(), m_itemListArea[7].normal->Height());

		m_itemListArea[8].normal = g_pFresh->RegisterBitmap("replaydel_btn_n");
		m_itemListArea[8].over = g_pFresh->RegisterBitmap("replaydel_btn_o");
		m_itemListArea[8].rect = WRect(103.0f, 66.0f, m_itemListArea[8].normal->Width(), m_itemListArea[8].normal->Height());
	}

	if (IsLocalContent((localContentType_t)0x52)) {
		m_itemListArea[9].normal = g_pFresh->RegisterBitmap("card_btn_n");
		m_itemListArea[9].over = g_pFresh->RegisterBitmap("card_btn_o");
		m_itemListArea[9].rect = WRect(144.0f, 66.0f, m_itemListArea[9].normal->Width(), m_itemListArea[9].normal->Height());
	}

	if (IsLocalContent((localContentType_t)0x58)) {
		m_itemListArea[11].normal = g_pFresh->RegisterBitmap("ucc_draw_n");
		m_itemListArea[11].over = g_pFresh->RegisterBitmap("ucc_draw_o");

		m_itemListArea[13].normal = g_pFresh->RegisterBitmap("ucc_redraw_btn_n");
		m_itemListArea[13].over = g_pFresh->RegisterBitmap("ucc_redraw_btn_o");

		m_itemListArea[12].normal = g_pFresh->RegisterBitmap("ucc_copy_n");
		m_itemListArea[12].over = g_pFresh->RegisterBitmap("ucc_copy_o");

		m_uccDrawRects[0] = WRect(103.0f, 66.0f, m_itemListArea[11].normal->Width(), m_itemListArea[11].normal->Height());
		m_uccDrawRects[1] = WRect(144.0f, 66.0f, m_itemListArea[11].normal->Width(), m_itemListArea[11].normal->Height());

		m_pUccStateBitmap[0] = g_pFresh->RegisterBitmap("ucc_state_original");
		m_pUccStateBitmap[1] = g_pFresh->RegisterBitmap("ucc_state_copied");
		m_pUccStateBitmap[2] = g_pFresh->RegisterBitmap("ucc_state_temp_saved");
	}

	m_itemListArea[14].normal = g_pFresh->RegisterBitmap("ucc_exhibit_btn_n");
	m_itemListArea[14].over = g_pFresh->RegisterBitmap("ucc_exhibit_btn_o");
	m_itemListArea[14].rect = WRect(144.0f, 66.0f, m_itemListArea[14].normal->Width(), m_itemListArea[14].normal->Height());

	m_itemListArea[15].normal = g_pFresh->RegisterBitmap("state_view_n");
	m_itemListArea[15].over = g_pFresh->RegisterBitmap("state_view_o");
	m_itemListArea[15].rect = WRect(80.0f, 64.0f, m_itemListArea[15].normal->Width(), m_itemListArea[15].normal->Height());

	m_pSkinLevelBitmap[0] = g_pFresh->RegisterBitmap("i_male");
	m_pSkinLevelBitmap[1] = g_pFresh->RegisterBitmap("i_female");
	m_pSkinNameBitmap = g_pFresh->RegisterBitmap("master_sign");
	m_pSkinSlotBitmap = g_pFresh->RegisterBitmap("team_base");

	m_pItemSlotBitmap[0] = g_pFresh->RegisterBitmap("item_slot_n");
	m_pItemSlotBitmap[1] = g_pFresh->RegisterBitmap("item_slot_o");
	m_pItemSlotBitmap[2] = g_pFresh->RegisterBitmap("item_slot_d");
	m_pItemSlotEmpty = g_pFresh->RegisterBitmap("item_slot_empty");

	m_pItemDescBottom[0] = g_pFresh->RegisterBitmap("itemdesc_01");
	m_pItemDescBottom[1] = g_pFresh->RegisterBitmap("itemdesc_02");

	m_pFurnitureItemBg = g_pFresh->RegisterBitmap("rmr_arg_item_base_n");
	m_pFurnitureItemDownBg = g_pFresh->RegisterBitmap("rmr_arg_item_base_d");
	m_pFurnitureSelFrame = g_pFresh->RegisterBitmap("rmr_arg_item_base_s");
	m_pFurnitureEmptyBg = g_pFresh->RegisterBitmap("rmr_arg_item_base_empty");

	m_pItemDescFrame[0] = g_pFresh->RegisterBitmap("rmr_tip_frm_01");
	m_pItemDescFrame[1] = g_pFresh->RegisterBitmap("rmr_tip_frm_02");
	m_pItemDescFrame[2] = g_pFresh->RegisterBitmap("rmr_tip_frm_03");
	m_pItemDescFrame[3] = g_pFresh->RegisterBitmap("rmr_tip_frm_04");
	m_pItemDescFrame[4] = g_pFresh->RegisterBitmap("rmr_tip_frm_05");
	m_pItemDescFrame[5] = g_pFresh->RegisterBitmap("rmr_tip_frm_06");
	m_pItemDescFrame[6] = g_pFresh->RegisterBitmap("rmr_tip_frm_07");
	m_pItemDescFrame[7] = g_pFresh->RegisterBitmap("rmr_tip_frm_08");
	m_pItemDescFrame[8] = g_pFresh->RegisterBitmap("rmr_tip_frm_09");

	m_itemListArea[10].normal = g_pFresh->RegisterBitmap("poster_btn_n");
	m_itemListArea[10].over = g_pFresh->RegisterBitmap("poster_btn_d");
	m_itemListArea[10].rect = WRect(144.0f, 66.0f, m_itemListArea[10].normal->Width(), m_itemListArea[10].normal->Height());

	m_pMarkBegin = g_pFresh->RegisterBitmap("mark_begin");
	m_pSelectItemMark = g_pFresh->RegisterBitmap("SelectItem_Mark");
}

// 0x60e4b0 (the KR Net_EquipCharacter)
void JpRealMyRoomMain::Net_EquipCharacter(void) {
	if (m_characterId != WSingleton<CSharedDoc>::Instance()->m_myCharacterId) {
		WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
		packet.Encode1(5);
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myCharacterId);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// 0x60e550 (the KR Net_EquipItem)
void JpRealMyRoomMain::Net_EquipItem(void) {
	int i;
	for (i = 0; i < 10; i++) {
		if (m_equipItem[i] != WSingleton<CSharedDoc>::Instance()->m_equipItem[i]) {
			break;
		}
	}

	if (i == 10) {
		return;
	}

	WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
	packet.Encode1(2);
	for (i = 0; i < 10; i++) {
		// The JP EncodeBuffer of the 0x28 bytes, as 10 u32 (the KR doc
		// array is unsigned long: 8 bytes under LP64).
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_equipItem[i]);
	}
	packet.Send(SEND_TO_DEFAULT);
}

// 0x60e600 (the KR Net_EquipGear)
void JpRealMyRoomMain::Net_EquipGear(void) {
	if (m_ballId != WSingleton<CSharedDoc>::Instance()->m_myBallId || m_clubId != WSingleton<CSharedDoc>::Instance()->m_myClubId) {
		WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
		packet.Encode1(3);
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myBallId);
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myClubId);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// 0x613590 (the KR Net_EquipDresserParts on the JP character record: the
// changed character's record is sent whole (0x201 bytes), and in avatar
// chat (RFLobbyDesktop::s_mode 1) the equipped character's change is also
// announced with C->S 0xc kind 4)
void JpRealMyRoomMain::Net_EquipDresserParts(void) {
	int i;

	std::map<unsigned int, sCharacterInfo>::iterator it = m_characterMap.begin(), docIt = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin();

	for (; it != m_characterMap.end() && docIt != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it, ++docIt) {
		sCharacterInfo& charInfo = it->second;
		sCharacterInfo& docInfo = docIt->second;

		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(charInfo.typeId);
		if (pChar == NULL) {
			continue;
		}

		int numParts = pChar->m_numParts + pChar->m_numExtraParts;
		bool changed = false;
		for (i = 0; i < numParts; i++) {
			if (charInfo.partTypeIds[i] != docInfo.partTypeIds[i] || charInfo.partIds[i] != docInfo.partIds[i]) {
				changed = true;
				break;
			}
		}

		for (i = 0; i < 5; i++) {
			if (charInfo.auxParts.tids[i] != docInfo.auxParts.tids[i]) {
				changed = true;
				break;
			}
		}

		if (!changed) {
			continue;
		}

		sCharacterInfo info = docInfo;

		for (i = 0; i < numParts; i++) {
			info.partTypeIds[i] = docInfo.partTypeIds[i];
			info.partIds[i] = docInfo.partIds[i];
		}

		for (i = numParts; i < 24; i++) {
			info.partTypeIds[i] = 0;
			info.partIds[i] = 0;
		}

		info.auxParts = docInfo.auxParts;

		WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
		packet.Encode1(0);
		JpCharacter_EncodeRecordWithTailOf(packet, &info, &docInfo);
		packet.Send(SEND_TO_DEFAULT);

		if (RFLobbyDesktop::s_mode == 1 && WSingleton<CSharedDoc>::Instance()->m_myCharacterId == info.id) {
			WSendPacket avatarPacket(0xc);
			avatarPacket.Encode1(4);
			avatarPacket.Encode4(info.id);
			avatarPacket.Send(SEND_TO_DEFAULT);
		}
	}
}

// 0x613930 (the KR Net_EquipSkin; the JP marks every equipped skin whose
// state kind is neither 0 nor 2 as 2, the KR only kind 1)
void JpRealMyRoomMain::Net_EquipSkin(void) {
	int i;
	for (i = 0; i < 6; i++) {
		if (m_equipSkin[i] != WSingleton<CSharedDoc>::Instance()->m_equipSkin[i]) {
			break;
		}
	}

	if (i == 6) {
		return;
	}

	for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_mySkinList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_mySkinList.end(); ++it) {
		eEquipSkin skin;
		if (!S5::UTIL::GetEquipSkinIndex(skin, it->m_typeId)) {
			continue;
		}

		if (it->m_typeId == WSingleton<CSharedDoc>::Instance()->m_equipSkin[skin]) {
			unsigned char kind = it->m_stateBits.kind;
			if (kind != 0 && kind != 2) {
				it->m_stateBits.kind = 2;
			}
		}
	}

	WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
	packet.Encode1(4);
	for (i = 0; i < 6; i++) {
		// The JP EncodeBuffer of the 0x18 bytes (see Net_EquipItem).
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_equipSkin[i]);
	}
	packet.Send(SEND_TO_DEFAULT);
}

// 0x614d20 (the KR Net_EquipCaddie)
void JpRealMyRoomMain::Net_EquipCaddie(void) {
	if (m_caddieId == WSingleton<CSharedDoc>::Instance()->m_myCaddieId) {
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_myCaddieId != 0) {
		std::map<unsigned int, sCaddieInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_caddieMap.find(WSingleton<CSharedDoc>::Instance()->m_myCaddieId);
		if (it == WSingleton<CSharedDoc>::Instance()->m_caddieMap.end()) {
			return;
		}
	}

	WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
	packet.Encode1(1);
	packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myCaddieId);
	packet.Send(SEND_TO_DEFAULT);
}

// 0x614de0 (the KR Net_EquipMascot; the JP first tests a byte of the JP
// mascot record, +0x2b, that the KR sMascotInfo has no field for (KR
// remainHours is a u16 there and the JP end time sits one byte lower,
// +0x2d): the KR test is kept until the record gains it)
void JpRealMyRoomMain::Net_EquipMascot(void) {
	sMascotInfo* pMascot = WSingleton<CSharedDoc>::Instance()->GetMyEquippedMascotInfo();

	if (pMascot) {
		if (ItemManager()->CompareSystemTime(pMascot->endTime, WSingleton<CSharedDoc>::Instance()->GetServerTime()) <= 0 && pMascot->pcBang == 0) {
			WSingleton<CSharedDoc>::Instance()->m_myMascotId = 0;
		}
	}

	CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
	if (m_mascotId != pDoc->m_myMascotId) {
		if (pDoc->m_myMascotId != 0) {
			std::map<unsigned int, sMascotInfo>::iterator it = pDoc->m_myMascotMap.find(pDoc->m_myMascotId);
			if (it == pDoc->m_myMascotMap.end()) {
				return;
			}
		}

		WSendPacket packet(GCT_REQUEST_EQUIP_ITEM);
		packet.Encode1(8);
		packet.Encode4(WSingleton<CSharedDoc>::Instance()->m_myMascotId);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// Port body (JP 0x60e810, JpRealMyRoomMain.h): the JP sends C->S 0x20 kind
// 9 with the equipped character's id and the 16 bytes at +0x1b8 of its JP
// record, which the port's KR sCharacterInfo has no field for; nothing is
// sent until it gains them.
void JpRealMyRoomMain::Net_SendEquipKind9(void) {
}

// 0x6077c0 "middle_ui.item_desc_bg" (the KR OnItemDescBgOwnerDraw): the two
// bottom pictures under a non-empty item description.
void JpRealMyRoomMain::OnItemDescBgOwnerDraw(intptr_t param) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	int lines = m_itemDesc->GetLineNum();
	if (lines <= 0) {
		return;
	}

	float w[2];
	float h[2];
	for (int i = 0; i < 2; i++) {
		w[i] = (float)m_pItemDescBottom[i]->Width();
		h[i] = (float)m_pItemDescBottom[i]->Height();
	}

	float y = m_itemDescPos.y + 320.0f;

	pGI->DrawTexture(m_pItemDescBottom[0], WRect(m_itemDescPos.x + 6.0f, y, w[0], h[0]), 0xFFFFFFFF, 0);

	pGI->DrawTexture(m_pItemDescBottom[1], WRect(m_itemDescPos.x + 262.0f, y, w[1], h[1]), 0xFFFFFFFF, 0);
}

// 0x60d0d0 "middle_ui.sbox.item_slot" (the KR OnItemSlotOwnerDraw): the
// equipped item slot's frame (normal / hovered / selected, normal when
// empty) and the item's icon at (+2, +4).
void JpRealMyRoomMain::OnItemSlotOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem != NULL) {

		FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

		unsigned long* pItemId = (unsigned long*)pItem->pData;
		if (pItemId != NULL) {

			unsigned long typeId = *pItemId;

			int state = 0;
			if (pItem->selected) {
				state = 2;
			} else if (pItem->underCursor) {
				state = 1;
			}
			if (typeId == 0) {
				state = 0;
			}

			const Bitmap* pFrame = m_pItemSlotBitmap[state];
			if (pFrame != NULL) {
				pGI->DrawTexture(pFrame, WRect(pItem->pos.x, pItem->pos.y, (float)pFrame->Width(), (float)pFrame->Height()), 0xFFFFFFFF, 0);
			}

			if (typeId != 0) {
				IFF_STRUCT::sItem* pIff = ItemManager()->FindItem(typeId);
				if (pIff != NULL) {

					const Bitmap* pIcon = g_pFresh->GetBitmap(pIff->m_icon);
					if (pIcon != NULL) {
						pGI->DrawTexture(pIcon, WRect(pItem->pos.x + 2.0f, pItem->pos.y + 4.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
					}
				}
			}
		}
	}
}

// 0x6093d0 "item_hide_desc_bg" (the KR OnItemDescBgOwnerDraw2): lays the
// hidden item's name and description out above y 488, left of item_hide_desc
// by item_hide_desc_bg's width, and draws the nine-piece "rmr_tip_frm"
// frame around them.
void JpRealMyRoomMain::OnItemHideDescBgOwnerDraw(intptr_t param) {
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == NULL) {
		return;
	}

	float w[9];
	float h[9];
	for (int i = 0; i < 9; i++) {
		w[i] = (float)m_pItemDescFrame[i]->Width();
		h[i] = (float)m_pItemDescFrame[i]->Height();
	}

	float nameH = m_itemHideName->GetRect().h;
	int lineHeight = m_itemHideDesc->GetLineHeight();
	int lineNum = m_itemHideDesc->GetLineNum();

	float textH = (float)(lineNum + 1) * lineHeight;
	float editW = m_itemHideDesc->GetRect().w;
	float totalH = textH + nameH;
	WPoint pos(m_itemHideDescLeft - m_itemHideDescBg->GetRect().w, 488.0f - totalH);
	m_itemHideName->SetRect(WRect(pos.x + 10.0f, pos.y + 15.0f, editW, nameH));
	m_itemHideDesc->SetRect(WRect(pos.x + 10.0f, pos.y + 45.0f, editW, textH));

	float frameW = editW + 10.0f;

	pGI->DrawTexture(m_pItemDescFrame[0], WRect(pos.x, pos.y, w[0], h[0]), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[1], WRect(w[0] + pos.x, pos.y, frameW - (w[2] + w[0]), h[1]), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[2], WRect(frameW - w[2] + pos.x, pos.y, w[2], h[2]), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[3], WRect(pos.x, h[1] + pos.y, w[3], totalH - (h[6] + h[0])), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[4], WRect(w[3] + pos.x, h[1] + pos.y, frameW - (w[5] + w[3]), totalH - (h[7] + h[1])), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[5], WRect(frameW - w[5] + pos.x, h[2] + pos.y, w[5], totalH - (h[8] + h[2])), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[6], WRect(pos.x, totalH - h[6] + pos.y, w[6], h[6]), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[7], WRect(w[6] + pos.x, totalH - h[7] + pos.y, frameW - (w[8] + w[6]), h[7]), 0xFFFFFFFF, 0);
	pGI->DrawTexture(m_pItemDescFrame[8], WRect(frameW - w[8] + pos.x, totalH - h[8] + pos.y, w[8], h[8]), 0xFFFFFFFF, 0);
}

// Port body (JP 0x6166b0, 3839 bytes plus its tail chunks up to 0x61a4b6;
// the KR CRealMyRoomMain::OnItemListOwnerDraw, RealMyRoomMainUI.cpp, is its
// structure). Kept: the row's frame (item_base n / o / d, "empty" for an
// empty row) with the text colour, the default case's item (the IFF common
// item's name and icon) and its icon moved by ReviseItemIconPoint (JP
// 0x6049d0), the name printed at (+182, +7) on every sub tab but 0x13.
// Left out: the per-sub-tab cases (jump tables 0x61a4bc / 0x61a50c /
// 0x61a5cc / 0x61a600: caddies, mascots, clubs, cards, furniture ...), the
// UCC name / icon (local content 0x58), the cut-in offset (0x9da070, -12),
// the item storage's dragged row (0xe7688c), the period / count / note /
// pang texts, the card slot and level marks, the tooltips (0xe75cd0) and
// the SelectItem_Mark (+0x24c) of the row whose id is +0x250.
void JpRealMyRoomMain::OnItemListOwnerDraw(intptr_t param) {
	FrListItem* pItem = (FrListItem*)param;
	if (pItem == NULL) {
		return;
	}

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	const Bitmap* pBg;
	if (pItem->pData != NULL) {
		if (pItem->selected) {
			pBg = m_cardPackItemBmp[2];
			pGI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		} else {
			pBg = pItem->underCursor ? m_cardPackItemBmp[1] : m_cardPackItemBmp[0];
			pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		}
	} else {
		pBg = m_cardPackItemBmp[3];
	}
	pGI->SetTextStyle(0);

	if (pBg != NULL) {
		pGI->DrawTexture(pBg, WRect(pItem->pos.x, pItem->pos.y, (float)pBg->Width(), (float)pBg->Height()), 0xFFFFFFFF, 0);
	}

	if (pItem->pData == NULL) {
		return;
	}

	// "알수없는 아이템" (0xd148fc, the KR literal "unknown item").
	const char* pName = "\xBE\xCB\xBC\xF6\xBE\xF8\xB4\xC2 \xBE\xC6\xC0\xCC\xC5\xDB";
	const char* pIconName = "caddie_no";
	sItemInfo* pInfo = (sItemInfo*)pItem->pData;
	unsigned long typeId = pInfo->m_typeId;
	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);
	if (pCommon != NULL) {
		pName = pCommon->m_name;
		pIconName = pCommon->m_icon;
	}

	const Bitmap* pIcon = g_pFresh->GetBitmap(pIconName);
	WPoint iconPos(pItem->pos.x, pItem->pos.y);
	ReviseItemIconPoint(typeId, iconPos);
	if (pIcon != NULL) {
		pGI->DrawTexture(pIcon, WRect(iconPos.x, iconPos.y, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
	}

	pGI->SetTextStyle(0);
	if (m_subTabIndex != 0x13) {
		pGI->Print11(pItem->pos + WPoint(182.0f, 7.0f), 2, "%s", pName);
	}
}

// Port body (JP 0x61f8b0, the KR SelectItemList(FrListItem*); jump table
// 0x6206a8 over 0x6206e4): only the parts case (JP index 0, sub tabs 2-4,
// 0x16 ...: the KR case 1-3 / 26) in the lobby (RFLobbyDesktop::s_mode 0),
// where the JP hands the part to the "LobbyAvatar" actor (0x18a: the part
// list, &type id, &item id, &result, &true). Left out: the rental
// extension dialog (FrRentalExtensionDlg), the level / angel wing /
// restriction notices (main actor 0x23), the card / aux part case
// (0x70000000 type ids, "LobbyAvatar" 0x18b), the lounge preview's
// SetPart (0x8b28b0 on +0x584), ShowMainModel 0x61a810 and the other sub
// tabs' cases (0x17 and the unlisted ones only return true).
bool JpRealMyRoomMain::SelectItemList(FrListItem* pItem) {
	switch (m_subTabIndex) {
	case 5:
	case 0x16: {
		// JP index 1: a card goes to the record's aux slot ((tid >> 21) &
		// 0x1f, "LobbyAvatar" 0x18b); anything else is the parts case.
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;
		g_audio->PlaySfx("item_select_cloth", 0, 0, 0, 0, 0.5f, 200.0f);
		if (pInfo != NULL && (pInfo->m_typeId & 0xFC000000) == 0x70000000 && m_pCharCards != NULL) {
			uint32_t typeId = (uint32_t)pInfo->m_typeId;
			uint32_t oldTypeId = m_pCharCards[(typeId >> 21) & 0x1F];
			m_pCharCards[(typeId >> 21) & 0x1F] = typeId;
			if (RFLobbyDesktop::s_mode == 0) {
				IActor* avatar = AfxGetTask()->GetActor("LobbyAvatar");
				if (avatar != NULL) {
					avatar << MsgObject(0, 0x18b, (intptr_t)&typeId, 1, 0, 0, 0);
				}
			}
			if (oldTypeId != 0) {
				for (FrListBox::ITEM_LIST::iterator it = m_itemList->m_itemList.begin(); it != m_itemList->m_itemList.end(); ++it) {
					sItemInfo* pOther = (sItemInfo*)(*it)->pData;
					if (pOther != NULL && (*it)->selected && pOther->m_typeId == oldTypeId) {
						m_itemList->UnselectItem(*it);
					}
				}
			}
			return true;
		}
	}
	// fall through: the parts
	case 2:
	case 3:
	case 4:
	case 0x1b: {
		sItemInfo* pInfo = (sItemInfo*)pItem->pData;
		if (pInfo == NULL || ItemManager()->FindPart(pInfo->m_typeId) == NULL) {
			return false;
		}

		CPartTidList& partList = WSingleton<CSharedDoc>::Instance()->GetMyPartTidList();
		if (!partList.CanSetPart(pInfo->m_typeId)) {
			m_itemList->UnselectItem(pItem);
			return false;
		}

		g_audio->PlaySfx("item_select_cloth", 0, 0, 0, 0, 0.5f, 200.0f);

		unsigned long overlapped = partList.GetOverlapped(pInfo->m_typeId);
		if (overlapped != 0) {
			for (FrListBox::ITEM_LIST::iterator it = m_itemList->m_itemList.begin(); it != m_itemList->m_itemList.end(); ++it) {
				sItemInfo* pOther = (sItemInfo*)(*it)->pData;
				if (pOther != NULL && (*it)->selected && *it != pItem && partList.IsEquipped(pOther->m_typeId, pOther->m_serial)) {
					if ((overlapped & partList.GetOverlapped(pOther->m_typeId)) != 0) {
						m_itemList->UnselectItem(*it);
					}
				}
			}
		}

		int result = 0;
		if (RFLobbyDesktop::s_mode == 0) {
			uint32_t typeId = (uint32_t)pInfo->m_typeId;
			uint32_t serial = (uint32_t)pInfo->m_serial;
			bool flag = true;
			IActor* avatar = AfxGetTask()->GetActor("LobbyAvatar");
			if (avatar != NULL) {
				avatar << MsgObject(0, 0x18a, (intptr_t)&partList, (intptr_t)&typeId, (intptr_t)&serial, (intptr_t)&result, (intptr_t)&flag);
			}
		}

		if (result != 0) {
			sCharacterInfo& charInfo = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId];
			for (int i = 0; i < partList.m_numTotalParts; i++) {
				charInfo.partTypeIds[i] = partList.m_partTids[i];
				charInfo.partIds[i] = partList.m_partIds[i];
			}
		}

		m_pCharParts = partList.m_partTids;
		memcpy(m_charPartIds, partList.m_partIds, sizeof(m_charPartIds));
		return true;
	}
	}

	return true;
}

// Port body (JP 0x620e00, 2365 bytes; the KR OnItemListBtnDown): the row
// under the cursor goes to SelectItemList when the click selected it.
// Left out: the +0x588 job test, the item storage mode (+0xa84, local
// content 0x70), the per-sub-tab cases (jump table 0x6222f8 over
// 0x622318), the UCC draw / self-design dialogs of the parts rows, and
// UnselectItemList 0x61dfb0 for a row the click unselected (not written).
void JpRealMyRoomMain::OnItemListBtnDown(void) {
	FrListItem* pItem = m_itemList->GetItemUnderCursor();
	if (pItem == NULL) {
		return;
	}

	if (pItem->selected) {
		SelectItemList(pItem);
	}
}
