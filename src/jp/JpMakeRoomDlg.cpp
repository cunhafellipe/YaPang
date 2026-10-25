// JP-only game code: FrGrandPrixRoomDlg and FrMultiplayRoomDlg, the lobby's
// GP and multiplayer room list forms. See JpMakeRoomDlg.h.
#include "JpMakeRoomDlg.h"

#include "InputManager.h"
#include "JpLobbyGlue.h"
#include "JpRoomListState.h"
#include "NetResourceManager.h"
#include "SharedDoc.h"
#include "exhibition.h"
#include "globalgamedefine.h"
#include "parttidlist.h"
#include "Packet.h"
#include "TaskManager.h"
#include "actor.h"
#include "ObjectFactory.h"
#include "SoundManager.h"
#include "TitleFontEx.h"
#include "bitmap.h"
#include "commonutil.h"
#include "frbutton.h"
#include "frgraphicinterface.h"
#include "frarea.h"
#include "fresh.h"
#include "frlistbox.h"
#include "frscrollbar.h"
#include "frwnd.h"
#include "frwndmanager.h"

// lib: 0x50f920, 0x50f930, 0x5171c0, 0x5171d0 (this-adjusting
// deleting-destructor thunks), 0xccc100, 0xccc120, 0xccc140, 0xccca20,
// 0xccca40, 0xccca60 (static initializers)

// 0xe74e1c (the instance; its data lies with the GP dialog's RTTI 0xe74e20)
template <> CGrandPrixClock* WSingleton<CGrandPrixClock>::m_pInstance = NULL;

// 0xe09064 (with the GP dialog's message map)
static const char* s_gatewayActorName = "Gateway";

// 0xccc100 (run-time type)
const WRTTI FrGrandPrixRoomDlg::m_RTTI("FrGrandPrixRoomDlg", &FrRoomListDlg::m_RTTI);

// 0xccca20 (run-time type)
const WRTTI FrMultiplayRoomDlg::m_RTTI("FrMultiplayRoomDlg", &FrRoomListDlg::m_RTTI);

// JP Fresh, declared only (as in JpGameRoomDlg.cpp): the JP FrListBox
// fields past the KR layout the room list INIT handlers set inline (dword
// +0xfc = 1, byte +0x114 = 1; roles not identified).
void JpFrListBox_SetRoomUserListFlags(FrListBox* list);

// 0x50f1e0
const sFRESH_MSGMAP* FrGrandPrixRoomDlg::GetMessageMap(void) const {
	return &FrGrandPrixRoomDlg::_MsgMap;
}

sFRESH_MSGMAP FrGrandPrixRoomDlg::_MsgMap = { &FrRoomListDlg::_MsgMap, &FrGrandPrixRoomDlg::_MsgEntries[0] };

#define GPROOM_VI(name, cmd, fn) { name, cmd, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrGrandPrixRoomDlg::fn }
#define GPROOM_VV(name, cmd, fn) { name, cmd, FrFuncVV, (FRESH_PMSG)&FrGrandPrixRoomDlg::fn }

