// Original: d:\build\custom\temp\code\client\projectg\tradedlg.cpp
#include "TradeDlg.h"
#include "SharedDoc.h"
#include "Packet.h"
#include "shared/globalnetworkdefine.h"
#include "ObjectFactory.h"
#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "frlistbox.h"
#include "frstatic.h"
#include "commonutil.h"
#include "binstr.h"
#include "TaskManager.h"
#include "actor.h"
#include "shared/Localize.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "fresh.h"
#include "frdesktop.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "UccManager.h"
#include "UccLibrary.h"
#include "bitmap.h"
#include "wtemplate.h"
#include "ChatMsg.h"
#include "ItemStorage.h"
#include "ItemInfoDlg.h"

// Title-font file list of FrTradeResultDlg (no symbol).
extern "C" char* data_a9cd08[];
// Title-font file list of FrTradeBuyDlg (no symbol); its second entry
// (0xA9CCFC) is FrTradeFinishEditDlg's.
extern "C" char* data_a9ccf8[];
// Title-font file lists of FrTradeDlg (no symbol).
extern "C" char* data_a9ccf4[];
extern "C" char* data_a9ccf0[];
// Title-font file lists of FrTradeEditDlg (no symbol).
extern "C" char* data_a9cd04[];
extern "C" char* data_a9cd00[];

#include <stdlib.h>

// WTL::CString. The original's _AtlWinModule initializer pair (_$E1/_$E2)
// was folded by the linker into another unit's identical copy, so the
// unit's own _$E<n> are re-paired in symbol_mappings.json.
#include <windows.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

#include <algorithm>

bool IsUccClothes(unsigned long typeId);
void SetTradeResultCode(int code, int msgId, sTradeItem* pItem);
void DrawPartsSlot(WPoint pt, sTradeItem* pItem);   // tradedlg.cpp:27
void DrawAbility(const sTradeItem& item, FrListItem* pListItem);

// Ten bytes of stat values (five shorts) copied as one unit.
struct sStatBonus5 {
	short m_value[5];
};

static const char* s_gatewayActorName = "Gateway";

// tradedlg.cpp:27
void DrawPartsSlot(WPoint pt, sTradeItem* pItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pItem->typeId);
	if (pPart == 0) {
		return;
	}

	float charX = 5.0f;
	float caddieX = 5.0f;

	for (int slot = 1; slot < pPart->m_charCardSlots + 1; slot++) {
		std::list<sSCardAvilityPeriodInfo>::iterator it;
		bool bFound = false;
		for (it = WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].begin(); it != WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].end(); ++it) {
			if (pItem->typeId != it->partsTypeId || pItem->guid != it->partsId) {
				continue;
			}
			IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(it->typeId);
			if (pCard == 0) {
				continue;
			}
			if (slot != it->slot) {
				continue;
			}
			if (it->kind != 0) {
				continue;
			}
			bFound = true;
			const Bitmap* pBmp = g_pFresh->GetBitmap(pCard->m_texture);
			if (pBmp == 0) {
				continue;
			}
			WRect rect(pt.x + charX, pt.y + 54.0f, (float)pBmp->Width(), (float)pBmp->Height());
			pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);
			charX += 16.0f;
		}
		if (!bFound) {
			const Bitmap* pBmp = g_pFresh->GetBitmap("card_character_empty");
			if (pBmp) {
				WRect rect(pt.x + charX, pt.y + 54.0f, (float)pBmp->Width(), (float)pBmp->Height());
				pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);
				charX += 16.0f;
			}
		}
	}

	for (int slot = 1; slot < pPart->m_caddieCardSlots + 1; slot++) {
		std::list<sSCardAvilityPeriodInfo>::iterator it;
		bool bFound = false;
		for (it = WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].begin(); it != WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].end(); ++it) {
			if (pItem->typeId != it->partsTypeId || pItem->guid != it->partsId) {
				continue;
			}
			IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(it->typeId);
			if (pCard == 0) {
				continue;
			}
			if (slot != it->slot) {
				continue;
			}
			if (it->kind != 1) {
				continue;
			}
			bFound = true;
			const Bitmap* pBmp = g_pFresh->GetBitmap(MakeStr(pCard->m_texture));
			if (pBmp == 0) {
				continue;
			}
			WRect rect(pt.x + caddieX, pt.y + 70.0f, (float)pBmp->Width(), (float)pBmp->Height());
			pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);
			caddieX += 16.0f;
		}
		if (!bFound) {
			const Bitmap* pBmp = g_pFresh->GetBitmap("card_caddie_empty");
			if (pBmp) {
				WRect rect(pt.x + caddieX, pt.y + 70.0f, (float)pBmp->Width(), (float)pBmp->Height());
				pGDI->DrawTexture(pBmp, rect, 0xFFFFFFFF, 0);
				caddieX += 16.0f;
			}
		}
	}
}

// shareddoc.h:570
const char* CSharedDoc::GetTradeTitle(void) {
	return m_tradeTitle.c_str();
}

// shareddoc.h:572
int CSharedDoc::GetCountVisitor(void) {
	return m_countVisitor;
}

// shareddoc.h:590
__int64 CSharedDoc::GetTradeIncome(void) {
	return m_tradeIncome;
}

// tradedlg.h:13
sPartEquipInfo::sPartEquipInfo(void) {
	typeId = 0;
	guid = 0;
}

// tradedlg.h:140
eTypeSalesTrade FrTradeDlg::GetPackageSale(void) const {
	return m_packageSale;
}

// tradedlg.h:141
bool FrTradeDlg::IsPackageSale(void) {
	return m_packageSale == TYPE_SALES_PACKAGE ? true : false;
}

// tradedlg.h:222
void FrTradeBuyDlg::SetClosing(bool bClosing) {
	m_bClosing = bClosing;
}

// tradedlg.h:233
void FrTradeBuyDlg::SetPackageSale(void) {
	m_packageSale = TYPE_SALES_PACKAGE;
}

// tradedlg.h:234
eTypeSalesTrade FrTradeBuyDlg::GetPackageSale(void) {
	return m_packageSale;
}

// tradedlg.h:464
void FrTradeEditDlg::SetPackageSale(eTypeSalesTrade type) {
	m_packageSale = type;
}

// tradedlg.h:465
bool FrTradeEditDlg::IsPackageSale(void) {
	return m_packageSale == TYPE_SALES_PACKAGE ? true : false;
}

// tradedlg.h:544
void FrTradeFinishEditDlg::SetPackageSale(eTypeSalesTrade type) {
	m_packageSale = type;
}

// tradedlg.h:545
bool FrTradeFinishEditDlg::IsPackageSale(void) {
	return m_packageSale == TYPE_SALES_PACKAGE ? true : false;
}

// tradedlg.h: no line record (empty constructor of a nested helper class)
FrTradeDlg::MiniBtn::MiniBtn(void) {
}

// tradedlg.h: no line record (empty constructor of a nested helper class)
FrTradeEditDlg::MiniBtn::MiniBtn(void) {
}

// tradedlg.h: no line record (empty constructor of a nested helper class)
FrTradeFinishEditDlg::MiniBtn::MiniBtn(void) {
}

// tradedlg.cpp:125
void DrawAbility(const sTradeItem& item, FrListItem* pListItem) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (!pGDI)
		return;

	sStatBonus5 slot = {0};
	sStatBonus5 base;

	unsigned long kind = item.typeId >> 26;
	bool bStats = true;

	switch (kind) {
	case 2: {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(item.typeId);

		if (!pPart)
			return;

		base = *(const sStatBonus5*)pPart->m_baseStat;
		slot = *(const sStatBonus5*)pPart->m_slotStat;

		if (IsLocalContent((localContentType_t)0x51) && pListItem)
			DrawPartsSlot(pListItem->pos, (sTradeItem*)&item);
		break;
	}
	case 7: {
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(item.typeId);

		if (!pCaddie)
			return;

		base = *(const sStatBonus5*)pCaddie->m_levels;
		break;
	}
	case 8: {
		IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(item.typeId);

		if (!pCadItem)
			return;

		base = *(const sStatBonus5*)pCadItem->m_levels;
		break;
	}
	default:
		bStats = false;
		break;
	}

	if (bStats) {
		float y = 0.0f;
		const char* statName = "";

		for (int i = 0; i < 5; i++) {
			if (base.m_value[i] == 0 && slot.m_value[i] == 0)
				continue;

			switch (i) {
			case 0:
				statName = "\xC6\xC4\xBF\xF6";
				break;
			case 1:
				statName = "\xC4\xC1\xC6\xAE\xB7\xD1";
				break;
			case 2:
				statName = "\xC1\xA4\xC8\xAE\xB5\xB5";
				break;
			case 3:
				statName = "\xBD\xBA\xC7\xC9";
				break;
			case 4:
				statName = "\xC4\xBF\xBA\xEA";
				break;
			}

			if (base.m_value[i] <= 0 && slot.m_value[i] <= 0)
				continue;

			pGDI->SetTextColor(0xFF0000FF, 0xFFFFFFFF);

			char* text;

			if (base.m_value[i] == 0)
				text = MakeStr("%s\xBD\xBD\xB7\xD4%+d", statName, slot.m_value[i]);
			else if (slot.m_value[i] == 0)
				text = MakeStr("%s%+d", statName, base.m_value[i]);
			else
				text = MakeStr("%s%+d/\xBD\xBD\xB7\xD4%+d", statName, base.m_value[i], slot.m_value[i]);

			pGDI->Print(WPoint(pListItem->pos.x + 182.0f, y + 36.0f + pListItem->pos.y), 2, text);
			y += 14.0f;
		}
	} else if (kind == 4) {
		pGDI->SetTextColor(0xFF009000, 0xFFFFFFFF);
		pGDI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 36.0f), 2, "\xBE\xF7\xB1\xD7\xB7\xB9\xC0\xCC\xB5\xE5\xC7\xD1 \xBA\xF1\xBF\xEB:");
		pGDI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 50.0f), 2, MakeStr("%d\xC6\xCE", item.period));
	}
}

// tradedlg.cpp:235
__int64 SumForTradeItemList(__int64 sum, const sTradeItem& item) {
	return sum + item.price;
}

// tradedlg.cpp:240
bool CheckPackageSaleItem(const sTradeItem& item) {
	switch (item.typeId >> 26) {
	case 2:
		return IsUccClothes(item.typeId) ? true : false;
	case 4:
		return false;
	}

	return true;
}

// tradedlg.cpp:256
bool CannotPurchaseItem(const FrListItem* pListItem) {
	const sTradeItem* pItem = (const sTradeItem*)pListItem->pData;

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pItem->typeId);

	return (pItem->statFlag & 1) ? true : false;
}

// tradedlg.cpp:273
void SetTextColorByPrice(__int64 price) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI) {
		if (price > 9999999) {
			pGDI->SetTextColor(0xFFF50018, 0xFFFFFFFF);
		} else if (price > 999999) {
			pGDI->SetTextColor(0xFFD5009B, 0xFFFFFFFF);
		} else if (price > 9999) {
			pGDI->SetTextColor(0xFFC79500, 0xFFFFFFFF);
		} else if (price > 999) {
			pGDI->SetTextColor(0xFF0E0E0E, 0xFFFFFFFF);
		} else {
			pGDI->SetTextColor(0xFF626262, 0xFFFFFFFF);
		}
	}
}

// tradedlg.cpp:325
const char* MakeStatusString(int type, int value, int slot) {
	const char* names[5] = {
		"\xC6\xC4\xBF\xF6",
		"\xC4\xC1\xC6\xAE\xB7\xD1",
		"\xC1\xA4\xC8\xAE\xB5\xB5",
		"\xBD\xBA\xC7\xC9",
		"\xC4\xBF\xBA\xEA"
	};

	if (value) {
		if (slot) {
			return MakeStr("%s+%d/%s+%d", names[type], value, "\xBD\xBD\xB7\xD4", slot);
		}
		else
			return MakeStr("%s+%d", names[type], value);
	}
	else if (slot) {
		return MakeStr("%s%s+%d", names[type], "\xBD\xBD\xB7\xD4", slot);
	}
	return "";
}

// tradedlg.cpp:346
const char* MakeCardStatusString(int type, int value) {
	const char* names[5] = {
		"\xC6\xC4\xBF\xF6\xBD\xBD\xB7\xD4",
		"\xC4\xC1\xC6\xAE\xB7\xD1\xBD\xBD\xB7\xD4",
		"\xC1\xA4\xC8\xAE\xB5\xB5\xBD\xBD\xB7\xD4",
		"\xBD\xBA\xC7\xC9\xBD\xBD\xB7\xD4",
		"\xC4\xBF\xBA\xEA\xBD\xBD\xB7\xD4"
	};

	if (value) {
		return MakeStr("%s+%d", names[type], value);
	}
	return "";
}

// tradedlg.cpp:361 (run-time type and factory registration of FrTradeDlg)
const WRTTI FrTradeDlg::m_RTTI("FrTradeDlg", &FrForm::m_RTTI);

IObject* FrTradeDlgMakeInstance(void) {
	return new FrTradeDlg;
}

struct __sFrTradeDlg {
	__sFrTradeDlg(void) {
		ObjectFactory().AddObjectFunctor(FrTradeDlgMakeInstance, "FrTradeDlg");
	}
};
static __sFrTradeDlg s_registerFrTradeDlg;

// tradedlg.cpp:363
const sFRESH_MSGMAP* FrTradeDlg::GetMessageMap(void) const {
	return &FrTradeDlg::_MsgMap;
}

sFRESH_MSGMAP FrTradeDlg::_MsgMap = { &FrForm::_MsgMap, &FrTradeDlg::_MsgEntries[0] };

