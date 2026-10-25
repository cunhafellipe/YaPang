// JP-only game code: FrPointShopDlg, the Tiki point shop, and
// FrPointShopMessageDlg, its result popup. See JpPointShopDlg.h.
#include "JpPointShopDlg.h"
#include "JpLobbyGlue.h"

#include "ObjectFactory.h"
#include "ItemManager.h"
#include "Packet.h"
#include "RFTextManager.h"
#include "TaskManager.h"
#include "TaskMain.h"
#include "actor.h"
#include "classdefine.h"
#include "frscrollbar.h"
#include "SharedDoc.h"
#include "TitleFontEx.h"
#include "commonutil.h"
#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "fresh.h"
#include "frlistbox.h"
#include "frgraphicinterface.h"
#include "frviewer.h"
#include "frwnd.h"
#include "frwndmanager.h"
#include "bitmap.h"

#include <string.h>

// lib: 0x8526d0 (CPointShop deleting destructor), 0x84e7a0 / 0x84e900 /
// 0x84f9c0 (std::map clear / erase / copy), 0x850a10, 0x850a20, 0x84dee0, 0x84def0, 0x84bf80, 0x84bfa0
// (this-adjusting deleting-destructor thunks), 0xcde0b0, 0xcddf00,
// 0xcdde70, 0xcddf90 (message-map initializers), 0x84c050 / 0xcdde50 and
// 0x84c210 / 0xcddf70 (FrPointShopHelp / FrPointShopNoticeDlg factory
// registrations)

// --- FrPointShopHelp --------------------------------------------------------

const WRTTI FrPointShopHelp::m_RTTI("FrPointShopHelp", &FrForm::m_RTTI);

// 0x84b8d0
const sFRESH_MSGMAP* FrPointShopHelp::GetMessageMap(void) const {
	return &FrPointShopHelp::_MsgMap;
}

sFRESH_MSGMAP FrPointShopHelp::_MsgMap = { &FrForm::_MsgMap, &FrPointShopHelp::_MsgEntries[0] };

sFRESH_ENTRY FrPointShopHelp::_MsgEntries[] = {
	{ "desc_view", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopHelp::OnDescViewInit },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x84b8e0
FrPointShopHelp::FrPointShopHelp(void) {
	m_pDescView = 0;
}

// 0x84b920
FrPointShopHelp::~FrPointShopHelp(void) {
}

// --- FrPointShopNoticeDlg ---------------------------------------------------

const WRTTI FrPointShopNoticeDlg::m_RTTI("FrPointShopNoticeDlg", &FrForm::m_RTTI);

// 0x84b960
const sFRESH_MSGMAP* FrPointShopNoticeDlg::GetMessageMap(void) const {
	return &FrPointShopNoticeDlg::_MsgMap;
}

sFRESH_MSGMAP FrPointShopNoticeDlg::_MsgMap = { &FrForm::_MsgMap, &FrPointShopNoticeDlg::_MsgEntries[0] };

sFRESH_ENTRY FrPointShopNoticeDlg::_MsgEntries[] = {
	{ "background", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopNoticeDlg::OnBackgroundInit },
	{ "tag", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopNoticeDlg::OnTagInit },
	{ "tag", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrPointShopNoticeDlg::OnTagOwnerDraw },
	{ "desc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopNoticeDlg::OnDescInit },
	{ "Apply", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopNoticeDlg::OnApplyClick },
	{ "Exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopNoticeDlg::OnExitClick },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x84b970
FrPointShopNoticeDlg::FrPointShopNoticeDlg(void) {
	m_pExchange = 0;
	m_pTag = 0;
	m_pDesc = 0;
	m_pTec1Bmp = g_pFresh->GetBitmap("tikipoint_popup_tec1");
	m_pTec2Bmp = g_pFresh->GetBitmap("tikipoint_popup_tec2");
	m_pBgBmp = g_pFresh->GetBitmap("tikipoint_popup_bg");
}

// 0x84ba30
FrPointShopNoticeDlg::~FrPointShopNoticeDlg(void) {
	m_pTec1Bmp = 0;
	m_pTec2Bmp = 0;
	m_pBgBmp = 0;
	m_pExchange = 0;
}

// 0x84ba70
void FrPointShopNoticeDlg::OnExitClick(void) {
	Close(true);
}

// 0x84bfe0
IObject* FrPointShopHelpMakeInstance(void) {
	return new FrPointShopHelp;
}

struct __sFrPointShopHelp {
	__sFrPointShopHelp(void) {
		ObjectFactory().AddObjectFunctor(FrPointShopHelpMakeInstance, "FrPointShopHelp");
	}
};
static __sFrPointShopHelp s_registerFrPointShopHelp;

// 0x84c0a0
void FrPointShopHelp::OnDescViewInit(PG_MSGARG param) {
	m_pDescView = DYNAMIC_CAST(FrViewer, param);
	m_pDescView->ShowScrollBar(true);
	m_pDescView->Open("pointshop_explanation.jpg");
}

// 0x84c1a0
IObject* FrPointShopNoticeDlgMakeInstance(void) {
	return new FrPointShopNoticeDlg;
}

struct __sFrPointShopNoticeDlg {
	__sFrPointShopNoticeDlg(void) {
		ObjectFactory().AddObjectFunctor(FrPointShopNoticeDlgMakeInstance, "FrPointShopNoticeDlg");
	}
};
static __sFrPointShopNoticeDlg s_registerFrPointShopNoticeDlg;

// 0x84c260
void FrPointShopNoticeDlg::OnBackgroundInit(PG_MSGARG param) {
	m_pBackground = DYNAMIC_CAST(FrArea, param);
}

// 0x84c2d0
void FrPointShopNoticeDlg::OnTagInit(PG_MSGARG param) {
	m_pTag = DYNAMIC_CAST(FrArea, param);
}

// 0x84c340
void FrPointShopNoticeDlg::OnTagOwnerDraw(void) {
	if (m_pTag == 0 || m_pExchange == 0) {
		return;
	}
	const Bitmap* bmp;
	int mode = m_pExchange->GetMode();
	if (mode == 1) {
		bmp = m_pTec1Bmp;
	} else if (mode == 2) {
		bmp = m_pTec2Bmp;
	} else {
		return;
	}
	if (bmp == 0) {
		return;
	}
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi) {
		WRect rect = m_pTag->GetRect();
		rect.w = (float)bmp->Width();
		rect.h = (float)bmp->Height();
		gdi->DrawTexture(bmp, rect, 0xffffffff, 0);
	}
}

// 0x84c410
void FrPointShopNoticeDlg::OnDescInit(PG_MSGARG param) {
	m_pDesc = DYNAMIC_CAST(FrEdit, param);
}

// 0x84d790
FrForm* CreatePointShopHelp(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrPointShopHelp>(manager, owner, name, parent);
}

// 0x84d860
FrForm* CreatePointShopNoticeDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrPointShopNoticeDlg>(manager, owner, name, parent);
}

// --- FrPointShopMessageDlg -------------------------------------------------

const WRTTI FrPointShopMessageDlg::m_RTTI("FrPointShopMessageDlg", &FrForm::m_RTTI);

// 0x84dac0
void FrPointShopNoticeDlg::OnApplyClick(void) {
	if (m_pExchange == 0) {
		Close(true);
		return;
	}
	if (m_pExchange->GetMode() == 2 && m_pExchange->GetTotal() > JpSharedDoc_GetTikiPoints()) {
		Close(true);
		// 0xd31548, EUC-KR: "the points needed for the purchase are not
		// enough", translated by the JP text lookup 0xb4eb80.
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2e1,
			(intptr_t)Jp_LookupText("\xb1\xb8\xb8\xc5\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 "
				"\xc6\xf7\xc0\xce\xc6\xae\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9."),
			0, 0, 0, 0));
		return;
	}
	m_pExchange->Send();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	Close(true);
}

