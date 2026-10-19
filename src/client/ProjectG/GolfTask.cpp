// Original: d:\build\custom\temp\code\client\projectg\golftask.cpp
// The original holds the out-of-line copy of CMouseCursor::SetTempActive
// (mousecursor.h) in this object.
#define GOLFTASK_MOUSECURSOR_INLINE
// ... and of CTask::IsInited (taskmanager.h).
#define GOLFTASK_TASKMANAGER_INLINE
// ... and of CGolfDoc::IsReported (golfdoc.h).
#define GOLFTASK_GOLFDOC_INLINE

#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer

#include <algorithm>

#include "GolfTask.h"

#include "ApproachDesc.h"
#include "BonusTicket.h"

#include "CapturedBg.h"
#include "exhibition.h"
#include "cardsystem.h"
#include "ChatMsg.h"
#include "ClientSetting.h"
#include "GolfDoc.h"
#include "ObjectFactory.h"
#include "Packet.h"
#include "QuitWindow.h"
#include "SharedDoc.h"
#include "SoundManager.h"
#include "actor.h"
#include "club.h"
#include "commonutil.h"
#include "fresh.h"
#include "frdesktop.h"
#include "frgraphicinterface.h"
#include "frwndmanager.h"
#include "guilddefine.h"
#include "hackingmanager.h"
#include "InputManager.h"
#include "Localize.h"
#include "LoginInfo.h"
#include "messengerdlg.h"
#include "OptionDlg.h"
#include "ProjectG.h"
#include "TaskManager.h"
#include "mousecursor.h"
#include "wview.h"
#include "headicon.h"
#include "user_info.h"
#include "UccLibrary.h"
#include "wind.h"
#include "quadtree.h"
#include "golfball.h"
#include "autotarget.h"

// shareddoc.inl (the overload without a game type, 0x40fce0; not declared
// in SharedDoc.h yet).
int IsMassGame(void);

extern int g_iRecvPacketType;

// File-level actor class and name strings (data 0xa7e3b0..0xa7e3c0; their
// header of origin is not recovered yet).
static const char* s_guildActorClass = "CGuildActor";
static const char* s_guildActorName = "GuildActor";
static const char* s_gatewayActorClass = "CGatewayActor";
static const char* s_gatewayActorName = "Gateway";

// The player state message 0x40 sends after the round (case 0x97) and
// message 0xBB hands back for every player (golftask.cpp:1590-1654,
// 1828-1913). Its header of origin is not recovered (/Zd).
#pragma pack(push, 1)
struct sPlayerSyncData {
	float angle;                    // 0x00, CGolfDoc::m_cameraData.m_yaw / GameCamera().m_yaw
	WVector lastShotPos;            // 0x04, sPlayerData::m_lastShotPos
	WVector ballPos;                // 0x10, sPlayerData::m_ballPos
	unsigned char holes[18];        // 0x1C, sPlayerData::m_holes
	unsigned char holeIn[18];       // 0x2E, sPlayerData::m_holeIn
	__int64 holeRecord[18];         // 0x40, sPlayerData::m_holeRecord
	unsigned long ghostResult;      // 0xD0, sPlayerData::m_ghostResult
	unsigned char totalScore;       // 0xD4, sPlayerData::m_totalScore
	int ghostAward;                 // 0xD5, sPlayerData::m_ghostAward
	PADDING(0xD9, 0xDD);
	unsigned char numItems;         // 0xDD, sPlayerData::m_numItems
	unsigned long shotItemTypeId;   // 0xDE, sPlayerData::m_shotItemTypeId
	float comboGauge;               // 0xE2, sPlayerData::m_comboGauge
	unsigned char powerShotLevel;   // 0xE6, sPlayerData::m_powerShotLevel
	unsigned char rivalLevel;       // 0xE7, sPlayerData::m_rivalLevel
	unsigned char powerShot;        // 0xE8, CClub::PowerShotType
	unsigned char club;             // 0xE9, CClub::GetIndex
	unsigned long frame;            // 0xEA, CQuadTree::GetFrame
	unsigned char state;            // 0xEE, sPlayerData::m_state (mass game: the ball has pre-data)
	__int64 extraPang;              // 0xEF, sPlayerData::m_extraPang
};
#pragma pack(pop)
CHECK_SIZE(sPlayerSyncData, 0xF7);
CHECK_FIELD(sPlayerSyncData, holes, 0x1C);
CHECK_FIELD(sPlayerSyncData, holeRecord, 0x40);
CHECK_FIELD(sPlayerSyncData, numItems, 0xDD);
CHECK_FIELD(sPlayerSyncData, comboGauge, 0xE2);
CHECK_FIELD(sPlayerSyncData, frame, 0xEA);
CHECK_FIELD(sPlayerSyncData, extraPang, 0xEF);

// clientsetting.h:422
int COption::gGetCutinFlag(void) {
	return m_option.m_cutinFlag;
}

// golftask.cpp:75 (run-time type and factory registration of CGolfTask)
const WRTTI CGolfTask::m_RTTI("CGolfTask", &CTask::m_RTTI);

IObject* CGolfTaskMakeInstance(void) {
	return new CGolfTask;
}

struct __sCGolfTask {
	__sCGolfTask(void) {
		ObjectFactory().AddObjectFunctor(CGolfTaskMakeInstance, "CGolfTask");
	}
};
static __sCGolfTask s_registerCGolfTask;

// golftask.cpp:79
CGolfTask::CGolfTask(void) {
	m_unknown38 = false;
	IActor::m_defaultSkipLayer = 0x622021;
	m_unknown208 = -10.0f;
}

// golftask.cpp:99
void CGolfTask::Init(const char *backGround) {
	CTask::Init(0);
}

// golftask.cpp:104
void CGolfTask::Register(void) {
	SetMainActor(AddActor("CGolfRule", "GolfRule", 0x20021, ""));

	AddActor("CClock", "Clock", 0x20, "");
	AddActor("CGallery", "Gallery", 0, "");
	AddActor("CCamera", "Camera", 0x20, "");

	AddActor("CGolfBg", "GolfBg", 0x22001, "");

	AddActor("CGolfPlayer", "Player", 0x22000, "");
	AddActor("CGolfCaddie", "Caddie", 0x2000, "");
	AddActor("CMascot", "Mascot", 0x2000, "");
	AddActor("CNPC", "NPC", 0x2001, "");

	AddActor("CGrid", "Grid", 0x20, "");
	AddActor("CSpot", "Spot", 0x20, "");
	AddActor("CBall", "Ball", 0x2000, "");

	AddActor("CGuideLine", "GuideLine", 0x20, "");
	AddActor("CTrajectory", "Trajectory", 0, "");
	AddActor("CParticleEffect", "Effect", 0x20001, "");
	AddActor("CFx", "Fx", 0x20001, "");

	AddActor("CAmbientSound", "Ambient", 1, "");

	AddActor("CBubble", "Bubble", 0x20, "");
	AddActor("CLetter", "Letter", 0x20020, "");
	AddActor("CScreen", "Screen", 0x20, "");
	AddActor("CItemWindow", "Item", 0x2000, "");
	AddActor("CSpin", "Spin", 0, "");
	AddActor("CLogo", "Logo", 1, "");
	AddActor("CScore", "Score", 0x222020, "");
	AddActor("CGuideBook", "GuideBook", 0x22020, "");

	AddActor(s_guildActorClass, s_guildActorName, 0, "");

	AddActor(s_gatewayActorClass, s_gatewayActorName, 0, GetRTTI()->GetName());

	CTask *pPreservedTask = AfxGetPreservedTask();
	if (pPreservedTask) {
		IActor *pTutorial = pPreservedTask->GetActor("TutorialMain");
		if (pTutorial) {
			const char *className = 0;
			const char *actorName = 0;
			pTutorial << MsgObject(0, 0x14E, (intptr_t)&className, (intptr_t)&actorName, 0, 0, 0);
			AddActor(className, actorName, 0x20, "");
		}
	} else {
		AddActor("CCutin", "Cutin", 0, "");
	}

	AddActor("ShotPreview", "ShotPreview", 0, "");

	if (DetermineWhetherToAddActor_GroundItemMan())
		AddActor("GroundItemMan", "GroundItemMan", 0, "");
	AddActor("CMagicCarpetHelper", "MagicCarpetHelper", 0, "");
}

// golftask.cpp:213
void CGolfTask::Load(void) {
	IActor::m_pLayer = &GOLFDOC()->GameMode();

	new CQuitWindow;

	SetLoadingBackGround("loading_background.jpg");

	g_pFresh->GetDesktop()->SetViewFocus(true);

	CChatMsg::Instance()->ResetUiPosition();

	CTask::Load();
}

// golftask.cpp:228
void CGolfTask::Destroy(void) {
	CTask::Destroy();

	g_pFresh->GetDesktop()->SetWallPaper("gameroom_background.jpg", true);
	g_pFresh->GetDesktop()->SetBgColor(0xFFFFFFFF);
	CMouseCursor::Instance()->SetTempActive(false);

	if (CQuitWindow::Instance())
		delete CQuitWindow::Instance();
}