sFRESH_ENTRY FrTradeDlg::_MsgEntries[] = {
	// tradedlg.cpp:365
	{ "tradedlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnTradeDlgInit },
	// tradedlg.cpp:366
	{ "visitor_back", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnVisitorBackInit },
	// tradedlg.cpp:367
	{ "visitor_back", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrTradeDlg::OnVisitorBackOwnerDraw },
	// tradedlg.cpp:368
	{ "mypang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnMyPangInit },
	// tradedlg.cpp:369
	{ "mypang", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrTradeDlg::OnMyPangOwnerDraw },
	// tradedlg.cpp:370
	{ "trade_item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnTradeItemListInit },
	// tradedlg.cpp:371
	{ "trade_item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrTradeDlg::OnTradeItemListLClick },
	// tradedlg.cpp:372
	{ "trade_item_list", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrTradeDlg::OnTradeItemListRClick },
	// tradedlg.cpp:373
	{ "trade_item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnTradeItemListOwnerDraw },
	// tradedlg.cpp:374
	{ "noitem01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnNoItem01Init },
	// tradedlg.cpp:375
	{ "noitem02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnNoItem02Init },
	// tradedlg.cpp:376
	{ "visitor", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnVisitorInit },
	// tradedlg.cpp:377
	{ "userid", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnUserIdInit },
	// tradedlg.cpp:378
	{ "trade_title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnTitleInit },
	// tradedlg.cpp:379
	{ "edit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnEditBnInit },
	// tradedlg.cpp:380
	{ "edit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeDlg::OnEditBnUp },
	// tradedlg.cpp:381
	{ "close", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnCloseBnInit },
	// tradedlg.cpp:382
	{ "close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeDlg::OnCloseBnUp },
	// tradedlg.cpp:383
	{ "hide", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnHideBnInit },
	// tradedlg.cpp:384
	{ "hide", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeDlg::OnHideBnUp },
	// tradedlg.cpp:385
	{ "exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnExitBnInit },
	// tradedlg.cpp:386
	{ "exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeDlg::OnExitBnUp },
	// tradedlg.cpp:389
	{ "package", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeDlg::OnPackagePurchaseButtonInit },
	// tradedlg.cpp:390
	{ "package", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeDlg::OnPackagePurchaseLButtonUp },
	// tradedlg.cpp:394
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// tradedlg.cpp:397
FrTradeDlg::FrTradeDlg(void) {
	m_pVisitorBack = 0;
	m_pTradeItemList = 0;
	m_pNoItem01 = 0;
	m_pNoItem02 = 0;
	m_pCountVisitor = 0;
	m_pUserId = 0;
	m_pTitleEdit = 0;
	m_pEditBn = 0;
	m_pCloseBn = 0;
	m_pHideBn = 0;
	m_pExitBn = 0;

	m_pItemBaseN = g_pFresh->GetBitmap("item_base_n");
	m_pItemBaseO = g_pFresh->GetBitmap("item_base_o");
	m_pItemBaseD = g_pFresh->GetBitmap("item_base_d");
	m_pItemBaseEmpty = g_pFresh->GetBitmap("item_base_empty");

	m_pBmpUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pBmpUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pBmpUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = data_a9ccf4;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	tagWTITLEFONT pangInfo;
	memset(&pangInfo, 0, sizeof(pangInfo));
	pangInfo.filename = data_a9ccf0;
	pangInfo.fontw = 9;
	pangInfo.fonth = 16;
	pangInfo.numPages = 1;
	pangInfo.texw = 128;
	pangInfo.texh = 32;
	pangInfo.pCharSet = "1234567890,-";
	m_pPangFont = g_resrcmng->GetTitleFont();
	m_pPangFont->Create(&pangInfo);

	m_bMyShop = false;

	m_shopOwnerUID = 0xFFFFFFFF;
	memset(m_shopData, 0, sizeof(m_shopData));

	m_bSendingServer = false;
	m_selectedItem = 0;

	m_pPackagePurchaseButton = 0;
	m_totalPrice = 0;
	m_packageSale = (eTypeSalesTrade)0;

	m_buyBtnN.pBitmap = g_pFresh->GetBitmap("buy_btn_n");
	m_buyBtnN.rect = WRect(144.0f, 66.0f, m_buyBtnN.pBitmap->Width(), m_buyBtnN.pBitmap->Height());
	m_buyBtnN.state = -1;
	m_buyBtnO.pBitmap = g_pFresh->GetBitmap("buy_btn_o");
	m_buyBtnO.rect = WRect(144.0f, 66.0f, m_buyBtnO.pBitmap->Width(), m_buyBtnO.pBitmap->Height());
	m_buyBtnO.state = -1;

	m_clockBtn.pBitmap = g_pFresh->GetBitmap("clock");
	m_clockBtn.rect = WRect(50.0f, 35.0f, m_clockBtn.pBitmap->Width(), m_clockBtn.pBitmap->Height());
	m_deleteBtn.pBitmap = g_pFresh->GetBitmap("delete");
	m_deleteBtn.rect = WRect(50.0f, 35.0f, m_deleteBtn.pBitmap->Width(), m_deleteBtn.pBitmap->Height());
}

// tradedlg.cpp:486
FrTradeDlg::~FrTradeDlg(void) {
	if (m_pPangFont) {
		m_pPangFont->Erase();
	}

	if (m_pTitleFont) {
		m_pTitleFont->Erase();
	}

	if (g_resrcmng && m_pPangFont) { g_resrcmng->Release(m_pPangFont); m_pPangFont = 0; }
	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }

	if (!m_bMyShop) {
		if (WSingleton<CTaskManager>::Instance() && WSingleton<CTaskManager>::Instance()->GetCurrentTask()
			&& WSingleton<CSharedDoc>::IsInstantiated()) {
			WSingleton<CSharedDoc>::Instance()->SendTradeExitShop(m_shopOwnerUID);
		}
	}
}

// tradedlg.cpp:513
void FrTradeDlg::OnTradeDlgInit(int) {
}

// tradedlg.cpp:518
void FrTradeDlg::OnVisitorBackInit(int param) {
	m_pVisitorBack = DYNAMIC_CAST(FrArea, param);
}

// tradedlg.cpp:524
void FrTradeDlg::OnVisitorBackOwnerDraw(void) {
}

// tradedlg.cpp:529
void FrTradeDlg::OnMyPangInit(intptr_t param) {
	FrArea* pArea = DYNAMIC_CAST(FrArea, param);
}

// tradedlg.cpp:535
void FrTradeDlg::OnMyPangOwnerDraw(void) {
	DrawPang();
}

// tradedlg.cpp:541
void FrTradeDlg::OnTradeItemListInit(int param) {
	m_pTradeItemList = DYNAMIC_CAST(FrListBox, param);

	if (m_pTradeItemList) {
		m_pTradeItemList->UseRightButton(true);
	}
}

// tradedlg.cpp:552
void FrTradeDlg::OnTradeItemListLClick(void) {
	FrListItem* pListItem = m_pTradeItemList->GetItemUnderCursor();

	if (pListItem) {
		if (!pListItem->selected) {
			m_pTradeItemList->ToggleItem(pListItem);
		}

		if (m_bMyShop) {
			return;
		}
		if (m_bSendingServer) {
			return;
		}

		if (m_packageSale == TYPE_SALES_PACKAGE) {
			return;
		}

		sTradeItem* pItem = (sTradeItem*)pListItem->pData;
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pItem->typeId);

		if (pItem->statFlag & 1) {
			return;
		}

		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

		if ((m_buyBtnN.rect + pListItem->pos).IsInRect(mouse)) {
			m_buyBtnN.state = pListItem->idx;

			pItem = (sTradeItem*)pListItem->pData;

			if (pItem->price > WSingleton<CSharedDoc>::Instance()->m_myPang) {
				SetTradeResultCode(0x15, 225, 0);
				return;
			}

			if (WSingleton<CSharedDoc>::Instance()->m_myLevel == 0) {
				SetTradeResultCode(0x17, 225, 0);
				return;
			}

			FrTradeBuyDlg* pDlg = CreateForm<FrTradeBuyDlg>(g_pFresh->GetManager(), this, "tradebuydlg", 0);
			if (pDlg) {
				pDlg->Open((FRESH_RESULT_CALLBACK)&FrTradeDlg::OnTradeBuyDlgResult, 0x40);
				pDlg->SetFadeout(false);
				pDlg->SetBuyItem(pItem);
				pDlg->SetControlsBuyItem(pItem);
			}
		}
	}
}

// tradedlg.cpp:625
// Right click on a shop item: opens its information dialog and lists the
// part's stats, its cards' stats and its other slots' descriptions in the
// ability box, then selects the item.
void FrTradeDlg::OnTradeItemListRClick(void) {
	FrListItem* pListItem = m_pTradeItemList->GetItemUnderCursor();

	if (pListItem) {
		sTradeItem* pItem = (sTradeItem*)pListItem->pData;

		if (pItem) {
			FrItemInfoDlg* pDlg = CreateForm<FrItemInfoDlg>(g_pFresh->GetManager(), this, "iteminfo", 0);

			if (pDlg) {
				pDlg->Open(0, 3);
				pDlg->SetItemInfo(pItem);

				if (IsLocalContent((localContentType_t)0x51)) {
					if ((pItem->typeId & 0xFC000000) == 0x08000000) {
						FrEdit* pAbility = DYNAMIC_CAST(FrEdit, pDlg->FindChildByName("avility"));
						if (pAbility) {
							IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pItem->typeId);
							if (pPart) {
								for (int i = 0; i < 5; i++) {
									if (pPart->m_baseStat[i] != 0 || pPart->m_slotStat[i] != 0)
										pAbility->AddText(MakeStr("\\c0xff0000ff\\c%s\\c0xff000000\\c", MakeStatusString(i, pPart->m_baseStat[i], pPart->m_slotStat[i])), false, true);
								}
							}

							for (int slot = 0; slot < 8; slot++) {
								if (pItem->m_cardTypeIds[slot] != 0) {
									int kind = (pItem->m_cardTypeIds[slot] >> 22) & 0xF;
									if (kind == 0) {
										IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(pItem->m_cardTypeIds[slot]);
										if (pCard) {
											for (int i = 0; i < 5; i++) {
												if (pCard->m_stat[i] > 0)
													pAbility->AddText(MakeStr("\\c0xff60ee0e\\c%s\\c0xff000000\\c", MakeCardStatusString(i, pCard->m_stat[i])), false, true);
											}
										}
									} else if (kind == 1) {
										IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pItem->m_cardTypeIds[slot]);
										if (pDesc) {
											char text[512];
											strcpy(text, pDesc->m_text);

											for (int i = 0; i < 512; i++) {
												if (text[i] == '\n') {
													text[i] = 0;
													break;
												}
											}

											pAbility->AddText(MakeStr("\\c0xffff0000\\c%s\\c0xff000000\\c", text), false, true);
										}
									}
								}
							}
						}
					}
				}
			}
		}

		if (!pListItem->selected) {
			m_pTradeItemList->ToggleItem(pListItem);
		}
	}
}

// tradedlg.cpp:722
void FrTradeDlg::OnTradeItemListOwnerDraw(int param) {
	FrListItem* pListItem = (FrListItem*)param;

	if (pListItem == 0) {
		return;
	}

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == 0) {
		return;
	}

	if (pListItem->pData == 0) {
		return;
	}

	const Bitmap* pBase = m_pItemBaseN;
	unsigned long diffuse = 0xFFFFFFFF;

	pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	sTradeItem item = *(sTradeItem*)pListItem->pData;
	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(item.typeId);

	if (pCommon == 0) {
		return;
	}

	if (!m_bMyShop) {
		if (item.statFlag & 1) {
			diffuse = 0xFF909090;
		}
	}

	unsigned long baseDiffuse = diffuse;
	if (m_packageSale == TYPE_SALES_PACKAGE) {
		baseDiffuse = 0x20FFFFFF;
	}

	pGI->SetTextStyle(0);

	if (pBase) {
		WRect rect(pListItem->pos.x, pListItem->pos.y, (float)pBase->Width(), (float)pBase->Height());

		pGI->DrawTexture(pBase, rect, baseDiffuse, 0);
	}

	switch (item.typeId >> 26) {
	case 2: {
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(((item.typeId >> 18) & 0xFF) | 0x04000000);
		if (pChar != 0) {
			const Bitmap* pCharBmp = g_pFresh->GetBitmap(MakeStr("s_%s", pChar->m_icon));
			if (pCharBmp != 0) {
				WRect rect(pListItem->pos.x + 3.0f, pListItem->pos.y + 3.0f, (float)pCharBmp->Width(), (float)pCharBmp->Height());
				pGI->DrawTexture(pCharBmp, rect, 0xFFFFFFFF, 0);
			}
		}
		break;
	}
	}

	const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->m_icon);

	if (pIcon) {
		float offX;
		float offY;

		switch (item.typeId >> 26) {
		case 2:
			offX = 7.0f;
			offY = 2.0f;
			break;
		default:
			offX = 16.0f;
			offY = 27.0f;
			break;
		}

		if (IsLocalContent((localContentType_t)0x57) && (item.uccStatus & 1) && IsUccClothes(item.typeId)) {
			pIcon = GetUccItemIcon(item.guid);
		}

		if (pIcon) {
			WRect rect(offX + pListItem->pos.x, offY + pListItem->pos.y, (float)pIcon->Width(), (float)pIcon->Height());
			pGI->DrawTexture(pIcon, rect, diffuse, 0);
		}
	}

	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(item.typeId)) {
		if ((item.uccStatus & 1) && item.uccSeq == 1 && m_pBmpUccOriginal) {
			pGI->DrawTexture(m_pBmpUccOriginal, WRect(pListItem->pos.x + 50.0f, pListItem->pos.y + 52.0f, (float)m_pBmpUccOriginal->Width(), (float)m_pBmpUccOriginal->Height()), 0xFFFFFFFF, 0);
		} else if ((item.uccStatus & 1) && item.uccSeq > 1 && m_pBmpUccCopied) {
			pGI->DrawTexture(m_pBmpUccCopied, WRect(pListItem->pos.x + 50.0f, pListItem->pos.y + 52.0f, (float)m_pBmpUccCopied->Width(), (float)m_pBmpUccCopied->Height()), 0xFFFFFFFF, 0);
		} else if ((item.uccStatus & 2) && m_pBmpUccTempSaved) {
			pGI->DrawTexture(m_pBmpUccTempSaved, WRect(pListItem->pos.x + 50.0f, pListItem->pos.y + 52.0f, (float)m_pBmpUccTempSaved->Width(), (float)m_pBmpUccTempSaved->Height()), 0xFFFFFFFF, 0);
		}
	}

	const char* pName;
	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(item.typeId)) {
		pName = GetUccItemName(item.guid);
	} else {
		pName = pCommon->m_name;
	}

	pGI->SetTextStyle(1);
	pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 7.0f), 2, pName);
	pGI->SetTextStyle(0);

	char price[32] = { 0 };

	MakePriceStrA(price, item.price);
	SetTextColorByPrice(item.price);

	if ((int)item.count > 1) {
		const char* str = MakeStr("%d", item.count);
		float x = pListItem->pos.x + 54.0f;

		for (unsigned int i = 0; i < strlen(str); i++) {
			x -= m_pTitleFont->GetCharWidthA(g_view, str[i] - '0');
		}

		m_pTitleFont->Print(g_view, x, pListItem->pos.y + 55.0f, str, 0, 0xFFFFFFFF, 0);

		pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 23.0f), 2, MakeStr("\xB0\xB3\xB4\xE7 %s\xC6\xCE", price));
	} else {
		pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 23.0f), 2, MakeStr("%s\xC6\xCE", price));
	}

	pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	DrawAbility(item, pListItem);

	if (!m_bMyShop && m_packageSale != TYPE_SALES_PACKAGE && !(item.statFlag & 1)) {
		if (pListItem->underCursor) {
			if ((m_buyBtnN.rect + pListItem->pos).IsInRect(g_pFresh->GetManager()->GetMousePos())) {
				pGI->DrawTexture(m_buyBtnO.pBitmap, m_buyBtnO.rect + pListItem->pos, 0xFFFFFFFF, 0);
			} else {
				pGI->DrawTexture(m_buyBtnN.pBitmap, m_buyBtnN.rect + pListItem->pos, 0xFFFFFFFF, 0);
			}
		} else {
			pGI->DrawTexture(m_buyBtnN.pBitmap, m_buyBtnN.rect + pListItem->pos, 0xFFFFFFFF, 0);
		}
	}

	switch (item.typeId >> 26) {
	case 2:
	case 4:
	case 7:
	case 16:
	case 28: {
		const Bitmap* pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", (pCommon->m_level & 0x7F) + 1));
		if (pLevel) {
			WRect rcLevel(pListItem->pos.x + 40.0f - float2int(pLevel->Width() * 0.5f), pListItem->pos.y + 51.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height());

			pGI->DrawTexture(pLevel, rcLevel, diffuse, 0);
		}
		break;
	}
	}
}

// tradedlg.cpp:962
void FrTradeDlg::OnNoItem01Init(int param) {
	m_pNoItem01 = DYNAMIC_CAST(FrStatic, param);

	if (m_pNoItem01) {
		m_pNoItem01->SetVisible(false);
	}
}

// tradedlg.cpp:973
void FrTradeDlg::OnNoItem02Init(int param) {
	m_pNoItem02 = DYNAMIC_CAST(FrStatic, param);

	if (m_pNoItem02) {
		m_pNoItem02->SetVisible(false);
	}
}

// tradedlg.cpp:984
void FrTradeDlg::OnVisitorInit(int param) {
	m_pCountVisitor = DYNAMIC_CAST(FrEdit, param);
	UpdateCountVisitor();
}

