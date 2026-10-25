// JP-only game code: the JP CShopMain's cart and buy part (JpShopMain,
// JpShopMain.h): checkout, the buy dialog's result, the gift and caddie
// upgrade requests, the cart / aux cart owner draws. From the JP exe (node
// scripts/disasm.ts --jp 0x<va>); the KR CShopMain bodies
// (yapang/src/client/ProjectG/ShopMain.cpp) are the structure reference.
// The item records are the port's (KR IFF_STRUCT) ones, read by the KR
// member names where the JP reads its own record offsets.
#include <string.h>

#include <list>
#include <map>

#include "JpShopMain.h"
#include "JpLobbyGlue.h"
#include "ItemManager.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "CheckoutDlg.h"
#include "UpgradeCaddieDlg.h"
#include "UccLibrary.h"
#include "UccManager.h"
#include "frarea.h"
#include "RealMyRoomData.h"
#include "parttidlist.h"
#include "InputManager.h"
#include "actor.h"
#include "fresh.h"
#include "frbutton.h"
#include "frlistbox.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "RFTextManager.h"
#include "commonutil.h"

// The Wangreal zero rect at JP 0xf145f8 (defined in JpItemSearchDlg.cpp).
extern const WRect data_f145f8;

// 0x62ba40 (JpAlchemyDlg.h, defined in JpLobbyGlue.cpp): the JP CSharedDoc's
// 64-bit IsControlServerService.
uint64_t SharedDoc_IsControlServerService64(CSharedDoc* doc, uint64_t service);

// 0xaa25b0: a JP FrCheckoutDlg member (not in CheckoutDlg.h yet) that copies
// the shop's visible caddies (JpShopMain +0x1c0) into the dialog's JP +0x3f0
// map when the source is not empty and not the dialog's own map.
void JpCheckoutDlg_SetVisibleCaddies(FrCheckoutDlg* dlg, const std::map<unsigned int, IFF_STRUCT::sCaddie>& caddies);

// 0xd45760: the 35 UCC part type ids the shop sells as blanks (JP only).
static const uint32_t s_uccBlankParts[35] = {
	0x8006055, 0x800a035, 0x8044085, 0x8048025, 0x80440b3, 0x8084044, 0x808a025,
	0x80c408b, 0x80ca023, 0x80c40b2, 0x810404f, 0x8108030, 0x8144084, 0x814a02c,
	0x81440bf, 0x8184087, 0x8188020, 0x81840c7, 0x81c404b, 0x81ca01f, 0x820404a,
	0x820a00e, 0x8204091, 0x8244064, 0x824a004, 0x824408f, 0x8284026, 0x8284027,
	0x828a012, 0x830401b, 0x830401e, 0x830a00c, 0x838400e, 0x8384011, 0x838a006
};

// 0xe350c8: the cart row each card-slot kind (typeId bits 18-25) of a UCC
// part sits in (the KR data_aa7d28 with 15 entries).
static const int s_cartRowKinds[15] = { 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0 };

// 0xa82ce0 (JP; name ours): the type id is one of the UCC blanks.
static bool IsUccBlankPart(unsigned long typeId) {
	for (int i = 0; i < 35; i++) {
		if (s_uccBlankParts[i] == typeId)
			return true;
	}
	return false;
}

// 0xb2df30 (the JP CTaskMain::MakeStatusString, free in the JP; name ours):
// the KR one with the stat names and "slot" looked up once (Jp_LookupText).
static const char* JpShop_MakeStatusString(int slot, int value, int slotValue) {
	static const char* names[5] = {
		Jp_LookupText("\xc6\xc4\xbf\xf6"),
		Jp_LookupText("\xc4\xc1\xc6\xae\xb7\xd1"),
		Jp_LookupText("\xc1\xa4\xc8\xae\xb5\xb5"),
		Jp_LookupText("\xbd\xba\xc7\xc9"),
		Jp_LookupText("\xc4\xbf\xba\xea"),
	};
	static const char* slotName = Jp_LookupText("\xbd\xbd\xb7\xd4");

	if (value == 0) {
		if (slotValue == 0)
			return "";
		return MakeStr("%s%s+%d", names[slot], slotName, slotValue);
	}
	if (slotValue == 0)
		return MakeStr("%s+%d", names[slot], value);
	return MakeStr("%s+%d/%s+%d", names[slot], value, slotName, slotValue);
}

