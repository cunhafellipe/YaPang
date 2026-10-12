// Original: d:\build\custom\temp\code\client\projectg\offlinetradedlg.cpp
// This unit emits the out-of-line copies of CSharedDoc::GetTradeMode and
// GetMaketList (defined in the class body in shareddoc.h).
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#define OFFLINETRADEDLG_SHAREDDOC_INLINE
#include "OfflineTradeDlg.h"
#include "ObjectFactory.h"
#include "shared/Localize.h"
#include "ToolTip.h"
#include "fredit.h"
#include "frcombobox.h"
#include "frlistbox.h"
#include "frbutton.h"
#include "frarea.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "actor.h"
#include "commonutil.h"
#include "binstr.h"
#include "StandingInLine.h"
#include "Packet.h"
#include "fresh.h"
#include "frviewer.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "bitmap.h"
#include "UccManager.h"

#include <stdlib.h>
#include <string.h>

void SetTradeResultCode(int code, int msgId, sTradeItem* pItem);

// offlinetradedlg.h: no line record (empty constructor of a nested helper class)
FrOfflineTradeFinishEditDlg::MiniBtn::MiniBtn(void) {
}

// offlinetradedlg.h: no line record (empty constructor of a nested helper class)
COfflineTradeDlg::MiniBtn::MiniBtn(void) {
}

// offlinetradedlg.h: no line record (empty constructor of a nested helper class)
COfflineShopEditDlg::MiniBtn::MiniBtn(void) {
}

// offlinetradedlg.h: no line record (empty constructor of a nested helper class)
COfflineShopDlg::MiniBtn::MiniBtn(void) {
}

// offlinetradedlg.h: no line record (empty constructor of a nested helper class)
COfflineShopVisitorDlg::MiniBtn::MiniBtn(void) {
}

// offlinetradedlg.cpp:34 (run-time type and factory registration of FrOfflineTradeFinishEditDlg)
const WRTTI FrOfflineTradeFinishEditDlg::m_RTTI("FrOfflineTradeFinishEditDlg", &FrForm::m_RTTI);

IObject* FrOfflineTradeFinishEditDlgMakeInstance(void) {
	return new FrOfflineTradeFinishEditDlg;
}

struct __sFrOfflineTradeFinishEditDlg {
	__sFrOfflineTradeFinishEditDlg(void) {
		ObjectFactory().AddObjectFunctor(FrOfflineTradeFinishEditDlgMakeInstance, "FrOfflineTradeFinishEditDlg");
	}
};
static __sFrOfflineTradeFinishEditDlg s_registerFrOfflineTradeFinishEditDlg;

// offlinetradedlg.cpp:36
const sFRESH_MSGMAP* FrOfflineTradeFinishEditDlg::GetMessageMap(void) const {
	return &FrOfflineTradeFinishEditDlg::_MsgMap;
}

sFRESH_MSGMAP FrOfflineTradeFinishEditDlg::_MsgMap = { &FrForm::_MsgMap, &FrOfflineTradeFinishEditDlg::_MsgEntries[0] };