// tradedlg.cpp:992
void FrTradeDlg::OnUserIdInit(int param) {
	m_pUserId = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:998
void FrTradeDlg::OnTitleInit(intptr_t param) {
	m_pTitleEdit = DYNAMIC_CAST(FrEdit, param);
	if (m_pTitleEdit) { m_pTitleEdit->ClearLine(); m_pTitleEdit->SetLine(1, WSingleton<CSharedDoc>::Instance()->GetTradeTitle(), 0, false, 0); }
}

// tradedlg.cpp:1006
void FrTradeDlg::OnEditBnInit(int param) {
	m_pEditBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:1012
void FrTradeDlg::OnEditBnUp(void) {
	IActor* pActor = AfxGetTask()->GetActor(s_gatewayActorName);
	if (pActor) {
		pActor << MsgObject(0, 0x273, m_packageSale == TYPE_SALES_PACKAGE ? 4 : 5, 0, 0, 0, 0);
	}

	FrTradeEditDlg* pDlg = CreateForm<FrTradeEditDlg>(g_pFresh->GetManager(), this, "tradeeditdlg", 0);

	if (pDlg) {
		pDlg->m_packageSale = m_packageSale;

		pDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
		pDlg->SetFadeout(false);

		WSingleton<CSharedDoc>::Instance()->SendTradeEditShop();

		OnFreshOkay();
	}
}

// tradedlg.cpp:1041
void FrTradeDlg::OnCloseBnInit(intptr_t param) {
	m_pCloseBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:1047
void FrTradeDlg::OnCloseBnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->GetStateTrade() == STATE_TRADE_OPEN) {
		SetTradeResultCode(0x13, 225, 0);
	} else {
		SetTradeResultCode(0x12, 225, 0);
	}
}

// tradedlg.cpp:1060
void FrTradeDlg::OnHideBnInit(int param) {
	m_pHideBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:1066
void FrTradeDlg::OnHideBnUp(void) {
	IActor* pActor = AfxGetTask()->GetActor(s_gatewayActorName);
	if (pActor) {
		pActor << MsgObject(0, 0x273, m_packageSale == TYPE_SALES_PACKAGE ? 4 : 5, 0, 0, 0, 0);
	}

	OnFreshOkay();
}

// tradedlg.cpp:1080
void FrTradeDlg::OnExitBnInit(int param) {
	m_pExitBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:1086
void FrTradeDlg::OnExitBnUp(void) {
	IActor* pActor = AfxGetTask()->GetActor(s_gatewayActorName);
	if (pActor) {
		pActor << MsgObject(0, 0x273, 5, 0, 0, 0, 0);
	}

	OnFreshOkay();
}

// tradedlg.cpp:1099
void FrTradeDlg::OnPackagePurchaseButtonInit(int param) {
	m_pPackagePurchaseButton = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:1104
void FrTradeDlg::OnPackagePurchaseLButtonUp(void) {
	if (m_packageSale != TYPE_SALES_PACKAGE) {
		return;
	}

	if (m_bMyShop) {
		return;
	}

	if (m_bSendingServer) {
		return;
	}

	if (FindIf(m_pTradeItemList->m_itemList, CannotPurchaseItem)) {
		return;
	}

	if (m_totalPrice > WSingleton<CSharedDoc>::Instance()->m_myPang) {
		SetTradeResultCode(0x15, 225, 0);
		return;
	}

	if (WSingleton<CSharedDoc>::Instance()->m_myLevel == 0) {
		SetTradeResultCode(0x17, 225, 0);
		return;
	}

	FrTradeBuyDlg* pDlg = CreateForm<FrTradeBuyDlg>(g_pFresh->GetManager(), this, "tradebuydlg", 0);
	if (pDlg) {
		pDlg->Open((FRESH_RESULT_CALLBACK)&FrTradeDlg::OnTradeBuyDlgResult, 0x40);
		pDlg->SetFadeout(false);

		pDlg->m_packageSale = TYPE_SALES_PACKAGE;
		pDlg->SetBuyItemList();
		pDlg->SetControlsItemList();
	}
}

// tradedlg.cpp:1150
bool FrTradeDlg::OnInit(void) {
	EnableDrag(false);

	switch (WSingleton<CSharedDoc>::Instance()->GetStateTrade()) {
	case 1:
	case STATE_TRADE_OPEN:
	case STATE_TRADE_EDIT:
		if (WSingleton<CSharedDoc>::Instance()->GetListTradeItem()->size() <= 0) {
			if (m_pNoItem01) m_pNoItem01->SetVisible(true); if (m_pNoItem02) m_pNoItem02->SetVisible(true);
			if (m_pHideBn) m_pHideBn->Enable(false);
		} else {
			if (m_pNoItem01) m_pNoItem01->SetVisible(false); if (m_pNoItem02) m_pNoItem02->SetVisible(false);
			if (m_pHideBn) m_pHideBn->Enable(true);
		}
		break;
	}

	return true;
}

// tradedlg.cpp:1185
void FrTradeDlg::OpenTradeShop(unsigned long uid) {
	if (uid == 0xFFFFFFFF) {
		OnFreshOkay();
		return;
	}

	m_shopOwnerUID = uid;

	if (uid == WSingleton<CSharedDoc>::Instance()->m_myUID) {
		m_bMyShop = true;
		SetListTradeItem(false);

		if (m_pVisitorBack) m_pVisitorBack->SetVisible(true);
		if (m_pCountVisitor) m_pCountVisitor->SetVisible(true);
		if (m_pEditBn) m_pEditBn->SetVisible(true);
		if (m_pCloseBn) m_pCloseBn->SetVisible(true);
		if (m_pHideBn) m_pHideBn->SetVisible(true);
		if (m_pExitBn) m_pExitBn->SetVisible(false);
		if (m_pPackagePurchaseButton) m_pPackagePurchaseButton->SetVisible(false);
	}
	else {
		m_bMyShop = false;
		if (m_pVisitorBack) m_pVisitorBack->SetVisible(false);
		if (m_pCountVisitor) m_pCountVisitor->SetVisible(false);
		if (m_pEditBn) m_pEditBn->SetVisible(false);
		if (m_pCloseBn) m_pCloseBn->SetVisible(false);
		if (m_pHideBn) m_pHideBn->SetVisible(false);
		if (m_pExitBn) m_pExitBn->SetVisible(true);
	}

	if (m_pUserId) {
		char szName[64] = "";
		sprintf(szName, "\xB3\xAA\xC0\xC7 \xBB\xF3\xC1\xA1");

		if (!m_bMyShop) {
			std::list<sSlotInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it) {
				if (it->m_ruleUID == WSingleton<CSharedDoc>::Instance()->GetTradeUID()) {
					sprintf(szName, MakeStr("%s \xB4\xD4\xC0\xC7 \xBB\xF3\xC1\xA1", it->m_nickname));
					break;
				}
			}
		}

		m_pUserId->SetLine(1, szName, 0, false, 0);
	}

	if (!m_bMyShop) {
		m_pNoItem01->SetCaption("\xB8\xF0\xB5\xE7 \xBB\xF3\xC7\xB0\xC0\xCC \xC7\xB0\xC0\xFD\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.");
		m_pNoItem02->SetCaption("\xC0\xCC\xBF\xEB\xC7\xD8 \xC1\xD6\xBC\xC5\xBC\xAD \xB0\xA8\xBB\xE7\xC7\xD5\xB4\xCF\xB4\xD9.");
	}
}

// tradedlg.cpp:1263
template <class T>
struct _TradeItemSort {
	bool operator()(const T& a, const T& b) const {
		if ((a.typeId >> 26) == 2 && (b.typeId >> 26) != 2) {
			return false;
		}

		if ((a.typeId >> 26) == 2 && (b.typeId >> 26) == 2) {
			int aIndex = a.typeId & 0x3FFFFFF;
			int bIndex = b.typeId & 0x3FFFFFF;

			if (aIndex < bIndex) {
				return true;
			}
			if (aIndex > bIndex) {
				return false;
			}
		}

		if (a.typeId < b.typeId) {
			return true;
		}
		if (a.typeId > b.typeId) {
			return false;
		}

		return a.guid < b.guid;
	}
};

// tradedlg.cpp:1290
void FrTradeDlg::SetListTradeItem(bool bClear) {
	if (m_pTradeItemList == 0)
		return;

	m_totalPrice = 0;

	m_pTradeItemList->ClearItem();

	if (bClear)
		return;

	std::list<sTradeItem>* pList = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
	for (std::list<sTradeItem>::iterator it = pList->begin(); it != pList->end(); ++it) {
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(it->typeId);

		if (pCommon) {
			it->statFlag = 0;

			if ((pCommon->m_level & 0x7F) > WSingleton<CSharedDoc>::Instance()->m_myLevel)
				it->statFlag |= 1;

			m_pTradeItemList->AddItem(&*it);

			m_totalPrice += it->price;
		}
	}
}

// tradedlg.cpp:1330
void FrTradeDlg::DrawPang(void) {
	if (!g_pFresh->GetManager()->GetGDI())
		return;
	if (!m_pPangFont)
		return;

	char buf[32] = ""; WPoint pos(m_rect.x, m_rect.y);
	static int s_fontWidth = m_pPangFont->GetFontInfo()->fontw;
	static float s_textWidth;

	if (m_bMyShop) {
		MakePriceStrA(buf, WSingleton<CSharedDoc>::Instance()->GetTradeIncome());
	} else if (GetPackageSale() == TYPE_SALES_SINGLE) {
		MakePriceStrA(buf, WSingleton<CSharedDoc>::Instance()->m_myPang);
	} else {
		MakePriceStrA(buf, m_totalPrice);
	}

	s_textWidth = (s_fontWidth - 1.0f) * (strlen(buf) - 1);
	m_pPangFont->Print(g_view, pos.x + 130.0f - s_textWidth, pos.y + 380.0f, buf, 0, 0xffffffff, 0);
}

// tradedlg.cpp:1368
void FrTradeDlg::UpdateTradeTitle(void) {
	if (m_pTitleEdit) {
		m_pTitleEdit->ClearLine();
		m_pTitleEdit->SetLine(1, WSingleton<CSharedDoc>::Instance()->GetTradeTitle(), 0, false, 0);
	}
}

// tradedlg.cpp:1378
void FrTradeDlg::UpdateCountVisitor(void) {
	if (m_pCountVisitor) {
		char buf[32] = { 0 };
		MakePriceStrA(buf, WSingleton<CSharedDoc>::Instance()->GetCountVisitor());
		m_pCountVisitor->SetLine(1, buf, 0, false, 0);
	}
}

// tradedlg.cpp:1390
bool FrTradeDlg::OnTradeBuyDlgResult(int ret, FrForm* pForm) {
	if (ret == FrOK) {
		EnableSendingServer(true);
	}

	return true;
}

// tradedlg.cpp:1401
void FrTradeDlg::EnableSendingServer(bool bSending) {
	m_bSendingServer = bSending;

	if (m_pExitBn) {
		m_pExitBn->Enable(!bSending);
	}
}

// tradedlg.cpp:1412
void FrTradeDlg::ShowStaticOutOfStock(bool bShow) {
	if (m_pNoItem01) {
		m_pNoItem01->SetVisible(bShow);
	}
	if (m_pNoItem02) {
		m_pNoItem02->SetVisible(bShow);
	}
}

// tradedlg.cpp:1421
void FrTradeDlg::EnableHideBn(bool bEnable) {
	if (m_pHideBn) {
		m_pHideBn->Enable(bEnable);
	}
}

// tradedlg.cpp:1430
void FrTradeDlg::SetPackageSale(eTypeSalesTrade type) {
	m_packageSale = type;
	if (type != TYPE_SALES_PACKAGE && m_pPackagePurchaseButton) {
		m_pPackagePurchaseButton->SetVisible(false);
		m_pPackagePurchaseButton->Enable(false);
	}
}

// tradedlg.cpp:1441 (run-time type and factory registration of FrTradeBuyDlg)
const WRTTI FrTradeBuyDlg::m_RTTI("FrTradeBuyDlg", &FrForm::m_RTTI);

IObject* FrTradeBuyDlgMakeInstance(void) {
	return new FrTradeBuyDlg;
}

struct __sFrTradeBuyDlg {
	__sFrTradeBuyDlg(void) {
		ObjectFactory().AddObjectFunctor(FrTradeBuyDlgMakeInstance, "FrTradeBuyDlg");
	}
};
static __sFrTradeBuyDlg s_registerFrTradeBuyDlg;

// tradedlg.cpp:1443
const sFRESH_MSGMAP* FrTradeBuyDlg::GetMessageMap(void) const {
	return &FrTradeBuyDlg::_MsgMap;
}

sFRESH_MSGMAP FrTradeBuyDlg::_MsgMap = { &FrForm::_MsgMap, &FrTradeBuyDlg::_MsgEntries[0] };

sFRESH_ENTRY FrTradeBuyDlg::_MsgEntries[] = {
	// tradedlg.cpp:1445
	{ "tradebuydlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnTradeBuyDlgInit },
	// tradedlg.cpp:1446
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnOkBnInit },
	// tradedlg.cpp:1447
	{ "ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeBuyDlg::OnOkBnUp },
	// tradedlg.cpp:1448
	{ "buyitem_icon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnBuyItemIconInit },
	// tradedlg.cpp:1449
	{ "buyitem_icon", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrTradeBuyDlg::OnBuyItemIconOwnerDraw },
	// tradedlg.cpp:1450
	{ "buyitem_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnBuyItemNameInit },
	// tradedlg.cpp:1451
	{ "buyitem_ability", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnBuyItemAbilityInit },
	// tradedlg.cpp:1452
	{ "my_pang", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnMyPangInit },
	// tradedlg.cpp:1453
	{ "price", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnPriceInit },
	// tradedlg.cpp:1454
	{ "pang_left", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnPangLeftInit },
	// tradedlg.cpp:1455
	{ "num", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnNumberInit },
	// tradedlg.cpp:1456
	{ "num_next", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnNumberNextBnInit },
	// tradedlg.cpp:1457
	{ "num_next", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeBuyDlg::OnNumberNextBnUp },
	// tradedlg.cpp:1458
	{ "num_prev", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnNumberPrevBnInit },
	// tradedlg.cpp:1459
	{ "num_prev", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeBuyDlg::OnNumberPrevBnUp },
	// tradedlg.cpp:1462
	{ "purchase_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnPurchaseListInit },
	// tradedlg.cpp:1463
	{ "purchase_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeBuyDlg::OnPurchaseListOwnerdraw },
	// tradedlg.cpp:1467
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// tradedlg.cpp:1470
FrTradeBuyDlg::FrTradeBuyDlg(void) {
	m_pBuyItemIcon = 0;
	m_pBuyItemName = 0;
	m_pBuyItemAbility = 0;
	m_pMyPang = 0;
	m_pPrice = 0;
	m_pPangLeft = 0;
	m_pNumberNextBn = 0;
	m_pNumberPrevBn = 0;
	m_pNumber = 0;
	memset(&m_buyOrder, 0, sizeof(m_buyOrder));

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	m_pBuyItem = 0;
	m_bClosing = false;
	m_pPurchaseList = 0;
	m_packageSale = (eTypeSalesTrade)0;
	info.filename = data_a9ccf8;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	m_pBmpUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pBmpUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pBmpUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");

	m_lastNumber = 0;
}

// tradedlg.cpp:1515
FrTradeBuyDlg::~FrTradeBuyDlg(void) {
	if (m_pTitleFont) {
		m_pTitleFont->Erase();
	}

	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }
}

// tradedlg.cpp:1526
void FrTradeBuyDlg::OnTradeBuyDlgInit(int) {
}

// tradedlg.cpp:1531
void FrTradeBuyDlg::OnOkBnInit(intptr_t param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:1537
void FrTradeBuyDlg::OnOkBnUp(void) {
	bool bEnoughPang = true;

	if (m_packageSale == TYPE_SALES_PACKAGE) {
		std::list<sTradeItem>* pList = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
		if (pList->size() == 0) {
			SetTradeResultCode(0x0D, 225, 0);
			OnFreshCancel();
			return;
		}

		// std::accumulate in the original (<numeric> is missing from externs/).
		__int64 total = 0;
		for (std::list<sTradeItem>::iterator sumIt = pList->begin(); sumIt != pList->end(); ++sumIt) total = SumForTradeItemList(total, *sumIt);

		if (total > WSingleton<CSharedDoc>::Instance()->m_myPang) {
			SetTradeResultCode(0x15, 225, 0);
			bEnoughPang = false;
		}

		for (std::list<sTradeItem>::iterator it = pList->begin(); it != pList->end(); ++it) {
			if (!WSingleton<CSharedDoc>::Instance()->ConfirmInsertItem(it->typeId, it->count)) {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				WRect rect = pForm->GetRect();
				pForm->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
				pForm->SetTimeLimit(12.0f);
				// "You already own 20,000 of this item: you cannot buy or move it."
				pForm->SetMessage("\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce "
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb1\xb8\xc0\xd4 \xb6\xc7\xb4\xc2 \xbf\xc5\xb1\xe6 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
				OnFreshCancel();
				return;
			}
		}
	} else {
		if (m_pBuyItem == 0 || m_pNumber == 0) {
			SetTradeResultCode(0, 125, 0);
			OnFreshCancel();
			return;
		}

		m_buyOrder = *m_pBuyItem;

		std::list<sTradeItem>* pList = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
		std::list<sTradeItem>::iterator it;
		for (it = pList->begin(); it != pList->end(); ++it) {
			if (it->index == m_buyOrder.index) {
				break;
			}
		}

		if (it == pList->end()) {
			SetTradeResultCode(0x0D, 225, 0);
			OnFreshCancel();
			return;
		}

		m_buyOrder.count = atoi(m_pNumber->GetLine(1, false));

		if ((int)m_buyOrder.count >= (int)m_pBuyItem->count) {
			m_buyOrder.count = m_pBuyItem->count;
		}

		if ((int)m_buyOrder.count <= 1) {
			m_buyOrder.count = 1;
		}

		if (m_buyOrder.price * (int)m_buyOrder.count > WSingleton<CSharedDoc>::Instance()->m_myPang) {
			SetTradeResultCode(0x15, 225, 0);
			bEnoughPang = false;
		}

		if (!WSingleton<CSharedDoc>::Instance()->ConfirmInsertItem(m_buyOrder.typeId, m_buyOrder.count)) {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			WRect rect = pForm->GetRect();
			pForm->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
			pForm->SetTimeLimit(12.0f);
			// "You already own 20,000 of this item: you cannot buy or move it."
			pForm->SetMessage("\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce "
				"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb1\xb8\xc0\xd4 \xb6\xc7\xb4\xc2 \xbf\xc5\xb1\xe6 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
			OnFreshCancel();
			return;
		}
	}

	if (bEnoughPang) {
		FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel", 0);
		// "Do you really want to buy it?"
		pForm->SetMessage("\\c0xffff0000\\c\xc1\xa4\xb8\xbb \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?", false);
		pForm->Open((FRESH_RESULT_CALLBACK)&FrTradeBuyDlg::OnConfirmResult, 3);
		return;
	}

	OnFreshCancel();
}

// tradedlg.cpp:1650
bool CheckTradeItem(unsigned long typeId, const sTradeItem& item) {
	if (WSingleton<CSharedDoc>::Instance()->GetStateTrade() == STATE_TRADE_EDIT) {
		if (typeId == 0 || typeId == 0xFFFFFFFF)
			return false;
		if (item.guid == 0)
			return false;
		if (item.typeId == 0)
			return false;
		if (item.price == 0)
			return false;
		return (int)item.count > 0 ? true : false;
	}

	return false;
}

// tradedlg.cpp:1664
bool FrTradeBuyDlg::OnConfirmResult(int ret, FrForm* pForm) {
	if (ret == 1) {
		if (pForm) {
			if (m_packageSale == TYPE_SALES_PACKAGE) {
				// 0x11A: the package purchase; packet.h has no enumerator for it.
				WSendPacket packet((enumClientPacket)0x11A);
				packet.Encode4(WSingleton<CSharedDoc>::Instance()->GetTradeUID());

				unsigned char count = 0;
				int mark = packet.GetMark();
				packet.Encode1(count);

				std::list<sTradeItem>* pList = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
				if (pList) {
					std::list<sTradeItem>::iterator it = pList->begin();
					std::list<sTradeItem>::iterator end = pList->end();

					for (; it != end; ++it) {
						if (!CheckTradeItem(WSingleton<CSharedDoc>::Instance()->GetTradeUID(), *it)) {
							SetTradeResultCode(0, 125, 0);
							m_bClosing = true;
							OnFreshCancel();
							break;
						}

						packet.EncodeBuffer(&*it, sizeof(sTradeItem));
						count++;
					}
				}

				packet.EncodeData(mark, (char*)&count, 1);
				packet.Send(SEND_TO_DEFAULT);
			} else {
				if (!WSingleton<CSharedDoc>::Instance()->SendTradeBuyItem(WSingleton<CSharedDoc>::Instance()->GetTradeUID(), m_buyOrder)) {
					SetTradeResultCode(0, 125, 0);
					m_bClosing = true;
					OnFreshCancel();
				} else {
					m_bClosing = true;
					OnFreshOkay();
				}
			}
		}
	}

	return true;
}

// tradedlg.cpp:1738
void FrTradeBuyDlg::OnBuyItemIconInit(int param) {
	m_pBuyItemIcon = DYNAMIC_CAST(FrArea, param);
}

// tradedlg.cpp:1744
void FrTradeBuyDlg::OnBuyItemIconOwnerDraw(void) {
	if (m_pBuyItem == 0) {
		return;
	}

	WPoint pt(m_pBuyItemIcon->GetRect().x, m_pBuyItemIcon->GetRect().y);

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

	if (pGI == 0) {
		return;
	}
	if (m_pBuyItem->typeId == 0) {
		return;
	}

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(m_pBuyItem->typeId);

	if (pCommon == 0) {
		return;
	}

	float offX = 0.0f;
	float offY = 0.0f;
	const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->m_icon);

	if ((m_pBuyItem->typeId & 0xFC000000) == 0x08000000) {
		offX = -12.0f;
		offY = -20.0f;
	}

	if (IsLocalContent((localContentType_t)0x57) && (m_pBuyItem->uccStatus & 1) && IsUccClothes(m_pBuyItem->typeId)) {
		pIcon = GetUccItemIcon(m_pBuyItem->guid);
	}

	if (pIcon) {
		WRect rc(pt.x + offX + 14.0f, pt.y + offY + 11.0f, (float)pIcon->Width(), (float)pIcon->Height());

		pGI->DrawTexture(pIcon, rc, 0xFFFFFFFF, 0);
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGI->SetTextStyle(4);

		if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(m_pBuyItem->typeId)) {
			if ((m_pBuyItem->uccStatus & 1) && m_pBuyItem->uccSeq == 1 && m_pBmpUccOriginal) {
				pGI->DrawTexture(m_pBmpUccOriginal, WRect(pt.x + 51.0f, pt.y + 43.0f, (float)m_pBmpUccOriginal->Width(), (float)m_pBmpUccOriginal->Height()), 0xFFFFFFFF, 0);
			} else if ((m_pBuyItem->uccStatus & 1) && m_pBuyItem->uccSeq > 1 && m_pBmpUccCopied) {
				pGI->DrawTexture(m_pBmpUccCopied, WRect(pt.x + 51.0f, pt.y + 43.0f, (float)m_pBmpUccCopied->Width(), (float)m_pBmpUccCopied->Height()), 0xFFFFFFFF, 0);
			} else if ((m_pBuyItem->uccStatus & 2) && m_pBmpUccTempSaved) {
				pGI->DrawTexture(m_pBmpUccTempSaved, WRect(pt.x + 51.0f, pt.y + 43.0f, (float)m_pBmpUccTempSaved->Width(), (float)m_pBmpUccTempSaved->Height()), 0xFFFFFFFF, 0);
			}
		}

		if ((int)m_pBuyItem->count > 1) {
			const char* str = MakeStr("%d", m_pBuyItem->count);
			float x = pt.x + 54.0f;

			for (unsigned int i = 0; i < strlen(str); i++) {
				x -= m_pTitleFont->GetCharWidthA(g_view, str[i] - '0');
			}

			m_pTitleFont->Print(g_view, x, pt.y + 42.0f, str, 0, 0xFFFFFFFF, 0);
		}
	}

	switch (m_pBuyItem->typeId >> 26) {
	case 2:
	case 4:
	case 7:
	case 16:
	case 28: {
		const Bitmap* pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", (pCommon->m_level & 0x7F) + 1));
		if (pLevel) {
			WRect rcLevel(pt.x + 38.0f - float2int(pLevel->Width() * 0.5f), pt.y + 35.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height());

			pGI->DrawTexture(pLevel, rcLevel, 0xFFFFFFFF, 0);
		}
		break;
	}
	}
}

// tradedlg.cpp:1853
void FrTradeBuyDlg::OnBuyItemNameInit(int param) {
	m_pBuyItemName = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:1859
void FrTradeBuyDlg::OnBuyItemAbilityInit(int param) {
	m_pBuyItemAbility = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:1865
void FrTradeBuyDlg::OnMyPangInit(intptr_t param) {
	m_pMyPang = DYNAMIC_CAST(FrEdit, param);

	if (m_pMyPang) {
		char buf[32] = { 0 };
		MakePriceStrA(buf, WSingleton<CSharedDoc>::Instance()->m_myPang);

		m_pMyPang->SetLine(1, buf, 0, false, 0);
	}
}

// tradedlg.cpp:1880
void FrTradeBuyDlg::OnPriceInit(intptr_t param) {
	m_pPrice = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:1886
void FrTradeBuyDlg::OnPangLeftInit(int param) {
	m_pPangLeft = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:1892
void FrTradeBuyDlg::OnNumberInit(int param) {
	m_pNumber = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:1898
void FrTradeBuyDlg::OnNumberNextBnInit(int param) {
	m_pNumberNextBn = DYNAMIC_CAST(FrButton, param);
	if (m_pNumberNextBn) {
		m_pNumberNextBn->SetPushDelay(0.0f);
	}
}

// tradedlg.cpp:1909
void FrTradeBuyDlg::OnNumberNextBnUp(void) {
	if (m_pNumberNextBn == 0) return;
	if (m_pNumber == 0) return;
	if (m_pBuyItem == 0) return;

	int number = atoi(m_pNumber->GetLine(1, false));

	if (++number >= (int)m_pBuyItem->count) {
		number = m_pBuyItem->count;
		m_pNumberNextBn->Enable(false);
	}

	m_pNumberPrevBn->Enable(true);
	m_pNumber->ClearLine();
	m_pNumber->SetLine(1, MakeStr("%d", number), 0, false, 0);

	UpdatePrice();
}

// tradedlg.cpp:1933
void FrTradeBuyDlg::OnNumberPrevBnInit(int param) {
	m_pNumberPrevBn = DYNAMIC_CAST(FrButton, param);
	if (m_pNumberPrevBn) {
		m_pNumberPrevBn->SetPushDelay(0.0f);
	}
}

// tradedlg.cpp:1944
void FrTradeBuyDlg::OnNumberPrevBnUp(void) {
	if (m_pNumberPrevBn == 0) return;
	if (m_pNumber == 0) return;
	if (m_pBuyItem == 0) return;

	int number = atoi(m_pNumber->GetLine(1, false));

	--number;

	if (number <= 1) {
		number = 1;
		m_pNumberPrevBn->Enable(false);
	}

	m_pNumberNextBn->Enable(true);
	m_pNumber->ClearLine();
	m_pNumber->SetLine(1, MakeStr("%d", number), 0, false, 0);

	UpdatePrice();
}

// tradedlg.cpp:1968
void FrTradeBuyDlg::OnPurchaseListInit(int param) {
	m_pPurchaseList = DYNAMIC_CAST(FrListBox, param);
}

// tradedlg.cpp:1973
void FrTradeBuyDlg::OnPurchaseListOwnerdraw(int param) {
	FrListItem* pListItem = (FrListItem*)param;

	if (pListItem == 0) {
		return;
	}

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == 0) {
		return;
	}

	sTradeItem* pItem = (sTradeItem*)pListItem->pData;
	if (pItem == 0) {
		return;
	}

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pItem->typeId);
	if (pCommon == 0) {
		return;
	}

	pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	if ((pItem->typeId & 0xFC000000) == 0x08000000) {
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(((pItem->typeId >> 18) & 0xFF) | 0x04000000);
		if (pChar != 0) {
			const Bitmap* pCharBmp = g_pFresh->GetBitmap(MakeStr("s_%s", pChar->m_icon));
			if (pCharBmp != 0) {
				WRect rect(pListItem->pos.x + 3.0f, pListItem->pos.y + 3.0f, (float)pCharBmp->Width(), (float)pCharBmp->Height());
				pGI->DrawTexture(pCharBmp, rect, 0xFFFFFFFF, 0);
			}
		}
	}

	const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->m_icon);

	if (pIcon) {
		float offX;
		float offY;

		switch (pItem->typeId >> 26) {
		case 2:
			offX = 7.0f;
			offY = 2.0f;
			break;
		default:
			offX = 16.0f;
			offY = 27.0f;
			break;
		}

		if (IsLocalContent((localContentType_t)0x57) && (pItem->uccStatus & 1) && IsUccClothes(pItem->typeId)) {
			pIcon = GetUccItemIcon(pItem->guid);
		}

		if (pIcon) {
			WRect rect(offX + pListItem->pos.x, offY + pListItem->pos.y, (float)pIcon->Width(), (float)pIcon->Height());
			pGI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
		}
	}

	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(pItem->typeId)) {
		if ((pItem->uccStatus & 1) && pItem->uccSeq == 1 && m_pBmpUccOriginal) {
			pGI->DrawTexture(m_pBmpUccOriginal, WRect(pListItem->pos.x + 50.0f, pListItem->pos.y + 52.0f, (float)m_pBmpUccOriginal->Width(), (float)m_pBmpUccOriginal->Height()), 0xFFFFFFFF, 0);
		} else if ((pItem->uccStatus & 1) && pItem->uccSeq > 1 && m_pBmpUccCopied) {
			pGI->DrawTexture(m_pBmpUccCopied, WRect(pListItem->pos.x + 50.0f, pListItem->pos.y + 52.0f, (float)m_pBmpUccCopied->Width(), (float)m_pBmpUccCopied->Height()), 0xFFFFFFFF, 0);
		} else if ((pItem->uccStatus & 2) && m_pBmpUccTempSaved) {
			pGI->DrawTexture(m_pBmpUccTempSaved, WRect(pListItem->pos.x + 50.0f, pListItem->pos.y + 52.0f, (float)m_pBmpUccTempSaved->Width(), (float)m_pBmpUccTempSaved->Height()), 0xFFFFFFFF, 0);
		}
	}

	const char* pName;
	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(pItem->typeId)) {
		pName = GetUccItemName(pItem->guid);
	} else {
		pName = pCommon->m_name;
	}

	pGI->SetTextStyle(1);
	pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 7.0f), 2, pName);
	pGI->SetTextStyle(0);

	char price[32] = { 0 };

	MakePriceStrA(price, pItem->price);
	SetTextColorByPrice(pItem->price);

	pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 23.0f), 2, MakeStr("%s\xC6\xCE", price));
	pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	DrawAbility(*pItem, pListItem);

	switch (pItem->typeId >> 26) {
	case 2:
	case 4: {
		const Bitmap* pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", (pCommon->m_level & 0x7F) + 1));
		if (pLevel) {
			WRect rcLevel(pListItem->pos.x + 40.0f - float2int(pLevel->Width() * 0.5f), pListItem->pos.y + 51.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height());

			pGI->DrawTexture(pLevel, rcLevel, 0xFFFFFFFF, 0);
		}
		break;
	}
	}
}

// tradedlg.cpp:2112
bool FrTradeBuyDlg::OnInit(void) {
	EnableDrag(false);

	return true;
}

// tradedlg.cpp:2120
void FrTradeBuyDlg::OnProc(const float dt) {
	if (m_pBuyItem && m_pNumberNextBn && m_pNumberPrevBn) {
		int number = atoi(m_pNumber->GetLine(1, false));

		if (number <= 1) {
			m_pNumberNextBn->Enable(true);
			m_pNumberPrevBn->Enable(false);
			m_pNumber->ClearLine();
			m_pNumber->SetLine(1, MakeStr("%d", 1), 0, false, 0);
		} else {
			int count = m_pBuyItem->count;
			if (number >= count) {
				m_pNumberNextBn->Enable(false);
				m_pNumberPrevBn->Enable(true);
				m_pNumber->ClearLine();
				m_pNumber->SetLine(1, MakeStr("%d", count), 0, false, 0);
			} else {
				m_pNumberNextBn->Enable(true);
				m_pNumberPrevBn->Enable(true);
			}
		}

		UpdatePrice();
	}
}

// tradedlg.cpp:2152
void FrTradeBuyDlg::SetBuyItem(sTradeItem* pItem) {
	m_pBuyItemName->ClearLine();
	m_pBuyItemAbility->ClearLine();

	m_pBuyItem = pItem;

	if (!pItem)
		return;

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pItem->typeId);

	if (!pCommon)
		return;

	if (!m_pBuyItemName)
		return;

	if (IsLocalContent((localContentType_t)0x57) && (pItem->uccStatus & 1) && IsUccClothes(pItem->typeId)) {
		const char* pName = GetUccItemName(pItem->guid);

		m_pBuyItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", pName), false, true);
	} else {
		m_pBuyItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", pCommon->m_name), false, true);
	}

	sStatBonus5 base;
	sStatBonus5 slot = {0};

	switch (pItem->typeId >> 26) {
	case 2: {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pItem->typeId);

		if (!pPart)
			return;

		base = *(const sStatBonus5*)pPart->m_baseStat;
		slot = *(const sStatBonus5*)pPart->m_slotStat;
		break;
	}
	case 7: {
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(pItem->typeId);

		if (!pCaddie)
			return;

		base = *(const sStatBonus5*)pCaddie->m_levels;
		break;
	}
	case 8: {
		IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(pItem->typeId);

		if (!pCadItem)
			return;

		base = *(const sStatBonus5*)pCadItem->m_levels;
		break;
	}
	default:
		return;
	}

	const char* statName = "";

	for (int i = 0; i < 5; i++) {
		if (base.m_value[i] == 0 && slot.m_value[i] == 0)
			continue;

		switch (i) {
		case 0:
			statName = "\xC6\xC4\xBF\xF6";
			break;
		case 1:
			statName = "\xC4\xC1\xC6\xAE\xB7\xD1";
			break;
		case 2:
			statName = "\xC1\xA4\xC8\xAE\xB5\xB5";
			break;
		case 3:
			statName = "\xBD\xBA\xC7\xC9";
			break;
		case 4:
			statName = "\xC4\xBF\xBA\xEA";
			break;
		}

		if (base.m_value[i] <= 0 && slot.m_value[i] <= 0)
			continue;

		if (base.m_value[i] == 0)
			m_pBuyItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s\xBD\xBD\xB7\xD4%+d\\c0xff000000\\c", statName, slot.m_value[i]), false, true);
		else if (slot.m_value[i] == 0)
			m_pBuyItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s%+d\\c0xff000000\\c", statName, base.m_value[i]), false, true);
		else
			m_pBuyItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s%+d/\xBD\xBD\xB7\xD4%+d\\c0xff000000\\c", statName, base.m_value[i], slot.m_value[i]), false, true);
	}
}

// tradedlg.cpp:2269
void FrTradeBuyDlg::SetBuyItemList(void) {
	std::list<sTradeItem>* pList = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
	for (std::list<sTradeItem>::iterator it = pList->begin(); it != pList->end(); ++it) {
		m_pPurchaseList->AddItem(&*it);
	}
}

// tradedlg.cpp:2279
void FrTradeBuyDlg::SetControlsItemList(void) {
	m_pNumber->SetLine(1, "1", 0, false, 0);

	UpdatePriceList();

	m_pNumberNextBn->Enable(false);
	m_pNumberPrevBn->Enable(false);
	m_pNumber->Enable(false);
}

// tradedlg.cpp:2290
void FrTradeBuyDlg::UpdatePriceList(void) {
	char priceStr[32] = { 0 };
	char pangLeftStr[32] = { 0 };

	std::list<sTradeItem>* pList = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
	// std::accumulate in the original, most likely (<numeric> is missing from externs/).
	__int64 total = 0;
	for (std::list<sTradeItem>::iterator it = pList->begin(); it != pList->end(); ++it) total = SumForTradeItemList(total, *it);

	MakePriceStrA(priceStr, total);
	m_pPrice->SetLine(1, priceStr, 0, false, 0);

	MakePriceStrA(pangLeftStr, WSingleton<CSharedDoc>::Instance()->m_myPang - total);
	m_pPangLeft->SetLine(1, pangLeftStr, 0, false, 0);
}

// tradedlg.cpp:2309
void FrTradeBuyDlg::SetControlsBuyItem(sTradeItem* pItem) {
	if (m_pPrice == 0) return;
	if (m_pPangLeft == 0) return;
	if (m_pNumberNextBn == 0) return;
	if (m_pNumberPrevBn == 0) return;
	if (m_pNumber == 0) return;

	m_pPrice->ClearLine();
	m_pPangLeft->ClearLine();
	m_pNumber->ClearLine();

	if (pItem == 0) {
		m_pNumberNextBn->Enable(false);
		m_pNumberPrevBn->Enable(false);
		m_pNumber->Enable(false);
		return;
	}

	m_pNumber->SetLine(1, "1", 0, false, 0);

	UpdatePrice();

	if (pItem->count == 1) {
		m_pNumberNextBn->Enable(false);
		m_pNumberPrevBn->Enable(false);
		m_pNumber->Enable(false);
	} else {
		m_pNumberNextBn->Enable(true);
		m_pNumberPrevBn->Enable(true);
		m_pNumber->Enable(true);
	}
}

// tradedlg.cpp:2351
void FrTradeBuyDlg::UpdatePrice(void) {
	if (m_pPrice == 0) return;
	if (m_pNumber == 0) return;
	if (m_pBuyItem == 0) return;

	char priceStr[32] = { 0 };
	char pangLeftStr[32] = { 0 };
	int number = atoi(m_pNumber->GetLine(1, false));

	if (m_lastNumber != number) {
		MakePriceStrA(priceStr, m_pBuyItem->price * number);
		m_pPrice->SetLine(1, priceStr, 0, false, 0);

		MakePriceStrA(pangLeftStr, WSingleton<CSharedDoc>::Instance()->m_myPang - m_pBuyItem->price * number);
		m_pPangLeft->SetLine(1, pangLeftStr, 0, false, 0);

		m_lastNumber = number;
	}
}

// tradedlg.cpp:2377 (run-time type and factory registration of FrTradeResultDlg)
const WRTTI FrTradeResultDlg::m_RTTI("FrTradeResultDlg", &FrForm::m_RTTI);

IObject* FrTradeResultDlgMakeInstance(void) {
	return new FrTradeResultDlg;
}

struct __sFrTradeResultDlg {
	__sFrTradeResultDlg(void) {
		ObjectFactory().AddObjectFunctor(FrTradeResultDlgMakeInstance, "FrTradeResultDlg");
	}
};
static __sFrTradeResultDlg s_registerFrTradeResultDlg;

// tradedlg.cpp:2379
const sFRESH_MSGMAP* FrTradeResultDlg::GetMessageMap(void) const {
	return &FrTradeResultDlg::_MsgMap;
}

sFRESH_MSGMAP FrTradeResultDlg::_MsgMap = { &FrForm::_MsgMap, &FrTradeResultDlg::_MsgEntries[0] };

sFRESH_ENTRY FrTradeResultDlg::_MsgEntries[] = {
	// tradedlg.cpp:2381
	{ "traderesultdlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeResultDlg::OnTradeResultDlgInit },
	// tradedlg.cpp:2382
	{ "resultitem_icon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeResultDlg::OnResultItemIconInit },
	// tradedlg.cpp:2383
	{ "resultitem_icon", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrTradeResultDlg::OnResultItemIconOwnerDraw },
	// tradedlg.cpp:2384
	{ "resultitem_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeResultDlg::OnResultItemNameInit },
	// tradedlg.cpp:2385
	{ "resultitem_ability", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeResultDlg::OnResultItemAbilityInit },
	// tradedlg.cpp:2388
	{ "purchase_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeResultDlg::OnPurchaseListInit },
	// tradedlg.cpp:2389
	{ "purchase_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeResultDlg::OnPurchaseListOwnerdraw },
	// tradedlg.cpp:2393
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// tradedlg.cpp:2396
FrTradeResultDlg::FrTradeResultDlg(void) {
	m_pResultItemIcon = 0;
	m_pResultItemName = 0;
	m_pResultItemAbility = 0;
	memset(&m_resultItem, 0, sizeof(m_resultItem));

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = data_a9cd08;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	m_pBmpUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pBmpUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pBmpUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");

	m_pPurchaseList = 0;
}

// tradedlg.cpp:2430
FrTradeResultDlg::~FrTradeResultDlg(void) {
	if (m_pTitleFont) {
		m_pTitleFont->Erase();
	}

	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }
}

// tradedlg.cpp:2441
void FrTradeResultDlg::OnTradeResultDlgInit(int) {
}

// tradedlg.cpp:2446
void FrTradeResultDlg::OnResultItemIconInit(intptr_t param) {
	m_pResultItemIcon = DYNAMIC_CAST(FrArea, param);
}

// tradedlg.cpp:2452
void FrTradeResultDlg::OnResultItemIconOwnerDraw(void) {
	if (m_resultItem.typeId == 0) {
		return;
	}
	if (m_pResultItemIcon == 0) {
		return;
	}

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

	if (pGI == 0) {
		return;
	}

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(m_resultItem.typeId);

	if (pCommon == 0) {
		return;
	}

	float offX = 0.0f;
	float offY = 0.0f;
	const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->m_icon);

	if ((m_resultItem.typeId & 0xFC000000) == 0x08000000) {
		offX = -12.0f;
		offY = -20.0f;
	}

	if (IsLocalContent((localContentType_t)0x57) && (m_resultItem.uccStatus & 1) && IsUccClothes(m_resultItem.typeId)) {
		pIcon = GetUccItemIcon(m_resultItem.guid);
	}

	if (pIcon) {
		WPoint pt(m_pResultItemIcon->GetRect().x, m_pResultItemIcon->GetRect().y);

		WRect rc(pt.x + offX + 14.0f, pt.y + offY + 11.0f, (float)pIcon->Width(), (float)pIcon->Height());

		pGI->DrawTexture(pIcon, rc, 0xFFFFFFFF, 0);
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGI->SetTextStyle(4);

		if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(m_resultItem.typeId)) {
			if ((m_resultItem.uccStatus & 1) && m_resultItem.uccSeq == 1 && m_pBmpUccOriginal) {
				WPoint ptUcc(m_pResultItemIcon->GetRect().x, m_pResultItemIcon->GetRect().y);
				pGI->DrawTexture(m_pBmpUccOriginal, WRect(ptUcc.x + 51.0f, ptUcc.y + 43.0f, (float)m_pBmpUccOriginal->Width(), (float)m_pBmpUccOriginal->Height()), 0xFFFFFFFF, 0);
			} else if ((m_resultItem.uccStatus & 1) && m_resultItem.uccSeq > 1 && m_pBmpUccCopied) {
				WPoint ptUcc(m_pResultItemIcon->GetRect().x, m_pResultItemIcon->GetRect().y);
				pGI->DrawTexture(m_pBmpUccCopied, WRect(ptUcc.x + 51.0f, ptUcc.y + 43.0f, (float)m_pBmpUccCopied->Width(), (float)m_pBmpUccCopied->Height()), 0xFFFFFFFF, 0);
			} else if ((m_resultItem.uccStatus & 2) && m_pBmpUccTempSaved) {
				WPoint ptUcc(m_pResultItemIcon->GetRect().x, m_pResultItemIcon->GetRect().y);
				pGI->DrawTexture(m_pBmpUccTempSaved, WRect(ptUcc.x + 51.0f, ptUcc.y + 43.0f, (float)m_pBmpUccTempSaved->Width(), (float)m_pBmpUccTempSaved->Height()), 0xFFFFFFFF, 0);
			}
		}

		if ((int)m_resultItem.count > 1) {
			const char* str = MakeStr("%d", m_resultItem.count);
			float x = pt.x + 54.0f;

			for (unsigned int i = 0; i < strlen(str); i++) {
				x -= m_pTitleFont->GetCharWidthA(g_view, str[i] - '0');
			}

			m_pTitleFont->Print(g_view, x, pt.y + 42.0f, str, 0, 0xFFFFFFFF, 0);
		}

		switch (m_resultItem.typeId >> 26) {
		case 2:
		case 4:
		case 7:
		case 16:
		case 28: {
			const Bitmap* pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", (pCommon->m_level & 0x7F) + 1));
			if (pLevel) {
				WRect rcLevel(pt.x + 38.0f - float2int(pLevel->Width() * 0.5f), pt.y + 35.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height());

				pGI->DrawTexture(pLevel, rcLevel, 0xFFFFFFFF, 0);
			}
			break;
		}
		}
	}
}

// tradedlg.cpp:2552
void FrTradeResultDlg::OnResultItemNameInit(intptr_t param) {
	m_pResultItemName = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:2558
void FrTradeResultDlg::OnResultItemAbilityInit(intptr_t param) {
	m_pResultItemAbility = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:2564
void FrTradeResultDlg::OnPurchaseListInit(int param) {
	m_pPurchaseList = DYNAMIC_CAST(FrListBox, param);
}

// tradedlg.cpp:2569
void FrTradeResultDlg::OnPurchaseListOwnerdraw(int param) {
	FrListItem* pListItem = (FrListItem*)param;

	if (pListItem == 0) {
		return;
	}

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == 0) {
		return;
	}

	if (pListItem->pData == 0) {
		return;
	}

	pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	sTradeItem* pItem = (sTradeItem*)pListItem->pData;
	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pItem->typeId);

	if ((pItem->typeId & 0xFC000000) == 0x08000000) {
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(((pItem->typeId >> 18) & 0xFF) | 0x04000000);
		if (pChar != 0) {
			const Bitmap* pCharBmp = g_pFresh->GetBitmap(MakeStr("s_%s", pChar->m_icon));
			if (pCharBmp != 0) {
				WRect rect(pListItem->pos.x + 3.0f, pListItem->pos.y + 3.0f, (float)pCharBmp->Width(), (float)pCharBmp->Height());
				pGI->DrawTexture(pCharBmp, rect, 0xFFFFFFFF, 0);
			}
		}
	}

	const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->m_icon);

	if (pIcon) {
		float offX;
		float offY;

		switch (pItem->typeId >> 26) {
		case 2:
			offX = 7.0f;
			offY = 2.0f;
			break;
		default:
			offX = 16.0f;
			offY = 27.0f;
			break;
		}

		if (IsLocalContent((localContentType_t)0x57) && (pItem->uccStatus & 1) && IsUccClothes(pItem->typeId)) {
			pIcon = GetUccItemIcon(pItem->guid);
		}

		if (pIcon) {
			WRect rect(offX + pListItem->pos.x, offY + pListItem->pos.y, (float)pIcon->Width(), (float)pIcon->Height());
			pGI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
		}
	}

	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(pItem->typeId)) {
		if ((pItem->uccStatus & 1) && pItem->uccSeq == 1 && m_pBmpUccOriginal) {
			pGI->DrawTexture(m_pBmpUccOriginal, WRect(pListItem->pos.x + 50.0f, pListItem->pos.y + 52.0f, (float)m_pBmpUccOriginal->Width(), (float)m_pBmpUccOriginal->Height()), 0xFFFFFFFF, 0);
		} else if ((pItem->uccStatus & 1) && pItem->uccSeq > 1 && m_pBmpUccCopied) {
			pGI->DrawTexture(m_pBmpUccCopied, WRect(pListItem->pos.x + 50.0f, pListItem->pos.y + 52.0f, (float)m_pBmpUccCopied->Width(), (float)m_pBmpUccCopied->Height()), 0xFFFFFFFF, 0);
		} else if ((pItem->uccStatus & 2) && m_pBmpUccTempSaved) {
			pGI->DrawTexture(m_pBmpUccTempSaved, WRect(pListItem->pos.x + 50.0f, pListItem->pos.y + 52.0f, (float)m_pBmpUccTempSaved->Width(), (float)m_pBmpUccTempSaved->Height()), 0xFFFFFFFF, 0);
		}
	}

	const char* pName;
	if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(pItem->typeId)) {
		pName = GetUccItemName(pItem->guid);
	} else {
		pName = pCommon->m_name;
	}

	pGI->SetTextStyle(1);
	pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 7.0f), 2, pName);
	pGI->SetTextStyle(0);

	char price[32] = { 0 };

	MakePriceStrA(price, pItem->price);
	SetTextColorByPrice(pItem->price);

	pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 23.0f), 2, MakeStr("%s\xC6\xCE", price));
	pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	DrawAbility(*pItem, pListItem);

	switch (pItem->typeId >> 26) {
	case 2:
	case 4: {
		const Bitmap* pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", (pCommon->m_level & 0x7F) + 1));
		if (pLevel) {
			WRect rcLevel(pListItem->pos.x + 40.0f - float2int(pLevel->Width() * 0.5f), pListItem->pos.y + 51.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height());

			pGI->DrawTexture(pLevel, rcLevel, 0xFFFFFFFF, 0);
		}
		break;
	}
	}
}

