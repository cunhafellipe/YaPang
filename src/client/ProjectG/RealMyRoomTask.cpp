// Original: d:\build\custom\temp\code\client\projectg\realmyroomtask.cpp
#include "NetworkSystem.h"
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer

#include "guilddefine.h"
#include "RealMyRoomTask.h"

#include "ObjectFactory.h"
#include "actor.h"
#include "fresh.h"
#include "Packet.h"
#include "frdesktop.h"
#include "frwndmanager.h"
#include "partsinfo.h"
#include "frwnd.h"
#include "wview.h"
#include "QuitWindow.h"
#include "avatar_task.h"
#include "SharedDoc.h"
#include "ClientSetting.h"
#include "RealMyRoomData.h"
#include "camerawork.h"
#include "TransferableItem.h"
#include "ItemStorage.h"
#include "Localize.h"
#include "mousecursor.h"
#include "frgaugebar.h"
#include "UccLibrary.h"
#include "commonutil.h"
#include "PangGiftDlg.h"

// Class and actor name pairs of the task's extra actors (unnamed data;
// pairs at 0x0 and 0xC).
extern "C" const char *data_a8d820[5];

// The loading parameters CTask::Load hands to the loading thread (layout in
// taskmanager.cpp, where it is defined; its header is not known yet, the
// same local copy as Avatar_Task.cpp's).
struct sLoadParam {
	WList<sActor *> *m_pActorList; // 0x0
	int m_actorCount; // 0x4
	int m_progress;   // 0x8
	bool m_bInited;   // 0xC
	bool m_bLoaded;   // 0xD
};
extern sLoadParam g_LoadParam;

// realmyroomtask.cpp:55 (run-time type and factory registration of CRealMyRoomTask)
const WRTTI CRealMyRoomTask::m_RTTI("CRealMyRoomTask", &CTask::m_RTTI);

IObject* CRealMyRoomTaskMakeInstance(void) {
	return new CRealMyRoomTask;
}

struct __sCRealMyRoomTask {
	__sCRealMyRoomTask(void) {
		ObjectFactory().AddObjectFunctor(CRealMyRoomTaskMakeInstance, "CRealMyRoomTask");
	}
};
static __sCRealMyRoomTask s_registerCRealMyRoomTask;

// realmyroomtask.cpp:58
CRealMyRoomTask::CRealMyRoomTask(void) {
	m_bDestroying = false;
	InitializeCriticalSection(&m_burdenLock);

	new CRealMyRoom_AvatarManager;
	new CRealMyRoomData;

	new CCameraWork;

	new CTransferableItem;

	if (IsLocalContent((localContentType_t)0x71)) {
		new CItemStorage;
	}

	IActor::m_defaultSkipLayer = 0;
	IActor::m_pLayer = 0;
}

// realmyroomtask.cpp:81
CRealMyRoomTask::~CRealMyRoomTask(void) {
	COption::Instance()->gSetAvatarNewBie(0);

	m_bDestroying = true;
	m_anotherHand.WaitUntilWorkDone(9999);
	m_anotherHand.BreakWorkingThread();

	DeleteCriticalSection(&m_burdenLock);

	if (CRealMyRoom_AvatarManager::IsInstantiated()) delete CRealMyRoom_AvatarManager::Instance();
	if (CRealMyRoomData::IsInstantiated()) delete CRealMyRoomData::Instance();
	if (CCameraWork::IsInstantiated()) delete CCameraWork::Instance();
	if (CTransferableItem::IsInstantiated()) delete CTransferableItem::Instance();

	if (IsLocalContent((localContentType_t)0x71)) {
		if (CItemStorage::IsInstantiated()) delete CItemStorage::Instance();
	}
}

// realmyroomtask.cpp:108
void CRealMyRoomTask::Register(void) {
	SetMainActor(AddActor("CRealMyRoomMain", "RealMyRoom", 0, ""));

	AddActor("CCamera", "Camera", 0x20, "");

	AddActor("CItemBuff", "ItemBuff", 0, "");

	AddActor(data_a8d820[0], data_a8d820[1], 0, "");

	AddActor(data_a8d820[3], data_a8d820[4], 0, GetRTTI()->GetName());

	if (AfxGetPreservedTask()) {
		IActor *pTutorial = AfxGetPreservedTask()->GetActor("TutorialMain");
		if (pTutorial) {
			const char *className = 0;
			const char *name = 0;

			pTutorial << MsgObject(0, 0x14E, (intptr_t)&className, (intptr_t)&name, 0, 0, 0);
			AddActor(className, name, 0, "");
		}
	}
}

// realmyroomtask.cpp:139
void CRealMyRoomTask::PreLoadInit(void) {
	sActor *pActor = m_actorList.Start();
	while (pActor) {

		pActor->m_pActor->OnPreLoadInit();
		pActor = m_actorList.Next();
	}

	if (!((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false))
		new CQuitWindow;

	SetGameType(8);

	pActor = m_actorList.Start();
	while (pActor) {

		pActor->m_pActor->OnLoad();
		pActor = m_actorList.Next();
	}

	pActor = m_actorList.Start();
	while (pActor) {

		pActor->m_pActor->OnInit();
		pActor = m_actorList.Next();
	}

	if (!IsInited()) {
		m_stateFlags.Enable(2);
		OnInitFinished();
	}

	g_LoadParam.m_actorCount = 1;
	g_LoadParam.m_progress = 0;

	if (m_tipDlgState) {
		m_tipDlgState->SetRange(0, 100, 0);
	}

	m_pLoadingFresh = new Fresh;
	m_tipDlgState = NULL;

	if (m_loadingBackGround != "")
		m_pLoadingFresh->Init("loading.xml", false, m_loadingBackGround.c_str());
	else
		m_pLoadingFresh->Init("loading.xml", false, NULL);

	m_pLoadingFresh->OpenLayout("LOADING", NULL, true);

	if (CMouseCursor::Instance()) {
		CMouseCursor::Instance()->SetActive(true);
		CMouseCursor::Instance()->SetMode(2);
	}
}

// realmyroomtask.cpp:210
void CRealMyRoomTask::Load(void) {
	m_anotherHand.Work(this, 0, 0, false, 0);

	g_LoadParam.m_progress = 1;
	g_LoadParam.m_bLoaded = true;
}

// realmyroomtask.cpp:228
void CRealMyRoomTask::Init(const char *backGround) {
	if (m_pLoadingFresh) { delete m_pLoadingFresh; m_pLoadingFresh = 0; }

	g_pFresh->GetDesktop()->Init(g_view->GetWidth(), g_view->GetHeight());

	g_pFresh->GetDesktop()->SetWallPaper(backGround, true);

	SetWhisper(false);
}

// realmyroomtask.cpp:247
void CRealMyRoomTask::OnAnotherHand(int nWork, void *pData) {
	std::list<CRealMyRoom_AvatarBurden> burdens;

	while (!m_bDestroying) {
		std::list<CRealMyRoom_AvatarBurden>::iterator it;

		EnterCriticalSection(&m_burdenLock);
		for (it = m_addedBurdens.begin(); it != m_addedBurdens.end(); ++it)
			burdens.push_back(*it);
		m_addedBurdens.clear();

		LeaveCriticalSection(&m_burdenLock);

		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
			// The local player's burdens load first.
			it = burdens.begin();
			while (!m_bDestroying && it != burdens.end()) {
				if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar == CRealMyRoom_AvatarManager::Instance()->FindAvatarByGuid(it->m_guid)) {
					if (it->LoadAvatarBurden()) {
						EnterCriticalSection(&m_burdenLock);
						m_avatarBurdens.push_back(*it);
						LeaveCriticalSection(&m_burdenLock);
					}

					it = burdens.erase(it);
				} else {
					++it;
				}
			}
		}

		it = burdens.begin();
		while (!m_bDestroying && it != burdens.end()) {
			if (it->LoadAvatarBurden()) {
				EnterCriticalSection(&m_burdenLock);
				m_avatarBurdens.push_back(*it);
				LeaveCriticalSection(&m_burdenLock);

				if (!m_bDestroying)
					Sleep(15);
			}

			it = burdens.erase(it);
		}

		DeleteBurdenPet(burdens, m_removedBurdens);

		if (!m_bDestroying)
			Sleep(500);
	}

	DeleteBurdenPet(burdens, m_removedBurdens);
	DeleteBurdenPet(burdens, m_avatarBurdens);
}