// 0x84fbd0
// JP 0xa18400 = KR CItemManager::GetItemName (the record's +0x8), 0xa17cf0
// = FindCommonItem (the icon name at JP +0x49 = KR m_icon). The texts are
// EUC-KR keys of the JP text lookup 0xb4eb80: "do you want to buy it?",
// "exchange for %d points in total?" and "[note] exchanged items cannot be
// restored.". Mode 1 also calls GetExchangePoint per entry and drops the
// result.
void FrPointShopNoticeDlg::OnDraw(void) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0) {
		return;
	}
	WRect rect = GetRect();
	if (m_pBackground && m_pBgBmp) {
		gdi->DrawTexture(m_pBgBmp, rect, 0xffffffff, 0);
	}
	if (m_pExchange == 0) {
		return;
	}
	switch (m_pExchange->GetMode()) {
	case 1: {
		gdi->SetTextColor(0xffa34706, 0xffffffff);
		gdi->SetTextStyle(1);
		CPointShop::EntryMap entries = m_pExchange->GetEntries();
		float y = (float)(5 - static_cast<uint32_t>(entries.size())) * 6.0 + 75.0;
		for (CPointShop::EntryMap::iterator it = entries.begin(); it != entries.end(); ++it) {
			uint32_t typeId = it->second.typeId;
			uint32_t count = it->second.count;
			const char* name = ItemManager()->GetItemName(typeId);
			int maxCount = m_pExchange->GetMaxCount(typeId);
			m_pExchange->GetExchangePoint(typeId);
			WPoint pos(rect.x + 127.0, rect.y + y);
			gdi->Print(pos, 1, MakeStr("%s(%d) x %d", name, maxCount, count));
			y += 15.0;
		}
		gdi->SetTextColor(0xff8a19a5, 0xffffffff);
		gdi->SetTextStyle(2);
		int total = m_pExchange->GetTotal();
		WPoint totalPos(rect.x + 127.0, rect.y + 180.0);
		gdi->Print(totalPos, 1, MakeStr(Jp_LookupText("\xc3\xd1 %d\xc6\xf7\xc0\xce\xc6\xae\xb7\xce "
			"\xb1\xb3\xc8\xaf \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?"), total));
		gdi->SetTextColor(0xffa34706, 0xffffffff);
		gdi->SetTextStyle(0);
		WPoint notePos(rect.x + 127.0, rect.y + 230.0);
		gdi->Print(notePos, 1, Jp_LookupText("[\xc1\xd6\xc0\xc7] \xb1\xb3\xc8\xaf\xc7\xd1 "
			"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xba\xb9\xbf\xf8\xc7\xd2 \xbc\xf6 "
			"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9."));
		break;
	}
	case 2: {
		gdi->SetTextColor(0xffa34706, 0xffffffff);
		gdi->SetTextStyle(1);
		CPointShop::EntryMap entries = m_pExchange->GetEntries();
		for (CPointShop::EntryMap::iterator it = entries.begin(); it != entries.end(); ++it) {
			uint32_t typeId = it->second.typeId;
			const char* name = ItemManager()->GetItemName(typeId);
			const JpPointShopGoods* goods = JpItemManager_FindGoods(typeId);
			if (goods == 0) {
				continue;
			}
			const IFF_STRUCT::sItem* record =
				reinterpret_cast<const IFF_STRUCT::sItem*>(JpItemManager_FindCommonItem(typeId));
			if (record == 0) {
				continue;
			}
			const Bitmap* bmp = g_pFresh->GetBitmap(record->m_icon);
			if (bmp) {
				WRect iconRect(rect.x + 95.0, rect.y + 60.0, (float)bmp->Width(), (float)bmp->Height());
				gdi->DrawTexture(bmp, iconRect, 0xffffffff, 0);
			}
			WPoint pos(rect.x + 127.0, rect.y + 120.0);
			gdi->Print(pos, 1, MakeStr("%s(%d) : %dTP", name, JpPointShopGoods_GetCount(goods),
				JpPointShopGoods_GetPrice(goods)));
		}
		gdi->SetTextColor(0xff8a19a5, 0xffffffff);
		gdi->SetTextStyle(2);
		int total = m_pExchange->GetTotal();
		WPoint totalPos(rect.x + 127.0, rect.y + 180.0);
		gdi->Print(totalPos, 1, MakeStr(Jp_LookupText("\xb1\xb8\xb8\xc5 \xc7\xcf\xbd\xc3"
			"\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?"), total));
		break;
	}
	default:
		break;
	}
}

// 0x84b940
const sFRESH_MSGMAP* FrPointShopMessageDlg::GetMessageMap(void) const {
	return &FrPointShopMessageDlg::_MsgMap;
}

sFRESH_MSGMAP FrPointShopMessageDlg::_MsgMap = { &FrForm::_MsgMap, &FrPointShopMessageDlg::_MsgEntries[0] };

sFRESH_ENTRY FrPointShopMessageDlg::_MsgEntries[] = {
	{ "background", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopMessageDlg::OnBackgroundInit },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x84b950
void FrPointShopMessageDlg::SetSuccess(int success) {
	m_success = success;
}

// 0x84c130
void FrPointShopMessageDlg::OnBackgroundInit(PG_MSGARG param) {
	m_pBackground = DYNAMIC_CAST(FrArea, param);
}

// 0x84d9d0
// The JP 0xbed280 is KR FrGraphicInterface::Print (cdecl, the text passed
// as the format), 0xbec440 / 0xbec460 KR SetTextColor / SetTextStyle.
void FrPointShopMessageDlg::OnDraw(void) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0) {
		return;
	}
	WRect rect = GetRect();
	if (m_pBackground) {
		const Bitmap* bmp = m_success ? m_pSuccessBg : m_pFailBg;
		if (bmp) {
			gdi->DrawTexture(bmp, rect, 0xffffffff, 0);
		}
	}
	gdi->SetTextColor(0xff000000, 0xffffffff);
	gdi->SetTextStyle(2);
	gdi->SetTextColor(0xffff0000, 0xffffffff);
	WPoint pos(rect.x + 127.0f, rect.y + 116.0f);
	gdi->Print(pos, 1, m_message.c_str());
}

// 0x84de70
FrPointShopMessageDlg::~FrPointShopMessageDlg(void) {
	m_pSuccessBg = 0;
	m_pFailBg = 0;
}

// 0x84e820
FrPointShopMessageDlg::FrPointShopMessageDlg(void) {
	m_success = 1;
	m_pSuccessBg = g_pFresh->GetBitmap("tikipoint_popup_bg");
	m_pFailBg = g_pFresh->GetBitmap("tikipoint_popup_fail");
}

// 0x84e8d0: std::string::assign(text, strlen(text)).
void FrPointShopMessageDlg::SetMessage(const char* text) {
	if (text) {
		m_message.assign(text, strlen(text));
	}
}

// 0x84ea10
IObject* FrPointShopMessageDlgMakeInstance(void) {
	return new FrPointShopMessageDlg;
}

struct __sFrPointShopMessageDlg {
	__sFrPointShopMessageDlg(void) {
		ObjectFactory().AddObjectFunctor(FrPointShopMessageDlgMakeInstance, "FrPointShopMessageDlg");
	}
};
static __sFrPointShopMessageDlg s_registerFrPointShopMessageDlg;

// 0x655680
FrForm* CreatePointShopMessageDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrPointShopMessageDlg>(manager, owner, name, parent);
}

// 0x65f2a3 (inline in the JP CTaskMain::HandleMsgCommon 0x65d8e0, case
// 0x2e1): the JP owner is the task's FrCmdTarget base (+0x18).
void TaskMain_OpenPointShopMessageDlg(CTaskMain* task, const char* text, int success) {
	FrPointShopMessageDlg* dlg = static_cast<FrPointShopMessageDlg*>(
		CreatePointShopMessageDlg(g_pFresh->GetManager(), task, "PointShopMessageDlg", NULL));
	dlg->SetMessage(text);
	dlg->SetSuccess(success);
	dlg->Open(NULL, 3);
	dlg->SetTopmost(true);
}