// tradedlg.cpp:2704
bool FrTradeResultDlg::OnInit(void) {
	EnableDrag(false);

	return true;
}

// Fills a list box with the items of a container (used with std::for_each).
class CListAssignHelper {
public:
	CListAssignHelper(FrListBox* pList);

	template <class T>
	void operator()(T& item);

private:
	FrListBox* m_pList;   // 0x0
};

// tradedlg.cpp:2716
CListAssignHelper::CListAssignHelper(FrListBox* pList)
	: m_pList(pList) {
}

// tradedlg.cpp:2722
template <class T>
void CListAssignHelper::operator()(T& item) {
	m_pList->AddItem(&item);
}

// tradedlg.cpp:2731
void FrTradeResultDlg::SetResultItemList(std::vector<sTradeItem>* pList) {
	m_resultItemList.swap(*pList);

	std::for_each(m_resultItemList.begin(), m_resultItemList.end(), CListAssignHelper(m_pPurchaseList));
}

// tradedlg.cpp:2749
void FrTradeResultDlg::SetResultItem(sTradeItem* pItem) {
	m_pResultItemName->ClearLine();
	m_pResultItemAbility->ClearLine();

	if (pItem) {
		m_resultItem = *pItem;
	} else {
		memset(&m_resultItem, 0, sizeof(m_resultItem));
		return;
	}

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pItem->typeId);

	if (!pCommon)
		return;

	if (!m_pResultItemName)
		return;

	if (IsLocalContent((localContentType_t)0x57) && (pItem->uccStatus & 1) && IsUccClothes(pItem->typeId)) {
		const char* pName = GetUccItemName(pItem->guid);

		m_pResultItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", pName), false, true);
	} else {
		m_pResultItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", pCommon->m_name), false, true);
	}

	sStatBonus5 base;
	sStatBonus5 slot = {0};

	switch (pItem->typeId >> 26) {
	case 2: {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pItem->typeId);

		if (!pPart)
			return;

		base = *(const sStatBonus5*)pPart->m_baseStat;
		slot = *(const sStatBonus5*)pPart->m_slotStat;
		break;
	}
	case 7: {
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(pItem->typeId);

		if (!pCaddie)
			return;

		base = *(const sStatBonus5*)pCaddie->m_levels;
		break;
	}
	case 8: {
		IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(pItem->typeId);

		if (!pCadItem)
			return;

		base = *(const sStatBonus5*)pCadItem->m_levels;
		break;
	}
	default:
		return;
	}

	const char* statName = "";

	for (int i = 0; i < 5; i++) {
		if (base.m_value[i] == 0 && slot.m_value[i] == 0)
			continue;

		switch (i) {
		case 0:
			statName = "\xC6\xC4\xBF\xF6";
			break;
		case 1:
			statName = "\xC4\xC1\xC6\xAE\xB7\xD1";
			break;
		case 2:
			statName = "\xC1\xA4\xC8\xAE\xB5\xB5";
			break;
		case 3:
			statName = "\xBD\xBA\xC7\xC9";
			break;
		case 4:
			statName = "\xC4\xBF\xBA\xEA";
			break;
		}

		if (base.m_value[i] <= 0 && slot.m_value[i] <= 0)
			continue;

		if (base.m_value[i] == 0)
			m_pResultItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s\xBD\xBD\xB7\xD4%+d\\c0xff000000\\c", statName, slot.m_value[i]), false, true);
		else if (slot.m_value[i] == 0)
			m_pResultItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s%+d\\c0xff000000\\c", statName, base.m_value[i]), false, true);
		else
			m_pResultItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s%+d/\xBD\xBD\xB7\xD4%+d\\c0xff000000\\c", statName, base.m_value[i], slot.m_value[i]), false, true);
	}
}

// tradedlg.cpp:2870 (run-time type and factory registration of FrTradeEditDlg)
const WRTTI FrTradeEditDlg::m_RTTI("FrTradeEditDlg", &FrForm::m_RTTI);

IObject* FrTradeEditDlgMakeInstance(void) {
	return new FrTradeEditDlg;
}

struct __sFrTradeEditDlg {
	__sFrTradeEditDlg(void) {
		ObjectFactory().AddObjectFunctor(FrTradeEditDlgMakeInstance, "FrTradeEditDlg");
	}
};
static __sFrTradeEditDlg s_registerFrTradeEditDlg;

// tradedlg.cpp:2872
const sFRESH_MSGMAP* FrTradeEditDlg::GetMessageMap(void) const {
	return &FrTradeEditDlg::_MsgMap;
}

sFRESH_MSGMAP FrTradeEditDlg::_MsgMap = { &FrForm::_MsgMap, &FrTradeEditDlg::_MsgEntries[0] };