// golftask.cpp:240
void CGolfTask::Process(float dt) {
	if (HackingManager::Instance()) {
		HackingManager::Instance()->ResetCrypticValue(0x14);
	}

	if (CSharedDoc::Instance()->m_playingMode == (eReplayModeType)1) {
		// Escape: the item window, the chat window, the messenger, then the quit window
		if (g_input->GetDown("ESCAPE", true) == 1 && !g_pFresh->GetManager()->HasEscKeyWindow()) {
			int itemState = 0;
			GetActor("Item") << MsgObject(0, 0x199, (intptr_t)&itemState, 0, 0, 0, 0);

			if (CChatMsg::Instance()->GetOpenState() == 3) {
				CChatMsg::Instance()->Close(false, true);
			} else if (itemState != 3) {
				GetActor("Item") << MsgObject(0, 0x195, 0, 0, 0, 0, 0);
			} else if (CMessengerInfo::Instance()->Close(false)) {
				if (CQuitWindow::Instance()) {
					CQuitWindow::Instance()->Process();
				}
			}
		}
	}

	CSharedDoc::Instance()->m_chatManager.CheckPenaltyTime();

	if (CChatMsg::Instance()->GetOpenState() != 3) {
		unsigned char roomType = CSharedDoc::Instance()->m_roomType;
		if (roomType != 0xB && roomType != 0xC) {
			if (!g_pFresh->GetManager()->IsKeyExclusive() && (g_input->GetDown("ENTER", true) || g_input->GetDown("PADENTER", true))) {
				if ((g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) && (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true))) {
					// "\xC6\xC0\xBF\xA1\xB0\xD4": to the team
					GetActor("GolfRule") << MsgObject(0, 0x37, (intptr_t)"\xC6\xC0\xBF\xA1\xB0\xD4", 0, 0, 0, 0);
				} else if (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true)) {
					// "\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4": to everyone
					GetActor("GolfRule") << MsgObject(0, 0x37, (intptr_t)"\xB8\xF0\xB5\xCE\xBF\xA1\xB0\xD4", 0, 0, 0, 0);
				} else if (g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) {
					GetActor("GolfRule") << MsgObject(0, 0x65, 0, 0, 0, 0, 0);
				}

				CChatMsg::Instance()->Open(true, true);
			}
		}
	}

	// "\xB8\xC5\xC5\xA9\xB7\xCE\xC3\xA2": the macro window key
	if (g_input->GetDown("\xB8\xC5\xC5\xA9\xB7\xCE\xC3\xA2", true) && !g_input->Get("LALT", true) && !g_input->Get("RALT", true) && !g_input->Get("LCONTROL", true) && !g_input->Get("RCONTROL", true)) {
		FrOptionDlg *pOptionDlg = CreateForm<FrOptionDlg>(g_pFresh->GetManager(), this, "option", 0);
		if (pOptionDlg) {
			pOptionDlg->Open(0, 1);
			pOptionDlg->SetApplyScrSize(true);
			pOptionDlg->SetCloseResult(FrCANCEL);
			pOptionDlg->OpenTab(FrOptionDlg::TAB_MACRO);
		}
	}

	float taskDt = dt;
	if (IsLocalContent((localContentType_t)0x4F) && CSharedDoc::Instance()->m_playingMode == (eReplayModeType)2) {
		if (CSharedDoc::Instance()->m_replayState == 4) {
			taskDt = 0;
		}
	}

	CTask::Process(taskDt);

	g_pFresh->OnProcess(dt);

	CChatMsg::Instance()->ProcessWindow(dt);

	if (m_unknown208 > 0.0f) {
		m_unknown208 -= dt;

		if (m_unknown208 < 0.1f) {
			m_unknown208 = -10.0f;

			CSharedDoc::Instance()->m_mySlot = 0xFFFF;
			CProjectG::Instance()->FromGolfToLobbySettings();
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
			CTaskManager::Instance()->PostMsg(0, "Lobby", 0, (intptr_t)"ROOMLIST", 0, 0, 0);
			CTaskManager::Instance()->PostMsg(0, "Lobby", 0x1A, 0, 0, 0, 0);

			switch (CSharedDoc::Instance()->m_leaveReason) {
			case 1:
				if (CSharedDoc::Instance()->m_roomType != 10) {
					CTaskManager::Instance()->PostMsg(0, "Lobby", 0x23, (intptr_t)"\xC1\xA2\xBC\xD3\xC0\xCC \xBA\xD2\xBE\xC8\xC7\xCF\xBF\xA9 \xB0\xD4\xC0\xD3\xBF\xA1\xBC\xAD \xC5\xF0\xC0\xE5\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0);
				}
				break;
			case 2:
				CTaskManager::Instance()->PostMsg(0, "Lobby", 0x23, (intptr_t)"\xC5\xB8\xC0\xD3\xBE\xC6\xBF\xF4 3\xB9\xF8\xC0\xB8\xB7\xCE \xBD\xC7\xB0\xDD\xB5\xC7\xBE\xEE \xC5\xF0\xC0\xE5\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0);
				break;
			case 4:
				CTaskManager::Instance()->PostMsg(0, "Lobby", 0x23, (intptr_t)"\xB1\xE2\xBA\xEA\xBE\xF7 3\xB9\xF8\xC0\xB8\xB7\xCE \xBD\xC7\xB0\xDD\xB5\xC7\xBE\xEE \xC5\xF0\xC0\xE5\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0);
				break;
			case 8:
				CTaskManager::Instance()->PostMsg(0, "Lobby", 0x23, (intptr_t)"\xB0\xD4\xC0\xD3\xC1\xF8\xC7\xE0\xBF\xA1 \xC7\xCA\xBF\xE4\xC7\xD1 \xC6\xCE\xC0\xCC \xBE\xF8\xBE\xEE\xBC\xAD \xC5\xF0\xC0\xE5\xB5\xC7\xBE\xFA\xBD\xC0\xB4\xCF\xB4\xD9.", 0, 0, 0);
				break;
			}
		}
	}
}

// golftask.cpp:387
void CGolfTask::Display(void) {
	CTask::Display();

	if (GOLFDOC()->GameMode() & 0x400000) {
		GetActor("Score") << MsgObject(0, 0x12F, 3, 0, 0, 0, 0);
		GetActor("GolfRule") << MsgObject(0, 0x78, 2, 0, 0, 0, 0);
	}

	if (g_pFresh->GetDesktop()) {
		g_pFresh->GetDesktop()->OnDisplay(true, false, true);
	}

	if (!CChatMsg::Instance()->HasBeenPrinted()) {
		CChatMsg::Instance()->PrintInScreen();
		CChatMsg::Instance()->DrawChatWindow();
		CChatMsg::Instance()->PrintInChatWindow();
	}

	if (!CCapturedBg::Instance()->IsCaptured() || GOLFDOC()->m_gameState != 15) {
		g_pFresh->OnDisplay(false);
	}

	if (GOLFDOC()->GameMode() & 0x10) {
		if (CCapturedBg::Instance()) {
			CCapturedBg::Instance()->Capture();
		}
	}

	if ((GOLFDOC()->GameMode() & 1) && OnlinePlay() && !IsMassGame()) {
		DrawLoadingInfo(g_pFresh);
	}
}

// golftask.cpp:433
void CGolfTask::OnDisplayWhileLoading(void) {
	CTask::OnDisplayWhileLoading();
	OnlinePlay();
}

// golftask.cpp:444
void CGolfTask::UpdateLoadingInfo(unsigned long uid, unsigned char progress) {
	std::list<sSlotInfo>::iterator it = std::find(CSharedDoc::Instance()->m_roomPlayerList.begin(), CSharedDoc::Instance()->m_roomPlayerList.end(), uid);
	if (it != CSharedDoc::Instance()->m_roomPlayerList.end()) {
		m_loadingInfo[it->m_uid] = progress;
	}
}

// golftask.cpp:451
void CGolfTask::DrawLoadingInfo(Fresh *pFresh) {
	FrGraphicInterface *pGDI = pFresh->GetManager()->GetGDI();
	if (pGDI == 0) {
		return;
	}

	int offset;
	float y = g_view->GetHeight() - 100.0f;

	std::list<sSlotInfo>::iterator it;
	for (it = CSharedDoc::Instance()->m_roomPlayerList.begin(), offset = 0; it != CSharedDoc::Instance()->m_roomPlayerList.end(); ++it) {
		sSlotInfo &player = *it;
		if (player.m_uid == CSharedDoc::Instance()->GetPangyaUserInfo().guid) {
			continue;
		}

		unsigned char progress = m_loadingInfo[player.m_uid];

		pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
		pGDI->SetTextStyle(2);

		char *text;
		if (progress == 10) {
			text = MakeStr("%s\xb4\xd4\xc0\xba \xb7\xce\xb5\xf9 \xbf\xcf\xb7\xe1 \xc0\xd4\xb4\xcf\xb4\xd9.", player.m_nickname);
		} else {
			text = MakeStr("%s\xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 %d%% \xb7\xce\xb5\xf9 \xc1\xdf \xc0\xd4\xb4\xcf\xb4\xd9.", player.m_nickname, progress * 10);
		}

		pFresh->GetManager()->PrintText(WPoint(20.0f, y + offset), 0, text, -1.0f, 0xFFFFFFFF);
		offset += 20;
	}
}

// golftask.cpp:492
void CGolfTask::OnInitFinished(void) {
	if (OnlinePlay()) {
		WSendPacket packet(GCT_LOAD_OK);
		packet.Send(SEND_TO_DEFAULT);
	}

	if (CChatMsg::Instance()) {
		CChatMsg::Instance()->Close(true, true);
		CChatMsg::Instance()->EnableCaretMove(true);
	}

	GetActor("GolfRule") << MsgObject(0, 0x29, 0, 0, 0, 0, 0);

	if (GalleryMode()) {
		switch (CSharedDoc::Instance()->m_roomType) {
		case 4:
		case 5:
		case 6:
		case 9:
		case 10: {
			WSendPacket packet(GCT_CHANGE_TARGET);
			packet.Encode4(CSharedDoc::Instance()->m_roomPlayers[0].m_guid);
			packet.Send(SEND_TO_DEFAULT);
			break;
		}
		}
	}
}

// golftask.cpp:527
// FIRST PASS: the switch carries the cases in source-line order; the cases
// not written yet (they fall to OnPacketCommon until then) are listed in the
// worker report.
int CGolfTask::OnPacket(WReceivedPacket &packet) {
	unsigned short type = packet.Decode2();

	g_iRecvPacketType = type;

	switch (g_iRecvPacketType) {
	case 0xF2: {
		unsigned char screenMode = packet.Decode1();
		GetActor("Screen") << MsgObject(0, 0x1CF, screenMode, 0, 0, 0, 0);
		break;
	}
	case 0xC6:
		if (IsLocalContent((localContentType_t)0x87)) {
			__int64 pang = packet.Decode8();
			__int64 calc = packet.Decode8();

			if (calc != 0) {
				if (CSharedDoc::Instance()->m_myPang == pang + calc) {
					CSharedDoc::Instance()->m_myPang = pang;
				} else {
					WSendPacket report((enumClientPacket)0x33);

					std::string text;
					char buf[128];
					sprintf(buf, "[Pang Hacker] Old Pangya %I64d, Recive  %I64d , Calculation %I64d", CSharedDoc::Instance()->m_myPang, pang, calc);
					text = buf;

					report.Encode1(3);
					report.EncodeStr(text);
					report.Send(SEND_TO_DEFAULT);
					break;
				}
			} else {
				CSharedDoc::Instance()->m_myPang = pang;
			}

			CSharedDoc::Instance()->m_roomPlayers[MyIndex()].m_pang = pang;
		} else {
			__int64 pang = packet.Decode8();
			CSharedDoc::Instance()->m_myPang = pang;
			CSharedDoc::Instance()->m_roomPlayers[MyIndex()].m_pang = pang;
		}
		break;
	case 0xC5: {
		float gameSpeed;
		packet.DecodeBuffer(&gameSpeed, 4);
		WSingleton<CProjectG>::Instance()->SetGameSpeed(gameSpeed);

		unsigned long uid = packet.Decode4();

		if (uid == MyGuid(false)) {
			if (!CLoginInfo::Instance()->IsPcBang()) {
				CSharedDoc* pDoc = CSharedDoc::Instance();
				std::list<sItemInfo>::iterator it = std::find(pDoc->m_itemInfoList.begin(), pDoc->m_itemInfoList.end(), 0x1A000011);

				if (it != pDoc->m_itemInfoList.end()) {
					if (it->m_stateBits.kind) {
						if (it->m_remainHours <= 0) {
							pDoc->m_itemInfoList.erase(it);
						}
					} else {
						it->m_count--;

						if (it->m_count == 0) {
							CSharedDoc::Instance()->m_itemInfoList.erase(it);
						}
					}
				}
			}
		}

		GetActor("Letter") << MsgObject(0, 0x193, 0, 0, 0, 0, 0);
		break;
	}
	case 0x52:
	case 0x65:
		if (CMessengerInfo::Instance()->GetMyStatus() != MSN::STATUS_BUSY) {
			CMessengerInfo::Instance()->SetMyStatus(MSN::STATUS_ONLINE);
		}

		GetActor("GolfRule") << MsgObject(0, 0x8F, 1, 0, 0, 0, 0);
		break;
	case 0x113:
		if (IsLocalContent((localContentType_t)0x32)) {
			unsigned char tickets[100] = { 0 };
			std::string name;
			packet.DecodeStr(name);

			packet.DecodeBuffer(tickets, 100);
			CBonusTicket::Instance().SetTicketFromServer(name, tickets, 100);
		}
		break;
	case 0x51: {
		unsigned long uid = packet.Decode4();

		GetActor("GolfRule") << MsgObject(0, 0x243, 0, 0, 0, 0, 0);

		if (IsMassGame()) {
			GetActor("GolfRule") << MsgObject(0, 0x81, 0, 0, 0, 0, 0);
			break;
		}

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			GetActor("GolfRule") << MsgObject(0, 0x81, CSharedDoc::Instance()->GetIndex(uid), 0, 0, 0, 0);
		}

		if (IsLocalContent((localContentType_t)0x28)) {
			CMatchHistory::Instance()->RequestHistory();
		}
		break;
	}
	case 0x9C: {
		unsigned char bgType = packet.Decode1();
		packet.Decode1();
		packet.Decode1();

		GetActor("GolfBg") << MsgObject(0, 0x142, bgType, 0, 0, 0, 0);
		break;
	}
	case 0x59: {
		unsigned char intensity = packet.Decode1();
		unsigned char windColor = packet.Decode1();
		WSingleton<CCardManager>::Instance()->SetWindColor(windColor);
		unsigned char direction = packet.Decode1();
		unsigned char flag = packet.Decode1();
		unsigned char enabled = packet.Decode1();

		Wind().m_windEnabled = true;

		Wind().Init(intensity, direction, flag, enabled);
		break;
	}
	case 0x53: {
		unsigned long uid = packet.Decode4();
		sShotProtocol shot;
		packet.DecodeBuffer(&shot, sizeof(shot));

		packet.GetRemainLength();

		GetActor("GroundItemMan") << MsgObject(0, 0x279, (intptr_t)&packet, 0, 0, 0, 0);

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			GOLFDOC()->m_shotProtocol = shot;
			GetActor("GolfRule") << MsgObject(0, 0x7A, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x54: {
		unsigned long uid = packet.Decode4();
		unsigned long cameraData;
		packet.DecodeBuffer(&cameraData, 4);

		if (uid == MyGuid(false)) {
			break;
		}

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			GetActor("Camera") << MsgObject(0, 0xF2, (intptr_t)&cameraData, 0, 0, 0, 0);
		}
		break;
	}
	case 0x55: {
		unsigned long uid = packet.Decode4();
		unsigned char data[5];
		packet.DecodeBuffer(data, 5);

		if (uid == MyGuid(false)) {
			break;
		}

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			memcpy(GOLFDOC()->m_zeroedBytes46E, data, 5);
			GetActor("GolfRule") << MsgObject(0, 0x7B, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x56: {
		unsigned long uid = packet.Decode4();
		unsigned char value = packet.Decode1();

		if (uid == MyGuid(false)) {
			break;
		}

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			GetActor("Player") << MsgObject(0, 0x11F, value, 0, 0, 0, 0);
		}
		break;
	}
	case 0x57: {
		unsigned long uid = packet.Decode4();
		unsigned char clubIndex = packet.Decode1();

		if (uid == MyGuid(false)) {
			break;
		}

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			GolfClub().SetClub(clubIndex);
			g_audio->PlaySfx("\xc5\xac\xb7\xb4\xb1\xb3\xc3\xbc", 0, 0, 0, 0, 0.5f, 200.0f);
		}
		break;
	}
	case 0x58: {
		unsigned long uid = packet.Decode4();
		unsigned long itemId = packet.Decode4();

		// The result is not used (the body is compiled out).
		IsLocalContent((localContentType_t)0x51);

		if (OnlinePlay()) {
			unsigned long playerUid = packet.Decode4();

			if (!CSharedDoc::Instance()->AreWePlayingTogether(playerUid)) {
				break;
			}

			GetActor("Item") << MsgObject(0, 0x19F, playerUid, uid, itemId, 0, 0);
		} else {
			GetActor("Item") << MsgObject(0, 0x19F, -1, uid, itemId, 0, 0);
		}
		break;
	}
	case 0x5A: {
		unsigned long uid = packet.Decode4();

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			GetActor("GolfRule") << MsgObject(0, 0x7C, uid, 0, 0, 0, 0);
		}
		break;
	}
	case 0x5B: {
		unsigned long uid = packet.Decode4();
		unsigned short iconId = packet.Decode2();

		if (uid == MyGuid(false)) {
			break;
		}

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			if (CHeadIcon::Instance()) {
				CHeadIcon::Instance()->SetIcon(uid, iconId);
			}
		}
		break;
	}
	case 0x5C: {
		unsigned long uid = packet.Decode4();

		if (uid == MyGuid(false)) {
			break;
		}

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			GetActor("Caddie") << MsgObject(0, 0x1DA, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x61: {
		unsigned long uid = packet.Decode4();

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			if (CSharedDoc::Instance()->m_roomType == 7) {
				GOLFDOC()->m_clearedShort194 = packet.Decode2();
			}

			GetActor("GolfRule") << MsgObject(0, 0x7D, uid, 0, 0, 0, 0);
		}
		break;
	}
	case 0x63:
		if (CSharedDoc::Instance()->m_roomType == 7) {
			GOLFDOC()->m_selectedIndexA = packet.Decode4();
		}

		GetActor("GolfRule") << MsgObject(0, 0x7E, 0, 0, 0, 0, 0);
		break;
	case 0x64:
	case 0x8F:
	case 0xB1:
		// the game's end: every player's round result
		if (!GOLFDOC()->m_netGameEnd) {
			if (CMessengerInfo::Instance()->GetMyStatus() != MSN::STATUS_BUSY) {
				CMessengerInfo::Instance()->SetMyStatus(MSN::STATUS_ONLINE);
			}

			unsigned char count = packet.Decode1();

			if (CSharedDoc::Instance()->m_roomType == 7) {
				GOLFDOC()->m_selectedIndexA = packet.Decode4();
				GOLFDOC()->m_selectedIndexB = packet.Decode4();
			}

			for (int i = 0; i < count; i++) {
				sRoundResult result;
				packet.DecodeBuffer(&result, sizeof(result));

				if (CSharedDoc::Instance()->AreWePlayingTogether(result.m_uid)) {
					GOLFDOC()->m_roundResults[CSharedDoc::Instance()->GetIndex(result.m_uid)] = result;
				}
			}

			if (type == 0x8F) {
				TEAM(0)->m_resetCount = packet.Decode1();
				TEAM(1)->m_resetCount = packet.Decode1();
				GOLFDOC()->m_teamGameWinner = (eTeamName)packet.Decode1();
			}
		}

		GetActor("GolfRule") << MsgObject(0, 0x80, 0, 0, 0, 0, 0);
		break;
	case 0x8D: {
		unsigned long uid = packet.Decode4();

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			GetActor("GolfRule") << MsgObject(0, 0x83, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x62: {
		sShotResult msg;
		packet.DecodeBuffer(&msg, sizeof(msg));

		if (CSharedDoc::Instance()->AreWePlayingTogether(msg.m_uid)) {
			unsigned char index;

			if (IsMassGame()) {
				index = 0;
			} else {
				index = CSharedDoc::Instance()->GetIndex(msg.m_uid);
			}

			GOLFDOC()->m_shotResultMsg[index] = msg;

			GetActor("GolfRule") << MsgObject(0, 0x79, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x6A: {
		// mass game: a player's state changed (2: finished, 3: skipped)
		if (!IsMassGame()) {
			break;
		}

		unsigned long uid = packet.Decode4();
		unsigned char state = packet.Decode1();

		sRivalData &result = CSharedDoc::Instance()->m_gameResultList[CSharedDoc::Instance()->GetIndex(uid)];

		if (!CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			break;
		}

		switch (state) {
		case 3: {
			for (std::vector<sRivalData>::iterator it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
				if (it->m_state != 3) {
					result.m_sortGroup--;
				}
			}

			GetActor("Score") << MsgObject(0, 0x133, result.m_hole - 1, 0, 0, 0, 0);
			break;
		}
		case 2: {
			for (std::vector<sRivalData>::iterator it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
				if (it->m_state == 0) {
					result.m_finishOrder--;
				}
			}

			if (uid == MyGuid(true)) {
				GOLFDOC()->m_netGameEnd = true;
				GOLFDOC()->m_statUpdated = false;

				if (CMessengerInfo::Instance()->GetMyStatus() != MSN::STATUS_BUSY) {
					CMessengerInfo::Instance()->SetMyStatus(MSN::STATUS_ONLINE);
				}
			}
			break;
		}
		}

		result.m_state = state;
		GetActor("Score") << MsgObject(0, 0x2A, 0, 0, 0, 0, 0);
		break;
	}
	case 0x6B: {
		// mass game: a player's hole result
		if (!IsMassGame()) {
			break;
		}

		unsigned long uid = packet.Decode4();
		unsigned char hole = packet.Decode1();
		unsigned char shots = packet.Decode1();
		int score = packet.Decode4();

		__int64 pang = packet.Decode8();
		__int64 extraPang = packet.Decode8();

		int holeFinished = packet.Decode1();

		if (!CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			break;
		}

		CSharedDoc::Instance()->LogGstUpdateHole(uid, hole, score, pang);

		std::vector<sRivalData>::iterator end = CSharedDoc::Instance()->m_gameResultList.end();
		std::vector<sRivalData>::iterator it = std::find_if(CSharedDoc::Instance()->m_gameResultList.begin(), end, sFindRival(uid));

		if (it == end) {
			break;
		}

		if (holeFinished == 1) {
			unsigned char holeShots = shots - it->m_totalShots;

			it->m_holeShots[hole - 1] = holeShots;
			it->m_holeScore[hole - 1] = score - it->m_score;
			it->m_holePang[hole - 1] = pang - it->m_pang;

			it->m_totalShots = shots;
			it->m_score = score;
			it->m_hole = CSharedDoc::Instance()->m_holeOrder[GetHoleIndex(hole)];
		}

		it->m_pang = pang;
		it->m_extraPang = extraPang;

		if (CSharedDoc::Instance()->m_roomType == 6) {
			GetActor("GolfRule") << MsgObject(0, 0x5E, uid, hole, 0, 0, 0);
			if (CSharedDoc::Instance()->m_trophyUID[0] == uid || CSharedDoc::Instance()->m_trophyUID[1] == uid) {
				GetActor("GolfRule") << MsgObject(0, 0x9F, uid, 0, 0, 0, 0);
			}
		}

		GetActor("Score") << MsgObject(0, 0x2A, 0, 0, 0, 0, 0);

		if (holeFinished != 1) {
			break;
		}

		GetActor("Score") << MsgObject(0, 0x136, hole, 0, 0, 0, 0);
		GetActor("Score") << MsgObject(0, 0x137, 0, 0, 0, 0, 0);
		GetActor("Spot") << MsgObject(0, 0xF9, CSharedDoc::Instance()->GetIndex(uid), it->m_hole, 0, 0, 0);
		break;
	}
	case 0x6C: {
		// mass game: where a player's ball lies
		if (!IsMassGame()) {
			break;
		}

		unsigned long uid = packet.Decode4();
		unsigned char hole = packet.Decode1();

		float pos[2];
		packet.DecodeBuffer(pos, sizeof(pos));

		unsigned int approachDistance;
		int approachPoint;
		if (CSharedDoc::Instance()->m_roomType == 10) {
			approachDistance = packet.Decode4();
			approachPoint = packet.Decode4();
		}

		unsigned short distance = packet.Decode2();

		if (!CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			break;
		}

		unsigned char index = CSharedDoc::Instance()->GetIndex(uid);

		if (CSharedDoc::Instance()->m_roomType == 10) {
			CSharedDoc::Instance()->m_gameResultList[index].m_approachPoint = approachPoint;
			CSharedDoc::Instance()->m_gameResultList[index].m_approachDistance = approachDistance;

			if (approachPoint == -1) {
				CSharedDoc::Instance()->m_gameResultList[index].m_approachDistanceCopy = approachPoint;
			}

			GetActor("Score") << MsgObject(0, 0x2A, index == MyIndex(), 0, 0, 0, 0);
		}

		GetActor("Spot") << MsgObject(0, 0xF8, index, hole, distance, 0, 0);

		CSharedDoc::Instance()->m_gameResultList[index].m_ballPos = WVector(pos[0], 0.0f, pos[1]);
		break;
	}
	case 0x77:
		if (CSharedDoc::Instance()->m_gameStatistics[0].exp == 0xFFFFFFFF) {
			if (CSharedDoc::Instance()->m_roomType == 10) {
				unsigned long uid = packet.Decode4();

				if (uid != 0xFFFFFFFF) {
					CSharedDoc* pDoc = CSharedDoc::Instance();
					std::map<unsigned long, sBriefUserInfo>::iterator it = pDoc->m_userMap.find(uid);

					if (it != pDoc->m_userMap.end()) {
						WSendPacket request((enumClientPacket)0x2F);
						request.Encode4(it->second.id);
						request.Encode1(5);
						request.Send(SEND_TO_DEFAULT);
					}
				}
			} else {
				CSharedDoc::Instance()->m_gameStatistics[0].exp = packet.Decode4();
				CSharedDoc::Instance()->m_matchTypeId = packet.Decode4();
				CSharedDoc::Instance()->m_value456C = packet.Decode1();
				CSharedDoc::Instance()->m_winningTeam = packet.Decode1();

				if (CSharedDoc::Instance()->m_roomType == 6) {
					CSharedDoc::Instance()->m_value5018 = packet.Decode4();
					CSharedDoc::Instance()->m_matchGameCount = packet.Decode4();

					CSharedDoc::Instance()->m_value47B4[0] = packet.Decode4();
					CSharedDoc::Instance()->m_value47B4[1] = packet.Decode4();
				}
			}

			packet.DecodeBuffer(CSharedDoc::Instance()->m_awardPlayers, 0x60);
		}

		if (CSharedDoc::Instance()->m_roomType == 10) {
			break;
		}

		if (GOLFDOC()->m_statUpdated) {
			GetActor("GolfRule") << MsgObject(0, 0x8F, 1, 0, 0, 0, 0);
		}
		break;
	case 0x5F: {
		unsigned long uid = packet.Decode4();

		if (!CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			break;
		}

		std::map<unsigned long, sRoomSlot>::iterator slot = CSharedDoc::Instance()->m_roomSlotMap.find(uid);
		if (slot != CSharedDoc::Instance()->m_roomSlotMap.end()) {
			if (slot->second.m_pPet) { delete slot->second.m_pPet; slot->second.m_pPet = 0; }
			CSharedDoc::Instance()->m_roomSlotMap.erase(slot);
		}

		std::list<sSlotInfo>& players = CSharedDoc::Instance()->m_roomPlayerList;
		std::list<sSlotInfo>::iterator it;
		it = std::find(players.begin(), players.end(), uid);
		if (it != players.end()) {
			unsigned char connection = it->m_connection;

			players.erase(it);

			for (it = players.begin(); it != players.end(); ++it) {
				if (it->m_connection > connection) {
					it->m_connection--;
				}
			}
		}

		if (!IsMassGame()) {
			GetActor("GolfRule") << MsgObject(0, 0x82, uid, 0, 0, 0, 0);
		}
		break;
	}
	case 0x7A: {
		// the new room master
		std::list<sSlotInfo>& players = CSharedDoc::Instance()->m_roomPlayerList;

		unsigned long uid = packet.Decode4();
		packet.Decode2();

		for (std::list<sSlotInfo>::iterator it = players.begin(); it != players.end(); ++it) {
			it->m_attributeBits.roomMaster = (it->m_uid == uid);
		}
		break;
	}
	case 0x7C:
		CSharedDoc::Instance()->m_leaveReason = packet.Decode2();
		m_unknown208 = 5.0f;
		break;
	case 0x5E: {
		unsigned char data[12];
		packet.DecodeBuffer(data, 12);

		GetActor("GolfRule") << MsgObject(0, 0x8D, (intptr_t)data, 0, 0, 0, 0);
		break;
	}
	case 0x88:
		GetActor("GolfRule") << MsgObject(0, 0x93, 0, 0, 0, 0, 0);
		break;
	case 0x89: {
		unsigned long uid = packet.Decode4();
		int paused = packet.Decode1();

		if (paused != 0) {
			WSingleton<CChatMsg>::Instance()->AddChatMsg(MakeStr("%s\xb4\xd4\xc0\xcc \xb0\xd4\xc0\xd3\xc0\xbb \xc0\xe1\xbd\xc3 \xc1\xdf\xc1\xf6\xbd\xc3\xc4\xd7\xbd\xc0\xb4\xcf\xb4\xd9.", CSharedDoc::Instance()->m_roomPlayers[CSharedDoc::Instance()->GetIndex(uid)].m_name), 0xFFFF7878, false, false);
		} else {
			WSingleton<CChatMsg>::Instance()->AddChatMsg(MakeStr("%s\xb4\xd4\xc0\xcc \xc1\xdf\xc1\xf6\xb5\xc8 \xb0\xd4\xc0\xd3\xc0\xbb \xc0\xe7\xb0\xb3\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", CSharedDoc::Instance()->m_roomPlayers[CSharedDoc::Instance()->GetIndex(uid)].m_name), 0xFFFF7878, false, false);
		}

		GetActor("GolfRule") << MsgObject(0, 0x84, paused, (intptr_t)"\xc7\xf6\xc0\xe7 \xb0\xd4\xc0\xd3\xc0\xcc \xc0\xe1\xbd\xc3 \xc1\xdf\xb4\xdc\xb5\xc8 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.\n\xb0\xd4\xc0\xd3\xc0\xbb \xb0\xe8\xbc\xd3\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad\xb4\xc2 \xc8\xae\xc0\xce\xc0\xbb \xb4\xad\xb7\xaf\xc1\xd6\xbc\xbc\xbf\xe4.", 0, 0, 0);
		break;
	}
	case 0x8A: {
		// mass game over: the players still playing finish every remaining
		// hole at +5
		if (CMessengerInfo::Instance()->GetMyStatus() != MSN::STATUS_BUSY) {
			CMessengerInfo::Instance()->SetMyStatus(MSN::STATUS_ONLINE);
		}

		unsigned char finishOrder = (unsigned char)CSharedDoc::Instance()->m_gameResultList.size();
		for (std::vector<sRivalData>::iterator it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
			if (it->m_state == 0) {
				it->m_finishOrder = finishOrder--;
				it->m_state = 2;

				for (;;) {
					it->m_holePang[it->m_hole - 1] = 0;
					it->m_holeShots[it->m_hole - 1] = CSharedDoc::Instance()->m_courseHoleOn[it->m_hole - 1] + 5;
					it->m_holeScore[it->m_hole - 1] = 5;
					it->m_score += 5;

					if (it->m_hole == CSharedDoc::Instance()->m_holeOrder[CSharedDoc::Instance()->m_holes - 1]) {
						break;
					}

					it->m_hole = CSharedDoc::Instance()->m_holeOrder[GetHoleIndex(it->m_hole)];
				}
			}
		}
		break;
	}
	case 0x8E:
		PassMsgToActor("GolfRule", MsgObject(0, 0x94, 0, 0, 0, 0, 0));
		break;
	case 0xA9: {
		int count = packet.Decode1();
		for (int i = 0; i < count; i++) {
			unsigned long uid = packet.Decode4();
			sWinningPrize prize;
			packet.DecodeBuffer(&prize, sizeof(prize));
			CSharedDoc::Instance()->SetWinningPrize(uid, prize);
		}
		break;
	}
	case 0x90:
		GetActor("GolfRule") << MsgObject(0, 0x95, 0, 0, 0, 0, 0);
		break;
	case 0x82: {
		// A whisper: bit 7 of the first byte marks a GM, the rest 0 for the
		// echo of the player's own whisper.
		unsigned char flags = packet.Decode1();
		unsigned char gm = flags >> 7;
		flags &= 0x7F;

		bool own = flags == 0;

		int notify;
		unsigned long color = 0xFFD2A2FF;
		if (gm) {
			color = 0xFF6EECFB;
		}

		std::string nickname;
		std::string text;
		std::string message;

		packet.DecodeStr(nickname);
		packet.DecodeStr(message);

		if (own) {
			text = MyNick();
			text += ">";
			text += nickname;
		} else {
			if (!WSingleton<COption>::Instance()->gIsWhisperAllowed()) {
				break;
			}

			std::list<std::string>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(), WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(), nickname);
			if (it != WSingleton<CSharedDoc>::Instance()->m_ignoreList.end()) {
				break;
			}

			GetActor("GolfRule") << MsgObject(0, 0x3F, (intptr_t)&notify, 0, 0, 0, 0);
			GetActor("GolfRule") << MsgObject(0, 0x38, (intptr_t)nickname.c_str(), 0, 0, 0, 0);

			text = nickname + ">" + MyNick();
		}

		WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(message.c_str(), false);

		text += " : \x08'";
		text += message;
		text += "'";

		WSingleton<CChatMsg>::Instance()->AddChatMsg(text, color, false, true);
		break;
	}
	case 0xAE: {
		// a message sent to the player (shown like a whisper)
		std::string nickname;
		std::string message;
		std::string text;

		packet.DecodeStr(nickname);
		packet.DecodeStr(message);

		if (WSingleton<COption>::Instance()->gIsWhisperAllowed()) {
			std::list<std::string>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(), WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(), nickname);
			if (it == WSingleton<CSharedDoc>::Instance()->m_ignoreList.end()) {
				WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(message.c_str(), false);

				// "<nickname>>to all"
				text = nickname + ">" + "\xc6\xc0\xbf\xa1\xb0\xd4";
				text += " : \x08";
				text += message;

				WSingleton<CChatMsg>::Instance()->AddChatMsg(text, 0xFF00C6FF, false, true);
			}
		}
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
			// their info window and pick [block chat]."
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			pForm->SetMessage("\xbd\xc5\xb0\xed\xb0\xa1 \xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xc1\xa2\xbc\xf6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.\n\n\xc6\xaf\xc1\xa4 \xbb\xf3\xb4\xeb\xc0\xc7 \xb4\xeb\xc8\xad\xb8\xa6 \xc2\xf7\xb4\xdc \xc7\xcf\xb7\xc1\xb8\xe9 \xbb\xf3\xb4\xeb \xc1\xa4\xba\xb8\xc3\xa2\xc0\xbb \xb8\xb6\xbf\xec\xbd\xba \xb4\xf5\xba\xed \xc5\xac\xb8\xaf \xc8\xc4 \\c0xffff0000\\c[\xb4\xeb\xc8\xad \xc2\xf7\xb4\xdc \xbe\xc6\xc0\xcc\xc4\xdc]\\c0xff000000\\c\xc0\xbb \xbc\xb1\xc5\xc3 \xc7\xcf\xbd\xc3\xb8\xe9 \xb5\xcb\xb4\xcf\xb4\xd9.", false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
		}
		break;
	case 0x9A: {
		unsigned char count = packet.Decode1();
		for (unsigned char i = 0; i < count; i++) {
			unsigned long arrow = packet.Decode4();
			GOLFDOC()->m_commandArrows.push_back(arrow);
		}

		if (OnlinePlay()) {
			GetActor("Cutin") << MsgObject(0, 0x78, 8, 0, 0, 0, 0);
		}
		break;
	}
	case 0x97: {
		if (GOLFDOC()->GameMode() & 0x30E3) {
			break;
		}

		unsigned char count = packet.Decode1();
		std::vector<unsigned long> uids;

		uids.reserve(count);

		for (unsigned char i = 0; i < count; i++) {
			uids.push_back(packet.Decode4());
		}

		WSendPacket send((enumClientPacket)0x40);

		sPlayerData* pPlayer = PLAYER(0);
		sPlayerSyncData data;

		data.club = GolfClub().GetIndex();
		data.frame = GetPVS().GetFrame();
		data.angle = GOLFDOC()->m_cameraData.m_yaw;

		data.powerShotLevel = pPlayer->m_powerShotLevel;
		data.comboGauge = (float)float2int(pPlayer->m_comboGauge);
		data.numItems = pPlayer->m_numItems;
		data.shotItemTypeId = pPlayer->m_shotItemTypeId;
		data.rivalLevel = pPlayer->m_rivalLevel;
		data.lastShotPos.x = pPlayer->m_lastShotPos.x;
		data.lastShotPos.y = pPlayer->m_lastShotPos.y;
		data.lastShotPos.z = pPlayer->m_lastShotPos.z;
		data.ballPos.x = pPlayer->m_ballPos.x;
		data.ballPos.y = pPlayer->m_ballPos.y;
		data.ballPos.z = pPlayer->m_ballPos.z;
		data.powerShot = GolfClub().PowerShotType();
		data.extraPang = pPlayer->m_extraPang;

		for (int hole = 0; hole < 18; hole++) {
			data.holes[hole] = pPlayer->m_holes[hole];
			data.holeIn[hole] = pPlayer->m_holeIn[hole];
			data.holeRecord[hole] = pPlayer->m_holeRecord[hole];
		}

		if (IsMassGame()) {
			data.state = GolfBall().m_preDataIndex != -1;
		} else {
			data.state = pPlayer->m_state;
		}

		send.EncodeBuffer(&data, sizeof(data));

		send.Encode1(count);
		for (unsigned char j = 0; j < count; j++) {
			send.Encode4(uids[j]);
		}

		send.Send(SEND_TO_DEFAULT);
		break;
	}
	case 0xBC:
		GetActor("GolfRule") << MsgObject(0, 0x9E, 1, 0, 0, 0, 0);
		break;
	case 0xBA:
		// joining a game: as a spectator, the whole room; as a player, the
		// spectator who joined
		if (GalleryMode()) {
			unsigned char count = packet.Decode1();

			if (count == 0x0B) {
				// "Notice: could not find a game room open to spectators."
				WSingleton<CChatMsg>::Instance()->AddChatMsg("\xbe\xcb\xb8\xb2 : \xb5\xd1\xb7\xaf\xba\xb8\xb1\xe2\xb0\xa1 \xb0\xa1\xb4\xc9\xc7\xd1 \xb0\xd4\xc0\xd3\xc1\xdf\xc0\xce \xb9\xe6\xc0\xbb \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFFFF7878, false, false);
				break;
			}

			if (count == 0x10) {
				// "Notice: the game may end before loading finishes."
				WSingleton<CChatMsg>::Instance()->AddChatMsg("\xbe\xcb\xb8\xb2 : \xb7\xce\xb5\xf9\xb5\xc7\xb1\xe2 \xc0\xfc\xbf\xa1 \xb0\xd4\xc0\xd3\xc0\xcc \xc1\xbe\xb7\xe1\xb5\xc9 \xbf\xec\xb7\xc1\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFFFF7878, false, false);
				break;
			}

			packet.DecodeBuffer(&CSharedDoc::Instance()->m_roomInfo, sizeof(sRoomInfo));
			CSharedDoc::Instance()->m_pangPercent = packet.Decode4();

			count = packet.Decode1();
			CSharedDoc::Instance()->m_roomPlayerList.clear();

			for (unsigned char i = 0; i < count; i++) {
				sSlotInfo slot;
				packet.DecodeBuffer(&slot, sizeof(slot));
				CSharedDoc::Instance()->m_roomPlayerList.push_back(slot);
			}

			if (IsMassGame(CSharedDoc::Instance()->m_roomInfo.m_gameType)) {
				packet.DecodeBuffer(&CSharedDoc::Instance()->m_roomPlayers[0], sizeof(sUserInfo));

				if (IsLocalContent((localContentType_t)0x57)) {
					RequestUccInfo(CSharedDoc::Instance()->m_roomPlayers[0]);
				}

				CSharedDoc::Instance()->m_userInfo.galleryGuid = CSharedDoc::Instance()->m_roomPlayers[0].m_guid;
			} else {
				if (count > 3) {
					break;
				}

				for (unsigned char i = 0; i < count; i++) {
					packet.DecodeBuffer(&CSharedDoc::Instance()->m_roomPlayers[i], sizeof(sUserInfo));

					if (IsLocalContent((localContentType_t)0x57)) {
						RequestUccInfo(CSharedDoc::Instance()->m_roomPlayers[i]);
					}
				}
			}

			SetCurMap(packet.Decode1());
			SetGameType(packet.Decode1());
			SetHoleType(packet.Decode1());
			SetHoles(packet.Decode1());
			CSharedDoc::Instance()->m_matchTypeId = packet.Decode4();
			SetShotTimeLimit(packet.Decode4());
			SetGameTimeLimit(packet.Decode4());

			for (count = 0; count < CSharedDoc::Instance()->m_holes; count++) {
				CSharedDoc::Instance()->m_holeRandom[count] = packet.Decode4();
				CSharedDoc::Instance()->m_holePin[count] = (unsigned char)(CSharedDoc::Instance()->m_holeRandom[count] % 3);
				CSharedDoc::Instance()->m_holeOrder[count] = packet.Decode1();
			}

			GOLFDOC()->m_currentHole = packet.Decode1();
			CSharedDoc::Instance()->m_value4D74 = 0;

			if (AfxGetTask()->IsInited()) {
				AfxGetTask()->SetInited(false);
				AfxGetTask()->Reload();
			}

			GetActor("GolfBg") << MsgObject(0, 0x140, 0, 0, 0, 0, 0);
			GetActor("GolfBg") << MsgObject(0, 0x13E, 0, 0, 0, 0, 0);

			GetActor("Screen") << MsgObject(0, 0x49, 0, 0, 0, 0, 0);
		} else {
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(slot));

			CSharedDoc::Instance()->m_roomPlayerList2.clear();
			CSharedDoc::Instance()->m_roomPlayerList2.push_back(slot);
			CSharedDoc::Instance()->m_roomInfo.m_galleryCount = 1;

			GetActor("GolfRule") << MsgObject(0, 0x9E, 0, 0, 0, 0, 0);

			IsMyTurn(true);
		}
		break;
	case 0xBB: {
		// Gallery: the round's state for every player (the data case 0x97 sends).
		if (!GalleryMode()) {
			break;
		}

		unsigned char bgType = packet.Decode1();
		packet.Decode1();
		packet.Decode1();

		GetActor("GolfBg") << MsgObject(0, 0x142, bgType, 1, 0, 0, 0);

		unsigned char intensity = packet.Decode1();
		unsigned char direction = packet.Decode1();

		Wind().Init(intensity, direction, 0, 1);

		unsigned long remainTime = CSharedDoc::Instance()->m_shotTimeLimit;
		remainTime -= packet.Decode4();

		unsigned long turnUid = packet.Decode4();

		unsigned char count = packet.Decode1();

		for (unsigned char i = 0; i < count; i++) {
			sPlayerSyncData data;
			packet.DecodeBuffer(&data, sizeof(data));

			PLAYER(i)->m_powerShotLevel = data.powerShotLevel;
			PLAYER(i)->m_comboGauge = data.comboGauge;
			PLAYER(i)->m_numItems = data.numItems;
			PLAYER(i)->m_shotItemTypeId = data.shotItemTypeId;
			PLAYER(i)->m_rivalLevel = data.rivalLevel;
			PLAYER(i)->m_lastShotPos.x = data.lastShotPos.x;
			PLAYER(i)->m_lastShotPos.y = data.lastShotPos.y;
			PLAYER(i)->m_lastShotPos.z = data.lastShotPos.z;
			PLAYER(i)->m_ballPos.x = data.ballPos.x;
			PLAYER(i)->m_ballPos.y = data.ballPos.y;
			PLAYER(i)->m_ballPos.z = data.ballPos.z;

			for (unsigned char hole = 0; hole < 18; hole++) {
				PLAYER(i)->m_holes[hole] = data.holes[hole];
				PLAYER(i)->m_holeIn[hole] = data.holeIn[hole];
				PLAYER(i)->m_holeRecord[hole] = (int)data.holeRecord[hole];
			}

			PLAYER(i)->m_uid = CSharedDoc::Instance()->m_roomPlayers[i].m_guid;
			PLAYER(i)->m_turnIndex = 0;

			PLAYER(i)->m_ghostResult = data.ghostResult;
			PLAYER(i)->m_totalScore = data.totalScore;

			PLAYER(i)->m_ghostAward = data.ghostAward;
			PLAYER(i)->m_extraPang = (int)data.extraPang;

			PLAYER(i)->m_index = i;
			PLAYER(i)->m_recordedHoles = GetHoleIndex(GOLFDOC()->m_currentHole) - 1;
			PLAYER(i)->m_state = data.state;

			if (PLAYER(i)->m_uid == turnUid) {
				GOLFDOC()->m_myPlayerIndex = i;

				GolfBall().m_pos = PLAYER(i)->m_ballPos;
				GolfBall().m_groundArea = GetPVS().GetMeshIndex(GolfBall().m_pos, false);

				GameCamera().m_yaw = data.angle;

				GolfClub().SetApproachMode();
				GolfClub().SetClub(data.club);
				GolfClub().SetPowerShot(data.powerShot);
				GetPVS().SetFrame(data.frame);
			}
		}

		GetActor("GolfRule") << MsgObject(0, 0x78, 0x100000, 0, 0, 0, 0);

		GOLFDOC()->m_remainShotTime = remainTime;
		break;
	}
	case 0x96: {
		// Gallery: the state of the player being watched
		if (!GalleryMode()) {
			break;
		}

		if (!IsMassGame()) {
			CSharedDoc::Instance()->m_userInfo.galleryGuid = packet.Decode4();
			break;
		}

		packet.DecodeBuffer(&CSharedDoc::Instance()->m_roomPlayers[0], sizeof(sUserInfo));

		if (IsLocalContent((localContentType_t)0x57)) {
			RequestUccInfo(CSharedDoc::Instance()->m_roomPlayers[0]);
		}

		// drop the equipment the player's level does not allow
		IFF_STRUCT::sCaddie *pCaddie = ItemManager()->FindCaddie(CSharedDoc::Instance()->m_roomPlayers[0].m_caddieInfo.typeId);
		if ((pCaddie && CSharedDoc::Instance()->m_roomPlayers[0].m_level < (pCaddie->m_level & 0x7F))
			|| ((CSharedDoc::Instance()->m_roomPlayers[0].m_caddieInfo.flags & 2) && CSharedDoc::Instance()->m_roomPlayers[0].m_caddieInfo.days == 0 && pCaddie->fee > 0)) {
			CSharedDoc::Instance()->m_roomPlayers[0].m_caddieInfo.typeId = 0;
		}

		IFF_STRUCT::sClubSet *pClubSet = ItemManager()->FindClubSet(CSharedDoc::Instance()->m_roomPlayers[0].m_clubSet.typeId);

		if (pClubSet) {
			unsigned char clubLevel = pClubSet->m_level;
			unsigned char userLevel = CSharedDoc::Instance()->m_roomPlayers[0].m_level;
			if ((userLevel < (clubLevel & 0x7F) && !(clubLevel & 0x80)) || (userLevel > (clubLevel & 0x7F) && (clubLevel & 0x80))) {
				CSharedDoc::Instance()->m_roomPlayers[0].m_clubSet.typeId = 0x10000000;
			}
		}

		// pack the equipped items to the front
		unsigned long items[10];
		memcpy(items, CSharedDoc::Instance()->m_roomPlayers[0].m_equipItem, sizeof(items));

		unsigned char used = 0;
		for (unsigned char i = 0; i < 10; i++) {
			if (items[i] != 0) {
				CSharedDoc::Instance()->m_roomPlayers[0].m_equipItem[used++] = items[i];
			}
		}

		for (; used < 10; used++) {
			CSharedDoc::Instance()->m_roomPlayers[0].m_equipItem[used] = 0;
		}

		CSharedDoc::Instance()->m_userInfo.galleryGuid = CSharedDoc::Instance()->m_roomPlayers[0].m_guid;

		if (WSingleton<CHeadIcon>::Instance()) {
			WSingleton<CHeadIcon>::Instance()->SetLevel();
		}

		unsigned char hole = packet.Decode1();

		unsigned char bgType = packet.Decode1();
		packet.Decode1();
		packet.Decode1();

		unsigned char intensity = packet.Decode1();
		unsigned char direction = packet.Decode1();
		unsigned char windType = packet.Decode1();

		sPlayerSyncData data;
		packet.DecodeBuffer(&data, sizeof(data));

		GolfClub();

		GetActor("Player") << MsgObject(0, 0x49, 0, 0, 0, 0, 0);

		GetActor("Screen") << MsgObject(0, 0x49, 0, 0, 0, 0, 0);

		GetActor("Player") << MsgObject(0, 0x11C, CSharedDoc::Instance()->m_roomPlayers[0].m_clubSet.typeId, 0, 0, 0, 0);

		GetActor("Ball") << MsgObject(0, 0x4C, CSharedDoc::Instance()->m_roomPlayers[0].m_ballId, 0, 0, 0, 0);
		GetActor("Screen") << MsgObject(0, 0x4C, CSharedDoc::Instance()->m_roomPlayers[0].m_ballId, 0, 0, 0, 0);

		GetActor("Caddie") << MsgObject(0, 0x4A, CSharedDoc::Instance()->m_roomPlayers[0].m_caddieInfo.typeId, CSharedDoc::Instance()->m_roomPlayers[0].m_caddieInfo.level - 1, CSharedDoc::Instance()->m_roomPlayers[0].m_caddieInfo.cadItemTypeId, 0, 0);

		GetActor("Mascot") << MsgObject(0, 0x4E, CSharedDoc::Instance()->m_roomPlayers[0].m_mascot.id, (intptr_t)&CSharedDoc::Instance()->m_roomPlayers[0].m_mascot, 0, 0, 0);

		GOLFDOC()->SetPlayerLevel(0);

		GetActor("GolfBg") << MsgObject(0, 0x142, bgType, 1, 0, 0, 0);

		GOLFDOC()->m_currentHole = hole;
		GetActor("GolfBg") << MsgObject(0, 0x1E8, 0, 0, 0, 0, 0);

		Wind().Init(intensity, direction, windType, 1);

		sPlayerData *pPlayer = PLAYER(0);

		pPlayer->m_powerShotLevel = data.powerShotLevel;
		pPlayer->m_comboGauge = data.comboGauge;
		pPlayer->m_numItems = data.numItems;
		pPlayer->m_shotItemTypeId = data.shotItemTypeId;
		pPlayer->m_rivalLevel = data.rivalLevel;
		pPlayer->m_lastShotPos.x = data.lastShotPos.x;
		pPlayer->m_lastShotPos.y = data.lastShotPos.y;
		pPlayer->m_lastShotPos.z = data.lastShotPos.z;
		pPlayer->m_ballPos.x = data.ballPos.x;
		pPlayer->m_ballPos.y = data.ballPos.y;
		pPlayer->m_ballPos.z = data.ballPos.z;

		for (unsigned char i = 0; i < 18; i++) {
			pPlayer->m_holes[i] = data.holes[i];
			pPlayer->m_holeIn[i] = data.holeIn[i];
			pPlayer->m_holeRecord[i] = (int)data.holeRecord[i];
		}

		GolfBall().m_pos = pPlayer->m_ballPos;
		GolfBall().m_groundArea = GetPVS().GetMeshIndex(GolfBall().m_pos, false);

		switch (data.state) {
		case 0:
			GameCamera().m_yaw = data.angle;
			GolfClub().SetApproachMode();
			break;
		case 2:
			GolfBall().m_pos = GetTeeShotPos();
			GolfBall().m_pos.y += GolfBall().m_radius;
			pPlayer->m_ballPos = GolfBall().m_pos;
			pPlayer->m_lastShotPos = GolfBall().m_pos;
		case 1:
			GolfClub().SelectBestClub();
			GOLFDOC()->m_cameraData.m_yaw = CalcDeltaAngle(GOLFDOC()->m_pAutoTarget->GetPoint(0) - GolfBall().m_pos, WVector::UNIT_POS_Z);
			break;
		}

		pPlayer->m_uid = CSharedDoc::Instance()->m_userInfo.galleryGuid;

		pPlayer->m_turnIndex = 0;
		pPlayer->m_ghostResult = CSharedDoc::Instance()->m_gameResultList[CSharedDoc::Instance()->GetIndex(pPlayer->m_uid)].m_score;
		pPlayer->m_totalScore = CSharedDoc::Instance()->m_gameResultList[CSharedDoc::Instance()->GetIndex(pPlayer->m_uid)].m_totalShots;

		pPlayer->m_ghostAward = (int)data.holeRecord[hole - 1] + (int)CSharedDoc::Instance()->m_gameResultList[CSharedDoc::Instance()->GetIndex(pPlayer->m_uid)].m_pang;
		pPlayer->m_extraPang = (int)data.extraPang;

		pPlayer->m_index = CSharedDoc::Instance()->m_gameResultList[CSharedDoc::Instance()->GetIndex(pPlayer->m_uid)].m_value13F;

		pPlayer->m_recordedHoles = GetHoleIndex(hole) - 1;
		pPlayer->m_state = 0;

		GolfClub().SetClub(data.club);
		GolfClub().SetPowerShot(data.powerShot);

		PassMsgToActor("GolfRule", MsgObject(0, 0x78, 0x100000, 0, 0, 0, 0));

		GetPVS().SetFrame(data.frame);
		break;
	}
	case 0x99: {
		if (!IsMassGame()) {
			break;
		}

		unsigned long uid = packet.Decode4();
		CSharedDoc* pDoc = CSharedDoc::Instance();
		pDoc->m_gameResultList[pDoc->GetIndex(uid)].m_joined = packet.Decode4();
		break;
	}
	case 0x95:
		CSharedDoc::Instance()->m_matchTypeId = packet.Decode4();
		break;
	case 0xA1: {
		unsigned long uid = packet.Decode4();
		unsigned char progress = packet.Decode1();

		if (uid == MyGuid(false)) {
			break;
		}

		UpdateLoadingInfo(uid, progress);
		break;
	}
	case 0xA2: {
		if (IsMyTurn(false)) {
			unsigned short count = packet.Decode2();
			unsigned long typeId;
			if (IsLocalContent((localContentType_t)0x4F)) {
				typeId = packet.Decode4();
			} else {
				typeId = 0x1A000000;
			}

			std::list<sItemInfo>::iterator it = std::find(CSharedDoc::Instance()->m_itemInfoList.begin(), CSharedDoc::Instance()->m_itemInfoList.end(), typeId);

			if (it != CSharedDoc::Instance()->m_itemInfoList.end()) {
				it->m_count = count;
				if (count == 0) {
					CSharedDoc::Instance()->m_itemInfoList.erase(it);
				}
			}
		}

		GetActor("GolfRule") << MsgObject(0, 0x97, 0, 0, 0, 0, 0);
		break;
	}
	case 0xAC: {
		// the gallery (spectator) list
		std::list<sSlotInfo>& gallery = CSharedDoc::Instance()->m_roomPlayerList2;
		std::list<sSlotInfo>::iterator it;

		unsigned char type = packet.Decode1();

		switch (type) {
		case 0: {
			gallery.clear();

			unsigned char count = packet.Decode1();

			sSlotInfo slot;
			for (int i = 0; i < count; i++) {
				packet.DecodeBuffer(&slot, sizeof(slot));
				gallery.push_back(slot);
			}
			break;
		}
		case 1: {
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(slot));

			it = std::find(gallery.begin(), gallery.end(), slot.m_uid);
			if (it == gallery.end()) {
				gallery.push_back(slot);
			}
			break;
		}
		case 2: {
			unsigned long uid = packet.Decode4();

			it = std::find(gallery.begin(), gallery.end(), uid);
			if (it != gallery.end()) {
				gallery.erase(it);
			}
			break;
		}
		}

		CSharedDoc::Instance()->m_roomInfo.m_galleryCount = (unsigned char)gallery.size();
		break;
	}
	case 0x46: {
		// the room's player list
		std::list<sSlotInfo>& players = CSharedDoc::Instance()->m_roomPlayerList;
		std::list<sSlotInfo>::iterator it;

		unsigned char type = packet.Decode1();
		packet.Decode2();

		switch (type) {
		case 0:
		case 5: {
			if (type == 0) {
				players.clear();
			}

			unsigned char count = packet.Decode1();

			sSlotInfo slot;
			sCharacterInfo characterInfo;

			for (int i = 0; i < count; i++) {
				packet.DecodeBuffer(&slot, sizeof(slot));
				players.push_back(slot);

				packet.DecodeBuffer(&characterInfo, sizeof(characterInfo));
				if (IsLocalContent((localContentType_t)0x57)) {
					RequestUccInfo(characterInfo);
				}
			}
			break;
		}
		case 1: {
			sSlotInfo slot;
			packet.DecodeBuffer(&slot, sizeof(slot));

			if (!IsMassGame()) {
				sRoomSlot roomSlot;

				sCharacterInfo characterInfo;
				packet.DecodeBuffer(&characterInfo, sizeof(characterInfo));
				if (IsLocalContent((localContentType_t)0x57)) {
					RequestUccInfo(characterInfo);
				}

				roomSlot.m_characterInfo = characterInfo;
				CSharedDoc::Instance()->m_roomSlotMap[slot.m_uid] = roomSlot;
			}

			it = std::find(players.begin(), players.end(), slot.m_uid);
			if (it == players.end()) {
				players.push_back(slot);
			}
			break;
		}
		case 2: {
			unsigned long uid = packet.Decode4();

			std::map<unsigned long, sRoomSlot>::iterator slot = CSharedDoc::Instance()->m_roomSlotMap.find(uid);
			if (slot != CSharedDoc::Instance()->m_roomSlotMap.end()) {
				if (slot->second.m_pPet) { delete slot->second.m_pPet; slot->second.m_pPet = 0; }
				CSharedDoc::Instance()->m_roomSlotMap.erase(slot);
			}

			it = std::find(players.begin(), players.end(), uid);
			if (it != players.end()) {
				unsigned char connection = it->m_connection;

				players.erase(it);

				for (it = players.begin(); it != players.end(); ++it) {
					if (it->m_connection > connection) {
						it->m_connection--;
					}
				}
			}
			break;
		}
		case 3: {
			sSlotInfo slot;
			unsigned long uid = packet.Decode4();

			packet.DecodeBuffer(&slot, sizeof(slot));

			it = std::find(players.begin(), players.end(), uid);

			std::map<unsigned long, sRoomSlot>::iterator entry = CSharedDoc::Instance()->m_roomSlotMap.find(uid);
			if (entry != CSharedDoc::Instance()->m_roomSlotMap.end()) {
				entry->second.m_angelFlags.flag1 = slot.m_flags57.flag1;
				entry->second.m_angelFlags.flag0 = slot.m_flags57.flag0;
			}
			break;
		}
		}

		CSharedDoc::Instance()->m_roomInfo.m_userCount = (unsigned char)players.size();
		break;
	}
	case 0xAA: {
		unsigned long uid = packet.Decode4();
		unsigned char penalty = packet.Decode1();

		if (uid == MyGuid(false)) {
			break;
		}

		if (CSharedDoc::Instance()->AreWePlayingTogether(uid)) {
			PLAYER(CSharedDoc::Instance()->GetIndex(uid))->m_chatPenalty = penalty;
		}
		break;
	}
	case 0xAB: {
		std::string name;
		packet.DecodeStr(name);
		unsigned char channelId = packet.Decode1();

		CSharedDoc* pDoc = CSharedDoc::Instance();
		std::list<sChannelInfo>::iterator it = std::find(pDoc->m_channelList.begin(), pDoc->m_channelList.end(), channelId);
		char text[128];
		if (it == pDoc->m_channelList.end()) {
			sprintf(text, "%s\xb4\xd4\xc0\xba [%s]\xbf\xa1\xbc\xad \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", name.c_str(), pDoc->m_curServer.name);
		} else {
			sprintf(text, "%s\xb4\xd4\xc0\xba [%s]\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xd8 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", name.c_str(), it->name);
		}

		WSingleton<CChatMsg>::Instance()->AddChatMsg(text, 0xFFFF7878, false, false);
		break;
	}
	case 0x76: {
		unsigned long uid = packet.Decode4();
		unsigned char state = packet.Decode1();

		CSharedDoc* pDoc = CSharedDoc::Instance();
		std::list<sSlotInfo>::iterator it = std::find(pDoc->m_roomPlayerList.begin(), pDoc->m_roomPlayerList.end(), uid);

		if (it != pDoc->m_roomPlayerList.end()) {
			it->m_flags55.flag1 = (state == 0);
		}
		break;
	}
	case 0xC0: {
		unsigned long uid = packet.Decode4();
		CSharedDoc::Instance()->m_teamPoint[0] = packet.Decode2();
		CSharedDoc::Instance()->m_teamPoint[1] = packet.Decode2();
		unsigned char resultIndex = packet.Decode1();
		unsigned char rivalIndex = packet.Decode1();

		sGameResultInfo* pResult = &CSharedDoc::Instance()->m_gameResultList[CSharedDoc::Instance()->GetIndex(uid)];
		sRivalData* pRival = CSharedDoc::Instance()->GetRival(uid);
		if (pResult && pRival) {
			GetActor("Score") << MsgObject(0, 0x57, CSharedDoc::Instance()->m_teamPoint[0], CSharedDoc::Instance()->m_teamPoint[1], 0, 0, 0);
			GetActor("Screen") << MsgObject(0, 0x1CE, CSharedDoc::Instance()->m_teamPoint[0], CSharedDoc::Instance()->m_teamPoint[1], 0, 0, 0);

			pResult->m_uniteGuildIndex = resultIndex;
			pRival->m_uniteGuildIndex = rivalIndex;
		}
		break;
	}
	case 0xCA: {
		sPrizeInfo prizes[128] = {};
		unsigned long uid = packet.Decode4();
		int count = packet.Decode1();

		if (count > 0) {
			packet.DecodeBuffer(prizes, sizeof(prizes));
			CSharedDoc::Instance()->SetExtPrize(uid, count, prizes);
		}
		break;
	}
	case 0x147: {
		char desc[128] = "\0";
		unsigned char missionType = packet.Decode1();
		unsigned char prizeType = 0;
		unsigned char winnerType = 0;

		if (missionType != 0) {
			prizeType = packet.Decode1();
			winnerType = packet.Decode1();

			unsigned long param1 = packet.Decode4();
			unsigned long param2 = packet.Decode4();
			std::string text("\0");
			packet.DecodeStr(text);

			CApproachDesc approachDesc(missionType, param1, param2, text.c_str());
			char* pDesc = approachDesc.GetDesc();

			if (pDesc) {
				strcpy(desc, pDesc);
			}
		}

		GOLFDOC()->SetApproachMissionType((eApproachMissionType)missionType);
		GOLFDOC()->SetApproachPrizeType((eApproachPrizeType)prizeType);
		GOLFDOC()->SetApproachWinnerType((eApproachWinnerType)winnerType);
		GOLFDOC()->SetApproachMissionDesc(desc);
		break;
	}
	case 0x148: {
		unsigned long winnerUid = 0xFFFFFFFF;

		GOLFDOC()->ClearApproachCurHoleResult();

		unsigned char count = packet.Decode1();

		for (unsigned char i = 0; i < count; i++) {
			sApproachResultData result;
			packet.DecodeBuffer(&result, sizeof(result));

			if (result.m_unk09 == 1) {
				winnerUid = result.m_uid;
			}

			if (result.m_uid == CSharedDoc::Instance()->m_userInfo.guid) {
				GOLFDOC()->AddApproachTotalPrize(result.m_unk0A);
				GOLFDOC()->AddApproachTotalPrize(result.m_unk17);
				GOLFDOC()->AddApproachTotalPrize(result.m_unk16);
			}

			GOLFDOC()->SetApproachCurHoleResult(result);
		}

		if (winnerUid != 0xFFFFFFFF) {
			std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(winnerUid);

			if (it != CSharedDoc::Instance()->m_userMap.end()) {
				WSendPacket request(GCT_REQUEST_USERINFO);
				request.Encode4(it->second.id);
				request.Encode1(5);
				request.Send(SEND_TO_DEFAULT);

				CSharedDoc::Instance()->m_userInfoMod = 1;
			}
		}
		break;
	}
	case 0x146: {
		GOLFDOC()->ClearApproachGameResult();

		unsigned char count = packet.Decode1();

		for (unsigned char i = 0; i < count; i++) {
			sApproachResultData result;
			packet.DecodeBuffer(&result, sizeof(result));
			GOLFDOC()->SetApproachGameResult(result);
		}
		break;
	}
	case 0x149: {
		if (CMessengerInfo::Instance()->GetMyStatus() != MSN::STATUS_BUSY) {
			CMessengerInfo::Instance()->SetMyStatus(MSN::STATUS_ONLINE);
		}

		for (std::vector<sGameResultInfo>::iterator it = CSharedDoc::Instance()->m_gameResultList.begin(); it != CSharedDoc::Instance()->m_gameResultList.end(); ++it) {
			if (it->m_state == 0) {
				it->m_state = 2;
			}
		}

		GetActor("GolfRule") << MsgObject(0, 0xA6, 0, 0, 0, 0, 0);
		CSharedDoc::Instance()->SetAppGiftDownloadState(CSharedDoc::APP_GIFT_DOWNLOAD_READY);
		break;
	}
	case 0x192: {
		std::string imageName;
		CUTIN_ANI_STYPEINFO aniInfo;
		unsigned char result = packet.Decode1();

		if (result == 1) {
			unsigned long skinId = packet.Decode4();
			packet.Decode4();
			packet.DecodeBuffer(&aniInfo, sizeof(aniInfo));

			if (COption::Instance()->gGetCutinFlag() == 1) {
				IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(skinId);
				IFF_STRUCT::S5::sCutinInformation* pCutin = ItemManager()->FindCutinInformation(pSkin->m_typeId);

				if (pCutin) {
					char text[64] = "";
					imageName = pCutin->m_images[0].m_name;

					GetActor("Cutin") << MsgObject(0, 0x78, 4, 0, 0, 0, 0);

					strcmpi(imageName.c_str(), aniInfo.m_image0);

					sprintf(text, "%s", imageName.c_str());

					GetActor("Cutin") << MsgObject(0, 0x24C, (intptr_t)text, (intptr_t)&aniInfo, pCutin->m_condition, skinId, 0);
					break;
				}
			}
		} else {
			int mode = packet.Decode2();
			if (mode == 0) {
				GetActor("GolfRule") << MsgObject(0, 0x24D, 0, (intptr_t)"NON CUTIN_MODE", 0, 0, 0);
			} else {
				GetActor("GolfRule") << MsgObject(0, 0x24D, 0, (intptr_t)"GST_RESPONSE_CUTIN_FAILED", 0, 0, 0);
			}
			break;
		}

		GetActor("GolfRule") << MsgObject(0, 0x24D, 0, (intptr_t)"GST_RESPONSE_CUTIN_FAILED", 0, 0, 0);
		break;
	}
	default:
		return OnPacketCommon(packet);
	}

	return 1;
}

// golftask.cpp:2699
bool CGolfTask::DetermineWhetherToAddActor_GroundItemMan(void) {
	bool bAdd = false;
	switch (CSharedDoc::Instance()->m_roomType) {
	case 0:
	case 1:
	case 3:
	case 4:
	case 5:
	case 6:
		if (CSharedDoc::Instance()->m_playingMode != 2 && CSharedDoc::Instance()->m_gameMode != 1) {
			bAdd = true;
		}
		break;
	}
	return bAdd;
}