sFRESH_ENTRY FrOfflineTradeFinishEditDlg::_MsgEntries[] = {
	// offlinetradedlg.cpp:38
	{ "tradefinisheditdlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOfflineTradeFinishEditDlg::OnTradeFinishEditDlgInit },
	// offlinetradedlg.cpp:39
	{ "trade_item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOfflineTradeFinishEditDlg::OnTradeItemListInit },
	// offlinetradedlg.cpp:40
	{ "trade_item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOfflineTradeFinishEditDlg::OnTradeItemListOwnerDraw },
	// offlinetradedlg.cpp:41
	{ "trade_item_list", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOfflineTradeFinishEditDlg::OnTradeItemListLBtnUp },
	// offlinetradedlg.cpp:42
	{ "userid", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOfflineTradeFinishEditDlg::OnUserIdInit },
	// offlinetradedlg.cpp:43
	{ "trade_title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOfflineTradeFinishEditDlg::OnTitleInit },
	// offlinetradedlg.cpp:44
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOfflineTradeFinishEditDlg::OnOkBnInit },
	// offlinetradedlg.cpp:45
	{ "ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOfflineTradeFinishEditDlg::OnOkBnUp },
	// offlinetradedlg.cpp:47
	{ "tooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOfflineTradeFinishEditDlg::OnToolTipInit },
	// offlinetradedlg.cpp:48
	{ "tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOfflineTradeFinishEditDlg::OnToolTipOnwerDraw },
	// offlinetradedlg.cpp:51
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// offlinetradedlg.cpp:55
FrOfflineTradeFinishEditDlg::FrOfflineTradeFinishEditDlg(void) {
	m_pSelectedSlotItem = 0;

	m_extendBtnNormal.pBitmap = g_pFresh->GetBitmap("rmr_extend_btn_n");
	m_extendBtnNormal.rect = WRect(96.0f, 66.0f, m_extendBtnNormal.pBitmap->Width(), m_extendBtnNormal.pBitmap->Height());
	m_extendBtnNormal.state = -1;
	m_extendBtnOver.pBitmap = g_pFresh->GetBitmap("rmr_extend_btn_o");
	m_extendBtnOver.rect = WRect(96.0f, 66.0f, m_extendBtnOver.pBitmap->Width(), m_extendBtnOver.pBitmap->Height());
	m_extendBtnOver.state = -1;

	m_returnBtnNormal.pBitmap = g_pFresh->GetBitmap("rmr_return_n");
	m_returnBtnNormal.rect = WRect(144.0f, 66.0f, m_returnBtnNormal.pBitmap->Width(), m_returnBtnNormal.pBitmap->Height());
	m_returnBtnNormal.state = -1;
	m_returnBtnOver.pBitmap = g_pFresh->GetBitmap("rmr_return_o");
	m_returnBtnOver.rect = WRect(144.0f, 66.0f, m_returnBtnOver.pBitmap->Width(), m_returnBtnOver.pBitmap->Height());
	m_returnBtnOver.state = -1;

	m_deleteBtnNormal.pBitmap = g_pFresh->GetBitmap("delete_btn_n");
	m_deleteBtnNormal.rect = WRect(144.0f, 66.0f, m_deleteBtnNormal.pBitmap->Width(), m_deleteBtnNormal.pBitmap->Height());
	m_deleteBtnNormal.state = -1;
	m_deleteBtnOver.pBitmap = g_pFresh->GetBitmap("delete_btn_o");
	m_deleteBtnOver.rect = WRect(144.0f, 66.0f, m_deleteBtnOver.pBitmap->Width(), m_deleteBtnOver.pBitmap->Height());
	m_deleteBtnOver.state = -1;

	m_buyBtnNormal.pBitmap = g_pFresh->GetBitmap("buy_btn_n");
	m_buyBtnNormal.rect = WRect(144.0f, 66.0f, m_buyBtnNormal.pBitmap->Width(), m_buyBtnNormal.pBitmap->Height());
	m_buyBtnNormal.state = -1;
	m_buyBtnOver.pBitmap = g_pFresh->GetBitmap("buy_btn_o");
	m_buyBtnOver.rect = WRect(144.0f, 66.0f, m_buyBtnOver.pBitmap->Width(), m_buyBtnOver.pBitmap->Height());
	m_buyBtnOver.state = -1;

	m_delete.pBitmap = g_pFresh->GetBitmap("delete");
	m_delete.rect = WRect(144.0f, 35.0f, m_delete.pBitmap->Width(), m_delete.pBitmap->Height());
	m_tradeState1.pBitmap = g_pFresh->GetBitmap("rmr_trade_state1");
	m_tradeState1.rect = WRect(70.0f, 35.0f, m_tradeState1.pBitmap->Width(), m_tradeState1.pBitmap->Height());
	m_tradeState2.pBitmap = g_pFresh->GetBitmap("rmr_trade_state2");
	m_tradeState2.rect = WRect(70.0f, 35.0f, m_tradeState2.pBitmap->Width(), m_tradeState2.pBitmap->Height());
	m_tradeState3.pBitmap = g_pFresh->GetBitmap("rmr_trade_state3");
	m_tradeState3.rect = WRect(70.0f, 35.0f, m_tradeState3.pBitmap->Width(), m_tradeState3.pBitmap->Height());

	m_uploadBtnNormal.pBitmap = g_pFresh->GetBitmap("rmr_item_upload_n");
	m_uploadBtnNormal.rect = WRect(144.0f, 66.0f, m_uploadBtnNormal.pBitmap->Width(), m_uploadBtnNormal.pBitmap->Height());
	m_uploadBtnNormal.state = -1;
	m_uploadBtnOver.pBitmap = g_pFresh->GetBitmap("rmr_item_upload_o");
	m_uploadBtnOver.rect = WRect(144.0f, 66.0f, m_uploadBtnOver.pBitmap->Width(), m_uploadBtnOver.pBitmap->Height());
	m_uploadBtnOver.state = -1;
}

// offlinetradedlg.cpp:112
FrOfflineTradeFinishEditDlg::~FrOfflineTradeFinishEditDlg(void) {
}

// offlinetradedlg.cpp:118
void FrOfflineTradeFinishEditDlg::OnTradeItemListOwnerDraw(int param) {
	FrTradeFinishEditDlg::OnTradeItemListOwnerDraw(param);

	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (!pGDI) {
		return;
	}

	const WPoint& mousePos = pManager->GetMousePos();

	FrListItem* pItem = (FrListItem*)param;
	if (!pItem || !pItem->pData) {
		return;
	}

	const Bitmap* pSlotBitmap = g_pFresh->GetBitmap("rmr_item_slot");

	sStallSlot* pSlot = (sStallSlot*)pItem->pData;

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	if (pSlot->reserved == 0) {
		if (pSlotBitmap) {
			pGDI->DrawTexture(pSlotBitmap, WRect(pItem->pos.x, pItem->pos.y, (float)pSlotBitmap->Width(), (float)pSlotBitmap->Height()), 0xFFFFFFFF, 0);
		}
		// "buy a shop item slot"
		pGDI->Print(pItem->pos + WPoint(182.0f, 7.0f), 2, "\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4 \xb1\xb8\xb8\xc5");

		if (pItem->underCursor && (m_buyBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
			pGDI->DrawTexture(m_buyBtnOver.pBitmap, m_buyBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x16);
		} else {
			WRect rect;
			rect = m_buyBtnNormal.rect + pItem->pos;
			pGDI->DrawTexture(m_buyBtnNormal.pBitmap, rect, 0xFFFFFFFF, 0);
		}
	} else {
		__int64 hours = 0;

		if (pSlot->sold == 0) {
			FILETIME slotTime;
			FILETIME serverTime;
			SystemTimeToFileTime(&pSlot->expireTime, &slotTime);
			SystemTimeToFileTime(&CSharedDoc::Instance()->GetServerTime(), &serverTime);

			__int64 diff = ((__int64)slotTime.dwHighDateTime - serverTime.dwHighDateTime) * 0x100000000 - serverTime.dwLowDateTime + slotTime.dwLowDateTime;
			hours = diff / 60 / 60 / 10000000;

			pGDI->DrawTexture(m_tradeState3.pBitmap, m_tradeState3.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);

			if (hours >= 0 && diff >= 0) {
				pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
				if (hours / 24 >= 1) {
					// "%d days"
					pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "%d\xc0\xcf", hours / 24 + 1);
				} else {
					// "%d hours"
					pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "%d\xbd\xc3", hours);
				}
			} else {
				pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
				// "expired"
				pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "\xb1\xe2\xb0\xa3\xb8\xb8\xb7\xe1");
			}
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

			if (pItem->underCursor && (m_extendBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
				pGDI->DrawTexture(m_extendBtnOver.pBitmap, m_extendBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x43);
			} else {
				pGDI->DrawTexture(m_extendBtnNormal.pBitmap, m_extendBtnNormal.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}

		if (pSlot->item.typeId == 0) {
			if (pSlotBitmap) {
				pGDI->DrawTexture(pSlotBitmap, WRect(pItem->pos.x, pItem->pos.y, (float)pSlotBitmap->Width(), (float)pSlotBitmap->Height()), 0xFFFFFFFF, 0);
			}
			// "shop item slot"
			pGDI->Print(pItem->pos + WPoint(182.0f, 7.0f), 2, "\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4");

			if (pItem->underCursor && (m_uploadBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
				pGDI->DrawTexture(m_uploadBtnOver.pBitmap, m_uploadBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x44);
			} else {
				pGDI->DrawTexture(m_uploadBtnNormal.pBitmap, m_uploadBtnNormal.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		} else if (pSlot->sold == 0 && hours < 0) {
			if (pItem->underCursor && (m_returnBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
				pGDI->DrawTexture(m_returnBtnOver.pBitmap, m_returnBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x45);
			} else {
				pGDI->DrawTexture(m_returnBtnNormal.pBitmap, m_returnBtnNormal.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		} else {
			if (pItem->underCursor && (m_deleteBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
				pGDI->DrawTexture(m_deleteBtnOver.pBitmap, m_deleteBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x42);
			} else {
				pGDI->DrawTexture(m_deleteBtnNormal.pBitmap, m_deleteBtnNormal.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}
	}
}

// offlinetradedlg.cpp:275
void FrOfflineTradeFinishEditDlg::OnTradeItemListLBtnUp(void) {
	FrListItem* pItem = m_pTradeItemList->GetItemUnderCursor();
	if (pItem == 0) {
		return;
	}

	FrWndManager* pManager = g_pFresh->GetManager();
	const WPoint& mousePos = pManager->GetMousePos();
	sStallSlot* pSlot = (sStallSlot*)pItem->pData;

	if (pSlot->reserved == 0
		&& ((m_buyBtnNormal.rect + pItem->pos).IsInRect(mousePos)
		|| (m_buyBtnOver.rect + pItem->pos).IsInRect(mousePos))) {
		CBuyingSlotDlg* pDlg = CreateForm<CBuyingSlotDlg>(pManager, this, "buyslotdlg", 0);
		pDlg->SetSlotNum(pItem->idx + 1);
		pDlg->Open((FRESH_RESULT_CALLBACK)0, 1);
		return;
	}

	if (pSlot->reserved == 1 && pSlot->item.typeId == 0
		&& ((m_uploadBtnNormal.rect + pItem->pos).IsInRect(mousePos)
		|| (m_uploadBtnOver.rect + pItem->pos).IsInRect(mousePos))) {
		m_pSelectedSlotItem = pItem;
		FrForm::OnOK();
		return;
	}

	if (pSlot->reserved == 1
		&& ((m_extendBtnNormal.rect + pItem->pos).IsInRect(mousePos)
		|| (m_extendBtnOver.rect + pItem->pos).IsInRect(mousePos))) {
		CBuyingSlotDlg* pDlg = CreateForm<CBuyingSlotDlg>(pManager, this, "buyslotdlg", 0);
		pDlg->SetSlotNum(pItem->idx + 1);
		pDlg->Open((FRESH_RESULT_CALLBACK)0, 1);
	}
}

// offlinetradedlg.cpp:324
void FrOfflineTradeFinishEditDlg::SetListTradeItem(std::list<sTradeItem>* pList) {
	sStall* pStall = CSharedDoc::Instance()->GetStallItem();

	m_pTradeItemList->ClearItem();

	for (int i = 0; i < pStall->open; i++) {
		m_pTradeItemList->AddItem(&pStall->slots[i]);
	}
}

// offlinetradedlg.cpp:347
void FrOfflineTradeFinishEditDlg::GetSelectSlotInfo(int& index, int& used) {
	if (m_pSelectedSlotItem == 0) {
		index = used = 0;
		return;
	}

	sStallSlot* pSlot = (sStallSlot*)m_pSelectedSlotItem->pData;
	index = pSlot->index;
	used = pSlot->used;
}

// offlinetradedlg.cpp:363
void FrOfflineTradeFinishEditDlg::OnToolTipInit(intptr_t param) {
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

// offlinetradedlg.cpp:369
void FrOfflineTradeFinishEditDlg::OnToolTipOnwerDraw(int) {
	CIconToolTip::GetInstance()->Render();
}

// offlinetradedlg.cpp:397 (run-time type and factory registration of COfflineTradeDlg)
const WRTTI COfflineTradeDlg::m_RTTI("COfflineTradeDlg", &FrForm::m_RTTI);

IObject* COfflineTradeDlgMakeInstance(void) {
	return new COfflineTradeDlg;
}

struct __sCOfflineTradeDlg {
	__sCOfflineTradeDlg(void) {
		ObjectFactory().AddObjectFunctor(COfflineTradeDlgMakeInstance, "COfflineTradeDlg");
	}
};
static __sCOfflineTradeDlg s_registerCOfflineTradeDlg;

// offlinetradedlg.cpp:399
const sFRESH_MSGMAP* COfflineTradeDlg::GetMessageMap(void) const {
	return &COfflineTradeDlg::_MsgMap;
}

sFRESH_MSGMAP COfflineTradeDlg::_MsgMap = { &FrForm::_MsgMap, &COfflineTradeDlg::_MsgEntries[0] };

sFRESH_ENTRY COfflineTradeDlg::_MsgEntries[] = {
	// offlinetradedlg.cpp:402
	{ "AreaSub_Nick", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnAreaSubNickInit },
	// offlinetradedlg.cpp:403
	{ "AreaSub_Nick", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnAreaSubNickOwnerDraw },
	// offlinetradedlg.cpp:404
	{ "AreaSub_Nick", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnAreaSubNickLBtnUP },

	// offlinetradedlg.cpp:406
	{ "AreaSub_Price", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnAreaSubPriceInit },
	// offlinetradedlg.cpp:407
	{ "AreaSub_Price", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnAreaSubPriceOwnerDraw },
	// offlinetradedlg.cpp:408
	{ "AreaSub_Price", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnAreaSubPriceLBtnUP },

	// offlinetradedlg.cpp:410
	{ "AreaSub_ItemName", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnAreaSubItemNameInit },
	// offlinetradedlg.cpp:411
	{ "AreaSub_ItemName", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnAreaSubItemNameOwnerDraw },
	// offlinetradedlg.cpp:412
	{ "AreaSub_ItemName", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnAreaSubItemNameLBtnUP },

	// offlinetradedlg.cpp:414
	{ "Edit_ItemSearch", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnItemSearchInit },
	// offlinetradedlg.cpp:415
	{ "Edit_ItemSearch", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&COfflineTradeDlg::OnItemSearchEnterKey },
	// offlinetradedlg.cpp:416
	{ "Edit_ItemSearch", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnItemSearchLBtnUp },

	// offlinetradedlg.cpp:418
	{ "BtnMain_search", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnBtnMainSearchInit },
	// offlinetradedlg.cpp:419
	{ "BtnMain_search", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnBtnMainSearchUp },

	// offlinetradedlg.cpp:421
	{ "num_maxpage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnEditMaxPageInit },
	// offlinetradedlg.cpp:422
	{ "num_curpage", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnEditCurrentPageInit },
	// offlinetradedlg.cpp:423
	{ "num_curpage", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&COfflineTradeDlg::OnEditCurrentPageEnterKey },

	// offlinetradedlg.cpp:425
	{ "BtnPage_Prev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnBtnPrevInit },
	// offlinetradedlg.cpp:426
	{ "BtnPage_Prev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnBtnPrevBtnUp },

	// offlinetradedlg.cpp:428
	{ "BtnPage_Next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnBtnNextInit },
	// offlinetradedlg.cpp:429
	{ "BtnPage_Next", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnBtnNextBtnUp },

	// offlinetradedlg.cpp:431
	{ "BtnPage_go", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnBtnGoPageInit },
	// offlinetradedlg.cpp:432
	{ "BtnPage_go", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnBtnGoPageBtnUp },

	// offlinetradedlg.cpp:434
	{ "BtnRefresh", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnBtnRefreshInit },
	// offlinetradedlg.cpp:435
	{ "BtnRefresh", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnBtnRefreshBtnUp },

	// offlinetradedlg.cpp:437
	{ "BtnSub_Quick", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnBtnDetailInit },
	// offlinetradedlg.cpp:438
	{ "BtnSub_Quick", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnBtnDetailBtnUp },

	// offlinetradedlg.cpp:440
	{ "offlinetrade_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnMaketListInit },
	// offlinetradedlg.cpp:441
	{ "offlinetrade_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnMaketListOwnerDraw },
	// offlinetradedlg.cpp:442
	{ "offlinetrade_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnMaketListBtnDown },

	// offlinetradedlg.cpp:447
	{ "Edit_Msg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnEditMsgInit },
	// offlinetradedlg.cpp:448
	{ "Edit_Msg", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineTradeDlg::OnEditMsgLBtnUp },

	// offlinetradedlg.cpp:450
	{ "tooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnToolTipInit },
	// offlinetradedlg.cpp:451
	{ "tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineTradeDlg::OnToolTipOnwerDraw },

	// offlinetradedlg.cpp:454
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// offlinetradedlg.cpp:458
COfflineTradeDlg::COfflineTradeDlg(void) {
	m_pAreaSubNick = 0;
	m_pAreaSubPrice = 0;
	m_pAreaSubItemName = 0;
	m_pItemSearchEdit = 0;
	m_pMaketList = 0;
	m_pCurrentPageEdit = 0;
	m_pMaxPageEdit = 0;
	m_pBtnPrev = 0;
	m_pBtnNext = 0;
	m_pBtnGoPage = 0;
	m_pBtnMainSearch = 0;
	m_pBtnRefresh = 0;
	m_pStandingInLine = 0;
	m_pBtnDetail = 0;
	m_pEditMsg = 0;
	m_maxPage = 0;
	m_currentPage = 0;
	m_searchType = 1;
	m_searchOrder = 0;
	m_searchString.clear();

	// "Pick an item"
	SetSearchString("\xB9\xB0\xB0\xC7\xC0\xBB \xB0\xF1\xB6\xF3\xBA\xB8\xB7\xC5");

	m_myRoomState = false;
	m_canVisitMyRoom = 0;

	m_moveBtnNormal.pBitmap = g_pFresh->GetBitmap("rmr_btn_move_n");
	m_moveBtnNormal.rect = WRect(490.0f, 1.0f, m_moveBtnNormal.pBitmap->Width(), m_moveBtnNormal.pBitmap->Height());
	m_moveBtnOver.pBitmap = g_pFresh->GetBitmap("rmr_btn_move_o");
	m_moveBtnOver.rect = WRect(490.0f, 1.0f, m_moveBtnOver.pBitmap->Width(), m_moveBtnOver.pBitmap->Height());
}

// offlinetradedlg.cpp:507
COfflineTradeDlg::~COfflineTradeDlg(void) {
}

// offlinetradedlg.cpp:511
bool COfflineTradeDlg::OnInit(void) {
	if (m_pItemSearchEdit) {
		m_pItemSearchEdit->SetFontColor(0xFFD1D1D0);
		m_pItemSearchEdit->ClearLine();
		m_pItemSearchEdit->AddLine("\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5", 0, false);
		m_pItemSearchEdit->SetKeyFocus(true);
	}

	int canVisit = 0;
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x231, (intptr_t)&canVisit, 2, 3, 0, 0));

	m_canVisitMyRoom = canVisit;

	if (m_canVisitMyRoom == 0) {
		m_pEditMsg->SetVisible(true);
		m_pEditMsg->AddText("* \xc7\xf6\xc0\xe7 \xbb\xf3\xc5\xc2\xbf\xa1\xbc\xad\xb4\xc2 \xb4\xd9\xb8\xa5 "
			"\xbb\xe7\xb6\xf7\xc0\xc7 \xb8\xb6\xc0\xcc\xb7\xeb\xc0\xbb \xb9\xe6\xb9\xae "
			"\xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false, true);
	} else {
		m_pEditMsg->SetVisible(false);
	}

	return true;
}

// offlinetradedlg.cpp:538
void COfflineTradeDlg::OnProc(const float dt) {
	FrForm::OnProc(dt);
	CIconToolTip::GetInstance()->ClearToolTip();
}

// offlinetradedlg.cpp:544
void COfflineTradeDlg::OnAreaSubNickInit(intptr_t param) {
	m_pAreaSubNick = DYNAMIC_CAST(FrViewer, param);

	if (m_pAreaSubNick) {
		WRect rect = m_pAreaSubNick->GetRect();

		m_pAreaSubNick->SetRect(WRect(rect.x, rect.y, rect.w + 55.0f, rect.h));
	}
}

// offlinetradedlg.cpp:556
void COfflineTradeDlg::OnAreaSubNickOwnerDraw(int) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

	WRect rect = m_pAreaSubNick->GetRect();

	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

	if (mouse.x >= rect.x && mouse.x <= rect.x + rect.w && mouse.y >= rect.y && mouse.y <= rect.y + rect.h) {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
		pGDI->SetTextStyle(2);
	} else {
		pGDI->SetTextStyle(0);
	}

	pGDI->Print(WPoint(rect.x + 10.0f, rect.y), 0, "\xb4\xd0\xb3\xd7\xc0\xd3");
}

// offlinetradedlg.cpp:582
void COfflineTradeDlg::OnAreaSubNickLBtnUP(void) {
	m_searchType = 4;
	m_searchOrder = 0;
	m_currentPage = 1;
	if (SetSearchString("")) {
		RequestMaketList();
	}
}

// offlinetradedlg.cpp:592
void COfflineTradeDlg::OnAreaSubPriceInit(intptr_t param) {
	m_pAreaSubPrice = DYNAMIC_CAST(FrViewer, param);

	if (m_pAreaSubPrice) {
		WRect rect = m_pAreaSubPrice->GetRect();

		m_pAreaSubPrice->SetRect(WRect(rect.x, rect.y, rect.w + 35.0f, rect.h));
	}
}

// offlinetradedlg.cpp:603
void COfflineTradeDlg::OnAreaSubPriceOwnerDraw(int) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

	WRect rect = m_pAreaSubPrice->GetRect();

	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

	if (mouse.x >= rect.x && mouse.x <= rect.x + rect.w && mouse.y >= rect.y && mouse.y <= rect.y + rect.h) {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
		pGDI->SetTextStyle(2);
	} else {
		pGDI->SetTextStyle(0);
	}

	pGDI->Print(WPoint(rect.x + 10.0f, rect.y), 0, "\xb0\xa1\xb0\xdd");
}

// offlinetradedlg.cpp:632
void COfflineTradeDlg::OnAreaSubPriceLBtnUP(void) {
	m_searchType = 3;
	m_searchOrder = 0;
	m_currentPage = 1;
	if (SetSearchString("")) {
		RequestMaketList();
	}
}

// offlinetradedlg.cpp:643
void COfflineTradeDlg::OnAreaSubItemNameInit(intptr_t param) {
	m_pAreaSubItemName = DYNAMIC_CAST(FrViewer, param);

	if (m_pAreaSubItemName) {
		WRect rect = m_pAreaSubItemName->GetRect();

		m_pAreaSubItemName->SetRect(WRect(rect.x, rect.y, rect.w + 75.0f, rect.h));
	}
}

// offlinetradedlg.cpp:655
void COfflineTradeDlg::OnAreaSubItemNameOwnerDraw(int) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);

	WRect rect = m_pAreaSubItemName->GetRect();

	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

	if (mouse.x >= rect.x && mouse.x <= rect.x + rect.w && mouse.y >= rect.y && mouse.y <= rect.y + rect.h) {
		pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
		pGDI->SetTextStyle(2);
	} else {
		pGDI->SetTextStyle(0);
	}

	pGDI->Print(WPoint(rect.x + 10.0f, rect.y), 0, "\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc\xb8\xa7");
}

// offlinetradedlg.cpp:682
void COfflineTradeDlg::OnAreaSubItemNameLBtnUP(void) {
	m_searchType = 1;
	m_searchOrder = 0;
	m_currentPage = 1;
	if (SetSearchString("")) {
		RequestMaketList();
	}
}

// offlinetradedlg.cpp:691
void COfflineTradeDlg::OnMaketListInit(intptr_t param) {
	m_pMaketList = DYNAMIC_CAST(FrListBox, param);
}

// offlinetradedlg.cpp:706
void COfflineTradeDlg::SetListItem(void) {
	char text[32];  // unused: keeps the original's /GS cookie

	std::list<sSaleItem>* pList = CSharedDoc::Instance()->GetMaketList();

	if (m_pMaketList) {
		m_pMaketList->ClearItem();

		for (std::list<sSaleItem>::iterator it = pList->begin(); it != pList->end(); ++it) {
			m_pMaketList->AddItem(&(*it));
		}
	}
}

// offlinetradedlg.cpp:726
void COfflineTradeDlg::OnMaketListOwnerDraw(int param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	FrListItem* pItem = (FrListItem*)param;

	if (!pGDI || !pItem) {
		return;
	}

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	pGDI->SetTextStyle(0);

	sSaleItem* pSaleItem = (sSaleItem*)pItem->pData;

	const Bitmap* pSelectBitmap = g_pFresh->GetBitmap("maketlist_select");

	if (pItem->selected || pItem->underCursor) {
		if (pSelectBitmap) {
			pGDI->DrawTexture(pSelectBitmap, WRect(pItem->pos.x, pItem->pos.y - 5.0f, 520.0f, 23.0f), 0xFFFFFFFF, 0);
		}
	}

	if (pSaleItem->m_public == 1) {
		// "public"
		pGDI->Print(WPoint(pItem->pos.x + 470.0f, pItem->pos.y + 1.0f), 2, "\xB0\xF8  \xB0\xB3");
	} else {
		pGDI->SetTextColor(0xFFD1D1D0, 0xFFFFFFFF);
		// "private"
		pGDI->Print(WPoint(pItem->pos.x + 470.0f, pItem->pos.y + 1.0f), 2, "\xBA\xF1\xB0\xF8\xB0\xB3");
	}

	IFF_ITEM_COMMON* pCommon = CSharedDoc::Instance()->m_itemManager.FindCommonItem(pSaleItem->m_typeId);

	if (pCommon) {
		pGDI->Print(WPoint(pItem->pos.x + 160.0f, pItem->pos.y + 1.0f), 2, "%s", pCommon->m_name);
	}

	pGDI->Print(WPoint(pItem->pos.x + 250.0f, pItem->pos.y + 1.0f), 2, "%d", pSaleItem->m_price);
	pGDI->Print(WPoint(pItem->pos.x + 280.0f, pItem->pos.y + 1.0f), 2, "%d", pSaleItem->m_count);
	pGDI->Print(WPoint(pItem->pos.x + 400.0f, pItem->pos.y + 1.0f), 2, "%s", pSaleItem->m_ownerNick);

	if (m_canVisitMyRoom) {
		if (pItem->underCursor && WRect(m_moveBtnNormal.rect.x + pItem->pos.x, pItem->pos.y + m_moveBtnNormal.rect.y,
				m_moveBtnNormal.rect.w, m_moveBtnNormal.rect.h).IsInRect(g_pFresh->GetManager()->GetMousePos())) {
			pGDI->DrawTexture(m_moveBtnOver.pBitmap, m_moveBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x16);
		} else {
			pGDI->DrawTexture(m_moveBtnNormal.pBitmap, WRect(pItem->pos.x + m_moveBtnNormal.rect.x, pItem->pos.y + m_moveBtnNormal.rect.y,
				m_moveBtnNormal.rect.w, m_moveBtnNormal.rect.h), 0xFFFFFFFF, 0);
		}
	}

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
}

// offlinetradedlg.cpp:795
void COfflineTradeDlg::OnMaketListBtnDown(void) {
	FrListItem* pItem = m_pMaketList->GetItemUnderCursor();
	if (!pItem) {
		return;
	}

	if (g_pFresh->IsCurrentLayout("REALMYROOM") || g_pFresh->IsCurrentLayout("REALMYROOMVISITOR")) {
		return;
	}

	sSaleItem* pSaleItem = (sSaleItem*)pItem->pData;
	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();

	if ((m_moveBtnNormal.rect + pItem->pos).IsInRect(mousePos)
		|| (m_moveBtnOver.rect + pItem->pos).IsInRect(mousePos)) {
		CSharedDoc::Instance()->m_myRoomOwnerUID = pSaleItem->m_ownerUID;

		if (CSharedDoc::Instance()->m_myRoomOwnerUID == 0xFFFFFFFF) {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			// "Could not get the information of the user to visit."
			pForm->SetMessage("\xB9\xE6\xB9\xAE\xC7\xD2 \xC0\xAF\xC0\xFA\xC0\xC7 \xC1\xA4\xBA\xB8\xB8\xA6 \xBE\xF2\xBE\xEE\xBF\xC0\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		} else {
			if (CSharedDoc::Instance()->m_myUID == CSharedDoc::Instance()->m_myRoomOwnerUID) {
				CSharedDoc::Instance()->m_rmrMode = 1;
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x222, 0, 0, 0, 0, 0));
			} else {
				CSharedDoc::Instance()->m_rmrMode = 5;
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x222, 0, 0, 0, 0, 0));
			}
		}
	}
}

// offlinetradedlg.cpp:835
void COfflineTradeDlg::OnItemSearchInit(intptr_t param) {
	m_pItemSearchEdit = DYNAMIC_CAST(FrEdit, param);

	if (m_pItemSearchEdit) {
		m_pItemSearchEdit->SetFontColor(0xFFD1D1D0);
		m_pItemSearchEdit->ClearLine();
		m_pItemSearchEdit->AddLine("\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5", 0, false);
		m_pItemSearchEdit->SetKeyFocus(true);
	}
}

// offlinetradedlg.cpp:849
bool COfflineTradeDlg::OnItemSearchEnterKey(int) {
	OnAreaSubItemNameLBtnUP();
	return true;
}

// offlinetradedlg.cpp:857
void COfflineTradeDlg::OnItemSearchLBtnUp(void) {
	std::string text = m_pItemSearchEdit->GetLine(1, false);
	std::string empty;
	empty.clear();

	const char* pText = text.c_str();

	if (stricmp("\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5", pText) == 0) {
		m_pItemSearchEdit->SetFontColor(0xFF000000);
		m_pItemSearchEdit->ClearLine();

		m_pItemSearchEdit->AddLine("", 0, false);
		m_pItemSearchEdit->SetKeyFocus(true);
	}
}

// offlinetradedlg.cpp:877
void COfflineTradeDlg::OnBtnMainSearchInit(intptr_t param) {
	m_pBtnMainSearch = DYNAMIC_CAST(FrButton, param);
}

// offlinetradedlg.cpp:885
void COfflineTradeDlg::OnBtnMainSearchUp(void) {
	m_searchType = 1;
	m_searchOrder = 0;
	m_currentPage = 1;
	if (SetSearchString("")) {
		RequestMaketList();
	}
}

// offlinetradedlg.cpp:911
void COfflineTradeDlg::OnEditMaxPageInit(intptr_t param) {
	m_pMaxPageEdit = DYNAMIC_CAST(FrEdit, param);

	if (m_pMaxPageEdit) {
		m_pMaxPageEdit->AddLine(MakeStr("%d", m_maxPage), 0, false);
	}
}

// offlinetradedlg.cpp:922
void COfflineTradeDlg::OnEditCurrentPageInit(intptr_t param) {
	m_pCurrentPageEdit = DYNAMIC_CAST(FrEdit, param);

	if (m_pCurrentPageEdit) {
		m_pCurrentPageEdit->AddLine(MakeStr("%d", m_currentPage), 0, false);
	}
}

// offlinetradedlg.cpp:932
bool COfflineTradeDlg::OnEditCurrentPageEnterKey(int) {
	return true;
}

// offlinetradedlg.cpp:938
void COfflineTradeDlg::SetPageData(int currentPage, int maxPage) {
	if (currentPage != -1) {
		m_currentPage = currentPage;
		m_pBtnDetail->SetVisible(false);
	}

	m_maxPage = maxPage;

	if (m_currentPage < 1) {
		m_currentPage = 1;
	}
	if (m_currentPage > m_maxPage) {
		m_currentPage = m_maxPage;
	}

	if (m_pCurrentPageEdit) {
		m_pCurrentPageEdit->ClearLine();
		m_pCurrentPageEdit->AddLine(MakeStr("%d", m_currentPage), 0, false);
	}

	if (m_pMaxPageEdit) {
		m_pMaxPageEdit->ClearLine();
		m_pMaxPageEdit->AddLine(MakeStr("%d", m_maxPage), 0, false);
	}
}

// offlinetradedlg.cpp:969
void COfflineTradeDlg::RequestMaketList(void) {
	const char* pText = m_searchString.c_str();

	if (stricmp("\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5", pText) == 0) {
		m_searchString.clear();
	}

	if (m_searchString.size() != 0) {
		if (IsLocalContent((localContentType_t)0x66)) {
			WSendPacket packet(GCT_OFFLINESHOP_REQUEST_SALEITEMFIND);
			packet.Encode1((unsigned char)m_searchType);
			packet.Encode1((unsigned char)m_searchOrder);
			packet.EncodeStr(m_searchString);
			packet.Encode2((unsigned short)m_currentPage);
			packet.Send(SEND_TO_DEFAULT);

			CTaskManager* pTaskManager = CTaskManager::Instance();
			(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));

			m_pBtnDetail->SetVisible(true);
		}
	} else {
		if (IsLocalContent((localContentType_t)0x66)) {
			WSendPacket packet(GCT_OFFLINESHOP_REQUEST_SALEITEMLIST);
			packet.Encode2((unsigned short)m_currentPage);
			packet.Send(SEND_TO_DEFAULT);

			CTaskManager* pTaskManager = CTaskManager::Instance();
			(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
		}
	}
}

// offlinetradedlg.cpp:1021
void COfflineTradeDlg::OnBtnPrevInit(intptr_t param) {
	m_pBtnPrev = DYNAMIC_CAST(FrButton, param);
}

// offlinetradedlg.cpp:1029
void COfflineTradeDlg::OnBtnPrevBtnUp(void) {
	SetPageData(--m_currentPage, m_maxPage);
	RequestMaketList();
}

// offlinetradedlg.cpp:1048
void COfflineTradeDlg::OnBtnNextInit(intptr_t param) {
	m_pBtnNext = DYNAMIC_CAST(FrButton, param);
}

// offlinetradedlg.cpp:1057
void COfflineTradeDlg::OnBtnNextBtnUp(void) {
	SetPageData(++m_currentPage, m_maxPage);
	RequestMaketList();
}

// offlinetradedlg.cpp:1076
void COfflineTradeDlg::OnBtnGoPageInit(intptr_t param) {
	m_pBtnGoPage = DYNAMIC_CAST(FrButton, param);
}

// offlinetradedlg.cpp:1085
void COfflineTradeDlg::OnBtnGoPageBtnUp(void) {
	m_currentPage = atoi(m_pCurrentPageEdit->GetLine(1, false));

	SetPageData(m_currentPage, m_maxPage);
	RequestMaketList();
}

// offlinetradedlg.cpp:1104
void COfflineTradeDlg::OnBtnRefreshInit(intptr_t param) {
	m_pBtnRefresh = DYNAMIC_CAST(FrButton, param);
}

// offlinetradedlg.cpp:1112
void COfflineTradeDlg::OnBtnRefreshBtnUp(void) {
	SetPageData(m_currentPage = 1, m_maxPage);

	m_searchType = 1;
	m_searchOrder = 0;
	m_searchString.clear();

	SetSearchString("\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5");

	m_myRoomState = false;
	m_canVisitMyRoom = 0;
	m_pEditMsg->SetVisible(true);
	m_pEditMsg->AddText("* \xc7\xf6\xc0\xe7 \xbb\xf3\xc5\xc2\xbf\xa1\xbc\xad\xb4\xc2 \xb4\xd9\xb8\xa5 "
		"\xbb\xe7\xb6\xf7\xc0\xc7 \xb8\xb6\xc0\xcc\xb7\xeb\xc0\xbb \xb9\xe6\xb9\xae "
		"\xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false, true);

	if (IsLocalContent((localContentType_t)0x66)) {
		WSendPacket packet(GCT_OFFLINESHOP_REQUEST_SALEITEMLIST);
		packet.Encode2((unsigned short)m_currentPage);
		packet.Send(SEND_TO_DEFAULT);

		m_pItemSearchEdit->SetFontColor(0xFFD1D1D0);
		m_pItemSearchEdit->ClearLine();
		m_pItemSearchEdit->AddLine("\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5", 0, false);
		m_pItemSearchEdit->SetKeyFocus(true);

		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	}

	m_pBtnDetail->SetVisible(false);
}

// offlinetradedlg.cpp:1153
void COfflineTradeDlg::OnBtnDetailInit(intptr_t param) {
	m_pBtnDetail = DYNAMIC_CAST(FrButton, param);

	if (m_pBtnDetail) {
		m_pBtnDetail->SetVisible(false);
	}
}

// offlinetradedlg.cpp:1163
void COfflineTradeDlg::OnBtnDetailBtnUp(void) {
	WRect rect;
	rect = m_pBtnDetail->GetRect();

	if (!m_pStandingInLine) {
		m_pStandingInLine = CreateForm<CStandingInLine>(g_pFresh->GetManager(), this, "arraydlg", 0);
		if (m_pStandingInLine) {
			m_pStandingInLine->Open((FRESH_RESULT_CALLBACK)&COfflineTradeDlg::OnStandingInLineDlgResult,
				WPoint(rect.x + 10.0f, rect.y), 1);
		}
	}
}

// offlinetradedlg.cpp:1180
void COfflineTradeDlg::OnEditMsgInit(intptr_t param) {
	m_pEditMsg = DYNAMIC_CAST(FrEdit, param);

	if (m_pEditMsg) {
		m_pEditMsg->SetVisible(false);
	}
}

// offlinetradedlg.cpp:1191
void COfflineTradeDlg::OnEditMsgLBtnUp(void) {
}

// offlinetradedlg.cpp:1198
void COfflineTradeDlg::OnToolTipInit(intptr_t param) {
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

// offlinetradedlg.cpp:1204
void COfflineTradeDlg::OnToolTipOnwerDraw(int) {
	CIconToolTip::GetInstance()->Render();
}

// offlinetradedlg.cpp:1229
bool COfflineTradeDlg::SetSearchString(std::string searchString) {
	if (searchString.size() == 0) {
		std::string text;

		if (m_pItemSearchEdit) {
			text = m_pItemSearchEdit->GetLine(1, false);
		}

		if (text.size() == 0) {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage("\xb0\xcb\xbb\xf6\xc7\xcf\xb7\xc1\xb4\xc2 \xb4\xdc\xbe\xee\xb0\xa1 "
				"\xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xb0\xc5\xb3\xaa \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
				"\xb4\xd9\xbd\xc3 \xc0\xd4\xb7\xc2\xc7\xcf\xbc\xbc\xbf\xe4", false);
			pForm->Open(0, 3);

			m_searchString.clear();
			return false;
		}

		SetSearchString(text);
	} else {
		m_searchString = searchString;
	}

	return true;
}

// offlinetradedlg.cpp:1265
bool COfflineTradeDlg::OnStandingInLineDlgResult(int ret, FrForm* pForm) {
	int selectNum = 0;

	if (ret == 1) {
		if (m_pStandingInLine) {
			selectNum = m_pStandingInLine->GetSelectNum();
		}
	}

	switch (selectNum) {
	case 0: m_searchOrder = 0; break;
	case 1: m_searchOrder = 10; break;
	case 2: m_searchOrder = 11; break;
	case 3: m_searchOrder = 12; break;
	case 4: m_searchOrder = 13; break;
	case 5: m_searchOrder = 14; break;
	case 6: m_searchOrder = 15; break;
	case 7: m_searchOrder = 16; break;
	case 8: m_searchOrder = 17; break;
	case 9: m_searchOrder = 18; break;
	case 11: m_searchOrder = 21; break;
	case 12: m_searchOrder = 23; break;
	case 13: m_searchOrder = 22; break;
	case 14: m_searchOrder = 24; break;
	case 15: m_searchOrder = 19; break;
	}

	m_currentPage = 1;
	RequestMaketList();

	m_pStandingInLine = 0;

	return true;
}

// offlinetradedlg.cpp:1313 (run-time type and factory registration of CBuyingSlotDlg)
const WRTTI CBuyingSlotDlg::m_RTTI("CBuyingSlotDlg", &FrForm::m_RTTI);

IObject* CBuyingSlotDlgMakeInstance(void) {
	return new CBuyingSlotDlg;
}

struct __sCBuyingSlotDlg {
	__sCBuyingSlotDlg(void) {
		ObjectFactory().AddObjectFunctor(CBuyingSlotDlgMakeInstance, "CBuyingSlotDlg");
	}
};
static __sCBuyingSlotDlg s_registerCBuyingSlotDlg;

// offlinetradedlg.cpp:1314
const sFRESH_MSGMAP* CBuyingSlotDlg::GetMessageMap(void) const {
	return &CBuyingSlotDlg::_MsgMap;
}

sFRESH_MSGMAP CBuyingSlotDlg::_MsgMap = { &FrForm::_MsgMap, &CBuyingSlotDlg::_MsgEntries[0] };

sFRESH_ENTRY CBuyingSlotDlg::_MsgEntries[] = {
	// offlinetradedlg.cpp:1316
	{ "days", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CBuyingSlotDlg::OnDaysInit },
	// offlinetradedlg.cpp:1317
	{ "days", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CBuyingSlotDlg::OnDaysBtnDown },

	// offlinetradedlg.cpp:1319
	{ "my_money", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CBuyingSlotDlg::OnMyMoneyInit },
	// offlinetradedlg.cpp:1320
	{ "money_left", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CBuyingSlotDlg::OnMoneyLeftInit },
	// offlinetradedlg.cpp:1321
	{ "price", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CBuyingSlotDlg::OnPriceInit },

	// offlinetradedlg.cpp:1323
	{ "checkout", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CBuyingSlotDlg::OnCheckOutInit },
	// offlinetradedlg.cpp:1324
	{ "checkout", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&CBuyingSlotDlg::OnCheckOutBtnDown },

	// offlinetradedlg.cpp:1326
	{ "slot_price", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&CBuyingSlotDlg::OnSlotPriceInit },

	// offlinetradedlg.cpp:1328
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// offlinetradedlg.cpp:1333
struct sdays_t {
	const char* const m_name;   // 0x0
	const int m_days;           // 0x4
};

sdays_t sDay[] = {
	{ "1\xc0\xcf", 1 },
	{ "7\xc0\xcf", 7 },
	{ "15\xc0\xcf", 15 },
	{ "30\xc0\xcf", 30 },
	{ "365\xc0\xcf", 365 },
	{ "\xb9\xab\xc1\xa6\xc7\xd1", 0 },
};

// offlinetradedlg.cpp:1342
CBuyingSlotDlg::CBuyingSlotDlg(void) {
	m_pDays = 0;
	m_pMyMoney = 0;
	m_pMoneyLeft = 0;
	m_pPrice = 0;
	m_pCheckOut = 0;
	m_pSlotPrice = 0;

	m_price = 0;
}

// offlinetradedlg.cpp:1358
CBuyingSlotDlg::~CBuyingSlotDlg(void) {
}

// offlinetradedlg.cpp:1362
void CBuyingSlotDlg::OnDaysInit(intptr_t param) {
	m_pDays = DYNAMIC_CAST(FrComboBox, param);

	if (m_pDays) {
		m_pDays->ClearList();

		IFF_STRUCT::sFurniture* pFurniture = ItemManager()->FindFurniture(0x4800601E);

		for (int i = 0; i < 4; i++) {
			if (pFurniture->m_dayPrices[i] != 0) {
				m_pDays->AddString(sDay[i].m_name);
			}
		}

		m_pDays->SetLine(1, sDay[0].m_name, 0, false, 0);
		m_price = pFurniture->m_dayPrices[0];
		m_pSlotPrice->ClearLine();
		m_pSlotPrice->AddText(MakeStr("%d \xc6\xce", m_price), false, true);
	}
}

// offlinetradedlg.cpp:1387
void CBuyingSlotDlg::OnDaysBtnDown(void) {
	IFF_STRUCT::sFurniture* pFurniture = ItemManager()->FindFurniture(0x4800601E);
	const char* pText = m_pDays->GetLine(1, false);

	for (int i = 0; i < 5; i++) {
		if (stricmp(sDay[i].m_name, pText) == 0) {
			m_price = pFurniture->m_dayPrices[i];
			m_pSlotPrice->ClearLine();
			m_pSlotPrice->AddText(MakeStr("%d \xc6\xce", m_price), false, true);
		}
	}

	RecalculatePang();
}

// offlinetradedlg.cpp:1405
void CBuyingSlotDlg::OnPriceInit(intptr_t param) {
	m_pPrice = DYNAMIC_CAST(FrEdit, param);

	if (m_pPrice) {
		char buf[32];

		MakePriceStrA(buf, m_price);
		strcat(buf, "\xc6\xce");
		m_pPrice->SetLine(1, buf, 0, false, 0);
	}
}

// offlinetradedlg.cpp:1420
void CBuyingSlotDlg::OnMyMoneyInit(intptr_t param) {
	m_pMyMoney = DYNAMIC_CAST(FrEdit, param);

	if (m_pMyMoney) {
		char buf[32];

		MakePriceStrA(buf, CSharedDoc::Instance()->m_myPang);
		strcat(buf, "\xc6\xce");
		m_pMyMoney->SetLine(1, buf, 0, false, 0);
	}
}

// offlinetradedlg.cpp:1434
void CBuyingSlotDlg::OnMoneyLeftInit(intptr_t param) {
	m_pMoneyLeft = DYNAMIC_CAST(FrEdit, param);

	if (m_pMoneyLeft) {
		char buf[32];

		__int64 left = CSharedDoc::Instance()->m_myPang - m_price;
		if (left < 0) {
			MakePriceStrA(buf, m_price - CSharedDoc::Instance()->m_myPang);
			strcat(buf, "\xc6\xce \xba\xce\xc1\xb7");
			m_pMoneyLeft->SetLine(1, buf, 0xFFFF0000, false, 0);
		} else {
			MakePriceStrA(buf, left);
			strcat(buf, "\xc6\xce");
			m_pMoneyLeft->SetLine(1, buf, 0, false, 0);
		}
	}
}

// offlinetradedlg.cpp:1457
void CBuyingSlotDlg::RecalculatePang(void) {
	char buf[32];

	if (m_pPrice) {
		MakePriceStrA(buf, m_price);
		strcat(buf, "\xc6\xce");
		m_pPrice->SetLine(1, buf, 0, false, 0);
	}

	if (m_pMyMoney) {
		MakePriceStrA(buf, CSharedDoc::Instance()->m_myPang);
		strcat(buf, "\xc6\xce");
		m_pMyMoney->SetLine(1, buf, 0, false, 0);
	}

	if (m_pMoneyLeft) {
		__int64 left = CSharedDoc::Instance()->m_myPang - m_price;
		if (left < 0) {
			MakePriceStrA(buf, m_price - CSharedDoc::Instance()->m_myPang);
			strcat(buf, "\xc6\xce \xba\xce\xc1\xb7");
			m_pMoneyLeft->SetLine(1, buf, 0xFFFF0000, false, 0);
		} else {
			MakePriceStrA(buf, left);
			strcat(buf, "\xc6\xce");
			m_pMoneyLeft->SetLine(1, buf, 0, false, 0);
		}
	}
}

// offlinetradedlg.cpp:1529
void CBuyingSlotDlg::OnCheckOutInit(intptr_t param) {
	m_pCheckOut = DYNAMIC_CAST(FrButton, param);
}

// offlinetradedlg.cpp:1537
void CBuyingSlotDlg::OnCheckOutBtnDown(void) {
	IFF_STRUCT::sFurniture* pFurniture = ItemManager()->FindFurniture(0x4800601E);
	const char* pText = m_pDays->GetLine(1, false);

	int days = 0;
	int i;
	for (i = 0; i < 5; i++) {
		if (stricmp(sDay[i].m_name, pText) == 0) {
			days = sDay[i].m_days;
			break;
		}
	}

	sStall* pStall = CSharedDoc::Instance()->GetStallItem();

	if (pFurniture->m_dayPrices[i] > CSharedDoc::Instance()->m_myPang) {
		FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		pNotify->SetMessage("\xbc\xd2\xc1\xf6\xc7\xcf\xbd\xc5 \xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9. "
			"\xbd\xbd\xb7\xd4\xc0\xbb \xb1\xb8\xb8\xc5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
		pNotify->Open(0, 3);
	} else if (IsLocalContent((localContentType_t)0x66)) {
		WSendPacket packet(GCT_OFFLINESHOP_REQUEST_BUY_SLOT);
		packet.Encode4(pStall->ownerUID);
		packet.Encode4(0x4800601E);
		packet.Encode1(m_slotNum);
		packet.Encode4(days * 1440);
		packet.Send(SEND_TO_DEFAULT);

		CTaskManager* pTaskManager = CTaskManager::Instance();
		(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	}

	FrForm::OnOK();
}

// offlinetradedlg.cpp:1597
void CBuyingSlotDlg::OnSlotPriceInit(intptr_t param) {
	m_pSlotPrice = DYNAMIC_CAST(FrEdit, param);

	if (m_pSlotPrice) {
		m_pSlotPrice->ClearLine();
		m_pSlotPrice->AddText(MakeStr("%d \xc6\xce", m_price), false, true);
	}
}

// offlinetradedlg.cpp:1657 (run-time type and factory registration of COfflineShopEditDlg)
const WRTTI COfflineShopEditDlg::m_RTTI("COfflineShopEditDlg", &FrForm::m_RTTI);

IObject* COfflineShopEditDlgMakeInstance(void) {
	return new COfflineShopEditDlg;
}

struct __sCOfflineShopEditDlg {
	__sCOfflineShopEditDlg(void) {
		ObjectFactory().AddObjectFunctor(COfflineShopEditDlgMakeInstance, "COfflineShopEditDlg");
	}
};
static __sCOfflineShopEditDlg s_registerCOfflineShopEditDlg;

// offlinetradedlg.cpp:1659
const sFRESH_MSGMAP* COfflineShopEditDlg::GetMessageMap(void) const {
	return &COfflineShopEditDlg::_MsgMap;
}

sFRESH_MSGMAP COfflineShopEditDlg::_MsgMap = { &FrForm::_MsgMap, &COfflineShopEditDlg::_MsgEntries[0] };

sFRESH_ENTRY COfflineShopEditDlg::_MsgEntries[] = {
	// offlinetradedlg.cpp:1663
	{ "areaBg01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnAreaBg01Init },
	// offlinetradedlg.cpp:1664
	{ "areaBg02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnAreaBg02Init },
	// offlinetradedlg.cpp:1665
	{ "areaBg03", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnAreaBg03Init },
	// offlinetradedlg.cpp:1666
	{ "areaBg04", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnAreaBg04Init },
	// offlinetradedlg.cpp:1667
	{ "areaBg05", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnAreaBg05Init },
	// offlinetradedlg.cpp:1670
	{ "edit_slot", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnStallSlotNumInit },
	// offlinetradedlg.cpp:1671
	{ "edit_slotstate", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnSlotNumInit },
	// offlinetradedlg.cpp:1673
	{ "tradeeditdlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTradeEditDlgInit },
	// offlinetradedlg.cpp:1674
	{ "my_item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnMyItemListInit },
	// offlinetradedlg.cpp:1675
	{ "my_item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnMyItemListLClick },
	// offlinetradedlg.cpp:1676
	{ "my_item_list", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnMyItemListRClick },
	// offlinetradedlg.cpp:1677
	{ "my_item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnMyItemListOwnerDraw },
	// offlinetradedlg.cpp:1678
	{ "trade_item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTradeItemListInit },
	// offlinetradedlg.cpp:1679
	{ "trade_item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnTradeItemListLClick },
	// offlinetradedlg.cpp:1681
	{ "trade_item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTradeItemListOwnerDraw },
	// offlinetradedlg.cpp:1682
	{ "visitor", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnVisitorInit },
	// offlinetradedlg.cpp:1683
	{ "userid", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnUserIdInit },
	// offlinetradedlg.cpp:1684
	{ "trade_title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTitleInit },
	// offlinetradedlg.cpp:1685
	{ "trade_title_edit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTitleEditInit },
	// offlinetradedlg.cpp:1686
	{ "trade_title_edit", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&COfflineShopEditDlg::OnTitleEditEnterKey },
	// offlinetradedlg.cpp:1687
	{ "trade_title_edit_bn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTitleEditBnInit },
	// offlinetradedlg.cpp:1688
	{ "trade_title_edit_bn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnTitleEditBnUp },
	// offlinetradedlg.cpp:1689
	{ "trade_myitem_icon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTradeMyItemIconInit },
	// offlinetradedlg.cpp:1690
	{ "trade_myitem_icon", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnTradeMyItemIconOwnerDraw },
	// offlinetradedlg.cpp:1691
	{ "trade_myitem_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTradeMyItemNameInit },
	// offlinetradedlg.cpp:1692
	{ "trade_myitem_ability", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTradeMyItemAbilityInit },
	// offlinetradedlg.cpp:1693
	{ "trade_myitem_number", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTradeMyItemNumberInit },
	// offlinetradedlg.cpp:1694
	{ "trade_myitem_price", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTradeMyItemPriceInit },
	// offlinetradedlg.cpp:1695
	{ "trade_myitem_price", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnTradeMyItemPriceClick },
	// offlinetradedlg.cpp:1696
	{ "trade_myitem_ok_bn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTradeMyItemOkBnInit },
	// offlinetradedlg.cpp:1697
	{ "trade_myitem_ok_bn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnTradeMyItemOkBnUp },
	// offlinetradedlg.cpp:1698
	{ "trade_myitem_cancel_bn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnTradeMyItemCancelBnInit },
	// offlinetradedlg.cpp:1699
	{ "trade_myitem_cancel_bn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnTradeMyItemCancelBnUp },
	// offlinetradedlg.cpp:1700
	{ "open", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnOpenBnInit },
	// offlinetradedlg.cpp:1701
	{ "open", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnOpenBnUp },
	// offlinetradedlg.cpp:1702
	{ "close", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnCloseBnInit },
	// offlinetradedlg.cpp:1703
	{ "close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnCloseBnUp },
	// offlinetradedlg.cpp:1704
	{ "exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnExitBnInit },
	// offlinetradedlg.cpp:1705
	{ "exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopEditDlg::OnExitBnUp },
	// offlinetradedlg.cpp:1707
	{ "tooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnToolTipInit },
	// offlinetradedlg.cpp:1708
	{ "tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopEditDlg::OnToolTipOnwerDraw },
	// offlinetradedlg.cpp:1711
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// offlinetradedlg.cpp:1714
COfflineShopEditDlg::COfflineShopEditDlg(void) {
	m_pAreaBg01 = 0;
	m_pAreaBg02 = 0;
	m_pAreaBg03 = 0;
	m_pAreaBg04 = 0;
	m_pAreaBg05 = 0;
	m_pShopCloseBn = 0;

	m_pStallSlotNum = 0;
	m_pSlotNum = 0;

	m_usedSlotCount = 0;
	m_selectData[0] = 0;
	m_selectData[1] = 0;

	m_pFinishEditDlg = 0;
	m_closing = 0;

	m_buyBtnNormal.pBitmap = g_pFresh->GetBitmap("buy_btn_n");
	m_buyBtnNormal.rect = WRect(120.0f, 30.0f, m_buyBtnNormal.pBitmap->Width(), m_buyBtnNormal.pBitmap->Height());
	m_buyBtnOver.pBitmap = g_pFresh->GetBitmap("buy_btn_o");
	m_buyBtnOver.rect = WRect(120.0f, 30.0f, m_buyBtnOver.pBitmap->Width(), m_buyBtnOver.pBitmap->Height());

	m_delBtnNormal.pBitmap = g_pFresh->GetBitmap("trade_item_del_bn_normal");
	m_delBtnNormal.rect = WRect(120.0f, 30.0f, m_delBtnNormal.pBitmap->Width(), m_delBtnNormal.pBitmap->Height());
	m_delBtnOver.pBitmap = g_pFresh->GetBitmap("trade_item_del_bn_over");
	m_delBtnOver.rect = WRect(120.0f, 30.0f, m_delBtnOver.pBitmap->Width(), m_delBtnOver.pBitmap->Height());
}

// offlinetradedlg.cpp:1748
COfflineShopEditDlg::~COfflineShopEditDlg(void) {
	if (m_closing == 0) {
		if (IsLocalContent((localContentType_t)102)) {
		}
	}
}

// offlinetradedlg.cpp:1770
void COfflineShopEditDlg::OnAreaBg01Init(int param) {
	m_pAreaBg01 = DYNAMIC_CAST(FrArea, param);

	if (m_pAreaBg01) {
		m_pAreaBg01->SetBgImg("rmr_trade_edit_back_01");
	}
}

// offlinetradedlg.cpp:1787
void COfflineShopEditDlg::OnAreaBg02Init(int param) {
	m_pAreaBg02 = DYNAMIC_CAST(FrArea, param);

	if (m_pAreaBg02) {
		m_pAreaBg02->SetBgImg("rmr_trade_edit_back_02");
	}
}

// offlinetradedlg.cpp:1804
void COfflineShopEditDlg::OnAreaBg03Init(int param) {
	m_pAreaBg03 = DYNAMIC_CAST(FrArea, param);

	if (m_pAreaBg03) {
		m_pAreaBg03->SetBgImg("rmr_trade_edit_back_03");
	}
}

// offlinetradedlg.cpp:1822
void COfflineShopEditDlg::OnAreaBg04Init(int param) {
	m_pAreaBg04 = DYNAMIC_CAST(FrArea, param);

	if (m_pAreaBg04) {
		m_pAreaBg04->SetBgImg("rmr_trade_edit_back_04");
	}
}

// offlinetradedlg.cpp:1839
void COfflineShopEditDlg::OnAreaBg05Init(int param) {
	m_pAreaBg05 = DYNAMIC_CAST(FrArea, param);

	if (m_pAreaBg05) {
		m_pAreaBg05->SetVisible(false);
	}
}

// offlinetradedlg.cpp:1855
void COfflineShopEditDlg::OnStallSlotNumInit(int param) {
	m_pStallSlotNum = DYNAMIC_CAST(FrEdit, param);
}

// offlinetradedlg.cpp:1864
void COfflineShopEditDlg::OnSlotNumInit(int param) {
	m_pSlotNum = DYNAMIC_CAST(FrEdit, param);
}

// offlinetradedlg.cpp:1873
void COfflineShopEditDlg::OnTradeItemListInit(int param) {
	m_pShopItemList = DYNAMIC_CAST(FrListBox, param);

	if (IsLocalContent((localContentType_t)0x66) && CSharedDoc::Instance()->GetTradeMode() == 1) {
		sStall* pStall = CSharedDoc::Instance()->GetStallItem();

		for (int i = 0; i < pStall->open; i++) {
			m_pShopItemList->AddItem(&pStall->slots[i]);
		}
	}
}

// offlinetradedlg.cpp:1889
void COfflineShopEditDlg::OnTradeItemListLClick(void) {
	FrListItem* pItem = m_pShopItemList->GetItemUnderCursor();

	if (pItem == 0 || pItem->pData == 0) {
		return;
	}

	pItem->selected = !pItem->selected;
}

// offlinetradedlg.cpp:1932
void COfflineShopEditDlg::OnTradeItemListOwnerDraw(int param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI == 0) {
		return;
	}

	FrListItem* pListItem = (FrListItem*)param;
	if (pListItem == 0) {
		return;
	}

	sStallSlot* pSlot = (sStallSlot*)pListItem->pData;

	if (m_pItemBaseNormalBmp) {
		WRect rect(pListItem->pos.x + 1.0f, pListItem->pos.y + 1.0f, (float)m_pItemBaseNormalBmp->Width(), (float)m_pItemBaseNormalBmp->Height());

		pGDI->DrawTexture(m_pItemBaseNormalBmp, rect, 0xFFFFFFFF, 0);
	}

	if (pSlot->reserved == 1 && pSlot->item.typeId == 0) {
		// "Shop item slot"
		pGDI->Print(WPoint(pListItem->pos.x + 10.0f, pListItem->pos.y + 13.0f), 0, "\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4");
	}

	if (pSlot->reserved == 0 && pSlot->item.typeId == 0) {
		// "Buy a shop item slot"
		pGDI->Print(WPoint(pListItem->pos.x + 10.0f, pListItem->pos.y + 13.0f), 0, "\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4 \xb1\xb8\xb8\xc5  ");
	} else if (pSlot->item.typeId == 0x4800581F || pSlot->item.typeId == 0x4800601E) {
		// "Shop item slot"
		pGDI->Print(WPoint(pListItem->pos.x + 10.0f, pListItem->pos.y + 13.0f), 0, "\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4");
	} else if (pSlot->item.typeId != 0) {
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pSlot->item.typeId);

		if (m_pItemBaseNormalBmp) {
			WRect rect(pListItem->pos.x + 1.0f, pListItem->pos.y + 1.0f, (float)m_pItemBaseNormalBmp->Width(), (float)m_pItemBaseNormalBmp->Height());

			pGDI->DrawTexture(m_pItemBaseNormalBmp, rect, 0xFFFFFFFF, 0);
		}

		char price[32] = { 0 };

		MakePriceStrA(price, pSlot->item.price);

		const char* pPriceText;
		if (pSlot->item.count == 1) {
			pPriceText = MakeStr("%s\xC6\xCE", price);
		} else {
			pPriceText = MakeStr("\xB0\xB3\xB4\xE7%s\xC6\xCE", price);
		}

		pGDI->Print(WPoint(pListItem->pos.x + 10.0f, pListItem->pos.y + 13.0f), 0, "%s", pCommon->m_name);
	}
}

// offlinetradedlg.cpp:2044
void COfflineShopEditDlg::OnVisitorInit(int param) {
	m_pVisitor = DYNAMIC_CAST(FrEdit, param);

	if (IsLocalContent((localContentType_t)0x66) && CSharedDoc::Instance()->GetTradeMode() == 1) {
		m_pVisitor->SetVisible(false);
	}
}

// offlinetradedlg.cpp:2062
void COfflineShopEditDlg::OnOpenBnInit(int param) {
	m_pShopOpenBn = DYNAMIC_CAST(FrButton, param);

	if (m_pShopOpenBn) {
		m_pShopOpenBn->SetVisible(false);
	}
}

// offlinetradedlg.cpp:2079
void COfflineShopEditDlg::OnTradeMyItemOkBnUp(void) {
	if (m_pTradeMyItem == 0) {
		return;
	}

	sTradeItem item = *m_pTradeMyItem;
	int number = atoi(m_pTradeMyItemNumber->GetLine(1, false));

	// The price text keeps its digits and drops the thousands separators;
	// any other character empties it.
	char price[10] = { 0 };
	const char* pText = m_pTradeMyItemPrice->GetLine(1, false);

	char* p = price;
	for (int i = 0; i < 9; i++) {
		if (pText[i] == 0) {
			break;
		}

		if (pText[i] < '0' || pText[i] > '9') {
			if (pText[i] == ',') {
				continue;
			}

			price[0] = 0;
			break;
		}

		*p = pText[i];
		p++;
	}

	__int64 llPrice = atoi(price);

	int stock = item.count;
	for (std::list<sTradeItem>::iterator it = m_tradeItemList.begin(); it != m_tradeItemList.end(); ++it) {
		if (&*it == m_pTradeMyItem) {
			stock = it->count;

			for (std::list<sTradeItem>::iterator mine = m_myItemList.begin(); mine != m_myItemList.end(); ++mine) {
				if (mine->guid == m_pTradeMyItem->guid) {
					stock += mine->count;
					break;
				}
			}
			break;
		}
	}

	if (number < 1 || number > stock) {
		SetTradeResultCode(0x0A, 225, 0);
		return;
	}

	if (llPrice <= 0) {
		SetTradeResultCode(0x0B, 225, 0);
		return;
	}

	if (CSharedDoc::Instance()->GetStallItem()->ownerUID != m_selectData[0]) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		// "The shelf information does not match. Registration failed."
		pForm->SetMessage("\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb\xc0\xc7 \xc1\xa4\xba\xb8\xb0\xa1 "
			"\xc0\xcf\xc4\xa1\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb5\xee\xb7\xcf \xbd\xc7\xc6\xd0\xc0\xd4\xb4\xcf\xb4\xd9.", false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);

		m_selectData[0] = 0;
		m_selectData[1] = 0;

		OnExitBnUp();
		return;
	}

	item.price = llPrice;
	item.count = number;

	if (IsLocalContent((localContentType_t)0x66)) {
		// 0xC7: register an item on the offline shop; packet.h has no enumerator for it.
		WSendPacket packet((enumClientPacket)0xC7);
		packet.Encode4(m_selectData[0]);
		packet.Encode2(1);
		packet.Encode1((unsigned char)m_selectData[1]);
		packet.EncodeBuffer(&item, 0xAA);
		packet.Send(SEND_TO_DEFAULT);

		CTaskManager* pTaskManager = CTaskManager::Instance();
		(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	}
}

// offlinetradedlg.cpp:2260
void COfflineShopEditDlg::SetSelectData(unsigned long first, unsigned long second) {
	m_selectData[0] = first;
	m_selectData[1] = second;
}

// offlinetradedlg.cpp:2266
void COfflineShopEditDlg::OnCloseBnInit(intptr_t param) {
	m_pShopCloseBn = DYNAMIC_CAST(FrButton, param);

	if (m_pShopCloseBn) {
		m_pShopCloseBn->SetVisible(false);
	}
}

// offlinetradedlg.cpp:2298
void COfflineShopEditDlg::OnExitBnUp(void) {
	m_closing = 1;

	COfflineShopDlg* pDlg = CreateForm<COfflineShopDlg>(g_pFresh->GetManager(), this, "offlinetradedlg", 0);

	if (pDlg) {
		pDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
		pDlg->OpenTradeShop(CSharedDoc::Instance()->m_myRoomOwnerUID);
		pDlg->SetListTradeItem(false);
		pDlg->SetFadeout(false);
	}

	OnFreshCancel();
}

// offlinetradedlg.cpp:2317
bool COfflineShopEditDlg::OnInit(void) {
	FrForm::EnableDrag(false);

	SetListEnableTradingMyItem();
	SetListTradingItem();
	SetListMyItem();
	SetListTradeItem();
	SetTradeMyItem(0);
	SetControlsTradeMyItem(0, false);

	return true;
}

// offlinetradedlg.cpp:2336
void COfflineShopEditDlg::InitAllUI(void) {
	FrForm::EnableDrag(false);

	SetListEnableTradingMyItem();
	SetListTradingItem();
	SetListMyItem();
	SetListTradeItem();
	SetTradeMyItem(0);
	SetControlsTradeMyItem(0, false);
}

// offlinetradedlg.cpp:2349
void COfflineShopEditDlg::OnProc(const float dt) {
	FrForm::OnProc(dt);
	CIconToolTip::GetInstance()->ClearToolTip();
}

// offlinetradedlg.cpp:2356
void COfflineShopEditDlg::SetListTradeItem(void) {
	FrTradeEditDlg::SetListTradeItem();

	if (m_pShopItemList) {
		m_pShopItemList->ClearItem();
		m_usedSlotCount = 0;

		sStall* pStall = CSharedDoc::Instance()->GetStallItem();
		if (!pStall) {
			std::list<sStall>::iterator it = CSharedDoc::Instance()->GetListStallItem()->begin();
			pStall = &(*it);
		}
		if (!pStall) {
			return;
		}

		for (int i = 0; i < pStall->open; i++) {
			if (pStall->slots[i].used) {
				m_pShopItemList->AddItem(&pStall->slots[i]);
			}
			if (pStall->slots[i].item.typeId != 0) {
				m_usedSlotCount++;
			}
		}

		if (m_pStallSlotNum) {
			m_pStallSlotNum->ClearLine();
			m_pStallSlotNum->AddLine(MakeStr("%d", pStall->open), 0, false);
		}

		if (m_pSlotNum) {
			m_pSlotNum->ClearLine();
			m_pSlotNum->AddLine(MakeStr("%2d/%2d", m_usedSlotCount, pStall->visible), 0, false);
		}
	}
}

// offlinetradedlg.cpp:2410
void COfflineShopEditDlg::SetSlotData(void) {
	m_usedSlotCount = 0;

	sStall* pStall = CSharedDoc::Instance()->GetStallItem();
	if (!pStall) {
		std::list<sStall>::iterator it = CSharedDoc::Instance()->GetListStallItem()->begin();
		pStall = &(*it);
	}
	if (!pStall) {
		return;
	}

	for (int i = 0; i < pStall->open; i++) {
		if (pStall->slots[i].item.typeId != 0) {
			m_usedSlotCount++;
		}
	}

	if (m_pStallSlotNum) {
		m_pStallSlotNum->ClearLine();
		m_pStallSlotNum->AddLine(MakeStr("%d", pStall->open), 0, false);
	}

	if (m_pSlotNum) {
		m_pSlotNum->ClearLine();
		m_pSlotNum->AddLine(MakeStr("%2d/%2d", m_usedSlotCount, pStall->visible), 0, false);
	}
}

// offlinetradedlg.cpp:2448
bool COfflineShopEditDlg::IsCheckCardPats(unsigned long partsId, unsigned long partsTypeId) {
	std::list<sSCardAvilityPeriodInfo>& cards = CSharedDoc::Instance()->m_cardPeriodList[0];
	std::list<sSCardAvilityPeriodInfo>::iterator it;
	for (it = cards.begin(); it != cards.end(); ++it) {
		if (it->partsTypeId == partsTypeId && it->partsId == partsId) {
			break;
		}
	}

	return it == cards.end() ? true : false;
}

// offlinetradedlg.cpp:2468
bool COfflineShopEditDlg::OnOfflineEquitCancelDlgResult(int ret, FrForm* pForm) {
	if (ret == 1) {
		FrListItem* pItem = m_pShopItemList->GetItemUnderCursor();

		if (pItem) {
			if (IsLocalContent((localContentType_t)0x66)) {
				WSendPacket packet(GCT_OFFLINESHOP_REQUEST_ITEM_OFF);
				packet.Encode4(CSharedDoc::Instance()->GetStallItem()->ownerUID);
				packet.Encode2(1);
				packet.Encode1(pItem->idx + 1);
				packet.Send(SEND_TO_DEFAULT);

				CTaskManager* pTaskManager = CTaskManager::Instance();
				(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
			}
		}
	}

	return true;
}

// offlinetradedlg.cpp:2493
bool COfflineShopEditDlg::OnOfflineTradeItemOnDlgResult(int ret, FrForm* pForm) {
	if (ret == 1) {
		if (m_pFinishEditDlg) {
			int index;
			int used;
			m_pFinishEditDlg->GetSelectSlotInfo(index, used);

			sTradeItem item = *m_pTradeMyItem;
			int number = atoi(m_pTradeMyItemNumber->GetLine(1, false));

			char price[10] = { 0 };
			const char* pText = m_pTradeMyItemPrice->GetLine(1, false);

			char* p = price;
			for (int i = 0; i < 9; i++) {
				if (pText[i] == 0) {
					break;
				}

				if (pText[i] < '0' || pText[i] > '9') {
					if (pText[i] == ',') {
						continue;
					}

					price[0] = 0;
					break;
				}

				*p = pText[i];
				p++;
			}

			__int64 llPrice = atoi(price);

			item.price = llPrice;
			item.count = number;

			if (IsLocalContent((localContentType_t)0x66)) {
				// 0xC7: register an item on the offline shop; packet.h has no enumerator for it.
				WSendPacket packet((enumClientPacket)0xC7);
				packet.Encode4(index);
				packet.Encode2(1);
				packet.Encode1(used);
				packet.EncodeBuffer(&item, 0xAA);
				packet.Send(SEND_TO_DEFAULT);

				CTaskManager* pTaskManager = CTaskManager::Instance();
				(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
			}
		}
	}

	m_pFinishEditDlg = 0;
	return true;
}

// offlinetradedlg.cpp:2573
void COfflineShopEditDlg::OnToolTipInit(intptr_t param) {
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

// offlinetradedlg.cpp:2579
void COfflineShopEditDlg::OnToolTipOnwerDraw(int) {
	CIconToolTip::GetInstance()->Render();
}

// offlinetradedlg.cpp:2604
void COfflineShopEditDlg::DelListMyItem(sTradeItem* pItem, unsigned long, unsigned char) {
	if (pItem->typeId == 0) {
		SetListMyItem();
	} else {
		FrTradeEditDlg::DelListMyItem(pItem);
	}

	SetTradeMyItem(0);
	SetControlsTradeMyItem(0, false);

	SetListTradeItem();

	if (m_pMyItemList) {
		m_pMyItemList->UnselectAllItem();
	}
}

// offlinetradedlg.cpp:2641
bool COfflineShopEditDlg::OnOfflineTradeBuyDlgResult(int ret, FrForm* pForm) {
	if (ret == 1) {
		FrOfflineTradeFinishEditDlg* pDlg = CreateForm<FrOfflineTradeFinishEditDlg>(g_pFresh->GetManager(), this, "offlinetradefinisheditdlg", 0);

		if (pDlg) {
			pDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
			pDlg->SetFadeout(false);
			pDlg->SetListTradeItem(&m_tradeItemList);
			pDlg->SetTradeUserId();
			pDlg->SetTradeTitle(m_tradeTitle);
		}
	}

	return true;
}

// offlinetradedlg.cpp:2666 (run-time type and factory registration of COfflineShopDlg)
const WRTTI COfflineShopDlg::m_RTTI("COfflineShopDlg", &FrForm::m_RTTI);

IObject* COfflineShopDlgMakeInstance(void) {
	return new COfflineShopDlg;
}

struct __sCOfflineShopDlg {
	__sCOfflineShopDlg(void) {
		ObjectFactory().AddObjectFunctor(COfflineShopDlgMakeInstance, "COfflineShopDlg");
	}
};
static __sCOfflineShopDlg s_registerCOfflineShopDlg;

// offlinetradedlg.cpp:2668
const sFRESH_MSGMAP* COfflineShopDlg::GetMessageMap(void) const {
	return &COfflineShopDlg::_MsgMap;
}

sFRESH_MSGMAP COfflineShopDlg::_MsgMap = { &FrForm::_MsgMap, &COfflineShopDlg::_MsgEntries[0] };

sFRESH_ENTRY COfflineShopDlg::_MsgEntries[] = {
	// offlinetradedlg.cpp:2670
	{ "tradedlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnTradeDlgInit },
	// offlinetradedlg.cpp:2671
	{ "visitor_back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnVisitorBackInit },
	// offlinetradedlg.cpp:2672
	{ "visitor_back", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&COfflineShopDlg::OnVisitorBackOwnerDraw },
	// offlinetradedlg.cpp:2673
	{ "mypang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnMyPangInit },
	// offlinetradedlg.cpp:2674
	{ "mypang", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&COfflineShopDlg::OnMyPangOwnerDraw },
	// offlinetradedlg.cpp:2675
	{ "trade_item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnTradeItemListInit },
	// offlinetradedlg.cpp:2676
	{ "trade_item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&COfflineShopDlg::OnTradeItemListLClick },
	// offlinetradedlg.cpp:2677
	{ "trade_item_list", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&COfflineShopDlg::OnTradeItemListRClick },
	// offlinetradedlg.cpp:2678
	{ "trade_item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnTradeItemListOwnerDraw },
	// offlinetradedlg.cpp:2679
	{ "noitem01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnNoItem01Init },
	// offlinetradedlg.cpp:2680
	{ "noitem02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnNoItem02Init },
	// offlinetradedlg.cpp:2681
	{ "visitor", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnVisitorInit },
	// offlinetradedlg.cpp:2682
	{ "userid", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnUserIdInit },
	// offlinetradedlg.cpp:2683
	{ "trade_title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnTitleInit },
	// offlinetradedlg.cpp:2684
	{ "edit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnEditBnInit },
	// offlinetradedlg.cpp:2685
	{ "edit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopDlg::OnEditBnUp },
	// offlinetradedlg.cpp:2686
	{ "close", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnCloseBnInit },
	// offlinetradedlg.cpp:2687
	{ "close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopDlg::OnCloseBnUp },
	// offlinetradedlg.cpp:2688
	{ "hide", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnHideBnInit },
	// offlinetradedlg.cpp:2689
	{ "hide", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopDlg::OnHideBnUp },
	// offlinetradedlg.cpp:2690
	{ "exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnExitBnInit },
	// offlinetradedlg.cpp:2691
	{ "exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopDlg::OnExitBnUp },
	// offlinetradedlg.cpp:2693
	{ "tooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnToolTipInit },
	// offlinetradedlg.cpp:2694
	{ "tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopDlg::OnToolTipOnwerDraw },
	// offlinetradedlg.cpp:2697
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// offlinetradedlg.cpp:2700
COfflineShopDlg::COfflineShopDlg(void) {
	m_pVisitor = 0;
	m_closing = 0;
	m_pSelectedSlotItem = 0;

	m_extendBtnNormal.pBitmap = g_pFresh->GetBitmap("rmr_extend_btn_n");
	m_extendBtnNormal.rect = WRect(96.0f, 66.0f, m_extendBtnNormal.pBitmap->Width(), m_extendBtnNormal.pBitmap->Height());
	m_extendBtnNormal.state = -1;
	m_extendBtnOver.pBitmap = g_pFresh->GetBitmap("rmr_extend_btn_o");
	m_extendBtnOver.rect = WRect(96.0f, 66.0f, m_extendBtnOver.pBitmap->Width(), m_extendBtnOver.pBitmap->Height());
	m_extendBtnOver.state = -1;

	m_returnBtnNormal.pBitmap = g_pFresh->GetBitmap("rmr_return_n");
	m_returnBtnNormal.rect = WRect(144.0f, 66.0f, m_returnBtnNormal.pBitmap->Width(), m_returnBtnNormal.pBitmap->Height());
	m_returnBtnNormal.state = -1;
	m_returnBtnOver.pBitmap = g_pFresh->GetBitmap("rmr_return_o");
	m_returnBtnOver.rect = WRect(144.0f, 66.0f, m_returnBtnOver.pBitmap->Width(), m_returnBtnOver.pBitmap->Height());
	m_returnBtnOver.state = -1;

	m_deleteBtnNormal.pBitmap = g_pFresh->GetBitmap("delete_btn_n");
	m_deleteBtnNormal.rect = WRect(144.0f, 66.0f, m_deleteBtnNormal.pBitmap->Width(), m_deleteBtnNormal.pBitmap->Height());
	m_deleteBtnNormal.state = -1;
	m_deleteBtnOver.pBitmap = g_pFresh->GetBitmap("delete_btn_o");
	m_deleteBtnOver.rect = WRect(144.0f, 66.0f, m_deleteBtnOver.pBitmap->Width(), m_deleteBtnOver.pBitmap->Height());
	m_deleteBtnOver.state = -1;

	m_buyBtnNormal.pBitmap = g_pFresh->GetBitmap("buy_btn_n");
	m_buyBtnNormal.rect = WRect(144.0f, 66.0f, m_buyBtnNormal.pBitmap->Width(), m_buyBtnNormal.pBitmap->Height());
	m_buyBtnNormal.state = -1;
	m_buyBtnOver.pBitmap = g_pFresh->GetBitmap("buy_btn_o");
	m_buyBtnOver.rect = WRect(144.0f, 66.0f, m_buyBtnOver.pBitmap->Width(), m_buyBtnOver.pBitmap->Height());
	m_buyBtnOver.state = -1;

	m_delete.pBitmap = g_pFresh->GetBitmap("delete");
	m_delete.rect = WRect(144.0f, 35.0f, m_delete.pBitmap->Width(), m_delete.pBitmap->Height());
	m_tradeState1.pBitmap = g_pFresh->GetBitmap("rmr_trade_state1");
	m_tradeState1.rect = WRect(70.0f, 35.0f, m_tradeState1.pBitmap->Width(), m_tradeState1.pBitmap->Height());
	m_tradeState2.pBitmap = g_pFresh->GetBitmap("rmr_trade_state2");
	m_tradeState2.rect = WRect(70.0f, 35.0f, m_tradeState2.pBitmap->Width(), m_tradeState2.pBitmap->Height());
	m_tradeState3.pBitmap = g_pFresh->GetBitmap("rmr_trade_state3");
	m_tradeState3.rect = WRect(70.0f, 35.0f, m_tradeState3.pBitmap->Width(), m_tradeState3.pBitmap->Height());

	m_uploadBtnNormal.pBitmap = g_pFresh->GetBitmap("rmr_item_upload_n");
	m_uploadBtnNormal.rect = WRect(144.0f, 66.0f, m_uploadBtnNormal.pBitmap->Width(), m_uploadBtnNormal.pBitmap->Height());
	m_uploadBtnNormal.state = -1;
	m_uploadBtnOver.pBitmap = g_pFresh->GetBitmap("rmr_item_upload_o");
	m_uploadBtnOver.rect = WRect(144.0f, 66.0f, m_uploadBtnOver.pBitmap->Width(), m_uploadBtnOver.pBitmap->Height());
	m_uploadBtnOver.state = -1;
}

// offlinetradedlg.cpp:2762
COfflineShopDlg::~COfflineShopDlg(void) {
	if (m_closing == 0) {
		if (IsLocalContent((localContentType_t)102)) {
		}
	}
}

// offlinetradedlg.cpp:2802
void COfflineShopDlg::OnVisitorBackOwnerDraw(void) {
}

// offlinetradedlg.cpp:2807
void COfflineShopDlg::OnVisitorInit(int param) {
	m_pVisitor = DYNAMIC_CAST(FrEdit, param);

	if (IsLocalContent((localContentType_t)0x66) && CSharedDoc::Instance()->GetTradeMode() == 1) {
		m_pVisitor->SetVisible(false);
	}
}

// offlinetradedlg.cpp:2826
void COfflineShopDlg::OnCloseBnUp(void) {
	m_closing = 1;

	if (IsLocalContent((localContentType_t)0x66)) {
		CTaskManager* pTaskManager = CTaskManager::Instance();
		(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 0x22F, 0, 0, 0, 0, 0));
	}

	FrForm::OnOK();
}

// offlinetradedlg.cpp:2847
void COfflineShopDlg::OnVisitorBackInit(int param) {
	m_pVisitorBack = DYNAMIC_CAST(FrArea, param);

	if (m_pVisitorBack) {
		m_pVisitorBack->SetVisible(false);
	}
}

// offlinetradedlg.cpp:2857
void COfflineShopDlg::OnTradeItemListOwnerDraw(int param) {
	FrTradeDlg::OnTradeItemListOwnerDraw(param);

	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (!pGDI) {
		return;
	}

	const WPoint& mousePos = pManager->GetMousePos();

	FrListItem* pItem = (FrListItem*)param;
	if (!pItem || !pItem->pData) {
		return;
	}

	const Bitmap* pSlotBitmap = g_pFresh->GetBitmap("rmr_item_slot");

	sStallSlot* pSlot = (sStallSlot*)pItem->pData;

	pGDI->SetTextStyle(0);
	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	if (pSlot->reserved == 0) {
		if (pSlotBitmap) {
			pGDI->DrawTexture(pSlotBitmap, WRect(pItem->pos.x, pItem->pos.y, (float)pSlotBitmap->Width(), (float)pSlotBitmap->Height()), 0xFFFFFFFF, 0);
		}
		// "buy a shop item slot"
		pGDI->Print(pItem->pos + WPoint(182.0f, 7.0f), 2, "\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4 \xb1\xb8\xb8\xc5");

		if (pItem->underCursor && (m_buyBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
			pGDI->DrawTexture(m_buyBtnOver.pBitmap, m_buyBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x16);
		} else {
			WRect rect;
			rect = m_buyBtnNormal.rect + pItem->pos;
			pGDI->DrawTexture(m_buyBtnNormal.pBitmap, rect, 0xFFFFFFFF, 0);
		}
	} else {
		__int64 hours = 0;

		if (pSlot->sold == 0) {
			FILETIME slotTime;
			FILETIME serverTime;
			SystemTimeToFileTime(&pSlot->expireTime, &slotTime);
			SystemTimeToFileTime(&CSharedDoc::Instance()->GetServerTime(), &serverTime);

			__int64 diff = ((__int64)slotTime.dwHighDateTime - serverTime.dwHighDateTime) * 0x100000000 - serverTime.dwLowDateTime + slotTime.dwLowDateTime;
			hours = diff / 60 / 60 / 10000000;

			pGDI->DrawTexture(m_tradeState3.pBitmap, m_tradeState3.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);

			if (hours >= 0 && diff >= 0) {
				pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
				if (hours / 24 >= 1) {
					if (hours % 24 != 0) {
						// "%d days"
						pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "%d\xc0\xcf", hours / 24 + 1);
					} else {
						pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "%d\xc0\xcf", hours / 24);
					}
				} else {
					// "%d hours"
					pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "%d\xbd\xc3", hours);
				}
			} else {
				pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
				// "expired"
				pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "\xb1\xe2\xb0\xa3\xb8\xb8\xb7\xe1");
			}
			pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

			if (pItem->underCursor && (m_extendBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
				pGDI->DrawTexture(m_extendBtnOver.pBitmap, m_extendBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x43);
			} else {
				pGDI->DrawTexture(m_extendBtnNormal.pBitmap, m_extendBtnNormal.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}

		if (pSlot->item.typeId == 0) {
			if (pSlotBitmap) {
				pGDI->DrawTexture(pSlotBitmap, WRect(pItem->pos.x, pItem->pos.y, (float)pSlotBitmap->Width(), (float)pSlotBitmap->Height()), 0xFFFFFFFF, 0);
			}
			// "shop item slot"
			pGDI->Print(pItem->pos + WPoint(182.0f, 7.0f), 2, "\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4");

			if (pItem->underCursor && (m_uploadBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
				pGDI->DrawTexture(m_uploadBtnOver.pBitmap, m_uploadBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x44);
			} else {
				pGDI->DrawTexture(m_uploadBtnNormal.pBitmap, m_uploadBtnNormal.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		} else if (pSlot->sold == 0 && hours < 0) {
			if (pItem->underCursor && (m_returnBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
				pGDI->DrawTexture(m_returnBtnOver.pBitmap, m_returnBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x45);
			} else {
				pGDI->DrawTexture(m_returnBtnNormal.pBitmap, m_returnBtnNormal.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		} else {
			if (pItem->underCursor && (m_deleteBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
				pGDI->DrawTexture(m_deleteBtnOver.pBitmap, m_deleteBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x42);
			} else {
				pGDI->DrawTexture(m_deleteBtnNormal.pBitmap, m_deleteBtnNormal.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			}
		}
	}
}

// offlinetradedlg.cpp:3021
void COfflineShopDlg::OnToolTipInit(intptr_t param) {
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

// offlinetradedlg.cpp:3027
void COfflineShopDlg::OnToolTipOnwerDraw(int) {
	CIconToolTip::GetInstance()->Render();
}

// offlinetradedlg.cpp:3054
void COfflineShopDlg::OnTradeItemListLClick(void) {
	FrListItem* pItem = m_pTradeItemList->GetItemUnderCursor();
	if (pItem == 0 || pItem->pData == 0) {
		return;
	}

	FrWndManager* pManager = g_pFresh->GetManager();
	const WPoint& mousePos = pManager->GetMousePos();
	sStallSlot* pSlot = (sStallSlot*)pItem->pData;

	if (pSlot->reserved == 0
		&& ((m_buyBtnNormal.rect + pItem->pos).IsInRect(mousePos)
		|| (m_buyBtnOver.rect + pItem->pos).IsInRect(mousePos))) {
		CBuyingSlotDlg* pDlg = CreateForm<CBuyingSlotDlg>(pManager, this, "buyslotdlg", 0);

		pDlg->SetSlotNum(pItem->idx + 1);
		pDlg->Open((FRESH_RESULT_CALLBACK)0, 1);
		return;
	}

	if (pSlot->reserved == 1 && pSlot->item.typeId == 0
		&& ((m_uploadBtnNormal.rect + pItem->pos).IsInRect(mousePos)
		|| (m_uploadBtnOver.rect + pItem->pos).IsInRect(mousePos))) {
		m_pSelectedSlotItem = pItem;
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno", 0);

		if (pForm == 0) {
			return;
		}

		pForm->Open((FRESH_RESULT_CALLBACK)&COfflineShopDlg::OnOfflineShopEditDlgCreateResult, 0xC0);
		pForm->SetFadeout(false);
		// "Do you want to register the item?"
		pForm->SetMessage("\xbe\xc6\xc0\xcc\xc5\xdb \xb5\xee\xb7\xcf\xc0\xbb \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?", false);
		return;
	}

	if (pSlot->reserved == 1
		&& ((m_extendBtnNormal.rect + pItem->pos).IsInRect(mousePos)
		|| (m_extendBtnOver.rect + pItem->pos).IsInRect(mousePos))) {
		CBuyingSlotDlg* pDlg = CreateForm<CBuyingSlotDlg>(pManager, this, "buyslotdlg", 0);

		pDlg->SetSlotNum(pItem->idx + 1);
		pDlg->Open((FRESH_RESULT_CALLBACK)0, 1);
		return;
	}

	if (pSlot->reserved == 1
		&& ((m_returnBtnNormal.rect + pItem->pos).IsInRect(mousePos)
		|| (m_returnBtnOver.rect + pItem->pos).IsInRect(mousePos)
		|| (m_deleteBtnNormal.rect + pItem->pos).IsInRect(mousePos)
		|| (m_deleteBtnOver.rect + pItem->pos).IsInRect(mousePos))) {
		if (IsLocalContent((localContentType_t)0x66)) {
			WSendPacket packet(GCT_OFFLINESHOP_REQUEST_ITEM_OFF);
			packet.Encode4(CSharedDoc::Instance()->GetStallItem()->ownerUID);
			packet.Encode2(1);
			packet.Encode1(pItem->idx + 1);
			packet.Send(SEND_TO_DEFAULT);

			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
		}
		return;
	}

	pItem->selected = !pItem->selected;
}

// offlinetradedlg.cpp:3137
bool COfflineShopDlg::OnOfflineShopEditDlgCreateResult(int ret, FrForm* pForm) {
	if (ret == 1) {
		if (m_pSelectedSlotItem && m_pSelectedSlotItem->pData) {
			sStallSlot* pSlot = (sStallSlot*)m_pSelectedSlotItem->pData;

			m_closing = 1;
			COfflineShopEditDlg* pDlg = CreateForm<COfflineShopEditDlg>(g_pFresh->GetManager(), this, "tradeeditdlg", 0);

			if (pDlg) {
				pDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
				pDlg->SetSelectData(pSlot->index, pSlot->used);
				pDlg->SetFadeout(false);

				m_pSelectedSlotItem = 0;
				OnFreshOkay();
			}
		} else {
			FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pNotify->SetMessage("\xbc\xb1\xc5\xc3\xc7\xd1 \xbd\xbd\xb7\xd4\xc0\xc7 \xc1\xa4\xba\xb8\xb0\xa1 "
				"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xc6\xed\xc1\xfd \xbd\xc7\xc6\xd0\xc0\xd4\xb4\xcf\xb4\xd9.", false);
			pNotify->Open(0, 3);
		}
	} else {
		m_pSelectedSlotItem = 0;
	}

	return true;
}

// offlinetradedlg.cpp:3174
void COfflineShopDlg::OnProc(const float dt) {
	FrForm::OnProc(dt);
	CIconToolTip::GetInstance()->ClearToolTip();
}

// offlinetradedlg.cpp:3181
void COfflineShopDlg::SetListTradeItem(bool) {
	FrTradeDlg::SetListTradeItem(false);

	m_pTradeItemList->UseRightButton(false);

	sStall* pStall = CSharedDoc::Instance()->GetStallItem();

	m_pTradeItemList->ClearItem();

	for (int i = 0; i < pStall->open; i++) {
		m_pTradeItemList->AddItem(&pStall->slots[i]);
	}
}

// offlinetradedlg.cpp:3204 (run-time type and factory registration of COfflineShopVisitorDlg)
const WRTTI COfflineShopVisitorDlg::m_RTTI("COfflineShopVisitorDlg", &FrForm::m_RTTI);

IObject* COfflineShopVisitorDlgMakeInstance(void) {
	return new COfflineShopVisitorDlg;
}

struct __sCOfflineShopVisitorDlg {
	__sCOfflineShopVisitorDlg(void) {
		ObjectFactory().AddObjectFunctor(COfflineShopVisitorDlgMakeInstance, "COfflineShopVisitorDlg");
	}
};
static __sCOfflineShopVisitorDlg s_registerCOfflineShopVisitorDlg;

// offlinetradedlg.cpp:3206
const sFRESH_MSGMAP* COfflineShopVisitorDlg::GetMessageMap(void) const {
	return &COfflineShopVisitorDlg::_MsgMap;
}

sFRESH_MSGMAP COfflineShopVisitorDlg::_MsgMap = { &FrForm::_MsgMap, &COfflineShopVisitorDlg::_MsgEntries[0] };

sFRESH_ENTRY COfflineShopVisitorDlg::_MsgEntries[] = {
	// offlinetradedlg.cpp:3208
	{ "tradedlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnTradeDlgInit },
	// offlinetradedlg.cpp:3209
	{ "visitor_back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnVisitorBackInit },
	// offlinetradedlg.cpp:3210
	{ "visitor_back", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&COfflineShopVisitorDlg::OnVisitorBackOwnerDraw },
	// offlinetradedlg.cpp:3211
	{ "mypang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnMyPangInit },
	// offlinetradedlg.cpp:3212
	{ "mypang", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&COfflineShopVisitorDlg::OnMyPangOwnerDraw },
	// offlinetradedlg.cpp:3213
	{ "trade_item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnTradeItemListInit },
	// offlinetradedlg.cpp:3214
	{ "trade_item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&COfflineShopVisitorDlg::OnTradeItemListLClick },
	// offlinetradedlg.cpp:3215
	{ "trade_item_list", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&COfflineShopVisitorDlg::OnTradeItemListRClick },
	// offlinetradedlg.cpp:3216
	{ "trade_item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnTradeItemListOwnerDraw },
	// offlinetradedlg.cpp:3217
	{ "noitem01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnNoItem01Init },
	// offlinetradedlg.cpp:3218
	{ "noitem02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnNoItem02Init },
	// offlinetradedlg.cpp:3219
	{ "visitor", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnVisitorInit },
	// offlinetradedlg.cpp:3220
	{ "userid", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnUserIdInit },
	// offlinetradedlg.cpp:3221
	{ "trade_title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnTitleInit },
	// offlinetradedlg.cpp:3222
	{ "edit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnEditBnInit },
	// offlinetradedlg.cpp:3223
	{ "edit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopVisitorDlg::OnEditBnUp },
	// offlinetradedlg.cpp:3224
	{ "close", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnCloseBnInit },
	// offlinetradedlg.cpp:3225
	{ "close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopVisitorDlg::OnCloseBnUp },
	// offlinetradedlg.cpp:3226
	{ "hide", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnHideBnInit },
	// offlinetradedlg.cpp:3227
	{ "hide", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopVisitorDlg::OnHideBnUp },
	// offlinetradedlg.cpp:3228
	{ "exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnExitBnInit },
	// offlinetradedlg.cpp:3229
	{ "exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineShopVisitorDlg::OnExitBnUp },
	// offlinetradedlg.cpp:3231
	{ "tooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnToolTipInit },
	// offlinetradedlg.cpp:3232
	{ "tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineShopVisitorDlg::OnToolTipOnwerDraw },
	// offlinetradedlg.cpp:3235
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// offlinetradedlg.cpp:3238
COfflineShopVisitorDlg::COfflineShopVisitorDlg(void) {
	m_pVisitor = 0;

	m_buyBtnNormal.pBitmap = g_pFresh->GetBitmap("buy_btn_n");
	m_buyBtnNormal.rect = WRect(144.0f, 66.0f, m_buyBtnNormal.pBitmap->Width(), m_buyBtnNormal.pBitmap->Height());
	m_buyBtnNormal.state = -1;
	m_buyBtnOver.pBitmap = g_pFresh->GetBitmap("buy_btn_o");
	m_buyBtnOver.rect = WRect(144.0f, 66.0f, m_buyBtnOver.pBitmap->Width(), m_buyBtnOver.pBitmap->Height());
	m_buyBtnOver.state = -1;

	m_tradeState1.pBitmap = g_pFresh->GetBitmap("rmr_trade_state1");
	m_tradeState1.rect = WRect(70.0f, 35.0f, m_tradeState1.pBitmap->Width(), m_tradeState1.pBitmap->Height());
	m_tradeState2.pBitmap = g_pFresh->GetBitmap("rmr_trade_state2");
	m_tradeState2.rect = WRect(70.0f, 35.0f, m_tradeState2.pBitmap->Width(), m_tradeState2.pBitmap->Height());
	m_tradeState3.pBitmap = g_pFresh->GetBitmap("rmr_trade_state3");
	m_tradeState3.rect = WRect(70.0f, 35.0f, m_tradeState3.pBitmap->Width(), m_tradeState3.pBitmap->Height());
}

// offlinetradedlg.cpp:3299
COfflineShopVisitorDlg::~COfflineShopVisitorDlg(void) {
}

// offlinetradedlg.cpp:3304
void COfflineShopVisitorDlg::OnEditBnUp(void) {
}

// offlinetradedlg.cpp:3320
void COfflineShopVisitorDlg::OnVisitorBackOwnerDraw(void) {
}

// offlinetradedlg.cpp:3326
void COfflineShopVisitorDlg::OnCloseBnUp(void) {
	FrTradeDlg::OnOK();
}

// offlinetradedlg.cpp:3341
void COfflineShopVisitorDlg::OnVisitorInit(int param) {
	m_pVisitor = DYNAMIC_CAST(FrEdit, param);

	if (IsLocalContent((localContentType_t)0x66) && CSharedDoc::Instance()->GetTradeMode() == 1) {
		m_pVisitor->SetVisible(false);
	}
}

// offlinetradedlg.cpp:3360
void COfflineShopVisitorDlg::OnVisitorBackInit(int param) {
	m_pVisitorBack = DYNAMIC_CAST(FrArea, param);

	if (m_pVisitorBack) {
		m_pVisitorBack->SetVisible(false);
	}
}

// offlinetradedlg.cpp:3370
void COfflineShopVisitorDlg::OnTradeItemListOwnerDraw(int param) {
	FrTradeDlg::OnTradeItemListOwnerDraw(param);

	FrWndManager* pManager = g_pFresh->GetManager();
	FrGraphicInterface* pGDI = pManager->GetGDI();

	if (!pGDI) {
		return;
	}

	const WPoint& mousePos = pManager->GetMousePos();

	FrListItem* pItem = (FrListItem*)param;
	if (!pItem || !pItem->pData) {
		return;
	}

	sStallSlot* pSlot = (sStallSlot*)pItem->pData;

	const Bitmap* pSlotBitmap = g_pFresh->GetBitmap("rmr_item_slot");

	if (pSlot->reserved != 1 || pSlot->item.typeId == 0) {
		return;
	}

	FILETIME slotTime;
	FILETIME serverTime;
	SystemTimeToFileTime(&pSlot->expireTime, &slotTime);
	SystemTimeToFileTime(&CSharedDoc::Instance()->GetServerTime(), &serverTime);

	__int64 hours = (((__int64)slotTime.dwHighDateTime - serverTime.dwHighDateTime) * 0x100000000 - serverTime.dwLowDateTime + slotTime.dwLowDateTime) / 60 / 60 / 10000000;

	if (hours >= 0) {
		if (pSlot->sold == 0) {
			pGDI->DrawTexture(m_tradeState3.pBitmap, m_tradeState3.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);

			pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
			if (hours / 24 > 1) {
				if (hours % 24 != 0) {
					// "%d days"
					pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "%d\xc0\xcf", hours / 24 + 1);
				} else {
					pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "%d\xc0\xcf", hours / 24);
				}
			} else {
				// "%d hours"
				pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "%d\xbd\xc3", hours);
			}
		}

		if (pItem->underCursor && (m_buyBtnNormal.rect + pItem->pos).IsInRect(mousePos)) {
			pGDI->DrawTexture(m_buyBtnOver.pBitmap, m_buyBtnOver.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)0x16);
		} else {
			pGDI->DrawTexture(m_buyBtnNormal.pBitmap, m_buyBtnNormal.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);
		}
	} else {
		pGDI->DrawTexture(m_tradeState3.pBitmap, m_tradeState3.rect + pItem->pos + WPoint(0.0f, 0.0f), 0xFFFFFFFF, 0);

		pGDI->SetTextColor(0xFFFFFFFF, 0xFFFFFFFF);
		// "cannot buy"
		pGDI->Print(m_tradeState3.rect.TopLeft() + pItem->pos + WPoint(55.0f, 6.0f), 2, "\xb1\xb8\xb8\xc5 \xba\xd2\xb0\xa1");
	}
}

// offlinetradedlg.cpp:3466
void COfflineShopVisitorDlg::OnTradeItemListLClick(void) {
	FrListItem* pItem = m_pTradeItemList->GetItemUnderCursor();
	if (pItem == 0) {
		return;
	}

	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();
	sStallSlot* pSlot = (sStallSlot*)pItem->pData;

	FILETIME slotTime;
	FILETIME serverTime;
	SystemTimeToFileTime(&pSlot->expireTime, &slotTime);
	SystemTimeToFileTime(&CSharedDoc::Instance()->GetServerTime(), &serverTime);

	if (pSlot->reserved == 1
		&& ((((__int64)slotTime.dwHighDateTime - serverTime.dwHighDateTime) * 0x100000000 - serverTime.dwLowDateTime + slotTime.dwLowDateTime) / 60 / 60 / 10000000 >= 0
		|| pSlot->sold != 0)
		&& ((m_buyBtnNormal.rect + pItem->pos).IsInRect(mousePos)
		|| (m_buyBtnOver.rect + pItem->pos).IsInRect(mousePos))) {
		COfflineBuyDlg* pDlg = CreateForm<COfflineBuyDlg>(g_pFresh->GetManager(), this, "tradebuydlg", 0);

		if (pDlg == 0) {
			return;
		}

		pDlg->Open((FRESH_RESULT_CALLBACK)&FrTradeDlg::OnTradeBuyDlgResult, 0x40);
		pDlg->SetFadeout(false);
		sTradeItem* pTradeItem = &pSlot->item;
		pDlg->SetBuyItem(pTradeItem);
		pDlg->SetBuyitemData(*pSlot);
		pDlg->SetControlsBuyItem(pTradeItem);
		return;
	}

	pItem->selected = !pItem->selected;
}

// offlinetradedlg.cpp:3517
void COfflineShopVisitorDlg::OnToolTipInit(intptr_t param) {
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

// offlinetradedlg.cpp:3523
void COfflineShopVisitorDlg::OnToolTipOnwerDraw(int) {
	CIconToolTip::GetInstance()->Render();
}

// offlinetradedlg.cpp:3548
bool COfflineShopVisitorDlg::OnOfflineShopEditDlgCreateResult(int, FrForm *) {
	return true;
}

// offlinetradedlg.cpp:3554
void COfflineShopVisitorDlg::OnProc(const float dt) {
	FrForm::OnProc(dt);
	CIconToolTip::GetInstance()->ClearToolTip();
}

// offlinetradedlg.cpp:3561
void COfflineShopVisitorDlg::SetListTradeItem(bool) {
	FrTradeDlg::SetListTradeItem(false);

	sStall* pStall = CSharedDoc::Instance()->GetStallItem();

	m_pTradeItemList->UseRightButton(false);
	m_pTradeItemList->ClearItem();

	for (int i = 0; i < pStall->open; i++) {
		if (pStall->slots[i].reserved == 1 && pStall->slots[i].item.typeId != 0) {
			m_pTradeItemList->AddItem(&pStall->slots[i]);
		}
	}
}

// offlinetradedlg.cpp:3584 (run-time type and factory registration of COfflineBuyDlg)
const WRTTI COfflineBuyDlg::m_RTTI("COfflineBuyDlg", &FrForm::m_RTTI);

IObject* COfflineBuyDlgMakeInstance(void) {
	return new COfflineBuyDlg;
}

struct __sCOfflineBuyDlg {
	__sCOfflineBuyDlg(void) {
		ObjectFactory().AddObjectFunctor(COfflineBuyDlgMakeInstance, "COfflineBuyDlg");
	}
};
static __sCOfflineBuyDlg s_registerCOfflineBuyDlg;

// offlinetradedlg.cpp:3586
const sFRESH_MSGMAP* COfflineBuyDlg::GetMessageMap(void) const {
	return &COfflineBuyDlg::_MsgMap;
}

sFRESH_MSGMAP COfflineBuyDlg::_MsgMap = { &FrForm::_MsgMap, &COfflineBuyDlg::_MsgEntries[0] };

sFRESH_ENTRY COfflineBuyDlg::_MsgEntries[] = {
	// offlinetradedlg.cpp:3588
	{ "tradebuydlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnTradeBuyDlgInit },
	// offlinetradedlg.cpp:3589
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnOkBnInit },
	// offlinetradedlg.cpp:3590
	{ "ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineBuyDlg::OnOkBnUp },
	// offlinetradedlg.cpp:3591
	{ "buyitem_icon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnBuyItemIconInit },
	// offlinetradedlg.cpp:3592
	{ "buyitem_icon", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&COfflineBuyDlg::OnBuyItemIconOwnerDraw },
	// offlinetradedlg.cpp:3593
	{ "buyitem_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnBuyItemNameInit },
	// offlinetradedlg.cpp:3594
	{ "buyitem_ability", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnBuyItemAbilityInit },
	// offlinetradedlg.cpp:3595
	{ "my_pang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnMyPangInit },
	// offlinetradedlg.cpp:3596
	{ "price", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnPriceInit },
	// offlinetradedlg.cpp:3597
	{ "pang_left", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnPangLeftInit },
	// offlinetradedlg.cpp:3598
	{ "num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnNumberInit },
	// offlinetradedlg.cpp:3599
	{ "num_next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnNumberNextBnInit },
	// offlinetradedlg.cpp:3600
	{ "num_next", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineBuyDlg::OnNumberNextBnUp },
	// offlinetradedlg.cpp:3601
	{ "num_prev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&COfflineBuyDlg::OnNumberPrevBnInit },
	// offlinetradedlg.cpp:3602
	{ "num_prev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&COfflineBuyDlg::OnNumberPrevBnUp },
	// offlinetradedlg.cpp:3604
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// offlinetradedlg.cpp:3607
COfflineBuyDlg::COfflineBuyDlg(void) {
}

// offlinetradedlg.cpp:3645
COfflineBuyDlg::~COfflineBuyDlg(void) {
}

// offlinetradedlg.cpp:3655
void COfflineBuyDlg::OnOkBnUp(void) {
	bool bEnoughPang = true;
	m_buyCount = 0;

	if (IsLocalContent((localContentType_t)0x66) && CSharedDoc::Instance()->GetTradeMode() == 1) {
		std::list<sStall>* pStallList = CSharedDoc::Instance()->GetListStallItem();
		std::list<sStall>::iterator it;

		for (it = pStallList->begin(); it != pStallList->end(); ++it) {
			if (it->ownerUID == m_buyItem.index) {
				break;
			}
		}

		if (it != pStallList->end()) {
			m_buyCount = atoi(m_pNumber->GetLine(1, false));

			if (m_buyCount >= (int)m_pBuyItem->count) {
				m_buyCount = m_pBuyItem->count;
			}

			if (m_buyCount <= 1) {
				m_buyCount = 1;
			}

			if (m_buyOrder.price * (int)m_buyOrder.count > CSharedDoc::Instance()->m_myPang) {
				bEnoughPang = false;
			}

			if (!CSharedDoc::Instance()->ConfirmInsertItem(m_buyOrder.typeId, m_buyOrder.count)) {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				WRect rect = pForm->GetRect();
				pForm->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
				pForm->SetTimeLimit(12.0f);
				// "You already own 20,000 of this item: you cannot buy or move it."
				pForm->SetMessage("\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce "
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb1\xb8\xc0\xd4 \xb6\xc7\xb4\xc2 \xbf\xc5\xb1\xe6 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
			} else if (bEnoughPang) {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel", 0);
				// "Do you really want to buy it?"
				pForm->SetMessage("\\c0xffff0000\\c\xc1\xa4\xb8\xbb \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?", false);
				pForm->Open((FRESH_RESULT_CALLBACK)&COfflineBuyDlg::OnConfirmResult, 3);
				return;
			}
		}

		OnFreshCancel();
	} else {
		FrTradeBuyDlg::OnOkBnUp();
	}
}

// offlinetradedlg.cpp:3733
bool COfflineBuyDlg::OnConfirmResult(int result, FrForm* pForm) {
	if (result == 1) {
		if (pForm) {
			if (IsLocalContent((localContentType_t)0x66)) {
				WSendPacket packet(GCT_OFFLINESHOP_REQUEST_BUY_ITEM);
				packet.Encode4(CSharedDoc::Instance()->m_myRoomOwnerUID);
				packet.Encode4(m_buyItem.index);
				packet.Encode1(m_buyItem.used);
				packet.Encode2(m_buyCount);
				packet.Encode8(m_buyItem.item.price);
				packet.Send(SEND_TO_DEFAULT);

				CTaskManager* pTaskManager = CTaskManager::Instance();
				(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));

				m_buyCount = 0;
			}

			OnFreshOkay();
		}
	}

	return true;
}