sFRESH_ENTRY FrTradeEditDlg::_MsgEntries[] = {
	// tradedlg.cpp:2874
	{ "tradeeditdlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTradeEditDlgInit },
	// tradedlg.cpp:2875
	{ "my_item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnMyItemListInit },
	// tradedlg.cpp:2876
	{ "my_item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnMyItemListLClick },
	// tradedlg.cpp:2877
	{ "my_item_list", FRCMD_RBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnMyItemListRClick },
	// tradedlg.cpp:2878
	{ "my_item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnMyItemListOwnerDraw },
	// tradedlg.cpp:2879
	{ "trade_item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTradeItemListInit },
	// tradedlg.cpp:2880
	{ "trade_item_list", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnTradeItemListLClick },
	// tradedlg.cpp:2882
	{ "trade_item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTradeItemListOwnerDraw },
	// tradedlg.cpp:2883
	{ "visitor", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnVisitorInit },
	// tradedlg.cpp:2884
	{ "userid", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnUserIdInit },
	// tradedlg.cpp:2885
	{ "trade_title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTitleInit },
	// tradedlg.cpp:2886
	{ "trade_title_edit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTitleEditInit },
	// tradedlg.cpp:2887
	{ "trade_title_edit", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrTradeEditDlg::OnTitleEditEnterKey },
	// tradedlg.cpp:2888
	{ "trade_title_edit_bn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTitleEditBnInit },
	// tradedlg.cpp:2889
	{ "trade_title_edit_bn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnTitleEditBnUp },
	// tradedlg.cpp:2890
	{ "trade_myitem_icon", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTradeMyItemIconInit },
	// tradedlg.cpp:2891
	{ "trade_myitem_icon", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnTradeMyItemIconOwnerDraw },
	// tradedlg.cpp:2892
	{ "trade_myitem_name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTradeMyItemNameInit },
	// tradedlg.cpp:2893
	{ "trade_myitem_ability", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTradeMyItemAbilityInit },
	// tradedlg.cpp:2894
	{ "trade_myitem_number", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTradeMyItemNumberInit },
	// tradedlg.cpp:2895
	{ "trade_myitem_price", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTradeMyItemPriceInit },
	// tradedlg.cpp:2896
	{ "trade_myitem_price", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnTradeMyItemPriceClick },
	// tradedlg.cpp:2897
	{ "trade_myitem_ok_bn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTradeMyItemOkBnInit },
	// tradedlg.cpp:2898
	{ "trade_myitem_ok_bn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnTradeMyItemOkBnUp },
	// tradedlg.cpp:2899
	{ "trade_myitem_cancel_bn", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTradeMyItemCancelBnInit },
	// tradedlg.cpp:2900
	{ "trade_myitem_cancel_bn", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnTradeMyItemCancelBnUp },
	// tradedlg.cpp:2901
	{ "open", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnOpenBnInit },
	// tradedlg.cpp:2902
	{ "open", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnOpenBnUp },
	// tradedlg.cpp:2903
	{ "close", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnCloseBnInit },
	// tradedlg.cpp:2904
	{ "close", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnCloseBnUp },
	// tradedlg.cpp:2905
	{ "exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnExitBnInit },
	// tradedlg.cpp:2906
	{ "exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnExitBnUp },
	// tradedlg.cpp:2909
	{ "package_check", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnPackageSaleInit },
	// tradedlg.cpp:2910
	{ "package_check", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeEditDlg::OnPackageSaleButtonUp },
	// tradedlg.cpp:2911
	{ "tooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTooltipAreaInit },
	// tradedlg.cpp:2912
	{ "tooltip", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeEditDlg::OnTooltipAreaOwnerDraw },
	// tradedlg.cpp:2916
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// tradedlg.cpp:2919
FrTradeEditDlg::FrTradeEditDlg(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	m_pTradeItemList = 0;
	m_pMyItemList = 0;
	m_pVisitor = 0;
	m_pUserId = 0;
	m_pTitle = 0;
	m_pTitleEdit = 0;
	m_pTradeMyItemIcon = 0;
	m_pTradeMyItemName = 0;
	m_pTradeMyItemAbility = 0;
	m_pTradeMyItemNumber = 0;
	m_pTradeMyItemPrice = 0;
	m_pTradeMyItemOkBn = 0;
	m_pTradeMyItemCancelBn = 0;
	m_pOpenBn = 0;
	m_pCloseBn = 0;
	m_pExitBn = 0;
	m_pPackageSaleBtn = 0;
	m_pTooltipArea = 0;

	info.filename = data_a9cd04;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	tagWTITLEFONT numberInfo;
	memset(&numberInfo, 0, sizeof(numberInfo));
	numberInfo.filename = data_a9cd00;
	numberInfo.fontw = 9;
	numberInfo.fonth = 16;
	numberInfo.numPages = 1;
	numberInfo.texw = 128;
	numberInfo.texh = 32;
	numberInfo.pCharSet = "1234567890,-";
	m_pNumberFont = g_resrcmng->GetTitleFont();
	m_pNumberFont->Create(&numberInfo);

	m_pMyItemBaseClickBmp = g_pFresh->GetBitmap("trade_myitem_base_click");
	m_pMyItemBaseOverBmp = g_pFresh->GetBitmap("trade_myitem_base_over");
	m_pItemBaseNormalBmp = g_pFresh->GetBitmap("trade_item_base_normal");
	m_pItemBaseOverBmp = g_pFresh->GetBitmap("trade_item_base_over");
	m_pItemBaseClickBmp = g_pFresh->GetBitmap("trade_item_base_click");

	m_pBmpUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pBmpUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pBmpUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");

	memset(m_titleBuffer, 0, sizeof(m_titleBuffer));
	memset(m_tradeTitle, 0, sizeof(m_tradeTitle));
	m_myItemList.clear();
	m_tradeItemList.clear();
	m_pTradeMyItem = 0;

	m_delBtnN.pBitmap = g_pFresh->GetBitmap("trade_item_del_bn_normal");
	m_delBtnN.rect = WRect(120.0f, 30.0f, m_delBtnN.pBitmap->Width(), m_delBtnN.pBitmap->Height());
	m_delBtnO.pBitmap = g_pFresh->GetBitmap("trade_item_del_bn_over");
	m_delBtnO.rect = WRect(120.0f, 30.0f, m_delBtnO.pBitmap->Width(), m_delBtnO.pBitmap->Height());
	m_editBtnN.pBitmap = g_pFresh->GetBitmap("trade_item_edit_bn_normal");
	m_editBtnN.rect = WRect(80.0f, 30.0f, m_editBtnN.pBitmap->Width(), m_editBtnN.pBitmap->Height());
	m_editBtnO.pBitmap = g_pFresh->GetBitmap("trade_item_edit_bn_over");
	m_editBtnO.rect = WRect(80.0f, 30.0f, m_editBtnO.pBitmap->Width(), m_editBtnO.pBitmap->Height());

	m_clockBtn.pBitmap = g_pFresh->GetBitmap("clock");
	m_clockBtn.rect = WRect(50.0f, 35.0f, m_clockBtn.pBitmap->Width(), m_clockBtn.pBitmap->Height());
	m_deleteBtn.pBitmap = g_pFresh->GetBitmap("delete");
	m_deleteBtn.rect = WRect(50.0f, 35.0f, m_deleteBtn.pBitmap->Width(), m_deleteBtn.pBitmap->Height());

	m_packageSale = (eTypeSalesTrade)0;
}

// tradedlg.cpp:3012
FrTradeEditDlg::~FrTradeEditDlg(void) {
	if (m_pNumberFont) {
		m_pNumberFont->Erase();
	}

	if (m_pTitleFont) {
		m_pTitleFont->Erase();
	}

	if (g_resrcmng && m_pNumberFont) { g_resrcmng->Release(m_pNumberFont); m_pNumberFont = 0; }
	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }
}

// tradedlg.cpp:3029
void FrTradeEditDlg::OnTradeEditDlgInit(int) {
}

// tradedlg.cpp:3034
void FrTradeEditDlg::OnMyItemListInit(intptr_t param) {
	m_pMyItemList = DYNAMIC_CAST(FrListBox, param);

	if (m_pMyItemList) {
		m_pMyItemList->UseRightButton(true);
	}
}

// tradedlg.cpp:3045
void FrTradeEditDlg::OnMyItemListLClick(void) {
	if (m_tradeItemList.size() < 6) {
		FrListItem* pListItem = m_pMyItemList->GetItemUnderCursor();

		if (pListItem) {
			if (!pListItem->selected) {
				m_pMyItemList->ToggleItem(pListItem);
			} else {
				sTradeItem* pItem = (sTradeItem*)pListItem->pData;
				SetTradeMyItem(pItem);
				SetControlsTradeMyItem(pItem, false);
			}
		}
	}
}

// tradedlg.cpp:3068
// Right click on one of my items: opens its information dialog with the
// part's, cards' and slots' stats (as FrTradeDlg::OnTradeItemListRClick),
// then selects the item, or picks it for the trade when already selected.
void FrTradeEditDlg::OnMyItemListRClick(void) {
	if (m_tradeItemList.size() < 6) {
		FrListItem* pListItem = m_pMyItemList->GetItemUnderCursor();

		if (pListItem) {
			sTradeItem* pItem = (sTradeItem*)pListItem->pData;

			if (pItem) {
				FrItemInfoDlg* pDlg = CreateForm<FrItemInfoDlg>(g_pFresh->GetManager(), this, "iteminfo", 0);

				if (pDlg) {
					pDlg->Open(0, 3);
					pDlg->SetItemInfo(pItem);

					if (IsLocalContent((localContentType_t)0x51)) {
						FrEdit* pAbility = DYNAMIC_CAST(FrEdit, pDlg->FindChildByName("avility"));
						if (pAbility) {
							IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pItem->typeId);
							if (pPart) {
								for (int i = 0; i < 5; i++) {
									if (pPart->m_baseStat[i] != 0 || pPart->m_slotStat[i] != 0)
										pAbility->AddText(MakeStr("\\c0xff0000ff\\c%s\\c0xff000000\\c", MakeStatusString(i, pPart->m_baseStat[i], pPart->m_slotStat[i])), false, true);
								}
							}

							for (int slot = 0; slot < 8; slot++) {
								if (pItem->m_cardTypeIds[slot] != 0) {
									int kind = (pItem->m_cardTypeIds[slot] >> 22) & 0xF;
									if (kind == 0) {
										IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(pItem->m_cardTypeIds[slot]);
										if (pCard) {
											for (int i = 0; i < 5; i++) {
												if (pCard->m_stat[i] > 0)
													pAbility->AddText(MakeStr("\\c0xff60ee0e\\c%s\\c0xff000000\\c", MakeCardStatusString(i, pCard->m_stat[i])), false, true);
											}
										}
									} else if (kind == 1) {
										IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pItem->m_cardTypeIds[slot]);
										if (pDesc) {
											char text[512];
											strcpy(text, pDesc->m_text);

											for (int i = 0; i < 512; i++) {
												if (text[i] == '\n') {
													text[i] = 0;
													break;
												}
											}

											pAbility->AddText(MakeStr("\\c0xffff0000\\c%s\\c0xff000000\\c", text), false, true);
										}
									}
								}
							}
						}
					}
				}
			}

			if (!pListItem->selected) {
				m_pMyItemList->ToggleItem(pListItem);
			} else {
				SetTradeMyItem(pItem);
				SetControlsTradeMyItem(pItem, false);
			}
		}
	}
}

// tradedlg.cpp:3164
void FrTradeEditDlg::OnMyItemListOwnerDraw(int param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	FrListItem* pListItem = (FrListItem*)param;
	if (pListItem == 0) {
		return;
	}

	sTradeItem* pItem = (sTradeItem*)pListItem->pData;
	const Bitmap* pBaseBmp = 0;

	if (pListItem->selected) {
		pBaseBmp = m_pMyItemBaseClickBmp;
		pGDI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pListItem->underCursor) {
		pBaseBmp = m_pMyItemBaseOverBmp;
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	} else {
		pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	if (pBaseBmp != 0) {
		WRect rect(pListItem->pos.x + 2.0f, pListItem->pos.y, (float)pBaseBmp->Width(), (float)pBaseBmp->Height());

		pGDI->DrawTexture(pBaseBmp, rect, 0xFFFFFFFF, 0);
	}

	if ((pItem->typeId & 0xFC000000) == 0x08000000) {
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(((pItem->typeId >> 18) & 0xFF) | 0x04000000);
		if (pChar != 0) {
			const Bitmap* pCharBmp = g_pFresh->GetBitmap(MakeStr("s_%s", pChar->m_icon));
			if (pCharBmp != 0) {
				WRect rect(pListItem->pos.x + 7.0f, pListItem->pos.y + 5.0f, (float)pCharBmp->Width(), (float)pCharBmp->Height());
				pGDI->DrawTexture(pCharBmp, rect, 0xFFFFFFFF, 0);
			}
		}
	}

	WPoint pt(pListItem->pos.x + 13.0f, pListItem->pos.y + 20.0f);
	DrawItem(pt, pItem, 0);
	if (IsLocalContent((localContentType_t)0x51)) {
		if ((pItem->typeId & 0xFC000000) == 0x08000000) {
			sTradeItem* pData = (sTradeItem*)pListItem->pData;
			DrawPartsSlot(pt, pData);
		}
	}
}

// tradedlg.cpp:3234
void FrTradeEditDlg::OnTradeItemListInit(int param) {
	m_pTradeItemList = DYNAMIC_CAST(FrListBox, param);
}

// tradedlg.cpp:3240
void FrTradeEditDlg::OnTradeItemListLClick(void) {
	FrListItem* pListItem = m_pTradeItemList->GetItemUnderCursor();

	if (pListItem == 0) {
		return;
	}

	sTradeItem* pItem = (sTradeItem*)pListItem->pData;

	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	if ((m_delBtnN.rect + pListItem->pos).IsInRect(mouse)) {
		m_pTradeItemList->SelectItem(m_pTradeItemList->GetItemUnderCursor(), true);
		SetTradeResultCode(0x14, 225, 0);
		return;
	}

	bool bOverlap = false;
	for (std::list<sTradeItem>::iterator it = m_myItemList.begin(); it != m_myItemList.end(); ++it) {
		if (it->guid == pItem->guid) {
			if ((int)(it->count + pItem->count) > 1) {
				bOverlap = true;
			}
			break;
		}
	}

	SetTradeMyItem(pItem);
	SetControlsTradeMyItem(pItem, bOverlap);
}

// tradedlg.cpp:3279
void FrTradeEditDlg::OnTradeItemListOwnerDraw(int param) {
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	if (pGDI == 0) {
		return;
	}

	FrListItem* pListItem = (FrListItem*)param;
	if (pListItem == 0) {
		return;
	}

	sTradeItem* pItem = (sTradeItem*)pListItem->pData;
	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pItem->typeId);

	if (m_pItemBaseNormalBmp) {
		WRect rect(pListItem->pos.x + 1.0f, pListItem->pos.y + 1.0f, (float)m_pItemBaseNormalBmp->Width(), (float)m_pItemBaseNormalBmp->Height());

		pGDI->DrawTexture(m_pItemBaseNormalBmp, rect, 0xFFFFFFFF, 0);
	}

	char price[32] = { 0 };

	MakePriceStrA(price, pItem->price);

	const char* pPriceText;
	if (pItem->count == 1) {
		pPriceText = MakeStr("%s\xC6\xCE", price);
	} else {
		pPriceText = MakeStr("\xB0\xB3\xB4\xE7%s\xC6\xCE", price);
	}

	if (IsLocalContent((localContentType_t)0x57) && (pItem->uccStatus & 1) && IsUccClothes(pItem->typeId)) {
		const char* pName = GetUccItemName(pItem->guid);

		pGDI->Print(WPoint(pListItem->pos.x + 10.0f, pListItem->pos.y + 13.0f), 0, "%s", pName);
	} else {
		pGDI->Print(WPoint(pListItem->pos.x + 10.0f, pListItem->pos.y + 13.0f), 0, "%s", pCommon->m_name);
	}

	SetTextColorByPrice(pItem->price);
	pGDI->Print(WPoint(pListItem->pos.x + 12.0f, pListItem->pos.y + 32.0f), 0, pPriceText);

	pGDI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	pGDI->DrawTexture(m_delBtnN.pBitmap, m_delBtnN.rect + pListItem->pos, 0xFFFFFFFF, 0);
}

// tradedlg.cpp:3342
void FrTradeEditDlg::OnVisitorInit(int param) {
	m_pVisitor = DYNAMIC_CAST(FrEdit, param);

	if (m_pVisitor) {
		char buf[32] = { 0 };
		MakePriceStrA(buf, WSingleton<CSharedDoc>::Instance()->GetCountVisitor());
		m_pVisitor->SetLine(1, buf, 0, false, 0);
	}
}

// tradedlg.cpp:3357
void FrTradeEditDlg::OnUserIdInit(int param) {
	m_pUserId = DYNAMIC_CAST(FrEdit, param);

	if (m_pUserId) {
		m_pUserId->SetLine(1, "\xB3\xAA\xC0\xC7 \xBB\xF3\xC1\xA1", 0, false, 0);
	}
}

// tradedlg.cpp:3369
void FrTradeEditDlg::OnTitleInit(intptr_t param) {
	m_pTitle = DYNAMIC_CAST(FrEdit, param);
	if (m_pTitle) {
		m_pTitle->ClearLine();
		m_pTitle->SetLine(1, WSingleton<CSharedDoc>::Instance()->GetTradeTitle(), 0, false, 0);
	}
}

// tradedlg.cpp:3377
void FrTradeEditDlg::OnTitleEditInit(int param) {
	m_pTitleEdit = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:3383
bool FrTradeEditDlg::OnTitleEditEnterKey(int) {
	eResultTrade ret = SendTradeTitle();
	if (ret != RESULT_TRADE_OK) {
		SetTradeResultCode(ret, 225, 0);
	}

	return true;
}

// tradedlg.cpp:3395
void FrTradeEditDlg::OnTitleEditBnInit(int param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:3401
void FrTradeEditDlg::OnTitleEditBnUp(void) {
	eResultTrade ret = SendTradeTitle();
	if (ret != RESULT_TRADE_OK) {
		SetTradeResultCode(ret, 225, 0);
	}
}

// tradedlg.cpp:3411
void FrTradeEditDlg::OnTradeMyItemIconInit(int param) {
	m_pTradeMyItemIcon = DYNAMIC_CAST(FrArea, param);
}

// tradedlg.cpp:3417
void FrTradeEditDlg::OnTradeMyItemIconOwnerDraw(void) {
	if (m_pTradeMyItem) {
		WPoint pt(m_pTradeMyItemIcon->GetRect().x, m_pTradeMyItemIcon->GetRect().y);

		DrawItem(pt, m_pTradeMyItem, 0);
		if (IsLocalContent((localContentType_t)0x51)) {
			if ((m_pTradeMyItem->typeId & 0xFC000000) == 0x08000000) {
				DrawPartsSlot(pt, m_pTradeMyItem);
			}
		}
	}

	DrawPang();
}

// tradedlg.cpp:3438
void FrTradeEditDlg::OnTradeMyItemNameInit(int param) {
	m_pTradeMyItemName = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:3444
void FrTradeEditDlg::OnTradeMyItemAbilityInit(int param) {
	m_pTradeMyItemAbility = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:3450
void FrTradeEditDlg::OnTradeMyItemNumberInit(int param) {
	m_pTradeMyItemNumber = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:3456
void FrTradeEditDlg::OnTradeMyItemPriceInit(int param) {
	m_pTradeMyItemPrice = DYNAMIC_CAST(FrEdit, param);
	if (m_pTradeMyItemPrice) {
		m_pTradeMyItemPrice->SetCharLimit(8, false);
	}
}

// tradedlg.cpp:3468
void FrTradeEditDlg::OnTradeMyItemPriceClick(void) {
	if (m_pTradeMyItemPrice) {
		m_pTradeMyItemPrice->SetKeyFocus(true);
	}
}

// tradedlg.cpp:3475
void FrTradeEditDlg::OnTradeMyItemOkBnInit(int param) {
	m_pTradeMyItemOkBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:3481
void FrTradeEditDlg::OnTradeMyItemOkBnUp(void) {
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

	if (IsPushedPackageSaleButton()) {
		if (CheckPackageSaleItem(item)) {
			SetTradeResultCode(0x1D, 225, 0);
			return;
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

	if (llPrice > 30000000) {
		SetTradeResultCode(0x0C, 225, 0);
		return;
	}

	item.count = number;
	item.price = llPrice;

	DelListMyItem(&item);
	AddListTradeItem(&item);

	SetTradeMyItem(0);
	SetControlsTradeMyItem(0, false);

	if (WSingleton<CSharedDoc>::Instance()->GetListTradeItem()->size() > 0) {
		SetControlsButton(true, true);
	} else {
		SetControlsButton(true, false);
	}

	if (m_pMyItemList) {
		m_pMyItemList->UnselectAllItem();
	}
}

// tradedlg.cpp:3619
void FrTradeEditDlg::OnTradeMyItemCancelBnInit(int param) {
	m_pTradeMyItemCancelBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:3625
void FrTradeEditDlg::OnTradeMyItemCancelBnUp(void) {
	SetTradeMyItem(0);
	if (m_pTradeMyItemNumber) {
		m_pTradeMyItemNumber->ClearLine();
	}
	if (m_pTradeMyItemPrice) {
		m_pTradeMyItemPrice->ClearLine();
	}
	m_pTradeMyItemNumber->Enable(false);
	m_pTradeMyItemPrice->Enable(false);
	m_pTradeMyItemOkBn->Enable(false);
	m_pTradeMyItemCancelBn->Enable(false);
}

// tradedlg.cpp:3632
void FrTradeEditDlg::OnOpenBnInit(int param) {
	m_pOpenBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:3638
void FrTradeEditDlg::OnOpenBnUp(void) {
	int ret = CheckTradeEdit();

	if (ret == 1) {
		FrTradeFinishEditDlg* pDlg = CreateForm<FrTradeFinishEditDlg>(g_pFresh->GetManager(), this, "tradefinisheditdlg", 0);
		if (pDlg) {
			pDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
			pDlg->SetFadeout(false);
			pDlg->SetListTradeItem(&m_tradeItemList);
			if (pDlg->m_pUserId) pDlg->m_pUserId->SetLine(1, "\xB3\xAA\xC0\xC7 \xBB\xF3\xC1\xA1", 0, false, 0);
			pDlg->SetTradeTitle(m_tradeTitle);

			pDlg->m_packageSale = m_pPackageSaleBtn->GetStatus() == FrButton::PRESSED ? TYPE_SALES_PACKAGE : TYPE_SALES_SINGLE;
		}
	}
	else {
		SetTradeResultCode(ret, 225, 0);
	}
}

// tradedlg.cpp:3665
void FrTradeEditDlg::OnCloseBnInit(intptr_t param) {
	m_pCloseBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:3671
void FrTradeEditDlg::OnCloseBnUp(void) {
	SetTradeResultCode(18, 225, 0);
}

// tradedlg.cpp:3677
void FrTradeEditDlg::OnExitBnInit(int param) {
	m_pExitBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:3683
void FrTradeEditDlg::OnExitBnUp(void) {
	if (WSingleton<CSharedDoc>::Instance()->GetListTradeItem()->size() > 0) {
		WSingleton<CSharedDoc>::Instance()->SendTradeOpenShop();
	} else {
		WSingleton<CSharedDoc>::Instance()->SendTradeCloseShop();
	}

	OnFreshCancel();
}

// tradedlg.cpp:3698
void FrTradeEditDlg::OnPackageSaleInit(int param) {
	m_pPackageSaleBtn = DYNAMIC_CAST(FrButton, param);
	if (m_pPackageSaleBtn) {
		m_toolTipManager.Add((FrWnd**)&m_pPackageSaleBtn, (eBarButton)0x47);
	}
}

// tradedlg.cpp:3707
void FrTradeEditDlg::OnPackageSaleButtonUp(void) {
}

// tradedlg.cpp:3717
void FrTradeEditDlg::OnTooltipAreaInit(int param) {
	m_pTooltipArea = DYNAMIC_CAST(FrArea, param);
}

// tradedlg.cpp:3722
void FrTradeEditDlg::OnTooltipAreaOwnerDraw(int) {
	m_toolTipManager.Process();
}

// tradedlg.cpp:3729
bool FrTradeEditDlg::OnInit(void) {
	EnableDrag(false);

	SetListEnableTradingMyItem();
	SetListTradingItem();
	SetListMyItem();
	SetListTradeItem();
	SetTradeMyItem(0);
	if (m_pTradeMyItemNumber) m_pTradeMyItemNumber->ClearLine(); if (m_pTradeMyItemPrice) m_pTradeMyItemPrice->ClearLine(); m_pTradeMyItemNumber->Enable(false); m_pTradeMyItemPrice->Enable(false); m_pTradeMyItemOkBn->Enable(false); m_pTradeMyItemCancelBn->Enable(false);

	if (WSingleton<CSharedDoc>::Instance()->GetListTradeItem()->size() > 0) {
		m_pOpenBn->Enable(true); m_pCloseBn->Enable(true); m_pExitBn->Enable(true);
	} else {
		m_pOpenBn->Enable(false); m_pCloseBn->Enable(false); m_pExitBn->Enable(true);
	}

	if (m_packageSale == TYPE_SALES_PACKAGE) {
		if (m_pPackageSaleBtn) m_pPackageSaleBtn->SetStatus(FrButton::PRESSED);
	}

	return true;
}

// tradedlg.cpp:3756
bool FrTradeEditDlg::PushPackageSaleButton(void) {
	if (!m_pPackageSaleBtn) {
		return false;
	}

	m_pPackageSaleBtn->SetStatus(FrButton::PRESSED);

	return true;
}

// tradedlg.cpp:3768
bool FrTradeEditDlg::IsPushedPackageSaleButton(void) {
	return m_pPackageSaleBtn->GetStatus() == FrButton::PRESSED ? true : false;
}

// tradedlg.cpp:3775
void FrTradeEditDlg::SetListEnableTradingMyItem(void) {
	m_myItemList.clear();

	sTradeItem item;

	// Parts: one of each, minus what is already on sale.
	std::list<sItemInfo>& partsList = WSingleton<CSharedDoc>::Instance()->m_myPartsList;
	for (std::list<sItemInfo>::iterator it = partsList.begin(); it != partsList.end(); ++it) {
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(it->m_typeId);

		if (pCommon == 0) {
			continue;
		}

		if (((pCommon->m_flags0 >> 1) & 0xF) == 1 || ((pCommon->m_flags0 >> 1) & 0xF) == 3) {
			item.guid = it->m_serial;

			item.typeId = it->m_typeId;
			int trading = 0;
			std::list<sTradeItem>* pTrade = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
			for (std::list<sTradeItem>::iterator t = pTrade->begin(); t != pTrade->end(); ++t) {
				if (t->guid == it->m_serial) {
					trading += t->count;
				}
			}
			item.count = 1 - trading;
			item.price = 0;

			if (IsLocalContent((localContentType_t)0x51)) {
				IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(it->m_typeId);
				if (pPart) {
					item.m_charCardSlotCount = pPart->m_charCardSlots;
					item.m_caddieCardSlotCount = pPart->m_caddieCardSlots;

					memset(item.uccData, 0, sizeof(item.uccData));

					for (std::list<sSCardAvilityPeriodInfo>::iterator card = WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].begin(); card != WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].end(); ++card) {
						if (card->partsTypeId == it->m_typeId && card->partsId == it->m_serial) {
							item.count = 0;
							break;
						}
					}
				}
			}

			if ((int)item.count > 0) {
				m_myItemList.push_back(item);
			}
		}
	}

	// UCC parts: only those not on sale and not worn.
	if (IsLocalContent((localContentType_t)0x57)) {
		std::list<sItemInfo>& uccList = WSingleton<CSharedDoc>::Instance()->m_myItemList;
		for (std::list<sItemInfo>::iterator it = uccList.begin(); it != uccList.end(); ++it) {
			int trading = 0;
			std::list<sTradeItem>* pTrade = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
			for (std::list<sTradeItem>::iterator t = pTrade->begin(); t != pTrade->end(); ++t) {
				if (t->guid == it->m_serial) {
					trading += t->count;
				}
			}
			if (trading > 0) {
				continue;
			}

			IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(it->m_typeId);
			if (pCommon == 0) {
				continue;
			}

			if (((pCommon->m_flags0 >> 1) & 0xF) == 1 || ((pCommon->m_flags0 >> 1) & 0xF) == 3) {
				bool bWorn = false;
				for (std::map<unsigned int, sCharacterInfo>::const_iterator ch = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); ch != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++ch) {
					if (ch->second.typeId == (((it->m_typeId >> 18) & 0xFF) | 0x04000000)) {
						int slot = (it->m_typeId >> 13) & 0x1F;

						if (ch->second.partIds[slot] == it->m_serial) {
							bWorn = true;
							break;
						}
					}
				}
				if (bWorn) {
					continue;
				}

				item.guid = it->m_serial;
				item.typeId = it->m_typeId;
				strncpy(item.uccIdx, it->m_uccKey, 9);
				item.uccSeq = it->m_uccSeq;
				item.count = 1;
				item.price = 0;
				item.uccStatus = it->m_uccStatus;

				if (IsLocalContent((localContentType_t)0x51)) {
					IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(it->m_typeId);
					if (pPart) {
						item.m_charCardSlotCount = pPart->m_charCardSlots;
						item.m_caddieCardSlotCount = pPart->m_caddieCardSlots;

						memset(item.uccData, 0, sizeof(item.uccData));

						for (std::list<sSCardAvilityPeriodInfo>::iterator card = WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].begin(); card != WSingleton<CSharedDoc>::Instance()->m_cardPeriodList[0].end(); ++card) {
							if (card->partsTypeId == it->m_typeId && card->partsId == it->m_serial) {
								item.count = 0;
								break;
							}
						}
					}
				}

				if ((int)item.count > 0) {
					m_myItemList.push_back(item);
				}
			}
		}
	}

	// Stackable items.
	std::list<sItemInfo>& extraList = WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3;
	for (std::list<sItemInfo>::iterator it = extraList.begin(); it != extraList.end(); ++it) {
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(it->m_typeId);

		if (pCommon == 0) {
			continue;
		}

		if (((pCommon->m_flags0 >> 1) & 0xF) == 1 || ((pCommon->m_flags0 >> 1) & 0xF) == 3) {
			item.guid = it->m_serial;

			item.typeId = it->m_typeId;
			int trading = 0;
			std::list<sTradeItem>* pTrade = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
			for (std::list<sTradeItem>::iterator t = pTrade->begin(); t != pTrade->end(); ++t) {
				if (t->guid == it->m_serial) {
					trading += t->count;
				}
			}
			item.count = it->m_count - trading;
			item.price = 0;

			if ((int)item.count > 0) {
				m_myItemList.push_back(item);
			}
		}
	}

	SetListEquipParts();

	for (std::list<sTradeItem>::iterator it = m_myItemList.begin(); it != m_myItemList.end(); ) {
		if (IsEquipParts(it->typeId, it->guid)) {
			it = m_myItemList.erase(it);
		} else {
			++it;
		}
	}

	std::list<sItemInfo>& infoList = WSingleton<CSharedDoc>::Instance()->m_itemInfoList;
	for (std::list<sItemInfo>::iterator it = infoList.begin(); it != infoList.end(); ++it) {
		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(it->m_typeId);

		if (pCommon == 0) {
			continue;
		}

		if (((pCommon->m_flags0 >> 1) & 0xF) == 1 || ((pCommon->m_flags0 >> 1) & 0xF) == 3) {
			item.guid = it->m_serial;

			item.typeId = it->m_typeId;
			int trading = 0;
			std::list<sTradeItem>* pTrade = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
			for (std::list<sTradeItem>::iterator t = pTrade->begin(); t != pTrade->end(); ++t) {
				if (t->guid == it->m_serial) {
					trading += t->count;
				}
			}
			item.count = it->m_count - trading - GetNumEquipItem(it->m_typeId);
			item.price = 0;

			if ((int)item.count > 0) {
				m_myItemList.push_back(item);
			}
		}
	}

	// Balls, except the equipped one.
	std::list<sItemInfo>& gearList = WSingleton<CSharedDoc>::Instance()->m_myGearList;
	for (std::list<sItemInfo>::iterator it = gearList.begin(); it != gearList.end(); ++it) {
		if (it->m_typeId == WSingleton<CSharedDoc>::Instance()->m_myBallId) {
			continue;
		}

		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(it->m_typeId);

		if (pCommon == 0) {
			continue;
		}

		if (((pCommon->m_flags0 >> 1) & 0xF) == 1 || ((pCommon->m_flags0 >> 1) & 0xF) == 3) {
			item.guid = it->m_serial;
			item.typeId = it->m_typeId;
			int trading = 0;
			std::list<sTradeItem>* pTrade = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
			for (std::list<sTradeItem>::iterator t = pTrade->begin(); t != pTrade->end(); ++t) {
				if (t->guid == it->m_serial) {
					trading += t->count;
				}
			}
			item.count = it->m_count - trading;
			item.price = 0;

			if ((int)item.count > 0) {
				m_myItemList.push_back(item);
			}
		}
	}

	// Caddies, except the equipped one.
	std::map<unsigned int, sCaddieInfo>& caddieMap = WSingleton<CSharedDoc>::Instance()->m_caddieMap;
	for (std::map<unsigned int, sCaddieInfo>::iterator it = caddieMap.begin(); it != caddieMap.end(); ++it) {
		if (it->second.id == WSingleton<CSharedDoc>::Instance()->m_myCaddieId) {
			continue;
		}

		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(it->second.typeId);

		if (pCommon == 0) {
			continue;
		}

		if (((pCommon->m_flags0 >> 1) & 0xF) == 1 || ((pCommon->m_flags0 >> 1) & 0xF) == 3) {
			item.guid = it->second.id;
			item.typeId = it->second.typeId;
			int trading = 0;
			std::list<sTradeItem>* pTrade = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
			for (std::list<sTradeItem>::iterator t = pTrade->begin(); t != pTrade->end(); ++t) {
				if (t->guid == it->second.id) {
					trading += t->count;
				}
			}
			item.count = 1 - trading;
			item.price = 0;

			if ((int)item.count > 0) {
				m_myItemList.push_back(item);
			}
		}
	}

	// Club sets, except the equipped one; the enchant total goes into the
	// field after the price.
	std::map<unsigned int, sItemInfo>& clubMap = WSingleton<CSharedDoc>::Instance()->m_myItemMap;
	for (std::map<unsigned int, sItemInfo>::iterator it = clubMap.begin(); it != clubMap.end(); ++it) {
		if (it->second.m_serial == WSingleton<CSharedDoc>::Instance()->m_myClubId) {
			continue;
		}

		IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(it->second.m_typeId);

		if (pCommon == 0) {
			continue;
		}

		if (((pCommon->m_flags0 >> 1) & 0xF) == 1 || ((pCommon->m_flags0 >> 1) & 0xF) == 3) {
			memset(&item, 0, sizeof(item));

			item.guid = it->second.m_serial;
			item.typeId = it->second.m_typeId;
			int trading = 0;
			std::list<sTradeItem>* pTrade = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
			for (std::list<sTradeItem>::iterator t = pTrade->begin(); t != pTrade->end(); ++t) {
				if (t->guid == it->second.m_serial) {
					trading += t->count;
				}
			}
			item.count = 1 - trading;
			item.price = 0;

			item.period = 0;
			memcpy(item.stats, it->second.m_clubStatLevel, sizeof(item.stats));

			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < (short)item.stats[i]; j++) {
					IFF_STRUCT::sEnchant* pEnchant = ItemManager()->FindEnchant(((i | 0x340) << 20) | j);

					if (pEnchant) {
						item.period += pEnchant->m_value;
					}
				}
			}

			if ((int)item.count > 0) {
				m_myItemList.push_back(item);
			}
		}
	}

	// Cards.
	if (IsLocalContent((localContentType_t)0x51)) {
		std::list<sCards>& cardList = WSingleton<CSharedDoc>::Instance()->m_myCardList;
		for (std::list<sCards>::iterator it = cardList.begin(); it != cardList.end(); ++it) {
			IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(it->m_typeId);

			if (pCommon == 0) {
				continue;
			}

			if (((pCommon->m_flags0 >> 1) & 0xF) == 1 || ((pCommon->m_flags0 >> 1) & 0xF) == 3) {
				item.guid = it->m_guid;
				item.typeId = it->m_typeId;
				int trading = 0;
				std::list<sTradeItem>* pTrade = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
				for (std::list<sTradeItem>::iterator t = pTrade->begin(); t != pTrade->end(); ++t) {
					if (t->guid == it->m_guid) {
						trading += t->count;
					}
				}
				item.count = it->m_count - trading;
				item.price = 0;

				if ((int)item.count > 0) {
					m_myItemList.push_back(item);
				}
			}
		}
	}
}

// tradedlg.cpp:4082
void FrTradeEditDlg::SetListEquipParts(void) {
	m_equipPartsList.clear();

	for (std::map<unsigned int, sCharacterInfo>::iterator it = WSingleton<CSharedDoc>::Instance()->m_characterMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_characterMap.end(); ++it) {
		for (int i = 0; i < 24; i++) {
			if (it->second.partTypeIds[i] != 0) {
				sPartEquipInfo info;
				info.typeId = it->second.partTypeIds[i];
				info.guid = it->second.partIds[i];
				m_equipPartsList.push_back(info);
			}
		}
	}
}

// tradedlg.cpp:4111
bool FrTradeEditDlg::IsEquipParts(unsigned long typeId, unsigned long guid) {
	bool bFound = false;

	for (std::list<sPartEquipInfo>::iterator it = m_equipPartsList.begin(); it != m_equipPartsList.end(); ++it) {
		if ((it->guid == 0 && it->typeId == typeId) || (it->guid != 0 && it->typeId == typeId && it->guid == guid)) {
			m_equipPartsList.erase(it);
			bFound = true;
			break;
		}
	}

	return bFound;
}

// tradedlg.cpp:4137
int FrTradeEditDlg::GetNumEquipItem(unsigned long typeId) {
	int num = 0;

	for (int i = 0; i < 10; i++) {
		if (typeId == WSingleton<CSharedDoc>::Instance()->m_equipItem[i]) {
			num++;
		}
	}

	return num;
}

// tradedlg.cpp:4154
int FrTradeEditDlg::GetNumTradingItem(unsigned long guid) {
	int num = 0;

	std::list<sTradeItem>* pList = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
	for (std::list<sTradeItem>::iterator it = pList->begin(); it != pList->end(); ++it) {
		if (it->guid == guid) {
			num += it->count;
		}
	}

	return num;
}

// tradedlg.cpp:4172
void FrTradeEditDlg::SetListTradingItem(void) {
	std::list<sTradeItem>* pList = WSingleton<CSharedDoc>::Instance()->GetListTradeItem();
	for (std::list<sTradeItem>::iterator it = pList->begin(); it != pList->end(); ++it) {
		m_tradeItemList.push_back(*it);
	}
}

// tradedlg.cpp:4184
void FrTradeEditDlg::SetListMyItem(void) {
	if (m_pMyItemList) {
		m_pMyItemList->ClearItem();

		_TradeItemSort<sTradeItem> sorter;
		m_myItemList.sort(sorter);
		for (std::list<sTradeItem>::iterator it = m_myItemList.begin(); it != m_myItemList.end(); ++it) {
			m_pMyItemList->AddItem(&*it);
		}
	}
}

// tradedlg.cpp:4228
void FrTradeEditDlg::SetListTradeItem(void) {
	if (m_pTradeItemList) {
		m_pTradeItemList->ClearItem();

		for (std::list<sTradeItem>::iterator it = m_tradeItemList.begin(); it != m_tradeItemList.end(); ++it) {
			m_pTradeItemList->AddItem(&*it);
		}
	}
}

// tradedlg.cpp:4242
void FrTradeEditDlg::SetTradeMyItem(sTradeItem* pItem) {
	if (m_pTradeMyItemName)
		m_pTradeMyItemName->ClearLine();
	if (m_pTradeMyItemAbility)
		m_pTradeMyItemAbility->ClearLine();

	m_pTradeMyItem = pItem;

	if (!pItem)
		return;

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(pItem->typeId);

	if (!pCommon)
		return;

	if (!m_pTradeMyItemName)
		return;

	if (IsLocalContent((localContentType_t)0x57) && (pItem->uccStatus & 1) && IsUccClothes(pItem->typeId)) {
		const char* pName = GetUccItemName(pItem->guid);

		m_pTradeMyItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", pName), false, true);
	} else {
		m_pTradeMyItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", pCommon->m_name), false, true);
	}

	sStatBonus5 base;
	sStatBonus5 slot = {0};

	unsigned long kind = pItem->typeId >> 26;
	bool bStats = true;

	switch (kind) {
	case 2: {
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(pItem->typeId);

		if (!pPart)
			return;

		base = *(const sStatBonus5*)pPart->m_baseStat;
		slot = *(const sStatBonus5*)pPart->m_slotStat;
		break;
	}
	case 7: {
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(pItem->typeId);

		if (!pCaddie)
			return;

		base = *(const sStatBonus5*)pCaddie->m_levels;
		break;
	}
	case 8: {
		IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(pItem->typeId);

		if (!pCadItem)
			return;

		base = *(const sStatBonus5*)pCadItem->m_levels;
		break;
	}
	default:
		bStats = false;
		break;
	}

	if (bStats) {
		const char* statName = "";

		for (int i = 0; i < 5; i++) {
			if (base.m_value[i] == 0 && slot.m_value[i] == 0)
				continue;

			switch (i) {
			case 0:
				statName = "\xC6\xC4\xBF\xF6";
				break;
			case 1:
				statName = "\xC4\xC1\xC6\xAE\xB7\xD1";
				break;
			case 2:
				statName = "\xC1\xA4\xC8\xAE\xB5\xB5";
				break;
			case 3:
				statName = "\xBD\xBA\xC7\xC9";
				break;
			case 4:
				statName = "\xC4\xBF\xBA\xEA";
				break;
			}

			if (base.m_value[i] <= 0 && slot.m_value[i] <= 0)
				continue;

			if (base.m_value[i] == 0)
				m_pTradeMyItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s\xBD\xBD\xB7\xD4%+d\\c0xff000000\\c", statName, slot.m_value[i]), false, true);
			else if (slot.m_value[i] == 0)
				m_pTradeMyItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s%+d\\c0xff000000\\c", statName, base.m_value[i]), false, true);
			else
				m_pTradeMyItemAbility->AddText(MakeStr("\\c0xff0000ff\\c%s%+d/\xBD\xBD\xB7\xD4%+d\\c0xff000000\\c", statName, base.m_value[i], slot.m_value[i]), false, true);
		}
	} else if (kind == 4) {
		m_pTradeMyItemAbility->AddText(MakeStr("\\c0xff009000\\c\xBE\xF7\xB1\xD7\xB7\xB9\xC0\xCC\xB5\xE5\xC7\xD1 \xBA\xF1\xBF\xEB:\n%d\xC6\xCE\\c0xff000000\\c", pItem->period), false, true);
	}
}

// tradedlg.cpp:4364
void FrTradeEditDlg::AddListTradeItem(sTradeItem* pItem) {
	if (pItem == 0) return;
	if (m_pTradeMyItem == 0) return;

	std::list<sTradeItem>::iterator it;
	for (it = m_tradeItemList.begin(); it != m_tradeItemList.end(); ++it) {
		if (&*it == m_pTradeMyItem) break;
	}

	bool bExist = false;
	if (it != m_tradeItemList.end()) {
		if (&*it == m_pTradeMyItem) {
			bExist = true;
		}
	}

	if (!bExist) {
		std::list<sTradeItem>::iterator itNew = m_tradeItemList.insert(m_tradeItemList.end(), *pItem);
		m_pTradeItemList->AddItem(&*itNew);
	}
	else {
		*it = *pItem;
	}
}

// tradedlg.cpp:4415
void FrTradeEditDlg::DelListTradeItem(sTradeItem* pItem) {
	if (pItem) {
		for (std::list<sTradeItem>::iterator it = m_tradeItemList.begin(); it != m_tradeItemList.end(); ++it) {
			if (&*it == pItem) {
				m_pTradeItemList->DelItem(&*it);
				m_tradeItemList.erase(it);
				break;
			}
		}
	}
}

// tradedlg.cpp:4433
void FrTradeEditDlg::AddListMyItem(sTradeItem* pItem) {
	if (pItem == 0) return;

	std::list<sTradeItem>::iterator it;
	for (it = m_myItemList.begin(); it != m_myItemList.end(); ++it) {
		if (it->guid == pItem->guid) break;
	}

	if (it == m_myItemList.end()) {
		std::list<sTradeItem>::iterator itNew = m_myItemList.insert(m_myItemList.end(), *pItem);
		m_pMyItemList->AddItem(&*itNew);
	}
	else {
		if ((int)pItem->count > 0) {
			it->count += pItem->count;
		}
	}
}

// tradedlg.cpp:4466
void FrTradeEditDlg::DelListMyItem(sTradeItem* pItem) {
	if (pItem) {
		if (m_pTradeMyItem) {
			std::list<sTradeItem>::iterator it;
			for (it = m_myItemList.begin(); it != m_myItemList.end(); ++it) {
				if (it->guid == pItem->guid) {
					break;
				}
			}

			if (it == m_myItemList.end()) {
				return;
			}

			unsigned long tradingCount = 0;
			for (std::list<sTradeItem>::iterator trading = m_tradeItemList.begin(); trading != m_tradeItemList.end(); ++trading) {
				if (&*trading == m_pTradeMyItem) {
					tradingCount = trading->count;
					break;
				}
			}

			if (&*it == m_pTradeMyItem) {
				if (it->count == pItem->count) {
					m_pMyItemList->DelItem(&*it);
					m_myItemList.erase(it);
				} else if ((int)it->count > (int)pItem->count) {
					it->count -= pItem->count;
				}
			} else {
				it->count += tradingCount - pItem->count;
			}
		}
	}
}

// tradedlg.cpp:4517
void FrTradeEditDlg::CancelSelectedTradeItem(void) {
	if (m_pTradeItemList) {
		FrListItem* pSelected = m_pTradeItemList->GetSelected();

		if (pSelected) {
			sTradeItem* pItem = (sTradeItem*)pSelected->pData;

			if (pItem) {
				if (pItem == m_pTradeMyItem) {
					SetTradeMyItem(0);
					SetControlsTradeMyItem(0, false);
				}

				AddListMyItem(pItem);
				DelListTradeItem(pItem);

				if (m_tradeItemList.size() <= 0) {
					if (WSingleton<CSharedDoc>::Instance()->GetListTradeItem()->size() > 0) {
						SetControlsButton(false, true);
					} else {
						SetControlsButton(false, false);
					}
				}
			}
		}
	}
}

// tradedlg.cpp:4550
void FrTradeEditDlg::DrawPang(void) {
	if (!g_pFresh->GetManager()->GetGDI())
		return;
	if (!m_pNumberFont)
		return;

	char buf[32] = "";
	WRect rect = GetRect();
	static int s_fontWidth = m_pNumberFont->GetFontInfo()->fontw;
	static float s_textWidth;

	MakePriceStrA(buf, WSingleton<CSharedDoc>::Instance()->m_myPang);
	s_textWidth = (s_fontWidth - 1.0f) * (strlen(buf) - 1);
	m_pNumberFont->Print(g_view, rect.x + 130.0f - s_textWidth, rect.y + 483.0f, buf, 0, 0xffffffff, 0);
}

// tradedlg.cpp:4569
void FrTradeEditDlg::DrawItem(WPoint pt, sTradeItem* pItem, unsigned char flag) {
	if (pItem == 0) {
		return;
	}

	unsigned long typeId = pItem->typeId;
	int count = pItem->count;
	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();

	if (pGI == 0) {
		return;
	}
	if (typeId == 0) {
		return;
	}

	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(typeId);

	if (pCommon == 0) {
		return;
	}

	float offX = 0.0f;
	float offY = 0.0f;
	const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->m_icon);

	if (IsLocalContent((localContentType_t)0x57) && (pItem->uccStatus & 1) && IsUccClothes(pItem->typeId)) {
		pIcon = GetUccItemIcon(pItem->guid);
	}

	if ((typeId >> 26) == 2) {
		offX = -12.0f;
		offY = -20.0f;
	}

	if (pIcon) {
		WRect rc(pt.x + offX + 14.0f, pt.y + offY + 11.0f, (float)pIcon->Width(), (float)pIcon->Height());

		pGI->DrawTexture(pIcon, rc, 0xFFFFFFFF, 0);
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
		pGI->SetTextStyle(4);

		if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(pItem->typeId)) {
			if ((pItem->uccStatus & 1) && pItem->uccSeq == 1 && m_pBmpUccOriginal) {
				pGI->DrawTexture(m_pBmpUccOriginal, WRect(pt.x + 46.0f, pt.y + 48.0f, (float)m_pBmpUccOriginal->Width(), (float)m_pBmpUccOriginal->Height()), 0xFFFFFFFF, 0);
			} else if ((pItem->uccStatus & 1) && pItem->uccSeq > 1 && m_pBmpUccCopied) {
				pGI->DrawTexture(m_pBmpUccCopied, WRect(pt.x + 46.0f, pt.y + 48.0f, (float)m_pBmpUccCopied->Width(), (float)m_pBmpUccCopied->Height()), 0xFFFFFFFF, 0);
			} else if ((pItem->uccStatus & 2) && m_pBmpUccTempSaved) {
				pGI->DrawTexture(m_pBmpUccTempSaved, WRect(pt.x + 46.0f, pt.y + 48.0f, (float)m_pBmpUccTempSaved->Width(), (float)m_pBmpUccTempSaved->Height()), 0xFFFFFFFF, 0);
			}
		}

		if (count > 1) {
			const char* str = MakeStr("%d", count);
			float x = pt.x + 54.0f;

			for (unsigned int i = 0; i < strlen(str); i++) {
				x -= m_pTitleFont->GetCharWidthA(g_view, str[i] - '0');
			}

			m_pTitleFont->Print(g_view, x, pt.y + 42.0f, str, 0, 0xFFFFFFFF, 0);
		}
	}

	switch (typeId >> 26) {
	case 2:
	case 4:
	case 7:
	case 16:
	case 28: {
		const Bitmap* pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", (pCommon->m_level & 0x7F) + 1));
		if (pLevel) {
			WRect rcLevel(pt.x + 38.0f - float2int(pLevel->Width() * 0.5f), pt.y + 35.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height());

			pGI->DrawTexture(pLevel, rcLevel, 0xFFFFFFFF, 0);
		}
		break;
	}
	}
}

// tradedlg.cpp:4668
void FrTradeEditDlg::SetControlsButton(bool bOpen, bool bClose) {
	m_pOpenBn->Enable(bOpen);
	m_pCloseBn->Enable(bClose);
	m_pExitBn->Enable(true);
}

// tradedlg.cpp:4676
void FrTradeEditDlg::SetControlsTradeMyItem(sTradeItem* pItem, bool bEnable) {
	if (m_pTradeMyItemNumber)
		m_pTradeMyItemNumber->ClearLine();
	if (m_pTradeMyItemPrice)
		m_pTradeMyItemPrice->ClearLine();

	if (pItem == 0) {
		m_pTradeMyItemNumber->Enable(false);
		m_pTradeMyItemPrice->Enable(false);
		m_pTradeMyItemOkBn->Enable(false);
		m_pTradeMyItemCancelBn->Enable(false);
	} else {
		m_pTradeMyItemNumber->Enable(true);
		m_pTradeMyItemPrice->Enable(true);
		m_pTradeMyItemOkBn->Enable(true);
		m_pTradeMyItemCancelBn->Enable(true);

		if (pItem->count == 1) {
			m_pTradeMyItemNumber->SetLine(1, "1", 0, false, 0);
			m_pTradeMyItemNumber->Enable(bEnable);
		}

		if (pItem->price > 0) {
			m_pTradeMyItemNumber->SetLine(1, MakeStr("%d", pItem->count), 0, false, 0);
			m_pTradeMyItemPrice->SetLine(1, MakeStr("%I64d", pItem->price), 0, false, 0);
		}
	}
}

// tradedlg.cpp:4712
int FrTradeEditDlg::CheckTradeEdit(void) {
	memset(m_tradeTitle, 0, sizeof(m_tradeTitle));

	if (m_tradeItemList.size() <= 0) {
		return 8;
	}

	const char* title = WSingleton<CSharedDoc>::Instance()->GetTradeTitle();
	if (strlen(title) >= 0x20) {
		return 6;
	}

	char buf[0x100] = {0};
	strcpy(buf, title);

	if (WSingleton<CSharedDoc>::Instance()->m_chatManager.Filtering(buf)) {
		return 7;
	}

	int len = strlen(title);
	if (len == 0) {
		len = strlen(WSingleton<CSharedDoc>::Instance()->GetTradeTitle());
		if (len == 0) {
			return 5;
		}
	}

	WTL::CString strTitle(title);
	strTitle.TrimRight();
	if (strTitle.GetLength() == 0) {
		return 0x11;
	}

	if (m_pPackageSaleBtn->GetStatus() == FrButton::PRESSED) {
		if (FindIf(m_tradeItemList, CheckPackageSaleItem)) {
			return 0x1e;
		}
	}

	strcpy(m_tradeTitle, title);
	return 1;
}

// tradedlg.cpp:4769
void FrTradeEditDlg::UpdateTradeTitle(void) {
	if (m_pTitle) {
		m_pTitle->ClearLine();
		m_pTitle->SetLine(1, WSingleton<CSharedDoc>::Instance()->GetTradeTitle(), 0, false, 0);
	}
}

// tradedlg.cpp:4778
void FrTradeEditDlg::UpdateTradeTitleEdit(void) {
	if (m_pTitleEdit) {
		m_pTitleEdit->ClearLine();
		m_pTitleEdit->SetLine(1, WSingleton<CSharedDoc>::Instance()->GetTradeTitle(), 0, false, 0);
	}
}

// tradedlg.cpp:4788
eResultTrade FrTradeEditDlg::SendTradeTitle(void) {
	eResultTrade ret = RESULT_TRADE_OK;

	if (m_pTitleEdit) {
		WTL::CString strTitle(m_pTitleEdit->GetLine(1, false));
		strTitle.TrimRight();
		if (strTitle.GetLength() == 0) {
			return (eResultTrade)5;
		}

		const char* line = m_pTitleEdit->GetLine(1, false);
		int len = strlen(line);

		char buf[0x100] = {0};
		strcpy(buf, line);

		if (WSingleton<CSharedDoc>::Instance()->m_chatManager.Filtering(buf)) {
			ret = (eResultTrade)7;
		} else if (len > 0 && len < 0x20) {
			if (strcmp(line, WSingleton<CSharedDoc>::Instance()->GetTradeTitle()) != 0) {
				WSingleton<CSharedDoc>::Instance()->SendTradeTitle(std::string(line));
			}
		}

		m_pTitleEdit->ClearLine();
	}

	return ret;
}

// tradedlg.cpp:4826 (run-time type and factory registration of FrTradeFinishEditDlg)
const WRTTI FrTradeFinishEditDlg::m_RTTI("FrTradeFinishEditDlg", &FrForm::m_RTTI);

IObject* FrTradeFinishEditDlgMakeInstance(void) {
	return new FrTradeFinishEditDlg;
}

struct __sFrTradeFinishEditDlg {
	__sFrTradeFinishEditDlg(void) {
		ObjectFactory().AddObjectFunctor(FrTradeFinishEditDlgMakeInstance, "FrTradeFinishEditDlg");
	}
};
static __sFrTradeFinishEditDlg s_registerFrTradeFinishEditDlg;

// tradedlg.cpp:4828
const sFRESH_MSGMAP* FrTradeFinishEditDlg::GetMessageMap(void) const {
	return &FrTradeFinishEditDlg::_MsgMap;
}

sFRESH_MSGMAP FrTradeFinishEditDlg::_MsgMap = { &FrForm::_MsgMap, &FrTradeFinishEditDlg::_MsgEntries[0] };

sFRESH_ENTRY FrTradeFinishEditDlg::_MsgEntries[] = {
	// tradedlg.cpp:4830
	{ "tradefinisheditdlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeFinishEditDlg::OnTradeFinishEditDlgInit },
	// tradedlg.cpp:4831
	{ "trade_item_list", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeFinishEditDlg::OnTradeItemListInit },
	// tradedlg.cpp:4832
	{ "trade_item_list", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeFinishEditDlg::OnTradeItemListOwnerDraw },
	// tradedlg.cpp:4833
	{ "userid", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeFinishEditDlg::OnUserIdInit },
	// tradedlg.cpp:4834
	{ "trade_title", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeFinishEditDlg::OnTitleInit },
	// tradedlg.cpp:4835
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeFinishEditDlg::OnOkBnInit },
	// tradedlg.cpp:4836
	{ "ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeFinishEditDlg::OnOkBnUp },
	// tradedlg.cpp:4838
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// tradedlg.cpp:4841
FrTradeFinishEditDlg::FrTradeFinishEditDlg(void) {
	m_pTradeItemList = 0;
	m_pUserId = 0;
	m_pTitle = 0;

	m_pItemBaseN = g_pFresh->GetBitmap("item_base_n");
	m_pItemBaseO = g_pFresh->GetBitmap("item_base_o");
	m_pItemBaseD = g_pFresh->GetBitmap("item_base_d");
	m_pItemBaseEmpty = g_pFresh->GetBitmap("item_base_empty");

	m_pBmpUccOriginal = g_pFresh->RegisterBitmap("ucc_state_original");
	m_pBmpUccCopied = g_pFresh->RegisterBitmap("ucc_state_copied");
	m_pBmpUccTempSaved = g_pFresh->RegisterBitmap("ucc_state_temp_saved");

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = &data_a9ccf8[1];
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:,";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);

	memset(m_tradeTitle, 0, sizeof(m_tradeTitle));
	m_pListTradeItem = 0;
	m_tradeTitleSource = 0;

	m_packageSale = (eTypeSalesTrade)0;

	m_clockBtn.pBitmap = g_pFresh->GetBitmap("clock");
	m_clockBtn.rect = WRect(50.0f, 35.0f, m_clockBtn.pBitmap->Width(), m_clockBtn.pBitmap->Height());
	m_deleteBtn.pBitmap = g_pFresh->GetBitmap("delete");
	m_deleteBtn.rect = WRect(50.0f, 35.0f, m_deleteBtn.pBitmap->Width(), m_deleteBtn.pBitmap->Height());
}

// tradedlg.cpp:4887
FrTradeFinishEditDlg::~FrTradeFinishEditDlg(void) {
	if (m_pTitleFont) {
		m_pTitleFont->Erase();
	}

	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }
}

// tradedlg.cpp:4898
void FrTradeFinishEditDlg::OnTradeFinishEditDlgInit(int) {
}

// tradedlg.cpp:4903
void FrTradeFinishEditDlg::OnUserIdInit(int param) {
	m_pUserId = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:4909
void FrTradeFinishEditDlg::OnTitleInit(intptr_t param) {
	m_pTitle = DYNAMIC_CAST(FrEdit, param);
}

// tradedlg.cpp:4915
void FrTradeFinishEditDlg::OnOkBnInit(intptr_t param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:4921
void FrTradeFinishEditDlg::OnOkBnUp(void) {
	int result = 0;

	if (m_pListTradeItem->size() > 0 && m_pListTradeItem->size() <= 6
		&& m_tradeTitleSource > 0) {
		if (m_tradeTitleSource == 2) {
			WSingleton<CSharedDoc>::Instance()->SendTradeTitle(m_tradeTitle);
		}

		// 0x7C: the open-shop item list; packet.h has no enumerator for it.
		WSendPacket packet((enumClientPacket)0x7C);

		packet.Encode4(m_packageSale);

		packet.Encode4(m_pListTradeItem->size());

		int index = 0;
		for (std::list<sTradeItem>::iterator it = m_pListTradeItem->begin(); it != m_pListTradeItem->end(); ++it) {
			sTradeItem* pItem = &*it;
			pItem->index = index;
			packet.EncodeBuffer(pItem, 0xAA);
			index++;
		}

		packet.Send(SEND_TO_DEFAULT);
	} else {
		SetTradeResultCode(result, 225, 0);

		WSingleton<CSharedDoc>::Instance()->SendTradeCloseShop();
	}

	OnFreshOkay();
}

// tradedlg.cpp:4970
bool FrTradeFinishEditDlg::OnInit(void) {
	EnableDrag(false);

	return true;
}

// tradedlg.cpp:4979
void FrTradeFinishEditDlg::OnTradeItemListInit(int param) {
	m_pTradeItemList = DYNAMIC_CAST(FrListBox, param);
}

// tradedlg.cpp:4985
void FrTradeFinishEditDlg::OnTradeItemListOwnerDraw(int param) {
	FrListItem* pListItem = (FrListItem*)param;

	if (pListItem == 0) {
		return;
	}

	FrGraphicInterface* pGI = g_pFresh->GetManager()->GetGDI();
	if (pGI == 0) {
		return;
	}

	const Bitmap* pBaseBmp;
	if (pListItem->pData == 0) {
		pBaseBmp = m_pItemBaseEmpty;
	} else if (pListItem->selected) {
		pBaseBmp = m_pItemBaseD;
		pGI->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
	} else if (pListItem->underCursor) {
		pBaseBmp = m_pItemBaseO;
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	} else {
		pBaseBmp = m_pItemBaseN;
		pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);
	}

	pGI->SetTextStyle(0);

	if (pBaseBmp) {
		unsigned long color = 0xFFFFFFFF;
		if (m_packageSale == TYPE_SALES_PACKAGE) {
			color = 0x20FFFFFF;
		}

		pGI->DrawTexture(pBaseBmp, WRect(pListItem->pos.x, pListItem->pos.y, (float)pBaseBmp->Width(), (float)pBaseBmp->Height()), color, 0);
	}

	if (pListItem->pData == 0) {
		return;
	}

	sTradeItem item = *(sTradeItem*)pListItem->pData;
	IFF_ITEM_COMMON* pCommon = ItemManager()->FindCommonItem(item.typeId);

	if (pCommon == 0) {
		return;
	}

	unsigned long kind = item.typeId >> 26;
	if (kind == 0x12 && (((item.typeId >> 11) & 0x7FF) == 12 || ((item.typeId >> 11) & 0x7FF) == 11)) {
		return;
	}

	if (kind == 2) {
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(((item.typeId >> 18) & 0xFF) | 0x04000000);
		if (pChar != 0) {
			const Bitmap* pCharBmp = g_pFresh->GetBitmap(MakeStr("s_%s", pChar->m_icon));
			if (pCharBmp != 0) {
				WRect rect(pListItem->pos.x + 3.0f, pListItem->pos.y + 3.0f, (float)pCharBmp->Width(), (float)pCharBmp->Height());
				pGI->DrawTexture(pCharBmp, rect, 0xFFFFFFFF, 0);
			}
		}
	}

	const Bitmap* pIcon = g_pFresh->GetBitmap(pCommon->m_icon);

	if (pIcon) {
		float offX;
		float offY;

		switch (kind) {
		case 2:
			offX = 7.0f;
			offY = 2.0f;
			break;
		default:
			offX = 16.0f;
			offY = 27.0f;
			break;
		}

		if (IsLocalContent((localContentType_t)0x57) && (item.uccStatus & 1) && IsUccClothes(item.typeId)) {
			pIcon = GetUccItemIcon(item.guid);
		}

		if (pIcon) {
			WRect rect(offX + pListItem->pos.x, offY + pListItem->pos.y, (float)pIcon->Width(), (float)pIcon->Height());
			pGI->DrawTexture(pIcon, rect, 0xFFFFFFFF, 0);
		}

		if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(item.typeId)) {
			if ((item.uccStatus & 1) && item.uccSeq == 1 && m_pBmpUccOriginal) {
				pGI->DrawTexture(m_pBmpUccOriginal, WRect(pListItem->pos.x + 49.0f, pListItem->pos.y + 54.0f, (float)m_pBmpUccOriginal->Width(), (float)m_pBmpUccOriginal->Height()), 0xFFFFFFFF, 0);
			} else if ((item.uccStatus & 1) && item.uccSeq > 1 && m_pBmpUccCopied) {
				pGI->DrawTexture(m_pBmpUccCopied, WRect(pListItem->pos.x + 49.0f, pListItem->pos.y + 54.0f, (float)m_pBmpUccCopied->Width(), (float)m_pBmpUccCopied->Height()), 0xFFFFFFFF, 0);
			} else if ((item.uccStatus & 2) && m_pBmpUccTempSaved) {
				pGI->DrawTexture(m_pBmpUccTempSaved, WRect(pListItem->pos.x + 49.0f, pListItem->pos.y + 54.0f, (float)m_pBmpUccTempSaved->Width(), (float)m_pBmpUccTempSaved->Height()), 0xFFFFFFFF, 0);
			}
		}
	}

	if (IsLocalContent((localContentType_t)0x57) && (item.uccStatus & 1) && IsUccClothes(item.typeId)) {
		const char* pName = GetUccItemName(item.guid);

		pGI->SetTextStyle(1);
		pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 7.0f), 2, pName);
	} else {
		pGI->SetTextStyle(1);
		pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 7.0f), 2, pCommon->m_name);
	}
	pGI->SetTextStyle(0);

	char price[32] = { 0 };

	MakePriceStrA(price, item.price);
	SetTextColorByPrice(item.price);

	if ((int)item.count > 1) {
		const char* str = MakeStr("%d", item.count);
		float x = pListItem->pos.x + 54.0f;

		for (unsigned int i = 0; i < strlen(str); i++) {
			x -= m_pTitleFont->GetCharWidthA(g_view, str[i] - '0');
		}

		m_pTitleFont->Print(g_view, x, pListItem->pos.y + 55.0f, str, 0, 0xFFFFFFFF, 0);

		pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 23.0f), 2, MakeStr("\xB0\xB3\xB4\xE7 %s\xC6\xCE", price));
	} else {
		pGI->Print(WPoint(pListItem->pos.x + 182.0f, pListItem->pos.y + 23.0f), 2, MakeStr("%s\xC6\xCE", price));
	}

	pGI->SetTextColor(0xFF000000, 0xFFFFFFFF);

	DrawAbility(item, pListItem);

	switch (kind) {
	case 2:
	case 4:
	case 7:
	case 16:
	case 28: {
		const Bitmap* pLevel = g_pFresh->GetBitmap(MakeStr("level_%03d", (pCommon->m_level & 0x7F) + 1));
		if (pLevel) {
			WRect rcLevel(pListItem->pos.x + 40.0f - float2int(pLevel->Width() * 0.5f), pListItem->pos.y + 51.0f - float2int(pLevel->Height() * 0.5f), (float)pLevel->Width(), (float)pLevel->Height());

			pGI->DrawTexture(pLevel, rcLevel, 0xFFFFFFFF, 0);
		}
		break;
	}
	}
}

// tradedlg.cpp:5206
void FrTradeFinishEditDlg::SetTradeUserId(void) {
	if (m_pUserId) {
		m_pUserId->SetLine(1, "\xB3\xAA\xC0\xC7 \xBB\xF3\xC1\xA1", 0, false, 0);
	}
}

// tradedlg.cpp:5217
void FrTradeFinishEditDlg::SetTradeTitle(char* title) {
	if (title) {
		if (strlen(title) < 0x20) {
			m_tradeTitleSource = 0;
			int len = strlen(title);
			int docLen = strlen(WSingleton<CSharedDoc>::Instance()->GetTradeTitle());

			if (len <= 0 && docLen <= 0) {
				m_tradeTitleSource = 0;
				return;
			}

			if (len > 0 && len < 0x20) {
				strcpy(m_tradeTitle, title);
				m_tradeTitleSource = 2;
			} else if (docLen > 0 && docLen < 0x20) {
				strcpy(m_tradeTitle, WSingleton<CSharedDoc>::Instance()->GetTradeTitle());
				m_tradeTitleSource = 1;
			}

			if (m_pTitle) {
				m_pTitle->SetLine(1, m_tradeTitle, 0, false, 0);
			}
		}
	}
}

// tradedlg.cpp:5251
void FrTradeFinishEditDlg::SetListTradeItem(std::list<sTradeItem>* pList) {
	if (m_pTradeItemList) {
		m_pListTradeItem = pList;
		m_pTradeItemList->ClearItem();

		for (std::list<sTradeItem>::iterator it = m_pListTradeItem->begin(); it != m_pListTradeItem->end(); ++it) {
			m_pTradeItemList->AddItem(&*it);
		}
	}
}

// tradedlg.cpp:5263 (run-time type and factory registration of FrTradeWarningDlg)
const WRTTI FrTradeWarningDlg::m_RTTI("FrTradeWarningDlg", &FrForm::m_RTTI);

IObject* FrTradeWarningDlgMakeInstance(void) {
	return new FrTradeWarningDlg;
}

struct __sFrTradeWarningDlg {
	__sFrTradeWarningDlg(void) {
		ObjectFactory().AddObjectFunctor(FrTradeWarningDlgMakeInstance, "FrTradeWarningDlg");
	}
};
static __sFrTradeWarningDlg s_registerFrTradeWarningDlg;

// tradedlg.cpp:5265
const sFRESH_MSGMAP* FrTradeWarningDlg::GetMessageMap(void) const {
	return &FrTradeWarningDlg::_MsgMap;
}

sFRESH_MSGMAP FrTradeWarningDlg::_MsgMap = { &FrForm::_MsgMap, &FrTradeWarningDlg::_MsgEntries[0] };

sFRESH_ENTRY FrTradeWarningDlg::_MsgEntries[] = {
	// tradedlg.cpp:5267
	{ "TradeWarningDlg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeWarningDlg::OnTradeWarningDlgInit },
	// tradedlg.cpp:5268
	{ "msg01", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeWarningDlg::OnMsg01Init },
	// tradedlg.cpp:5269
	{ "msg02", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeWarningDlg::OnMsg02Init },
	// tradedlg.cpp:5270
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeWarningDlg::OnOkBnInit },
	// tradedlg.cpp:5271
	{ "ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeWarningDlg::OnOkBnUp },
	// tradedlg.cpp:5272
	{ "cancel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeWarningDlg::OnCancelBnInit },
	// tradedlg.cpp:5273
	{ "cancel", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeWarningDlg::OnCancelBnUp },
	// tradedlg.cpp:5275
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// tradedlg.cpp:5278
FrTradeWarningDlg::FrTradeWarningDlg(void)
	: m_pMsg01(0)
	, m_pMsg02(0) {
	m_warningType = 1;
}

// tradedlg.cpp:5286
FrTradeWarningDlg::~FrTradeWarningDlg(void) {
}

// tradedlg.cpp:5291
void FrTradeWarningDlg::OnTradeWarningDlgInit(int) {
}

// tradedlg.cpp:5296
void FrTradeWarningDlg::OnMsg01Init(int param) {
	m_pMsg01 = DYNAMIC_CAST(FrStatic, param);
}

// tradedlg.cpp:5302
void FrTradeWarningDlg::OnMsg02Init(int param) {
	m_pMsg02 = DYNAMIC_CAST(FrStatic, param);
}

// tradedlg.cpp:5308
void FrTradeWarningDlg::OnOkBnInit(intptr_t param) {
	m_pOkBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:5314
void FrTradeWarningDlg::OnOkBnUp(void) {
	switch (m_warningType) {
	case 0x12:
	case 0x13: {
		WSingleton<CSharedDoc>::Instance()->SendTradeCloseShop();

		IActor* pActor = AfxGetTask()->GetActor(s_gatewayActorName);
		if (pActor) {
			pActor << MsgObject(0, 0x273, 5, 0, 0, 0, 0);
		}
		break;
	}
	case 0x14: {
		FrTradeEditDlg* pDlg = FrStaticCast<FrTradeEditDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradeeditdlg"));

		if (pDlg) {
			pDlg->CancelSelectedTradeItem();
		}
		break;
	}
	}

	OnFreshOkay();
}

// tradedlg.cpp:5355
void FrTradeWarningDlg::OnCancelBnInit(int param) {
	m_pCancelBn = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:5361
void FrTradeWarningDlg::OnCancelBnUp(void) {
	OnFreshCancel();
}

// tradedlg.cpp:5367
void FrTradeWarningDlg::SetControls(int type) {
	if (type == 0) {
		m_pCancelBn->SetVisible(false);
		m_pOkBn->MoveWindow(WPoint(m_rect.x + 92.0f, m_rect.y + 137.0f));
	} else {
		m_pOkBn->SetVisible(true);
		m_pCancelBn->SetVisible(true);
		m_pOkBn->MoveWindow(WPoint(m_rect.x + 50.0f, m_rect.y + 137.0f));
		m_pCancelBn->MoveWindow(WPoint(m_rect.x + 135.0f, m_rect.y + 137.0f));
	}
}

// tradedlg.cpp:5384
void FrTradeWarningDlg::SetWarningMessageType(int type, unsigned char subType) {
	WPoint pos;
	m_warningType = type;
	m_pMsg01->SetTextColor(0xFF000000, 0xFFFFFFFF);

	switch (m_warningType) {
	case 0:
		m_pMsg01->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		m_pMsg01->SetCaption("\xbf\xc0\xb7\xf9");
		m_pMsg02->SetCaption("\xb0\xfc\xb7\xc3\xb5\xc8 \xb5\xa5\xc0\xcc\xc5\xcd\xb0\xa1 \xc3\xca\xb1\xe2\xc8\xad\xb5\xcb\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 2:
		m_pMsg01->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		m_pMsg01->SetCaption("\xbf\xc0\xb7\xf9");
		m_pMsg02->SetCaption("\xc0\xcc\xb9\xcc \xbb\xf3\xc1\xa1\xc0\xcc \xb0\xb3\xbc\xb3\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 3:
		m_pMsg01->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		m_pMsg01->SetCaption("\xbb\xf3\xc1\xa1\xc0\xcc \xb4\xdd\xc7\xfb\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb4\xd9\xc0\xbd \xb1\xe2\xc8\xb8\xbf\xa1 \xc0\xcc\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 4:
		m_pMsg01->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		m_pMsg01->SetCaption("\xbb\xf3\xc1\xa1\xc0\xcc \xc6\xed\xc1\xfd\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb0\xb3\xbc\xb3 \xc8\xc4 \xc0\xcc\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 5:
		m_pMsg01->SetCaption("\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xb8\xed\xc0\xcc \xc0\xd4\xb7\xc2\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xc0\xd4\xb7\xc2 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 6:
		m_pMsg01->SetCaption("\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xb8\xed\xc0\xcc \xc0\xdf\xb8\xf8\xc0\xd4\xb7\xc2\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xc1\xa4\xbb\xf3 \xc0\xd4\xb7\xc2 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 17:
		m_pMsg01->SetCaption("\xbb\xf3\xc1\xa1\xb8\xed\xc0\xbb \xc0\xd4\xb7\xc2\xc7\xcf\xbd\xc5 \xc8\xc4 \xc8\xae\xc0\xce \xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf\xbc\xad");
		m_pMsg02->SetCaption("\xbb\xe7\xbf\xeb \xb0\xa1\xb4\xc9\xc7\xd1 \xbb\xf3\xc1\xa1\xb8\xed\xc0\xce\xc1\xf6 \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 7:
		m_pMsg01->SetCaption("\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xb8\xed\xbf\xa1 \xc0\xfb\xb4\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba");
		m_pMsg02->SetCaption("\xb4\xdc\xbe\xee\xb0\xa1 \xc6\xf7\xc7\xd4\xb5\xc7\xbe\xee\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 8:
		m_pMsg01->SetCaption("\xb5\xee\xb7\xcf\xc7\xcf\xbd\xc5 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xc0\xbb \xb0\xb3\xbc\xb3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 9:
		m_pMsg01->SetCaption("\xbc\xd2\xc0\xaf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xc0\xbb \xb0\xb3\xbc\xb3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 10:
		m_pMsg01->SetCaption("\xbe\xc6\xc0\xcc\xc5\xdb \xbc\xf6\xb7\xae\xc0\xcc \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb4\xd9\xbd\xc3 \xc7\xd1\xb9\xf8 \xc8\xae\xc0\xce\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 11:
		m_pMsg01->SetCaption("\xbe\xc6\xc0\xcc\xc5\xdb \xb0\xa1\xb0\xdd\xc0\xcc \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb4\xd9\xbd\xc3 \xc7\xd1\xb9\xf8 \xc8\xae\xc0\xce\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 12:
		m_pMsg01->SetCaption("\xbe\xc6\xc0\xcc\xc5\xdb \xb0\xa1\xb0\xdd\xc0\xcc 3\xc3\xb5\xb8\xb8\xc6\xce \xc1\xa6\xc7\xd1\xc0\xbb \xb3\xd1\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb4\xd9\xbd\xc3 \xc7\xd1\xb9\xf8 \xc8\xae\xc0\xce\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		return;
	case 13:
		m_pMsg01->SetCaption("\xba\xbb \xbb\xf3\xc7\xb0\xc0\xba \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb4\xd9\xb8\xa5 \xbb\xf3\xc7\xb0\xc0\xbb \xb1\xb8\xb8\xc5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 14:
		m_pMsg01->SetCaption("\xbb\xf3\xc1\xa1 \xc0\xcc\xbf\xeb\xc0\xda\xb0\xa1 \xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 18:
		m_pMsg01->SetCaption("\xb5\xee\xb7\xcf\xb5\xc8 \xb8\xf0\xb5\xe7 \xb9\xb0\xc7\xb0\xc0\xcc \xc3\xeb\xbc\xd2\xb5\xcb\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xc0\xbb \xb4\xdd\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?");
		SetControls(1);
		return;
	case 19:
		m_pMsg01->SetCaption("\xb8\xf0\xb5\xe7 \xbb\xf3\xc7\xb0\xc0\xcc \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1\xc0\xbb \xb4\xdd\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?");
		SetControls(1);
		return;
	case 20:
		m_pMsg01->SetCaption("\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xc5 \xb9\xb0\xc7\xb0\xc0\xbb \xb5\xee\xb7\xcf");
		m_pMsg02->SetCaption("\xc3\xeb\xbc\xd2\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?");
		SetControls(1);
		return;
	case 21:
		m_pMsg01->SetCaption("\xbc\xd2\xc1\xf6\xc7\xcf\xbd\xc5 \xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb9\xb0\xc7\xb0\xc0\xbb \xb1\xb8\xb8\xc5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 22:
		m_pMsg01->SetCaption("\xb8\xf0\xb5\xe7 \xbb\xf3\xc7\xb0\xc0\xcc \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xc5\xbc\xad \xb0\xa8\xbb\xe7\xc7\xd5\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 15:
		m_pMsg01->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		if (subType) {
			m_pMsg01->SetCaption("\xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1 \xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
		} else {
			m_pMsg01->SetCaption("\xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7\xc1\xdf\xc0\xce \xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xd4\xb4\xcf\xb4\xd9.");
		}
		m_pMsg02->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		m_pMsg02->SetCaption("\xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 24:
		m_pMsg01->SetCaption("\xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb7\xc1\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb3\xca\xb9\xab \xb8\xb9\xc0\xcc \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb4\xf5 \xc0\xcc\xbb\xf3 \xb1\xb8\xb8\xc5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 16:
		m_pMsg01->SetTextColor(0xFFFF0000, 0xFFFFFFFF);
		m_pMsg01->SetCaption("\xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 25:
		m_pMsg01->SetCaption("\xc7\xf6\xc0\xe7 \xc4\xab\xb5\xe5\xb0\xa1 \xc0\xce\xc3\xa6\xc6\xae\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba");
		m_pMsg02->SetCaption("\xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 26:
		m_pMsg01->SetCaption("\xc3\xa4\xb3\xce\xbf\xa1 \xc0\xa7\xc4\xa1\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 27:
		m_pMsg01->SetCaption("\xb4\xeb\xc8\xad\xb9\xe6\xbf\xa1 \xc0\xa7\xc4\xa1\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 28:
		m_pMsg01->SetCaption("\xb4\xf5 \xc0\xcc\xbb\xf3 \xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xbb \xb8\xb8\xb5\xe5\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pMsg02->SetCaption("\xb4\xd9\xb8\xa5 \xb4\xeb\xc8\xad\xb9\xe6\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 29:
		m_pMsg01->SetCaption("\xb9\xad\xc0\xbd\xc6\xc7\xb8\xc5\xb7\xce \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	case 30:
		m_pMsg01->SetCaption("\xb9\xad\xc0\xbd\xc6\xc7\xb8\xc5\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd1");
		m_pMsg02->SetCaption("\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc6\xf7\xc7\xd4\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pCancelBn->SetVisible(false); pos.x = m_rect.x + 92.0f; pos.y = m_rect.y + 137.0f; m_pOkBn->MoveWindow(pos);
		break;
	default:
		OnFreshCancel();
		break;
	}
}

// tradedlg.cpp:5587
void FrTradeWarningDlg::SetWarningMessageForPackage(int code) {
	if (code >= 1900001 && code <= 1900039) {
		m_pMsg01->SetCaption(MakeStr("\xB9\xAD\xC0\xBD\xC6\xC7\xB8\xC5 \xBF\xC0\xB7\xF9 [%d]", code));
		SetControls(0);
	} else {
		SetWarningMessageType(code, 0);
	}
}

// tradedlg.cpp:5603 (run-time type and factory registration of FrTradeLevelLimitDlg)
const WRTTI FrTradeLevelLimitDlg::m_RTTI("FrTradeLevelLimitDlg", &FrForm::m_RTTI);

IObject* FrTradeLevelLimitDlgMakeInstance(void) {
	return new FrTradeLevelLimitDlg;
}

struct __sFrTradeLevelLimitDlg {
	__sFrTradeLevelLimitDlg(void) {
		ObjectFactory().AddObjectFunctor(FrTradeLevelLimitDlgMakeInstance, "FrTradeLevelLimitDlg");
	}
};
static __sFrTradeLevelLimitDlg s_registerFrTradeLevelLimitDlg;

// tradedlg.cpp:5605
const sFRESH_MSGMAP* FrTradeLevelLimitDlg::GetMessageMap(void) const {
	return &FrTradeLevelLimitDlg::_MsgMap;
}

sFRESH_MSGMAP FrTradeLevelLimitDlg::_MsgMap = { &FrForm::_MsgMap, &FrTradeLevelLimitDlg::_MsgEntries[0] };

sFRESH_ENTRY FrTradeLevelLimitDlg::_MsgEntries[] = {
	// tradedlg.cpp:5607
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrTradeLevelLimitDlg::OnOkBnInit },
	// tradedlg.cpp:5608
	{ "ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrTradeLevelLimitDlg::OnOkBnUp },

	// tradedlg.cpp:5610
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// tradedlg.cpp:5613
FrTradeLevelLimitDlg::FrTradeLevelLimitDlg(void) {
}

// tradedlg.cpp:5617
FrTradeLevelLimitDlg::~FrTradeLevelLimitDlg(void) {
}

// tradedlg.cpp:5621
void FrTradeLevelLimitDlg::OnOkBnInit(intptr_t param) {
	FrButton* pButton = DYNAMIC_CAST(FrButton, param);
}

// tradedlg.cpp:5626
void FrTradeLevelLimitDlg::OnOkBnUp(void) {
	OnFreshOkay();
}

// tradedlg.cpp:5631
bool FrTradeLevelLimitDlg::OnInit(void) {
	return true;
}

// tradedlg.cpp:5637
void SetTradeResultCode(int code, int msgId, sTradeItem* pItem) {
	if (code == 15) {
		FrTradeWarningDlg* pWarn = FrStaticCast<FrTradeWarningDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradewarningdlg"));
		if (!pWarn) {
			FrTradeWarningDlg* pDlg = CreateForm<FrTradeWarningDlg>(g_pFresh->GetManager(), 0, "tradewarningdlg", 0);
			if (pDlg) {
				pDlg->Open((FRESH_RESULT_CALLBACK)0, 1);
				pDlg->SetWarningMessageType(15, (unsigned char)msgId);
				pDlg->SetFadeout(false);
			}
		}
	}
	else if (code == 1) {
		if (msgId == 234) {
			if (IsLocalContent((localContentType_t)0x71) == true && WSingleton<CItemStorage>::Instance()) {
				if (WSingleton<CItemStorage>::Instance()->IsVisible()) {
					return;
				}
			}

			FrTradeResultDlg* pDlg = CreateForm<FrTradeResultDlg>(g_pFresh->GetManager(), 0, "traderesultdlg", 0);
			if (pDlg) {
				pDlg->Open((FRESH_RESULT_CALLBACK)0, 1);
				pDlg->SetFadeout(false);
				pDlg->SetResultItem(pItem);
			}
		}
	}
	else if (code == 23) {
		FrTradeLevelLimitDlg* pDlg = CreateForm<FrTradeLevelLimitDlg>(g_pFresh->GetManager(), 0, "tradelevellimitdlg", 0);
		if (pDlg) {
			pDlg->Open((FRESH_RESULT_CALLBACK)0, 0x40);
		}
	}
	else {
		FrForm* pResult = FrStaticCast<FrForm>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("traderesultdlg"));
		if (!pResult) {
			FrTradeWarningDlg* pWarn = FrStaticCast<FrTradeWarningDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradewarningdlg"));
			if (!pWarn) {
				FrTradeWarningDlg* pDlg = CreateForm<FrTradeWarningDlg>(g_pFresh->GetManager(), 0, "tradewarningdlg", 0);
				if (pDlg) {
					pDlg->Open((FRESH_RESULT_CALLBACK)0, 1);
					pDlg->SetWarningMessageType(code, 0);
					pDlg->SetFadeout(false);
				}
			}
		}
		else {
			switch (code) {
			case 3:
				WSingleton<CChatMsg>::Instance()->AddChatMsg(std::string("\xBB\xF3\xC1\xA1\xC0\xCC \xB4\xDD\xC7\xFB\xBD\xC0\xB4\xCF\xB4\xD9. \xB4\xD9\xC0\xBD \xB1\xE2\xC8\xB8\xBF\xA1 \xC0\xCC\xBF\xEB\xC7\xD8\xC1\xD6\xBC\xBC\xBF\xE4."), 0xFFFF, false, false);
				break;
			case 4:
				WSingleton<CChatMsg>::Instance()->AddChatMsg(std::string("\xBB\xF3\xC1\xA1\xC0\xCC \xC6\xED\xC1\xFD\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9. \xB0\xB3\xBC\xB3 \xC8\xC4 \xC0\xCC\xBF\xEB\xC7\xD8\xC1\xD6\xBC\xBC\xBF\xE4."), 0xFFFF, false, false);
				break;
			}
		}
	}
}