// realmyroomtask.cpp:345
void CRealMyRoomTask::Process(float dt) {
	std::list<CRealMyRoom_AvatarBurden> burdens;
	std::list<CRealMyRoom_AvatarBurden>::iterator it;

	EnterCriticalSection(&m_burdenLock);
	for (it = m_avatarBurdens.begin(); it != m_avatarBurdens.end(); ++it) {
		burdens.push_back(*it);
	}
	m_avatarBurdens.clear();
	LeaveCriticalSection(&m_burdenLock);

	for (it = burdens.begin(); it != burdens.end(); ++it) {
		CRealMyRoom_AvatarBurden &burden = *it;
		CRealMyRoom_Avatar *pAvatar = CRealMyRoom_AvatarManager::Instance()->FindAvatarByGuid(burden.m_guid);
		if (pAvatar) {
			if (pAvatar->m_pPetFrame) {
				pAvatar->m_pPetFrame = NULL;
				RemoveAvatarBurden(pAvatar->m_burden);
			}

			pAvatar->m_burden = burden;
			pAvatar->m_burdenChanged = true;
		} else {
			RemoveAvatarBurden(burden);
		}
	}

	CTask::Process(dt);

	g_pFresh->OnProcess(dt);

	FrWnd *pFocused = g_pFresh->GetManager()->GetKeyFocused();
	if (pFocused && pFocused->IsVisible()) {
		g_pFresh->GetManager()->SetExclusiveKey(true);
	} else {
		g_pFresh->GetManager()->SetExclusiveKey(false);
	}
}

// realmyroomtask.cpp:398
void CRealMyRoomTask::RestorePreserved(void) {
	g_pFresh->GetDesktop()->SetViewFocus(true);

	SetGameType(2);

	g_pFresh->GetDesktop()->Init(g_view->GetWidth(), g_view->GetHeight());
	g_pFresh->GetDesktop()->SetWallPaper(NULL, true);

	CTask::RestorePreserved();
}

