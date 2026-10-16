// Original: d:\build\custom\temp\code\client\projectg\equipdlg.cpp
#include "EquipDlg.h"
#include "ObjectFactory.h"
#include "SharedDoc.h"
#include "bitmap.h"
#include "frbutton.h"
#include "frlistbox.h"
#include "fresh.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "cardsystem.h"
#include "Localize.h"
#include "shared/classdefine.h"
#include "commonutil.h"
#include "TaskManager.h"
#include "actor.h"
#include "wview.h"
#include "globalgamedefine.h"
#include "frdesktop.h"
#include "frarea.h"
#include "fredit.h"
#include "UccManager.h"

#include <algorithm>

#include <string.h>

// File names of the item count title font (nameless data).
extern "C" char* data_ab3fe8[];

// equipdlg.cpp:18 (run-time type and factory registration of FrEquipDlg)
const WRTTI FrEquipDlg::m_RTTI("FrEquipDlg", &FrForm::m_RTTI);

IObject* FrEquipDlgMakeInstance(void) {
	return new FrEquipDlg;
}

struct __sFrEquipDlg {
	__sFrEquipDlg(void) {
		ObjectFactory().AddObjectFunctor(FrEquipDlgMakeInstance, "FrEquipDlg");
	}
};
static __sFrEquipDlg s_registerFrEquipDlg;

// equipdlg.cpp:20
const sFRESH_MSGMAP* FrEquipDlg::GetMessageMap(void) const {
	return &FrEquipDlg::_MsgMap;
}

sFRESH_MSGMAP FrEquipDlg::_MsgMap = { &FrForm::_MsgMap, &FrEquipDlg::_MsgEntries[0] };