// 0xa83fc0 (the KR DrawStatus3)
void JpShopMain::DrawStatus3(FrGraphicInterface* graphic, float x, float y, short* const status, short* const slotStatus) {
	float offsetY = 0.0f;
	for (int i = 0; i < 5; i++) {
		if (status[i] != 0 || slotStatus[i] != 0) {
			graphic->Print11(WPoint(x, offsetY + y), 0, JpShop_MakeStatusString(i, status[i], slotStatus[i]));
			offsetY += 12.0f;
		}
	}
}

// 0xa84da0 (JP): the cart list goes 90 left of the avatar rect, the aux cart
// list 10 right of it (or 120 left of main_bg3 when it would reach it).
void JpShopMain::PlaceCartLists(void) {
	if (m_cartList) {
		WPoint pt(m_rect6b0.x - 90.0f, m_cartList->GetRect().y);
		m_cartList->MoveWindow(pt);
	}

	if (m_auxCartList == NULL)
		return;

	float x = m_rect6b0.w + m_rect6b0.x + 10.0f;
	if (m_mainBg[2]->GetRect().x <= 10.0f + m_auxCartList->GetRect().w + x)
		x = m_mainBg[2]->GetRect().x - 120.0f;

	WPoint pt(x, m_auxCartList->GetRect().y);
	m_auxCartList->MoveWindow(pt);
	CheckAuxCartScroll();
}

// 0xa873d0 (the KR SendGift: the JP main actor opens the gift page, message
// 0x4D with the item, instead of the KR FrGiftDlg)
void JpShopMain::SendGift(const IFF_ITEM_COMMON& item) {
	if (SharedDoc_IsControlServerService64(CSharedDoc::Instance(), 0xC) != 0) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xBC\xB1\xB9\xB0\xC7\xD4 \xB0\xFC\xB7\xC3 \xBA\xCE\xBA\xD0 \xC1\xA1\xB0\xCB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9."), 0, 0, 0, 0));
		return;
	}

	if (GiftForbiddenMode()) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xBC\xB1\xB9\xB0\xC7\xCF\xB1\xE2\xB8\xA6 \xC7\xD2 \xBC\xF6 \xBE\xF8\xB4\xC2 \xB0\xE8\xC1\xA4\xC0\xD4\xB4\xCF\xB4\xD9."), 0, 0, 0, 0));
		return;
	}

	if (!m_cartList)
		return;

	if (m_cartList->m_itemList.size() == 0 && m_auxCartList->m_itemList.size() == 0) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xB1\xB8\xB8\xC5\xC7\xD2 \xBB\xF3\xC7\xB0\xC0\xBB \xBC\xB1\xC5\xC3\xC7\xD8 \xC1\xD6\xBD\xCA\xBD\xC3\xBF\xC0"), 0, 0, 0, 0));
		return;
	}

	unsigned char shopFlag = item.m_flags0 >> 5;
	if (shopFlag == 0 || shopFlag == 3 || shopFlag == 4) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xBC\xB1\xB9\xB0\xC7\xD2 \xBC\xF6 \xBE\xF8\xB4\xC2 \xBB\xF3\xC7\xB0\xC0\xD4\xB4\xCF\xB4\xD9."), 0, 0, 0, 0));
	}

	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x4D, (intptr_t)&item, 0, 0, 0, 0));
}

