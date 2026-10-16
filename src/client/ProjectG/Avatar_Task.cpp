// Original: d:\build\custom\temp\code\client\projectg\avatar_task.cpp
// The original holds the out-of-line copy of COption::gSetAvatarNewBie
// (clientsetting.h:409) in this object.
#define AVATAR_TASK_CLIENTSETTING_INLINE
#include "TaskManager.h"
#include "avatar_avatar.h"
#include "avatar_task.h"
#include "ChatMsg.h"
#include "ClientSetting.h"
#include "QuitWindow.h"
#include "ObjectFactory.h"
#include "fresh.h"
#include "frdesktop.h"
#include "frwndmanager.h"
#include "wview.h"
#include "shared/Localize.h"
#include "partsinfo.h"
#include "SharedDoc.h"
#include "fx.h"
#include "actor.h"
#include "mousecursor.h"
#include "InputManager.h"
#include "frgaugebar.h"
#include "scenemanager.h"
#include "Packet.h"
#include "pg_boot_todo.h"
#include "UccLibrary.h"
#include "GolfDoc.h"
#include "shared/s5/specialprize/specialprizeitem.h"
#include <algorithm>

// The class and actor names of the task's last actor (two adjacent pointers
// of unnamed data).
extern const char *data_00a8438c;
extern const char *data_00a84390;

// The loading parameters CTask::Load hands to the loading thread (layout in
// taskmanager.cpp, where it is defined; its header is not known yet).
struct sLoadParam {
	WList<sActor *> *m_pActorList; // 0x0
	int m_actorCount; // 0x4
	int m_progress;   // 0x8
	bool m_bInited;   // 0xC
	bool m_bLoaded;   // 0xD
};
extern sLoadParam g_LoadParam;

// Type of the packet being handled (set by every task's OnPacket).
extern int g_iRecvPacketType;

// Set once the avatar loading thread loaded the player's own avatar.
extern bool g_SALoadingState;

// avatar_avatar.h:53
bool ntAvatarBurden::operator<(const ntAvatarBurden &other) const {
	return m_key < other.m_key;
}

// avatar_avatar.h:58 (a burden whose pet frame is the -1 sentinel never matches)
bool ntAvatarBurden::operator==(const ntAvatarBurden &other) const {
	return m_key == other.m_key && m_pPetFrame != (CPetFrame *)-1 && other.m_pPetFrame != (CPetFrame *)-1;
}

// taskmanager.h:102
void CTask::OnInitFinished(void) {
}

// taskmanager.h:107
bool CTask::IsRecieveWhisper(void) {
	return m_bRecieveWhisper;
}

// avatar_task.cpp:38 (run-time type and factory registration of ntAvatarChatTask)
const WRTTI ntAvatarChatTask::m_RTTI("ntAvatarChatTask", &CTask::m_RTTI);

IObject* ntAvatarChatTaskMakeInstance(void) {
	return new ntAvatarChatTask;
}

struct __sntAvatarChatTask {
	__sntAvatarChatTask(void) {
		ObjectFactory().AddObjectFunctor(ntAvatarChatTaskMakeInstance, "ntAvatarChatTask");
	}
};
static __sntAvatarChatTask s_registerntAvatarChatTask;

// avatar_task.cpp:41
ntAvatarChatTask::ntAvatarChatTask(void) {
	m_unknown38 = false;
	m_destroying = false;
	InitializeCriticalSection(&m_burdenLock);

	new ntAvatarManager;
}

// avatar_task.cpp:52
ntAvatarChatTask::~ntAvatarChatTask(void) {
	COption::Instance()->gSetAvatarNewBie(0);

	COption::Instance()->vSetMinimumScreenSize(0, 800, 600);

	m_destroying = true;
	m_anotherHand.WaitUntilWorkDone(9999);
	m_anotherHand.BreakWorkingThread();

	DeleteCriticalSection(&m_burdenLock);

	if (ntAvatarManager::IsInstantiated()) delete ntAvatarManager::Instance();
}