sFRESH_ENTRY FrGrandPrixRoomDlg::_MsgEntries[] = {
	GPROOM_VI("roomlist", FRCMD_INIT, OnRoomListInit),
	GPROOM_VI("roomlist", FRCMD_OWNERDRAW, OnRoomListOwnerDraw),
	GPROOM_VV("roomlist", FRCMD_LBUTTONDOWN, OnRoomListLBtnDown),
	GPROOM_VV("roomlist", FRCMD_RBUTTONUP, OnRoomListRBtnUp),
	GPROOM_VV("roomlist", FRCMD_DBLCLICK, OnRoomListDblClick),
	GPROOM_VI("btn_sort_event", FRCMD_INIT, OnSortEventInit),
	GPROOM_VV("btn_sort_event", FRCMD_LBUTTONUP, OnSortEventLBtnUp),
	GPROOM_VI("btn_sort_rookie", FRCMD_INIT, OnSortRookieInit),
	GPROOM_VV("btn_sort_rookie", FRCMD_LBUTTONUP, OnSortRookieLBtnUp),
	GPROOM_VI("btn_sort_c1", FRCMD_INIT, OnSortC1Init),
	GPROOM_VV("btn_sort_c1", FRCMD_LBUTTONUP, OnSortC1LBtnUp),
	GPROOM_VI("btn_sort_c2", FRCMD_INIT, OnSortC2Init),
	GPROOM_VV("btn_sort_c2", FRCMD_LBUTTONUP, OnSortC2LBtnUp),
	GPROOM_VI("btn_sort_c3", FRCMD_INIT, OnSortC3Init),
	GPROOM_VV("btn_sort_c3", FRCMD_LBUTTONUP, OnSortC3LBtnUp),
	GPROOM_VI("gp_limited_tooltip", FRCMD_INIT, OnGpLimitedTooltipInit),
	GPROOM_VI("gpstate_ticket", FRCMD_INIT, OnGpStateTicketInit),
	GPROOM_VI("gpstate", FRCMD_INIT, OnGpStateInit),
	GPROOM_VI("gpstate", FRCMD_OWNERDRAW, OnGpStateOwnerDraw),
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

#undef GPROOM_VI
#undef GPROOM_VV

// 0x50f1f0
void FrGrandPrixRoomDlg::OnProc(const float dt) {
	FrRoomListDlg::OnProc(dt);
	UpdateRoomList(dt);
}

// 0x50f220
void FrGrandPrixRoomDlg::OnRoomListLBtnDown(void) {
	m_pRoomState->LBtnDown(this);
}

// 0x50f230
void FrGrandPrixRoomDlg::OnRoomListRBtnUp(void) {
	m_pRoomState->RBtnUp(this);
}

// 0x50f240
void FrGrandPrixRoomDlg::OnRoomListDblClick(void) {
	m_pRoomState->DblClick(this);
}

// 0x50f270
void FrGrandPrixRoomDlg::StopLobbyAmbience(void) {
	g_audio->StopSfx("gp_lobby_amb");
	if (WSingleton<CGrandPrixClock>::Instance()) {
		WSingleton<CGrandPrixClock>::Instance()->m_active = 0;
	}
}

// 0x50f2a0
void FrGrandPrixRoomDlg::OnRoomListInit(intptr_t param) {
	m_pAltRoomList = DYNAMIC_CAST(FrListBox, param);
	if (m_pAltRoomList) {
		JpFrListBox_SetRoomUserListFlags(m_pAltRoomList);
	}
}

// 0x50f330
void FrGrandPrixRoomDlg::OnRoomListOwnerDraw(intptr_t param) {
	if (!param) {
		// JP FrWnd m_nFlags +0x1c bit 0 cleared inline: the tool tip.
		m_pAltRoomList->EnableToolTip(false);
		return;
	}
	m_pRoomState->OwnerDraw(param, this);
}

// 0x50f360
void FrGrandPrixRoomDlg::OnSortEventInit(intptr_t param) {
	m_pSortButtons[0] = DYNAMIC_CAST(FrButton, param);
}

// 0x50f3d0
void FrGrandPrixRoomDlg::OnSortRookieInit(intptr_t param) {
	m_pSortButtons[1] = DYNAMIC_CAST(FrButton, param);
}

// 0x50f440
void FrGrandPrixRoomDlg::OnSortC1Init(intptr_t param) {
	m_pSortButtons[2] = DYNAMIC_CAST(FrButton, param);
}

// 0x50f4b0
void FrGrandPrixRoomDlg::OnSortC2Init(intptr_t param) {
	m_pSortButtons[3] = DYNAMIC_CAST(FrButton, param);
}

// 0x50f520
void FrGrandPrixRoomDlg::OnSortC3Init(intptr_t param) {
	m_pSortButtons[4] = DYNAMIC_CAST(FrButton, param);
}

// 0x50f590
void FrGrandPrixRoomDlg::OnGpLimitedTooltipInit(intptr_t param) {
	m_pGpLimitedTooltip = DYNAMIC_CAST(FrArea, param);
}

// 0x50f600
void FrGrandPrixRoomDlg::OnGpStateTicketInit(intptr_t param) {
	m_pGpStateTicket = DYNAMIC_CAST(FrArea, param);
}

// 0x50f670
void FrGrandPrixRoomDlg::OnGpStateInit(intptr_t param) {
	FrArea* area = DYNAMIC_CAST(FrArea, param);
	const WRect& rect = area->GetRect();
	float y = (float)(rect.y + 9.0);
	WSingleton<CGrandPrixClock>::Instance()->m_x = (float)(rect.x + 10.0);
	WSingleton<CGrandPrixClock>::Instance()->m_y = y;
}

// 0x50f700
void FrGrandPrixRoomDlg::OnGpStateOwnerDraw(intptr_t param) {
	CGrandPrixClock* clock = WSingleton<CGrandPrixClock>::Instance();
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	const Bitmap* bitmap = g_pFresh->GetBitmap("gp_txt_clock");
	pGDI->DrawTexture(bitmap, WRect((float)(clock->m_x + 45.0), (float)(clock->m_y - 24.0), bitmap->Width(), bitmap->Height()), 0xffffffff, 0);
	clock->Draw(pGDI);
	m_pTitleFont->Print((float)(clock->m_x + 185.0), (float)(clock->m_y + 5.0), MakeStr(" %d", m_value2f4));
}

// 0x50f8a0
FrGrandPrixRoomDlg::~FrGrandPrixRoomDlg(void) {
	StopLobbyAmbience();
}

// 0x50f940
void FrGrandPrixRoomDlg::SelectSortButton(int sort) {
	for (int i = 0; i < 5; i++) {
		m_pSortButtons[i]->SetStatus(FrButton::NORMAL);
	}
	switch (sort) {
	case 0:
		m_pSortButtons[1]->SetStatus(FrButton::PRESSED);
		break;
	case 1:
		m_pSortButtons[2]->SetStatus(FrButton::PRESSED);
		break;
	case 2:
		m_pSortButtons[3]->SetStatus(FrButton::PRESSED);
		break;
	case 3:
		m_pSortButtons[4]->SetStatus(FrButton::PRESSED);
		break;
	case 0x63:
		m_pSortButtons[0]->SetStatus(FrButton::PRESSED);
		break;
	}
	JpSharedDoc_SetRoomSort(sort);
}

// 0x50fa40
void FrGrandPrixRoomDlg::OnSortEventLBtnUp(void) {
	SelectSortButton(0x63);
	m_pRoomState->Sort(0x63, this);
}

// 0x50fa60
void FrGrandPrixRoomDlg::OnSortRookieLBtnUp(void) {
	SelectSortButton(0);
	m_pRoomState->Sort(0, this);
}

// 0x50fa80
void FrGrandPrixRoomDlg::OnSortC1LBtnUp(void) {
	SelectSortButton(1);
	m_pRoomState->Sort(1, this);
}

// 0x50faa0
void FrGrandPrixRoomDlg::OnSortC2LBtnUp(void) {
	SelectSortButton(2);
	m_pRoomState->Sort(2, this);
}

// 0x50fac0
void FrGrandPrixRoomDlg::OnSortC3LBtnUp(void) {
	SelectSortButton(3);
	m_pRoomState->Sort(3, this);
}

// 0x50fb20
void FrGrandPrixRoomDlg::SetupGpRoomList(void) {
	WSendPacket packet(0x5c);
	packet.Send(SEND_TO_DEFAULT);
	int sort = JpSharedDoc_GetRoomSort();
	m_pRoomState->Sort(sort, this);
	SelectSortButton(sort);
	m_value2f4 = 0;
	AfxGetTask()->PassMsgToActor(s_gatewayActorName, MsgObject(0, 0x753E, 4, 0x1A000264, (intptr_t)&m_value2f4, 0, 0));
	WSingleton<CGrandPrixClock>::Instance()->m_active = 1;
	g_audio->PlaySfx("gp_lobby_amb", NULL, 1, "gp_lobby_amb", NULL, 0.5f, 200.0f);
}

// 0x50fc80
bool FrGrandPrixRoomDlg::OnInit(void) {
	if (!FrRoomListDlg::OnInit()) {
		return false;
	}
	m_pRoomState->m_kind = 1;
	JpSharedDoc_SetCurrentRoomTab(1);
	m_pRoomState->Attach(this);
	SetupGpRoomList();
	if (m_pGpRoomTab) {
		m_pGpRoomTab->SetStatus(FrButton::PRESSED);
	}
	if (m_pUserList) {
		WRect rect = m_pUserList->GetRect();
		rect.h = (float)(rect.h - 80.0);
		m_pUserList->SetRect(rect);
	}
	return true;
}

// The room slots and no-manner parts reset both room-state functions run
// (inlined in both, name ours; the KR CLobbyMain::OnRoomList_Init body):
// deletes and clears every room slot's pet (JP CSharedDoc +0x3894 = the KR
// m_roomSlotMap), raises NetResourceManager's clear-garbage flag (JP +0x32f4)
// when asked, then, when the player's no-manner games reach 3% (JP +0x98c /
// +0x960), takes the restricted parts (0x4ce760) off the equipped
// character and its part list.
static void JpRoomList_ResetRoomSlots(bool clearGarbage) {
	std::map<DWORD, sRoomSlot>::iterator it;
	for (it = CSharedDoc::Instance()->m_roomSlotMap.begin(); it != CSharedDoc::Instance()->m_roomSlotMap.end(); ++it) {
		if (it->second.m_pPet) {
			delete it->second.m_pPet;
			it->second.m_pPet = NULL;
		}
	}
	CSharedDoc::Instance()->m_roomSlotMap.clear();
	if (clearGarbage) {
		WSingleton<NetResourceManager>::Instance()->SetClearGarbage();
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	uint32_t games = doc->m_myStatGamesPlayed;
	if (games < 1) {
		games = 1;
	}
	if (!((float)doc->m_myStatNoMannerGames / (float)games * 100.0f < 3.0f)) {
		CPartTidList& partTidList = doc->GetMyPartTidList();
		sCharacterInfo& info = WSingleton<CSharedDoc>::Instance()->m_characterMap[WSingleton<CSharedDoc>::Instance()->m_myCharacterId];
		for (int i = 0; i < partTidList.m_numTotalParts; i++) {
			if (Jp_IsNoMannerRestrictedPart(info.partTypeIds[i])) {
				info.partTypeIds[i] = 0;
				info.partIds[i] = 0;
			}
			if (Jp_IsNoMannerRestrictedPart(partTidList.m_partTids[i])) {
				partTidList.m_partTids[i] = 0;
			}
		}
	}
}

// 0x50fd90
void FrGrandPrixRoomDlg::InitGpRoomState(void) {
	JpSharedDoc_SetRoomGameType(0x17);
	JpSharedDoc_SetRoomGameMode(0x17);
	LoadUserListImages();
	JpRoomList_ResetRoomSlots(true);
}

// 0x50ff90
FrGrandPrixRoomDlg::FrGrandPrixRoomDlg(void) {
	m_pGpLimitedTooltip = NULL;
	m_pGpStateTicket = NULL;
	InitGpRoomState();
}

// 0x510000
IObject* FrGrandPrixRoomDlgMakeInstance(void) {
	return new FrGrandPrixRoomDlg;
}

struct __sFrGrandPrixRoomDlg {
	__sFrGrandPrixRoomDlg(void) {
		ObjectFactory().AddObjectFunctor(FrGrandPrixRoomDlgMakeInstance, "FrGrandPrixRoomDlg");
	}
};
static __sFrGrandPrixRoomDlg s_registerFrGrandPrixRoomDlg;

// 0x517140
const sFRESH_MSGMAP* FrMultiplayRoomDlg::GetMessageMap(void) const {
	return &FrMultiplayRoomDlg::_MsgMap;
}

sFRESH_MSGMAP FrMultiplayRoomDlg::_MsgMap = { &FrRoomListDlg::_MsgMap, &FrMultiplayRoomDlg::_MsgEntries[0] };

#define MTROOM_VI(name, cmd, fn) { name, cmd, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrMultiplayRoomDlg::fn }
#define MTROOM_VV(name, cmd, fn) { name, cmd, FrFuncVV, (FRESH_PMSG)&FrMultiplayRoomDlg::fn }

sFRESH_ENTRY FrMultiplayRoomDlg::_MsgEntries[] = {
	MTROOM_VI("roomlist", FRCMD_INIT, OnRoomListInit),
	MTROOM_VI("roomlist", FRCMD_OWNERDRAW, OnRoomListOwnerDraw),
	MTROOM_VV("roomlist", FRCMD_LBUTTONDOWN, OnRoomListLBtnDown),
	MTROOM_VV("roomlist", FRCMD_RBUTTONUP, OnRoomListRBtnUp),
	MTROOM_VV("roomlist", FRCMD_DBLCLICK, OnRoomListDblClick),
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

#undef MTROOM_VI
#undef MTROOM_VV

// 0x517150
FrMultiplayRoomDlg::~FrMultiplayRoomDlg(void) {
}

// 0x517180
void FrMultiplayRoomDlg::RefreshRoomList(void) {
	ReloadRoomList();
}

// 0x517190
void FrMultiplayRoomDlg::OnRoomListLBtnDown(void) {
	m_pRoomState->LBtnDown(this);
}

// 0x5171a0
void FrMultiplayRoomDlg::OnRoomListRBtnUp(void) {
	m_pRoomState->RBtnUp(this);
}

// 0x5171b0
void FrMultiplayRoomDlg::OnRoomListDblClick(void) {
	m_pRoomState->DblClick(this);
}

// 0x517210
void FrMultiplayRoomDlg::OnProc(const float dt) {
	FrRoomListDlg::OnProc(dt);
	if (m_pRoomList && !m_value14c) {
		// 0xd075c0 / 0xd075bc: CP949 key names (up, down)
		if (g_input->GetDown("\xbb\xf3", false)) {
			m_pRoomList->GetScrollBar()->ScrollUp(1);
		}
		if (g_input->GetDown("\xc7\xcf", false)) {
			m_pRoomList->GetScrollBar()->ScrollDown(1);
		}
	}
	UpdateRoomList(dt);
}

// 0x5172a0
void FrMultiplayRoomDlg::OnRoomListInit(intptr_t param) {
	m_pRoomList = DYNAMIC_CAST(FrListBox, param);
	if (m_pRoomList) {
		JpFrListBox_SetRoomUserListFlags(m_pRoomList);
	}
}

// 0x517330
void FrMultiplayRoomDlg::OnRoomListOwnerDraw(intptr_t param) {
	if (!param) {
		// JP FrWnd m_nFlags +0x1c bit 0 cleared inline: the tool tip.
		m_pRoomList->EnableToolTip(false);
		return;
	}
	m_pRoomState->OwnerDraw(param, this);
}

// 0x517360
bool FrMultiplayRoomDlg::OnInit(void) {
	if (!FrRoomListDlg::OnInit()) {
		return false;
	}
	m_pRoomState->m_kind = 0;
	JpSharedDoc_SetCurrentRoomTab(0);
	m_pRoomState->Attach(this);
	RefreshRoomList();
	PadRoomList(0);
	if (m_pMtRoomTab) {
		m_pMtRoomTab->SetStatus(FrButton::PRESSED);
	}
	return true;
}

// 0x5173c0
void FrMultiplayRoomDlg::ResetRoomState(void) {
	JpSharedDoc_SetRoomGameType(0x17);
	JpSharedDoc_SetRoomGameMode(0x17);
	ResetRoomListImages();
	JpRoomList_ResetRoomSlots(IsLocalContent((localContentType_t)0x58) != 0);
	m_value2ec = 6;
	m_flag2e9 = false;
}

// 0x5175f0
FrMultiplayRoomDlg::FrMultiplayRoomDlg(void) {
	ResetRoomState();
}

// 0x517660
IObject* FrMultiplayRoomDlgMakeInstance(void) {
	return new FrMultiplayRoomDlg;
}

struct __sFrMultiplayRoomDlg {
	__sFrMultiplayRoomDlg(void) {
		ObjectFactory().AddObjectFunctor(FrMultiplayRoomDlgMakeInstance, "FrMultiplayRoomDlg");
	}
};
static __sFrMultiplayRoomDlg s_registerFrMultiplayRoomDlg;

// 0x55cd50 (the original formats into a zeroed 4-byte buffer with
// sprintf_s(buf, 4, "%d", value); MakeStr gives the same digits for the
// clock's values)
void JpFormatTwoDigits(unsigned short value, char* out) {
	const char* text = MakeStr("%d", value);
	if (value < 10) {
		out[0] = '0';
		out[1] = text[0];
		return;
	}
	out[0] = text[0];
	out[1] = text[1];
}

// 0xe7516c: Draw's text buffer (zero-initialized; only its first two
// bytes are ever written).
static char s_clockText[4];

// 0x55d2e0
void CGrandPrixClock::Draw(FrGraphicInterface* pGDI) {
	if (!m_active) {
		return;
	}
	if (m_pFont) {
		JpFormatTwoDigits(m_leftNumber, s_clockText);
		m_pFont->Print(m_x, m_y, s_clockText, TITLEFONT_ALIGN_LEFT, 0.0f, 1.0f);
		JpFormatTwoDigits(m_rightNumber, s_clockText);
		m_pFont->Print((float)(m_x + 82.0), m_y, s_clockText, TITLEFONT_ALIGN_LEFT, 0.0f, 1.0f);
	}
	const Bitmap* image = m_flag44 ? m_pSetImage : m_pUnsetImage;
	pGDI->DrawTexture(image, WRect(m_x + 59.0, m_y - 1.0, (float)image->Width(), (float)image->Height()), 0xFFFFFFFF, 0);
	if (m_blinkCounter % 2 == 0) {
		pGDI->DrawTexture(m_pBlinkImage, WRect(m_x + 64.0, m_y + 14.0, (float)m_pBlinkImage->Width(), (float)m_pBlinkImage->Height()), 0xFFFFFFFF, 0);
	}
}

// 0xab4990
FrMultiplayRoomDlg* CreateMultiplayRoomDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrMultiplayRoomDlg>(manager, owner, name, parent);
}

// 0xab4a60
FrGrandPrixRoomDlg* CreateGrandPrixRoomDlg(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrGrandPrixRoomDlg>(manager, owner, name, parent);
}
