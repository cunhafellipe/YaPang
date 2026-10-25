// The JP CTaskMain's alchemy dialog opener and result handler (JP 0x655d70,
// 0x6525a0; names ours), from the JP exe's bodies (node scripts/disasm.ts
// --jp 0x<va>), in the KR terms of CTaskMain::OnUnderBar_MagicBoxUp and
// OnRecycleDlgResult (TaskMain.cpp): CreateForm, Open with a CTaskMain
// result handler, the avatar manager's SetActive around the dialog.
//
// FrAlchemyMPBreakDlg's message map (0xe07ad8 / 0xe07fe0) and its control
// init handlers follow the task glue.
//
// Packets: none sent here; the dialog's 0x18d (CAlchemyMPBreak 0x476a20) is
// in its undecompiled body (JpAlchemyDlg.h).
// Messages: 0x2EE to the current task's main actor when the service is
// closed (JP-only, Jp_NotifyServerServiceClosed).

#include <stdint.h>
#include <string.h>

#include <algorithm>
#include <vector>

#include "JpAlchemyDlg.h"
#include "JpCardComposeDlg.h"

#include "Packet.h"
#include "RFLobbyDesktop.h"
#include "SharedDoc.h"
#include "TaskMain.h"
#include "JpItemGlue.h"
#include "JpLobbyGlue.h"
#include "JpMasteryCard.h"
#include "JpMasterySlotAni.h"
#include "JpMPBreakCountSetDlg.h"
#include "TaskManager.h"
#include "actor.h"
#include "avatar_avatar.h"
#include "commonutil.h"
#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "fresh.h"
#include "frlistbox.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "bitmap.h"
#include "fxsequence.h"
#include "SoundManager.h"
#include "wfont.h"
#include "wresrcmng.h"
#include "wview.h"

// The JP service bit the alchemy dialog needs (bit 1 of the high dword).
static const uint64_t kAlchemyService = (uint64_t)2 << 32;

// JP CTaskMain +0x1a8, the open alchemy dialog: the KR CTaskMain has no such
// member, so the port keeps it here, with the task that opened it.
extern WMatrix g_camera;   // 0xf02430

static CTaskMain* s_alchemyDlgTask = NULL;
static FrAlchemyMPBreakDlg* s_alchemyDlg = NULL;

static FrAlchemyMPBreakDlg* AlchemyDlgOf(const CTaskMain* task) {
	return s_alchemyDlgTask == task ? s_alchemyDlg : NULL;
}

static void SetAlchemyDlg(CTaskMain* task, FrAlchemyMPBreakDlg* dlg) {
	s_alchemyDlgTask = task;
	s_alchemyDlg = dlg;
}

// The JP CTaskMain +0x1a8 read (inline in the JP).
FrForm* TaskMain_GetAlchemyDlg(const CTaskMain* task) {
	return AlchemyDlgOf(task);
}

// 0x65f259 (inline in the JP CTaskMain::HandleMsgCommon 0x65d8e0, case
// 0x2e4, the break result: param1 the MP, param2 the bonus).
void TaskMain_OnAlchemyBreakResult(CTaskMain* task, int mp, int bonus) {
	FrAlchemyMPBreakDlg* dlg = AlchemyDlgOf(task);
	if (dlg) {
		dlg->OnBreakResult(mp, bonus);
	}
}

// 0x65f27b (inline in the JP HandleMsgCommon, case 0x2e5): the exchange
// sequence starts at step 1.
void TaskMain_StartAlchemyBreak(CTaskMain* task) {
	FrAlchemyMPBreakDlg* dlg = AlchemyDlgOf(task);
	if (dlg) {
		dlg->SetProcState(1);
	}
}

// 0x651080: a JP CTaskMain member (HandleMsgCommon case 0x2e6, 0x65f297);
// it does not test the dialog.
void TaskMain_ResetAlchemyDlg(CTaskMain* task) {
	AlchemyDlgOf(task)->ResetView();
}

// 0xb2e7e0 (0xb2dce0 / 0xb2dcf0: the service's high and low dwords). The
// current task is AfxGetTask's (CTaskManager 0xe73df0, GetCurrentTask
// 0x9b79c0); its virtual +0x30 is SendMsgToMainActor.
void Jp_NotifyServerServiceClosed(uint64_t service) {
	int high = (int)(service >> 32);
	int low = (int)(service & 0xFFFFFFFFu);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2EE, high, low, 0, 0, 0));
}

// 0xa19530 (the JP CItemManager, thiscall on CSharedDoc +0x20c; name
// ours): whether a type id's record has an MP value (its JP +0x8c word,
// JpIffItemBase_GetBreakMP). Kinds 1, 3, 7..14, 16..19, 27, 29 and 30 have
// none (a byte table); every other kind is looked up (FindCommonItem
// 0xa17cf0).
static bool JpItemManager_HasBreakMP(uint32_t typeId) {
	switch (typeId >> 26) {
	case 1:
	case 3:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 16:
	case 17:
	case 18:
	case 19:
	case 27:
	case 29:
	case 30:
		return false;
	default:
		break;
	}
	const JpIffItemBase* item = JpItemManager_FindCommonItem(typeId);
	if (item == NULL) {
		return false;
	}
	return (int16_t)JpIffItemBase_GetBreakMP(item) != 0;
}