// avatar_task.cpp:68
void ntAvatarChatTask::PreLoadInit(void) {
	sActor *pActor = m_actorList.Start();
	while (pActor) {

		pActor->m_pActor->OnPreLoadInit();
		pActor = m_actorList.Next();
	}

	IActor::m_defaultSkipLayer = 0;
	IActor::m_pLayer = 0;
	SetLoadingBackGround("loading_background.jpg");

	CChatMsg::Instance()->ResetUiPosition();

	new CQuitWindow;

	SetGameType(2);

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
		m_stateFlags |= 2;
		OnInitFinished();
	}

	g_LoadParam.m_actorCount = 1;
	g_LoadParam.m_progress = 0;

	if (m_tipDlgState) {
		m_tipDlgState->SetRange(0, 100, 0);
	}

	m_pLoadingFresh = new Fresh;
	m_tipDlgState = NULL;

	m_pLoadingFresh->Init("loading.xml", false,
		m_loadingBackGround != "" ?
		m_loadingBackGround.c_str()
		:
		NULL);

	m_pLoadingFresh->OpenLayout("LOADING", NULL, true);

	if (m_unknown59) {
		m_pTipForm = LoadTipText(0, 0);
		if (m_pTipForm) {
			m_tipDlgState = DYNAMIC_CAST(FrGaugeBar, m_pTipForm->FindChildByName("progress"));
			if (m_tipDlgState) {
				m_tipDlgState->SetBarSpeed(200.0f);
			}
		}
	}

	if (CMouseCursor::Instance()) {
		CMouseCursor::Instance()->SetActive(true);
		CMouseCursor::Instance()->SetMode(2);
	}
}

// avatar_task.cpp:143
void ntAvatarChatTask::Load(void) {
	m_anotherHand.Work(this, 0, 0, false, 0);
}

// avatar_task.cpp:152
void ntAvatarChatTask::Init(const char *backGround) {
	if (m_pLoadingFresh) { delete m_pLoadingFresh; m_pLoadingFresh = 0; }

	g_pFresh->GetDesktop()->Init(g_view->GetWidth(), g_view->GetHeight());

	g_pFresh->GetDesktop()->SetWallPaper(backGround, true);

	CChatMsg::Instance()->SetActive(true, true, true);
}

// avatar_task.cpp:163
void ntAvatarChatTask::AddAvatarBurden(const ntAvatarBurden &burden) {
	EnterCriticalSection(&m_burdenLock);
	m_addList.push_back(burden);
	LeaveCriticalSection(&m_burdenLock);
}

// avatar_task.cpp:171
void ntAvatarChatTask::RemoveAvatarBurden(const ntAvatarBurden &burden) {
	if (m_destroying) {
		return;
	}

	EnterCriticalSection(&m_burdenLock);
	m_removeList.push_back(burden);
	LeaveCriticalSection(&m_burdenLock);
}

// avatar_task.cpp:184
void ntAvatarChatTask::DeleteBurdenPet(std::list<ntAvatarBurden> &burdens, std::list<ntAvatarBurden> &pending) {
	EnterCriticalSection(&m_burdenLock);
	for (std::list<ntAvatarBurden>::iterator pendingIt = pending.begin(); pendingIt != pending.end(); ++pendingIt) {
		burdens.push_back(*pendingIt);
	}
	pending.clear();
	LeaveCriticalSection(&m_burdenLock);

	std::list<ntAvatarBurden>::iterator it = burdens.begin();
	while (it != burdens.end()) {
		CPetFrame *pPetFrame = it->m_pPetFrame;
		if (pPetFrame != NULL && pPetFrame != (CPetFrame *)-1) {
			delete pPetFrame;
		}
		it = burdens.erase(it);
	}
}

// avatar_task.cpp:207
void ntAvatarChatTask::KeepUniqueCompleteList(const ntAvatarBurden &burden) {
	std::list<ntAvatarBurden>::iterator it;
	it = std::find(m_completeList.begin(), m_completeList.end(), burden);
	if (it != m_completeList.end()) {
		m_removeList.push_back(*it);
		m_completeList.erase(it);
	}
}

