// JP-only game code: FrRoomListSelector, the lobby kind selector
// (JpRoomListSelector.h), and its CreateForm instance.
#include "JpRoomListSelector.h"

#include "JpLobbyGlue.h"
#include "LobbyTask.h"
#include "ObjectFactory.h"
#include "Packet.h"
#include "TaskManager.h"
#include "actor.h"
#include "frarea.h"
#include "frbutton.h"
#include "fresh.h"
#include "frwnd.h"

// lib: 0xcd12b0 (m_RTTI's dynamic initializer, compiler-generated from
// this definition: WRTTI::WRTTI 0xa4c140, base FrForm)
const WRTTI FrRoomListSelector::m_RTTI("FrRoomListSelector", &FrForm::m_RTTI);

// 0x59e440
IObject* FrRoomListSelectorMakeInstance(void) {
	return new FrRoomListSelector;
}

// 0x59e4b0 (static init 0xcd12d0): registration with the form factory
struct __sFrRoomListSelector {
	__sFrRoomListSelector(void) {
		ObjectFactory().AddObjectFunctor(FrRoomListSelectorMakeInstance, "FrRoomListSelector");
	}
};
static __sFrRoomListSelector s_registerFrRoomListSelector;

// 0x59e260
const sFRESH_MSGMAP* FrRoomListSelector::GetMessageMap(void) const {
	return &FrRoomListSelector::_MsgMap;
}

sFRESH_MSGMAP FrRoomListSelector::_MsgMap = { &FrForm::_MsgMap, &FrRoomListSelector::_MsgEntries[0] };

// 0xe0f308 (the handlers are stored by the static init 0xcd12f0)
sFRESH_ENTRY FrRoomListSelector::_MsgEntries[] = {
	{ "normallobby", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRoomListSelector::OnNormalLobbyInit },
	{ "normallobby", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRoomListSelector::OnNormalLobbyClick },
	{ "grandprixlobby", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRoomListSelector::OnGrandPrixLobbyInit },
	{ "grandprixlobby", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRoomListSelector::OnGrandPrixLobbyClick },
	{ "expl", FRCMD_LBUTTONUP, FrFuncVV, (FRESH_PMSG)&FrRoomListSelector::OnExplClick },
	{ "img_multi", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRoomListSelector::OnMultiImgInit },
	{ "img_gp", FRCMD_INIT, FrFuncVI, (FRESH_PMSG)(FRESH_PMSGVI)&FrRoomListSelector::OnGrandPrixImgInit },
	{ 0, FRCMD_NONE, FrFuncNone, (FRESH_PMSG)0 }
};

// 0x59e270
FrRoomListSelector::FrRoomListSelector(void) {
	m_showingExpl = false;
}

// 0x59e2b0
FrRoomListSelector::~FrRoomListSelector(void) {
}

// 0x59e2d0
void FrRoomListSelector::OnExplClick(void) {
	if (m_showingExpl) {
		m_pMultiImg->SetBgImg(g_pFresh->GetBitmap("start_img_multi"));
		m_pGrandPrixImg->SetBgImg(g_pFresh->GetBitmap("start_img_grandprix"));
	} else {
		m_pMultiImg->SetBgImg(g_pFresh->GetBitmap("start_expl_multi"));
		m_pGrandPrixImg->SetBgImg(g_pFresh->GetBitmap("start_expl_grandprix"));
	}
	m_showingExpl = !m_showingExpl;
}

// 0x59e500
void FrRoomListSelector::OnNormalLobbyInit(PG_MSGARG param) {
	m_pNormalLobbyButton = DYNAMIC_CAST(FrButton, param);
}

// 0x59e570
void FrRoomListSelector::OnGrandPrixLobbyInit(PG_MSGARG param) {
	m_pGrandPrixLobbyButton = DYNAMIC_CAST(FrButton, param);
}

// 0x59e5e0
void FrRoomListSelector::OnMultiImgInit(PG_MSGARG param) {
	m_pMultiImg = DYNAMIC_CAST(FrArea, param);
}

// 0x59e650
void FrRoomListSelector::OnGrandPrixImgInit(PG_MSGARG param) {
	m_pGrandPrixImg = DYNAMIC_CAST(FrArea, param);
}

// 0x59e9f0
void FrRoomListSelector::OnNormalLobbyClick(void) {
	if (!JpSharedDoc_GetLobbyFlag()) {
		WSendPacket packet(0x81);
		packet.Send(SEND_TO_DEFAULT);
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
	} else {
		CTask* task = AfxGetTask();
		if (task != 0 && task->IsExactKindOf(&CLobbyTask::m_RTTI)) {
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0, (intptr_t)"TOPPAGE", 0, 0, 0, 0) << MsgObject(0, 0x8b, 4, 0, 0, 0, 0);
		} else {
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
			CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (intptr_t)"TOPPAGE", 0, 0, 0);
			CTaskManager::Instance()->PostMsg(0, "Lobby", 0x8b, 4, 0, 0, 0);
		}
	}
	Close(FrOK, true);
}

// 0x59ebd0: the Grand Prix button's request to the server (packet 0x176),
// then message 1 to the task's main actor. The JP exe calls it as a member
// of an empty stack object (`lea ecx,[ebp-1]`).
static void RequestGrandPrixLobby(void) {
	WSendPacket packet(0x176);
	packet.Send(SEND_TO_DEFAULT);
	AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
}

// 0x59efd0
void FrRoomListSelector::OnGrandPrixLobbyClick(void) {
	if (JpSharedDoc_GetLobbyFlag()) {
		CTask* task = AfxGetTask();
		if (task != 0 && task->IsExactKindOf(&CLobbyTask::m_RTTI)) {
			if (g_pFresh->IsCurrentLayout("TOPPAGE") || JpSharedDoc_GetLobbyMode() == 2) {
				AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0, (intptr_t)"TOPPAGE", 0, 0, 0, 0) << MsgObject(0, 0x8b, 4, 1, 0, 0, 0);
				Close(FrOK, true);
				return;
			}
		} else {
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
		}
	}
	RequestGrandPrixLobby();
	Close(FrOK, true);
}

// 0x654490
FrForm* CreateRoomListSelector(FrWndManager* manager, FrCmdTarget* owner, const char* name, FrWnd* parent) {
	return CreateForm<FrRoomListSelector>(manager, owner, name, parent);
}