// realmyroomtask.cpp:412
void CRealMyRoomTask::Destroy(void) {
	CTask::Destroy();

	if (!((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false))
		delete CQuitWindow::Instance();
}

// realmyroomtask.cpp:422
void CRealMyRoomTask::Display(void) {
	CTask::Display();

	if (g_pFresh->GetDesktop()) {
		g_pFresh->GetDesktop()->OnDisplay(true, false, true);
	}

	g_pFresh->OnDisplay(true);
}

// realmyroomtask.cpp:434
void CRealMyRoomTask::AddAvatarBurden(const CRealMyRoom_AvatarBurden &burden) {
	EnterCriticalSection(&m_burdenLock);
	m_addedBurdens.push_back(burden);
	LeaveCriticalSection(&m_burdenLock);
}

// realmyroomtask.cpp:442
void CRealMyRoomTask::RemoveAvatarBurden(const CRealMyRoom_AvatarBurden &burden) {
	if (m_bDestroying) {
		return;
	}

	EnterCriticalSection(&m_burdenLock);
	m_removedBurdens.push_back(burden);
	LeaveCriticalSection(&m_burdenLock);
}

// realmyroomtask.cpp:455
void CRealMyRoomTask::DeleteBurdenPet(std::list<CRealMyRoom_AvatarBurden> &burdens, std::list<CRealMyRoom_AvatarBurden> &pending) {
	std::list<CRealMyRoom_AvatarBurden>::iterator it;

	EnterCriticalSection(&m_burdenLock);
	for (it = pending.begin(); it != pending.end(); ++it) {
		burdens.push_back(*it);
	}
	pending.clear();
	LeaveCriticalSection(&m_burdenLock);

	for (it = burdens.begin(); it != burdens.end(); ) {
		if (it->m_pPetFrame) {
			delete it->m_pPetFrame;
		}
		it = burdens.erase(it);
	}
}

// The packet id of the packet being handled (defined in taskmanager.cpp).
extern int g_iRecvPacketType;

// wtemplate.h:409 (not yet in src/client/ProjectG/wtemplate.h, which this
// unit does not own; the same local copy as PrivateTradeDlg.cpp's): finds a
// list element by its serial.
struct _FindByGuid {
	unsigned long m_guid;

	_FindByGuid(unsigned long guid) : m_guid(guid) {}
	template <class T>
	bool operator()(const T& item) const { return m_guid == item.m_serial; }
};

// The reply of packet 0xA4 (original type not known; 0x18 bytes as the
// handler decodes it).
struct sPangCashResult {
	__int64 pang;          // 0x00, added to the player's pang
	__int64 bonusCash;     // 0x08, added to the player's bonus cash
	unsigned long result;  // 0x10, sent to the RealMyRoom actor (0xD1)
	unsigned long reserved14; // 0x14
};
CHECK_SIZE(sPangCashResult, 0x18);

// realmyroomtask.cpp:475
int CRealMyRoomTask::OnPacket(WReceivedPacket &packet) {
	g_iRecvPacketType = packet.Decode2();

	switch (g_iRecvPacketType) {
	case 0x16E: {
		static sSlotInfo slot;
		static sCharacterInfo charInfo;

		std::list<sSlotInfo> &players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;

		packet.DecodeBuffer(&slot, 0x152);
		packet.DecodeBuffer(&charInfo, 0x1BC);

		if (!((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false)) {
			std::list<sSlotInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end(); ++it) {
				if (sSlotInfo(*it).m_uid == slot.m_uid) break;
			}

			if (it == WSingleton<CSharedDoc>::Instance()->m_roomPlayerList.end())
				players.push_back(slot);
		}

		GetActor("RealMyRoom") << MsgObject(0, 0xE4, (intptr_t)&slot, (intptr_t)&charInfo, 0, 0, 0);
		break;
	}

	case 0xB6: {
		unsigned char count = packet.Decode1();
		if (count >= 0x80) {
			count = 0x80;
		}

		unsigned long ids[0x80];
		for (unsigned char i = 0; i < count; i++) {
			ids[i] = packet.Decode4();
		}
		GetActor("RealMyRoom") << MsgObject(0, 0xD5, (intptr_t)ids, count, 0, 0, 0);
		break;
	}

	case 0x13D:
		if (IsLocalContent((localContentType_t)0x5B)) {
			unsigned char value5055 = packet.Decode1();
			unsigned char value5056 = packet.Decode1();
			unsigned short count = packet.Decode2();

			for (unsigned short i = 0; i < count; i++) {
				sGiftInfo gift;

				packet.DecodeBuffer(&gift, 0xFB);

				WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(gift.m_message, false);
				WSingleton<CSharedDoc>::Instance()->m_giftList.push_back(gift);
			}

			WSingleton<CSharedDoc>::Instance()->m_value5055 = value5055;
			WSingleton<CSharedDoc>::Instance()->m_value5056 = value5056;
			WSingleton<CSharedDoc>::Instance()->m_value5058 = count;
			WSingleton<CSharedDoc>::Instance()->m_value505A = 1;
		}
		break;

	case 0x78: {
		unsigned char result = packet.Decode1();
		unsigned short pageSize = packet.Decode2();
		unsigned short count = packet.Decode2();
		unsigned short total = packet.Decode2();

		if (result == 1) {
			WSingleton<CSharedDoc>::Instance()->m_value5044 = pageSize;
			WSingleton<CSharedDoc>::Instance()->m_value5048 = count;
			WSingleton<CSharedDoc>::Instance()->m_value504C = total;

			WSingleton<CSharedDoc>::Instance()->m_giftList.clear();

			for (unsigned short i = 0; i < count; i++) {
				sGiftInfo gift;

				packet.DecodeBuffer(&gift, 0xFB);

				WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(gift.m_message, false);
				WSingleton<CSharedDoc>::Instance()->m_giftList.push_back(gift);
			}

			WSingleton<CSharedDoc>::Instance()->m_value5054 = 1;
		} else {
			int pages = (int)total / (int)pageSize;
			if ((int)pageSize * pages < (int)total) {
				pages++;
			}

			if (pages) {
				WSendPacket request((enumClientPacket)0x92);
				request.Encode4(pages);
				request.Send(SEND_TO_DEFAULT);
			} else {
				WSendPacket request((enumClientPacket)0x92);
				request.Encode4(1);
				request.Send(SEND_TO_DEFAULT);
			}
		}
		break;
	}

	case 0xDA: {
		unsigned long result = packet.Decode4();

		GetActor("RealMyRoom") << MsgObject(0, 0xD1, result, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0xD1, result, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		break;
	}

	case 0x79: {
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0xC7, 0, 0, 0, 0, 0);

		unsigned long status = packet.Decode4();

		switch (status) {
		case 1:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xBC\xB1\xB9\xB0 \xBF\xC5\xB1\xE2\xB1\xE2\xB0\xA1 \xBD\xC7\xC6\xD0\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 0: {
			unsigned long result = packet.Decode4();
			unsigned long guid = packet.Decode4();
			unsigned long typeId = packet.Decode4();

			unsigned short period = packet.Decode2();

			unsigned char flag = packet.Decode1();

			int msg;
			switch (typeId >> 26) {
			case 1: msg = 0xC8; break;
			case 2: msg = 0xCA; break;
			case 7: msg = 0xCB; break;
			case 8:
				period *= 24;
				msg = 0xCC;
				break;
			case 4: msg = 0xCF; break;
			case 14: msg = 0xD0; break;
			case 5: case 6: case 28: case 29: msg = 0xCE; break;
			case 15: msg = 0xC9; break;
			case 16: msg = 0xCD; break;
			case 31: msg = 0xE0; break;
			}

			GetActor("RealMyRoom") << MsgObject(0, msg, guid, typeId, period, flag, 0);
			GetActor("RealMyRoom") << MsgObject(0, 0xD1, result, 0, 0, 0, 0);
			break;
		}

		case 3:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xC0\xDF\xB8\xF8\xB5\xC8 \xBB\xF3\xC7\xB0 \xC4\xDA\xB5\xE5\xC0\xD4\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 4:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xC0\xCC\xB9\xCC \xB0\xA1\xC1\xF6\xB0\xED \xC0\xD6\xB4\xC2 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xD4\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 12:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xC7\xD8\xB4\xE7 \xC4\xB3\xB5\xF0\xB0\xA1 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 14:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xC7\xD8\xB4\xE7 \xC4\xB3\xB8\xAF\xC5\xCD\xB0\xA1 \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 13:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xC8\xDE\xB0\xA1\xC1\xDF\xC0\xCE \xC4\xB3\xB5\xF0\xC0\xD4\xB4\xCF\xB4\xD9. \xC0\xE7\xB0\xED\xBF\xEB \xC8\xC4 \xBC\xB1\xB9\xB0 \xBF\xC5\xB1\xE2\xB1\xE2\xB8\xA6 \xC7\xD8\xC1\xD6\xBC\xBC\xBF\xE4", 0, 0, 0, 0);
			break;

		case 5: {
			unsigned long result = packet.Decode4();
			unsigned char count = packet.Decode1();
			sBuyItemResult item;

			for (int i = 0; i < count; i++) {
				packet.DecodeBuffer(&item, 0x26);

				int msg;
				switch (item.typeId >> 26) {
				case 1: msg = 0xC8; break;
				case 2: msg = 0xCA; break;
				case 7: msg = 0xCB;
					break;
				case 4: msg = 0xCF;
					break;
				case 14: msg = 0xD0; break;
				case 5: case 6: case 28: msg = 0xCE; break;
				}

				GetActor("RealMyRoom") << MsgObject(0, msg, item.guid, item.typeId, 0, 0, 0);
			}

			GetActor("RealMyRoom") << MsgObject(0, 0xD1, result, 0, 0, 0, 0);
			break;
		}

		case 6:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xBC\xB1\xB9\xB0\xC0\xCC \xC1\xB8\xC0\xE7\xC7\xCF\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 16:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xB1\xE2\xB0\xA3\xC1\xA6 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xBB\xE7\xBF\xEB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 17:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xB0\xB9\xBC\xF6\xC1\xA6 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xBB\xE7\xBF\xEB\xC1\xDF\xC0\xD4\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;
		}
		break;
	}

	case 0x67: {
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		unsigned char result = packet.Decode1();
		switch (result) {
		case 5:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xB9\xB0\xB0\xC7\xC0\xBB \xC8\xAF\xBA\xD2\xB9\xDE\xC1\xF6 \xB8\xF8\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		default:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xC8\xAF\xBA\xD2\xC0\xCC \xBF\xCF\xB7\xE1\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;
		}

		unsigned char count = packet.Decode1();
		for (int i = 0; i < count; i++) {
			sBuyItemResult item;
			packet.DecodeBuffer(&item, 0x26);

			int msg;
			switch (item.typeId >> 26) {
			case 1: msg = 0xD6; break;
			case 2: msg = 0xD7; break;
			case 7: msg = 0xD8; break;
			case 6: msg = 0xD9; break;
			case 4: msg = 0xDA; break;
			case 5: msg = 0xDB; break;
			case 14: msg = 0xDC; break;
			default: continue;
			}

			GetActor("RealMyRoom") << MsgObject(0, msg, item.guid, item.typeId, 0, 0, 0);
		}

		WSingleton<CSharedDoc>::Instance()->m_myPang = packet.Decode8();

		WSingleton<CSharedDoc>::Instance()->m_myBonusCash = packet.Decode8();
		WSingleton<CSharedDoc>::Instance()->RefreshAllCookie();
		break;
	}

	case 0xA4: {
		sPangCashResult reply;
		packet.DecodeBuffer(&reply, 0x18);

		WSingleton<CSharedDoc>::Instance()->m_myPang += reply.pang;

		WSingleton<CSharedDoc>::Instance()->m_myBonusCash += reply.bonusCash;
		WSingleton<CSharedDoc>::Instance()->RefreshAllCookie();

		GetActor("RealMyRoom") << MsgObject(0, 0xD1, reply.result, 0, 0, 0, 0);
		break;
	}

	// A stat upgrade (1) or downgrade (2) of a club set or a character
	// (the answer of 0x4B): u8 type (bit 0: a club set), u8 stat, u32 id,
	// then the i64 pang it cost (both directions).
	case 0xA3: {
		int result = packet.Decode1();

		switch (result) {
		case 1: {
			unsigned char type = packet.Decode1();
			unsigned char stat = packet.Decode1();
			unsigned long id = packet.Decode4();

			CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
			if (type & 1) {
				std::map<unsigned int, sItemInfo>::iterator it = pDoc->m_myItemMap.find(id);
				if (it != pDoc->m_myItemMap.end())
					it->second.m_clubStatLevel[stat]++;
			} else {
				std::map<unsigned int, sCharacterInfo>::iterator it = pDoc->m_characterMap.find(id);
				if (it != pDoc->m_characterMap.end())
					it->second.unknown6A.level[stat]++;
			}

			__int64 pang = packet.Decode8();
			WSingleton<CSharedDoc>::Instance()->m_myPang -= pang;
			break;
		}

		case 2: {
			unsigned char type = packet.Decode1();
			unsigned char stat = packet.Decode1();
			unsigned long id = packet.Decode4();

			CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
			if (type & 1) {
				std::map<unsigned int, sItemInfo>::iterator it = pDoc->m_myItemMap.find(id);
				if (it != pDoc->m_myItemMap.end())
					it->second.m_clubStatLevel[stat]--;
			} else {
				std::map<unsigned int, sCharacterInfo>::iterator it = pDoc->m_characterMap.find(id);
				if (it != pDoc->m_characterMap.end())
					it->second.unknown6A.level[stat]--;
			}

			__int64 pang = packet.Decode8();
			WSingleton<CSharedDoc>::Instance()->m_myPang -= pang;
			break;
		}
		}

		GetActor("RealMyRoom") << MsgObject(0, 0xD2, result, 0, 0, 0, 0);
		break;
	}

	case 0xCB: {
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		unsigned char result = packet.Decode1();

		switch (result) {
		case 0: {
			unsigned long first = packet.Decode4();
			unsigned long second = packet.Decode4();
			unsigned short third = packet.Decode2();

			GetActor("RealMyRoom") << MsgObject(0, 0xDE, second, first, third, 0, 0);
			break;
		}

		case 1:
		case 2:
		case 3:
		case 4:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xC1\xB6\xC7\xD5\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			break;

		case 5:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xC3\xE0\xC7\xCF\xC7\xD5\xB4\xCF\xB4\xD9. \xC0\xCC\xBA\xA5\xC6\xAE\xBF\xA1 \xB4\xE7\xC3\xB7\xB5\xC7\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9. \xC0\xDA\xBC\xBC\xC7\xD1 \xBB\xE7\xC7\xD7\xC0\xBA \xC8\xA8\xC6\xE4\xC0\xCC\xC1\xF6\xB8\xA6 \xC2\xFC\xC1\xB6\xC7\xCF\xBC\xBC\xBF\xE4.", 0, 0, 0, 0);
			break;

		case 6:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xC0\xCC\xBA\xA5\xC6\xAE\xBF\xA1 \xB4\xE7\xC3\xB7\xC0\xCC \xBE\xC8\xB5\xC7\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			break;
		}
		break;
	}

	case 0xE0: {
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		unsigned char result = packet.Decode1();
		unsigned long mascotId = packet.Decode4();

		switch (result) {
		case 1:
		case 2:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xB8\xDE\xBC\xBC\xC1\xF6 \xBA\xAF\xB0\xE6\xBF\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);
			break;

		case 3:
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xB1\xDD\xBE\xD7\xC0\xCC \xBA\xCE\xC1\xB7\xC7\xD5\xB4\xCF\xB4\xD9", 0, 0, 0, 0);
			break;

		case 4: {
			GetActor("RealMyRoom") << MsgObject(0, 0x23, (intptr_t)"\xB8\xDE\xBC\xBC\xC1\xF6\xB0\xA1 \xBA\xAF\xB0\xE6 \xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0);

			std::string message;
			packet.DecodeStr(message);

			WSingleton<CSharedDoc>::Instance()->m_myPang = packet.Decode8();

			sMascotInfo *pMascot = WSingleton<CSharedDoc>::Instance()->GetMyMascotInfo(mascotId);

			if (pMascot) {
				memset(pMascot->message, 0, sizeof(pMascot->message));
				strncpy(pMascot->message, message.c_str(), 0x1D);
			}

			GetActor("RealMyRoom") << MsgObject(0, 0x4E, pMascot->id, (intptr_t)pMascot, 0, 0, 0);
			break;
		}
		}
		break;
	}

	case 0x124:
		if (IsLocalContent((localContentType_t)0x56)) {
			unsigned long result = packet.Decode4();

			GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x124, result, 0, 0, 0);
		}
		break;

	case 0x125:
		if (IsLocalContent((localContentType_t)0x56)) {
			WSingleton<CSharedDoc>::Instance()->m_furnitureList.clear();

			unsigned long result = packet.Decode4();
			if (result == 1) {
				int count = packet.Decode2();

				WSingleton<CRealMyRoomData>::Instance()->m_modelList.Reset();
				for (int i = 0; i < count; i++) {
					sFurniture_List furniture;
					memset(&furniture, 0, sizeof(furniture));

					packet.DecodeBuffer(&furniture, sizeof(furniture));

					WSingleton<CRealMyRoomData>::Instance()->AddModel(furniture);
				}

				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x22F, 0, 0, 0, 0, 0));

				if (count > 0) {
					if (MyUID() == WSingleton<CSharedDoc>::Instance()->m_myRoomOwnerUID) {
						WSingleton<CRealMyRoomData>::Instance()->SaveModel(true);
					}
				}
			} else {
				GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x125, result, 0, 0, 0);
			}

			GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		}
		break;

	case 0x13F: {
		unsigned char result = packet.Decode1();

		GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x13F, result, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x181:
		if (NET()->IsConnected(WNetworkSystem::NET_UNIT_MSN)) {
			WSendPacket request(0x27);
			request.EncodeStr(WSingleton<CSharedDoc>::Instance()->m_userInfo.id);
			request.EncodeStr(WSingleton<CSharedDoc>::Instance()->m_userInfo.id);
			request.Send((eSendTo)1);

			WSingleton<CSharedDoc>::Instance()->m_lastGiftReceiverNick = "";

			WSingleton<CSharedDoc>::Instance()->m_lastGiftReceiverUID = -1;
		}
		break;

	case 0x13E: {
		if (NET()->IsConnected(WNetworkSystem::NET_UNIT_MSN)) {
			WSendPacket request(0x27);

			request.Encode4(WSingleton<CSharedDoc>::Instance()->m_myUID);
			request.Encode4(WSingleton<CSharedDoc>::Instance()->m_lastGiftReceiverUID);

			request.Send((eSendTo)1);

			WSingleton<CSharedDoc>::Instance()->m_lastGiftReceiverUID = -1;

			WSingleton<CSharedDoc>::Instance()->m_lastGiftReceiverNick = "";
		}

		bool notified = false;
		int count = packet.Decode4();

		sMailIncludeItem item;

		for (int i = 0; i < count; i++) {
			packet.DecodeBuffer(&item, 0x35);

			if (item.count != 0) {
				if (!notified) {
					GetActor("RealMyRoom") << MsgObject(0, 0x242, item.typeId, (intptr_t)&notified, 0, 0, 0);
				}

				switch (item.typeId >> 26) {
			case 6: {
				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
					if (it->m_serial == item.guid) {
						if (it->m_typeId == item.typeId) {
							if (it->m_count > (int)item.count) {
								it->m_count -= (short)item.count;
							} else if (it->m_count == (int)item.count) {
								WSingleton<CSharedDoc>::Instance()->m_itemInfoList.erase(it);
							}
						}
						break;
					}
				}
				break;
			}

			case 5: {
				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end(); ++it) {
					if (it->m_serial == item.guid) {
						if (it->m_typeId == item.typeId) {
							if (it->m_count > (int)item.count) {
								it->m_count -= (short)item.count;
							} else if (it->m_count == (int)item.count) {
								WSingleton<CSharedDoc>::Instance()->m_myGearList.erase(it);
							}
						}
						break;
					}
				}
				break;
			}

			case 2:
				if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(item.typeId)) {
					std::list<sItemInfo>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()->m_myItemList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myItemList.end(); ++it) {
						if (it->m_serial == item.guid && it->m_typeId == item.typeId) {
							WSingleton<CSharedDoc>::Instance()->m_myItemList.erase(it);
							break;
						}
					}
				} else {
					CSharedDoc *pDoc = WSingleton<CSharedDoc>::Instance();
					std::list<sItemInfo>::iterator it;
					for (it = pDoc->m_myPartsList.begin(); it != pDoc->m_myPartsList.end(); ++it) {
						if (it->m_serial == item.guid) {
							if (it->m_typeId == item.typeId) {
								pDoc->m_myPartsList.erase(it);
							}
							break;
						}
					}
				}
				break;

			case 4: {
				std::map<unsigned int, sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_myItemMap.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myItemMap.end(); ++it) {
					if (it->second.m_serial == item.guid) {
						if (it->second.m_typeId == item.typeId) {
							WSingleton<CSharedDoc>::Instance()->m_myItemMap.erase(it);
						}
						break;
					}
				}
				break;
			}

			case 0x1C: {
				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.end(); ++it) {
					if (it->m_serial == item.guid) {
						if (it->m_typeId == item.typeId) {
							if (it->m_count > (int)item.count) {
								it->m_count -= (short)item.count;
							} else if (it->m_count == (int)item.count) {
								WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.erase(it);
							}
						}
						break;
					}
				}
				break;
			}

			case 7: {
				std::map<unsigned int, sCaddieInfo> &caddies = WSingleton<CSharedDoc>::Instance()->m_caddieMap;
				std::map<unsigned int, sCaddieInfo>::iterator it = caddies.find(item.guid);
				if (it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end()) {
					caddies.erase(it);
				}
				break;
			}

			case 0x1F: {
				std::list<sCards>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_myCardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myCardList.end(); ++it) {
					if (it->m_typeId == item.typeId) {
						it->m_count -= item.count;
						if (it->m_count < 1) {
							WSingleton<CSharedDoc>::Instance()->m_myCardList.erase(it);
						}
						break;
					}
				}
				break;
			}
				}
			}
		}

		if (notified) {
			GetActor("RealMyRoom") << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);
		}

		GetActor("RealMyRoom") << MsgObject(0, 0x225, 0x13E, 0, 0, 0, 0);
		break;
	}

	case 0x141: {
		int result = packet.Decode1();

		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x13F, result, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x140: {
		static sMailBoxListForPage mailPage;
		static sMailInfoBrief mailBrief;

		mailPage.totalPage = packet.Decode4();
		mailPage.page = packet.Decode4();
		mailPage.totalMail = packet.Decode4();
		mailPage.mailList.clear();

		for (int i = 0; i < mailPage.totalMail; i++) {
			memset(&mailBrief, 0x30, 0xC2);

			packet.DecodeBuffer(&mailBrief, 0xC2);
			mailPage.mailList.push_back(mailBrief);
			if (IsLocalContent((localContentType_t)0x57)) {
				RequestUccInfo(mailBrief.m_item);
			}
		}

		WSingleton<CTransferableItem>::Instance()->SetMailInfoPage(mailPage);

		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x223, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x143: {
		int result = packet.Decode1();

		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x13F, result, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x142: {
		static sMailInfo mailInfo;
		static sMailIncludeItem includeItem;

		mailInfo.mailIndex = packet.Decode4();

		strncpy(mailInfo.senderNick, packet.DecodeStr().c_str(), 0x16);
		strncpy(mailInfo.date, packet.DecodeStr().c_str(), 0x20);
		strncpy(mailInfo.message, packet.DecodeStr().c_str(), 0x320);

		mailInfo.isRead = packet.Decode1() == 1;

		mailInfo.itemList.clear();
		if (mailInfo.isRead) {
			int count = packet.Decode4();

			for (int i = 0; i < count; i++) {
				packet.DecodeBuffer(&includeItem, 0x35);
				mailInfo.itemList.push_back(includeItem);
				if (IsLocalContent((localContentType_t)0x57)) {
					RequestUccInfo(includeItem);
				}
			}
		}

		WSingleton<CTransferableItem>::Instance()->SetMailInfo(mailInfo);

		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x224, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x15C:
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x226, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;

	case 0x15D: {
		unsigned long result = packet.Decode4();

		GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x15D, result, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	case 0x145: {
		unsigned char result = packet.Decode1();

		GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x145, result, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);
		break;
	}

	// realmyroomtask.cpp:1600
	case 0x144: {
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0x244, 1, 0, 0, 0, 0);

		int count = packet.Decode4();

		sMailIncludeItem item;

		for (int i = 0; i < count; i++) {
			packet.DecodeBuffer(&item, 0x35);
			int num = (int)item.count < 1 ? 1 : item.count;

			unsigned long typeId = item.typeId;
			unsigned long guid = item.guid;
			unsigned char kind = item.kind;
			unsigned long flag = item.flag;

			if (typeId == 0x1A000010) {
				continue;
			}

			switch (typeId >> 26) {
			case 1:
				continue;

			case 2:
				if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(typeId)) {
					WSendPacket request((enumClientPacket)0xB1);
					request.Encode1(1);
					request.Encode4(guid);
					request.Encode1(1);
					request.Send((eSendTo)0);
					break;
				}

				{
					sItemInfo info;
					memset(&info, 0, sizeof(info));
					info.m_serial = guid;
					info.m_typeId = typeId;
					info.m_stateBits.kind = kind;
					info.m_remainHours = (int)flag / 60 / 24;
					info.m_flags28.flags01 |= 2;

					WSingleton<CSharedDoc>::Instance()->AddMyPartsList(info);
				}
				break;

			case 7: {
				sCaddieInfo caddie;
				memset(&caddie, 0, sizeof(caddie));
				caddie.id = guid;
				caddie.typeId = typeId;

				unsigned short days;
				if (typeId == 0x1C000001 || typeId == 0x1C000002 || typeId == 0x1C000003 || typeId == 0x1C000007) {
					days = (int)flag / 60 / 24;
					caddie.flags |= 2;
					caddie.days = days;
				} else {
					days = 0;
					caddie.days = days;
					caddie.flags &= ~2;
				}

				if (typeId >= 0x1C00000C && typeId <= 0x1C00000E) {
					caddie.level = kind;
					caddie.experience = flag;
				}

				std::map<unsigned int, sCaddieInfo> &caddieMap = WSingleton<CSharedDoc>::Instance()->m_caddieMap;
				std::map<unsigned int, sCaddieInfo>::iterator it = caddieMap.find(guid);
				if (it != WSingleton<CSharedDoc>::Instance()->m_caddieMap.end()) {
					it->second.days = days;
					break;
				}

				caddieMap.insert(std::map<unsigned int, sCaddieInfo>::value_type(guid, caddie));
				break;
			}

			case 8: {
				int hours = (int)flag / 60;
				std::map<unsigned int, sCaddieInfo> &caddieMap = WSingleton<CSharedDoc>::Instance()->m_caddieMap;
				std::map<unsigned int, sCaddieInfo>::iterator it = caddieMap.find(guid);
				if (it == WSingleton<CSharedDoc>::Instance()->m_caddieMap.end()) {
					break;
				}

				it->second.cadItemTypeId = typeId;
				it->second.hours = hours;
				if (it->second.days < hours) {
					it->second.days = hours;
				}
				break;
			}

			case 6: {
				sItemInfo info;
				memset(&info, 0, sizeof(info));
				info.m_serial = guid;
				info.m_typeId = typeId;
				info.m_stateBits.kind = kind;
				info.m_count = num;
				info.m_flags28.flags01 |= 2;

				if (info.m_stateBits.kind) {
					if (info.m_stateBits.kind == 3) info.m_remainHours = flag;
					else if (info.m_stateBits.kind == 4) info.m_remainHours = flag * 24;
					else if (info.m_stateBits.kind == 5) info.m_remainHours = flag * 24 * 30;
					else if (info.m_stateBits.kind == 2) info.m_remainHours = (int)flag / 60;
				}

				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
					if (it->m_serial == guid) {
						it->m_count = num;
						break;
					}
				}

				if (it == WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
					if (IsLocalContent((localContentType_t)0x14)) {
						if (typeId == 0x1A000042) {
							info.m_clubStatLevel[1] = packet.Decode2();
							info.m_clubStatLevel[2] = packet.Decode2();

							info.m_remainHours = 0xA8;

							SYSTEMTIME endTime;
							packet.DecodeBuffer(&endTime, sizeof(endTime));
							memcpy(info.m_reserved18, &endTime, sizeof(endTime));
							info.m_stateBits.kind = 2;
						}
					}

					WSingleton<CSharedDoc>::Instance()->m_itemInfoList.push_back(info);
				}
				break;
			}

			case 5: {
				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_myGearList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myGearList.end(); ++it) {
					if (it->m_serial == guid) {
						it->m_count = num;
						break;
					}
				}

				if (it == WSingleton<CSharedDoc>::Instance()->m_myGearList.end()) {
					sItemInfo info;
					memset(&info, 0, sizeof(info));
					info.m_serial = guid;
					info.m_typeId = typeId;
					info.m_count = num;
					info.m_stateBits.kind = 0;
					info.m_flags28.flags01 |= 2;

					WSingleton<CSharedDoc>::Instance()->m_myGearList.push_back(info);
				}
				break;
			}

			case 3:
			case 4: {
				sItemInfo info;
				memset(&info, 0, sizeof(info));
				info.m_serial = item.guid;
				info.m_typeId = typeId;
				info.m_flags28.flags01 |= 2;
				memcpy(info.m_clubStatLevel, item.stats, sizeof(item.stats));

				WSingleton<CSharedDoc>::Instance()->m_myItemMap.insert(std::map<unsigned int, sItemInfo>::value_type(item.guid, info));
				break;
			}

			case 29: {
				IFF_STRUCT::sQuestDrop *pDrop = WSingleton<CSharedDoc>::Instance()->m_itemManager.FindQuestDrop(typeId);
				if (pDrop) {
					std::list<sItemInfo>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()->m_myExtraItemList4.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myExtraItemList4.end(); ++it) {
						if (it->m_serial == guid) {
							it->m_count++;
							break;
						}
					}

					if (it == WSingleton<CSharedDoc>::Instance()->m_myExtraItemList4.end()) {
						sItemInfo info;
						memset(&info, 0, sizeof(info));
						info.m_serial = guid;
						info.m_typeId = typeId;
						info.m_count = 1;

						WSingleton<CSharedDoc>::Instance()->m_myExtraItemList4.push_back(info);
					}
				}
				break;
			}

			case 16: {
				sMascotInfo mascot;
				memset(&mascot, 0, sizeof(mascot));
				mascot.id = guid;
				strcpy(mascot.message, "PANGYA!");
				mascot.typeId = typeId;
				int hours = (int)flag / 60;
				mascot.remainHours = hours;

				SYSTEMTIME now;
				GetLocalTime(&now);
				FILETIME fileTime;
				SystemTimeToFileTime(&now, &fileTime);

				*(__int64 *)&fileTime +=
					hours * 36000000000i64;

				FileTimeToSystemTime(&fileTime, &now);

				mascot.endTime = now;

				WSingleton<CSharedDoc>::Instance()->AddMascot(&mascot);
				break;
			}

			case 28: {
				sItemInfo info;
				memset(&info, 0, sizeof(info));
				info.m_serial = guid;
				info.m_typeId = typeId;
				info.m_stateBits.kind = kind;
				info.m_count = num;
				info.m_flags28.flags01 |= 2;

				if (info.m_stateBits.kind) {
					if (info.m_stateBits.kind == 3) info.m_remainHours = flag;
					else if (info.m_stateBits.kind == 4) info.m_remainHours = flag * 24;
					else if (info.m_stateBits.kind == 5) info.m_remainHours = flag * 24 * 30;
					else if (info.m_stateBits.kind == 2) info.m_remainHours = (int)flag / 60;
				}

				std::list<sItemInfo>::iterator it;
				for (it = WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.end(); ++it) {
					if (it->m_serial == guid) {
						it->m_count = num;
						break;
					}
				}

				if (it == WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.end()) {
					WSingleton<CSharedDoc>::Instance()->m_myExtraItemList3.push_back(info);
				}
				break;
			}

			case 31: {
				IFF_STRUCT::sCard *pCard = WSingleton<CSharedDoc>::Instance()->m_itemManager.FindCard(typeId);
				if (pCard) {
					sCards card;
					memset(&card, 0, sizeof(card));
					card.m_guid = guid;
					card.m_typeId = typeId;
					card.m_count = num;
					card.unknown38 = 1; // FIRST PASS: the original stores a byte at 0x38

					bool found = false;
					std::list<sCards>::iterator it;
					for (it = WSingleton<CSharedDoc>::Instance()->m_myCardList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_myCardList.end(); ++it) {
						if (it->m_typeId == typeId) {
							it->m_count = num;
							found = true;
							break;
						}
					}

					if (!found) {
						WSingleton<CSharedDoc>::Instance()->m_myCardList.push_back(card);
					}
				}
				break;
			}

			case 14: {
				sItemInfo info;
				memset(&info, 0, sizeof(info));
				info.m_serial = guid;
				info.m_typeId = typeId;
				info.m_stateBits.kind = kind;
				info.m_flags28.flags01 |= 2;
				info.m_remainHours = flag * 24;

				WSingleton<CSharedDoc>::Instance()->m_mySkinList.push_back(info);
				break;
			}

			case 18: {
				IFF_STRUCT::sFurniture *pFurniture = WSingleton<CSharedDoc>::Instance()->m_itemManager.FindFurniture(typeId);
				if (pFurniture) {
					sFurniture_List furniture;
					memset(&furniture, 0, sizeof(furniture));

					furniture.m_id = guid;
					furniture.m_typeId = typeId;
					furniture.m_pos.x = pFurniture->m_pos[0];
					furniture.m_pos.y = pFurniture->m_pos[1];
					furniture.m_pos.z = pFurniture->m_pos[2];
					furniture.m_angle = pFurniture->m_angle;
					furniture.m_flags &= ~1;

					WSingleton<CSharedDoc>::Instance()->AddFurniture(furniture);
				}
				break;
			}
			}
		}

		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC6\xED\xC1\xF6\xBF\xA1 \xC6\xF7\xC7\xD4\xB5\xC8 \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xB8\xB6\xC0\xCC\xB7\xEB\xC0\xB8\xB7\xCE \xC0\xCC\xB5\xBF\xBD\xC3\xC4\xD7\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));

		GetActor("RealMyRoom") << MsgObject(0, 0x226, 0, 0, 0, 0, 0);
		GetActor("RealMyRoom") << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);
		break;
	}

	case 0x116:
		if (IsLocalContent((localContentType_t)0x14)) {
			GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

			WSingleton<CSharedDoc>::Instance()->m_tikiReportList.clear();

			long count = packet.Decode4();
			SYSTEMTIME reportTime;
			packet.DecodeBuffer(&reportTime, sizeof(reportTime));

			if (count == 0) {
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xC6\xBC\xC5\xB0 \xB8\xAE\xC6\xF7\xC6\xAE \xBF\xAD\xB6\xF7\xBF\xA1 \xBD\xC7\xC6\xD0 \xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9. ", 0, 0, 0, 0));
				break;
			}
			else if (count == -2) {
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xB5\xA5\xC0\xCC\xC5\xB8\xB0\xA1 \xC0\xAF\xC8\xBF\xC7\xCF\xC1\xF6 \xBE\xCA\xC0\xBA \xC6\xBC\xC5\xB0 \xB8\xAE\xC6\xF7\xC6\xAE \xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
				break;
			}
			else if (count == -1) {
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xB3\xAF\xC2\xA5\xB0\xA1 \xC0\xAF\xC8\xBF\xC7\xCF\xC1\xF6 \xBE\xCA\xC0\xBA \xC6\xBC\xC5\xB0 \xB8\xAE\xC6\xF7\xC6\xAE \xC0\xD4\xB4\xCF\xB4\xD9.", 0, 0, 0, 0));
				break;
			}

			if (IsLocalContent((localContentType_t)0x58)) {
				WSingleton<CSharedDoc>::Instance()->m_value4843 = 1;
			}

			unsigned long serial = 0;
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x204, (intptr_t)&serial, 0, 0, 0, 0);
			std::list<sItemInfo>::iterator it;
			for (it = WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(); it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(); ++it) {
				if (it->m_serial == serial)
					break;
			}

			if (it == WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end())
				return 0;

			for (int i = 0; i < count; ++i) {
				--it->m_count;

				if (it->m_count <= 0)
					break;
			}

			WSingleton<CSharedDoc>::Instance()->m_itemInfoList.erase(it);
			GetActor("RealMyRoom") << MsgObject(0, 0xDD, 0, 0, 0, 0, 0);

			for (int j = 0; j < count; ++j) {
				sCaddieReportData report;
				packet.DecodeBuffer(&report, sizeof(report));
				WSingleton<CSharedDoc>::Instance()->m_tikiReportList.push_back(report);
			}

			WSingleton<CSharedDoc>::Instance()->BuildTikiReportList();

			GetActor("RealMyRoom") << MsgObject(0, 0x205, (intptr_t)&reportTime, 0, 0, 0, 0);
		}
		break;

	case 0x17A:
	case 0x17C:
		break;

	case 0x121: {
		unsigned char result = packet.Decode1();

		switch (result) {
		case 0: {
			GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

			std::list<sPouchPrize> prizes;
			unsigned long itemId = packet.Decode4();
			int count = packet.Decode4();

			std::list<sItemInfo>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_itemInfoList.begin(), WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end(), itemId);
			if (it != WSingleton<CSharedDoc>::Instance()->m_itemInfoList.end()) {
				it->m_count--;
				if (it->m_count < 1) {
					WSingleton<CSharedDoc>::Instance()->m_itemInfoList.erase(it);
				}
			}

			std::string message;

			char text[64] = {0};
			sPouchPrize prize;
			for (int i = 0; i < count; i++) {
				packet.DecodeBuffer(&prize, 0x14);
				WSingleton<CSharedDoc>::Instance()->m_itemManager.FindCommonItem(prize.typeId);
			}

			switch (prize.typeId >> 26) {
			case 5:
				sprintf(text, "\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB %d \xB0\xB3 \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", prize.quantity);
				break;

			case 6:
				if (prize.typeId == 0x1A000010) {
					sprintf(text, "%d \xC6\xCE\xC0\xBB \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", prize.quantity);
					WSingleton<CSharedDoc>::Instance()->m_myPang += prize.quantity;
				} else {
					sprintf(text, "\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB %d \xB0\xB3 \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", prize.quantity);
				}
				break;

			default:
				sprintf(text, "\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.");
				break;
			}

			message = text;
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x26, (intptr_t)message.c_str(), prize.typeId, 0, 0, 0));
			GetActor("RealMyRoom") << MsgObject(0, 0xDD, itemId, 1, 0, 0, 0);
			break;
		}

		case 1: {
			GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

			unsigned long error = packet.Decode4();

			FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			std::string message = "\xC1\xA4\xC0\xC7\xB5\xC7\xC1\xF6 \xBE\xCA\xC0\xBA \xBF\xC0\xB7\xF9.";

			switch (error) {
			case 0:
				message = "\xC0\xDF\xB8\xF8\xB5\xC8 \xBE\xC6\xC0\xCC\xC5\xDB \xC4\xDA\xB5\xE5 \xC0\xD4\xB4\xCF\xB4\xD9. \xB0\xFC\xB8\xAE\xC0\xDA\xBF\xA1\xB0\xD4 \xB9\xAE\xC0\xC7\xC7\xCF\xBC\xBC\xBF\xE4.";
				break;

			case 1:
				message = "\xBC\xAD\xB9\xF6\xC0\xC7 \xBF\xE4\xC3\xBB\xC0\xCC \xB8\xB9\xBD\xC0\xB4\xCF\xB4\xD9. \xC0\xE1\xBD\xC3 \xC8\xC4 \xB4\xD9\xBD\xC3 \xBD\xC3\xB5\xB5 \xC7\xD8 \xC1\xD6\xBD\xCA\xBD\xC3\xBF\xC0.";
				break;

			case 2:
				message = "\xBE\xC6\xC0\xCC\xC5\xDB \xBC\xF6\xB7\xAE\xC0\xCC \xBA\xCE\xC1\xB7\xC7\xD5\xB4\xCF\xB4\xD9.\n \xC0\xE7\xB7\xCE\xB1\xD7\xC0\xCE\xC7\xCF\xBD\xC3\xB1\xE2 \xB9\xD9\xB6\xF8\xB4\xCF\xB4\xD9.";
				break;

			case 3:
				message = "\xBD\xC7\xC6\xD0\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.";
				break;
			}

			pForm->SetMessage(message.c_str(), false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
			break;
		}
		}
		break;
	}

	case 0x1A2: {
		GetActor("RealMyRoom") << MsgObject(0, 2, 0, 0, 0, 0, 0);

		unsigned long result = packet.Decode4();
		unsigned long itemId = packet.Decode4();
		unsigned long typeId = packet.Decode4();
		unsigned long quantity = packet.Decode4();

		const char *failTitle = "\xBB\xF3\xC0\xDA \xBF\xAD\xB1\xE2\xB0\xA1 \xBD\xC7\xC6\xD0\xC7\xDF\xBD\xC0\xB4\xCF\xB4\xD9.";

		char text[256] = "";

		switch (result) {
		case 0:
			if (!WSingleton<CSharedDoc>::Instance()->m_itemManager.FindCommonItem(typeId)) {
				char wrongType[64] = "";
				strcat(wrongType, "\xC0\xDF\xB8\xF8\xB5\xC8 \xBE\xC6\xC0\xCC\xC5\xDB \xC5\xB8\xC0\xD4ID \xC0\xD4\xB4\xCF\xB4\xD9.");

				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)wrongType, 0, 0, 0, 0));
				GetActor("RealMyRoom") << MsgObject(0, 0xDD, itemId, 1, 0, 0, 0);
				break;
			}

			switch (typeId >> 26) {
			case 5:
				sprintf(text, "\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB %d \xB0\xB3 \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", quantity);
				break;

			case 6:
				if (typeId == 0x1A000010) {
					sprintf(text, "%d \xC6\xCE\xC0\xBB \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", quantity);
				} else if (quantity > 1) {
					sprintf(text, "\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB %d \xB0\xB3 \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.", quantity);
				} else {
					sprintf(text, "\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.");
				}
				break;

			default:
				sprintf(text, "\xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xB9\xDE\xC0\xB8\xBC\xCC\xBD\xC0\xB4\xCF\xB4\xD9.");
				break;
			}

			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x26, (intptr_t)text, typeId, 0, 0, 0));
			GetActor("RealMyRoom") << MsgObject(0, 0xDD, itemId, 1, 0, 0, 0);

			if (IsLocalContent((localContentType_t)0x60)) {
				WSingleton<CSharedDoc>::Instance()->m_mailList.clear();
				WSendPacket request(0x15E);
				request.Send(SEND_TO_DEFAULT);
			} else {
				WSingleton<CSharedDoc>::Instance()->m_giftList.clear();
				WSendPacket request((enumClientPacket)0x93);
				request.Send(SEND_TO_DEFAULT);
			}

			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xBB\xF3\xC7\xB0\xC0\xCC \xBF\xEC\xC6\xED\xC0\xB8\xB7\xCE \xC0\xFC\xB4\xDE\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9. \xBF\xEC\xC6\xED\xC7\xD4\xC0\xBB \xC8\xAE\xC0\xCE\xC7\xCF\xBC\xBC\xBF\xE4.", 0, 0, 0, 0));
			break;

		case 4:
			strcpy(text, failTitle);
			strcat(text, "\x0A\xBC\xF6\xB7\xAE\xC0\xCC \xC3\xE6\xBA\xD0\xC7\xCF\xC1\xF6 \xBE\xCA\xBD\xC0\xB4\xCF\xB4\xD9.");

			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0));
			break;

		case 8:
			strcpy(text, failTitle);
			strcat(text, "\x0A\xBE\xC6\xC0\xCC\xC5\xDB \xC1\xBE\xB7\xF9 \xBF\xA1\xB7\xAF\xC0\xD4\xB4\xCF\xB4\xD9.");

			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0));
			break;

		case 0x200B3B:
		case 0x200B3C:
		case 0x200B3D: {
			IActor *pActor = GetActor(data_a8d820[4]);
			pActor << MsgObject(0, 0x276, 1, result, 0, 0, 0);
			break;
		}

		default:
			strcpy(text, failTitle);
			strcat(text, "\n");
			strcat(text, MakeStr("Error : %d", result));

			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0));
			break;
		}
		break;
	}

	case 0x175:
		if (IsLocalContent((localContentType_t)0x71) == true) {
			unsigned long result = packet.Decode4();
			unsigned long flags = packet.Decode4();

			if (result == 0) {
				if (flags & 2) {
					AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x236, 0, 0, 0, 0, 0);
					WSingleton<CSharedDoc>::Instance()->m_flag50F4 = true;
				} else if ((flags & 4) && (flags & 8)) {
					if (!WSingleton<CSharedDoc>::Instance()->m_flag50F4) {
						WSingleton<CSharedDoc>::Instance()->m_flag50F4 = true;
					} else {
						AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x235, 0, 0, 0, 0, 0);
					}

					WSingleton<CSharedDoc>::Instance()->m_storageLock = true;
				} else if ((flags & 4) && (flags & 0x10)) {
					if (!WSingleton<CSharedDoc>::Instance()->m_flag50F4) {
						WSingleton<CSharedDoc>::Instance()->m_flag50F4 = true;
					} else {
						WSendPacket request((enumClientPacket)0xCE);
						request.EncodeStr("\0"); // a two-byte literal in the original (??_C@_01LOCGONAA@)
						request.Send(SEND_TO_DEFAULT);
					}

					WSingleton<CSharedDoc>::Instance()->m_storageLock = false;
				}
			} else {
				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23F, 0x175, result, 0, 0, 0);
			}

			CItemStorage *pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->UnLock();
			}
		}
		break;

	case 0x17B:
		if (IsLocalContent((localContentType_t)0x71) == true) {
			unsigned long result = packet.Decode4();

			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23F, 0x17B, result, 0, 0, 0);

			CItemStorage *pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->UnLock();
			}
		}
		break;

	case 0x171:
		if (IsLocalContent((localContentType_t)0x71) == true) {
			unsigned long result = packet.Decode4();

			if (result != 0) {
				if (result == 0x66) {
					AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x234, 0, 0, 0, 0, 0);
				}
				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23F, 0x171, result, 0, 0, 0);
			} else {
				WSendPacket request((enumClientPacket)0xD7);
				request.Send(SEND_TO_DEFAULT);

				WSendPacket pageRequest((enumClientPacket)0xCF);
				pageRequest.Encode4(0x63);
				pageRequest.Encode2(1);
				pageRequest.Send(SEND_TO_DEFAULT);
			}

			CItemStorage *pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->UnLock();
			}
		}
		break;

	case 0x172:
		if (IsLocalContent((localContentType_t)0x71) == true) {
			CItemStorage *pStorage = WSingleton<CItemStorage>::Instance();

			if (pStorage) {
				pStorage->ClearStoredItemInfo();

				unsigned long totalPage = packet.Decode2();
				unsigned long curPage = packet.Decode2();
				unsigned int count = packet.Decode1();

				for (unsigned int i = 0; i < count; i++) {
					sStoredItemInfo info;
					memset(&info, 0, sizeof(info));

					packet.DecodeBuffer(&info, sizeof(info));

					pStorage->AddStoredItemInfo(info);
				}

				pStorage->SetTotalPage(totalPage);
				pStorage->SetCurPage(curPage);

				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23A, 0, 0, 0, 0, 0);

				pStorage->UnLock();
			}
		}
		break;

	case 0x174:
		if (IsLocalContent((localContentType_t)0x71) == true) {
			unsigned long result = packet.Decode4();

			if (result == 0) {
				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23D, result, result, 0, 0, 0);
			} else {
				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23F, 0x174, result, 0, 0, 0);
			}

			CItemStorage *pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->UnLock();
			}
		}
		break;

	case 0x173:
		if (IsLocalContent((localContentType_t)0x71) == true) {
			unsigned long result = packet.Decode4();

			if (result == 0) {
				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23E, result, result, 0, 0, 0);
			} else {
				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23F, 0x173, result, 0, 0, 0);
			}

			CItemStorage *pStorage = WSingleton<CItemStorage>::Instance();
			if (pStorage) {
				pStorage->UnLock();
			}
		}
		break;

	case 0x176: {
		unsigned long result = packet.Decode4();

		if (result != 0) {
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23F, 0x176, result, 0, 0, 0);
		}

		CItemStorage *pStorage = WSingleton<CItemStorage>::Instance();
		if (pStorage) {
			pStorage->UnLock();
		}
		break;
	}

	case 0x177: {
		CItemStorage *pStorage = WSingleton<CItemStorage>::Instance();
		if (pStorage) {
			pStorage->SetStoredPang(packet.Decode8());

			pStorage->UnLock();
		}
		break;
	}

	case 0x178: {
		FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		std::string message;

		unsigned long result = packet.Decode4();

		switch (result) {
		case 0: {
			message = "\xb5\xb9\xc7\xc7\xb4\xcf\xc0\xc7 \xba\xf1\xb9\xd0\xb1\xdd\xb0\xed \xbc\xb3\xc1\xa4\xc0\xcc \xba\xaf\xb0\xe6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9";
			unsigned char lock = packet.Decode1();
			WSingleton<CSharedDoc>::Instance()->m_storageLock = (lock == 1);
			break;
		}

		case 0x65:
			message = "\xbc\xb3\xc1\xa4\xc0\xcc \xba\xaf\xb0\xe6\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9";
			break;

		case 0x64:
			message = "DataBase\xc1\xf6\xbf\xac \xc0\xe1\xbd\xc3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0";
			break;

		case 0x75:
			message = "\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0";
			break;
		}

		pForm->SetMessage(message.c_str(), false);
		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		break;
	}

	case 0x179: {
		unsigned long result = packet.Decode4();

		FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

		switch (result) {
		case 0:
			pForm->SetMessage("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xba\xaf\xb0\xe6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", false);
			break;
		case 0x65:
			pForm->SetMessage("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.", false);
			break;
		case 0x64:
			pForm->SetMessage("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.", false);
			break;
		case 0x68:
			pForm->SetMessage("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xc0\xc7 \xb1\xe6\xc0\xcc\xb0\xa1 \xc0\xdf\xb8\xf8 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9..", false);
			break;
		case 0x72:
			pForm->SetMessage("'\xb5\xb9\xc7\xc7\xb4\xcf\xc0\xc7 \xba\xf1\xb9\xd0\xb1\xdd\xb0\xed'\xc0\xc7 \xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xbc\xb3\xc1\xa4 \xb5\xc7\xbe\xee \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.", false);
			break;
		case 0x75:
			pForm->SetMessage("\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", false);
			break;
		}

		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		break;
	}

	case 0x184:
		if (IsLocalContent((localContentType_t)0x76)) {
			FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);

			switch (packet.Decode1()) {
			case 1:
				pForm->SetMessage("\xbc\xd2\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xc0\xba \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9.", false);
				break;
			case 2:
				pForm->SetMessage("\xbf\xc0\xb4\xc3\xc0\xba \xc0\xcc \xb0\xa1\xb1\xb8\xb8\xa6 \xb4\xf5 \xc0\xcc\xbb\xf3 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				break;
			case 5:
				pForm->SetMessage("\xbf\xc0\xb4\xc3\xc0\xba \xc0\xcc \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb4\xf5 \xc0\xcc\xbb\xf3 \xbe\xf2\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				break;
			case 0:
				pForm->SetMessage("\xc0\xcc \xb0\xa1\xb1\xb8\xc0\xc7 \xb1\xe2\xb4\xc9\xc0\xbb \xbb\xe7\xbf\xeb\xc7\xcf\xb4\xc2\xb5\xa5 \xbd\xc3\xbd\xba\xc5\xdb \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. GM\xbf\xa1\xb0\xd4 \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.", false);
				break;
			case 3:
				pForm->SetMessage("\xb9\xe8\xc4\xa1\xb5\xc7\xc1\xf6 \xbe\xca\xc0\xba \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9. \xb9\xe8\xc4\xa1\xc7\xd1 \xb4\xd9\xc0\xbd \xc0\xfa\xc0\xe5\xb9\xf6\xc6\xb0\xc0\xbb \xb4\xad\xb7\xaf \xc0\xfa\xc0\xe5\xc7\xcf\xbd\xc5 \xc8\xc4\xbf\xa1 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				break;
			case 4:
				pForm->SetMessage("\xb0\xa1\xb1\xb8\xc0\xc7 \xb1\xe2\xb4\xc9\xc0\xbb \xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbe\xf2\xb4\xc2 \xb5\xa5\xbf\xa1 \xb9\xae\xc1\xa6\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbe\xf2\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. GM\xbf\xa1\xb0\xd4 \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.", false);
				break;
			case 6:
				pForm->SetMessage("\xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb0\xa1\xb1\xb8\xc0\xd4\xb4\xcf\xb4\xd9.", false);
				break;
			}

			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		} else {
			FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage("\xb3\xad\xb7\xce \xb1\xe2\xb4\xc9\xc0\xba \xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ", false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		}
		break;

	case 0x183:
		if (IsLocalContent((localContentType_t)0x76)) {
			unsigned long first = packet.Decode4();
			unsigned long second = packet.Decode4();

			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x245, first, second, 0, 0, 0);
		} else {
			FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage("\xb3\xad\xb7\xce \xb1\xe2\xb4\xc9\xc0\xba \xc7\xf6\xc0\xe7 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ", false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		}
		break;

	case 0x194: {
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);

		FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		int result = packet.Decode1();
		if (result == 0) {
			unsigned long typeId = packet.Decode4();
			unsigned long serial = packet.Decode4();
			std::list<sItemInfo> *pList = (typeId & 0xFC000000) == 0x08000000 ? &WSingleton<CSharedDoc>::Instance()->m_myPartsList : &WSingleton<CSharedDoc>::Instance()->m_itemInfoList;
			std::list<sItemInfo>::iterator it = std::find_if(pList->begin(), pList->end(), _FindByGuid(serial));
			if (it != pList->end()) {
				it->m_stateBits.expired = 0;
				it->m_remainHours = 0xA8;
				pForm->SetMessage("\xb4\xeb\xbf\xa9 \xb1\xe2\xb0\xa3\xc0\xcc \xbf\xac\xc0\xe5 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", false);
			} else {
				pForm->SetMessage("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
			}
		} else {
			switch (result) {
			case 1:
				pForm->SetMessage("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xb4\xeb\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.", false);
				break;
			case 2:
				pForm->SetMessage("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				break;
			case 3:
				pForm->SetMessage("\xb1\xe2\xb0\xa3\xc0\xcc \xb8\xb8\xb7\xe1\xb5\xc8 \xc0\xc7\xbb\xf3\xb8\xb8 \xb1\xe2\xb0\xa3\xc0\xbb \xbf\xac\xc0\xe5 \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				break;
			case 5:
				pForm->SetMessage("\xb1\xe2\xb0\xa3 \xbf\xac\xc0\xe5 \xbf\xe4\xc3\xbb\xc0\xcc \xb0\xc5\xba\xce \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				break;
			default:
				pForm->SetMessage(MakeStr("\xc3\xb3\xb8\xae\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(%d)", result), false);
				break;
			}
		}

		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		break;
	}

	case 0x195: {
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);

		FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
		int result = packet.Decode1();
		if (result == 0) {
			unsigned long typeId = packet.Decode4();
			unsigned long serial = packet.Decode4();
			std::list<sItemInfo> *pList = (typeId & 0xFC000000) == 0x08000000 ? &WSingleton<CSharedDoc>::Instance()->m_myPartsList : &WSingleton<CSharedDoc>::Instance()->m_itemInfoList;
			std::list<sItemInfo>::iterator it = std::find_if(pList->begin(), pList->end(), _FindByGuid(serial));
			if (it != pList->end()) {
				pList->erase(it);
				pForm->SetMessage("\xb4\xeb\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbb\xe8\xc1\xa6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", false);

				GetMainActor() << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);
			} else {
				pForm->SetMessage("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
			}
		} else {
			switch (result) {
			case 1:
				pForm->SetMessage("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xb4\xeb\xbf\xa9 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.", false);
				break;
			case 2:
				pForm->SetMessage("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xbf\xa1 \xb4\xeb\xc7\xd1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				break;
			case 4:
				pForm->SetMessage("\xc4\xab\xb5\xe5\xb0\xa1 \xc0\xd6\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xbb\xe8\xc1\xa6 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				break;
			case 5:
				pForm->SetMessage("\xbe\xc6\xc0\xcc\xc5\xdb \xbb\xe8\xc1\xa6 \xbf\xe4\xc3\xbb\xc0\xcc \xb0\xc5\xba\xce \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				break;
			default:
				pForm->SetMessage(MakeStr("\xc3\xb3\xb8\xae\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(%d)", result), false);
				break;
			}
		}

		pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
		break;
	}

	case 0x1B3: {
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		unsigned long result = packet.Decode4();
		if (result != 1) {
			GetActor(data_a8d820[1]) << MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		GetActor(data_a8d820[1]) << MsgObject(0, 0x263, 0x1A, 0x407, 0, 0, 0);
		break;
	}

	case 0x1BD: {
		unsigned long result = packet.Decode4();
		if (result != 1) {
			GetActor(data_a8d820[1]) << MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		GUILD_USER_INFO info;
		packet.DecodeBuffer(&info, sizeof(info));
		GetActor(data_a8d820[1]) << MsgObject(0, 0x263, 5, (intptr_t)&info, 0, 0, 0);
		break;
	}

	case 0x1B4: {
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		unsigned long result = packet.Decode4();
		if (result != 1) {
			GetActor(data_a8d820[1]) << MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		IActor *pActor = AfxGetTask()->GetActor(data_a8d820[1]);
		pActor << MsgObject(0, 0x263, 0x1A, 0x405, 0, 0, 0);

		std::string name;
		packet.DecodeStr(name);
		pActor << MsgObject(0, 0x263, 3, (intptr_t)&name, 0, 0, 0);
		break;
	}

	case 0x1B5: {
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		unsigned long result = packet.Decode4();
		if (result != 1) {
			GetActor(data_a8d820[1]) << MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		IActor *pActor = GetActor(data_a8d820[1]);
		pActor << MsgObject(0, 0x263, 0x1A, 0x40F, 0, 0, 0);
		pActor << MsgObject(0, 0x263, 0x19, 0, 0, 0, 0);
		break;
	}

	case 0x1C7: {
		unsigned long result = packet.Decode4();
		if (result != 1) {
			GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
			GetActor(data_a8d820[1]) << MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		unsigned long guildNumber = packet.Decode4();
		std::string name;
		packet.DecodeStr(name);

		GetActor(data_a8d820[1]) << MsgObject(0, 0x263, 0x15, guildNumber, (intptr_t)&name, 0, 0);
		break;
	}

	case 0x1C8: {
		GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		unsigned long result = packet.Decode4();
		if (result != 1) {
			GetActor(data_a8d820[1]) << MsgObject(0, 0x263, 0x1A, result, 0, 0, 0);
			break;
		}

		GetActor(data_a8d820[1]) << MsgObject(0, 0x263, 0x1A, 0x41D, 0, 0, 0);
		break;
	}

	default:
		return OnPacketCommon(packet);
	}

	return 1;
}