// avatar_task.cpp:222
void ntAvatarChatTask::OnAnotherHand(int nWork, void *pData) {
	std::list<ntAvatarBurden> burdens;

	while (!m_destroying) {
		EnterCriticalSection(&m_burdenLock);
		for (std::list<ntAvatarBurden>::iterator addIt = m_addList.begin(); addIt != m_addList.end(); ++addIt) {
			burdens.push_back(*addIt);
		}
		m_addList.clear();
		LeaveCriticalSection(&m_burdenLock);

		if (burdens.size() > 1) {
			burdens.reverse();
			burdens.sort();
			burdens.unique();
		}

		// Burdens whose pet frame is the -1 sentinel are only cancellations.
		std::list<ntAvatarBurden>::iterator it = burdens.begin();
		while (!m_destroying && it != burdens.end()) {
			if (it->m_pPetFrame == (CPetFrame *)-1) {
				it->m_pPetFrame = NULL;
				EnterCriticalSection(&m_burdenLock);
				KeepUniqueCompleteList(*it);
				LeaveCriticalSection(&m_burdenLock);

				unsigned long key = it->m_key;
				it = burdens.erase(it);
				if (it->m_key == key) {
					it = burdens.erase(it);
				}
			} else {
				++it;
			}
		}

		// The player's own avatar first: loading it ends the loading screen.
		if (ntAvatarManager::Instance()->m_pMyAvatar) {
			it = burdens.begin();
			while (!m_destroying && it != burdens.end()) {
				if (ntAvatarManager::Instance()->m_pMyAvatar == ntAvatarManager::Instance()->FindAvatarByGuid(it->m_key)) {
					if (it->Load()) {
						EnterCriticalSection(&m_burdenLock);

						KeepUniqueCompleteList(*it);
						m_completeList.push_back(*it);
						LeaveCriticalSection(&m_burdenLock);
					}

					it = burdens.erase(it);

					g_LoadParam.m_progress = 1;
					g_LoadParam.m_bLoaded = true;
					if (!g_SALoadingState) {
						g_SALoadingState = true;
					}
				} else {
					++it;
				}
			}
		}

		it = burdens.begin();
		while (!m_destroying && it != burdens.end()) {
			if (it->Load()) {
				EnterCriticalSection(&m_burdenLock);

				KeepUniqueCompleteList(*it);
				m_completeList.push_back(*it);
				LeaveCriticalSection(&m_burdenLock);

				if (!m_destroying) {
					Sleep(15);
				}
			}

			it = burdens.erase(it);
		}

		DeleteBurdenPet(burdens, m_removeList);

		if (!m_destroying) {
			Sleep(500);
		}
	}

	DeleteBurdenPet(burdens, m_removeList);
	DeleteBurdenPet(burdens, m_completeList);
}

// avatar_task.cpp:366
void ntAvatarChatTask::Register(void) {
	SetMainActor(AddActor("ntAvatarChatMain", "AvatarChat", 0, ""));

	AddActor("CNPC", "NPC", 0, "");

	if (IsLocalContent((localContentType_t)0x77)) {
		AddActor("CPrivateTrade", "PrivateTrade", 0, "");
	}

	AddActor(data_00a8438c, data_00a84390, 0, GetRTTI()->GetName());
}

// avatar_task.cpp:393
void ntAvatarChatTask::PreserveBack(const char *name) {
	CTask::PreserveBack(name);

	WSingleton<CChatMsg>::Instance()->Close(true, true);
}

// avatar_task.cpp:400
void ntAvatarChatTask::RestorePreserved(void) {
	if (CSharedDoc::Instance()->m_roomType == 8) {
		if (CFx::Instance()) {
			CFx::Instance()->SetActive(true);
		}
	}

	SetGameType(2);

	CTask::RestorePreserved();

	FrDesktop *pDesktop = g_pFresh->GetDesktop();
	pDesktop->SetViewFocus(true);
	pDesktop->Init(g_view->GetWidth(), g_view->GetHeight());
	pDesktop->SetWallPaper(0, true);

	CChatMsg::Instance()->ResetUiPosition();
}

// avatar_task.cpp:421
void ntAvatarChatTask::Destroy(void) {
	CTask::Destroy();

	delete CQuitWindow::Instance();
}

// avatar_task.cpp:428
void ntAvatarChatTask::ShowQuitDialog(void) {
	if (CChatMsg::Instance()->IsOpened()) {
		CChatMsg::Instance()->Close(true, true);
	} else {
		if (CQuitWindow::Instance()) {
			CQuitWindow::Instance()->Process();
		}
	}
}