sFRESH_ENTRY FrEquipDlg::_MsgEntries[] = {
	// equipdlg.cpp:22
	{ "additem", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrEquipDlg::OnEquipSlotInit },
	// equipdlg.cpp:23
	{ "additem", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrEquipDlg::OnEquipSlotBtnUp },
	// equipdlg.cpp:24
	{ "delitem", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrEquipDlg::OnUnequipSlotInit },
	// equipdlg.cpp:25
	{ "delitem", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrEquipDlg::OnUnequipSlotBtnUp },
	// equipdlg.cpp:26
	{ "reset", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrEquipDlg::OnResetSlotInit },
	// equipdlg.cpp:27
	{ "reset", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrEquipDlg::OnResetSlotBtnUp },
	// equipdlg.cpp:28
	{ "close", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrEquipDlg::OnCloseInit },
	// equipdlg.cpp:29
	{ "close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrEquipDlg::OnCloseBtnUp },
	// equipdlg.cpp:30
	{ "warehouse", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrEquipDlg::OnWarehouseInit },
	// equipdlg.cpp:31
	{ "warehouse", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrEquipDlg::OnWarehouseOwnerDraw },
	// equipdlg.cpp:32
	{ "warehouse", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrEquipDlg::OnWarehouseBtnDown },
	// equipdlg.cpp:33
	{ "warehouse", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrEquipDlg::OnWarehouseRBtnDown },
	// equipdlg.cpp:34
	{ "warehouse", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&FrEquipDlg::OnWarehouseDblClick },
	// equipdlg.cpp:35
	{ "itemslot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrEquipDlg::OnItemSlotInit },
	// equipdlg.cpp:36
	{ "itemslot", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrEquipDlg::OnItemSlotOwnerDraw },
	// equipdlg.cpp:37
	{ "itemslot", FRCMD_DBLCLICK, FrFuncVV, (FRESH_PMSG)&FrEquipDlg::OnItemSlotDblClick },
	// equipdlg.cpp:38
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// equipdlg.h: no line record (empty constructor of a nested helper class)
FrEquipDlg::MiniBtn::MiniBtn(void) {
}

// equipdlg.cpp:42
FrEquipDlg::FrEquipDlg(void) {
	m_pWarehouse = 0;
	m_pItemSlot = 0;

	memset(m_myItemSlot, 0, sizeof(m_myItemSlot));

	m_pWarehouseBnNormal = g_pFresh->GetBitmap("equipitem_warehouse_bn_normal");
	m_pWarehouseBnOver = g_pFresh->GetBitmap("equipitem_warehouse_bn_over");
	m_pWarehouseBnClick = g_pFresh->GetBitmap("equipitem_warehouse_bn_click");

	m_pItemSelectBnOver = g_pFresh->GetBitmap("equipitem_itemselect_bn_over");
	m_pItemSelectBnClick = g_pFresh->GetBitmap("equipitem_itemselect_bn_click");

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = data_ab3fe8;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";

	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);
}

// equipdlg.cpp:73
FrEquipDlg::~FrEquipDlg(void) {
	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }
}

// equipdlg.cpp:78
void FrEquipDlg::Init(unsigned long* pItemSlot, bool isMine) {
	if (!m_pWarehouse || !m_pItemSlot)
		return;

	memcpy(m_myItemSlot, pItemSlot, sizeof(unsigned long) * 10);

	m_itemCountMap.clear();

	if (!GalleryMode()) {
		// the warehouse: every owned item once, with its count
		for (std::list<sItemInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
			sItemInfo& info = *it;

			if (ItemManager()->IsNonVisibleItem(info.m_typeId) == true)
				continue;

			if (ItemManager()->GetCouponKind(info.m_typeId) != 0)
				continue;

			if (m_itemCountMap.find(info.m_typeId) == m_itemCountMap.end()) {
				m_itemCountMap[info.m_typeId] = info.m_count;
				m_pWarehouse->AddItem(&info);
			} else {
				m_itemCountMap[info.m_typeId] += info.m_count;
			}
		}

		if (isMine) {
			// the equipped items leave the warehouse
			for (int i = 0; i < 10; i++) {
				FrListBox::ITEM_LIST::iterator it = m_pWarehouse->m_itemList.begin();
				for (; it != m_pWarehouse->m_itemList.end(); ++it) {
					sItemInfo* pInfo = (sItemInfo*)(*it)->pData;

					if (pInfo->m_typeId == m_myItemSlot[i]) {
						if (--m_itemCountMap[pInfo->m_typeId] <= 0) {
							std::map<unsigned long, int>::iterator found = m_itemCountMap.find(pInfo->m_typeId);
							m_itemCountMap.erase(found);

							m_pWarehouse->_DelItem(it);
						}
						break;
					}
				}
			}
		}
	}

	if (IsLocalContent((localContentType_t)0x84)) {
		int numSlots = WSingleton<CSharedDoc>::Instance()->GetNumAddItemSlotByMascot() + 8;

		if (IsLocalContent((localContentType_t)0x51)) {
			int cardSlots = WSingleton<CCardManager>::Instance()->GetCardPeriodSlot();

			if (cardSlots > 0)
				numSlots += cardSlots;
		}

		for (int i = 0; i < 10; i++) {
			FrListItem* pItem = m_pItemSlot->AddItem(&m_myItemSlot[i]);

			if (i >= numSlots)
				UnequipItem(pItem);
		}
	} else {
		int extra = IsLocalContent((localContentType_t)0x51) ? 0 : 1;

		for (int i = 0; i < extra + 10; i++)
			m_pItemSlot->AddItem(&m_myItemSlot[i]);
	}
}

// equipdlg.cpp:196
void FrEquipDlg::OnEquipSlotInit(intptr_t param) {
	FrButton* pBtn = DYNAMIC_CAST(FrButton, param);
	if (pBtn) {
		if (GalleryMode()) {
			pBtn->Enable(false);
		} else {
			pBtn->SetPushDelay(0.0f);
		}
	}
}

// equipdlg.cpp:208
void FrEquipDlg::OnEquipSlotBtnUp(void) {
	FrListItem* pSelected = m_pWarehouse->GetSelected();
	if (pSelected) {
		EquipItem(pSelected);
	}
}

// equipdlg.cpp:216
void FrEquipDlg::OnUnequipSlotInit(intptr_t param) {
	FrButton* pBtn = DYNAMIC_CAST(FrButton, param);
	if (pBtn) {
		if (GalleryMode()) {
			pBtn->Enable(false);
		} else {
			pBtn->SetPushDelay(0.0f);
		}
	}
}

// equipdlg.cpp:228
void FrEquipDlg::OnUnequipSlotBtnUp(void) {
	FrListItem* pSelected = m_pItemSlot->GetSelected();
	if (pSelected) {
		// select the next equipped slot after the one emptied
		FrListBox::ITEM_LIST::iterator it = m_pItemSlot->m_itemList.begin();
		for (; it != m_pItemSlot->m_itemList.end(); ++it) {
			if (*it == pSelected)
				break;
		}

		for (++it; it != m_pItemSlot->m_itemList.end(); ++it) {
			if ((*it)->pData) {
				m_pItemSlot->SelectItem(*it, true);
				break;
			}
		}

		UnequipItem(pSelected);
	}
}

// equipdlg.cpp:279
void FrEquipDlg::OnResetSlotInit(intptr_t param) {
	FrButton* pBtn = DYNAMIC_CAST(FrButton, param);
	if (pBtn) {
		if (GalleryMode()) {
			pBtn->Enable(false);
		} else {
			pBtn->SetPushDelay(0.0f);
		}
	}
}

// equipdlg.cpp:291
void FrEquipDlg::OnResetSlotBtnUp(void) {
	FrListBox::ITEM_LIST::iterator it = m_pItemSlot->m_itemList.begin();
	for (; it != m_pItemSlot->m_itemList.end(); ++it) {
		UnequipItem(*it);
	}
}

// equipdlg.cpp:300
void FrEquipDlg::OnCloseInit(intptr_t) {
}

// equipdlg.cpp:304
void FrEquipDlg::OnCloseBtnUp(void) {
	Close((eFormRet)!GalleryMode(), true);
}

// equipdlg.cpp:309
void FrEquipDlg::OnWarehouseInit(intptr_t param) {
	m_pWarehouse = DYNAMIC_CAST(FrListBox, param);
	m_pWarehouse->UseRightButton(true);

	m_clockBtn.pBitmap = g_pFresh->GetManager()->GetBitmap("ICONS", "clock");
	m_clockBtn.rect = WRect(50.0f, 35.0f, m_clockBtn.pBitmap->Width(), m_clockBtn.pBitmap->Height());
	m_deleteBtn.pBitmap = g_pFresh->GetManager()->GetBitmap("ICONS", "delete");
	m_deleteBtn.rect = WRect(50.0f, 35.0f, m_deleteBtn.pBitmap->Width(), m_deleteBtn.pBitmap->Height());
}

// equipdlg.cpp:320
void FrEquipDlg::OnWarehouseOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	sItemInfo* pInfo = (sItemInfo*)pItem->pData;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	const Bitmap* pButton = pItem->selected ? m_pWarehouseBnClick : (pItem->underCursor ? m_pWarehouseBnOver : m_pWarehouseBnNormal);
	if (pButton) {
		WRect rect;
		rect.x = pItem->pos.x;
		rect.y = pItem->pos.y;
		rect.w = (float)pButton->Width();
		rect.h = (float)pButton->Height();
		pGDI->DrawTexture(pButton, rect, 0xFFFFFFFF, 0);
	}

	if (!pInfo->m_typeId)
		return;

	IFF_STRUCT::sItem* pData = ItemManager()->FindItem(pInfo->m_typeId);
	if (!pData)
		return;

	const Bitmap* pIcon = g_pFresh->GetManager()->GetBitmap("ITEMS", pData->m_icon);
	if (!pIcon)
		return;

	pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 18.0f, pItem->pos.y + 11.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(4);

	if (pInfo->m_stateBits.kind) {
		// a time-limited item: the clock icon, unless it expired
		if (pInfo->m_stateBits.expired)
			return;

		pGDI->DrawTexture(m_clockBtn.pBitmap, m_clockBtn.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
		return;
	}

	char* text;
	if (pInfo->m_typeId & 0x2000000) {
		if (pInfo->m_count <= 0)
			return;
		text = MakeStr("%d", pInfo->m_count);
	} else {
		text = MakeStr("%d", m_itemCountMap[pInfo->m_typeId]);
	}

	if (text && ItemManager()->IsDisplayItemNumber(pInfo->m_typeId)) {
		float x = pItem->pos.x + 70.0f;
		for (unsigned int i = 0; i < strlen(text); i++)
			x -= m_pTitleFont->GetCharWidth(g_view, text[i] - '0');
		m_pTitleFont->Print(g_view, x, pItem->pos.y + 45.0f, text, 0, 0xFFFFFFFF, 0);
	}
}

// equipdlg.cpp:405
void FrEquipDlg::OnWarehouseBtnDown(void) {
}

// equipdlg.cpp:411
void FrEquipDlg::OnWarehouseRBtnDown(void) {
	FrListItem* pItem = m_pWarehouse->GetItemUnderCursor();

	if (pItem) {
		OpenInformation(((sItemInfo*)pItem->pData)->m_typeId);
	}
}

// equipdlg.cpp:426
void FrEquipDlg::OnWarehouseDblClick(void) {
	if (GalleryMode())
		return;

	FrListItem* pItem = m_pWarehouse->GetItemUnderCursor();
	if (pItem) {
		EquipItem(pItem);

		FrListItem* pSelected = m_pWarehouse->GetSelected();
		if (pSelected)
			m_pWarehouse->UnselectItem(pSelected);
	}
}

// equipdlg.cpp:443
void FrEquipDlg::OnItemSlotInit(intptr_t param) {
	m_pItemSlot = DYNAMIC_CAST(FrListBox, param);
}

// equipdlg.cpp:448
void FrEquipDlg::OnItemSlotOwnerDraw(PG_MSGARG param) {
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;

	unsigned long* pTypeId = (unsigned long*)pItem->pData;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (IsLocalContent((localContentType_t)3)) {
		int numSlots = WSingleton<CSharedDoc>::Instance()->GetNumAddItemSlotByMascot() + 8;

		if (IsLocalContent((localContentType_t)0x51)) {
			// the card period adds slots too
			WSingleton<CCardManager>::Instance()->SetPlayerIndex(0xFF);
			WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();

			int cardSlots = WSingleton<CCardManager>::Instance()->GetCardPeriodSlot();
			if (cardSlots > 0)
				numSlots += cardSlots;

			if (WSingleton<CSharedDoc>::Instance()->GetNumAddItemSlotByMascot() == 0 && cardSlots == 0) {
				m_myItemSlot[8] = 0;
				m_myItemSlot[9] = 0;
			} else if (WSingleton<CSharedDoc>::Instance()->GetNumAddItemSlotByMascot() <= 0 || cardSlots <= 0) {
				m_myItemSlot[9] = 0;
			}
		}

		if (pItem->idx >= numSlots) {
			const Bitmap* pEmpty = g_pFresh->GetBitmap("equipitem_itemslot_empty");
			pGDI->DrawTexture(pEmpty, WRect(pItem->pos.x + 5.0f, pItem->pos.y + 5.0f, (float)pEmpty->Width(), (float)pEmpty->Height()), 0xFFFFFFFF, 0);
			return;
		}
	}

	const Bitmap* pFrame = pItem->selected ? m_pItemSelectBnClick : (pItem->underCursor ? m_pItemSelectBnOver : 0);
	if (pFrame) {
		WRect rect;
		rect.x = pItem->pos.x - 9.0f;
		rect.y = pItem->pos.y - 9.0f;
		rect.w = (float)pFrame->Width();
		rect.h = (float)pFrame->Height();
		pGDI->DrawTexture(pFrame, rect, 0xFFFFFFFF, 0);
	}

	if (pTypeId && *pTypeId) {
		IFF_STRUCT::sItem* pData = ItemManager()->FindItem(*pTypeId);
		if (pData) {
			const Bitmap* pIcon = g_pFresh->GetBitmap(pData->m_icon);
			if (pIcon) {
				pGDI->DrawTexture(pIcon, WRect(pItem->pos.x + 4.0f, pItem->pos.y + 4.0f, (float)pIcon->Width(), (float)pIcon->Height()), 0xFFFFFFFF, 0);
			}
		}
	}
}

// equipdlg.cpp:526
void FrEquipDlg::OnItemSlotDblClick(void) {
	if (GalleryMode())
		return;

	FrListItem* pItem = m_pItemSlot->GetItemUnderCursor();
	if (pItem) {
		unsigned long* pTypeId = (unsigned long*)pItem->pData;
		if (pTypeId && *pTypeId) {
			UnequipItem(pItem);
		}
	}
}

// equipdlg.cpp:542
unsigned long* FrEquipDlg::GetMyItemSlot(void) {
	return m_myItemSlot;
}

// equipdlg.cpp:547
void FrEquipDlg::EquipItem(FrListItem* pItem) {
	sItemInfo* pInfo = (sItemInfo*)pItem->pData;

	if (pInfo->m_typeId & 0x2000000) {
		// "an item that works without being equipped"
		if (WSingleton<CSharedDoc>::Instance()->m_gameMode == 1)
			AfxGetTask()->GetActor("Family") << MsgObject(0, 0x23, (intptr_t)"\xC0\xE5\xC2\xF8\xC7\xCF\xC1\xF6 \xBE\xCA\xC0\xBA \xC3\xA4 \xC8\xBF\xB7\xC2\xC0\xBB \xB9\xDF\xC8\xD6\xC7\xCF\xB4\xC2 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xD4\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
		else
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x23, (intptr_t)"\xC0\xE5\xC2\xF8\xC7\xCF\xC1\xF6 \xBE\xCA\xC0\xBA \xC3\xA4 \xC8\xBF\xB7\xC2\xC0\xBB \xB9\xDF\xC8\xD6\xC7\xCF\xB4\xC2 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xD4\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
		return;
	}

	int numSlots = 10;
	if (IsLocalContent((localContentType_t)3))
		numSlots = WSingleton<CSharedDoc>::Instance()->GetNumAddItemSlotByMascot() + 8;

	if (IsLocalContent((localContentType_t)0x51)) {
		WSingleton<CCardManager>::Instance()->SetPlayerIndex(0xFF);
		WSingleton<CCardManager>::Instance()->CalcCardPeriodAndStatus();

		if (IsLocalContent((localContentType_t)0x84)) {
			int cardSlots = WSingleton<CCardManager>::Instance()->GetCardPeriodSlot();
			if (cardSlots > 0) {
				numSlots += cardSlots;
			}
		} else {
			numSlots += WSingleton<CCardManager>::Instance()->GetCardPeriodSlot();
		}
	}

	// the first empty slot
	int i;
	for (i = 0; i < numSlots; i++) {
		if (!m_myItemSlot[i])
			break;
	}

	if (i == numSlots) {
		// "cannot equip any more"
		if (WSingleton<CSharedDoc>::Instance()->m_gameMode == 1)
			AfxGetTask()->GetActor("Family") << MsgObject(0, 0x23, (intptr_t)"\xB4\xF5 \xC0\xCC\xBB\xF3 \xC0\xE5\xC2\xF8\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
		else
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x23, (intptr_t)"\xB4\xF5 \xC0\xCC\xBB\xF3 \xC0\xE5\xC2\xF8\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
		return;
	}

	m_myItemSlot[i] = pInfo->m_typeId;

	std::map<unsigned long, int>::iterator it = m_itemCountMap.find(pInfo->m_typeId);
	if (it != m_itemCountMap.end()) {
		// the last one leaves the warehouse
		if (--m_itemCountMap[pInfo->m_typeId] <= 0) {
			m_itemCountMap.erase(it);
			m_pWarehouse->DelItem(pItem);
		}
	}
}

// equipdlg.cpp:615
void FrEquipDlg::UnequipItem(FrListItem* pItem) {
	unsigned long* pTypeId = (unsigned long*)pItem->pData;

	if (pTypeId && *pTypeId) {
		// give the item back to the warehouse
		if (m_itemCountMap.find(*pTypeId) == m_itemCountMap.end()) {
			m_itemCountMap[*pTypeId] = 1;

			CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
			for (std::list<sItemInfo>::iterator it = pDoc->m_itemInfoList.begin(); it != pDoc->m_itemInfoList.end(); ++it) {
				if ((*it).m_typeId == *pTypeId) {
					m_pWarehouse->AddItem(&*it);
					break;
				}
			}
		} else {
			m_itemCountMap[*pTypeId] = m_itemCountMap[*pTypeId] + 1;
		}

		*pTypeId = 0;
	}

	m_pItemSlot->UnselectItem(pItem);
}

// equipdlg.cpp:647
void FrEquipDlg::OpenInformation(unsigned long typeId) {
	if (FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("information")))
		return;

	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "information", 0);
	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);
	FrArea* pPortrait = DYNAMIC_CAST(FrArea, pForm->FindChildByName("portrait"));

	if (pPortrait) {
		if (pItem) {
			pPortrait->SetBgImg(pItem->m_icon);
		} else {
			pPortrait->SetBgImg("hide");
		}
	}

	if (pItem) {
		FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
		FrArea* pLevel = DYNAMIC_CAST(FrArea, pForm->FindChildByName("level"));

		if (pName && pLevel) {
			if (pItem->m_level & 0x80) {
				// "name: %s / level: ... or lower"
				pName->AddText(MakeStr("\xC0\xCC\xB8\xA7: %s\n\n\xB7\xB9\xBA\xA7:                \xC0\xCC\xC7\xCF", pItem->m_name), false, true);
			} else {
				// "name: %s / level: ... or higher"
				pName->AddText(MakeStr("\xC0\xCC\xB8\xA7: %s\n\n\xB7\xB9\xBA\xA7:                \xC0\xCC\xBB\xF3", pItem->m_name), false, true);
			}

			pLevel->SetBgImg(MakeStr("level_%03d", (pItem->m_level & 0x7F) + 1));
		}

		// the owned item's period
		std::list<sItemInfo>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(), WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(), typeId);

		if (it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
			int days = (*it).m_remainHours / 24, hours = (*it).m_remainHours % 24;

			switch ((*it).m_stateBits.kind) {
			case 1:
				// "period: %d days %d hours"
				pName->AddText(MakeStr("\xBB\xE7\xBF\xEB\xB1\xE2\xB0\xA3: %d\xC0\xCF %d\xBD\xC3\xB0\xA3", days, hours), false, true);
				break;

			case 2:
			case 3:
			case 4:
			case 5:
				if ((*it).m_stateBits.expired) {
					// "the period is over"
					pName->AddText("\\c0xffff0000\\c\xBB\xE7\xBF\xEB\xB1\xE2\xB0\xA3\\c0xff000000\\c\xC0\xCC \\c0xffff0000\\c\xB8\xB8\xB7\xE1\\c0xff000000\\c\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.\n", false, true);
				} else if (days == 0 && hours == 0) {
					// "time left: under an hour"
					pName->AddText("\xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3: 1\xBD\xC3\xB0\xA3\xB9\xCC\xB8\xB8\n", false, true);
				} else {
					// "time left: %d days %d hours"
					pName->AddText(MakeStr("\xB3\xB2\xC0\xBA\xBD\xC3\xB0\xA3: %d\xC0\xCF %d\xBD\xC3\xB0\xA3\n", days, hours), false, true);
				}
				break;
			}
		}

		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pItem->m_typeId);

		if (pDesc) {
			pForm->SetMessage(pDesc->m_text, false);
		}
	} else {
		FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));

		if (pName) {
			// "name: unknown / level: unknown"
			pName->AddText("\xC0\xCC\xB8\xA7: \xBA\xD2\xB8\xED\n\n\xB7\xB9\xBA\xA7: \xBA\xD2\xB8\xED", false, true);
		}

		// "a being of unknown identity, veiled"
		pForm->SetMessage("\xBA\xA3\xC0\xCF\xBF\xA1 \xBD\xCE\xC0\xCE \xC1\xA4\xC3\xBC\xBA\xD2\xB8\xED\xC0\xC7 \xC1\xB8\xC0\xE7", false);
	}

	pForm->Open(0, 3);
}