// 0x6525a0: the JP CTaskMain's alchemy dialog result handler (ret 8, always
// true). In avatar-chat mode with neither the card compose form (JP +0x18c)
// nor the recycle dialog (JP +0x164, the KR m_recycleDlg) open, the player's
// avatar drops state 0x20 (JP ntAvatar::SetState 0x7a4820, the KR SubState)
// and the avatar manager is active again.
bool CTaskMain::OnAlchemyDlgResult(int, FrForm*) {
	SetAlchemyDlg(this, NULL);

	if (RFLobbyDesktop::s_mode == 1 && TaskMain_GetCardComposeDlg(this) == NULL && !IsActiveRecycleDlg()) {
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

// 0x655d70: KR CTaskMain::OnUnderBar_MagicBoxUp's shape: the service test,
// CreateForm<FrAlchemyMPBreakDlg> (JP 0x6548a0) on "AlchemyDlg" opened with
// 0x6525a0 and 0x11; in avatar-chat mode (RFLobbyDesktop::s_mode, JP
// 0xe78c3c) the avatar manager (JP 0xe73e10, NULL-tested) goes inactive;
// CSharedDoc +0x10b25 is the KR m_noticeShown.
void TaskMain_CreateAlchemyDlg(CTaskMain* task) {
	if (SharedDoc_IsControlServerService64(CSharedDoc::Instance(), kAlchemyService)) {
		Jp_NotifyServerServiceClosed(kAlchemyService);
		return;
	}

	if (AlchemyDlgOf(task) == NULL) {
		FrAlchemyMPBreakDlg* dlg = CreateForm<FrAlchemyMPBreakDlg>(g_pFresh->GetManager(), task, "AlchemyDlg", 0);
		SetAlchemyDlg(task, dlg);
		dlg->Open((FRESH_RESULT_CALLBACK)&CTaskMain::OnAlchemyDlgResult, 0x11);

		if (RFLobbyDesktop::s_mode == 1) {
			ntAvatarManager* manager = WSingleton<ntAvatarManager>::Instance();
			if (manager) {
				manager->SetActive(false);
			}
		}
		WSingleton<CSharedDoc>::Instance()->m_noticeShown = false;
	}
}

// 0x4f3450
const sFRESH_MSGMAP* FrAlchemyMPBreakDlg::GetMessageMap(void) const {
	return &_MsgMap;
}

sFRESH_MSGMAP FrAlchemyMPBreakDlg::_MsgMap = { &FrForm::_MsgMap, &FrAlchemyMPBreakDlg::_MsgEntries[0] };

// 0xe07fe0 (the handlers are stored by the static init 0xccb220)
sFRESH_ENTRY FrAlchemyMPBreakDlg::_MsgEntries[] = {
	{ "My_TP_Point", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnMyTPPointInit },
	{ "My_TP_Point", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnMyTPPointDraw },
	{ "Alchemy_Exit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExitInit },
	{ "Alchemy_Exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnFreshOkay },
	{ "Alchemy_Help", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnHelpInit },
	{ "Alchemy_Help", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnHelpClick },
	{ "MagicBox", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnMagicBoxInit },
	{ "MagicBox", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnMagicBoxClick },
	{ "CardCompose", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnCardComposeInit },
	{ "CardCompose", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnCardComposeClick },
	{ "Exchange", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExchangeInit },
	{ "Exchange", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnExchangeClick },
	{ "Cookie_Item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnCookieItemInit },
	{ "Cookie_Item", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnCookieItemClick },
	{ "Pang_Item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnPangItemInit },
	{ "Pang_Item", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnPangItemClick },
	{ "Rare_Item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnRareItemInit },
	{ "Rare_Item", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnRareItemClick },
	{ "TikiTalk_Edit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnTikiTalkEditInit },
	{ "Exchange_ListBG1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExchangeListBG1Init },
	{ "Exchange_ListBG2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExchangeListBG2Init },
	{ "Exchange_ListBG3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExchangeListBG3Init },
	{ "Exchange_ListBG4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExchangeListBG4Init },
	{ "ExchangeOnEffect", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExchangeOnEffectInit },
	{ "ExchangeOnEffect", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnExchangeOnEffectDraw },
	{ "MPGauge", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnMPGaugeInit },
	{ "MPGauge", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnMPGaugeDraw },
	{ "ExpectMP", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExpectMPInit },
	{ "ExpectMP", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnExpectMPDraw },
	{ "PrePoint", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnPrePointInit },
	{ "PrePoint", FRCMD_OWNERDRAW, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnPrePointDraw },
	{ "Exchange_ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExchangeOkInit },
	{ "Exchange_ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnExchangeOkClick },
	{ "Own_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnOwnItemInit },
	{ "Own_item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnOwnItemDraw },
	{ "Own_item", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnOwnItemClick },
	{ "Exchange_item", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExchangeItemInit },
	{ "Exchange_item", FRCMD_OWNERDRAW, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakDlg::OnExchangeItemDraw },
	{ "Exchange_item", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakDlg::OnExchangeItemClick },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x4f35b0
void FrAlchemyMPBreakDlg::UpdateGauges(float dt) {
	if (m_pMileGauge) {
		m_pMileGauge->Update(dt);
	}
	if (m_pRespectGauge) {
		m_pRespectGauge->Update(dt);
	}
	if (m_pFullGauge) {
		m_pFullGauge->Update(dt);
	}
}

// 0x4f3490
void FrAlchemyMPBreakDlg::SetProcState(int state) {
	m_procState = state;
}

// 0x4f34a0 (the frame time is not read): 1: the respect gauge runs to the
// expected MP (at most 1000); 2: once it is there (at the limit, the full
// gauge then runs to 1000 first); 3: back to 0.
void FrAlchemyMPBreakDlg::UpdateGaugeState(float) {
	switch (m_gaugeState) {
	case 1: {
		int expectMP = m_breaker.m_expectMP;
		if (expectMP >= 1000) {
			if (m_pRespectGauge == NULL) {
				return;
			}
			m_pRespectGauge->SetTargetFromCurrent(1000.0f);
		} else {
			if (m_pRespectGauge == NULL) {
				return;
			}
			m_pRespectGauge->SetTargetFromCurrent((float)expectMP);
		}
		m_gaugeState = 2;
		break;
	}
	case 2:
		if (m_breaker.m_expectMP >= 1000) {
			if (m_pRespectGauge && m_pRespectGauge->IsFinished() && m_pRespectGauge->IsFull() && m_pFullGauge) {
				m_pFullGauge->SetTarget(1000.0f);
				m_gaugeState = 3;
			}
		} else if (m_pRespectGauge && m_pRespectGauge->IsFinished()) {
			m_gaugeState = 3;
		}
		break;
	case 3:
		m_gaugeState = 0;
		break;
	}
}

// 0x4f3610
void FrAlchemyMPBreakDlg::OnExchangeClick(void) {
}

// 0x4f3620
void FrAlchemyMPBreakDlg::OnMPGaugeDraw(void) {
	if (m_pRespectGauge) {
		m_pRespectGauge->Draw();
	}
	if (m_pMileGauge) {
		m_pMileGauge->Draw();
	}
	if (m_pFullGauge) {
		m_pFullGauge->Draw();
	}
}

// 0x4f4a70: WMatrix::IDENTITY is 0xe42504, -30.0f 0xd04d04.
void FrAlchemyMPBreakDlg::InitEffect(void) {
	m_effect.Init(g_resrcmng);
	m_effect.SetCamera(&g_camera);
	// JP: the effect's frame time (+0x4, the KR protected
	// CFxBox::m_frameTime) set to 33.333f (0xcff420), the KR constructor's
	// value; the KR CFxBox has no setter for it.
	m_effectCamera = WMatrix::IDENTITY;
	m_effectCamera.xm = 0.0f;
	m_effectCamera.ym = 0.0f;
	m_effectCamera.zm = -30.0f;
}

// 0x4f4b60: the three gauges at the "MPGauge" area (JP FrWnd +0x34/+0x38,
// the KR m_rect.x/.y): border 0, speed 2.0f (0xcfea84), limit 1000.0f
// (0xd0598c); the mile and respect gauges animate towards the expected MP,
// the full gauge (not animated) towards 0.
void FrAlchemyMPBreakDlg::InitGauges(void) {
	float x = m_pMPGauge->GetRect().x;
	float y = m_pMPGauge->GetRect().y;
	int expectMP = m_breaker.m_expectMP;
	JpMaskGaugePlacement placement;
	placement.x = x;
	placement.y = y;
	placement.unused8 = 0.0f;
	placement.limit = 1000.0f;
	m_pRespectGauge->Setup("gaugebar_nothing.tga", "gaugebar_respect.tga", placement, 0, true, 2.0f, true, true);
	m_pRespectGauge->Reset();
	float target = (float)expectMP;
	m_pRespectGauge->SetTarget(target);
	m_pMileGauge->Setup("gaugebar_nothing.tga", "gaugebar_mile.tga", placement, 0, true, 2.0f, true, false);
	m_pMileGauge->Reset();
	m_pMileGauge->SetTarget(target);
	m_pFullGauge->Setup("gaugebar_nothing.tga", "gaugebar_full.tga", placement, 0, false, 2.0f, true, false);
	m_pFullGauge->Reset();
	m_pFullGauge->SetTarget(0.0f);
}

// 0x4f4cc0: the JP FrWndManager's GDI is its +0x84 (KR GetGDI).
void FrAlchemyMPBreakDlg::DrawBitmap(const Bitmap* bmp, WRect rect, uint32_t color, bool bitmapSize) {
	if (bmp == NULL) {
		return;
	}
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi == NULL) {
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

// 0x4f4d70
void FrAlchemyMPBreakDlg::OnMyTPPointInit(intptr_t param) {
	m_pMyTPPoint = DYNAMIC_CAST(FrArea, param);
	if (m_pMyTPPoint) {
		m_pMyTPPoint->SetVisible(false);
	}
}

// 0x4f4df0: the held MP right-aligned at (x + 180, y + 10) of the area.
void FrAlchemyMPBreakDlg::OnMyTPPointDraw(void) {
	if (m_pMyTPPoint == NULL || m_pPointFont == NULL) {
		return;
	}
	FrArea* area = m_pMyTPPoint;
	const char* text = MakeStr("%d", m_breaker.m_heldMP);
	m_pPointFont->Print(area->GetRect().x + 180.0, area->GetRect().y + 10.0, text, TITLEFONT_ALIGN_RIGHT, 1.0f, 1.0f);
}

// 0x4f4e70: the JP FrButton keeps the push delay at +0x104 and the pushed
// time at +0x108 (0.01f, then 0.01f + 0.1f: the inlined SetPushDelay).
void FrAlchemyMPBreakDlg::OnHelpInit(intptr_t param) {
	m_pHelp = DYNAMIC_CAST(FrButton, param);
	if (m_pHelp) {
		m_pHelp->SetVisible(true);
	}
	if (m_pHelp) {
		m_pHelp->SetPushDelay(0.01f);
	}
}

// 0x4f4f10
void FrAlchemyMPBreakDlg::OnExitInit(intptr_t param) {
	m_pExit = DYNAMIC_CAST(FrButton, param);
	if (m_pExit) {
		m_pExit->SetVisible(true);
	}
	if (m_pExit) {
		m_pExit->SetPushDelay(0.01f);
	}
}

// 0x4f4fb0: 0xbe7a40 is the JP FrWnd::EnableHover (style 0x400, +0xc0, +0xc4).
void FrAlchemyMPBreakDlg::OnMagicBoxInit(intptr_t param) {
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

// 0x4f5060
void FrAlchemyMPBreakDlg::OnCardComposeInit(intptr_t param) {
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

// 0x4f5110
void FrAlchemyMPBreakDlg::OnExchangeInit(intptr_t param) {
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

// 0x4f51c0
void FrAlchemyMPBreakDlg::OnCookieItemInit(intptr_t param) {
	m_pCookieItem = DYNAMIC_CAST(FrButton, param);
	if (m_pCookieItem) {
		m_pCookieItem->SetVisible(false);
	}
	if (m_pCookieItem) {
		m_pCookieItem->SetPushDelay(0.01f);
	}
}

// 0x4f5260
void FrAlchemyMPBreakDlg::OnPangItemInit(intptr_t param) {
	m_pPangItem = DYNAMIC_CAST(FrButton, param);
	if (m_pPangItem) {
		m_pPangItem->SetVisible(false);
	}
	if (m_pPangItem) {
		m_pPangItem->SetPushDelay(0.01f);
	}
}

// 0x4f5300
void FrAlchemyMPBreakDlg::OnRareItemInit(intptr_t param) {
	m_pRareItem = DYNAMIC_CAST(FrButton, param);
	if (m_pRareItem) {
		m_pRareItem->SetVisible(false);
	}
	if (m_pRareItem) {
		m_pRareItem->SetPushDelay(0.01f);
	}
}

// 0x4f53a0
void FrAlchemyMPBreakDlg::OnTikiTalkEditInit(intptr_t param) {
	m_pTikiTalkEdit = DYNAMIC_CAST(FrEdit, param);
}

// 0x4f5410
void FrAlchemyMPBreakDlg::OnExchangeListBG1Init(intptr_t param) {
	m_pExchangeListBG[0] = DYNAMIC_CAST(FrArea, param);
	if (m_pExchangeListBG[0]) {
		m_pExchangeListBG[0]->SetVisible(false);
	}
}

// 0x4f5490
void FrAlchemyMPBreakDlg::OnExchangeListBG2Init(intptr_t param) {
	m_pExchangeListBG[1] = DYNAMIC_CAST(FrArea, param);
	if (m_pExchangeListBG[1]) {
		m_pExchangeListBG[1]->SetVisible(false);
	}
}

// 0x4f5510
void FrAlchemyMPBreakDlg::OnExchangeListBG3Init(intptr_t param) {
	m_pExchangeListBG[2] = DYNAMIC_CAST(FrArea, param);
	if (m_pExchangeListBG[2]) {
		m_pExchangeListBG[2]->SetVisible(false);
	}
}

// 0x4f5590
void FrAlchemyMPBreakDlg::OnExchangeListBG4Init(intptr_t param) {
	m_pExchangeListBG[3] = DYNAMIC_CAST(FrArea, param);
	if (m_pExchangeListBG[3]) {
		m_pExchangeListBG[3]->SetVisible(false);
	}
}

// 0x4f5610: the area is not NULL-tested (the original's).
void FrAlchemyMPBreakDlg::OnExchangeOnEffectInit(intptr_t param) {
	m_pExchangeOnEffect = DYNAMIC_CAST(FrArea, param);
	m_effect.SetScreenCenter(m_pExchangeOnEffect->GetRect().x, m_pExchangeOnEffect->GetRect().y);
}

// 0x4f56a0: the effect drawn in perspective from m_effectCamera, then the
// view restored (the same sequence as RFSoltAniWnd::OnDraw 0x5c0a80).
void FrAlchemyMPBreakDlg::OnExchangeOnEffectDraw(void) {
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
	g_view->UpdateCamera();
	m_effect.Display(g_view);
	// JP [4] 0xb4fe00 takes a second argument (1 here) that the KR
	// WView::Flush does not have.
	g_view->Flush(1);
	g_view->SetProjectionMode(mode);
	g_view->SetClip(clipNear, clipFar, false);
	g_view->SetFOV(fov);
	g_view->ResetClippingArea();     // 0xb52890 on (0, 0, width, height)
	g_view->ResetScreenCenter();     // 0xb4fcf0 on (width / 2, height / 2)
	g_view->UpdateCamera();
	g_view->SetFogEnable(fog);
}

// 0x4f5850
void FrAlchemyMPBreakDlg::OnMPGaugeInit(intptr_t param) {
	m_pMPGauge = DYNAMIC_CAST(FrArea, param);
	if (m_pMPGauge) {
		m_pMPGauge->SetVisible(false);
	}
}

// 0x4f58d0
void FrAlchemyMPBreakDlg::OnExpectMPInit(intptr_t param) {
	m_pExpectMP = DYNAMIC_CAST(FrArea, param);
	if (m_pExpectMP) {
		m_pExpectMP->SetVisible(false);
	}
}

// 0x4f5950: the selection's MP ("~?" are glyphs of the font texture).
void FrAlchemyMPBreakDlg::OnExpectMPDraw(void) {
	if (m_pExpectMP == NULL || m_pSmallFont == NULL || !m_pExpectMP->IsVisible()) {
		return;
	}
	FrArea* area = m_pExpectMP;
	const char* text = MakeStr("%d~?", m_breaker.m_selectionMP);
	m_pSmallFont->Print(area->GetRect().x, area->GetRect().y, text, TITLEFONT_ALIGN_RIGHT, 0.9f, 1.0f);
}

// 0x4f59c0
void FrAlchemyMPBreakDlg::OnPrePointInit(intptr_t param) {
	m_pPrePoint = DYNAMIC_CAST(FrArea, param);
	if (m_pPrePoint) {
		m_pPrePoint->SetVisible(false);
	}
}

// 0x4f5a40: the expected MP over 1000: the full gauges (thousands, "%02d",
// 16 below, while the full gauge shows something), then the rest of 1000.
void FrAlchemyMPBreakDlg::OnPrePointDraw(void) {
	if (m_pPrePoint == NULL || m_pSmallFont == NULL || !m_pPrePoint->IsVisible()) {
		return;
	}
	int expectMP = m_breaker.m_expectMP;
	if (expectMP >= 1000) {
		int full = expectMP / 1000;
		expectMP -= full * 1000;
		// The original does not test m_pPointFont here; the port does (it
		// keeps the fonts NULL).
		if (m_pFullGauge && m_pFullGauge->GetValue() != 0.0f && m_pPointFont) {
			FrArea* area = m_pPrePoint;
			const char* text = MakeStr("%02d", full);
			m_pPointFont->Print(area->GetRect().x, area->GetRect().y + 16.0, text, TITLEFONT_ALIGN_RIGHT, 0.9f, 1.0f);
		}
	}
	FrArea* area = m_pPrePoint;
	const char* text = MakeStr("%d/1000", expectMP);
	m_pSmallFont->Print(area->GetRect().x, area->GetRect().y, text, TITLEFONT_ALIGN_RIGHT, 0.9f, 1.0f);
}

// 0x4f5b50
void FrAlchemyMPBreakDlg::OnExchangeOkInit(intptr_t param) {
	m_pExchangeOk = DYNAMIC_CAST(FrButton, param);
	if (m_pExchangeOk) {
		m_pExchangeOk->SetVisible(false);
	}
}

// 0x4f5bd0
void FrAlchemyMPBreakDlg::OnOwnItemInit(intptr_t param) {
	m_pOwnItem = DYNAMIC_CAST(FrListBox, param);
	if (m_pOwnItem) {
		m_pOwnItem->SetVisible(false);
	}
	m_pOwnItem->EnableHover(true, 0.1f);
}

// 0x4f5c60
void FrAlchemyMPBreakDlg::OnExchangeItemInit(intptr_t param) {
	m_pExchangeItem = DYNAMIC_CAST(FrListBox, param);
	if (m_pExchangeItem) {
		m_pExchangeItem->SetVisible(false);
	}
}

// 0x4f68a0: CreateForm<FrMPBreakAboutDlg> (JP 0x4f61c0) on "mpbreak_about",
// opened without a result handler.
void FrAlchemyMPBreakDlg::OnHelpClick(void) {
	if (m_pHelp && m_pHelp->IsVisible()) {
		FrMPBreakAboutDlg* dlg = CreateForm<FrMPBreakAboutDlg>(g_pFresh->GetManager(), this, "mpbreak_about", 0);
		if (dlg) {
			dlg->Open(NULL, 3);
		}
	}
}

// 0x4f6900: closes the dialog, then message 0x338 to the current task's main
// actor (the JP CTaskMain opens the magic box from it).
void FrAlchemyMPBreakDlg::OnMagicBoxClick(void) {
	OnFreshCancel();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x338, 0, 0, 0, 0, 0));
}

// 0x4f6960: the same with message 0x324 (the card compose form).
void FrAlchemyMPBreakDlg::OnCardComposeClick(void) {
	OnFreshCancel();
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x324, 0, 0, 0, 0, 0));
}

// 0x4f74d0
FrAlchemyMPBreakDlg::OwnItem* FrAlchemyMPBreakDlg::FindOwnItem(unsigned int typeId) {
	for (std::list<OwnItem>::iterator it = m_ownItems.begin(); it != m_ownItems.end(); ++it) {
		if (it->typeId == typeId) {
			return &*it;
		}
	}
	return NULL;
}

// 0x4f7500: a row's data starts with its type id.
unsigned int FrAlchemyMPBreakDlg::GetOwnItemCount(FrListItem* item) {
	OwnItem* entry = FindOwnItem(*static_cast<const unsigned int*>(item->pData));
	if (entry && !entry->itemCounts.empty()) {
		return entry->count;
	}
	return 0;
}

// 0x4f7530: the JP CSharedDoc's IsEquipParts 0x63b9c0 (KR IsEquipParts),
// +0x37ec (JpSharedDoc_IsInList37ec), +0xa2e / +0xa32 / +0xa36.. (KR
// m_myClubId, m_myBallId, m_equipItem[10]: the 0x3fe shift of the KR
// fields). The record finders are the ones FindCommonItem picks per kind
// (0xa17160, 0xa171e0, 0xa17220, 0xa17260, 0xa178e0, 0xa17a10); the
// second jump table sends kinds 2, 4, 5, 6 and 0x1c to the sItemInfo
// checks and 0x1f to the card checks.
bool FrAlchemyMPBreakDlg::IsBreakable(uint32_t typeId, const void* record) {
	bool breakable = false;
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
	case 0x1c:
		item = JpItemManager_FindCommonItem(typeId);
		break;
	case 0x1f:
		item = JpItemManager_FindCard(typeId);
		break;
	default:
		return false;
	}
	if (item == NULL || !JpItemManager_HasBreakMP(JpIffItemBase_GetTypeId(item))) {
		return false;
	}
	CSharedDoc* doc = CSharedDoc::Instance();
	switch (kind) {
	case 2:
	case 4:
	case 5:
	case 6:
	case 0x1c: {
		const sItemInfo* info = static_cast<const sItemInfo*>(record);
		if (doc->IsEquipParts(info->m_serial, info->m_typeId) == 0) {
			breakable = true;
			if (JpSharedDoc_IsInList37ec(info->m_serial)) {
				breakable = false;
			}
		}
		if (kind == 4 && doc->m_myClubId == info->m_serial) {
			return false;
		}
		if (kind == 6) {
			int equipped = 0;
			for (int i = 0; i < 10; ++i) {
				if (typeId == doc->m_equipItem[i]) {
					++equipped;
				}
			}
			if ((unsigned int)(info->m_count - equipped) < 1) {
				return false;
			}
		} else if (kind == 5) {
			if (info->m_count < 1) {
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
			breakable = true;
		}
		if (card->m_count < 1) {
			breakable = false;
		}
		break;
	}
	default:
		break;
	}
	return breakable;
}

// 0x4f77b0
int FrAlchemyMPBreakDlg::GetSelectedCount(unsigned int typeId) {
	OwnItem* entry = FindOwnItem(typeId);
	if (entry && !entry->itemCounts.empty()) {
		AlchemyBreakItem key;
		key.itemId = entry->itemCounts.begin()->first;
		key.typeId = typeId;
		key.count = 0;
		AlchemyBreakItem* selected = m_breaker.FindSelection(key);
		if (selected && selected->count != 0) {
			return selected->count;
		}
	}
	return 0;
}

// 0x4f7820: an entry of the item tab (the record's break class) goes into
// "Own_item" unless its type id is already in "Exchange_item" with nothing
// left once the selected count is taken off (FrListBox::GetCurrentItemSize,
// GetItem, AddItem 0xbf0b60).
void FrAlchemyMPBreakDlg::FillOwnItemList(void) {
	if (m_viewState != 3) {
		return;
	}
	FrListBox* list = m_pOwnItem;
	if (list == NULL) {
		return;
	}
	for (std::list<OwnItem>::iterator it = m_ownItems.begin(); it != m_ownItems.end(); ++it) {
		const JpIffItemBase* item = JpItemManager_FindCommonItem(it->typeId);
		if (item == NULL) {
			continue;
		}
		int breakClass = JpIffItemBase_GetBreakClass(item);
		if (m_itemTab == 1) {
			if (breakClass != 1) {
				continue;
			}
		} else if (m_itemTab == 2) {
			if (breakClass != 2) {
				continue;
			}
		} else if (m_itemTab == 3 && breakClass != 3) {
			continue;
		}
		bool add = true;
		for (int i = 0; i < m_pExchangeItem->GetCurrentItemSize(true); i++) {
			FrListItem* row = m_pExchangeItem->GetItem(i);
			if (row && *static_cast<const unsigned int*>(row->pData) == it->typeId) {
				it->count -= GetSelectedCount(it->typeId);
				if (it->count < 1) {
					add = false;
				}
				break;
			}
		}
		if (add) {
			list->AddItem(&*it);
		}
	}
}

// 0x4f7c20: the result notice (string 0x1870b with the bonus and the total,
// else 0x1870d with the MP; MakeStr 0x98c3f0) to the current task's main
// actor as message 0x23.
void FrAlchemyMPBreakDlg::FinishBreakSequence(void) {
	std::string text;
	if (m_breaker.m_resultBonus != 0) {
		text = MakeStr(Jp_GetTableText(0x1870b), m_breaker.m_resultBonus, m_breaker.m_resultBonus + m_breaker.m_resultMP);
	} else {
		text = MakeStr(Jp_GetTableText(0x1870d), m_breaker.m_resultMP);
	}
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)text.c_str(), 0, 0, 0, 0));
}

// 0x4f7d50: the break sequence ("Item_break_B.seq" for a big break) at the
// centre of "ExchangeOnEffect", both coordinates rounded to an int
// (__ftol2_sse) before they are stored as floats.
void FrAlchemyMPBreakDlg::StartBreakSequence(void) {
	std::string name = "Item_break_N.seq";
	if (m_breaker.m_resultBonus != 0) {
		name = "Item_break_B.seq";
	}
	CFxSequence* sequence = m_effect.LoadSequence(name.c_str(), true, NULL);
	if (sequence == NULL) {
		return;
	}
	FrArea* area = m_pExchangeOnEffect;
	float x = area->GetRect().w * 0.5 + area->GetRect().x;
	float y = area->GetRect().h * 0.5 + area->GetRect().y;
	Jp_FxSequence_SetScreenPos(sequence, (float)(int)x, (float)(int)y);
	Jp_FxSequence_SetPos(sequence, g_jpSlotAniSequencePos);
	m_effect.AddSequence(sequence);
}

// 0x4f8000: the exchange sequence. 1: StartBreakSequence and the break
// sound ("item_break_B" over the limit, else "item_break_N"; g_audio slot 3
// with 0.5f / 200.0f); 2: the effect runs (dt in ms) until it finishes;
// 3 -> 4; 4: the gauges get their new targets; 5: waits for the mile gauge
// (over the limit it restarts the mile and respect gauges first); 6:
// FinishBreakSequence, back to 0. Then the gauge state and the gauges.
void FrAlchemyMPBreakDlg::OnProc(const float dt) {
	switch (m_procState) {
	case 1:
		StartBreakSequence();
		m_procState = 2;
		if (m_breaker.m_overLimit) {
			g_audio->PlaySfx("item_break_B", NULL, 0, NULL, NULL, 0.5f, 200.0f);
		} else {
			g_audio->PlaySfx("item_break_N", NULL, 0, NULL, NULL, 0.5f, 200.0f);
		}
		break;
	case 2: {
		if (m_effect.Finished()) {
			m_procState = 3;
		}
		float ms = (float)(dt * 1000.0);
		m_effect.ProcessAttacher(ms);
		m_effect.Process(ms);
		break;
	}
	case 3:
		m_procState = 4;
		break;
	case 4:
		if (m_breaker.m_overLimit) {
			if (m_pFullGauge) {
				m_pFullGauge->SetTarget(0.0f);
			}
			if (m_pMileGauge) {
				m_pMileGauge->SetTargetFromCurrent(1000.0f);
			}
		} else if (m_pMileGauge) {
			m_pMileGauge->SetTargetFromCurrent((float)m_breaker.m_expectMP);
		}
		m_procState = 5;
		break;
	case 5:
		if (m_breaker.m_overLimit) {
			JpMaskGauge* mile = m_pMileGauge;
			if (mile && mile->IsFinished()) {
				mile->Reset();
				m_pMileGauge->SetTarget((float)m_breaker.m_expectMP);
				if (m_pRespectGauge) {
					m_pRespectGauge->Reset();
					m_pRespectGauge->SetTarget((float)m_breaker.m_expectMP);
				}
				m_procState = 6;
			}
		} else if (m_pMileGauge->IsFinished()) {
			m_procState = 6;
		}
		break;
	case 6:
		FinishBreakSequence();
		m_procState = 0;
		break;
	}
	UpdateGaugeState(dt);
	UpdateGauges(dt);
}

// The two JP-only WTitleFontEx subclasses of the point draws (size 0xc,
// names ours, from their textures; the JpAttendanceDlg CCheckDateFont
// shape): +0x1dc the TP points (vtable 0xd1c304), +0x1e0 the MP points
// (vtable 0xd1c32c).
class CTPNumberFont : public WTitleFontEx {
public:
	CTPNumberFont(void);
};

class CMINumberFont : public WTitleFontEx {
public:
	CMINumberFont(void);
};

// 0x4f8210: the ids of [firstId, lastId) with a text, shuffled; the first
// one, 0 without any (0x540a10 is the JP table text).
int FrAlchemyMPBreakDlg::PickTextId(int firstId, int lastId) {
	std::vector<int> ids;
	int count = lastId - firstId;
	int id = firstId;
	for (int i = 0; i < count; i++, id++) {
		std::string text = Jp_GetTableText(id);
		if (!text.empty()) {
			ids.push_back(id);
		}
	}
	int result = 0;
	if (ids.size() != 0) {
		// lib: 0x4f73d0 (std::random_shuffle)
		std::random_shuffle(ids.begin(), ids.end());
		result = ids[0];
	}
	return result;
}

// 0xe16ae0 / 0xe16b04: the fonts' texture entries (0xd1c308, 0xd1c330).
static char s_tpNumberFontName[] = "number_tp.tga";
static char* s_tpNumberFontFile = s_tpNumberFontName;
static char s_miNumberFontName[] = "number_mi.tga";
static char* s_miNumberFontFile = s_miNumberFontName;
// 0xd1c040 / 0xd1c318: their character sets.
static char s_tpNumberFontChars[] = "1234567890";
static char s_miNumberFontChars[] = "1234567890~/?";

// 0x6f5600
CTPNumberFont::CTPNumberFont(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_tpNumberFontFile;
	info.numPages = 1;
	info.fontw = 8;
	info.fonth = 10;
	info.texw = 80;
	info.texh = 10;
	info.pCharSet = s_tpNumberFontChars;

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// 0x6f56c0
CMINumberFont::CMINumberFont(void) {
	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.filename = &s_miNumberFontFile;
	info.numPages = 1;
	info.fontw = 8;
	info.fonth = 10;
	info.texw = 104;
	info.texh = 10;
	info.pCharSet = s_miNumberFontChars;

	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&info);
}

// 0x4fa3d0: the port keeps the members it declares. The original also
// builds the 4 x 0x1c sub-objects (+0x1e4, 0x41a3b0); the owned-item list
// (+0x110) is a member, +0x11c and +0x1c4 are zeroed here. The two point fonts (+0x1dc 0x6f5600, +0x1e0 0x6f56c0) are
// built here. The
// effect (+0x258, 0x48e9b0) is a member; the three gauge bars (+0x1d0..
// +0x1d8, `new` 0x4c, 0x923410) are built here.
FrAlchemyMPBreakDlg::FrAlchemyMPBreakDlg(void) {
	m_pHelp = NULL;
	m_pExit = NULL;
	m_pExchange = NULL;
	m_pExchangeOk = NULL;
	m_pOwnItem = NULL;
	m_pExchangeItem = NULL;
	m_pCookieItem = NULL;
	m_pPangItem = NULL;
	m_pRareItem = NULL;
	m_pListButtonLeft = NULL;
	m_pListButtonRight = NULL;
	for (int i = 0; i < 4; i++) {
		m_pExchangeListBG[i] = NULL;
	}
	m_pMyTPPoint = NULL;
	m_pMPGauge = NULL;
	m_pExpectMP = NULL;
	m_pPrePoint = NULL;
	m_itemTab = 1;
	m_pTikiTalkEdit = NULL;
	m_pExchangeOnEffect = NULL;
	m_pNotice = NULL;
	m_pPointFont = new CTPNumberFont;
	m_pSmallFont = NULL;
	m_pSmallFont = new CMINumberFont;
	m_pMileGauge = NULL;
	m_pMileGauge = new JpMaskGauge;
	m_pRespectGauge = NULL;
	m_pRespectGauge = new JpMaskGauge;
	m_pFullGauge = NULL;
	m_pFullGauge = new JpMaskGauge;
	m_procState = 0;
	m_gaugeState = 0;
	m_exchangeAdded = false;
	m_pCountSetDlg = NULL;
	InitEffect();
}

// 0x4fa690 (0x4faa60 is the deleting destructor; 0x4fa670, 0x4fa680 the
// other bases' thunks). The port tears down what it declares (the
// owned-item list +0x110 and its nodes' maps, 0x4f7060 / 0x4fa130, go with
// the member); the original also zeroes +0x1c4; the point fonts +0x1dc / +0x1e0 (their slot
// 0), then the gauges, are deleted here; the effect is cleared (ClearAll
// 0x972b20) first here.
FrAlchemyMPBreakDlg::~FrAlchemyMPBreakDlg(void) {
	m_effect.ClearAll(false);
	m_pListButtonLeft = NULL;
	m_pListButtonRight = NULL;
	m_pCookieItem = NULL;
	m_pPangItem = NULL;
	m_pRareItem = NULL;
	m_pMPGauge = NULL;
	if (m_pPointFont) {
		delete m_pPointFont;
		m_pPointFont = NULL;
	}
	if (m_pSmallFont) {
		delete m_pSmallFont;
		m_pSmallFont = NULL;
	}
	if (m_pMileGauge) {
		delete m_pMileGauge;
		m_pMileGauge = NULL;
	}
	if (m_pRespectGauge) {
		delete m_pRespectGauge;
		m_pRespectGauge = NULL;
	}
	if (m_pFullGauge) {
		delete m_pFullGauge;
		m_pFullGauge = NULL;
	}}

// 0x4fb1a0: item tab 2 (FrButton::SetStatus 0xbf4fb0, FrListBox::ClearItem
// 0xbf0e50), then the list is rebuilt (0x4faca0) and shown (0x4f7820, a
// tail call).
void FrAlchemyMPBreakDlg::OnCookieItemClick(void) {
	m_itemTab = 2;
	if (m_pCookieItem) {
		m_pCookieItem->SetStatus(FrButton::PRESSED);
	}
	if (m_pPangItem) {
		m_pPangItem->SetStatus(FrButton::NORMAL);
	}
	if (m_pRareItem) {
		m_pRareItem->SetStatus(FrButton::NORMAL);
	}
	m_pOwnItem->ClearItem();
	CollectOwnItems();
	FillOwnItemList();
}

// 0x4fb200: item tab 1.
void FrAlchemyMPBreakDlg::OnPangItemClick(void) {
	m_itemTab = 1;
	if (m_pCookieItem) {
		m_pCookieItem->SetStatus(FrButton::NORMAL);
	}
	if (m_pPangItem) {
		m_pPangItem->SetStatus(FrButton::PRESSED);
	}
	if (m_pRareItem) {
		m_pRareItem->SetStatus(FrButton::NORMAL);
	}
	m_pOwnItem->ClearItem();
	CollectOwnItems();
	FillOwnItemList();
}

// 0x4fb260: item tab 3.
void FrAlchemyMPBreakDlg::OnRareItemClick(void) {
	m_itemTab = 3;
	if (m_pCookieItem) {
		m_pCookieItem->SetStatus(FrButton::NORMAL);
	}
	if (m_pPangItem) {
		m_pPangItem->SetStatus(FrButton::NORMAL);
	}
	if (m_pRareItem) {
		m_pRareItem->SetStatus(FrButton::PRESSED);
	}
	m_pOwnItem->ClearItem();
	CollectOwnItems();
	FillOwnItemList();
}

// 0x4fb460: the view state again (its controls and lists), an empty
// selection, then "Exchange_ok".
void FrAlchemyMPBreakDlg::ResetView(void) {
	SetViewState(m_viewState);
	m_breaker.ClearSelection();
	UpdateExchangeOk();
}

// 0x4fb490: message 0x104 to the current task's main actor, no drag, view
// state 3, the list button bitmaps, the selected tab buttons; in avatar-chat
// mode the player's avatar takes state 0x20 (ntAvatar::SetState 0x7a4820)
// and the avatar manager goes inactive.
bool FrAlchemyMPBreakDlg::OnInit(void) {
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x104, 0, 0, 0, 0, 0));
	EnableDrag(false);
	m_viewState = 3;
	m_breaker.SetState(m_viewState);
	m_pListButtonLeft = g_pFresh->RegisterBitmap("list_button_o_left");
	m_pListButtonRight = g_pFresh->RegisterBitmap("list_button_o_right");
	m_pExchange->SetStatus(FrButton::PRESSED);

	FrButton* tab = NULL;
	if (m_itemTab == 1) {
		tab = m_pPangItem;
	} else if (m_itemTab == 2) {
		tab = m_pCookieItem;
	} else if (m_itemTab == 3) {
		tab = m_pRareItem;
	}
	if (tab) {
		tab->SetStatus(FrButton::PRESSED);
	}

	m_breaker.UpdatePoints();
	InitGauges();
	SetViewState(m_viewState);
	UpdateTexts();
	UpdateView(0);
	UpdateExchangeOk();

	if (RFLobbyDesktop::s_mode == 1) {
		ntAvatarManager* manager = WSingleton<ntAvatarManager>::Instance();
		if (manager) {
			ntAvatar* myAvatar = manager->m_pMyAvatar;
			if (myAvatar) {
				myAvatar->AddState(0x20);
			}
			WSingleton<ntAvatarManager>::Instance()->SetActive(false);
		}
	}
	return true;
}

// 0x4f3350
const sFRESH_MSGMAP* FrAlchemyMPBreakNoticeDlg::GetMessageMap(void) const {
	return &_MsgMap;
}

sFRESH_MSGMAP FrAlchemyMPBreakNoticeDlg::_MsgMap = { &FrForm::_MsgMap, &FrAlchemyMPBreakNoticeDlg::_MsgEntries[0] };

// 0xe07f20 (the handlers are stored by the static init 0xccb120; "Exit" is
// the JP 0xbe5e00, the FrForm slot +0x88 jump of OnFreshCancel)
sFRESH_ENTRY FrAlchemyMPBreakNoticeDlg::_MsgEntries[] = {
	{ "FocuseMsg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakNoticeDlg::OnFocusMsgInit },
	{ "Msg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakNoticeDlg::OnMsgInit },
	{ "CautionMsg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrAlchemyMPBreakNoticeDlg::OnCautionMsgInit },
	{ "Apply", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakNoticeDlg::OnFreshOkay },
	{ "Exit", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrAlchemyMPBreakNoticeDlg::OnFreshCancel },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x4f3360
FrAlchemyMPBreakNoticeDlg::FrAlchemyMPBreakNoticeDlg(void) {
	m_pFocusMsg = NULL;
	m_pMsg = NULL;
	m_pCautionMsg = NULL;
}

// 0x4f33b0 (0x4f48f0 is the deleting destructor; 0x4f3bd0, 0x4f3be0 the
// other bases' thunks)
FrAlchemyMPBreakNoticeDlg::~FrAlchemyMPBreakNoticeDlg(void) {
}

// 0x4f33d0: the JP FrEdit::AddText(text, 0, 1, 0, 0); the KR AddText has
// the first three parameters.
void FrAlchemyMPBreakNoticeDlg::AddFocusText(const char* text) {
	if (text) {
		m_pFocusMsg->AddText(text, false, true);
	}
}

// 0x4f3400
void FrAlchemyMPBreakNoticeDlg::AddMsgText(const char* text) {
	if (text) {
		m_pMsg->AddText(text, false, true);
	}
}

// 0x4f4920
void FrAlchemyMPBreakNoticeDlg::OnFocusMsgInit(intptr_t param) {
	m_pFocusMsg = DYNAMIC_CAST(FrEdit, param);
}

// 0x4f4990
void FrAlchemyMPBreakNoticeDlg::OnMsgInit(intptr_t param) {
	m_pMsg = DYNAMIC_CAST(FrEdit, param);
}

// 0x4f4a00
void FrAlchemyMPBreakNoticeDlg::OnCautionMsgInit(intptr_t param) {
	m_pCautionMsg = DYNAMIC_CAST(FrEdit, param);
}

// 0x475740
int CAlchemyMPBreak::MakeKey(const AlchemyBreakItem& item) {
	AlchemyBreakItem copy = item;
	if (m_state == 3) {
		return copy.itemId;
	}
	return 0;
}

// 0x475770
void CAlchemyMPBreak::SetResult(int mp, int bonus) {
	m_resultMP = mp;
	m_resultBonus = bonus;
}

// 0x475790
void CAlchemyMPBreak::ClearWarnings(void) {
	memset(m_warnings, 0, sizeof(m_warnings));
}

// 0x4757a0
bool CAlchemyMPBreak::GetWarning(int index) {
	return m_warnings[index] != 0;
}

// 0x4757b0
void CAlchemyMPBreak::SetWarning(int index, char value) {
	m_warnings[index] = value;
}

// 0x4757d0
const char* CAlchemyMPBreak::GetWarningText(int index) {
	if (GetWarning(index)) {
		switch (index) {
		case 0:
			return Jp_GetTableText(0x18705);
		case 1:
			return Jp_GetTableText(0x18706);
		case 2:
			return Jp_GetTableText(0x18707);
		case 3:
			return Jp_GetTableText(0x18708);
		}
	}
	return NULL;
}

// 0x475840
bool CAlchemyMPBreak::HasWarning(void) {
	for (int i = 0; i < 4; i++) {
		if (m_warnings[i]) {
			return true;
		}
	}
	return false;
}

// 0x475ed0: the JP item manager's FindCommonItem 0xa17cf0.
int CAlchemyMPBreak::GetBreakClass(unsigned int typeId) {
	const JpIffItemBase* item = JpItemManager_FindCommonItem(typeId);
	if (item == NULL) {
		return 0;
	}
	return JpIffItemBase_GetBreakClass(item);
}

// 0x475f00
int CAlchemyMPBreak::GetItemTab(unsigned int typeId) {
	switch (GetBreakClass(typeId)) {
	case 1:
		return 1;
	case 2:
		return 2;
	case 3:
		return 3;
	default:
		return 0;
	}
}

// 0x475f40: the JP inventory count is a short (+0xc read with movsx).
int CAlchemyMPBreak::GetHeldCountA6(void) {
	const sItemInfo* item = Jp_GameState_FindInventoryItem(0x1A0002A6);
	if (item == NULL) {
		return 0;
	}
	return (int16_t)item->m_count;
}

// 0x475f60
int CAlchemyMPBreak::GetHeldCountA7(void) {
	const sItemInfo* item = Jp_GameState_FindInventoryItem(0x1A0002A7);
	if (item == NULL) {
		return 0;
	}
	return (int16_t)item->m_count;
}

// 0x475f80: parts, club sets, balls, items, kind 0x1c (0xa178e0) and
// cards; the record's MP word (JP +0x8c), 0 for the other kinds. Each JP
// finder is the one FindCommonItem picks for the kind.
int CAlchemyMPBreak::GetItemMP(unsigned int typeId) {
	int mp = 0;
	switch (typeId >> 26) {
	case 2:
	case 4:
	case 5:
	case 6:
	case 0x1c:
	case 0x1f: {
		const JpIffItemBase* item = JpItemManager_FindCommonItem(typeId);
		if (item) {
			mp = JpIffItemBase_GetBreakMP(item);
		}
		break;
	}
	default:
		break;
	}
	return mp;
}

// 0x476a20: WSendPacket 0xa4e800, Encode4 0xa4dd70, Send 0xa4dab0.
void CAlchemyMPBreak::SendBreak(void) {
	unsigned char count = (unsigned char)m_selection.size();
	if (m_state != 3) {
		return;
	}
	WSendPacket packet(0x18d);
	packet.Encode4(count);
	for (std::map<int, AlchemyBreakItem>::iterator it = m_selection.begin(); it != m_selection.end(); ++it) {
		packet.Encode4(it->second.typeId);
		packet.Encode4(it->second.itemId);
		packet.Encode4(it->second.count);
	}
	packet.Send(SEND_TO_DEFAULT);
}

// 0x476b20: the sum is 32-bit, returned zero-extended (edx = 0).
int64_t CAlchemyMPBreak::SumSelectionPrice(void) {
	uint32_t sum = 0;
	for (std::map<int, AlchemyBreakItem>::iterator it = m_selection.begin(); it != m_selection.end(); ++it) {
		const JpIffItemBase* item = JpItemManager_FindCommonItem(it->second.typeId);
		if (item) {
			sum += JpIffItemBase_GetBreakPrice(item);
		}
	}
	return static_cast<int64_t>(sum);
}

// 0x476ba0: 0x4767e0 is the selection map's clear.
int CAlchemyMPBreak::SetState(int state) {
	if (m_state == state) {
		return 0;
	}
	if (state <= 0 && state >= 4) {
		return -1;
	}
	m_selection.clear();
	m_state = state;
	return 1;
}

// 0x476be0: 0x4767e0 on the selection map (+0x8).
void CAlchemyMPBreak::ClearSelection(void) {
	m_selection.clear();
}

// 0x476bf0: in state 3, a selected part of class 7..9 (0x7e9580).
char CAlchemyMPBreak::CheckWarning0(void) {
	if (m_state == 3) {
		for (std::map<int, AlchemyBreakItem>::iterator it = m_selection.begin(); it != m_selection.end(); ++it) {
			if (JpItem_IsPartClass7to9(it->second.typeId)) {
				return 1;
			}
		}
	}
	return 0;
}

// 0x476c70: in state 3, a selected record (not a card, 0x7c000000) whose
// JP +0x94 is 3.
char CAlchemyMPBreak::CheckWarning1(void) {
	if (m_state == 3) {
		for (std::map<int, AlchemyBreakItem>::iterator it = m_selection.begin(); it != m_selection.end(); ++it) {
			const JpIffItemBase* item = JpItemManager_FindCommonItem(it->second.typeId);
			if (item && (JpIffItemBase_GetTypeId(item) & 0xfc000000u) != 0x7c000000u && JpIffItemBase_GetBreakClass(item) == 3) {
				return 1;
			}
		}
	}
	return 0;
}

// 0x476e60: in state 3, a selected card (0x7c000000, FindCard) of grade
// 1 or more.
char CAlchemyMPBreak::CheckWarning3(void) {
	if (m_state == 3) {
		for (std::map<int, AlchemyBreakItem>::iterator it = m_selection.begin(); it != m_selection.end(); ++it) {
			if ((it->second.typeId & 0xfc000000u) == 0x7c000000u) {
				const JpIffItemBase* card = JpItemManager_FindCard(it->second.typeId);
				if (card && JpIffCard_GetGrade(card) >= 1) {
					return 1;
				}
			}
		}
	}
	return 0;
}

// 0x476f10
void CAlchemyMPBreak::CheckWarnings(void) {
	ClearWarnings();
	char warning = CheckWarning0();
	if (warning) {
		SetWarning(0, warning);
	}
	warning = CheckWarning1();
	if (warning) {
		SetWarning(1, warning);
	}
	warning = CheckWarning2();
	if (warning) {
		SetWarning(2, warning);
	}
	warning = CheckWarning3();
	if (warning) {
		SetWarning(3, warning);
	}
}

// 0x476f90
int CAlchemyMPBreak::SumSelectionMP(void) {
	int sum = 0;
	for (std::map<int, AlchemyBreakItem>::iterator it = m_selection.begin(); it != m_selection.end(); ++it) {
		int mp = 0;
		if (m_state == 3) {
			mp = GetItemMP(it->second.typeId) * it->second.count;
		}
		sum += mp;
	}
	return sum;
}

// 0x477270
void CAlchemyMPBreak::UpdatePoints(void) {
	m_heldMP = GetHeldCountA6();
	m_selectionMP = SumSelectionMP();
	m_expectMP = SumSelectionMP() + GetHeldCountA7();
}

// 0x4772a0
void CAlchemyMPBreak::UpdateOverLimit(void) {
	int selectionMP = SumSelectionMP();
	int held = GetHeldCountA7();
	m_overLimit = false;
	if (selectionMP + held >= 1000) {
		m_overLimit = true;
	}
}

// 0x4772d0
int CAlchemyMPBreak::RemoveSelection(const AlchemyBreakItem& item, int* removedCount) {
	int key = MakeKey(item);
	if (key == 0) {
		return 0;
	}
	std::map<int, AlchemyBreakItem>::iterator it = m_selection.find(key);
	if (it == m_selection.end()) {
		return 0;
	}
	int count = it->second.count;
	*removedCount = count;
	it->second.count -= count;
	int left = it->second.count;
	if (left <= 0) {
		m_selection.erase(it);
	}
	return left;
}

// 0x477340
AlchemyBreakItem* CAlchemyMPBreak::FindSelection(const AlchemyBreakItem& item) {
	int key = MakeKey(item);
	if (key == 0) {
		return NULL;
	}
	std::map<int, AlchemyBreakItem>::iterator it = m_selection.find(key);
	if (it == m_selection.end()) {
		return NULL;
	}
	return &it->second;
}

// 0x477600: the vtable 0xcfe5d0 (not modeled), the selection map, the
// warnings; the original also zeroes +0x20 (not declared).
// m_state is left as it is.
CAlchemyMPBreak::CAlchemyMPBreak(void) {
	ClearWarnings();
	m_heldMP = 0;
	m_selectionMP = 0;
	m_expectMP = 0;
	m_resultMP = 0;
	m_resultBonus = 0;
	m_overLimit = false;
}

// 0x4776c0: 0x477570 is the map's operator[], 0x477010 its find.
AlchemyBreakItem* CAlchemyMPBreak::AddSelection(const AlchemyBreakItem& item) {
	AlchemyBreakItem copy = item;
	int key = MakeKey(item);
	if (key == 0) {
		return NULL;
	}
	AlchemyBreakItem* selected = FindSelection(item);
	if (selected) {
		return selected;
	}
	if (m_selection.size() <= 4) {
		m_selection[key] = copy;
		return &m_selection.find(key)->second;
	}
	return NULL;
}

// 0x4f9980
std::map<int, AlchemyBreakItem> CAlchemyMPBreak::GetSelection(void) {
	return m_selection;
}

// 0x4f31d0: FrForm's constructor; the original then zeroes +0x110..+0x13f
// (the help form's members, not declared by the port).
FrMPBreakAboutDlg::FrMPBreakAboutDlg(void) {
}

// 0x4f3250 (0x4f4160 is the deleting destructor): FrForm's destructor only.
FrMPBreakAboutDlg::~FrMPBreakAboutDlg(void) {
}

// 0x4f6680
bool FrAlchemyMPBreakDlg::OnBreakNoticeResult(int result, FrForm*) {
	if (result == 1) {
		if (m_breaker.HasWarning()) {
			m_breaker.ClearWarnings();
			OpenBreakNotice();
		} else {
			m_breaker.UpdateOverLimit();
			m_breaker.SendBreak();
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
		}
	}
	// The original also clears it after reopening the notice.
	m_pNotice = NULL;
	return true;
}

// 0x4f6740: the four warning lines, a line break, then either the warning
// footer 0x18709 or the line 0x18704 and the price line 0x18710 (MakeStr,
// the KR commonutil ring, with the 64-bit price); the title 0x1870a.
void FrAlchemyMPBreakDlg::OpenBreakNotice(void) {
	m_pNotice = CreateForm<FrAlchemyMPBreakNoticeDlg>(g_pFresh->GetManager(), this, "AlchemyMPBreakNoticeDlg", 0);
	for (int i = 0; i < 4; i++) {
		m_pNotice->AddMsgText(m_breaker.GetWarningText(i));
	}
	m_pNotice->AddMsgText("\n");

	const char* text;
	if (m_breaker.HasWarning()) {
		text = Jp_GetTableText(0x18709);
	} else {
		m_pNotice->AddMsgText(Jp_GetTableText(0x18704));
		text = MakeStr(Jp_GetTableText(0x18710), m_breaker.SumSelectionPrice());
	}
	m_pNotice->AddMsgText(text);
	m_pNotice->AddFocusText(Jp_GetTableText(0x1870a));
	m_pNotice->Open((FRESH_RESULT_CALLBACK)&FrAlchemyMPBreakDlg::OnBreakNoticeResult, 0x11);
}

// 0x4f6850: FrEdit::ClearLine, then the JP AddText(text, 0, 1, 0, 0).
void FrAlchemyMPBreakDlg::ShowTikiTalk(void) {
	if (m_pTikiTalkEdit == NULL) {
		return;
	}
	m_pTikiTalkEdit->ClearLine();
	m_pTikiTalkEdit->AddText(m_tikiTalk[m_tikiTalkIndex].c_str(), false, true);
}

// 0x4f8650
void FrAlchemyMPBreakDlg::UpdateTexts(void) {
	m_tikiTalk[0] = Jp_GetTableText(PickTextId(0x186a1, 0x186aa));
	m_tikiTalk[1] = Jp_GetTableText(PickTextId(0x186ab, 0x186b4));
	m_tikiTalk[2] = Jp_GetTableText(PickTextId(0x186b5, 0x186be));
	m_tikiTalk[3] = Jp_GetTableText(PickTextId(0x186bf, 0x186c8));
}

// 0x4f8730
void FrAlchemyMPBreakDlg::UpdateView(int tikiTalkIndex) {
	m_tikiTalkIndex = tikiTalkIndex;
	UpdateTexts();
	ShowTikiTalk();
}

// 0x4f99b0: FrWnd::Enable (slot +0x18) on a copy of the selection.
void FrAlchemyMPBreakDlg::UpdateExchangeOk(void) {
	if (m_pExchangeOk && m_viewState == 3) {
		std::map<int, AlchemyBreakItem> selection = m_breaker.GetSelection();
		m_pExchangeOk->Enable(selection.size() != 0);
	}
}

// 0x4f9a50: the original inlines the held-count lookup of 0x475f40 (an
// unsigned compare with 19999).
void FrAlchemyMPBreakDlg::OnExchangeOkClick(void) {
	std::map<int, AlchemyBreakItem> selection = m_breaker.GetSelection();
	if (selection.size() != 0) {
		if ((unsigned int)m_breaker.GetHeldCountA6() > 19999) {
			const char* text = Jp_GetTableText(0x1870e);
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0));
		} else {
			m_breaker.CheckWarnings();
			OpenBreakNotice();
		}
	}
}

// 0x4f89e0: the break's result into the CAlchemyMPBreak, its points, then
// the Tiki talk of text 3 (over the limit) or 2.
void FrAlchemyMPBreakDlg::OnBreakResult(int mp, int bonus) {
	m_breaker.SetResult(mp, bonus);
	m_breaker.UpdatePoints();
	if (m_breaker.m_overLimit) {
		UpdateView(3);
	} else {
		UpdateView(2);
	}
}

// 0x4f8b10: FrPointShopDlg::DrawItemRow's shape (0x84eda0) with the MP:
// the record (by kind: part, club set and ball with "bar_", item, kind
// 0x1c, card; JP +0x8 name, +0x49 icon, word +0x8c MP), a part's
// character base image (0x4ceb40), the icon, the name cut to `column` - 1
// characters and "...", then "x<count>" (the count in view state 3), the
// MP and "M" right-aligned at the "Own_item" row width. JP 0xbed2f0 is KR
// Print11, 0x98bdc0 strncpy.
void FrAlchemyMPBreakDlg::DrawItemRow(FrListItem* item, float height, int column, int count) {
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (item == NULL || gdi == NULL) {
		return;
	}
	gdi->SetTextColor(0xff000000, 0xffffffff);
	gdi->SetTextStyle(0);
	unsigned int typeId = *static_cast<const unsigned int*>(item->pData);
	std::string icon("");
	std::string name("");
	int characterId = 0;
	int mp = 0;
	int shownCount = 0;
	const IFF_STRUCT::sItem* record = NULL;
	switch (typeId >> 26) {
	case 2:
		record = reinterpret_cast<const IFF_STRUCT::sItem*>(JpItemManager_FindPart(typeId));
		if (record) {
			icon = record->m_icon;
			name = record->m_name;
			characterId = (typeId >> 18) & 0xff;
			mp = JpIffItemBase_GetBreakMP(reinterpret_cast<const JpIffItemBase*>(record));
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
			mp = JpIffItemBase_GetBreakMP(reinterpret_cast<const JpIffItemBase*>(record));
		}
		if (icon.length() != 0) {
			icon = std::string("bar_") + icon;
		}
		break;
	case 6:
	case 0x1c:
	case 0x1f:
		if (typeId >> 26 == 6) {
			record = reinterpret_cast<const IFF_STRUCT::sItem*>(JpItemManager_FindItem(typeId));
		} else if (typeId >> 26 == 0x1f) {
			record = reinterpret_cast<const IFF_STRUCT::sItem*>(JpItemManager_FindCard(typeId));
		} else {
			record = reinterpret_cast<const IFF_STRUCT::sItem*>(JpItemManager_FindCommonItem(typeId));
		}
		if (record) {
			icon = record->m_icon;
			name = record->m_name;
			mp = JpIffItemBase_GetBreakMP(reinterpret_cast<const JpIffItemBase*>(record));
		}
		break;
	default:
		break;
	}
	if (m_viewState == 3) {
		shownCount = count;
	}
	WRect rect;
	rect.x = item->pos.x + 5.0;
	rect.y = item->pos.y + 4.5;
	rect.w = 0.0f;
	rect.h = 0.0f;
	// A part: the character's base image under the icon.
	if ((typeId & 0xfc000000) == 0x08000000) {
		std::string base("");
		base = Jp_GetCharacterIconBaseName(characterId);
		if (base.length() != 0) {
			const Bitmap* bmp = g_pFresh->GetBitmap(base.c_str());
			if (bmp) {
				float scale = height / (bmp->Height() * 1.4f);
				if (scale > 1.0f) {
					scale = 1.0f;
				}
				rect.w = bmp->Width() * scale;
				rect.h = bmp->Height() * scale;
				gdi->DrawTexture(bmp, rect, 0xffffffff, 0);
			}
		}
	}
	if (icon.length() != 0) {
		const Bitmap* bmp = g_pFresh->GetBitmap(icon.c_str());
		if (bmp) {
			float scale = height / (bmp->Height() * 1.4f);
			scale = scale + ((typeId & 0xfc000000) == 0x18000000 ? scale * 0.5 : 0.0);
			if (scale > 1.0f) {
				scale = 1.0f;
			}
			rect.x = scale + scale + rect.w + rect.x;
			rect.w = bmp->Width() * scale;
			rect.h = bmp->Height() * scale;
			gdi->DrawTexture(bmp, rect, 0xffffffff, 0);
		}
	}
	if (name.length() != 0) {
		if (name.length() > static_cast<unsigned int>(column)) {
			char buf[0x20] = { 0 };
			strncpy(buf, name.c_str(), column - 1);
			name = buf;
			name.append("...", 3);
		}
		rect.x = height * 1.6f + item->pos.x;
		WPoint pos(rect.x, (height - 11.0) * 0.35f + rect.y);
		gdi->Print11(pos, 0, name.c_str());
	}
	float listWidth = 0.0f;
	if (m_pOwnItem) {
		listWidth = (float)m_pOwnItem->GetItemWidth();
	}
	std::string countText = MakeStr("x%d", shownCount);
	std::string mpText = MakeStr("%d", mp);
	std::string unitText("M", 1);
	gdi->GetTextExtend(countText.c_str());
	float mpWidth = gdi->GetTextExtend(mpText.c_str());
	float unitWidth = gdi->GetTextExtend(unitText.c_str());
	float textY = (height - 11.0) * 0.35f + rect.y;
	WPoint unitPos(item->pos.x + listWidth, textY);
	WPoint countPos(listWidth * 0.5 + rect.x, textY);
	WPoint mpPos(unitPos.x - (unitWidth + mpWidth), textY);
	gdi->SetTextColor(0xff871ba4, 0xffffffff);
	gdi->Print(countPos, 2, MakeStr("x%d", shownCount));
	gdi->SetTextStyle(1);
	gdi->SetTextColor(0xffd31b9f, 0xffffffff);
	gdi->Print(mpPos, 0, mpText.c_str());
	gdi->SetTextColor(0xff871ba4, 0xffffffff);
	gdi->Print(unitPos, 2, unitText.c_str());
}

// 0x4f9500: the selected row over "list_button_o_left", then the row
// with the owned count.
void FrAlchemyMPBreakDlg::OnOwnItemDraw(intptr_t param) {
	FrListItem* item = (FrListItem*)param;
	if (item == NULL) {
		return;
	}
	float w = (float)m_pOwnItem->GetItemWidth();
	float h = (float)m_pOwnItem->GetItemHeight();
	if (m_pOwnItem->GetSelected() == item) {
		DrawBitmap(m_pListButtonLeft, WRect(item->pos.x + 2.0, item->pos.y + 2.0, w, h), 0xffffffff, true);
	}
	OwnItem* entry = FindOwnItem(*static_cast<const unsigned int*>(item->pData));
	if (entry) {
		DrawItemRow(item, h + 1.0f, 0xe, entry->count);
	}
}

// 0x4f95c0: the selected row over "list_button_o_right", then the row
// with the selected count.
void FrAlchemyMPBreakDlg::OnExchangeItemDraw(intptr_t param) {
	FrListItem* item = (FrListItem*)param;
	if (item == NULL) {
		return;
	}
	float w = (float)m_pExchangeItem->GetItemWidth();
	float h = (float)m_pExchangeItem->GetItemHeight();
	if (m_pExchangeItem->GetSelected() == item) {
		DrawBitmap(m_pListButtonRight, WRect(item->pos.x + 2.0, item->pos.y + 2.0, w, h), 0xffffffff, true);
	}
	unsigned int typeId = *static_cast<const unsigned int*>(item->pData);
	OwnItem* entry = FindOwnItem(typeId);
	if (entry == NULL || entry->itemCounts.empty()) {
		return;
	}
	AlchemyBreakItem key;
	key.itemId = entry->itemCounts.begin()->first;
	key.typeId = typeId;
	key.count = 0;
	AlchemyBreakItem* selected = m_breaker.FindSelection(key);
	if (selected == NULL) {
		return;
	}
	DrawItemRow(item, h + 1.0f, 0x10, selected->count);
}

// 0x4f9d50 (ret 8, always true): on result 1 the chosen count of the row's
// first item id goes into the selection like OnOwnItemClick's one item;
// the respect gauge follows the expected MP (JpMaskGauge 0x9235e0), full
// from 1000 (0x923520).
bool FrAlchemyMPBreakDlg::OnCountSetResult(int result, FrForm*) {
	if (m_pCountSetDlg == NULL) {
		return true;
	}
	if (result == 1) {
		if (m_pCountSetDlg->GetCount() < 1) {
			m_pCountSetDlg = NULL;
			return true;
		}
		FrListItem* row = m_pCountSetDlg->GetItem();
		unsigned int typeId = *static_cast<const unsigned int*>(row->pData);
		OwnItem* entry = FindOwnItem(typeId);
		if (entry == NULL || entry->itemCounts.empty()) {
			m_pCountSetDlg = NULL;
			return true;
		}
		AlchemyBreakItem key;
		key.itemId = entry->itemCounts.begin()->first;
		key.typeId = typeId;
		key.count = 0;
		AlchemyBreakItem* item = m_breaker.AddSelection(key);
		if (item) {
			item->count += m_pCountSetDlg->GetCount();
			int count = m_pCountSetDlg->GetCount();
			entry->count -= count;
			if (entry->count < 1) {
				m_pOwnItem->DelItem(row->pData);
			}
			if (m_pRespectGauge) {
				m_breaker.UpdatePoints();
				m_pRespectGauge->SetTargetFromCurrent((float)m_breaker.m_expectMP);
				if (m_breaker.m_expectMP >= 1000) {
					if (m_pFullGauge) {
						m_pFullGauge->SetTarget(1000.0f);
					}
				}
			}
			UpdateView(1);
			if (m_pExchangeItem->FindItem(&item->typeId) == NULL) {
				m_pExchangeItem->AddItem(&item->typeId);
			}
		}
	}
	UpdateExchangeOk();
	m_pCountSetDlg = NULL;
	return true;
}

// 0x4f9ed0: the dialog is created once (owner this, the FrCmdTarget base)
// and opened with OnCountSetResult and 3.
void FrAlchemyMPBreakDlg::OpenCountSetDlg(FrListItem* item) {
	if (m_pCountSetDlg == NULL) {
		m_pCountSetDlg = CreateItemCountController(g_pFresh->GetManager(), this, "MPBreakCountSetDlg", NULL);
	}
	if (m_pCountSetDlg == NULL) {
		return;
	}
	m_pCountSetDlg->SetItem(item, GetOwnItemCount(item));
	m_pCountSetDlg->Open((FRESH_RESULT_CALLBACK)&FrAlchemyMPBreakDlg::OnCountSetResult, 3);
}

// 0x4f9ba0: the selected row's items go back to the owned entry (the row
// leaves "Exchange_item" when none is left), the gauges follow the
// expected MP (the full gauge empty below 1000), and the entry is listed
// again in "Own_item" when it belongs to the item tab (FrListBox::DelItem
// 0xbf0d70, FindItem 0xbefe40, JpMaskGauge::SetTarget 0x923520).
void FrAlchemyMPBreakDlg::OnExchangeItemClick(void) {
	FrListItem* selected = m_pExchangeItem->GetSelected();
	if (selected == NULL) {
		return;
	}
	unsigned int typeId = *static_cast<const unsigned int*>(selected->pData);
	OwnItem* entry = FindOwnItem(typeId);
	if (entry == NULL || entry->itemCounts.empty()) {
		return;
	}
	AlchemyBreakItem key;
	key.itemId = entry->itemCounts.begin()->first;
	key.typeId = typeId;
	key.count = 0;
	int removed = 0;
	if (m_breaker.RemoveSelection(key, &removed) == 0) {
		m_pExchangeItem->DelItem(selected->pData);
	}
	entry->count += removed;
	m_breaker.UpdatePoints();
	if (m_breaker.m_expectMP < 1000) {
		if (m_pFullGauge) {
			m_pFullGauge->SetTarget(0.0f);
		}
	}
	if (m_pRespectGauge) {
		m_pRespectGauge->SetTarget((float)m_breaker.m_expectMP);
	}
	if (m_pOwnItem->FindItem(entry) == NULL) {
		if (m_itemTab == m_breaker.GetItemTab(entry->typeId)) {
			m_pOwnItem->AddItem(entry);
		}
	}
	UpdateExchangeOk();
}

// 0x4f9f60: one item of the selected row's first item id goes into the
// selection (a stack of 5 or more opens the count dialog instead); the
// row leaves "Own_item" when none is left, the gauge state restarts and
// "Exchange_item" lists the selected entry (its data starts with the
// type id, like the owned entries').
void FrAlchemyMPBreakDlg::OnOwnItemClick(void) {
	FrListItem* selected = m_pOwnItem->GetSelected();
	if (selected == NULL) {
		return;
	}
	unsigned int typeId = *static_cast<const unsigned int*>(selected->pData);
	OwnItem* entry = FindOwnItem(typeId);
	if (entry == NULL || entry->itemCounts.empty()) {
		return;
	}
	if (entry->count >= 5) {
		OpenCountSetDlg(selected);
		return;
	}
	AlchemyBreakItem key;
	key.itemId = entry->itemCounts.begin()->first;
	key.typeId = typeId;
	key.count = 0;
	AlchemyBreakItem* item = m_breaker.AddSelection(key);
	if (item == NULL) {
		return;
	}
	item->count += 1;
	m_breaker.UpdatePoints();
	entry->count -= 1;
	if (entry->count < 1) {
		m_pOwnItem->DelItem(selected->pData);
	}
	m_gaugeState = 1;
	if (m_pExchangeItem->FindItem(&item->typeId) == NULL) {
		m_pExchangeItem->AddItem(&item->typeId);
	}
	m_exchangeAdded = true;
	UpdateExchangeOk();
	UpdateView(1);
}

// 0x4fab30: 0x4f8820 is the item map's operator[], 0x4fa310 the list's
// node insertion.
unsigned int FrAlchemyMPBreakDlg::AddOwnItem(unsigned int typeId, int itemId, unsigned int count, bool atFront) {
	OwnItem* entry = FindOwnItem(typeId);
	if (entry == NULL) {
		OwnItem item;
		item.typeId = typeId;
		item.count = count;
		if (itemId != 0) {
			item.itemCounts[itemId] = (int)count;
		}
		if (atFront) {
			m_ownItems.push_front(item);
		} else {
			m_ownItems.push_back(item);
		}
		return count;
	}
	entry->count += count;
	if (itemId != 0) {
		entry->itemCounts[itemId] = (int)count;
	}
	return entry->count;
}

// 0x4faca0: the owned records of the inventory (JP CSharedDoc +0x3734 KR
// m_myPartsList, +0x3750 m_myGearList, +0x375c m_itemInfoList, +0x3744
// m_myItemMap, +0x37e0 m_myCardList as in FrPointShopDlg 0x850f10; +0x37bc
// is KR m_myExtraItemList3: the three 0xc lists before the cards, KR
// +0x1218, +0x1224, +0x1230, sit at JP +0x37bc, +0x37c8, +0x37d4). The
// JP +0x3878 list (sItemInfo records, count 1, walked between +0x37bc and
// +0x3750) has no KR member and is left out. Offline (0x633000 =
// OnlinePlay() == 0) the 20 type ids of the original's table are tested
// without a record, which IsBreakable refuses.
void FrAlchemyMPBreakDlg::CollectOwnItems(void) {
	for (std::list<OwnItem>::iterator it = m_ownItems.begin(); it != m_ownItems.end(); ++it) {
		it->itemCounts.clear();
	}
	m_ownItems.clear();
	if (m_viewState != 3) {
		return;
	}
	if (OnlinePlay() == 0) {
		uint32_t offlineItems[20] = {
			0x18000006, 0x18000009, 0x1800000a, 0x1800000b, 0x1800000e,
			0x18000010, 0x18000011, 0x18000027, 0x18000028, 0x1800002e,
			0x1a000000, 0x1a000002, 0x1a00000a, 0x1a00000b, 0x1a00000d,
			0x1a00000e, 0x1a000011, 0x1a000040, 0x1a00004f, 0x1a0000c7,
		};
		for (unsigned int i = 0; i < 20; i++) {
			uint32_t typeId = offlineItems[i];
			if (IsBreakable(typeId, NULL)) {
				AddOwnItem(typeId, 1, 0x32, false);
			}
		}
		return;
	}
	CSharedDoc* doc = CSharedDoc::Instance();
	for (std::list<sItemInfo>::iterator it = doc->m_myPartsList.begin(); it != doc->m_myPartsList.end(); ++it) {
		sItemInfo* info = &*it;
		if (info->m_typeId != 0 && info->m_stateBits.kind == 0 && IsBreakable(info->m_typeId, info)) {
			AddOwnItem(info->m_typeId, info->m_serial, 1, false);
		}
	}
	for (std::list<sItemInfo>::iterator it = doc->m_myExtraItemList3.begin(); it != doc->m_myExtraItemList3.end(); ++it) {
		sItemInfo* info = &*it;
		if (info->m_typeId != 0 && info->m_stateBits.kind == 0 && IsBreakable(info->m_typeId, info)) {
			AddOwnItem(info->m_typeId, info->m_serial, (unsigned int)info->m_count, false);
		}
	}
	for (std::list<sItemInfo>::iterator it = doc->m_myGearList.begin(); it != doc->m_myGearList.end(); ++it) {
		sItemInfo* info = &*it;
		if (info->m_typeId != 0 && info->m_stateBits.kind == 0 && IsBreakable(info->m_typeId, info)) {
			AddOwnItem(info->m_typeId, info->m_serial, (unsigned int)info->m_count, false);
		}
	}
	for (std::list<sItemInfo>::iterator it = doc->m_itemInfoList.begin(); it != doc->m_itemInfoList.end(); ++it) {
		sItemInfo* info = &*it;
		if (info->m_typeId != 0 && info->m_stateBits.kind == 0 && IsBreakable(info->m_typeId, info)) {
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
			AddOwnItem(typeId, info->m_serial, (unsigned int)count, false);
		}
	}
	for (std::map<unsigned int, sItemInfo>::iterator it = doc->m_myItemMap.begin(); it != doc->m_myItemMap.end(); ++it) {
		sItemInfo* info = &it->second;
		if (info->m_typeId != 0 && info->m_stateBits.kind == 0 && IsBreakable(info->m_typeId, info)) {
			AddOwnItem(info->m_typeId, info->m_serial, 1, false);
		}
	}
	for (std::list<sCards>::iterator it = doc->m_myCardList.begin(); it != doc->m_myCardList.end(); ++it) {
		sCards* card = &*it;
		if (card->m_typeId != 0 && IsBreakable(card->m_typeId, card)) {
			AddOwnItem(card->m_typeId, card->m_guid, (unsigned int)card->m_count, false);
		}
	}
}

// 0x4fb2c0: FrWnd::SetVisible (slot +0x1c), FrListBox::ClearItem 0xbf0e50.
void FrAlchemyMPBreakDlg::SetViewState(int state) {
	m_viewState = state;
	m_breaker.SetState(state);
	bool visible = false;
	if (state == 3) {
		visible = true;
	}

	if (m_pMagicBox) {
		m_pMagicBox->SetVisible(true);
	}
	if (m_pCardCompose) {
		m_pCardCompose->SetVisible(true);
	}
	if (m_pExchange) {
		m_pExchange->SetVisible(true);
	}
	if (m_pMyTPPoint) {
		m_pMyTPPoint->SetVisible(visible);
	}
	if (m_pCookieItem) {
		m_pCookieItem->SetVisible(visible);
	}
	if (m_pPangItem) {
		m_pPangItem->SetVisible(visible);
	}
	if (m_pRareItem) {
		m_pRareItem->SetVisible(visible);
	}
	for (int i = 0; i < 4; i++) {
		if (m_pExchangeListBG[i]) {
			m_pExchangeListBG[i]->SetVisible(visible);
		}
	}
	if (m_pMPGauge) {
		m_pMPGauge->SetVisible(visible);
	}
	if (m_pExpectMP) {
		m_pExpectMP->SetVisible(visible);
	}
	if (m_pPrePoint) {
		m_pPrePoint->SetVisible(visible);
	}
	if (m_pExchangeOk) {
		m_pExchangeOk->SetVisible(visible);
	}
	if (m_pOwnItem) {
		m_pOwnItem->SetVisible(visible);
	}
	if (m_pOwnItem) {
		m_pOwnItem->ClearItem();
	}
	if (m_pExchangeItem) {
		m_pExchangeItem->SetVisible(visible);
	}
	if (m_pExchangeItem) {
		m_pExchangeItem->ClearItem();
	}
	CollectOwnItems();
	FillOwnItemList();
}
