// JP-only game code: FrUseTikiReportDlg, the form of layout "notify_tiki".
// See JpNotifyTikiDlg.h.
#include "JpNotifyTikiDlg.h"

#include "JpLobbyGlue.h"
#include "Localize.h"
#include "ObjectFactory.h"
#include "Packet.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "actor.h"
#include "fredit.h"
#include "fresh.h"
#include "frwnd.h"
#include "frwndmanager.h"

#include <list>
#include <vector>

// lib: 0xafa410 (deleting destructor), 0xafa360, 0xafa370 (this-adjusting
// deleting-destructor thunks),
// 0xcf1b90 (m_RTTI's dynamic initializer), 0xcf1bd0 (message-map
// initializer), 0xcf1bb0 (factory registration)

// JpGrandPrixAwardDlg.cpp: JP CSharedDoc +0xffc8, 0x109 bytes.
const void* JpSharedDoc_GetMyGameStatistics(void);
// 0xb4eb80 (RFTextManager.h): the text the JP text table gives a key.
const char* Jp_LookupText(const char* key);

// The tiki report item.
static const unsigned long kTikiReportTypeId = 0x1A000041;

const WRTTI FrUseTikiReportDlg::m_RTTI("FrUseTikiReportDlg", &FrForm::m_RTTI);

// 0xafa2b0
const sFRESH_MSGMAP* FrUseTikiReportDlg::GetMessageMap(void) const {
	return &FrUseTikiReportDlg::_MsgMap;
}

sFRESH_MSGMAP FrUseTikiReportDlg::_MsgMap = { &FrForm::_MsgMap, &FrUseTikiReportDlg::_MsgEntries[0] };

