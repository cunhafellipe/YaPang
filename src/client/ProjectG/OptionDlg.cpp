// Original: d:\build\custom\temp\code\client\projectg\optiondlg.cpp
#include "NetworkSystem.h"
#include "OptionDlg.h"
#include "ClientSetting.h"
#include "ObjectFactory.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frbutton.h"
#include "frstatic.h"
#include "fredit.h"
#include "frcombobox.h"
#include "frgaugebar.h"
#include "ProjectG.h"
#include "SharedDoc.h"
#include "SoundManager.h"
#include "SchoolDlg.h"
#include "TaskManager.h"
#include "GolfTask.h"
#include "ChatMsg.h"
#include "messengerdlg.h"
#include "LobbyTask.h"
#include "Localize.h"
#include "InputManager.h"
#include "wresrcmng.h"
#include "commonutil.h"
#include "wutil.h"
#include "mousecursor.h"
#include "shared/s5/SharedUtilities.h"
#include "binstr.h"
#include <string.h>
#include <algorithm>

// The gateway actor's name (each unit has its own copy).
static const char* s_gatewayActorName = "Gateway";

// clientsetting.h:386
float COption::gGetMouseSensitivity(void) const {
	return m_option.m_mouseSensitivity;
}

// optiondlg.h:261
void FrOptionDlg::sTab::Init(void) {
}

// optiondlg.cpp:125
FrOptionDlg::sTab::sTab(void) : m_bSelected(true), m_pTabBtn(0), m_numWnds(0), m_ppWnds(0), m_pppHoverWnds(0) {
}

// optiondlg.cpp:130
void FrOptionDlg::sTab::SetSelected(bool bSelected) {
	m_bSelected = bSelected;

	if (!bSelected) {
		m_pTabBtn->SetStatus(FrButton::NORMAL);
	}

	if (m_ppWnds) {
		for (int i = 0; i < m_numWnds; i++) {
			if (m_ppWnds[i]) {
				if (m_pbDisabled && m_pbDisabled[i]) {
					m_ppWnds[i]->Enable(false);
				} else {
					m_ppWnds[i]->Enable(m_bSelected);
				}

				m_ppWnds[i]->SetVisible(m_bSelected);
			}
		}
	}

	if (m_pppHoverWnds) {
		for (int i = 0; i < m_numWnds; i++) {
			if (m_pppHoverWnds[i] && *m_pppHoverWnds[i]) {
				if (m_pbDisabled && m_pbDisabled[i]) {
					(*m_pppHoverWnds[i])->Enable(false);
				} else {
					(*m_pppHoverWnds[i])->Enable(m_bSelected);
				}

				(*m_pppHoverWnds[i])->EnableHover(m_bSelected, 0);
				(*m_pppHoverWnds[i])->SetVisible(m_bSelected);
			}
		}
	}
}

// optiondlg.cpp:173
FrOptionDlg::sGameTab::sGameTab(void) {
	m_numWnds = 13;
	m_ppWnds = (FrWnd**)m_pStatics;
	m_pppHoverWnds = m_pHoverWnds;
	m_pbDisabled = 0;

	memset(m_pStatics, 0, sizeof(m_pStatics));

	m_pMsSensiGauge = 0;
	m_pWhisperBtn = 0;
	m_pInvitationBtn = 0;
	m_pFriendConfirmBtn = 0;
	m_pTransChatWinBtn = 0;
	m_pUiEffectBtn = 0;
	m_pNickEdit = 0;
	m_pChangeNickBtn = 0;
	m_pSchoolEdit = 0;
	m_pChangeSchoolBtn = 0;
	m_pPowerGaugeBtn = 0;
	m_pHideGUIBtn = 0;
	m_pHidePIBtn = 0;
	// m_pTerrainBtn is not cleared (the original skips it).
	m_pCutinDisplayBtn = 0;

	m_pHoverWnds[0] = (FrWnd**)&m_pMsSensiGauge;
	m_pHoverWnds[1] = (FrWnd**)&m_pWhisperBtn;
	m_pHoverWnds[2] = (FrWnd**)&m_pInvitationBtn;
	m_pHoverWnds[3] = (FrWnd**)&m_pFriendConfirmBtn;
	m_pHoverWnds[4] = (FrWnd**)&m_pTransChatWinBtn;
	m_pHoverWnds[5] = (FrWnd**)&m_pUiEffectBtn;
	m_pHoverWnds[6] = (FrWnd**)&m_pNickEdit;
	m_pHoverWnds[7] = (FrWnd**)&m_pSchoolEdit;
	m_pHoverWnds[8] = (FrWnd**)&m_pPowerGaugeBtn;
	m_pHoverWnds[9] = (FrWnd**)&m_pHideGUIBtn;
	m_pHoverWnds[10] = (FrWnd**)&m_pHidePIBtn;
	m_pHoverWnds[11] = (FrWnd**)&m_pTerrainBtn;

	m_pHoverWnds[12] = (FrWnd**)&m_pCutinDisplayBtn;

	m_bEnableChangeNick = false;
}

// optiondlg.cpp:216
void FrOptionDlg::sGameTab::SetSelected(bool bSelected) {
	sTab::SetSelected(bSelected);

	if (bSelected) {
		if (m_pChangeNickBtn) {
			m_pChangeNickBtn->SetVisible(true);
			m_pChangeNickBtn->Enable(m_bEnableChangeNick);
		}

		if (m_pChangeSchoolBtn) {
			m_pChangeSchoolBtn->SetVisible(true);
		}
	} else {
		if (m_pChangeNickBtn) {
			m_pChangeNickBtn->SetVisible(false);
		}

		if (m_pChangeSchoolBtn) {
			m_pChangeSchoolBtn->SetVisible(false);
		}
	}
}

// optiondlg.cpp:242
void FrOptionDlg::sGameTab::Init(void) {
	if (OnlinePlay() && !(((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI) : false))) {
		m_bEnableChangeNick = IsEnableChangeNickname();
	}
}

// optiondlg.cpp:250
bool FrOptionDlg::sGameTab::IsEnableChangeNickname(void) {
	if (IsLocalContent((localContentType_t)0x82)) {
		if (((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&CLobbyTask::m_RTTI) : false)
			&& (g_pFresh->IsCurrentLayout("GAMEROOM") || g_pFresh->IsCurrentLayout("GAMEROOM_EXT") || g_pFresh->IsCurrentLayout("GAMEROOM_EXTRES"))) {
			return false;
		}

		return true;
	}

	return false;
}

// optiondlg.cpp:278
FrOptionDlg::sVideoTab::sVideoTab(void) {
	m_numWnds = 13;
	m_ppWnds = (FrWnd**)m_pStatics;
	m_pppHoverWnds = m_pHoverWnds;
	m_pbDisabled = m_bDisabled;

	memset(m_pStatics, 0, sizeof(m_pStatics));
	memset(m_bDisabled, 0, sizeof(m_bDisabled));

	m_pFullScreenBtn = 0;
	m_pFillScreenBtn = 0;
	m_pWideModeBtn = 0;
	m_pOverallCmb = 0;
	m_pScrResCmb = 0;
	m_pTnlCmb = 0;
	m_pTexResCmb = 0;
	m_pShadowBtn = 0;
	m_pShadowmapBtn = 0;
	m_pReflectionBtn = 0;
	m_pLodBtn = 0;
	m_pNpcBtn = 0;
	m_pParticleCmb = 0;

	m_pHoverWnds[0] = (FrWnd**)&m_pFullScreenBtn;
	m_pHoverWnds[1] = (FrWnd**)&m_pFillScreenBtn;
	m_pHoverWnds[2] = (FrWnd**)&m_pWideModeBtn;
	m_pHoverWnds[3] = (FrWnd**)&m_pOverallCmb;
	m_pHoverWnds[4] = (FrWnd**)&m_pScrResCmb;
	m_pHoverWnds[5] = (FrWnd**)&m_pTnlCmb;
	m_pHoverWnds[6] = (FrWnd**)&m_pTexResCmb;
	m_pHoverWnds[7] = (FrWnd**)&m_pShadowBtn;
	m_pHoverWnds[8] = (FrWnd**)&m_pShadowmapBtn;
	m_pHoverWnds[9] = (FrWnd**)&m_pReflectionBtn;
	m_pHoverWnds[10] = (FrWnd**)&m_pLodBtn;
	m_pHoverWnds[11] = (FrWnd**)&m_pNpcBtn;
	m_pHoverWnds[12] = (FrWnd**)&m_pParticleCmb;

	// Options the video card cannot do are greyed out.
	if (!sOption::IsSupportShadow()) {
		m_bDisabled[7] = true;
	}

	if (!sOption::IsSupportReflection()) {
		m_bDisabled[9] = true;
	}

	if (!sOption::IsSupportLod()) {
		m_bDisabled[10] = true;
	}
}

// optiondlg.cpp:330
FrOptionDlg::sAudioTab::sAudioTab(void) {
	m_numWnds = 5;
	m_ppWnds = (FrWnd**)m_pStatics;
	m_pppHoverWnds = m_pHoverWnds;
	m_pbDisabled = m_bDisabled;

	memset(m_pStatics, 0, sizeof(m_pStatics));

	if (g_audio->GetMSS()) {
		memset(m_bDisabled, 0, sizeof(m_bDisabled));
	} else {
		memset(m_bDisabled, 1, sizeof(m_bDisabled));
	}

	m_pEnableBtn = 0;
	m_pOverallCmb = 0;
	m_pSfxGauge = 0;
	m_pBgmGauge = 0;
	m_pSpeakerCmb = 0;

	m_pHoverWnds[0] = (FrWnd**)&m_pEnableBtn;
	m_pHoverWnds[1] = (FrWnd**)&m_pOverallCmb;
	m_pHoverWnds[2] = (FrWnd**)&m_pSfxGauge;
	m_pHoverWnds[3] = (FrWnd**)&m_pBgmGauge;
	m_pHoverWnds[4] = (FrWnd**)&m_pSpeakerCmb;
}

// optiondlg.cpp:359
FrOptionDlg::sMacroTab::sMacroTab(void) {
	m_numWnds = 8;
	m_ppWnds = (FrWnd**)m_pStatics;
	m_pppHoverWnds = m_pHoverWnds;
	m_pbDisabled = m_bDisabled;

	memset(m_pStatics, 0, sizeof(m_pStatics));
	memset(m_bDisabled, 0, sizeof(m_bDisabled));

	for (int i = 0; i < 8; i++) {
		m_pMacroEdits[i] = 0;
		m_pHoverWnds[i] = (FrWnd**)&m_pMacroEdits[i];
	}
}

// optiondlg.cpp:381 (run-time type and factory registration of FrChangeNickDlg)
const WRTTI FrChangeNickDlg::m_RTTI("FrChangeNickDlg", &FrForm::m_RTTI);

IObject* FrChangeNickDlgMakeInstance(void) {
	return new FrChangeNickDlg;
}

struct __sFrChangeNickDlg {
	__sFrChangeNickDlg(void) {
		ObjectFactory().AddObjectFunctor(FrChangeNickDlgMakeInstance, "FrChangeNickDlg");
	}
};
static __sFrChangeNickDlg s_registerFrChangeNickDlg;

// optiondlg.cpp:383
const sFRESH_MSGMAP* FrChangeNickDlg::GetMessageMap(void) const {
	return &FrChangeNickDlg::_MsgMap;
}

sFRESH_MSGMAP FrChangeNickDlg::_MsgMap = { &FrForm::_MsgMap, &FrChangeNickDlg::_MsgEntries[0] };

