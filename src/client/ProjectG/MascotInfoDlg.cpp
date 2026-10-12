// Original: d:\build\custom\temp\code\client\projectg\mascotinfodlg.cpp
#include <stdio.h>
#include <string.h>
#include "MascotInfoDlg.h"
#include "ObjectFactory.h"
#include "frarea.h"
#include "frbutton.h"
#include "fredit.h"
#include "frstatic.h"
#include "logininfo.h"
#include "mascot.h"
#include "commonutil.h"
#include "SharedDoc.h"
#include "shared/classdefine.h"
#include "Packet.h"
#include "TaskManager.h"
#include "actor.h"
#include "binstr.h"
#include "fresh.h"
#include "frwndmanager.h"

// mascotinfodlg.cpp:21 (run-time type and factory registration of FrMascotInfoDlg)
const WRTTI FrMascotInfoDlg::m_RTTI("FrMascotInfoDlg", &FrForm::m_RTTI);

IObject* FrMascotInfoDlgMakeInstance(void) {
	return new FrMascotInfoDlg;
}

struct __sFrMascotInfoDlg {
	__sFrMascotInfoDlg(void) {
		ObjectFactory().AddObjectFunctor(FrMascotInfoDlgMakeInstance, "FrMascotInfoDlg");
	}
};
static __sFrMascotInfoDlg s_registerFrMascotInfoDlg;

// mascotinfodlg.cpp:23
const sFRESH_MSGMAP* FrMascotInfoDlg::GetMessageMap(void) const {
	return &FrMascotInfoDlg::_MsgMap;
}

sFRESH_MSGMAP FrMascotInfoDlg::_MsgMap = { &FrForm::_MsgMap, &FrMascotInfoDlg::_MsgEntries[0] };