// 0xa885a0 (the KR GetItemNumInMyPocket; the JP counts a 0x18 item of sub
// kind 1 whatever its state bits)
int JpShopMain::GetItemNumInMyPocket(unsigned long typeId, bool skipFlagged) {
	int count = 0;

	if (typeId == 0 || (typeId & 0xFC000000) == 0x04000000) {
		for (std::map<unsigned int, sCharacterInfo>::const_iterator it = CSharedDoc::Instance()->m_characterMap.begin(); it != CSharedDoc::Instance()->m_characterMap.end(); ++it) {
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(it->second.typeId);
			if (item) {
				if ((typeId == 0 || it->second.typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x08000000) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myPartsList.begin(); it != CSharedDoc::Instance()->m_myPartsList.end(); ++it) {
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(it->m_typeId);
			if (item) {
				if ((typeId == 0 || it->m_typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x18000000) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(it->m_typeId);

			if (item) {
				if ((typeId == 0 || it->m_typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1))) {
					if ((typeId & 0xFC000000) != 0x18000000) {
						if (it->m_stateBits.kind) {
							count = 100000;
						} else {
							count++;
						}
					} else if ((typeId & 0x1F00000) == 0x100000 || !it->m_stateBits.kind) {
						count++;
					} else {
						count = 100000;
					}
				}
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x1C000000) {
		for (std::map<unsigned int, sCaddieInfo>::iterator it = CSharedDoc::Instance()->m_caddieMap.begin(); it != CSharedDoc::Instance()->m_caddieMap.end(); ++it) {
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(it->second.typeId);
			if (item) {
				if ((typeId == 0 || it->second.typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	if (typeId == 0 || (typeId & 0xFC000000) == 0x14000000) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); ++it) {
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(it->m_typeId);
			if (item) {
				if ((typeId == 0 || it->m_typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	unsigned long type = typeId >> 26;
	if (type == 9 && (typeId & 0x3E00000) == 0x200000) {
		IFF_STRUCT::sSetItem* setItem = ItemManager()->FindSetItem(typeId);

		for (unsigned char i = 0; i < setItem->m_itemCount; i++) {
			if (typeId == 0 || (setItem->m_itemTypeIds[i] & 0xFC000000) != 0x04000000) {
				for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_myPartsList.begin(); it != CSharedDoc::Instance()->m_myPartsList.end(); ++it) {
					IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(it->m_typeId);
					if (item) {
						if ((typeId == 0 || it->m_typeId == setItem->m_itemTypeIds[i]) && (!skipFlagged || !(item->m_flags0 & 1)))
							count++;
					}
				}
			}
		}
	}

	if (typeId == 0 || type == 14) {
		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_mySkinList.begin(); it != CSharedDoc::Instance()->m_mySkinList.end(); ++it) {
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(it->m_typeId);
			if (item) {
				if ((typeId == 0 || it->m_typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	if (typeId == 0 || type == 15) {
		count = 1;
	}

	if (type == 4) {
		for (std::map<unsigned int, sItemInfo>::iterator it = CSharedDoc::Instance()->m_myItemMap.begin(); it != CSharedDoc::Instance()->m_myItemMap.end(); ++it) {
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(it->second.m_typeId);
			if (item) {
				if ((typeId == 0 || it->second.m_typeId == typeId) && (!skipFlagged || !(item->m_flags0 & 1)))
					count++;
			}
		}
	}

	return count;
}

// 0xa8a460 (the KR UpgradeCaddie)
void JpShopMain::UpgradeCaddie(unsigned long caddieTypeId) {
	unsigned long upgradeTypeId = 0;
	std::map<unsigned long, unsigned long>::iterator up = m_upgradeCaddies.find(caddieTypeId);
	if (up != m_upgradeCaddies.end()) {
		upgradeTypeId = up->second;
	} else {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xC7\xD8\xB4\xE7 \xC4\xB3\xB5\xF0\xB4\xC2 \xBE\xF7\xB1\xD7\xB7\xB9\xC0\xCC\xB5\xE5\xB0\xA1 \xBA\xD2\xB0\xA1\xB4\xC9 \xC7\xD5\xB4\xCF\xB4\xD9"), 0, 0, 0, 0));
	}

	short beforeLevels[5];
	short afterLevels[5];
	int beforeFee = 0;
	int afterFee = 0;
	memset(beforeLevels, 0, sizeof(beforeLevels));
	memset(afterLevels, 0, sizeof(afterLevels));

	typedef std::map<unsigned int, IFF_STRUCT::sCaddie> CaddieMap;
	CaddieMap::iterator before = ItemManager()->m_caddieMap.find(caddieTypeId);
	CaddieMap::iterator after = ItemManager()->m_caddieMap.find(upgradeTypeId);

	if (before != ItemManager()->m_caddieMap.end()) {
		memcpy(beforeLevels, before->second.m_levels, sizeof(beforeLevels));
		beforeFee = before->second.fee;
	}

	if (after != ItemManager()->m_caddieMap.end()) {
		memcpy(afterLevels, after->second.m_levels, sizeof(afterLevels));
		afterFee = after->second.fee;
	}

	int stats[5];
	stats[0] = afterLevels[0] - beforeLevels[0];
	stats[1] = afterLevels[1] - beforeLevels[1];
	stats[2] = afterLevels[2] - beforeLevels[2];
	stats[3] = afterLevels[3] - beforeLevels[3];
	stats[4] = afterLevels[4] - beforeLevels[4];

	m_upgradeCaddieDlg = CreateForm<FrUpgradeCaddieDlg>(g_pFresh->GetManager(), this, "upgradecaddie", NULL);
	if (m_upgradeCaddieDlg) {
		m_upgradeCaddieDlg->SetUpgradeInfo(caddieTypeId, stats, beforeFee, afterFee);
		m_upgradeCaddieDlg->Open(0, 3);
	}
}

// 0xa8aa50 (the KR OnPartCartListOwnerDraw; the zero-rect test 0xa82590 is
// WRect equality, written with the defined operator!=: the JP rows follow the cart
// list's position, which PlaceCartLists sets once the "LobbyAvatar" actor
// gave the avatar rect (message 0x194); no skin row, no 0x38 icon. The JP
// prints a UCC blank's name through GetUccItemName from its shop record
// (+0x238 serial, +0x23c UCC key), which the port's KR record lacks: the
// port prints the part name.)
void JpShopMain::OnPartCartListOwnerDraw(intptr_t param) {
	FrListItem* listItem = (FrListItem*)param;
	if (listItem == NULL)
		return;

	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic == NULL)
		return;

	IFF_STRUCT::sPart* part = (IFF_STRUCT::sPart*)listItem->pData;
	if (part == NULL)
		return;

	bool hasStatus = false;
	short status[5];
	short slotStatus[5];
	memset(status, 0, sizeof(status));
	memset(slotStatus, 0, sizeof(slotStatus));
	int row;
	switch (part->m_typeId >> 26) {
	case 2:
		for (int i = 0; i < 5; i++) {
			if (part->m_baseStat[i] != 0 || part->m_slotStat[i] != 0) {
				memcpy(status, part->m_baseStat, sizeof(status));
				memcpy(slotStatus, part->m_slotStat, sizeof(slotStatus));
				hasStatus = true;
				break;
			}
		}

		switch (part->slot) {
		case 0:
			row = 1;
			break;
		case 1:
			row = 2;
			break;
		case 4:
			row = 3;
			break;
		case 7:
		case 8:
		case 9: {
			int kind = (part->m_typeId >> 18) & 0xFF;
			if (kind < 15)
				row = (((part->m_typeId >> 13) & 0x1F) != (unsigned long)s_cartRowKinds[kind]) + 1;
			else
				row = 0;
			break;
		}
		default:
			return;
		}
		break;
	case 9: {
		int kind = (part->m_typeId >> 21) & 0x1F;
		if (kind == 6 || kind == 5 || kind == 4 || kind == 8)
			return;
		row = 1;
		break;
	}
	case 15:
		row = 0;
		break;
	default:
		return;
	}

	float rowW = (float)m_itFa02->Width();
	float rowH = (float)m_itFa02->Height();
	float iconW = (float)m_sicons[0]->Width();
	float iconH = (float)m_sicons[0]->Height();
	float cancelW = (float)m_cancelBtnNormal->Width();
	float cancelH = (float)m_cancelBtnNormal->Height();
	float x = 8.0f;
	float y = 129.0f;

	if (!(m_rect6b0 != data_f145f8)) {
		MsgObject msg(0, 0x194, (intptr_t)&m_rect6b0, 0, 0, 0, 0);
		IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
		if (actor) {
			actor << msg;
		}
		PlaceCartLists();
	}

	if (m_cartList) {
		x = m_cartList->GetRect().x;
		y = m_cartList->GetRect().y;
	}

	float rowY = (float)row * rowH + y;
	graphic->DrawTexture(m_itFa02, WRect(x, rowY, rowW, rowH), 0xFFFFFFFF, 0);
	graphic->DrawTexture(m_sicons[row], WRect(x + 4.0f, rowY + 5.0f, iconW, iconH), 0xFFFFFFFF, 0);

	graphic->SetTextColor(0xFFFFFFFF, 0xFF36779A);
	graphic->SetTextStyle(4);
	float textX = x + 8.0f;
	graphic->Print11(WPoint(textX, rowY + 27.0f), 0, part->m_name);
	graphic->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
	graphic->SetTextStyle(0);
	if (hasStatus)
		DrawStatus3(graphic, textX, rowY + 41.0f, status, slotStatus);

	WRect cancelRect(x + 104.0f, rowY + 12.0f, cancelW, cancelH);
	if (cancelRect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
		graphic->DrawTexture(m_cancelBtnOver, cancelRect, 0xFFFFFFFF, 0);
		if (g_input->GetButton(LEFT_BUTTON) == 2) {
			unsigned long typeId = part->m_typeId;
			m_rbuttonPartTypeIds.push_back(typeId);
		}
	} else {
		graphic->DrawTexture(m_cancelBtnNormal, cancelRect, 0xFFFFFFFF, 0);
	}
}

// 0xa8b040 (the KR OnAuxPartCartListOwnerDraw on the aux cart list's
// position; on shop tab 5 it sits 120 left of main_bg3; no status for a
// 0x38 item)
void JpShopMain::OnAuxPartCartListOwnerDraw(intptr_t param) {
	FrListItem* listItem = (FrListItem*)param;
	if (listItem == NULL)
		return;

	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic == NULL)
		return;

	IFF_STRUCT::sPart* part = (IFF_STRUCT::sPart*)listItem->pData;
	if (part == NULL)
		return;

	bool hasStatus = false;
	short status[5];
	short slotStatus[5];
	memset(status, 0, sizeof(status));
	memset(slotStatus, 0, sizeof(slotStatus));
	if ((part->m_typeId & 0xFC000000) != 0x38000000) {
		for (int i = 0; i < 5; i++) {
			if (part->m_baseStat[i] != 0 || part->m_slotStat[i] != 0) {
				memcpy(status, part->m_baseStat, sizeof(status));
				memcpy(slotStatus, part->m_slotStat, sizeof(slotStatus));
				hasStatus = true;
				break;
			}
		}
	}

	float x = 250.0f;
	float y = 129.0f;
	int index = listItem->no;
	float rowW = (float)m_itFa02->Width();
	float rowH = (float)m_itFa02->Height();
	float iconW = (float)m_sicons[0]->Width();
	float iconH = (float)m_sicons[0]->Height();
	float cancelW = (float)m_cancelBtnNormal->Width();
	float cancelH = (float)m_cancelBtnNormal->Height();

	if (!(m_rect6b0 != data_f145f8)) {
		MsgObject msg(0, 0x194, (intptr_t)&m_rect6b0, 0, 0, 0, 0);
		IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
		if (actor) {
			actor << msg;
		}
		PlaceCartLists();
	}

	if (m_auxCartList) {
		if (m_shopTab == 5 && m_mainBg[2])
			x = m_mainBg[2]->GetRect().x - 120.0f;
		else
			x = m_auxCartList->GetRect().x;
		y = m_auxCartList->GetRect().y;
	}

	float rowY = (float)index * rowH + y;
	graphic->DrawTexture(m_itFa02, WRect(x, rowY, rowW, rowH), 0xFFFFFFFF, 0);
	graphic->DrawTexture(m_sicons[4], WRect(x + 4.0f, rowY + 5.0f, iconW, iconH), 0xFFFFFFFF, 0);

	graphic->SetTextColor(0xFFFFFFFF, 0xFF36779A);
	graphic->SetTextStyle(4);
	float textX = x + 8.0f;
	graphic->Print11(WPoint(textX, rowY + 27.0f), 0, part->m_name);
	graphic->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
	graphic->SetTextStyle(0);
	if (hasStatus)
		DrawStatus3(graphic, textX, rowY + 41.0f, status, slotStatus);

	WRect cancelRect(x + 104.0f, rowY + 12.0f, cancelW, cancelH);
	if (cancelRect.IsInRect(g_pFresh->GetManager()->GetMousePos())) {
		graphic->DrawTexture(m_cancelBtnOver, cancelRect, 0xFFFFFFFF, 0);
		if (g_input->GetButton(LEFT_BUTTON) == 2) {
			unsigned long typeId = part->m_typeId;
			m_rbuttonPartTypeIds.push_back(typeId);
		}
	} else {
		graphic->DrawTexture(m_cancelBtnNormal, cancelRect, 0xFFFFFFFF, 0);
	}
}

// 0xa8b9e0 (the KR TakeOff_PartNCartList: the JP dresses the "LobbyAvatar"
// actor, message 0x18A with the tid list and the part's type id, instead of
// the KR pet frame's SetPart)
void JpShopMain::TakeOff_PartNCartList(CPartTidList& tidList, unsigned long overlapMask) {
	for (FrListBox::ITEM_LIST::iterator i = m_cartList->m_itemList.begin(); i != m_cartList->m_itemList.end();) {
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*i)->pData;

		if ((item->m_typeId >> 26) == 2) {
			unsigned long overlapped = tidList.GetOverlapped(item->m_typeId);
			if (overlapMask & overlapped) {
				UnselectItemFromItemList(item->m_typeId);
				i = DeleteItemFromCartList(item->m_typeId);

				if (tidList.IsEquipped(item->m_typeId, 0)) {
					MsgObject msg(0, 0x18A, (intptr_t)&tidList, (intptr_t)&item->m_typeId, 0, 0, 0);
					IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
					if (actor) {
						actor << msg;
					}
				}

				RestoreShoppersOldItem(item->m_typeId, tidList);
				UnselectItemList(item->m_typeId);
				continue;
			}

			++i;
			continue;
		} else if ((item->m_typeId >> 26) == 9) {
			IFF_STRUCT::sSetItem* setItem = (IFF_STRUCT::sSetItem*)item;

			for (int j = 0; j < setItem->m_itemCount; j++) {
				unsigned long typeId = setItem->m_itemTypeIds[j];

				if (tidList.IsEquipped(typeId, 0)) {
					MsgObject msg(0, 0x18A, (intptr_t)&tidList, (intptr_t)&typeId, 0, 0, 0);
					IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
					if (actor) {
						actor << msg;
					}
				}

				RestoreShoppersOldItem(typeId, tidList);
			}

			UnselectItemFromItemList(item->m_typeId);
			i = DeleteItemFromCartList(item->m_typeId);
			continue;
		}

		++i;
	}
}

// 0xa8bbb0 (the KR TakeOff_AuxPartNCartList, the avatar dressed as in
// TakeOff_PartNCartList)
void JpShopMain::TakeOff_AuxPartNCartList(CPartTidList& tidList, unsigned long overlapMask) {
	for (FrListBox::ITEM_LIST::iterator i = m_auxCartList->m_itemList.begin(); i != m_auxCartList->m_itemList.end();) {
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*i)->pData;

		if ((item->m_typeId & 0xFC000000) == 0x08000000) {
			unsigned long overlapped = tidList.GetOverlapped(item->m_typeId);
			if (overlapMask & overlapped) {
				UnselectItemFromItemList(item->m_typeId);
				i = DeleteItemFromAuxCartList(item->m_typeId);

				if (tidList.IsEquipped(item->m_typeId, 0)) {
					MsgObject msg(0, 0x18A, (intptr_t)&tidList, (intptr_t)&item->m_typeId, 0, 0, 0);
					IActor* actor = AfxGetTask()->GetActor("LobbyAvatar");
					if (actor) {
						actor << msg;
					}
				}

				RestoreShoppersOldItem(item->m_typeId, tidList);
				continue;
			}

			++i;
			continue;
		}

		++i;
	}
}

// 0xa8c490 (the KR OnBuyDlgResult)
bool JpShopMain::OnBuyDlgResult(int result, FrForm* form) {
	if (result == 1) {
		for (FrListBox::ITEM_LIST::iterator i = m_cartList->m_itemList.begin(); i != m_cartList->m_itemList.end(); ++i) {
			IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*i)->pData;

			if ((item->m_typeId & 0xFC000000) == 0x08000000 && IsUccClothes(item->m_typeId)) {
				RestoreShoppersOldItem(item->m_typeId, WSingleton<CSharedDoc>::Instance()->GetMyPartTidList());
			}
		}

		ResetItemList(false);
		ClearCartList(false, NULL);
		m_shoppersOldItems.clear();
	}

	m_buyDlg = NULL;
	return true;
}

// 0xa8c570 (the KR Checkout: the messages go to the main actor through
// Jp_LookupText, a UCC blank never blocks the cart, the dialog gets the
// visible caddies 0xaa25b0, and the rental flag is IsRentalSideTab)
void JpShopMain::Checkout(void) {
	if (SharedDoc_IsControlServerService64(CSharedDoc::Instance(), 4) != 0) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xBB\xF3\xC1\xA1 \xB0\xFC\xB7\xC3 \xBA\xCE\xBA\xD0 \xC1\xA1\xB0\xCB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9."), 0, 0, 0, 0));
		return;
	}

	int count = m_cartList->m_itemList.size();
	count += m_auxCartList->m_itemList.size();

	if (count == 0) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xB1\xB8\xB8\xC5\xC7\xD2 \xBB\xF3\xC7\xB0\xC0\xBB \xBC\xB1\xC5\xC3\xC7\xD8 \xC1\xD6\xBD\xCA\xBD\xC3\xBF\xC0"), 0, 0, 0, 0));
		return;
	}

	bool bNotForSale = false;
	FrListBox::ITEM_LIST::iterator it;
	for (it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*it)->pData;

		unsigned char shopFlag = item->m_flags0 >> 5;
		if ((shopFlag == 0 || shopFlag == 2 || shopFlag == 4) && !IsUccBlankPart(item->m_typeId)) {
			bNotForSale = true;
			break;
		}
	}

	if (!bNotForSale) {
		for (it = m_auxCartList->m_itemList.begin(); it != m_auxCartList->m_itemList.end(); ++it) {
			IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*it)->pData;

			unsigned char shopFlag = item->m_flags0 >> 5;
			if (shopFlag == 0 || shopFlag == 2 || shopFlag == 4) {
				bNotForSale = true;
				break;
			}
		}
	}

	if (bNotForSale) {
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xB1\xB8\xB8\xC5\xC7\xD2 \xBC\xF6 \xBE\xF8\xB4\xC2 \xBB\xF3\xC7\xB0\xC0\xCC \xC6\xF7\xC7\xD4\xB5\xC7\xBE\xEE \xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9."), 0, 0, 0, 0));
		return;
	}

	for (it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
		IFF_ITEM_COMMON* item = (IFF_ITEM_COMMON*)(*it)->pData;

		if ((item->m_typeId & 0xFC000000) == 0x48000000) {
			std::list<sFurniture_List>& furnitures = WSingleton<CSharedDoc>::Instance()->m_furnitureList;
			for (std::list<sFurniture_List>::iterator fit = furnitures.begin(); fit != WSingleton<CSharedDoc>::Instance()->m_furnitureList.end(); ++fit) {
				IFF_STRUCT::sFurniture* furniture = ItemManager()->FindFurniture(fit->m_typeId);

				if (!furniture)
					break;

				if (furniture->m_placeOnModel == 0 && fit->m_typeId == item->m_typeId) {
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)Jp_LookupText("\xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \xB9\xB0\xB0\xC7\xC0\xBB \xB1\xB8\xC0\xD4\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9."), 0, 0, 0, 0));
					return;
				}
			}
		}
	}

	FrCheckoutDlg* dlg = CreateForm<FrCheckoutDlg>(g_pFresh->GetManager(), this, "checkout", 0);
	m_buyDlg = dlg;

	for (it = m_cartList->m_itemList.begin(); it != m_cartList->m_itemList.end(); ++it) {
		FrCheckoutDlg::CartItem cartItem;
		cartItem.m_pItem = (IFF_ITEM_COMMON*)(*it)->pData;

		if ((cartItem.m_pItem->m_typeId & 0xFC000000) == 0x24000000) {
			IFF_STRUCT::sSetItem* setItem = ItemManager()->FindSetItem(cartItem.m_pItem->m_typeId);
			if (setItem) {
				cartItem.m_option = 0;
				for (int i = 0; i < setItem->m_itemCount; i++) {
					unsigned long typeId = setItem->m_itemTypeIds[i];
					if (typeId && (typeId >> 26) != 1 && (typeId >> 26) != 6)
						cartItem.m_option += GetItemNumInMyPocket(typeId, false);
				}
			}
		} else {
			cartItem.m_option = GetItemNumInMyPocket(cartItem.m_pItem->m_typeId, false);
		}

		cartItem.m_days = 5;
		dlg->Cart().push_back(cartItem);
	}

	for (it = m_auxCartList->m_itemList.begin(); it != m_auxCartList->m_itemList.end(); ++it) {
		FrCheckoutDlg::CartItem cartItem;
		cartItem.m_pItem = (IFF_ITEM_COMMON*)(*it)->pData;
		cartItem.m_option = GetItemNumInMyPocket(cartItem.m_pItem->m_typeId, false);

		cartItem.m_days = 5;
		dlg->Cart().push_back(cartItem);
	}

	JpCheckoutDlg_SetVisibleCaddies(dlg, m_visibleCaddies);
	bool bRental = IsRentalSideTab();
	dlg->Checkout(WSingleton<CSharedDoc>::Instance()->m_myPang, WSingleton<CSharedDoc>::Instance()->m_myCash, bRental);
	m_buyDlg->Open((FRESH_RESULT_CALLBACK)&JpShopMain::OnBuyDlgResult, 0x81);
}