sFRESH_ENTRY FrChangeNickDlg::_MsgEntries[] = {
	// optiondlg.cpp:385
	{ "sprice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeNickDlg::OnPriceStaticInit },
	// optiondlg.cpp:386
	{ "price", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeNickDlg::OnPriceInit },
	// optiondlg.cpp:387
	{ "smycookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeNickDlg::OnMyCookieStaticInit },
	// optiondlg.cpp:388
	{ "mycookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeNickDlg::OnMyCookieInit },
	// optiondlg.cpp:389
	{ "sremaincookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeNickDlg::OnRemainCookieStaticInit },
	// optiondlg.cpp:390
	{ "remaincookie", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeNickDlg::OnRemainCookieInit },
	// optiondlg.cpp:391
	{ "checkout", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrChangeNickDlg::OnCheckoutInit },
	// optiondlg.cpp:392
	{ "checkout", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrChangeNickDlg::OnCheckoutBtnUp },

	// optiondlg.cpp:394
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// optiondlg.cpp:398
FrChangeNickDlg::FrChangeNickDlg(void) : m_szNick(0) {
	CSharedDoc* pDoc = CSharedDoc::Instance();
	std::list<sItemInfo>::iterator it = std::find(pDoc->m_itemInfoList.begin(), pDoc->m_itemInfoList.end(), 0x1A000003);

	m_bHasTicket = it != pDoc->m_itemInfoList.end();

	m_pOptionDlg = 0;
}

// optiondlg.cpp:414
void FrChangeNickDlg::OnPriceStaticInit(int param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (!pStatic) {
		return;
	}

	if (m_bHasTicket || (CSharedDoc::Instance()->GetPangyaUserInfo().identity & 0x204)) {
		pStatic->SetVisible(false);
	}
}

// optiondlg.cpp:430
void FrChangeNickDlg::OnPriceInit(intptr_t param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (!pStatic) {
		return;
	}

	if (m_bHasTicket || (CSharedDoc::Instance()->GetPangyaUserInfo().identity & 0x204)) {
		pStatic->SetVisible(false);
	} else {
		// 49 cookies.
		if (IsLocalCountry(30)) {
			pStatic->SetCaption(MakeStr("%d\xB0\xB3(CK)", 49));
		} else {
			pStatic->SetCaption(MakeStr("%d\xB0\xB3", 49));
		}
	}
}

// optiondlg.cpp:454
void FrChangeNickDlg::OnMyCookieStaticInit(int param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (!pStatic) {
		return;
	}

	if (m_bHasTicket || (CSharedDoc::Instance()->GetPangyaUserInfo().identity & 0x204)) {
		pStatic->SetVisible(false);
	}
}

// optiondlg.cpp:470
void FrChangeNickDlg::OnMyCookieInit(int param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (!pStatic) {
		return;
	}

	if (m_bHasTicket || (CSharedDoc::Instance()->GetPangyaUserInfo().identity & 0x204)) {
		pStatic->SetVisible(false);
	} else {
		if (IsLocalCountry(30)) {
			pStatic->SetCaption(MakeStr("%I64d\xB0\xB3(CK)", CSharedDoc::Instance()->m_myCash));
		} else {
			pStatic->SetCaption(MakeStr("%I64d\xB0\xB3", CSharedDoc::Instance()->m_myCash));
		}
	}
}

// optiondlg.cpp:495
void FrChangeNickDlg::OnRemainCookieStaticInit(int param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (!pStatic) {
		return;
	}

	if (m_bHasTicket || (CSharedDoc::Instance()->GetPangyaUserInfo().identity & 0x204)) {
		pStatic->SetVisible(false);
	}
}

// optiondlg.cpp:511
void FrChangeNickDlg::OnRemainCookieInit(int param) {
	FrStatic* pStatic = DYNAMIC_CAST(FrStatic, param);
	if (!pStatic) {
		return;
	}

	if (m_bHasTicket || (CSharedDoc::Instance()->GetPangyaUserInfo().identity & 0x204)) {
		pStatic->SetVisible(false);
	} else {
		if (CSharedDoc::Instance()->m_myCash < 49) {
			// "%I64d cookies short".
			pStatic->SetCaption(MakeStr("%I64d\xB0\xB3 \xBA\xCE\xC1\xB7", 49 - CSharedDoc::Instance()->m_myCash));
		} else {
			if (IsLocalCountry(30)) {
				pStatic->SetCaption(MakeStr("%I64d\xB0\xB3(CK)", CSharedDoc::Instance()->m_myCash - 49));
			} else {
				pStatic->SetCaption(MakeStr("%I64d\xB0\xB3", CSharedDoc::Instance()->m_myCash - 49));
			}
		}
	}
}

// optiondlg.cpp:545
void FrChangeNickDlg::OnCheckoutInit(intptr_t param) {
	FrButton* pBtn = DYNAMIC_CAST(FrButton, param);
	if (!pBtn) {
		return;
	}

	CSharedDoc* pDoc = CSharedDoc::Instance();
	if (pDoc->m_myCash < 49 && !m_bHasTicket && !(pDoc->GetPangyaUserInfo().identity & 0x204)) {
		pBtn->Enable(false);
	}
}

// optiondlg.cpp:561
void FrChangeNickDlg::OnCheckoutBtnUp(void) {
	FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_okcancel", 0);
	// "Do you want to change your nickname?"
	pForm->SetMessage("\\c0xffff0000\\c\xB4\xEB\xC8\xAD\xB8\xED\xC0\xBB \xBA\xAF\xB0\xE6 \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);

	pForm->Open(static_cast<FRESH_RESULT_CALLBACK>(&FrChangeNickDlg::OnChangeNickConfirmDlgResult), 1);
}

// optiondlg.cpp:585
bool FrChangeNickDlg::OnInit(void) {
	char szText[1024] = "";

	// "- Before changing your nickname, check the nickname you want to change to."
	const char* szNotice = "\\c0xffff0000\\c-\xB4\xD0\xB3\xD7\xC0\xD3\xC0\xBB \xBA\xAF\xB0\xE6\xC7\xCF\xBD\xC3\xB1\xE2 \xC0\xFC\xBF\xA1 \xBA\xAF\xB0\xE6\xC7\xCF\xB0\xED\xC0\xDA\n  \xC7\xCF\xB4\xC2 \xB4\xD0\xB3\xD7\xC0\xD3\xC0\xBB \xB2\xC0 \xC8\xAE\xC0\xCE\xC7\xCF\xBF\xA9 \xC1\xD6\xBD\xCA\xBD\xC3\xBF\xC0.\n";
	strcpy(szText, szNotice);
	// "- This product cannot be withdrawn (purchase cancelled)."
	strcat(szText, "-\xC0\xCC \xBB\xF3\xC7\xB0\xC0\xBA \xC3\xBB\xBE\xE0\xC3\xB6\xC8\xB8(\xB1\xB8\xB8\xC5\xC3\xEB\xBC\xD2)\xB0\xA1 \xBA\xD2\xB0\xA1\xB4\xC9\xC7\xD1 \xBB\xF3\xC7\xB0\xC0\xD4\xB4\xCF\xB4\xD9.\\c0xff000000\\c\n");
	// "Changes the nickname from '%s' to '%s'."
	strcat(szText, MakeStr("\xB4\xEB\xC8\xAD\xB8\xED\xC0\xBB '%s'\xBF\xA1\xBC\xAD\n'%s'(\xC0\xB8)\xB7\xCE \xBA\xAF\xB0\xE6\xC7\xD5\xB4\xCF\xB4\xD9.", CSharedDoc::Instance()->GetPangyaUserInfo().nickname, m_szNick));

	if (CSharedDoc::Instance()->GetPangyaUserInfo().identity & 0x204) {
		// "This ID can change its nickname for free."
		strcat(szText, "\xC0\xCC \xBE\xC6\xC0\xCC\xB5\xF0\xB4\xC2 \xB9\xAB\xB7\xE1\xB7\xCE \xB4\xEB\xC8\xAD\xB8\xED \xBA\xAF\xB0\xE6\xC0\xCC \xB0\xA1\xB4\xC9\xC7\xD5\xB4\xCF\xB4\xD9.");
	}

	if (m_bHasTicket) {
		// "One nickname tag item is used up by the change."
		strcat(szText, "\n\n\xB4\xEB\xC8\xAD\xB8\xED\xC0\xBB \xBA\xAF\xB0\xE6\xC7\xD2 \xB6\xA7 \\c0xffff0000\\c\xB4\xEB\xC8\xAD\xB8\xED \xB8\xED\xC2\xFB \xBE\xC6\xC0\xCC\xC5\xDB\\c0xff000000\\c\xC0\xCC 1\xB0\xB3 \xBC\xD2\xB8\xF0\xB5\xCB\xB4\xCF\xB4\xD9.");
	} else {
		unsigned long billingType = CSharedDoc::Instance()->m_billingType;
		// "49 cookies are deducted by the change."
		strcat(szText, "\n\n\xB4\xEB\xC8\xAD\xB8\xED\xC0\xBB \xBA\xAF\xB0\xE6\xC7\xD2 \xB6\xA7 \\c0xffff0000\\c49\xC4\xED\xC5\xB0\\c0xff000000\\c\xB0\xA1 \xC2\xF7\xB0\xA8\xB5\xCB\xB4\xCF\xB4\xD9.");
		if (CSharedDoc::Instance()->m_myCash >= 49) {
			// "Final %s balance after the change: %I64d won" (tree cash / han coin).
			strcat(szText, MakeStr("\\c0xffff0000,t\\c\xBA\xAF\xB0\xE6 \xC8\xC4 \xC3\xD6\xC1\xBE %s \xC0\xDC\xBE\xD7:%I64d\xBF\xF8\\c0xff000000,n\\c", billingType == 2 ? "\xC6\xAE\xB8\xAE\xC4\xB3\xBD\xAC" : "\xC7\xD1\xC4\xDA\xC0\xCE", MyOutsideCash()));
		} else {
			__int64 cost = S5::ExchangeOwnCookieToCash(49, 100);
			__int64 remain = MyOutsideCash() - cost;
			if (remain >= 0) {
				strcat(szText, MakeStr("\\c0xffff0000,t\\c\xBA\xAF\xB0\xE6 \xC8\xC4 \xC3\xD6\xC1\xBE %s \xC0\xDC\xBE\xD7:%I64d\xBF\xF8\\c0xff000000,n\\c", billingType == 2 ? "\xC6\xAE\xB8\xAE\xC4\xB3\xBD\xAC" : "\xC7\xD1\xC4\xDA\xC0\xCE", remain));
			} else {
				// "Not enough %s balance."
				strcat(szText, MakeStr("\\c0xffff0000,t\\c%s \xC0\xDC\xBE\xD7\xC0\xCC \xBA\xCE\xC1\xB7\xC7\xD5\xB4\xCF\xB4\xD9.", billingType == 2 ? "\xC6\xAE\xB8\xAE\xC4\xB3\xBD\xAC" : "\xC7\xD1\xC4\xDA\xC0\xCE"));
			}
		}
	}

	SetMessage(szText, false);

	return true;
}

// optiondlg.cpp:690
bool FrChangeNickDlg::OnChangeNickConfirmDlgResult(int result, FrForm* pForm) {
	if (result == 1) {
		CSharedDoc* pDoc = CSharedDoc::Instance();
		if (pDoc->m_myCash >= 49 || m_bHasTicket || (pDoc->GetPangyaUserInfo().identity & 0x204)) {
			WSendPacket packet((enumClientPacket)0x38);   // change of nickname (no named id)

			packet.EncodeStr(std::string(m_szNick));
			packet.Send(SEND_TO_DEFAULT);

			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 1, 0, 0, 0, 0, 0);

			if (m_pOptionDlg) {
				m_pOptionDlg->EnableChangeNickControls(false);
			}

			Close(FrOK, true);
		} else {
			// "Not enough cookies. Please charge cookies at the shop."
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x23, (intptr_t)"\c0xffff0000\c\xC4\xED\xC5\xB0\c0xff000000\c\xB0\xA1 \xBA\xCE\xC1\xB7\xC7\xD5\xB4\xCF\xB4\xD9.\n\xBB\xF3\xC1\xA1\xBF\xA1\xBC\xAD \xC4\xED\xC5\xB0\xB8\xA6 \xC3\xE6\xC0\xFC\xC7\xD8 \xC1\xD6\xBC\xBC\xBF\xE4.", 0, 0, 0, 0);

			if (m_pOptionDlg) {
				m_pOptionDlg->EnableChangeNickControls(false);
			}
		}
	} else {
		if (m_pOptionDlg) {
			m_pOptionDlg->EnableChangeNickControls(true);
		}
	}

	m_pOptionDlg = 0;

	return true;
}

// optiondlg.cpp:745 (run-time type and factory registration of FrOptionDlg)
const WRTTI FrOptionDlg::m_RTTI("FrOptionDlg", &FrForm::m_RTTI);

FrOptionDlg::eTabType FrOptionDlg::ms_eCurTab;

IObject* FrOptionDlgMakeInstance(void) {
	return new FrOptionDlg;
}

struct __sFrOptionDlg {
	__sFrOptionDlg(void) {
		ObjectFactory().AddObjectFunctor(FrOptionDlgMakeInstance, "FrOptionDlg");
	}
};
static __sFrOptionDlg s_registerFrOptionDlg;

// optiondlg.cpp:747
const sFRESH_MSGMAP* FrOptionDlg::GetMessageMap(void) const {
	return &FrOptionDlg::_MsgMap;
}

sFRESH_MSGMAP FrOptionDlg::_MsgMap = { &FrForm::_MsgMap, &FrOptionDlg::_MsgEntries[0] };

// optiondlg.cpp:750
sFRESH_ENTRY FrOptionDlg::_MsgEntries[] = {
	// optiondlg.cpp:752
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::OnOkBtnInit },
	// optiondlg.cpp:753
	{ "game", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::OnGameTabInit },
	// optiondlg.cpp:754
	{ "game", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::OnGameTabUp },
	// optiondlg.cpp:755
	{ "video", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::OnVideoTabInit },
	// optiondlg.cpp:756
	{ "video", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::OnVideoTabUp },
	// optiondlg.cpp:757
	{ "audio", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::OnAudioTabInit },
	// optiondlg.cpp:758
	{ "audio", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::OnAudioTabUp },
	// optiondlg.cpp:759
	{ "macro", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::OnMacroTabInit },
	// optiondlg.cpp:760
	{ "macro", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::OnMacroTabUp },
	// optiondlg.cpp:763
	{ "g_ssensi", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnMsSensiStaticInit },
	// optiondlg.cpp:764
	{ "g_sensi", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnMsSensiGaugeInit },
	// optiondlg.cpp:765
	{ "g_swhisper", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnWhisperStaticInit },
	// optiondlg.cpp:766
	{ "g_whisper", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnWhisperBtnInit },
	// optiondlg.cpp:767
	{ "g_whisper", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnWhisperBtnUp },
	// optiondlg.cpp:768
	{ "g_sinvite", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnInvitationStaticInit },
	// optiondlg.cpp:769
	{ "g_invite", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnInvitationBtnInit },
	// optiondlg.cpp:770
	{ "g_invite", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnInvitationBtnUp },
	// optiondlg.cpp:771
	{ "g_sconfirm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnFriendConfirmStaticInit },
	// optiondlg.cpp:772
	{ "g_confirm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnFriendConfirmBtnInit },
	// optiondlg.cpp:773
	{ "g_confirm", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnFriendConfirmBtnUp },
	// optiondlg.cpp:774
	{ "g_schatwin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnTransChatWinStaticInit },
	// optiondlg.cpp:775
	{ "g_chatwin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnTransChatWinBtnInit },
	// optiondlg.cpp:776
	{ "g_chatwin", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnTransChatWinBtnUp },
	// optiondlg.cpp:777
	{ "g_suieffect", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnUiEffectStaticInit },
	// optiondlg.cpp:778
	{ "g_uieffect", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnUiEffectBtnInit },
	// optiondlg.cpp:779
	{ "g_uieffect", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnUiEffectBtnUp },
	// optiondlg.cpp:786
	{ "g_snick", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnNickStaticInit },
	// optiondlg.cpp:787
	{ "g_nick", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnNickEditInit },
	// optiondlg.cpp:782
	{ "g_nick", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrOptionDlg::gOnNickEditEnterKey },
	// optiondlg.cpp:783
	{ "g_changenick", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnChangeNickBtnInit },
	// optiondlg.cpp:784
	{ "g_changenick", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnChangeNickBtnUp },
	// optiondlg.cpp:785
	{ "g_sschool", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnSchoolStaticInit },
	// optiondlg.cpp:786
	{ "g_school", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnSchoolEditInit },
	// optiondlg.cpp:787
	{ "g_school", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrOptionDlg::gOnSchoolEditEnterKey },
	// optiondlg.cpp:788
	{ "g_changeschool", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnChangeSchoolBtnInit },
	// optiondlg.cpp:789
	{ "g_changeschool", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnChangeSchoolBtnUp },
	// optiondlg.cpp:790
	{ "g_spowergauge", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnPowerGaugeStaticInit },
	// optiondlg.cpp:791
	{ "g_powergauge", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnPowerGaugeBtnInit },
	// optiondlg.cpp:792
	{ "g_powergauge", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnPowerGaugeBtnUp },
	// optiondlg.cpp:793
	{ "g_shidegui", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnHideGUIStaticInit },
	// optiondlg.cpp:794
	{ "g_hidegui", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnHideGUIBtnInit },
	// optiondlg.cpp:795
	{ "g_hidegui", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnHideGUIBtnUp },
	// optiondlg.cpp:796
	{ "g_shidepi", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnHidePIStaticInit },
	// optiondlg.cpp:797
	{ "g_hidepi", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnHidePIBtnInit },
	// optiondlg.cpp:798
	{ "g_hidepi", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnHidePIBtnUp },
	// optiondlg.cpp:799
	{ "g_stooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnTerrainTooltipStaticInit },
	// optiondlg.cpp:800
	{ "g_btntooltip", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnTerrainBtnInit },
	// optiondlg.cpp:801
	{ "g_btntooltip", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnTerrainBtnUp },
	// optiondlg.cpp:802
	{ "g_cutin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnCutinDisplayStaticInit },
	// optiondlg.cpp:803
	{ "g_btncutin", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::gOnCutinDisplayBtnInit },
	// optiondlg.cpp:804
	{ "g_btncutin", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::gOnCutinDisplayBtnUp },
	// optiondlg.cpp:808
	{ "v_sfullscr", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnFullScreenStaticInit },
	// optiondlg.cpp:809
	{ "v_fullscr", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnFullScreenBtnInit },
	// optiondlg.cpp:810
	{ "v_fullscr", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::vOnFullScreenBtnUp },
	// optiondlg.cpp:812
	{ "v_sfillscr", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnFillScreenStaticInit },
	// optiondlg.cpp:813
	{ "v_fillscr", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnFillScreenBtnInit },
	// optiondlg.cpp:814
	{ "v_fillscr", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::vOnFillScreenBtnUp },
	// optiondlg.cpp:816
	{ "v_swidemode", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnWideModeStaticInit },
	// optiondlg.cpp:817
	{ "v_widemode", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnWideModeBtnInit },
	// optiondlg.cpp:818
	{ "v_widemode", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::vOnWideModeBtnUp },
	// optiondlg.cpp:819
	{ "v_soverall", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnOverallStaticInit },
	// optiondlg.cpp:820
	{ "v_overall", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnOverallCmbInit },
	// optiondlg.cpp:821
	{ "v_sres", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnScrResStaticInit },
	// optiondlg.cpp:822
	{ "v_res", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnScrResCmbInit },
	// optiondlg.cpp:826
	{ "v_stnl", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnTnlStaticInit },
	// optiondlg.cpp:827
	{ "v_tnl", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnTnlCmbInit },
	// optiondlg.cpp:829
	{ "v_stex", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnTexResStaticInit },
	// optiondlg.cpp:830
	{ "v_tex", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnTexResCmbInit },
	// optiondlg.cpp:831
	{ "v_sshadow", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnShadowStaticInit },
	// optiondlg.cpp:832
	{ "v_shadow", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnShadowBtnInit },
	// optiondlg.cpp:833
	{ "v_shadow", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::vOnShadowBtnUp },
	// optiondlg.cpp:834
	{ "v_ssdwmap", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnShadowmapStaticInit },
	// optiondlg.cpp:835
	{ "v_sdwmap", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnShadowmapBtnInit },
	// optiondlg.cpp:836
	{ "v_sdwmap", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::vOnShadowmapBtnUp },
	// optiondlg.cpp:837
	{ "v_srefl", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnReflectionStaticInit },
	// optiondlg.cpp:838
	{ "v_refl", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnReflectionBtnInit },
	// optiondlg.cpp:839
	{ "v_refl", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::vOnReflectionBtnUp },
	// optiondlg.cpp:843
	{ "v_slod", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnLodStaticInit },
	// optiondlg.cpp:844
	{ "v_lod", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnLodBtnInit },
	// optiondlg.cpp:845
	{ "v_lod", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::vOnLodBtnUp },
	// optiondlg.cpp:847
	{ "v_snpc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnNpcStaticInit },
	// optiondlg.cpp:848
	{ "v_npc", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnNpcBtnInit },
	// optiondlg.cpp:849
	{ "v_npc", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::vOnNpcBtnUp },
	// optiondlg.cpp:850
	{ "v_sparticle", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnParticleStaticInit },
	// optiondlg.cpp:851
	{ "v_particle", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::vOnParticleCmbInit },
	// optiondlg.cpp:854
	{ "a_senable", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::aOnEnableStaticInit },
	// optiondlg.cpp:855
	{ "a_enable", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::aOnEnableBtnInit },
	// optiondlg.cpp:856
	{ "a_enable", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::aOnEnableBtnUp },
	// optiondlg.cpp:857
	{ "a_soverall", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::aOnOverallStaticInit },
	// optiondlg.cpp:858
	{ "a_overall", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::aOnOverallCmbInit },
	// optiondlg.cpp:859
	{ "a_ssfx", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::aOnSfxStaticInit },
	// optiondlg.cpp:860
	{ "a_sfx", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::aOnSfxGaugeInit },
	// optiondlg.cpp:861
	{ "a_sbgm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::aOnBgmStaticInit },
	// optiondlg.cpp:862
	{ "a_bgm", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::aOnBgmGaugeInit },
	// optiondlg.cpp:863
	{ "a_sspeaker", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::aOnSpeakerStaticInit },
	// optiondlg.cpp:864
	{ "a_speaker", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::aOnSpeakerCmbInit },
	// optiondlg.cpp:868
	{ "m_smacro1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacroStatic1Init },
	// optiondlg.cpp:869
	{ "m_macro1", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacro1Init },
	// optiondlg.cpp:870
	{ "m_macro1", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::mOnMacroEnterKeyLBtnDown },
	// optiondlg.cpp:871
	{ "m_macro1", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrOptionDlg::mOnMacroEnterKey },
	// optiondlg.cpp:872
	{ "m_smacro2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacroStatic2Init },
	// optiondlg.cpp:873
	{ "m_macro2", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacro2Init },
	// optiondlg.cpp:874
	{ "m_macro2", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::mOnMacroEnterKeyLBtnDown },
	// optiondlg.cpp:875
	{ "m_macro2", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrOptionDlg::mOnMacroEnterKey },
	// optiondlg.cpp:876
	{ "m_smacro3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacroStatic3Init },
	// optiondlg.cpp:877
	{ "m_macro3", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacro3Init },
	// optiondlg.cpp:878
	{ "m_macro3", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::mOnMacroEnterKeyLBtnDown },
	// optiondlg.cpp:879
	{ "m_macro3", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrOptionDlg::mOnMacroEnterKey },
	// optiondlg.cpp:880
	{ "m_smacro4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacroStatic4Init },
	// optiondlg.cpp:881
	{ "m_macro4", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacro4Init },
	// optiondlg.cpp:882
	{ "m_macro4", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::mOnMacroEnterKeyLBtnDown },
	// optiondlg.cpp:883
	{ "m_macro4", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrOptionDlg::mOnMacroEnterKey },
	// optiondlg.cpp:884
	{ "m_smacro5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacroStatic5Init },
	// optiondlg.cpp:885
	{ "m_macro5", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacro5Init },
	// optiondlg.cpp:886
	{ "m_macro5", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::mOnMacroEnterKeyLBtnDown },
	// optiondlg.cpp:887
	{ "m_macro5", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrOptionDlg::mOnMacroEnterKey },
	// optiondlg.cpp:888
	{ "m_smacro6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacroStatic6Init },
	// optiondlg.cpp:889
	{ "m_macro6", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacro6Init },
	// optiondlg.cpp:890
	{ "m_macro6", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::mOnMacroEnterKeyLBtnDown },
	// optiondlg.cpp:891
	{ "m_macro6", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrOptionDlg::mOnMacroEnterKey },
	// optiondlg.cpp:892
	{ "m_smacro7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacroStatic7Init },
	// optiondlg.cpp:893
	{ "m_macro7", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacro7Init },
	// optiondlg.cpp:894
	{ "m_macro7", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::mOnMacroEnterKeyLBtnDown },
	// optiondlg.cpp:895
	{ "m_macro7", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrOptionDlg::mOnMacroEnterKey },
	// optiondlg.cpp:896
	{ "m_smacro8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacroStatic8Init },
	// optiondlg.cpp:897
	{ "m_macro8", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::mOnMacro8Init },
	// optiondlg.cpp:906
	{ "m_macro8", FRCMD_LBUTTONDOWN, FrFuncVV, (FRESH_PMSG)&FrOptionDlg::mOnMacroEnterKeyLBtnDown },
	// optiondlg.cpp:905
	{ "m_macro8", FRCMD_ENTERKEY, FrFuncBI, (FRESH_PMSG)(FRESH_PMSGBI)&FrOptionDlg::mOnMacroEnterKey },
	// optiondlg.cpp:905
	{ "t_hint", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::tOnHintInit },
	// optiondlg.cpp:906
	{ "", FRCMD_HOVERON, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::OnHoverOn },
	// optiondlg.cpp:907
	{ "", FRCMD_HOVEROFF, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrOptionDlg::OnHoverOff },

	// optiondlg.cpp:909
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// optiondlg.cpp:917
FrOptionDlg::FrOptionDlg(void) : m_bApplyScrSize(false) {
	m_option = *COption::Instance()->GetOption();
	m_bChanged1 = false;
	m_bChanged2 = false;
	m_pOkBtn = 0;
	m_pTabs[TAB_GAME] = &m_gameTab;
	m_pTabs[TAB_VIDEO] = &m_videoTab;
	m_pTabs[TAB_AUDIO] = &m_audioTab;
	m_pTabs[TAB_MACRO] = &m_macroTab;

	m_pChangeNickDlg = 0;
	m_pSchoolDlg = 0;

	m_pHintEdit = 0;
	m_hoverCount = 0;

	g_pFresh->GetManager()->SetExclusiveKey(true);
}

// optiondlg.cpp:938
FrOptionDlg::~FrOptionDlg(void) {
	if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI) : false) {
		if (!CChatMsg::Instance()->IsOpened()) {
			g_pFresh->GetManager()->SetExclusiveKey(false);
		}
	}
}

// optiondlg.cpp:949
void FrOptionDlg::SetApplyScrSize(bool bApply) {
	m_bApplyScrSize = bApply;
}

// optiondlg.cpp:954
void FrOptionDlg::OpenTab(eTabType type) {
	if (type < TAB_GAME || type >= 4) {
		return;
	}

	SetTab(type);
}

// optiondlg.cpp:962
void FrOptionDlg::OnOkBtnInit(intptr_t param) {
	m_pOkBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:967
void FrOptionDlg::OnGameTabInit(int param) {
	m_pTabs[TAB_GAME]->m_pTabBtn = DYNAMIC_CAST(FrButton, param);
	m_pTabs[TAB_GAME]->Init();
}

// optiondlg.cpp:973
void FrOptionDlg::OnGameTabUp(void) {
	SetTab(TAB_GAME);
}

// optiondlg.cpp:978
void FrOptionDlg::OnVideoTabInit(int param) {
	m_pTabs[TAB_VIDEO]->m_pTabBtn = DYNAMIC_CAST(FrButton, param);
	m_pTabs[TAB_VIDEO]->Init();
}

// optiondlg.cpp:984
void FrOptionDlg::OnVideoTabUp(void) {
	SetTab(TAB_VIDEO);
}

// optiondlg.cpp:989
void FrOptionDlg::OnAudioTabInit(int param) {
	m_pTabs[TAB_AUDIO]->m_pTabBtn = DYNAMIC_CAST(FrButton, param);
	m_pTabs[TAB_AUDIO]->Init();
}

// optiondlg.cpp:995
void FrOptionDlg::OnAudioTabUp(void) {
	SetTab(TAB_AUDIO);
}

// optiondlg.cpp:1000
void FrOptionDlg::OnMacroTabInit(int param) {
	m_pTabs[TAB_MACRO]->m_pTabBtn = DYNAMIC_CAST(FrButton, param);
	m_pTabs[TAB_MACRO]->Init();
}

// optiondlg.cpp:1006
void FrOptionDlg::OnMacroTabUp(void) {
	SetTab(TAB_MACRO);
}

// optiondlg.cpp:1011
void FrOptionDlg::gOnMsSensiStaticInit(int param) {
	m_gameTab.m_pStatics[0] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1016
void FrOptionDlg::gOnMsSensiGaugeInit(int param) {
	m_gameTab.m_pMsSensiGauge = DYNAMIC_CAST(FrGaugeBarEx, param);
}

// optiondlg.cpp:1021
void FrOptionDlg::gOnWhisperStaticInit(int param) {
	m_gameTab.m_pStatics[1] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1026
void FrOptionDlg::gOnWhisperBtnInit(intptr_t param) {
	m_gameTab.m_pWhisperBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1031
void FrOptionDlg::gOnWhisperBtnUp(void) {
	m_option.m_whisperAllowed = m_gameTab.m_pWhisperBtn->GetStatus() == FrButton::PRESSED;
}

// optiondlg.cpp:1036
void FrOptionDlg::gOnInvitationStaticInit(int param) {
	m_gameTab.m_pStatics[2] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1041
void FrOptionDlg::gOnInvitationBtnInit(int param) {
	m_gameTab.m_pInvitationBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1046
void FrOptionDlg::gOnInvitationBtnUp(void) {
	m_option.m_invitationAllowed = m_gameTab.m_pInvitationBtn->GetStatus() == FrButton::PRESSED;
}

// optiondlg.cpp:1051
void FrOptionDlg::gOnFriendConfirmStaticInit(int param) {
	m_gameTab.m_pStatics[3] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1056
void FrOptionDlg::gOnFriendConfirmBtnInit(int param) {
	m_gameTab.m_pFriendConfirmBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1061
void FrOptionDlg::gOnFriendConfirmBtnUp(void) {
	m_option.m_friendConfirmNeeded = m_gameTab.m_pFriendConfirmBtn->GetStatus() == FrButton::PRESSED;
}

// optiondlg.cpp:1066
void FrOptionDlg::gOnTransChatWinStaticInit(int param) {
	m_gameTab.m_pStatics[4] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1071
void FrOptionDlg::gOnTransChatWinBtnInit(int param) {
	m_gameTab.m_pTransChatWinBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1076
void FrOptionDlg::gOnTransChatWinBtnUp(void) {
	m_option.m_chatWinTransparent = m_gameTab.m_pTransChatWinBtn->GetStatus() == FrButton::PRESSED;
}

// optiondlg.cpp:1081
void FrOptionDlg::gOnUiEffectStaticInit(int param) {
	m_gameTab.m_pStatics[5] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1086
void FrOptionDlg::gOnTerrainTooltipStaticInit(int param) {
	m_gameTab.m_pStatics[11] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1091
void FrOptionDlg::gOnUiEffectBtnInit(int param) {
	m_gameTab.m_pUiEffectBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1096
void FrOptionDlg::gOnUiEffectBtnUp(void) {
	m_option.m_uiEffect = m_gameTab.m_pUiEffectBtn->GetStatus() == FrButton::PRESSED;
}

// optiondlg.cpp:1101
void FrOptionDlg::gOnNickStaticInit(intptr_t param) {
	m_gameTab.m_pStatics[6] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1106
void FrOptionDlg::gOnNickEditInit(int param) {
	m_gameTab.m_pNickEdit = DYNAMIC_CAST(FrEdit, param);
	m_gameTab.m_pNickEdit->HidePrivacy(true);

	if (!m_gameTab.m_bEnableChangeNick) {
		m_gameTab.m_pNickEdit->SetReadOnly(true);
		m_gameTab.m_pNickEdit->SetFontColor(0xff888888);
	}
}

// optiondlg.cpp:1119
void FrOptionDlg::gOnChangeNickBtnInit(int param) {
	m_gameTab.m_pChangeNickBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1124
void FrOptionDlg::gOnChangeNickBtnUp(void) {
	bool bCanChange = false;
	IActor* pGateway = AfxGetTask()->GetActor(s_gatewayActorName);
	pGateway << MsgObject(0, 0x26D, 0x10, (intptr_t)&bCanChange, 0, 0, 0);

	if (!bCanChange) {
		pGateway << MsgObject(0, 0x26D, 0x11, 0, 0, 0, 0);
		return;
	}

	if (m_gameTab.m_bEnableChangeNick && m_gameTab.m_pNickEdit && !m_pChangeNickDlg) {
		if (m_bChanged1) {
			m_bChanged2 = false;
		}

		char szNick[0x16];
		strcpy(szNick, m_gameTab.m_pNickEdit->GetLine(1, false));
		const char* szReason = CSharedDoc::Instance()->m_chatManager.FilteringNick(szNick);

		if (szReason) {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			if (pForm) {
				pForm->SetMessage(szReason, false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
			}

			m_bChanged1 = false;
			return;
		}

		const char* szLine = m_gameTab.m_pNickEdit->GetLine(1, false);
		cTokenV token;
		token.Init(szLine, strlen(szLine));

		int numQuotes = token.GetTokenNum(0, " '", 2) - 1;

		if (numQuotes > 0) {
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			if (pForm) {
				// "['] cannot be used in a nickname."
				pForm->SetMessage("[']\xB4\xC2 \xB4\xEB\xC8\xAD\xB8\xED\xBF\xA1 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", false);
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
			}
		} else if (strcmpi(CSharedDoc::Instance()->GetPangyaUserInfo().nickname, szLine) != 0) {
			m_pChangeNickDlg = CreateForm<FrChangeNickDlg>(g_pFresh->GetManager(), this, "changenick", 0);

			if (m_pChangeNickDlg) {
				m_pChangeNickDlg->SetOptionDlg(this); m_pChangeNickDlg->SetNick(m_gameTab.m_pNickEdit->GetLine(1, false));
				m_pChangeNickDlg->SetCloseResult(FrNONE);
				m_pChangeNickDlg->Open(static_cast<FRESH_RESULT_CALLBACK>(&FrOptionDlg::OnChangeNickConfirmDlgResult), 0);
			}

			EnableChangeNickControls(false);
		} else {
			if (m_bChanged1) {
				CloseDlg();
			} else {
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				if (pForm) {
					// "Please enter a new nickname."
					pForm->SetMessage("\xBB\xF5\xB7\xCE\xBF\xEE \xB4\xEB\xC8\xAD\xB8\xED\xC0\xBB \xC0\xD4\xB7\xC2\xC7\xD8 \xC1\xD6\xBC\xBC\xBF\xE4", false);
					pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
				}
			}
		}
	}
}

// optiondlg.cpp:1237
bool FrOptionDlg::gOnNickEditEnterKey(int key) {
	return false;
}

// optiondlg.cpp:1242
void FrOptionDlg::gOnSchoolStaticInit(int param) {
	m_gameTab.m_pStatics[7] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1247
void FrOptionDlg::gOnSchoolEditInit(int param) {
	m_gameTab.m_pSchoolEdit = DYNAMIC_CAST(FrEdit, param);
	m_gameTab.m_pSchoolEdit->SetReadOnly(true);
}

// optiondlg.cpp:1253
void FrOptionDlg::gOnChangeSchoolBtnInit(int param) {
	m_gameTab.m_pChangeSchoolBtn = DYNAMIC_CAST(FrButton, param);

	// The school can only be changed from the lobby, online.
	if (((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false)
		&& WSingleton<WNetworkSystem>::Instance()->IsConnected((WNetworkSystem::eNetUnit)0)) {
		m_gameTab.m_pChangeSchoolBtn->Enable(true);
	} else {
		m_gameTab.m_pChangeSchoolBtn->Enable(false);
	}
}

// optiondlg.cpp:1267
void FrOptionDlg::gOnChangeSchoolBtnUp(void) {
	if (m_gameTab.m_pSchoolEdit && !m_pSchoolDlg) {
		m_pSchoolDlg = CreateForm<FrSchoolDlg>(g_pFresh->GetManager(), this, "search_school", 0);
		m_pSchoolDlg->Open(static_cast<FRESH_RESULT_CALLBACK>(&FrOptionDlg::OnChangeSchoolDlgResult), 1);
	}
}

// optiondlg.cpp:1276
bool FrOptionDlg::gOnSchoolEditEnterKey(int key) {
	return false;
}

// optiondlg.cpp:1282
void FrOptionDlg::gOnPowerGaugeStaticInit(int param) {
	m_gameTab.m_pStatics[8] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1288
void FrOptionDlg::gOnPowerGaugeBtnInit(int param) {
	m_gameTab.m_pPowerGaugeBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1294
void FrOptionDlg::gOnPowerGaugeBtnUp(void) {
	m_option.m_powerGauge = m_gameTab.m_pPowerGaugeBtn->GetStatus() == FrButton::PRESSED;
}

// optiondlg.cpp:1299
void FrOptionDlg::gOnHideGUIStaticInit(int param) {
	m_gameTab.m_pStatics[9] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1304
void FrOptionDlg::gOnHideGUIBtnInit(int param) {
	m_gameTab.m_pHideGUIBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1309
void FrOptionDlg::gOnHideGUIBtnUp(void) {
	m_option.m_captureHideGUI = m_gameTab.m_pHideGUIBtn->GetStatus() == FrButton::PRESSED;
	CProjectG::Instance()->ResetCaptuerNum();
}

// optiondlg.cpp:1315
void FrOptionDlg::gOnHidePIStaticInit(int param) {
	m_gameTab.m_pStatics[10] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1320
void FrOptionDlg::gOnHidePIBtnInit(int param) {
	m_gameTab.m_pHidePIBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1325
void FrOptionDlg::gOnTerrainBtnInit(int param) {
	m_gameTab.m_pTerrainBtn = DYNAMIC_CAST(FrButton, param);

	if (COption::Instance()->gGetTerrainTooltip()) {
		m_gameTab.m_pTerrainBtn->SetStatus(FrButton::PRESSED);
	}
}

// optiondlg.cpp:1335
void FrOptionDlg::gOnTerrainBtnUp(void) {
}

// optiondlg.cpp:1341
void FrOptionDlg::gOnCutinDisplayStaticInit(int param) {
	m_gameTab.m_pStatics[12] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1347
void FrOptionDlg::gOnCutinDisplayBtnInit(int param) {
	m_gameTab.m_pCutinDisplayBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1353
void FrOptionDlg::gOnCutinDisplayBtnUp(void) {
	m_option.m_cutinFlag = m_gameTab.m_pCutinDisplayBtn->GetStatus() == FrButton::PRESSED;
}

// optiondlg.cpp:1359
void FrOptionDlg::gOnHidePIBtnUp(void) {
	m_option.m_captureHidePI = m_gameTab.m_pHidePIBtn->GetStatus() == FrButton::PRESSED;
	CProjectG::Instance()->ResetCaptuerNum();
}

// optiondlg.cpp:1365
void FrOptionDlg::vOnFullScreenStaticInit(int param) {
	m_videoTab.m_pStatics[0] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1370
void FrOptionDlg::vOnFullScreenBtnInit(int param) {
	m_videoTab.m_pFullScreenBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1375
void FrOptionDlg::vOnFullScreenBtnUp(void) {
	m_option.m_windowed = m_videoTab.m_pFullScreenBtn->GetStatus() != FrButton::PRESSED;

	char szCurRes[16];
	strcpy(szCurRes, COption::Instance()->vGetScrResText(m_option.m_screenWidth, m_option.m_screenHeight, m_option.m_screenBpp, m_option.m_windowed != 0));

	COption::Instance()->CheckDisplayModeList(g_resrcmng->VideoReference(), m_option.m_windowed != 0, m_option.m_wideMode != 0);
	m_videoTab.m_pScrResCmb->ClearList();

	bool bFirst = true;
	for (int i = 0; i < COption::Instance()->vGetScrResNum(); i++) {
		const char* text = COption::Instance()->vGetScrResText(i);

		int len = strlen(text);
		if (len == 0)
			continue;

		m_videoTab.m_pScrResCmb->AddString(text);

		if (bFirst) {
			bFirst = false;
			m_videoTab.m_pScrResCmb->SetLine(1, text, 0, false, 0);
		}

		if (strcmp(text, szCurRes) == 0)
			m_videoTab.m_pScrResCmb->SetLine(1, text, 0, false, 0);
	}
}

// optiondlg.cpp:1409
void FrOptionDlg::vOnFillScreenStaticInit(int param) {
	m_videoTab.m_pStatics[1] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1414
void FrOptionDlg::vOnFillScreenBtnInit(int param) {
	m_videoTab.m_pFillScreenBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1419
void FrOptionDlg::vOnFillScreenBtnUp(void) {
	if (m_videoTab.m_pFillScreenBtn && !m_videoTab.m_pFillScreenBtn->IsEnabled())
		return;

	m_option.m_fillScreen = m_videoTab.m_pFillScreenBtn->GetStatus() == FrButton::PRESSED;
}

// optiondlg.cpp:1428
void FrOptionDlg::vOnWideModeStaticInit(int param) {
	m_videoTab.m_pStatics[2] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1433
void FrOptionDlg::vOnWideModeBtnInit(int param) {
	m_videoTab.m_pWideModeBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1438
void FrOptionDlg::vOnWideModeBtnUp(void) {
	m_option.m_wideMode = m_videoTab.m_pWideModeBtn->GetStatus() == FrButton::PRESSED;

	char szCurRes[16];
	strcpy(szCurRes, COption::Instance()->vGetScrResText(m_option.m_screenWidth, m_option.m_screenHeight, m_option.m_screenBpp, m_option.m_windowed != 0));

	COption::Instance()->CheckDisplayModeList(g_resrcmng->VideoReference(), m_option.m_windowed != 0, m_option.m_wideMode != 0);
	m_videoTab.m_pScrResCmb->ClearList();

	bool bFirst = true;
	for (int i = 0; i < COption::Instance()->vGetScrResNum(); i++) {
		const char* text = COption::Instance()->vGetScrResText(i);

		int len = strlen(text);
		if (len == 0)
			continue;

		m_videoTab.m_pScrResCmb->AddString(text);

		if (bFirst) {
			bFirst = false;
			m_videoTab.m_pScrResCmb->SetLine(1, text, 0, false, 0);
		}

		if (strcmp(text, szCurRes) == 0)
			m_videoTab.m_pScrResCmb->SetLine(1, text, 0, false, 0);
	}
}

// optiondlg.cpp:1472
void FrOptionDlg::vOnOverallStaticInit(int param) {
	m_videoTab.m_pStatics[3] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1477
void FrOptionDlg::vOnOverallCmbInit(int param) {
	m_videoTab.m_pOverallCmb = DYNAMIC_CAST(FrComboBox, param);
}

// optiondlg.cpp:1482
void FrOptionDlg::vOnScrResStaticInit(int param) {
	m_videoTab.m_pStatics[4] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1487
void FrOptionDlg::vOnScrResCmbInit(int param) {
	m_videoTab.m_pScrResCmb = DYNAMIC_CAST(FrComboBox, param);
}

// optiondlg.cpp:1498
void FrOptionDlg::vOnTnlStaticInit(int param) {
	m_videoTab.m_pStatics[5] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1503
void FrOptionDlg::vOnTnlCmbInit(int param) {
	m_videoTab.m_pTnlCmb = DYNAMIC_CAST(FrComboBox, param);
}

// optiondlg.cpp:1509
void FrOptionDlg::vOnTexResStaticInit(int param) {
	m_videoTab.m_pStatics[6] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1514
void FrOptionDlg::vOnTexResCmbInit(int param) {
	m_videoTab.m_pTexResCmb = DYNAMIC_CAST(FrComboBox, param);
}

// optiondlg.cpp:1519
void FrOptionDlg::vOnShadowStaticInit(int param) {
	m_videoTab.m_pStatics[7] = DYNAMIC_CAST(FrStatic, param);

	if (!sOption::IsSupportShadow()) {
		m_videoTab.m_pStatics[7]->SetTextColor(0xff808080, 0xffffffff);
	}
}

// optiondlg.cpp:1527
void FrOptionDlg::vOnShadowBtnInit(int param) {
	m_videoTab.m_pShadowBtn = DYNAMIC_CAST(FrButton, param);

	if (!sOption::IsSupportShadow()) {
		m_videoTab.m_pShadowBtn->Enable(false);
	}
}

// optiondlg.cpp:1535
void FrOptionDlg::vOnShadowBtnUp(void) {
	m_option.m_shadowEnabled = m_videoTab.m_pShadowBtn->GetStatus() == FrButton::PRESSED;
	m_option.m_overallLevel = -1;
	m_videoTab.m_pOverallCmb->SetLine(1, m_option.vGetOverallText(m_option.m_overallLevel), 0, false, 0);
}

// optiondlg.cpp:1542
void FrOptionDlg::vOnShadowmapStaticInit(int param) {
	m_videoTab.m_pStatics[8] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1547
void FrOptionDlg::vOnShadowmapBtnInit(int param) {
	m_videoTab.m_pShadowmapBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1552
void FrOptionDlg::vOnShadowmapBtnUp(void) {
	m_option.m_shadowmapEnabled = m_videoTab.m_pShadowmapBtn->GetStatus() == FrButton::PRESSED;
	m_option.m_overallLevel = -1;
	m_videoTab.m_pOverallCmb->SetLine(1, m_option.vGetOverallText(m_option.m_overallLevel), 0, false, 0);
}

// optiondlg.cpp:1559
void FrOptionDlg::vOnReflectionStaticInit(int param) {
	m_videoTab.m_pStatics[9] = DYNAMIC_CAST(FrStatic, param);

	if (!sOption::IsSupportReflection()) {
		m_videoTab.m_pStatics[9]->SetTextColor(0xff808080, 0xffffffff);
	}
}

// optiondlg.cpp:1567
void FrOptionDlg::vOnReflectionBtnInit(int param) {
	m_videoTab.m_pReflectionBtn = DYNAMIC_CAST(FrButton, param);

	if (!sOption::IsSupportReflection()) {
		m_videoTab.m_pReflectionBtn->Enable(false);
	}
}

// optiondlg.cpp:1575
void FrOptionDlg::vOnReflectionBtnUp(void) {
	m_option.m_reflectionEnabled = m_videoTab.m_pReflectionBtn->GetStatus() == FrButton::PRESSED;
	m_option.m_overallLevel = -1;
	m_videoTab.m_pOverallCmb->SetLine(1, m_option.vGetOverallText(m_option.m_overallLevel), 0, false, 0);
}

// optiondlg.cpp:1588
void FrOptionDlg::vOnLodStaticInit(int param) {
	m_videoTab.m_pStatics[10] = DYNAMIC_CAST(FrStatic, param);

	if (!sOption::IsSupportLod()) {
		m_videoTab.m_pStatics[10]->SetTextColor(0xff808080, 0xffffffff);
	}
}

// optiondlg.cpp:1596
void FrOptionDlg::vOnLodBtnInit(int param) {
	m_videoTab.m_pLodBtn = DYNAMIC_CAST(FrButton, param);

	if (!sOption::IsSupportLod()) {
		m_videoTab.m_pLodBtn->Enable(false);
	}
}

// optiondlg.cpp:1604
void FrOptionDlg::vOnLodBtnUp(void) {
	m_option.m_lodEnabled = m_videoTab.m_pLodBtn->GetStatus() == FrButton::PRESSED;
	m_option.m_overallLevel = -1;
	m_videoTab.m_pOverallCmb->SetLine(1, m_option.vGetOverallText(m_option.m_overallLevel), 0, false, 0);
}

// optiondlg.cpp:1612
void FrOptionDlg::vOnNpcStaticInit(int param) {
	m_videoTab.m_pStatics[11] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1617
void FrOptionDlg::vOnNpcBtnInit(int param) {
	m_videoTab.m_pNpcBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1622
void FrOptionDlg::vOnNpcBtnUp(void) {
	m_option.m_npcEnabled = m_videoTab.m_pNpcBtn->GetStatus() == FrButton::PRESSED;
	m_option.m_overallLevel = -1;
	m_videoTab.m_pOverallCmb->SetLine(1, m_option.vGetOverallText(m_option.m_overallLevel), 0, false, 0);
}

// optiondlg.cpp:1629
void FrOptionDlg::vOnParticleStaticInit(int param) {
	m_videoTab.m_pStatics[12] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1634
void FrOptionDlg::vOnParticleCmbInit(int param) {
	m_videoTab.m_pParticleCmb = DYNAMIC_CAST(FrComboBox, param);
}

// optiondlg.cpp:1639
void FrOptionDlg::aOnEnableStaticInit(int param) {
	m_audioTab.m_pStatics[0] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1644
void FrOptionDlg::aOnEnableBtnInit(int param) {
	m_audioTab.m_pEnableBtn = DYNAMIC_CAST(FrButton, param);
}

// optiondlg.cpp:1649
void FrOptionDlg::aOnEnableBtnUp(void) {
	if (m_audioTab.m_pEnableBtn->GetStatus() == FrButton::PRESSED) {
		m_option.m_mssEnabled = 1;
		g_audio->SetVolume(m_option.m_sfxVolume);
		g_audio->SetBGMVolume(m_option.m_bgmVolume);
	} else {
		m_option.m_mssEnabled = 0;
		g_audio->SetVolume(0);
		g_audio->SetBGMVolume(0);
	}
}

// optiondlg.cpp:1665
void FrOptionDlg::aOnOverallStaticInit(int param) {
	m_audioTab.m_pStatics[1] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1670
void FrOptionDlg::aOnOverallCmbInit(int param) {
	m_audioTab.m_pOverallCmb = DYNAMIC_CAST(FrComboBox, param);
}

// optiondlg.cpp:1675
void FrOptionDlg::aOnSfxStaticInit(int param) {
	m_audioTab.m_pStatics[2] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1680
void FrOptionDlg::aOnSfxGaugeInit(int param) {
	m_audioTab.m_pSfxGauge = DYNAMIC_CAST(FrGaugeBarEx, param);
}

// optiondlg.cpp:1685
void FrOptionDlg::aOnBgmStaticInit(int param) {
	m_audioTab.m_pStatics[3] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1690
void FrOptionDlg::aOnBgmGaugeInit(int param) {
	m_audioTab.m_pBgmGauge = DYNAMIC_CAST(FrGaugeBarEx, param);
}

// optiondlg.cpp:1695
void FrOptionDlg::aOnSpeakerStaticInit(int param) {
	m_audioTab.m_pStatics[4] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1700
void FrOptionDlg::aOnSpeakerCmbInit(int param) {
	m_audioTab.m_pSpeakerCmb = DYNAMIC_CAST(FrComboBox, param);
}

// optiondlg.cpp:1705
void FrOptionDlg::mOnMacroStatic1Init(int param) {
	m_macroTab.m_pStatics[0] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1710
void FrOptionDlg::mOnMacroStatic2Init(int param) {
	m_macroTab.m_pStatics[1] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1715
bool FrOptionDlg::mOnMacroEnterKey(int key) {
	OnOK();
	Close(FrOK, true);
	return false;
}
// optiondlg.cpp:1722
void FrOptionDlg::mOnMacroEnterKeyLBtnDown(void) {
	OnLButtonDown(g_pFresh->GetManager()->GetMousePos());
}

// optiondlg.cpp:1727
void FrOptionDlg::mOnMacroStatic3Init(int param) {
	m_macroTab.m_pStatics[2] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1732
void FrOptionDlg::mOnMacroStatic4Init(int param) {
	m_macroTab.m_pStatics[3] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1737
void FrOptionDlg::mOnMacroStatic5Init(int param) {
	m_macroTab.m_pStatics[4] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1742
void FrOptionDlg::mOnMacroStatic6Init(int param) {
	m_macroTab.m_pStatics[5] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1747
void FrOptionDlg::mOnMacroStatic7Init(int param) {
	m_macroTab.m_pStatics[6] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1752
void FrOptionDlg::mOnMacroStatic8Init(int param) {
	m_macroTab.m_pStatics[7] = DYNAMIC_CAST(FrStatic, param);
}

// optiondlg.cpp:1757
void FrOptionDlg::mOnMacro1Init(int param) {
	m_macroTab.m_pMacroEdits[0] = DYNAMIC_CAST(FrEdit, param);
}

// optiondlg.cpp:1762
void FrOptionDlg::mOnMacro2Init(int param) {
	m_macroTab.m_pMacroEdits[1] = DYNAMIC_CAST(FrEdit, param);
}

// optiondlg.cpp:1767
void FrOptionDlg::mOnMacro3Init(int param) {
	m_macroTab.m_pMacroEdits[2] = DYNAMIC_CAST(FrEdit, param);
}

// optiondlg.cpp:1772
void FrOptionDlg::mOnMacro4Init(int param) {
	m_macroTab.m_pMacroEdits[3] = DYNAMIC_CAST(FrEdit, param);
}

// optiondlg.cpp:1777
void FrOptionDlg::mOnMacro5Init(int param) {
	m_macroTab.m_pMacroEdits[4] = DYNAMIC_CAST(FrEdit, param);
}

// optiondlg.cpp:1782
void FrOptionDlg::mOnMacro6Init(int param) {
	m_macroTab.m_pMacroEdits[5] = DYNAMIC_CAST(FrEdit, param);
}

// optiondlg.cpp:1787
void FrOptionDlg::mOnMacro7Init(int param) {
	m_macroTab.m_pMacroEdits[6] = DYNAMIC_CAST(FrEdit, param);
}

// optiondlg.cpp:1792
void FrOptionDlg::mOnMacro8Init(int param) {
	m_macroTab.m_pMacroEdits[7] = DYNAMIC_CAST(FrEdit, param);
}

// optiondlg.cpp:1797
bool FrOptionDlg::OnInit(void) {
	SetGameColtrols();
	InitVideoControls();
	SetVideoColtrols(true);
	SetAudioColtrols();
	SetMacroColtrols();

	eTabType curTab = ms_eCurTab;

	ms_eCurTab = (eTabType)4;
	SetTab(curTab);
	m_pTabs[ms_eCurTab]->m_pTabBtn->SetStatus(FrButton::PRESSED);

	return FrForm::OnInit();
}

// optiondlg.cpp:1824
bool FrOptionDlg::OnLButtonDown(const WPoint& pt) {
	if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI) : false) {
		if (g_input->GetButton(LEFT_BUTTON) == 1) {
			g_pFresh->GetManager()->SetExclusiveKey(true);
			CMessengerInfo::Instance()->SetGameFocus(false);
		}
	}

	return FrForm::OnLButtonDown(pt);
}

// optiondlg.cpp:1838
void FrOptionDlg::OnProc(const float deltaTime) {
	if (g_input->GetDown("TAB", true) && !g_input->Get("LALT", true) && !g_input->Get("RALT", true)) {
		switch (ms_eCurTab) {
		case TAB_GAME:
			SetTab(TAB_VIDEO);
			break;
		case TAB_VIDEO:
			SetTab(TAB_AUDIO);
			break;
		case TAB_AUDIO:
			SetTab(TAB_MACRO);
			break;
		case TAB_MACRO:
			SetTab(TAB_GAME);
			break;
		}
	}

	if (m_gameTab.m_pTabBtn && m_gameTab.m_pTabBtn->IsEnabled() && m_gameTab.m_pMsSensiGauge) {
		float pos = m_gameTab.m_pMsSensiGauge->GetPos();
		float sens;

		if (pos > 50.0f) {
			sens = pos * 0.02f;
		} else {
			sens = pos * 0.01f + 0.5f;
		}

		if (!WisEqual(m_option.m_mouseSensitivity, sens, g_EPSILON)) {
			m_option.m_mouseSensitivity = sens;
			g_input->SetSensitivity(sens);
		}
	}

	if (m_videoTab.m_pTabBtn && m_videoTab.m_pTabBtn->IsEnabled()) {
		bool bOverall = false;
		bool bRes = false;
		bool bAny = false;

		if (m_videoTab.m_pOverallCmb) {
			const char* text = m_option.vGetOverallText(m_option.m_overallLevel);
			const char* line = m_videoTab.m_pOverallCmb->GetLine(1, false);

			if (strcmp(text, line) != 0) {
				m_option.vResetByOverall(line);
				bOverall = true;
			}
		}

		if (!bOverall && m_videoTab.m_pScrResCmb) {
			const char* text = COption::Instance()->vGetScrResText(m_option.m_screenWidth, m_option.m_screenHeight, m_option.m_screenBpp, m_option.m_windowed != 0);
			const char* line = m_videoTab.m_pScrResCmb->GetLine(1, false);

			if (strcmp(text, line) != 0) {
				COption::Instance()->vGetScrResLvl(line, &m_option.m_screenWidth, &m_option.m_screenHeight, &m_option.m_screenBpp);
				bRes = true;
				bAny = true;
			}
		}

		if (m_videoTab.m_pFillScreenBtn) {
			if (m_option.m_windowed == 0) {
				m_videoTab.m_pFillScreenBtn->Enable(false);
			} else {
				m_videoTab.m_pFillScreenBtn->Enable(true);

				if (m_option.m_fillScreen) {
					m_gameTab.m_pPowerGaugeBtn->Enable(false);
				} else {
					m_gameTab.m_pPowerGaugeBtn->Enable(true);
				}
			}
		}

		if (!bOverall) {
			if (m_videoTab.m_pTexResCmb) {
				const char* text = m_option.vGetTexResText(m_option.m_mipmapLevel);
				const char* line = m_videoTab.m_pTexResCmb->GetLine(1, false);

				if (strcmp(text, line) != 0) {
					m_option.m_mipmapLevel = m_option.vGetTexResLvl(line);
					bRes = true;
					bAny = true;
				}
			}

			if (m_videoTab.m_pParticleCmb) {
				const char* text = m_option.vGetParticleText(m_option.m_particleLevel);
				const char* line = m_videoTab.m_pParticleCmb->GetLine(1, false);

				if (strcmp(text, line) != 0) {
					m_option.m_particleLevel = m_option.vGetParticleLvl(line);
					bRes = true;
					bAny = true;
				}
			}
		}

		if (bOverall) {
			SetVideoColtrols(false);
		} else if (bRes) {
			SetVideoColtrols(false);

			if (bAny) {
				m_option.m_overallLevel = -1;
				m_videoTab.m_pOverallCmb->SetLine(1, m_option.vGetOverallText(-1), 0, false, 0);
			}
		}
	}

	if (m_audioTab.m_pTabBtn && m_audioTab.m_pTabBtn->IsEnabled()) {
		if (m_audioTab.m_pSfxGauge) {
			float vol = m_audioTab.m_pSfxGauge->GetPos() * 0.01f;

			if (!WisEqual(m_option.m_sfxVolume, vol, g_EPSILON)) {
				m_option.m_sfxVolume = vol;

				if (m_option.m_mssEnabled == 1) {
					COption::Instance()->aApplySfxVolume(vol);
				}
			}
		}

		if (m_audioTab.m_pBgmGauge) {
			float vol = m_audioTab.m_pBgmGauge->GetPos() * 0.01f;

			if (!WisEqual(m_option.m_bgmVolume, vol, g_EPSILON)) {
				m_option.m_bgmVolume = vol;

				if (m_option.m_mssEnabled == 1) {
					COption::Instance()->aApplyBgmVolume(vol);
				}
			}
		}

		if (m_audioTab.m_pSpeakerCmb) {
			const char* text = m_option.aGetSpeakerText(m_option.m_speakerType);
			const char* line = m_audioTab.m_pSpeakerCmb->GetLine(1, false);

			if (strcmp(text, line) != 0) {
				m_option.m_speakerType = m_option.aGetSpeakerLvl(line);
			}
		}
	}
}

// optiondlg.cpp:2090
void FrOptionDlg::InitVideoControls(void) {
	int i;

	for (i = 0; i < 3; i++) {
		const char* text = m_option.vGetOverallText(i);
		m_videoTab.m_pOverallCmb->AddString(text);
	}

	COption::Instance()->CheckDisplayModeList(g_resrcmng->VideoReference(), m_option.m_windowed != 0, m_option.m_wideMode != 0);
	m_videoTab.m_pScrResCmb->ClearList();

	for (i = 0; i < COption::Instance()->vGetScrResNum(); i++) {
		const char* text = COption::Instance()->vGetScrResText(i);

		if (strlen(text)) {
			m_videoTab.m_pScrResCmb->AddString(text);
		}
	}

	if (sOption::sbHwTnlSupport) {
		const char* text = m_option.vGetTnlText(1);
		m_videoTab.m_pTnlCmb->AddString(text);
	}

	for (i = 0; i < 3; i++) {
		const char* text = m_option.vGetTexResText(i);
		m_videoTab.m_pTexResCmb->AddString(text);
	}

	for (i = 0; i < 3; i++) {
		const char* text = m_option.vGetParticleText(i);
		m_videoTab.m_pParticleCmb->AddString(text);
	}
}

// optiondlg.cpp:2150
void FrOptionDlg::SetVideoColtrols(bool bResetOverall) {
	if (!m_option.m_windowed) {
		m_videoTab.m_pFullScreenBtn->SetStatus(FrButton::PRESSED);
	}

	if (m_option.m_fillScreen) {
		m_videoTab.m_pFillScreenBtn->SetStatus(FrButton::PRESSED);
	}

	if (m_option.m_wideMode) {
		m_videoTab.m_pWideModeBtn->SetStatus(FrButton::PRESSED);
	}

	if (bResetOverall) {
		m_option.vResetByOverall(m_option.vGetOverallText(m_option.m_overallLevel));
	}

	const char* text = m_option.vGetOverallText(m_option.m_overallLevel);
	m_videoTab.m_pOverallCmb->SetLine(1, text, 0, false, 0);

	text = COption::Instance()->vGetScrResText(m_option.m_screenWidth, m_option.m_screenHeight, m_option.m_screenBpp, m_option.m_windowed != 0);
	m_videoTab.m_pScrResCmb->SetLine(1, text, 0, false, 0);

	text = m_option.vGetTnlText(m_option.m_tnlLevel);
	m_videoTab.m_pTnlCmb->SetLine(1, text, 0, false, 0);

	text = m_option.vGetTexResText(m_option.m_mipmapLevel);
	m_videoTab.m_pTexResCmb->SetLine(1, text, 0, false, 0);

	if (sOption::IsSupportShadow()) {
		m_videoTab.m_pShadowBtn->SetStatus(m_option.m_shadowEnabled == 1 ? FrButton::PRESSED : FrButton::NORMAL);
	}

	m_videoTab.m_pShadowmapBtn->SetStatus(m_option.m_shadowmapEnabled == 1 ? FrButton::PRESSED : FrButton::NORMAL);

	if (sOption::IsSupportReflection()) {
		m_videoTab.m_pReflectionBtn->SetStatus(m_option.m_reflectionEnabled == 1 ? FrButton::PRESSED : FrButton::NORMAL);
	}

	if (sOption::IsSupportLod()) {
		m_videoTab.m_pLodBtn->SetStatus(m_option.m_lodEnabled == 1 ? FrButton::PRESSED : FrButton::NORMAL);
	}

	m_videoTab.m_pNpcBtn->SetStatus(m_option.m_npcEnabled == 1 ? FrButton::PRESSED : FrButton::NORMAL);

	text = m_option.vGetParticleText(m_option.m_particleLevel);
	m_videoTab.m_pParticleCmb->SetLine(1, text, 0, false, 0);
}

// optiondlg.cpp:2213
void FrOptionDlg::SetAudioColtrols(void) {
	if (m_option.m_mssEnabled) {
		m_audioTab.m_pEnableBtn->SetStatus(FrButton::PRESSED);
	}

	for (int i = 0; i < 3; i++) {
		m_audioTab.m_pOverallCmb->AddString(m_option.aGetOverallText(i));
	}

	for (int i = 0; i < 6; i++) {
		m_audioTab.m_pSpeakerCmb->AddString(m_option.aGetSpeakerText(i));
	}

	const char* text = m_option.aGetOverallText(m_option.m_audioOverall);
	m_audioTab.m_pOverallCmb->SetLine(1, text, 0, false, 0);

	text = m_option.aGetSpeakerText(m_option.m_speakerType);
	m_audioTab.m_pSpeakerCmb->SetLine(1, text, 0, false, 0);

	m_audioTab.m_pSfxGauge->SetPos(Between(0, (int)(m_option.m_sfxVolume * 100.0f), 100));
	m_audioTab.m_pBgmGauge->SetPos(Between(0, (int)(m_option.m_bgmVolume * 100.0f), 100));
}

// optiondlg.cpp:2046
void FrOptionDlg::SetGameColtrols(void) {
	float pos = m_option.m_mouseSensitivity;

	if (pos > 1.0f)
		pos = pos * 50.0f;
	else
		pos = (pos - 0.5f) * 100.0f;

	pos = Between(0.0f, pos, 100.0f);

	m_gameTab.m_pMsSensiGauge->SetPos((int)pos);

	if (m_option.m_whisperAllowed && m_gameTab.m_pWhisperBtn)
		m_gameTab.m_pWhisperBtn->SetStatus(FrButton::PRESSED);
	if (m_option.m_invitationAllowed && m_gameTab.m_pInvitationBtn)
		m_gameTab.m_pInvitationBtn->SetStatus(FrButton::PRESSED);
	if (m_option.m_friendConfirmNeeded && m_gameTab.m_pFriendConfirmBtn)
		m_gameTab.m_pFriendConfirmBtn->SetStatus(FrButton::PRESSED);
	if (m_option.m_chatWinTransparent && m_gameTab.m_pTransChatWinBtn)
		m_gameTab.m_pTransChatWinBtn->SetStatus(FrButton::PRESSED);
	if (m_option.m_uiEffect && m_gameTab.m_pUiEffectBtn)
		m_gameTab.m_pUiEffectBtn->SetStatus(FrButton::PRESSED);
	if (m_option.m_powerGauge && m_gameTab.m_pPowerGaugeBtn)
		m_gameTab.m_pPowerGaugeBtn->SetStatus(FrButton::PRESSED);
	if (m_option.m_captureHideGUI && m_gameTab.m_pHideGUIBtn)
		m_gameTab.m_pHideGUIBtn->SetStatus(FrButton::PRESSED);
	if (m_option.m_captureHidePI && m_gameTab.m_pHidePIBtn)
		m_gameTab.m_pHidePIBtn->SetStatus(FrButton::PRESSED);
	if (m_gameTab.m_pNickEdit && CSharedDoc::Instance()->GetPangyaUserInfo().nickname[0])
		m_gameTab.m_pNickEdit->SetLine(1, CSharedDoc::Instance()->GetPangyaUserInfo().nickname, 0, false, 0);
	if (m_option.m_cutinFlag && m_gameTab.m_pCutinDisplayBtn)
		m_gameTab.m_pCutinDisplayBtn->SetStatus(FrButton::PRESSED);

	if (m_gameTab.m_pSchoolEdit && CSharedDoc::Instance()->GetPangyaUserInfo().schoolNo) {
		char szSchool[128];
		CSharedDoc::Instance()->LoadSchoolName(szSchool, CSharedDoc::Instance()->GetPangyaUserInfo().schoolNo);
		m_gameTab.m_pSchoolEdit->SetLine(1, szSchool, 0, false, 0);
	}
}

// optiondlg.cpp:2243
void FrOptionDlg::SetMacroColtrols(void) {
	for (int i = 0; i < 8; i++) {
		CSharedDoc::Instance()->m_chatManager.FilteringHack(m_option.m_macroText[i], false);
		m_macroTab.m_pMacroEdits[i]->SetLine(1, m_option.m_macroText[i], 0, false, 0);
	}
}

// optiondlg.cpp:2254
void FrOptionDlg::SetTab(eTabType type) {
	if (m_hoverCount) {
		m_hoverCount = 0;
	}

	if (type < TAB_GAME || type >= 4 || type == ms_eCurTab) {
		return;
	}

	ms_eCurTab = type;
	g_audio->PlaySfx("ui_button_ok_click", 0, 0, 0, 0, 0.5f, 200.0f);

	for (int i = 0; i < 4; i++) {
		m_pTabs[i]->SetSelected(i == type);
		m_pTabs[i]->m_pTabBtn->SetStatus(i == type ? FrButton::PRESSED : FrButton::NORMAL);

		if (!g_audio->GetMSS()) {
			if (type == TAB_AUDIO) {
				m_pHintEdit->ClearLine();
				// "No audio device is available."
				m_pHintEdit->AddText("\xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xbf\xc0\xb5\xf0\xbf\xc0 \xc0\xe5\xc4\xa1\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false, true);
				m_pHintEdit->SetVisible(true);
			} else {
				m_pHintEdit->SetVisible(false);
			}
		}
	}
}

// optiondlg.cpp:2302
void FrOptionDlg::OnCancel(void) {
	if (((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI) : false) && !CChatMsg::Instance()->IsOpened()) {
		g_pFresh->GetManager()->SetExclusiveKey(true);

		CMessengerInfo::Instance()->SetGameFocus(true);
	}

	g_input->SetSensitivity(Between(0.5f, COption::Instance()->gGetMouseSensitivity(), 2.0f));
	if (m_audioTab.m_pEnableBtn->GetStatus() == FrButton::PRESSED) {
		g_audio->SetBGMVolume(COption::Instance()->aGetBgmVolume());
		g_audio->SetVolume(COption::Instance()->aGetSfxVolume());
	} else {
		g_audio->SetVolume(0);
		g_audio->SetBGMVolume(0);
	}

	FrForm::OnCancel();
}

// optiondlg.cpp:2326
void FrOptionDlg::OnOK(void) {
	if (((__rtti_obj = AfxGetTask()) != 0 ? ((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI) : false) && !CChatMsg::Instance()->IsOpened()) {
		g_pFresh->GetManager()->SetExclusiveKey(true);

		CMessengerInfo::Instance()->SetGameFocus(true);
	}

	m_bChanged1 = true;
	m_bChanged2 = true;

	CloseDlg();
}

// optiondlg.cpp:2346
void FrOptionDlg::CloseDlg(void) {
	if (m_gameTab.m_pTerrainBtn->GetStatus() == FrButton::PRESSED) {
		m_option.m_terrainTooltip = 1;
	} else {
		m_option.m_terrainTooltip = 0;
	}

	bool bTerrainTooltip = m_option.m_terrainTooltip != 0;
	CMouseCursor::Instance()->SetTerrainToolTip(bTerrainTooltip);

	if (!m_bChanged1) {
		EnableChangeNickControls(true);
		return;
	}

	for (int i = 0; i < 8; i++) {
		strncpy(m_option.m_macroText[i], m_macroTab.m_pMacroEdits[i]->GetLine(1, false), 0x3f);
		m_option.m_macroText[i][0x3f] = 0;
		CSharedDoc::Instance()->m_chatManager.FilteringHack(m_option.m_macroText[i], true);
	}

	COption::Instance()->ApplyChange(m_option, m_bApplyScrSize);

	sGameOptionInfo info;
	memset(&info, 0, sizeof(info));

	for (int j = 0; j < 8; j++) {
		strcpy(info.m_macroText[j], m_option.m_macroText[j]);
	}

	WSendPacket packet(GCT_UPDATE_GAME_OPTION);
	packet.EncodeBuffer(&info, sizeof(info));
	packet.Send(SEND_TO_DEFAULT);

	FrForm::OnOK();
}

// optiondlg.cpp:2398
void FrOptionDlg::EnableChangeNickControls(bool bEnable) {
	if (m_gameTab.m_pChangeNickBtn && m_gameTab.m_pChangeNickBtn->IsVisible()) {
		m_gameTab.m_pChangeNickBtn->Enable(bEnable);
	}

	if (m_pOkBtn && m_pOkBtn->IsVisible()) {
		m_pOkBtn->Enable(bEnable);
	}
}

// optiondlg.cpp:2413
void FrOptionDlg::tOnHintInit(int param) {
	m_pHintEdit = DYNAMIC_CAST(FrEdit, param);
	m_pHintEdit->SetVisible(false);
}

// optiondlg.cpp:2420
const char* FrOptionDlg::MakeHint(eTabType type, int index) const {
	switch (type) {
	case TAB_GAME:
		switch (index) {
		case 0:
			return "\xb8\xb6\xbf\xec\xbd\xba \xb0\xa8\xb5\xb5\xb8\xa6 \xc1\xb6\xc0\xfd\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 1:
			return "\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xb1\xd3\xb8\xbb\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 \xc0\xd6\xb5\xb5\xb7\xcf \xc7\xe3\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 2:
			return "\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 \xc3\xca\xb4\xeb\xb8\xa6 \xc7\xe3\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 3:
			return "\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 \xc4\xa3\xb1\xb8 \xb5\xee\xb7\xcf\xbf\xe4\xc3\xbb\xbf\xa1 \xc0\xda\xb5\xbf\xc0\xb8\xb7\xce \xb5\xbf\xc0\xc7\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 4:
			return "\xb4\xeb\xc8\xad\xc7\xa5\xbd\xc3\xbf\xb5\xbf\xaa \xb9\xd9\xc5\xc1\xc0\xbb \xc5\xf5\xb8\xed\xc7\xcf\xb0\xd4 \xc7\xd5\xb4\xcf\xb4\xd9.";
		case 5:
			return "\xc3\xa2\xc0\xbb \xbf\xad\xb0\xc5\xb3\xaa \xb4\xdd\xc0\xbb \xb6\xa7 \xbe\xd6\xb4\xcf\xb8\xde\xc0\xcc\xbc\xc7 \xc8\xbf\xb0\xfa\xb8\xa6 \xc1\xdd\xb4\xcf\xb4\xd9.";
		case 6:
			return "\xb4\xeb\xc8\xad\xb8\xed\xc0\xbb \xba\xaf\xb0\xe6\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 7:
			return "\xc7\xd0\xb1\xb3 \xb7\xa9\xc5\xb7\xbf\xa1 \xbb\xe7\xbf\xeb\xb5\xc9 \xbc\xd2\xbc\xd3 \xc7\xd0\xb1\xb3\xb8\xa6 \xba\xaf\xb0\xe6\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 8:
			return "\xc7\xe2\xbb\xf3\xb5\xc8 \xc3\xd6\xbd\xc5\xc0\xc7 \xc5\xb8\xb1\xb8\xb9\xd9\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9(\xb1\xc7\xc0\xe5).";
		case 9:
			return "\xc4\xb8\xc3\xc4\xbd\xc3 GUI \xba\xce\xba\xd0\xc0\xbb \xb0\xa8\xc3\xdf\xb0\xed \xc0\xfa\xc0\xe5\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 10:
			return "\xc4\xb8\xc3\xc4\xbd\xc3 \xb0\xb3\xc0\xce\xc1\xa4\xba\xb8\xb8\xa6 \xb0\xa8\xc3\xdf\xb0\xed \xc0\xfa\xc0\xe5\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 11:
			return "\xc1\xf6\xb8\xe9\xbc\xd3\xbc\xba\xc5\xf8\xc6\xc1\xc0\xbb \xc4\xd1\xb0\xc5\xb3\xaa \xb2\xfc\xb4\xcf\xb4\xd9.";
		case 12:
			return "\xc4\xc6\xc0\xce \xc0\xcc\xb9\xcc\xc1\xf6\xb0\xa1 \xb0\xd4\xc0\xd3\xc1\xdf\xbf\xa1 \xbe\xc8\xba\xb8\xc0\xcc\xb5\xb5\xb7\xcf \xc7\xd5\xb4\xcf\xb4\xd9.";
		}
		break;
	case TAB_VIDEO:
		switch (index) {
		case 0:
			return "\xc6\xce\xbe\xdf\xb8\xa6 \xc0\xfc\xc3\xbc\xc8\xad\xb8\xe9\xc0\xb8\xb7\xce \xc1\xf1\xb1\xe6 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.";
		case 1:
			return "\xc6\xce\xbe\xdf\xb8\xa6 \xc0\xfc\xc3\xbc\xc3\xa2 \xc8\xad\xb8\xe9\xc0\xb8\xb7\xce \xc1\xf1\xb1\xe6 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.";
		case 2:
			return "\xc6\xce\xbe\xdf\xb8\xa6 \xbf\xcd\xc0\xcc\xb5\xe5 \xc8\xad\xb8\xe9\xbf\xa1\xbc\xad \xc1\xf1\xb1\xe6 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.";
		case 5:
			return "\xb1\xd7\xb7\xa1\xc7\xc8 \xc3\xb3\xb8\xae\xb9\xe6\xbd\xc4\xc0\xbb \xbc\xb1\xc5\xc3\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0(\xc0\xe7\xbd\xc3\xc0\xdb\xc7\xca\xbf\xe4).";
		case 3:
			return "\xbf\xa9\xb7\xaf\xb0\xa1\xc1\xf6 \xba\xf1\xb5\xf0\xbf\xc0 \xbc\xb3\xc1\xa4\xc0\xbb \xc7\xd1\xb2\xa8\xb9\xf8\xbf\xa1 \xb9\xd9\xb2\xdc \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.";
		case 4:
			return "\xc8\xad\xb8\xe9\xc0\xc7 \xc7\xd8\xbb\xf3\xb5\xb5\xb8\xa6 \xb0\xe1\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 6:
			return "\xc0\xfb\xc1\xa4\xc7\xd1 \xb1\xd7\xb7\xa1\xc7\xc8 \xbc\xbc\xb9\xd0\xb5\xb5\xb8\xa6 \xbc\xb1\xc5\xc3\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0(\xc0\xe7\xbd\xc3\xc0\xdb\xc7\xca\xbf\xe4).";
		case 7:
			return "\xc6\xce\xbe\xdf \xc1\xd6\xc0\xce\xb0\xf8\xb5\xe9\xc0\xc7 \xb1\xd7\xb8\xb2\xc0\xda\xb8\xa6 \xc4\xd1\xb0\xc5\xb3\xaa \xb2\xfc\xb4\xcf\xb4\xd9.";
		case 8:
			return "\xb3\xaa\xb9\xab\xb3\xaa \xb0\xc7\xb9\xb0 \xb5\xee\xc0\xc7 \xb1\xd7\xb8\xb2\xc0\xda\xb8\xa6 \xc4\xd1\xb0\xc5\xb3\xaa \xb2\xfc\xb4\xcf\xb4\xd9.";
		case 9:
			return "\xb9\xb0\xc7\xa5\xb8\xe9\xc0\xc7 \xb9\xdd\xbb\xe7 \xc8\xbf\xb0\xfa\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 10:
			return "LOD \xb1\xe2\xb4\xc9\xc0\xbb \xc8\xb0\xbc\xba\xc8\xad \xbd\xc3\xc5\xb5\xb4\xcf\xb4\xd9.";
		case 11:
			return "\xc6\xce\xbe\xdf\xbc\xb6 \xb0\xf7\xb0\xf7\xc0\xc7 \xc0\xdb\xc0\xba \xc4\xa3\xb1\xb8\xb5\xe9\xc0\xbb \xb8\xb8\xb3\xaa\xba\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?";
		case 12:
			return "\xbf\xa9\xb7\xaf\xb0\xa1\xc1\xf6 \xc6\xaf\xbc\xf6\xc8\xbf\xb0\xfa\xb5\xe9\xc0\xc7 \xc7\xb0\xc1\xfa\xc0\xbb \xb0\xe1\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9.";
		}
		break;
	case TAB_AUDIO:
		switch (index) {
		case 0:
			return "\xb8\xf0\xb5\xe7 \xbc\xd2\xb8\xae\xb8\xa6 \xc4\xd1\xb0\xc5\xb3\xaa \xb2\xfc\xb4\xcf\xb4\xd9.";
		case 1:
			return "\xc0\xfb\xc1\xa4\xc7\xd1 \xc0\xbd\xc1\xfa\xc0\xbb \xbc\xb1\xc5\xc3\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0.";
		case 2:
			return "\xc8\xbf\xb0\xfa\xc0\xbd\xc0\xc7 \xba\xbc\xb7\xfd\xc0\xbb \xc1\xb6\xc0\xfd\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 3:
			return "\xb9\xe8\xb0\xe6\xc0\xbd\xbe\xc7\xc0\xc7 \xba\xbc\xb7\xfd\xc0\xbb \xc1\xb6\xc0\xfd\xc7\xd5\xb4\xcf\xb4\xd9.";
		case 4:
			return "\xbd\xba\xc7\xc7\xc4\xbf \xc5\xb8\xc0\xd4\xc0\xbb \xb0\xe1\xc1\xa4\xc7\xd5\xb4\xcf\xb4\xd9.";
		}
		break;
	case TAB_MACRO:
		return 0;
	}

	return 0;
}

// optiondlg.cpp:2532
void FrOptionDlg::ProcessHoverMessage(FrWnd* pWnd) {
	if (!m_pHintEdit || !m_pTabs[ms_eCurTab]->m_bSelected) {
		return;
	}

	if (m_hoverCount > 0) {
		if (pWnd) {
			for (int i = 0; i < m_pTabs[ms_eCurTab]->m_numWnds; i++) {
				if (*m_pTabs[ms_eCurTab]->m_pppHoverWnds[i] == pWnd) {
					const char* hint = MakeHint(ms_eCurTab, i);
					if (hint) {
						m_pHintEdit->ClearLine();
						m_pHintEdit->AddText(hint, false, true);
						m_pHintEdit->SetVisible(true);
					}
				}
			}
		}
	} else if (m_hoverCount == 0) {
		m_pHintEdit->SetVisible(false);
	}
}

// optiondlg.cpp:2567
void FrOptionDlg::OnHoverOn(int param) {
	m_hoverCount++;
	ProcessHoverMessage(DYNAMIC_CAST(FrWnd, param));
}

// optiondlg.cpp:2573
void FrOptionDlg::OnHoverOff(int param) {
	m_hoverCount--;
	ProcessHoverMessage(0);
}

// optiondlg.cpp:2579
bool FrOptionDlg::OnChangeNickConfirmDlgResult(int result, FrForm* pForm) {
	m_pChangeNickDlg = 0;
	EnableChangeNickControls(true);

	return true;
}

// optiondlg.cpp:2588
bool FrOptionDlg::OnChangeSchoolDlgResult(int result, FrForm* pForm) {
	if (result == 1 && m_pSchoolDlg) {
		if (m_pSchoolDlg->m_schoolName.length() > 0) {
			char szSchool[1024];
			strcpy(szSchool, m_pSchoolDlg->m_schoolName.c_str());
			m_gameTab.m_pSchoolEdit->SetLine(1, strtok(szSchool, "("), 0, false, 0);
		} else {
			m_gameTab.m_pSchoolEdit->SetLine(1, "", 0, false, 0);
		}

		WSendPacket packet((enumClientPacket)0x3B);   // change of school (no named id)
		packet.Encode4(m_pSchoolDlg->m_schoolNo);
		packet.Send(SEND_TO_DEFAULT);

		IActor* pActor = AfxGetTask()->GetActor("Lobby");
		if (pActor) {
			pActor << MsgObject(0, 1, 0, 0, 0, 0, 0);
		}
	}

	m_pSchoolDlg = 0;

	return true;
}