sFRESH_ENTRY FrMascotInfoDlg::_MsgEntries[] = {
	// mascotinfodlg.cpp:25
	{ "portrait", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMascotInfoDlg::OnPortraitInit },
	// mascotinfodlg.cpp:26
	{ "vacation", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMascotInfoDlg::OnVacationInit },
	// mascotinfodlg.cpp:27
	{ "limit", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMascotInfoDlg::OnLimitInit },
	// mascotinfodlg.cpp:28
	{ "name", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMascotInfoDlg::OnNameInit },
	// mascotinfodlg.cpp:29
	{ "text_message", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMascotInfoDlg::OnTextMessageInit },
	// mascotinfodlg.cpp:30
	{ "mascot_msg", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMascotInfoDlg::OnMascotMsgInit },
	// mascotinfodlg.cpp:31
	{ "ok", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMascotInfoDlg::OnOkInit },
	// mascotinfodlg.cpp:32
	{ "ok", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrMascotInfoDlg::OnOkBtnUp },
	// mascotinfodlg.cpp:33
	{ "cancel", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMascotInfoDlg::OnCancelInit },
	// mascotinfodlg.cpp:34
	{ "cancel", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrMascotInfoDlg::OnCancelBtnUp },

	// mascotinfodlg.cpp:36
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// mascotinfodlg.cpp:40
FrMascotInfoDlg::FrMascotInfoDlg(void) {
	memset(&m_info, 0, sizeof(m_info));
	m_pPortrait = 0;
	m_pVacation = 0;
	m_pLimit = 0;
	m_pName = 0;
	m_pMascotMsg = 0;
	m_pOk = 0;
	m_pCancel = 0;
	m_pConfirmDlg = 0;
	m_pPuppet = 0;
}

// mascotinfodlg.cpp:57
void FrMascotInfoDlg::SetMascotInfo(const sMascotInfo& info, WPuppet* pPuppet) {


	m_info = info;
	m_msg = m_info.message;
	m_pPuppet = pPuppet;
}

// mascotinfodlg.cpp:75
void FrMascotInfoDlg::OnPortraitInit(intptr_t param) {
	m_pPortrait = DYNAMIC_CAST(FrArea, param);
}

// mascotinfodlg.cpp:80
void FrMascotInfoDlg::OnVacationInit(intptr_t param) {
	m_pVacation = DYNAMIC_CAST(FrArea, param);

	if (m_pVacation) {
		m_pVacation->SetVisible(false);
	}
}

// mascotinfodlg.cpp:88
void FrMascotInfoDlg::OnLimitInit(intptr_t param) {
	m_pLimit = DYNAMIC_CAST(FrArea, param);


	if (m_pLimit) {
		m_pLimit->SetVisible(false);
	}
}

// mascotinfodlg.cpp:96
void FrMascotInfoDlg::OnNameInit(intptr_t param) {
	m_pName = DYNAMIC_CAST(FrEdit, param);
}

// mascotinfodlg.cpp:101
void FrMascotInfoDlg::OnTextMessageInit(intptr_t param) {
	m_pTextMessage = DYNAMIC_CAST(FrStatic, param);


	if (m_pTextMessage) {
		m_pTextMessage->SetVisible(false);
	}
}

// mascotinfodlg.cpp:109
void FrMascotInfoDlg::OnMascotMsgInit(intptr_t param) {
	m_pMascotMsg = DYNAMIC_CAST(FrEdit, param);


	if (m_pMascotMsg) {
		m_pMascotMsg->SetVisible(false);
	}
}

// mascotinfodlg.cpp:119
void FrMascotInfoDlg::OnOkInit(intptr_t param) {
	m_pOk = DYNAMIC_CAST(FrButton, param);


	if (m_pOk) {
		m_pOk->Enable(false);
	}
}

// mascotinfodlg.cpp:127
void FrMascotInfoDlg::OnOkBtnUp(void) {
	if (strcmp(m_msg.c_str(), m_info.message) != 0) {

		if (m_info.typeId == 0x40000004) {
			if (strlen(m_msg.c_str()) > 8) {
				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xB8\xDE\xBC\xBC\xC1\xF6 \xC3\xD6\xB4\xEB \xB1\xE6\xC0\xCC\xB4\xC2 \xBF\xB5\xB9\xAE 8\xB1\xDB\xC0\xDA, \xC7\xD1\xB1\xDB 4\xB1\xDB\xC0\xDA \xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
				return;
			}
		}
		else {
			if (strlen(m_msg.c_str()) > 16) {
				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xB8\xDE\xBC\xBC\xC1\xF6 \xC3\xD6\xB4\xEB \xB1\xE6\xC0\xCC\xB4\xC2 \xBF\xB5\xB9\xAE 16\xB1\xDB\xC0\xDA, \xC7\xD1\xB1\xDB 8\xB1\xDB\xC0\xDA \xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
				return;
			}
		}

		cTokenV token;
		token.Init(m_msg.c_str(), strlen(m_msg.c_str()));
		int count = token.GetTokenNum(0, "'", 1) - 1;

		if (count > 0) {
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"[']\xB4\xC2 \xB8\xDE\xBC\xBC\xC1\xF6\xBF\xA1 \xBB\xE7\xBF\xEB\xC7\xD2 \xBC\xF6 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
		}
		else {
			char text[256];
			strcpy(text, m_msg.c_str());

			if (WSingleton<CSharedDoc>::Instance()->m_chatManager.Filtering(text)) {
				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xC0\xFB\xB4\xE7\xC7\xCF\xC1\xF6\xBE\xCA\xC0\xBA \xB4\xDC\xBE\xEE\xB0\xA1 \xC6\xF7\xC7\xD4\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			}
			else {
				if (!m_pConfirmDlg) {
					m_pConfirmDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "mascot_msg", 0);
				}

				if (m_pConfirmDlg) {
					m_pConfirmDlg->SetMessage("\xB8\xB6\xBD\xBA\xC4\xDA\xC6\xAE \xB8\xDE\xBC\xBC\xC1\xF6\xB8\xA6 \xBA\xAF\xB0\xE6 \xC7\xCF\xBD\xC3\xB0\xDA\xBD\xC0\xB4\xCF\xB1\xEE?", false);
					m_pConfirmDlg->Open((FRESH_RESULT_CALLBACK)&FrMascotInfoDlg::OnChangeMsgConfirmDlgResult, 1);
				}
			}
		}
	}
}

// mascotinfodlg.cpp:274
void FrMascotInfoDlg::OnCancelInit(intptr_t param) {
	m_pCancel = DYNAMIC_CAST(FrButton, param);
}

// mascotinfodlg.cpp:279
void FrMascotInfoDlg::OnCancelBtnUp(void) {
	if (strcmp(m_info.message, m_msg.c_str()) != 0) {

		IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(m_info.typeId);

		if (m_pPuppet && pMascot) {
			CMascot::ChangeMascotMsgInTex(m_info.message, pMascot, m_pPuppet, MakeStr("myroom%02d", pMascot->m_typeId & 0x3FFFFFF));
		}
	}

	OnFreshCancel();
}

// mascotinfodlg.cpp:292
bool FrMascotInfoDlg::OnChangeMsgConfirmDlgResult(int result, FrForm* pForm) {
	m_pConfirmDlg = 0;

	if (result == 1) {

		if (CLoginInfo::Instance()->IsPcBang() && m_info.pcBang) {

			WSendPacket packet(GCT_UPDATE_PCBANG_MASCOTMSG);
			packet.Encode1(1);
			packet.Encode4(m_info.id);
			packet.EncodeStr(m_msg);
			packet.Send(SEND_TO_DEFAULT);
		}
		else {
			WSendPacket packet(GCT_CHANGE_MASCOT);
			packet.Encode4(m_info.id);
			packet.EncodeStr(m_msg);
			packet.Send(SEND_TO_DEFAULT);
		}
		AfxGetTask()->PassMsgToActor("RealMyRoom", MsgObject(0, 1, 0, 0, 0, 0, 0));

		Close(FrOK, true);
	}



	return true;
}