// --- FrPointShopDlg ---------------------------------------------------------

const WRTTI FrPointShopDlg::m_RTTI("FrPointShopDlg", &FrForm::m_RTTI);

// 0x84ba80
const sFRESH_MSGMAP* FrPointShopDlg::GetMessageMap(void) const {
	return &FrPointShopDlg::_MsgMap;
}

sFRESH_MSGMAP FrPointShopDlg::_MsgMap = { &FrForm::_MsgMap, &FrPointShopDlg::_MsgEntries[0] };

sFRESH_ENTRY FrPointShopDlg::_MsgEntries[] = {
	{ "MyPoint", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnMyPointInit },
	{ "MyPoint", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnMyPointOwnerDraw },
	{ "PointShop_Exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExitInit },
	{ "PointShop_Exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnFreshOkay },
	{ "PointShop_Main", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnMainInit },
	{ "PointShop_Main", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnMainClick },
	{ "PointShop_Help", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnHelpInit },
	{ "PointShop_Help", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnHelpClick },
	{ "GotoMainBg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnGotoMainBgInit },
	{ "Intro_bg1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnIntroBg1Init },
	{ "Intro_bg2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnIntroBg2Init },
	{ "Intro_notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnIntroNoticeInit },
	{ "Intro_notice", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnIntroNoticeOwnerDraw },
	{ "Exchange", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExchangeInit },
	{ "Exchange", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnExchangeClick },
	{ "Buy", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnBuyInit },
	{ "Buy", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnBuyClick },
	{ "Exchange_ListBG1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExchangeListBg1Init },
	{ "Exchange_ListBG2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExchangeListBg2Init },
	{ "Exchange_ListBG3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExchangeListBg3Init },
	{ "Exchange_ListBG4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExchangeListBg4Init },
	{ "Exchange_ListBG5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExchangeListBg5Init },
	{ "Exchange_ListBG6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExchangeListBg6Init },
	{ "PrePoint", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnPrePointInit },
	{ "PrePoint", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnPrePointOwnerDraw },
	{ "Exchange_ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExchangeOkInit },
	{ "Exchange_ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnExchangeOkClick },
	{ "Own_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnOwnItemInit },
	{ "Own_item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnOwnItemOwnerDraw },
	{ "Own_item", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnOwnItemClick },
	{ "Exchange_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExchangeItemInit },
	{ "Exchange_item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnExchangeItemOwnerDraw },
	{ "Exchange_item", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnExchangeItemClick },
	{ "Shop_ListBG1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnShopListBg1Init },
	{ "Shop_ListBG2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnShopListBg2Init },
	{ "Shop_ListBG3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnShopListBg3Init },
	{ "Shop_ListBG4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnShopListBg4Init },
	{ "Shop_ListBG5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnShopListBg5Init },
	{ "Shop_ListBG6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnShopListBg6Init },
	{ "Shop_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnShopItemInit },
	{ "Shop_item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrPointShopDlg::OnShopItemOwnerDraw },
	{ "Shop_item", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrPointShopDlg::OnShopItemClick },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x84ba90
bool FrPointShopDlg::OnInit(void) {
	m_page = 3;
	m_pointShop.SetMode(3);
	m_pPointGetListMyBmp = g_pFresh->GetBitmap("tikipoint_btn_pointget_list_my_o");
	m_pPointGetListUseBmp = g_pFresh->GetBitmap("tikipoint_btn_pointget_list_use_o");
	m_pPointUseListNBmp = g_pFresh->GetBitmap("tikipoint_pointuse_list_n");
	m_pPointUseListOBmp = g_pFresh->GetBitmap("tikipoint_pointuse_list_o");
	m_pBuyBmp = g_pFresh->GetBitmap("tikipoint_buy");
	return true;
}

// 0x84c480
void FrPointShopDlg::DrawBitmap(const Bitmap* bmp, WRect rect, uint32_t color, bool bitmapSize) {
	if (bmp == 0) {
		return;
	}
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == 0) {
		return;
	}
	WRect dest = rect;
	if (bitmapSize) {
		dest.w = (float)bmp->Width();
		dest.h = (float)bmp->Height();
	}
	gdi->SetTextStyle(0);
	gdi->DrawTexture(bmp, dest, color, 0);
}

// 0x84c530
void FrPointShopDlg::OnMyPointInit(PG_MSGARG param) {
	m_pMyPoint = DYNAMIC_CAST(FrArea, param);
}

// 0x84c5a0
void FrPointShopDlg::OnMainInit(PG_MSGARG param) {
	m_pMainButton = DYNAMIC_CAST(FrButton, param);
	if (m_pMainButton) {
		m_pMainButton->SetVisible(false);
	}
	if (m_pMainButton) {
		m_pMainButton->SetPushDelay(0.01f);
	}
	if (m_pMainButton) {
		m_pMainButton->EnableBlink(true);
	}
}

// 0x84c650
void FrPointShopDlg::OnHelpInit(PG_MSGARG param) {
	m_pHelpButton = DYNAMIC_CAST(FrButton, param);
	if (m_pHelpButton) {
		m_pHelpButton->SetVisible(true);
	}
	if (m_pHelpButton) {
		m_pHelpButton->SetPushDelay(0.01f);
	}
}

// 0x84c6f0
void FrPointShopDlg::OnExitInit(PG_MSGARG param) {
	m_pExitButton = DYNAMIC_CAST(FrButton, param);
	if (m_pExitButton) {
		m_pExitButton->SetVisible(true);
	}
	if (m_pExitButton) {
		m_pExitButton->SetPushDelay(0.01f);
	}
}

// 0x84c790
void FrPointShopDlg::OnGotoMainBgInit(PG_MSGARG param) {
	m_pGotoMainBg = DYNAMIC_CAST(FrArea, param);
	if (m_pGotoMainBg) {
		m_pGotoMainBg->SetVisible(false);
	}
}

// 0x84c810
void FrPointShopDlg::OnIntroBg1Init(PG_MSGARG param) {
	m_pIntroBg1 = DYNAMIC_CAST(FrArea, param);
	if (m_pIntroBg1) {
		m_pIntroBg1->SetVisible(true);
	}
}

// 0x84c890
void FrPointShopDlg::OnIntroBg2Init(PG_MSGARG param) {
	m_pIntroBg2 = DYNAMIC_CAST(FrArea, param);
	if (m_pIntroBg2) {
		m_pIntroBg2->SetVisible(true);
	}
}

// 0x84c910
void FrPointShopDlg::OnIntroNoticeInit(PG_MSGARG param) {
	m_pIntroNotice = DYNAMIC_CAST(FrViewer, param);
	if (m_pIntroNotice) {
		m_pIntroNotice->SetVisible(true);
	}
	m_pIntroNotice->ShowScrollBar(true);
}

// 0x84c9b0
void FrPointShopDlg::OnIntroNoticeOwnerDraw(void) {
	if (m_pIntroNotice && m_pIntroNotice->IsVisible()) {
		m_pIntroNotice->Open("pointshop_notice.jpg");
	}
}

// 0x84c9d0
void FrPointShopDlg::OnExchangeInit(PG_MSGARG param) {
	m_pExchangeButton = DYNAMIC_CAST(FrButton, param);
	if (m_pExchangeButton) {
		m_pExchangeButton->SetVisible(true);
	}
	if (m_pExchangeButton) {
		m_pExchangeButton->SetPushDelay(0.01f);
	}
	if (m_pExchangeButton) {
		m_pExchangeButton->EnableHover(true, 0.0f);
	}
}

// 0x84ca80
void FrPointShopDlg::OnBuyInit(PG_MSGARG param) {
	m_pBuyButton = DYNAMIC_CAST(FrButton, param);
	if (m_pBuyButton) {
		m_pBuyButton->SetVisible(true);
	}
	if (m_pBuyButton) {
		m_pBuyButton->SetPushDelay(0.01f);
	}
}

// 0x84cb20
void FrPointShopDlg::OnExchangeListBg1Init(PG_MSGARG param) {
	m_pExchangeListBg[0] = DYNAMIC_CAST(FrArea, param);
	if (m_pExchangeListBg[0]) {
		m_pExchangeListBg[0]->SetVisible(false);
	}
}

// 0x84cba0
void FrPointShopDlg::OnExchangeListBg2Init(PG_MSGARG param) {
	m_pExchangeListBg[1] = DYNAMIC_CAST(FrArea, param);
	if (m_pExchangeListBg[1]) {
		m_pExchangeListBg[1]->SetVisible(false);
	}
}

// 0x84cc20
void FrPointShopDlg::OnExchangeListBg3Init(PG_MSGARG param) {
	m_pExchangeListBg[2] = DYNAMIC_CAST(FrArea, param);
	if (m_pExchangeListBg[2]) {
		m_pExchangeListBg[2]->SetVisible(false);
	}
}

// 0x84cca0
void FrPointShopDlg::OnExchangeListBg4Init(PG_MSGARG param) {
	m_pExchangeListBg[3] = DYNAMIC_CAST(FrArea, param);
	if (m_pExchangeListBg[3]) {
		m_pExchangeListBg[3]->SetVisible(false);
	}
}

// 0x84cd20
void FrPointShopDlg::OnExchangeListBg5Init(PG_MSGARG param) {
	m_pExchangeListBg[4] = DYNAMIC_CAST(FrArea, param);
	if (m_pExchangeListBg[4]) {
		m_pExchangeListBg[4]->SetVisible(false);
	}
}

// 0x84cda0
void FrPointShopDlg::OnExchangeListBg6Init(PG_MSGARG param) {
	m_pExchangeListBg[5] = DYNAMIC_CAST(FrArea, param);
	if (m_pExchangeListBg[5]) {
		m_pExchangeListBg[5]->SetVisible(false);
	}
}

// 0x84ce20
void FrPointShopDlg::OnPrePointInit(PG_MSGARG param) {
	m_pPrePoint = DYNAMIC_CAST(FrArea, param);
	if (m_pPrePoint) {
		m_pPrePoint->SetVisible(false);
	}
}

// 0x84cea0
void FrPointShopDlg::OnPrePointOwnerDraw(void) {
	if (m_pPrePoint == 0 || m_pWindFont == 0 || !m_pPrePoint->IsVisible()) {
		return;
	}
	m_pWindFont->SetColor(0xffd31b91);
	m_pWindFont->Print(m_pPrePoint->GetRect().x, m_pPrePoint->GetRect().y,
		MakeStr("%d", m_pointShop.GetTotal()), TITLEFONT_ALIGN_RIGHT, 0.9f, 1.0f);
}

// 0x84cf20
void FrPointShopDlg::OnExchangeOkInit(PG_MSGARG param) {
	m_pExchangeOkButton = DYNAMIC_CAST(FrButton, param);
	if (m_pExchangeOkButton) {
		m_pExchangeOkButton->SetVisible(false);
	}
}

// 0x84cfa0
void FrPointShopDlg::OnOwnItemInit(PG_MSGARG param) {
	m_pOwnItemList = DYNAMIC_CAST(FrListBox, param);
	if (m_pOwnItemList) {
		m_pOwnItemList->SetVisible(false);
	}
	m_pOwnItemList->EnableHover(true, 0.1f);
}

// 0x84d030
void FrPointShopDlg::OnExchangeItemInit(PG_MSGARG param) {
	m_pExchangeItemList = DYNAMIC_CAST(FrListBox, param);
	if (m_pExchangeItemList) {
		m_pExchangeItemList->SetVisible(false);
	}
}

// 0x84d0b0
void FrPointShopDlg::OnShopListBg1Init(PG_MSGARG param) {
	m_pShopListBg[0] = DYNAMIC_CAST(FrArea, param);
	if (m_pShopListBg[0]) {
		m_pShopListBg[0]->SetVisible(false);
	}
}

// 0x84d130
void FrPointShopDlg::OnShopListBg2Init(PG_MSGARG param) {
	m_pShopListBg[1] = DYNAMIC_CAST(FrArea, param);
	if (m_pShopListBg[1]) {
		m_pShopListBg[1]->SetVisible(false);
	}
}

// 0x84d1b0
void FrPointShopDlg::OnShopListBg3Init(PG_MSGARG param) {
	m_pShopListBg[2] = DYNAMIC_CAST(FrArea, param);
	if (m_pShopListBg[2]) {
		m_pShopListBg[2]->SetVisible(false);
	}
}

// 0x84d230
void FrPointShopDlg::OnShopListBg4Init(PG_MSGARG param) {
	m_pShopListBg[3] = DYNAMIC_CAST(FrArea, param);
	if (m_pShopListBg[3]) {
		m_pShopListBg[3]->SetVisible(false);
	}
}

// 0x84d2b0
void FrPointShopDlg::OnShopListBg5Init(PG_MSGARG param) {
	m_pShopListBg[4] = DYNAMIC_CAST(FrArea, param);
	if (m_pShopListBg[4]) {
		m_pShopListBg[4]->SetVisible(false);
	}
}

// 0x84d330
void FrPointShopDlg::OnShopListBg6Init(PG_MSGARG param) {
	m_pShopListBg[5] = DYNAMIC_CAST(FrArea, param);
	if (m_pShopListBg[5]) {
		m_pShopListBg[5]->SetVisible(false);
	}
}

// 0x84d3b0
void FrPointShopDlg::OnShopItemInit(PG_MSGARG param) {
	m_pShopItemList = DYNAMIC_CAST(FrListBox, param);
	if (m_pShopItemList) {
		m_pShopItemList->SetVisible(false);
	}
}

// 0x84dbc0
void FrPointShopDlg::OnMyPointOwnerDraw(void) {
	if (m_pMyPoint == 0 || m_pWindFont == 0) {
		return;
	}
	m_pWindFont->SetColor(0xffd31b91);
	m_pWindFont->Print(m_pMyPoint->GetRect().x, m_pMyPoint->GetRect().y,
		MakeStr("%d", JpSharedDoc_GetTikiPoints()), TITLEFONT_ALIGN_RIGHT, 1.0f, 1.0f);
}

// 0x84dc30
void FrPointShopDlg::OnHelpClick(void) {
	if (m_pHelpButton && m_pHelpButton->IsVisible()) {
		FrForm* help = CreatePointShopHelp(g_pFresh->GetManager(), this, "PointShopHelp", NULL);
		if (help) {
			help->Open(NULL, 3);
		}
	}
}

// 0x84e320
FrPointShopDlg::ListedItem* FrPointShopDlg::FindListedItem(uint32_t typeId) {
	for (std::list<ListedItem>::iterator it = m_listedItems.begin(); it != m_listedItems.end(); ++it) {
		if (it->typeId == typeId) {
			return &*it;
		}
	}
	return 0;
}

// Glue (no JP function: the list walk is inline in 0x84e350). The JP
// CSharedDoc +0x37ec list is KR m_cardPeriodList[0]: it follows the owned
// cards (+0x37e0, KR m_myCardList) and is walked the KR way, the element's
// +0xc (sSCardAvilityPeriodInfo::partsId, the item a card is put on)
// compared with an owned item's serial (KR CardRemove, CardDlg). So an
// item carrying a period card is not exchangeable.
bool JpSharedDoc_IsInList37ec(uint32_t itemId) {
	const std::list<sSCardAvilityPeriodInfo>& cards = CSharedDoc::Instance()->m_cardPeriodList[0];
	for (std::list<sSCardAvilityPeriodInfo>::const_iterator it = cards.begin(); it != cards.end(); ++it) {
		if (it->partsId == itemId) {
			return true;
		}
	}
	return false;
}

// 0x84e350
// The JP CSharedDoc +0xa32 / +0xa36.. are KR m_myBallId / m_equipItem[10]
// (the same 0x3fe shift as +0xa2a = m_myCharacterId).
bool FrPointShopDlg::IsExchangeable(uint32_t typeId, const void* record) {
	bool exchangeable = false;
	if (record == NULL) {
		return false;
	}
	uint32_t kind = typeId >> 26;
	const JpIffItemBase* item;
	switch (kind) {
	case 2:
		item = JpItemManager_FindPart(typeId);
		break;
	case 4:
		item = JpItemManager_FindClubSet(typeId);
		break;
	case 5:
		item = JpItemManager_FindBall(typeId);
		break;
	case 6:
		item = JpItemManager_FindItem(typeId);
		break;
	case 0x1f:
		item = JpItemManager_FindCard(typeId);
		break;
	default:
		return false;
	}
	if (item == NULL || JpIffItemBase_GetPointValue(item) == 0) {
		return false;
	}
	CSharedDoc* doc = CSharedDoc::Instance();
	switch (kind) {
	case 2:
	case 4:
	case 5:
	case 6: {
		const sItemInfo* info = static_cast<const sItemInfo*>(record);
		if (doc->IsEquipParts(info->m_serial, info->m_typeId) == 0) {
			exchangeable = true;
			if (JpSharedDoc_IsInList37ec(info->m_serial)) {
				exchangeable = false;
			}
		}
		uint32_t maxCount = static_cast<uint32_t>(m_pointShop.GetMaxCount(info->m_typeId));
		if (maxCount == 0) {
			return false;
		}
		if (kind == 6) {
			int equipped = 0;
			for (int i = 0; i < 10; ++i) {
				if (typeId == doc->m_equipItem[i]) {
					++equipped;
				}
			}
			if (static_cast<uint32_t>(info->m_count - equipped) / maxCount < 1) {
				return false;
			}
		} else if (kind == 5) {
			if (static_cast<uint32_t>(info->m_count) / maxCount < 1) {
				return false;
			}
			if (doc->m_myBallId == typeId) {
				return false;
			}
		}
		break;
	}
	case 0x1f: {
		const sCards* card = static_cast<const sCards*>(record);
		if (card->unknown8[0] == 0 && card->unknown8[1] == 0) {
			exchangeable = true;
		}
		uint32_t maxCount = static_cast<uint32_t>(m_pointShop.GetMaxCount(card->m_typeId));
		if (maxCount == 0 || static_cast<uint32_t>(card->m_count) / maxCount < 1) {
			return false;
		}
		break;
	}
	default:
		break;
	}
	return exchangeable;
}

// 0x84e5d0
void FrPointShopDlg::FillItemList(void) {
	FrListBox* list;
	if (m_page == 1) {
		list = m_pOwnItemList;
	} else if (m_page == 2) {
		list = m_pShopItemList;
	} else {
		return;
	}
	if (list) {
		for (std::list<ListedItem>::iterator it = m_listedItems.begin(); it != m_listedItems.end(); ++it) {
			list->AddItem(&*it);
		}
	}
}

// 0x84e620
void FrPointShopDlg::OnOwnItemClick(void) {
	FrListItem* selected = m_pOwnItemList->GetSelected();
	if (selected == 0) {
		return;
	}
	uint32_t typeId = static_cast<const ListedItem*>(selected->pData)->typeId;
	ListedItem* row = FindListedItem(typeId);
	if (row == 0 || row->itemCounts.empty()) {
		return;
	}
	CPointShop::Entry entry;
	entry.itemId = 0;
	entry.count = 0;
	entry.typeId = typeId;
	entry.itemId = row->itemCounts.begin()->first;
	CPointShop::Entry* stored = m_pointShop.Add(entry);
	if (stored) {
		uint32_t maxCount = static_cast<uint32_t>(m_pointShop.GetMaxCount(typeId));
		row->count -= maxCount;
		if (row->count < maxCount) {
			m_pOwnItemList->DelItem(selected->pData);
		}
		if (m_pExchangeItemList->FindItem(&stored->typeId) == 0) {
			m_pExchangeItemList->AddItem(&stored->typeId);
		}
	}
}

// 0x84e6e0
void FrPointShopDlg::OnExchangeItemClick(void) {
	FrListItem* selected = m_pExchangeItemList->GetSelected();
	if (selected == 0) {
		return;
	}
	// The exchange row's data: the selected entry's type id.
	uint32_t typeId = *static_cast<const uint32_t*>(selected->pData);
	ListedItem* row = FindListedItem(typeId);
	if (row == 0 || row->itemCounts.empty()) {
		return;
	}
	CPointShop::Entry entry;
	entry.itemId = 0;
	entry.count = 0;
	entry.typeId = typeId;
	entry.itemId = row->itemCounts.begin()->first;
	if (m_pointShop.Remove(entry) == 0) {
		m_pExchangeItemList->DelItem(selected->pData);
	}
	row->count += static_cast<uint32_t>(m_pointShop.GetMaxCount(typeId));
	if (m_pOwnItemList->FindItem(row) == 0) {
		m_pOwnItemList->AddItem(row);
	}
}

// 0x84eda0
// The JP record's name (+0x8) and icon name (+0x49) are the KR sItem's
// m_name / m_icon (JpItemGlue: the record is the KR one); JP 0xbed2f0 is
// KR FrGraphicInterface::Print11 (0xbecb20 PrintText11), 0x98c3f0 MakeStr,
// 0x98bdc0 strncpy. `column` is the name's maximum length (longer names are
// cut with "..."); the count and the point columns sit at 11 and 13 times it.
void FrPointShopDlg::DrawItemRow(FrListItem* item, float height, int column, uint32_t count) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (item == 0 || gdi == 0) {
		return;
	}
	gdi->SetTextColor(0xff000000, 0xffffffff);
	gdi->SetTextStyle(0);
	const ListedItem* row = (const ListedItem*)item->pData;
	std::string icon("");
	std::string name("");
	uint32_t typeId = row->typeId;
	int characterId = 0;
	int pointValue = 0;
	uint32_t shownCount = 0;
	uint32_t maxCount = 0;
	const IFF_STRUCT::sItem* record = 0;
	switch (typeId >> 26) {
	case 2:
		record = reinterpret_cast<const IFF_STRUCT::sItem*>(JpItemManager_FindPart(typeId));
		if (record) {
			icon = record->m_icon;
			name = record->m_name;
			pointValue = JpIffItemBase_GetPointValue(reinterpret_cast<const JpIffItemBase*>(record));
			characterId = (row->typeId >> 18) & 0xff;
		}
		break;
	case 4:
	case 5:
		if (typeId >> 26 == 4) {
			record = reinterpret_cast<const IFF_STRUCT::sItem*>(JpItemManager_FindClubSet(typeId));
		} else {
			record = reinterpret_cast<const IFF_STRUCT::sItem*>(JpItemManager_FindBall(typeId));
		}
		if (record) {
			icon = record->m_icon;
			name = record->m_name;
			pointValue = JpIffItemBase_GetPointValue(reinterpret_cast<const JpIffItemBase*>(record));
		}
		if (icon.length() != 0) {
			icon = std::string("bar_") + icon;
		}
		break;
	case 6:
	case 0x1f:
		if (typeId >> 26 == 6) {
			record = reinterpret_cast<const IFF_STRUCT::sItem*>(JpItemManager_FindItem(typeId));
		} else {
			record = reinterpret_cast<const IFF_STRUCT::sItem*>(JpItemManager_FindCard(typeId));
		}
		if (record) {
			icon = record->m_icon;
			name = record->m_name;
			pointValue = JpIffItemBase_GetPointValue(reinterpret_cast<const JpIffItemBase*>(record));
		}
		break;
	default:
		break;
	}
	if (m_page == 2) {
		const JpPointShopGoods* goods = JpItemManager_FindGoods(row->typeId);
		if (goods) {
			pointValue = JpPointShopGoods_GetPrice(goods);
			shownCount = JpPointShopGoods_GetCount(goods);
			maxCount = 0;
		}
	} else if (m_page == 1) {
		maxCount = static_cast<uint32_t>(m_pointShop.GetMaxCount(row->typeId));
		shownCount = count;
		if (maxCount != 0) {
			shownCount = count / maxCount;
		}
	}
	WRect rect;
	rect.x = item->pos.x + 5.0;
	rect.y = item->pos.y + 4.5;
	rect.w = 0.0f;
	rect.h = 0.0f;
	// A part: the character's base image under the icon.
	if ((row->typeId & 0xfc000000) == 0x08000000) {
		std::string base("");
		base = Jp_GetCharacterIconBaseName(characterId);
		if (base.length() != 0) {
			const Bitmap* bmp = g_pFresh->GetBitmap(base.c_str());
			if (bmp) {
				float scale = height / (bmp->Height() * 1.4f);
				if (scale > 1.0f) {
					scale = 1.0f;
				}
				if (m_page == 2) {
					rect.y -= 2.0;
				}
				rect.w = bmp->Width() * scale;
				rect.h = bmp->Height() * scale;
				gdi->DrawTexture(bmp, rect, 0xffffffff, 0);
				if (m_page == 2) {
					rect.y += 2.0;
				}
			}
		}
	}
	if (icon.length() != 0) {
		const Bitmap* bmp = g_pFresh->GetBitmap(icon.c_str());
		if (bmp) {
			float scale = height / (bmp->Height() * 1.4f);
			scale = scale + ((row->typeId & 0xfc000000) == 0x18000000 ? scale * 0.5 : 0.0);
			if (scale > 1.0f) {
				scale = 1.0f;
			}
			if (m_page == 2) {
				rect.y -= 2.0;
			}
			rect.x = scale + scale + rect.w + rect.x;
			rect.w = bmp->Width() * scale;
			rect.h = bmp->Height() * scale;
			gdi->DrawTexture(bmp, rect, 0xffffffff, 0);
			if (m_page == 2) {
				rect.y += 2.0;
			}
		}
	}
	if (name.length() != 0) {
		if (name.length() > static_cast<uint32_t>(column)) {
			char buf[0x20] = { 0 };
			strncpy(buf, name.c_str(), column - 1);
			name = buf;
			name.append("...", 3);
		}
		if (maxCount != 0) {
			name.append(MakeStr(" (%d)", maxCount));
		}
		float textX = height * 1.6f;
		rect.x = textX + item->pos.x;
		WPoint pos(rect.x, (height - 11.0) * 0.35f + rect.y);
		gdi->Print11(pos, 0, name.c_str());
	}
	gdi->SetTextColor(0xff871ba4, 0xffffffff);
	double offsetY = (height - 11.0) * 0.35f;
	WPoint countPos(static_cast<uint32_t>(column) * 11 + item->pos.x, offsetY + rect.y);
	gdi->Print(countPos, 2, MakeStr("x%d", shownCount));
	gdi->SetTextStyle(1);
	gdi->SetTextColor(0xffd31b9f, 0xffffffff);
	float pointX = static_cast<float>(static_cast<uint32_t>(column) * 13);
	WPoint pointPos(pointX + item->pos.x - 4.0, rect.y + offsetY);
	gdi->Print(pointPos, 2, MakeStr("%d", pointValue));
	gdi->SetTextColor(0xff871ba4, 0xffffffff);
	WPoint tpPos(pointX + item->pos.x, rect.y + offsetY);
	gdi->Print(tpPos, 0, "TP");
}

// 0x84f700
void FrPointShopDlg::OnOwnItemOwnerDraw(PG_MSGARG param) {
	FrListItem* item = (FrListItem*)param;
	if (item == 0) {
		return;
	}
	float w = (float)m_pOwnItemList->GetItemWidth();
	float h = (float)m_pOwnItemList->GetItemHeight();
	if (m_pOwnItemList->GetSelected() == item) {
		DrawBitmap(m_pPointGetListMyBmp, WRect(item->pos.x + 2.0, item->pos.y + 2.0, w, h), 0xffffffff, true);
	}
	ListedItem* row = FindListedItem(((const ListedItem*)item->pData)->typeId);
	if (row) {
		DrawItemRow(item, h + 1.0f, 0x10, row->count);
	}
}

// 0x84f7c0
void FrPointShopDlg::OnExchangeItemOwnerDraw(PG_MSGARG param) {
	FrListItem* item = (FrListItem*)param;
	if (item == 0) {
		return;
	}
	float w = (float)m_pExchangeItemList->GetItemWidth();
	float h = (float)m_pExchangeItemList->GetItemHeight();
	if (m_pExchangeItemList->GetSelected() == item) {
		DrawBitmap(m_pPointGetListUseBmp, WRect(item->pos.x + 2.0, item->pos.y + 2.0, w, h), 0xffffffff, true);
	}
	uint32_t typeId = ((const ListedItem*)item->pData)->typeId;
	ListedItem* row = FindListedItem(typeId);
	if (row == 0 || row->itemCounts.empty()) {
		return;
	}
	CPointShop::Entry key;
	key.itemId = row->itemCounts.begin()->first;
	key.typeId = typeId;
	key.count = 0;
	CPointShop::Entry* entry = m_pointShop.Find(key);
	int maxCount = m_pointShop.GetMaxCount(typeId);
	if (entry && maxCount) {
		DrawItemRow(item, h + 1.0f, 0x10, entry->count * maxCount);
	}
}

// 0x84f8d0
void FrPointShopDlg::OnShopItemOwnerDraw(PG_MSGARG param) {
	FrListItem* item = (FrListItem*)param;
	if (item == 0) {
		return;
	}
	float w = (float)m_pShopItemList->GetItemWidth();
	float h = (float)m_pShopItemList->GetItemHeight();
	if (m_pShopItemList->GetSelected() == item) {
		DrawBitmap(m_pPointUseListOBmp, WRect(item->pos.x, item->pos.y, w, h), 0xffffffff, false);
	}
	DrawBitmap(m_pBuyBmp, WRect(item->pos.x + 412.0, item->pos.y + 10.0, w, h), 0xffffffff, true);
	DrawItemRow(item, h, 0x1e, m_pointShop.GetMaxCount(((const ListedItem*)item->pData)->typeId));
}

// 0x850850
FrPointShopDlg::FrPointShopDlg(void) {
	m_pGotoMainBg = 0;
	m_pIntroBg1 = 0;
	m_pIntroBg2 = 0;
	m_pIntroNotice = 0;
	m_pMainButton = 0;
	m_pHelpButton = 0;
	m_pExitButton = 0;
	m_pExchangeButton = 0;
	m_pBuyButton = 0;
	for (int i = 0; i < 6; i++) {
		m_pExchangeListBg[i] = 0;
		m_pShopListBg[i] = 0;
	}
	m_pMyPoint = 0;
	m_pPrePoint = 0;
	m_pExchangeOkButton = 0;
	m_pOwnItemList = 0;
	m_pExchangeItemList = 0;
	m_pShopItemList = 0;
	m_pPointGetListMyBmp = 0;
	m_pPointGetListUseBmp = 0;
	m_pPointUseListNBmp = 0;
	m_pPointUseListOBmp = 0;
	m_pBuyBmp = 0;
	m_page = 0;
	m_pWindFont = 0;
	m_pWindFont = new CWindFont;
}

// 0x850a30
FrPointShopDlg::~FrPointShopDlg(void) {
	m_pPointGetListMyBmp = 0;
	m_pPointGetListUseBmp = 0;
	m_pPointUseListNBmp = 0;
	m_pPointUseListOBmp = 0;
	m_pBuyBmp = 0;
	if (m_pWindFont) {
		delete m_pWindFont;
		m_pWindFont = 0;
	}
}

// 0x850c40
IObject* FrPointShopDlgMakeInstance(void) {
	return new FrPointShopDlg;
}

struct __sFrPointShopDlg {
	__sFrPointShopDlg(void) {
		ObjectFactory().AddObjectFunctor(FrPointShopDlgMakeInstance, "FrPointShopDlg");
	}
};
static __sFrPointShopDlg s_registerFrPointShopDlg;

// 0x8500a0
// The JP FrEdit::AddText takes five arguments (text, 0, 1, 0, 0); the KR
// one takes the first three. The JP edit's scroll bar (+0x90) is KR
// FrWnd::GetScrollBar().
void FrPointShopNoticeDlg::SetExchange(CPointShop* exchange) {
	m_pExchange = exchange;
	if (exchange == 0 || exchange->GetMode() != 2) {
		return;
	}
	CPointShop::EntryMap entries = exchange->GetEntries();
	for (CPointShop::EntryMap::iterator it = entries.begin(); it != entries.end(); ++it) {
		CPointShop::Entry entry = it->second;
		IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(entry.typeId);
		if (desc && strlen(desc->m_text) != 0) {
			m_pDesc->ClearLine();
			m_pDesc->AddText(desc->m_text, false, true);
			m_pDesc->GetScrollBar()->ScrollToFirst();
		}
	}
}

// 0x850200
void FrPointShopDlg::OnProc(const float dt) {
	if (m_pExchangeOkButton && m_page == 1) {
		CPointShop::EntryMap entries = m_pointShop.GetEntries();
		m_pExchangeOkButton->Enable(entries.size() != 0);
	}
}

// 0x8502b0
void FrPointShopDlg::OnExchangeOkClick(void) {
	CPointShop::EntryMap entries = m_pointShop.GetEntries();
	if (entries.size() != 0) {
		FrPointShopNoticeDlg* notice = (FrPointShopNoticeDlg*)CreatePointShopNoticeDlg(
			g_pFresh->GetManager(), this, "PointShopNoticeDlg", NULL);
		notice->SetExchange(&m_pointShop);
		notice->Open(NULL, 0);
	}
}

// 0x8503a0
void FrPointShopDlg::OnShopItemClick(void) {
	FrListItem* item = m_pShopItemList->GetSelected();
	if (item == 0) {
		return;
	}
	m_pointShop.Clear();
	// The shop row's data: a listed row (the goods' type id).
	CPointShop::Entry entry;
	entry.count = 0;
	entry.itemId = 0;
	entry.typeId = static_cast<const ListedItem*>(item->pData)->typeId;
	if (m_pointShop.Add(entry)) {
		FrPointShopNoticeDlg* notice = (FrPointShopNoticeDlg*)CreatePointShopNoticeDlg(
			g_pFresh->GetManager(), this, "PointShopNoticeDlg", NULL);
		if (notice) {
			notice->SetExchange(&m_pointShop);
			notice->Open(NULL, 0);
		}
	}
}

// 0x850da0
uint32_t FrPointShopDlg::AddListedItem(uint32_t typeId, uint32_t itemId, uint32_t count, bool atFront) {
	ListedItem* row = FindListedItem(typeId);
	if (row == NULL) {
		ListedItem item;
		item.typeId = typeId;
		item.count = count;
		if (itemId != 0) {
			item.itemCounts[itemId] = static_cast<int32_t>(count);
		}
		if (atFront) {
			m_listedItems.push_front(item);
		} else {
			m_listedItems.push_back(item);
		}
		return count;
	}
	row->count += count;
	if (itemId != 0) {
		row->itemCounts[itemId] = static_cast<int32_t>(count);
	}
	return row->count;
}

// 0x850f10
// The JP CSharedDoc lists are the KR members in the same order: +0x3734
// m_myPartsList, +0x3744 m_myItemMap, +0x3750 m_myGearList, +0x375c
// m_itemInfoList, +0x37e0 m_myCardList (the element fields read are KR
// sItemInfo's m_serial, m_typeId, m_count, m_stateBits.kind and sCards'
// m_guid, m_typeId, m_count).
void FrPointShopDlg::BuildItemList(void) {
	for (std::list<ListedItem>::iterator it = m_listedItems.begin(); it != m_listedItems.end(); ++it) {
		it->itemCounts.clear();
	}
	m_listedItems.clear();
	if (m_page == 1) {
		CSharedDoc* doc = CSharedDoc::Instance();
		for (std::list<sItemInfo>::iterator it = doc->m_myPartsList.begin(); it != doc->m_myPartsList.end(); ++it) {
			sItemInfo* info = &*it;
			if (info->m_typeId != 0 && info->m_stateBits.kind == 0 && IsExchangeable(info->m_typeId, info)) {
				AddListedItem(info->m_typeId, info->m_serial, 1, false);
			}
		}
		for (std::list<sItemInfo>::iterator it = doc->m_myGearList.begin(); it != doc->m_myGearList.end(); ++it) {
			sItemInfo* info = &*it;
			if (info->m_typeId != 0 && info->m_stateBits.kind == 0 && IsExchangeable(info->m_typeId, info)) {
				AddListedItem(info->m_typeId, info->m_serial, static_cast<uint32_t>(info->m_count), false);
			}
		}
		for (std::list<sItemInfo>::iterator it = doc->m_itemInfoList.begin(); it != doc->m_itemInfoList.end(); ++it) {
			sItemInfo* info = &*it;
			if (info->m_typeId != 0 && info->m_stateBits.kind == 0 && IsExchangeable(info->m_typeId, info)) {
				uint32_t typeId = info->m_typeId;
				int count = info->m_count;
				if ((typeId & 0x2000000) == 0) {
					int equipped = 0;
					for (int i = 0; i < 10; ++i) {
						if (typeId == doc->m_equipItem[i]) {
							++equipped;
						}
					}
					count -= equipped;
				}
				AddListedItem(typeId, info->m_serial, static_cast<uint32_t>(count), false);
			}
		}
		for (std::map<unsigned int, sItemInfo>::iterator it = doc->m_myItemMap.begin(); it != doc->m_myItemMap.end(); ++it) {
			sItemInfo* info = &it->second;
			if (info->m_typeId != 0 && info->m_stateBits.kind == 0 && IsExchangeable(info->m_typeId, info)) {
				AddListedItem(info->m_typeId, info->m_serial, 1, false);
			}
		}
		for (std::list<sCards>::iterator it = doc->m_myCardList.begin(); it != doc->m_myCardList.end(); ++it) {
			sCards* card = &*it;
			if (card->m_typeId != 0 && IsExchangeable(card->m_typeId, card)) {
				AddListedItem(card->m_typeId, card->m_guid, static_cast<uint32_t>(card->m_count), false);
			}
		}
	} else if (m_page == 2) {
		// The JP item manager's goods map (+0x200), walked in key order.
		int goodsCount = JpItemManager_GetGoodsCount();
		for (int i = 0; i < goodsCount; ++i) {
			const JpPointShopGoods* goods = JpItemManager_GetGoodsAt(i);
			if (JpPointShopGoods_IsActive(goods) && JpPointShopGoods_GetTypeId(goods) != 0) {
				AddListedItem(JpPointShopGoods_GetTypeId(goods), 0, JpPointShopGoods_GetCount(goods),
					JpPointShopGoods_IsListedFirst(goods));
			}
		}
	}
}

// 0x851310
void FrPointShopDlg::SetPage(int page) {
	m_page = page;
	m_pointShop.SetMode(page);
	bool exchange = false;
	bool shop = false;
	bool intro = false;
	if (page == 1) {
		exchange = true;
	} else if (page == 2) {
		shop = true;
	} else if (page == 3) {
		intro = true;
	}
	if (m_pGotoMainBg) {
		m_pGotoMainBg->SetVisible(!intro);
	}
	if (m_pMainButton) {
		m_pMainButton->SetVisible(!intro);
	}
	if (m_pIntroBg1) {
		m_pIntroBg1->SetVisible(intro);
	}
	if (m_pIntroBg2) {
		m_pIntroBg2->SetVisible(intro);
	}
	if (m_pIntroNotice) {
		m_pIntroNotice->SetVisible(intro);
	}
	if (m_pExchangeButton) {
		m_pExchangeButton->SetVisible(intro);
	}
	if (m_pBuyButton) {
		m_pBuyButton->SetVisible(intro);
	}
	for (int i = 0; i < 6; i++) {
		if (m_pExchangeListBg[i]) {
			m_pExchangeListBg[i]->SetVisible(exchange);
		}
	}
	if (m_pPrePoint) {
		m_pPrePoint->SetVisible(exchange);
	}
	if (m_pExchangeOkButton) {
		m_pExchangeOkButton->SetVisible(exchange);
	}
	if (m_pOwnItemList) {
		m_pOwnItemList->SetVisible(exchange);
	}
	if (m_pOwnItemList) {
		m_pOwnItemList->ClearItem();
	}
	if (m_pExchangeItemList) {
		m_pExchangeItemList->SetVisible(exchange);
	}
	if (m_pExchangeItemList) {
		m_pExchangeItemList->ClearItem();
	}
	for (int i = 0; i < 6; i++) {
		if (m_pShopListBg[i]) {
			m_pShopListBg[i]->SetVisible(shop);
		}
	}
	if (m_pShopItemList) {
		m_pShopItemList->SetVisible(shop);
	}
	if (m_pShopItemList) {
		m_pShopItemList->ClearItem();
	}
	BuildItemList();
	FillItemList();
}

// 0x851570
void FrPointShopDlg::OnMainClick(void) {
	SetPage(3);
}

// 0x851580
void FrPointShopDlg::OnExchangeClick(void) {
	SetPage(1);
}

// 0x851590
void FrPointShopDlg::OnBuyClick(void) {
	SetPage(2);
}

// --- CPointShop -------------------------------------------------------------

// 0x84fba0
CPointShop::EntryMap CPointShop::GetEntries(void) const {
	return m_entries;
}

// 0x8515c0
uint32_t CPointShop::GetKey(const Entry& entry) const {
	if (m_mode == 1) {
		return entry.itemId;
	}
	if (m_mode == 2) {
		return entry.typeId;
	}
	return 0;
}

// 0x851810
int CPointShop::GetExchangePoint(uint32_t typeId) {
	int points = 0;
	const JpIffItemBase* item;
	switch ((typeId >> 26) - 2) {
	case 0:
		item = JpItemManager_FindPart(typeId);
		break;
	case 2:
		item = JpItemManager_FindClubSet(typeId);
		break;
	case 3:
		item = JpItemManager_FindBall(typeId);
		break;
	case 4:
		item = JpItemManager_FindItem(typeId);
		break;
	case 29:
		item = JpItemManager_FindCard(typeId);
		break;
	default:
		return points;
	}
	if (item) {
		points = JpIffItemBase_GetPointValue(item);
	}
	return points;
}

// 0x8518e0
int CPointShop::GetMaxCount(uint32_t typeId) {
	int count = 0;
	const JpIffItemBase* item;
	switch ((typeId >> 26) - 2) {
	case 0:
		if (JpItemManager_FindPart(typeId)) {
			count = 1;
		}
		return count;
	case 2:
		if (JpItemManager_FindClubSet(typeId)) {
			count = 1;
		}
		return count;
	case 3:
		item = JpItemManager_FindBall(typeId);
		break;
	case 4:
		item = JpItemManager_FindItem(typeId);
		break;
	case 29:
		item = JpItemManager_FindCard(typeId);
		break;
	default:
		return count;
	}
	if (item) {
		count = JpIffItemBase_GetMaxCount(item);
	}
	return count;
}

// 0x851e90
void CPointShop::Send(void) {
	uint8_t size = static_cast<uint8_t>(m_entries.size());
	if (m_mode == 1) {
		WSendPacket packet(0x128);
		packet.Encode1(size);
		for (EntryMap::iterator it = m_entries.begin(); it != m_entries.end(); ++it) {
			packet.Encode4(it->second.typeId);
			packet.Encode4(it->second.itemId);
			packet.Encode4(it->second.count);
			packet.Encode4(GetExchangePoint(it->second.typeId));
		}
		packet.Send(SEND_TO_DEFAULT);
	} else if (m_mode == 2) {
		WSendPacket packet(0x129);
		packet.Encode1(size);
		for (EntryMap::iterator it = m_entries.begin(); it != m_entries.end(); ++it) {
			const JpPointShopGoods* goods = JpItemManager_FindGoods(it->second.typeId);
			if (goods) {
				packet.Encode4(it->second.typeId);
				packet.Encode4(it->second.count);
				packet.Encode4(JpPointShopGoods_GetPrice(goods));
			}
		}
		packet.Send(SEND_TO_DEFAULT);
	}
}

// 0x852080
int CPointShop::SetMode(int mode) {
	if (m_mode == mode) {
		return 0;
	}
	if (mode <= 0 && mode >= 4) {
		return -1;
	}
	Clear();
	m_mode = mode;
	return 1;
}

// 0x8520c0
void CPointShop::Clear(void) {
	m_entries.clear();
}

// 0x8520d0
int CPointShop::GetTotal(void) {
	if (m_mode == 3) {
		return 0;
	}
	int total = 0;
	for (EntryMap::iterator it = m_entries.begin(); it != m_entries.end(); ++it) {
		int points = 0;
		if (m_mode == 1) {
			points = GetExchangePoint(it->second.typeId) * it->second.count;
		} else if (m_mode == 2) {
			const JpPointShopGoods* goods = JpItemManager_FindGoods(it->second.typeId);
			if (goods) {
				points = JpPointShopGoods_GetPrice(goods) * it->second.count;
			}
		}
		total += points;
	}
	return total;
}

// 0x8523e0
int CPointShop::Remove(const Entry& entry) {
	if (m_mode == 3) {
		return 0;
	}
	uint32_t key = GetKey(entry);
	if (key == 0) {
		return 0;
	}
	EntryMap::iterator it = m_entries.find(key);
	if (it == m_entries.end()) {
		return 0;
	}
	it->second.count--;
	int count = static_cast<int>(it->second.count);
	if (count <= 0) {
		m_entries.erase(it);
	}
	return count;
}

// 0x852450
CPointShop::Entry* CPointShop::Find(const Entry& entry) {
	if (m_mode == 3) {
		return 0;
	}
	uint32_t key = GetKey(entry);
	if (key == 0) {
		return 0;
	}
	EntryMap::iterator it = m_entries.find(key);
	if (it == m_entries.end()) {
		return 0;
	}
	return &it->second;
}

// 0x8524d0
CPointShop::~CPointShop(void) {
	Clear();
}

// 0x852650
CPointShop::CPointShop(void) {
}

// 0x852700
CPointShop::Entry* CPointShop::Add(const Entry& entry) {
	if (m_mode == 3) {
		return 0;
	}
	Entry added = entry;
	uint32_t key = GetKey(entry);
	if (key == 0) {
		return 0;
	}
	Entry* found = Find(entry);
	if (found) {
		uint32_t kind = found->typeId >> 26;
		if (kind != 2 && kind != 4) {
			found->count++;
			return found;
		}
	} else if (m_entries.size() <= 4) {
		m_entries[key] = added;
		Entry* stored = &m_entries.find(key)->second;
		stored->count++;
		return stored;
	}
	return 0;
}

// 0x654630
FrForm* CreatePointShopDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrPointShopDlg>(manager, owner, name, parent);
}