// avatar_task.cpp:441
void ntAvatarChatTask::Process(float dt) {
	if (g_input->GetDown("ESCAPE", true) == 1 && !g_pFresh->GetManager()->HasEscKeyWindow()) {
		if (CSharedDoc::Instance()->GetStateTrade()) {
			// "You cannot leave the chat room while using the shop."
			std::string msg("\xbb\xf3\xc1\xa1 \xc0\xcc\xbf\xeb\xc1\xdf\xbf\xa1\xb4\xc2 \xb4\xeb\xc8\xad\xb9\xe6\xbf\xa1\xbc\xad \xb3\xaa\xb0\xa5\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			CChatMsg::Instance()->AddChatMsg(msg, 0xFFFF00, true, false);
		} else {
			ShowQuitDialog();
		}
	}

	// Hand the burdens the loading thread completed to their avatars.
	std::list<ntAvatarBurden> completed;

	EnterCriticalSection(&m_burdenLock);
	for (std::list<ntAvatarBurden>::iterator completeIt = m_completeList.begin(); completeIt != m_completeList.end(); ++completeIt) {
		completed.push_back(*completeIt);
	}
	m_completeList.clear();
	LeaveCriticalSection(&m_burdenLock);

	for (std::list<ntAvatarBurden>::iterator it = completed.begin(); it != completed.end(); ++it) {
		ntAvatarBurden &burden = *it;

		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(burden.m_key);
		if (pAvatar) {
			// avatar_task.cpp:470: the avatar previous pet frame goes back to the loading thread.
			if (pAvatar->m_pPetFrame) {
				RemoveAvatarBurden(ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, pAvatar->m_characterInfo, pAvatar->m_pPetFrame));

				pAvatar->m_pPetFrame = NULL;
			}

			pAvatar->m_burden = burden;
			pAvatar->m_burdenReady = true;
		} else {
			RemoveAvatarBurden(burden);
		}
	}

	CTask::Process(dt);

	g_pFresh->OnProcess(dt);

	CChatMsg::Instance()->ProcessWindow(dt);

	FrWnd *pKeyFocused = g_pFresh->GetManager()->GetKeyFocused();
	if (pKeyFocused && pKeyFocused->IsVisible()) {
		g_pFresh->GetManager()->SetExclusiveKey(true);
	} else {
		g_pFresh->GetManager()->SetExclusiveKey(false);
	}
}

// avatar_task.cpp:506
void ntAvatarChatTask::Display(void) {
	CTask::Display();

	if (g_pFresh->GetDesktop()) {
		g_pFresh->GetDesktop()->OnDisplay(true, false, true);
	}

	if (!CChatMsg::Instance()->HasBeenPrinted()) {
		CChatMsg::Instance()->PrintInScreen();
		CChatMsg::Instance()->DrawChatWindow();
		CChatMsg::Instance()->PrintInChatWindow();
	}

	g_pFresh->OnDisplay(true);
}