// mascotinfodlg.cpp:321
bool FrMascotInfoDlg::OnInit(void) {
	if (m_info.typeId == 0) {
		return false;
	}

	if ((m_info.typeId & 0x3FFFFFF) == 4) {
		m_pMascotMsg->SetCharLimit(8, true);
	}

	IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(m_info.typeId);

	bool described = false;

	if (m_pPortrait) {
		if (pMascot) {
			m_pPortrait->SetBgImg(pMascot->m_icon);

			if (m_pName) {
				char text[512] = "";

				sprintf(text, "\xc0\xcc  \xb8\xa7: %s", pMascot->m_name);

				if (pMascot->m_itemDropRate > 0) {
					strcat(text, MakeStr("\n\xbe\xc6\xc0\xcc\xc5\xdb \xb5\xe5\xb7\xd3\xb7\xfc : %d%%", pMascot->m_itemDropRate));
				}
				if (pMascot->m_pangRate > 0) {
					strcat(text, MakeStr("\n\xc3\xdf\xb0\xa1 \xc6\xce \xc8\xb9\xb5\xe6\xb7\xfc : %d%%", pMascot->m_pangRate - 100));
				}
				if (pMascot->m_expRate > 0) {
					strcat(text, MakeStr("\n\xc3\xdf\xb0\xa1 \xb0\xe6\xc7\xe8\xc4\xa1 \xc8\xb9\xb5\xe6\xb7\xfc : %d%%", pMascot->m_expRate - 100));
				}
				if (pMascot->m_addItemSlots > 0) {
					strcat(text, MakeStr("\n\xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4 : %d\xc4\xad", pMascot->m_addItemSlots));
				}

				unsigned short days = m_info.remainHours / 24, hours = m_info.remainHours % 24;

				strcat(text, "\n\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3 : ");

				bool unlimited = false;

				if (IsLocalContent((localContentType_t)0x6C)) {
					if (m_info.typeId == 0x40000006) {
						strcat(text, "\xb9\xab\xc1\xa6\xc7\xd1");
						unlimited = true;
					}

					if (m_info.typeId == 0x40000004) {
						strcat(text, "PC\xb9\xe6 \xb9\xab\xc1\xa6\xc7\xd1");
						unlimited = true;
					}

					if (m_info.typeId == 0x40000013) {
						strcat(text, "\xb9\xab\xc1\xa6\xc7\xd1");
						unlimited = true;
					}
				}

				if (!unlimited) {
					if (days > 0) {
						strcat(text, MakeStr("%d\xc0\xcf ", days));
					}
					if (hours > 0) {
						strcat(text, MakeStr("%d\xbd\xc3\xb0\xa3", hours));
					}
					if (days <= 0 && hours <= 0) {
						strcat(text, "\xc1\xbe\xb7\xe1");
					}
				}

				m_pName->AddText(text, false, true);

				IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pMascot->m_typeId);
				if (pDesc) {
					SetMessage(pDesc->m_text, false);
				}

				described = true;
			}
		}
		else {
			m_pPortrait->SetBgImg("hide");
		}
	}

	if (!described) {
		if (m_pName) {
			m_pName->AddText("\xc0\xcc\xb8\xa7: \xba\xd2\xb8\xed\n\n\xb7\xb9\xba\xa7: \xba\xd2\xb8\xed", false, true);
		}

		SetMessage("\xba\xa3\xc0\xcf\xbf\xa1 \xbd\xce\xc0\xce \xc1\xa4\xc3\xbc\xba\xd2\xb8\xed\xc0\xc7 \xc1\xb8\xc0\xe7", false);
	}

	if (pMascot) {
		if (WSingleton<CSharedDoc>::Instance()->m_myLevel < (pMascot->m_level & 0x7F)) {
			if (m_pLimit) {
				m_pLimit->SetVisible(true);
			}

			SetDesc("\xb7\xb9\xba\xa7 \xc1\xa6\xc7\xd1\xc0\xb8\xb7\xce \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		}

		if (pMascot->m_msgInTexture) {
			if (m_pMascotMsg) {
				m_pMascotMsg->SetVisible(true);
				m_pMascotMsg->SetLine(1, m_info.message, 0, false, 0);
			}

			if (m_pTextMessage) {
				m_pTextMessage->SetVisible(true);
			}

			if (m_pOk) {
				m_pOk->Enable(true);
			}
		}
	}

	return true;
}

// mascotinfodlg.cpp:506
void FrMascotInfoDlg::OnProc(const float dt) {
	if (m_pMascotMsg) {

		if (stricmp(m_pMascotMsg->GetLine(1, false), m_msg.c_str()) != 0) {

			m_msg = m_pMascotMsg->GetLine(1, false);

			IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(m_info.typeId);

			if (m_pPuppet && pMascot) {

				CMascot::ChangeMascotMsgInTex(m_msg.c_str(), pMascot, m_pPuppet, MakeStr("myroom%02d", pMascot->m_typeId & 0x3FFFFFF));
			}
		}
	}
}