sFRESH_ENTRY FrUseTikiReportDlg::_MsgEntries[] = {
	{ "notice", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUseTikiReportDlg::OnNoticeInit },
	{ "amount", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrUseTikiReportDlg::OnAmountInit },
	{ "yes", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUseTikiReportDlg::OnYesClick },
	{ "no", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrUseTikiReportDlg::OnNoClick },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0xafa2c0
FrUseTikiReportDlg::FrUseTikiReportDlg(void) {
	m_unknown110 = NULL;
	m_pNotice = NULL;
	m_pAmount = NULL;
}

FrUseTikiReportDlg::~FrUseTikiReportDlg(void) {
}

// 0xafa320
void FrUseTikiReportDlg::SetNotice(const char* text) {
	m_pNotice->AddText(text, false, true);
}

// 0xafa340
void FrUseTikiReportDlg::SetAmount(const char* text) {
	m_pAmount->AddText(text, true, true);
}

// 0xafa440
void FrUseTikiReportDlg::OnNoticeInit(PG_MSGARG param) {
	m_pNotice = DYNAMIC_CAST(FrEdit, param);
}

// 0xafa4b0
void FrUseTikiReportDlg::OnAmountInit(PG_MSGARG param) {
	m_pAmount = DYNAMIC_CAST(FrEdit, param);
}

// 0xafa730
void FrUseTikiReportDlg::OnYesClick(void) {
	CSharedDoc* shared = CSharedDoc::Instance();
	std::list<sItemInfo>::iterator it;
	for (it = shared->m_itemInfoList.begin(); it != shared->m_itemInfoList.end(); ++it) {
		if (it->m_typeId == kTikiReportTypeId) {
			break;
		}
	}
	if (it != shared->m_itemInfoList.end() && it->m_count > 0) {
		--it->m_count;
		if (it->m_count < 1) {
			shared->m_itemInfoList.erase(it);
		}
	}

	WSendPacket packet(0xaa);
	packet.EncodeBuffer(JpSharedDoc_GetMyGameStatistics(), 0x109);
	packet.Send(SEND_TO_DEFAULT);

	FrWnd* yes = FindChildByName("yes");
	if (yes != NULL) {
		yes->Enable(false);
	}
	Jp_GameState_Set10488(false);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	Close(FrOK, false);
}

// 0xafa520
bool FrUseTikiReportDlg::OnSubDlgResult(int result, FrForm* form) {
	if (result != FrOK) {
		return true;
	}
	FrWnd* ok = form->FindChildByName("ok");
	if (ok != NULL) {
		ok->Enable(false);
	}

	// The KR SendRoomExitPacket's packet: in a room of kind 4 or 5 (JP
	// CSharedDoc +0x1094c) of the local content 0x14, the local player's
	// result line (JP +0x1042c, 0x1b8 bytes: the KR m_gameResultList)
	// gives its two 64-bit values.
	WSendPacket packet(0xf);
	packet.Encode1(0);
	packet.Encode2(0xFFFF);
	CSharedDoc* shared = CSharedDoc::Instance();
	const sRivalData* mine = NULL;
	if (IsLocalContent((localContentType_t)0x14)) {
		int kind = JpSharedDoc_GetRoomGameType();
		if (kind == 4 || kind == 5) {
			for (std::vector<sRivalData>::iterator it = shared->m_gameResultList.begin(); it != shared->m_gameResultList.end(); ++it) {
				if (it->m_uid == MyGuid(false)) {
					mine = &*it;
					break;
				}
			}
		}
	}
	if (mine != NULL) {
		packet.Encode8(mine->m_pang);
		packet.Encode8(mine->m_extraPang);
	} else {
		packet.Encode8(0);
		packet.Encode8(0);
	}
	packet.Send(SEND_TO_DEFAULT);

	Jp_GameState_Set10488(false);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	Close(FrNO, true);
	return true;
}

// 0xafa6b0
void FrUseTikiReportDlg::OnNoClick(void) {
	FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_tiki_sub", 0);
	// 0xd061e0: "The game is still on. Wait until the other players finish.
	// Going back to the lobby now does not raise the forced-quit rate, but
	// the experience, items, records ... earned in the game are all void.
	// Give up the game and go back to the lobby?" (CP949)
	form->SetMessage(Jp_LookupText(
		"\xbe\xc6\xc1\xf7 \xb0\xd4\xc0\xd3\xc0\xcc \xc1\xf8\xc7\xe0 \xc1\xdf \xc0\xd4\xb4\xcf\xb4\xd9.\n"
		"\xb4\xd9\xb8\xa5 \xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xb5\xe9\xc0\xcc \xb8\xf0\xb5\xce \xbf\xcf\xc1\xd6\xc7\xd2 \xb6\xa7\xb1\xee\xc1\xf6\n"
		"\xb1\xe2\xb4\xd9\xb7\xc1 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.\n\n"
		"\xc1\xf6\xb1\xdd \xb7\xce\xba\xf1\xb7\xce \xb3\xaa\xb0\xa1\xb0\xd4 \xb5\xc7\xb8\xe9,\n"
		"\\c0xffff0000\\c\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xc0\xb2\\c0xff000000\\c\xc0\xba \xbf\xc3\xb6\xf3\xb0\xa1\xc1\xf6 \xbe\xca\xc1\xf6\xb8\xb8\n"
		"\xb0\xd4\xc0\xd3 \xc1\xdf \xc8\xb9\xb5\xe6\xc7\xd1 "
		"\\c0xffff0000\\c\xb0\xe6\xc7\xe8\xc4\xa1, \xbe\xc6\xc0\xcc\xc5\xdb, \xb1\xe2\xb7\xcf \xb5\xee\xc0\xcc\n"
		"\xb8\xf0\xb5\xce \xb9\xab\xc8\xbf \xc3\xb3\xb8\xae\\c0xff000000\\c \xb5\xcb\xb4\xcf\xb4\xd9.\n\n"
		"\xb0\xd4\xc0\xd3\xc0\xbb \xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed \xb7\xce\xba\xf1\xb7\xce \xb3\xaa\xb0\xa1\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?"),
		false);
	form->Open((FRESH_RESULT_CALLBACK)&FrUseTikiReportDlg::OnSubDlgResult, 0x801);
}

// 0xafa380
IObject* FrUseTikiReportDlgMakeInstance(void) {
	return new FrUseTikiReportDlg;
}

struct __sFrUseTikiReportDlg {
	__sFrUseTikiReportDlg(void) {
		ObjectFactory().AddObjectFunctor(FrUseTikiReportDlgMakeInstance, "FrUseTikiReportDlg");
	}
};
static __sFrUseTikiReportDlg s_registerFrUseTikiReportDlg;

// 0xab4b30
FrUseTikiReportDlg* CreateUseTikiReportDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrUseTikiReportDlg>(manager, owner, name, parent);
}