// avatar_task.cpp:523
// FIRST PASS: only the switch skeleton and the small cases are written; the
// other cases carry the line range and calls of the original.
int ntAvatarChatTask::OnPacket(WReceivedPacket &packet) {
	unsigned short type = packet.Decode2();

	g_iRecvPacketType = type;

	switch (g_iRecvPacketType) {
	case 0x46: {
		static sSlotInfo slotInfo;
		static sCharacterInfo charInfo;

		unsigned char mode = packet.Decode1();
		unsigned short check = packet.Decode2();

		if (check == 0xFFFF) {
			std::list<sSlotInfo> &players = WSingleton<CSharedDoc>::Instance()->m_roomPlayerList;

			switch (mode) {
			case 0:
			case 5: {
				if (mode == 0) {
					players.clear();
				}
				int count = packet.Decode1();

				for (int i = 0; i < count; i++) {
					packet.DecodeBuffer(&slotInfo, 0x152);

					if (!IsMassGame(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType)) {
						packet.DecodeBuffer(&charInfo, 0x1BC);
						if (IsLocalContent((localContentType_t)0x57)) {
							RequestUccInfo(charInfo);
						}
					}

					players.push_back(slotInfo);

					GetActor("AvatarChat") << MsgObject(0, 0xE4, (intptr_t)&slotInfo, (intptr_t)&charInfo, 0, 0, 0);
				}
				break;
			}

			case 7: {
				packet.Decode1();

				packet.DecodeBuffer(&slotInfo, 0x152);

				if (!IsMassGame(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType)) {
					packet.DecodeBuffer(&charInfo, 0x1BC);
					if (IsLocalContent((localContentType_t)0x57)) {
						RequestUccInfo(charInfo);
					}
				}

				std::list<sSlotInfo>::iterator it = std::find(players.begin(), players.end(), slotInfo.m_uid);
				if (it == players.end()) {
					players.push_back(slotInfo);

					GetActor("AvatarChat") << MsgObject(0, 0xE4, (intptr_t)&slotInfo, (intptr_t)&charInfo, 0, 0, 0);
				}
				break;
			}

			case 1: {
				packet.DecodeBuffer(&slotInfo, 0x152);

				if (!IsMassGame(WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_gameType)) {
					packet.DecodeBuffer(&charInfo, 0x1BC);
					if (IsLocalContent((localContentType_t)0x57)) {
						RequestUccInfo(charInfo);
					}
				}

				std::list<sSlotInfo>::iterator it = std::find(players.begin(), players.end(), slotInfo.m_uid);
				if (it == players.end()) {
					players.push_back(slotInfo);

					GetActor("AvatarChat") << MsgObject(0, 0xE4, (intptr_t)&slotInfo, (intptr_t)&charInfo, 0, 0, 0);
				}
				break;
			}

			case 2: {
				unsigned long uid = packet.Decode4();

				std::list<sSlotInfo>::iterator it = std::find(players.begin(), players.end(), uid);

				if (it != players.end()) {
					players.erase(it);

					GetActor("AvatarChat") << MsgObject(0, 0xE5, uid, 0, 0, 0, 0);
				}
				break;
			}

			case 3: {
				unsigned long uid = packet.Decode4();

				packet.DecodeBuffer(&slotInfo, 0x152);

				std::list<sSlotInfo>::iterator it = std::find(players.begin(), players.end(), uid);
				if (it != players.end()) {
					GetActor("AvatarChat") << MsgObject(0, 0xE6, uid, (intptr_t)&slotInfo, 0, 0, 0);
				}
				break;
			}
			}

			WSingleton<CSharedDoc>::Instance()->m_roomInfo.m_userCount = (unsigned char)players.size();
		}
		break;
	}

	case 0x7A: {
		unsigned long guid = packet.Decode4();
		unsigned short value = packet.Decode2();
		GetActor("AvatarChat") << MsgObject(0, 0x5F, guid, value, 0, 0, 0);
		break;
	}

	case 0x7F: {
		std::string nickName;
		packet.DecodeStr(nickName);
		// "<nick>: you are about to force this player out.\nDo you agree?"
		nickName += "\xb4\xd4\xc0\xbb \xb0\xad\xc1\xa6 \xc5\xf0\xc0\xe5\xbd\xc3\xc5\xb0\xb7\xc1 \xc7\xd5\xb4\xcf\xb4\xd9.\n\xc2\xf9\xbc\xba\xc7\xcf\xbd\xca\xb4\xcf\xb1\xee?";
		GetActor("AvatarChat") << MsgObject(0, 0x31, (intptr_t)nickName.c_str(), packet.Decode4(), 0, 0, 0);
		break;
	}

	case 0x3F: {
		std::string text;
		packet.DecodeStr(text);
		GetActor("avatarChatMain") << MsgObject(0, 3, (intptr_t)text.c_str(), 0xFFFF7878, 0, 0, 0);
		break;
	}

	case 0x3E: {
		std::string nickName;
		std::string message;
		unsigned char chatType = packet.Decode1();
		std::string text;

		bool bWhisper = (chatType >> 7) != 0;
		chatType &= 0x7F;

		bool bCommand = false;
		bool bBalloon = false;
		unsigned long color = 0;
		packet.DecodeStr(nickName);
		packet.DecodeStr(message);

		WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(message.c_str(), false);

		switch (chatType) {
		case 0: {
			ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByNickName(nickName.c_str());
			if (pAvatar) {
				ntAvatar *pMyAvatar = ntAvatarManager::Instance()->m_pMyAvatar;
				if (pMyAvatar && (pAvatar->m_position - pMyAvatar->m_position).Magnitude() > 256.0f) {
					return 1;
				}
			}

			if (strcmp(nickName.c_str(), WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname) == 0) {
				color = 0xFFFFCC00;
			} else {
				std::list<std::string>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(), WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(), nickName);
				if (it != WSingleton<CSharedDoc>::Instance()->m_ignoreList.end()) {
					return 1;
				}

				color = 0xFFFFFFFF;
			}

			if (message.c_str()[0] == '!') {
				bCommand = true;
			}

			pAvatar = ntAvatarManager::Instance()->FindAvatarByNickName(nickName.c_str());
			if (pAvatar) {
				pAvatar->SetBalloonText(message.c_str());
			}

			bBalloon = true;
			if (bWhisper) {
				color = 0xFF6EECFB;
			} else {
				pAvatar = ntAvatarManager::Instance()->FindAvatarByNickName(nickName.c_str());
				if (pAvatar) {
					pAvatar->SetBalloonText(message.c_str());
				}

				GetActor("AvatarChat") << MsgObject(0, 0x60, (intptr_t)nickName.c_str(), (intptr_t)message.c_str(), 0, 0, 0);
			}

			text = nickName + " : \x08" + message;
			break;
		}

		case 1:
			// "Notice : <nick> is not in this channel."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xc3\xa4\xb3\xce\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 5:
			// "Notice : <nick> is not on this server."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 2:
			// "Notice : <nick> has left."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 3:
			// "The kick vote was rejected."
			text = "\xb0\xad\xc5\xf0\xbf\xa1 \xb0\xfc\xc7\xd1 \xc5\xf5\xc7\xa5\xb0\xa1 \xba\xce\xb0\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 4:
			// "Notice : <nick> cannot receive whispers now."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xbb \xbc\xf6\xbd\xc5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 6:
			// "Notice : <nick> is not connected."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xc1\xa2\xbc\xd3\xc1\xdf\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			break;

		case 7:
			text = "\xbe\xcb\xb8\xb2 : " + message;
			color = 0xFFFF7878;
			break;

		case 10:
			// "Notice : <nick> won a <n> pang pouch."
			text = "\xbe\xcb\xb8\xb2 : " + std::string(MakeStr("%s\xb4\xd4\xb2\xb2\xbc\xad %d \xc6\xce \xc1\xd6\xb8\xd3\xb4\xcf\xb8\xa6 \xc8\xb9\xb5\xe6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", nickName.c_str(), packet.Decode4()));
			color = 0xFFFF7878;
			break;

		default:
			text = nickName + " : " + message;
			break;
		}

		if (chatType != 0 || (!bCommand && WSingleton<CSharedDoc>::Instance()->m_pGolfDoc->IsShowChat())) {
			CChatMsg::Instance()->AddChatMsg(text, color, true, bBalloon);
		}
		break;
	}

	case 0x82: {
		unsigned char chatType = packet.Decode1();
		bool bWhisper = (chatType >> 7) != 0;
		chatType &= 0x7F;

		bool bMine = (chatType == 0);

		unsigned long color = 0xFFD2A2FF;
		if (bWhisper) {
			color = 0xFF6EECFB;
		}

		std::string nickName, message;
		std::string text;

		packet.DecodeStr(nickName);
		packet.DecodeStr(message);

		if (bMine) {
			text = WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname;
			text += ">";
			text += nickName;
		} else {
			if (!COption::Instance()->gIsWhisperAllowed()) {
				break;
			}

			std::list<std::string>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(), WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(), nickName);
			if (it != WSingleton<CSharedDoc>::Instance()->m_ignoreList.end()) {
				break;
			}

			// The receiver writes its answer into the slot (never read here).
			int reply;
			GetActor("AvatarChat") << MsgObject(0, 0x3F, (intptr_t)&reply, 0, 0, 0, 0);
			GetActor("AvatarChat") << MsgObject(0, 0x38, (intptr_t)nickName.c_str(), 0, 0, 0, 0);

			text = nickName + ">" + WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname;
		}

		WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(message.c_str(), false);

		text += " : \x08'";
		text += message;
		text += "'";

		if (AfxGetTask()->IsRecieveWhisper()) {
			CChatMsg::Instance()->AddChatMsg(text, color, true, true);
		} else {
			WSendPacket send(GCT_WHISPEROFF_TASK);
			send.EncodeStr(nickName);
			send.Send(SEND_TO_DEFAULT);
		}
		break;
	}

	case 0xC2: {
		unsigned long guid = packet.Decode4();
		int subType = packet.Decode1();

		switch (subType) {
		case 4: {
			float x, y, z;
			packet.DecodeBuffer(&x, 4);
			packet.DecodeBuffer(&y, 4);
			packet.DecodeBuffer(&z, 4);
			GetActor("AvatarChat") << MsgObject(0, 0x5B, guid, subType, (intptr_t)&x, (intptr_t)&y, (intptr_t)&z);
			break;
		}

		case 5: {
			unsigned long value = packet.Decode4();
			GetActor("AvatarChat") << MsgObject(0, 0x5B, guid, subType, value, 0, 0);
			break;
		}

		case 6: {
			float x, y, z;
			packet.DecodeBuffer(&x, 4);
			packet.DecodeBuffer(&y, 4);
			packet.DecodeBuffer(&z, 4);
			GetActor("AvatarChat") << MsgObject(0, 0x5B, guid, subType, (intptr_t)&x, (intptr_t)&y, (intptr_t)&z);
			break;
		}

		case 7: {
			std::string text;
			packet.DecodeStr(text);
			GetActor("AvatarChat") << MsgObject(0, 0x5B, guid, subType, (intptr_t)text.c_str(), 0, 0);
			break;
		}

		case 8: {
			unsigned long value = packet.Decode4();
			GetActor("AvatarChat") << MsgObject(0, 0x5B, guid, subType, value, 0, 0);
			break;
		}

		case 9:
			GetActor("AvatarChat") << MsgObject(0, 0x5B, guid, subType, 0, 0, 0);
			break;
		}
		break;
	}

	case 0x9C: {
		unsigned char weather = packet.Decode1();
		packet.Decode1();
		packet.Decode1();

		CSceneManager::Instance()->SetWeather(weather, false);
		break;
	}

	case 0x8C: {
		unsigned long guid = packet.Decode4();
		unsigned char value = packet.Decode1();
		GetActor("AvatarChat") << MsgObject(0, 0x3D, guid, value, 0, 0, 0);
		break;
	}

	case 0x76: {
		unsigned long guid = packet.Decode4();
		unsigned char value = packet.Decode1();

		GetActor("AvatarChat") << MsgObject(0, 0x29, guid, value, 0, 0, 0);
		break;
	}

	case 0x92:
		GOLFDOC()->m_reportSent = packet.Decode1() == 1;

		if (!GOLFDOC()->IsReported()) {
			break;
		}

		if (GOLFDOC()->m_reportSent) {
			// "A report has already been filed for the current game."
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage("\xc7\xf6\xc0\xe7 \xb0\xd4\xc0\xd3\xbf\xa1 \xb4\xeb\xc7\xd8\xbc\xad \xc0\xcc\xb9\xcc \xbd\xc5\xb0\xed\xb0\xa1 \xc1\xa2\xbc\xf6\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 4);

			GOLFDOC()->SetReport(false);
		} else {
			// "The report was filed. To block a player's chat, double-click
			// their info window and pick [block chat]." (this copy keeps the
			// source's continuation-line tabs, unlike GolfTask's)
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage("\xbd\xc5\xb0\xed\xb0\xa1 \xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xc1\xa2\xbc\xf6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.\n\n\t\t\t\t\t\t\t\t\t  \xc6\xaf\xc1\xa4 \xbb\xf3\xb4\xeb\xc0\xc7 \xb4\xeb\xc8\xad\xb8\xa6 \xc2\xf7\xb4\xdc \xc7\xcf\xb7\xc1\xb8\xe9 \xbb\xf3\xb4\xeb \xc1\xa4\xba\xb8\xc3\xa2\xc0\xbb \xb8\xb6\xbf\xec\xbd\xba \xb4\xf5\xba\xed \xc5\xac\xb8\xaf \xc8\xc4 \\c0xffff0000\\c[\xb4\xeb\xc8\xad \xc2\xf7\xb4\xdc \xbe\xc6\xc0\xcc\xc4\xdc]\\c0xff000000\\c\xc0\xbb \xbc\xb1\xc5\xc3 \xc7\xcf\xbd\xc3\xb8\xe9 \xb5\xcb\xb4\xcf\xb4\xd9.", false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
		}
		break;

	case 0x258:
		break;

	case 0x25D: {
		unsigned long guid = packet.Decode4();
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar) {
			for (int i = 0; i < 5; i++) {
				SpecialPrizeItem &item = pAvatar->GetMySpItem((eSPAVILITYTYPE)i);

				float value = 0;
				packet.DecodeBuffer(&value, 4);
				item.setvalue(value);
			}
		}
		break;
	}

	default:
		return OnPacketCommon(packet);
	}

	return 1;
}
