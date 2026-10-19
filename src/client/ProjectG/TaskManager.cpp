// Original: d:\build\custom\temp\code\client\projectg\taskmanager.cpp
#define TASKMANAGER_TASKMANAGER_INLINE
#define TASKMANAGER_LOGININFO_DECL_ONLY
#include <atlbase.h>	// stand-in for the precompiled header: _AtlWinModule initializer
#include "StandingInLine.h"   // its s_standingNames initializer is the unit's first
#include "guilddefine.h"
#include "NetworkSystem.h"
#include "TaskManager.h"
#include "fresh.h"
#include "actor.h"
#include "Packet.h"
#include "ProjectG.h"
#include "ClientSetting.h"
#include "frgaugebar.h"
#include "background.h"
#include "CapturedBg.h"
#include "hackingmanager.h"
#include "netsyncactor.h"
#include "frdesktop.h"
#include "GolfTask.h"
#include "ChatMsg.h"
#include "wview.h"
#include "SharedDoc.h"
#include "fx.h"
#include "work.h"
#include "workalivecheck.h"
#include "TaskMain.h"
#include "scenemanager.h"
#include "SoundManager.h"
#include "messengerdlg.h"
#include "golfball.h"
#include "ShopTask.h"
#include "binstr.h"
#include "westpak.h"
#include "wresrcmng.h"
#include "frform.h"
#include "PangGiftDlg.h"
#include "PrivateTrade.h"
#include "ServerDlg.h"
#include "UccManager.h"
#include "UccLibrary.h"
#include "NetResourceManager.h"
#include "contentsdoc.h"
#include "MessageManager.h"
#include "commonutil.h"
#include "TutorialTask.h"
#include "s5/Utilities.h"
#include "s5/SkinnedTheme.h"
#include "logininfo.h"
#include "user_info.h"
#include "onelineboard.h"
#include "parttidlist.h"
#include "RealMyRoomData.h"
#include "RealMyRoomTask.h"
#include "avatar_task.h"
#include "LobbyTask.h"
#include "Intrusion.h"
#include "itembuffactor.h"
#include "RankingDlg.h"
#include "PointEventDlg.h"
#include "NoticeBoard.h"
#include "DlgQuickStartSetting.h"
#include "WinnerNotice.h"
#include "BongdariShopDlg.h"
#include "ScratchDlg.h"
#include "CardBoxDlg.h"
#include "Treasure.h"
#include "TradeDlg.h"
#include "OfflineTradeDlg.h"
#include "cardsystem.h"
#include "RecycleDlg.h"
#include "ToolTip.h"
#include "mousecursor.h"
#include "SecurityKeyManager.h"
#include "CardRemove.h"
#include "s5/SupplyPack.h"
#include "GMToolkit.h"
#include "FrImageNoticeDlg.h"
#include "TradeHandler.h"
#include "shared/errorcode.h"
#include "shared/thirties/xtdebug.h"
#include "UccCopyDlg.h"
#include <stdlib.h>
#include <mmsystem.h>
#include <string.h>
#include "pg_boot_todo.h"

extern HWND g_hwnd; // the game window (ProjectG.cpp)

// ProjectG.cpp (no header declares it yet)
void TextOut(WView *view, float x, float y, const char *format, ...);

// Queue of GameGuard CSAuth2 answers: GameGuard's callback thread pushes
// them (CTask::PushMsg), the game thread sends them to the server (Process).
class GameGuardMsgHandler {
	LAYOUT_CHECK_FRIEND;

public:
	// One CSAuth2 answer, sent as is (0x10 bytes).
	struct _GG_AUTH_DATA {
		UNKNOWN(0x0, 0x10);
	};

	GameGuardMsgHandler(void);
	~GameGuardMsgHandler(void);
	void Process(void);
	void Push(void *pMsg);

private:
	UNKNOWN(0x0, 0x10);
	std::list<_GG_AUTH_DATA> m_authList; // 0x10
	CRITICAL_SECTION m_cs;               // 0x1C
	bool m_bPushed;                      // 0x34 (answers are waiting)
};
CHECK_SIZE(GameGuardMsgHandler::_GG_AUTH_DATA, 0x10);
BEGIN_LAYOUT_CHECK(GameGuardMsgHandler)
	CHECK_FIELD(GameGuardMsgHandler, m_authList, 0x10);
	CHECK_FIELD(GameGuardMsgHandler, m_cs, 0x1C);
	CHECK_FIELD(GameGuardMsgHandler, m_bPushed, 0x34);
END_LAYOUT_CHECK

extern bool g_bQuit;

// The room record of the old room-list layout (0x7B bytes), which
// CTask::OnPacketCommon 0x45 converts into an sRoomInfo under local content
// 0x5E; 0x15B prints it to the chat.
#pragma pack(push, 1)
struct sRoomInfoLegacy {
	char m_title[0x20];                  // 0x00
	unsigned char m_isPublic : 1;        // 0x20 bit 0
	unsigned char m_intrusionAccess : 1; // 0x20 bit 1
	unsigned char m_state : 1;           // 0x20 bit 2
	unsigned char m_userLimit;           // 0x21
	unsigned char m_userCount;           // 0x22
	unsigned char m_holeCount;           // 0x23
	unsigned char m_gameType;            // 0x24
	unsigned short m_roomNumber;         // 0x25
	unsigned char m_course;              // 0x27
	unsigned long m_trophy;              // 0x28
	unsigned long m_guildUID[2];         // 0x2C (with the names and marks: sRoomInfo's 0x4A-byte guild block at 0x5B)
	char m_guildName[2][21];             // 0x34
	char m_guildMark[2][12];             // 0x5E
	unsigned long m_serverId;            // 0x76, sGameServerInfo::id (0x15B looks the name up)
	unsigned char m_serverType;          // 0x7A, 0-4 (0x15B prints rookie, beginner, intermediate, free 1/2)
};
#pragma pack(pop)

CHECK_SIZE(sRoomInfoLegacy, 0x7B);
CHECK_FIELD(sRoomInfoLegacy, m_serverId, 0x76);
CHECK_FIELD(sRoomInfoLegacy, m_serverType, 0x7A);
CHECK_FIELD(sRoomInfoLegacy, m_userLimit, 0x21);
CHECK_FIELD(sRoomInfoLegacy, m_userCount, 0x22);
CHECK_FIELD(sRoomInfoLegacy, m_holeCount, 0x23);
CHECK_FIELD(sRoomInfoLegacy, m_gameType, 0x24);
CHECK_FIELD(sRoomInfoLegacy, m_roomNumber, 0x25);
CHECK_FIELD(sRoomInfoLegacy, m_course, 0x27);
CHECK_FIELD(sRoomInfoLegacy, m_trophy, 0x28);
CHECK_FIELD(sRoomInfoLegacy, m_guildUID, 0x2C);
CHECK_FIELD(sRoomInfoLegacy, m_guildName, 0x34);
CHECK_FIELD(sRoomInfoLegacy, m_guildMark, 0x5E);

// Loading state shared with the tasks' loaders (only the member this unit's
// code reads so far is named).
struct sLoadParam {
	WList<sActor *> *m_pActorList; // 0x0 (the actors CTask::OnAnotherHand loads)
	int m_actorCount; // 0x4 (their number: the loading gauge's range is m_actorCount * 100)
	int m_progress;   // 0x8 (loading step, shown by the loading gauge)
	bool m_bInited;   // 0xC (the loaded task was initialised: CTaskManager::ProcessTask; until then DisplayTask shows its loading screen)
	bool m_bLoaded;   // 0xD (set by CTask::OnAnotherHand once every actor is loaded)
};
BEGIN_LAYOUT_CHECK(sLoadParam)
	CHECK_FIELD(sLoadParam, m_actorCount, 0x4);
	CHECK_FIELD(sLoadParam, m_progress, 0x8);
	CHECK_FIELD(sLoadParam, m_bInited, 0xC);
	CHECK_FIELD(sLoadParam, m_bLoaded, 0xD);
END_LAYOUT_CHECK
extern sLoadParam g_LoadParam;

// Loading step last shown by CTask::OnProcessWhileLoading (unnamed data,
// file-local in the original: .bss, zero).
static int data_b79bd0;
extern int g_iRecvPacketType;
extern int g_iRecvDisconCode;

// GGC_Wrapper.cpp:119 (no header declares it yet).
void _GGC_CSAuth2(WReceivedPacket &packet);
// TradeDlg.cpp (TradeDlg.h does not declare it yet).
struct sTradeItem;
void SetTradeResultCode(int result, int packetType, sTradeItem *pItem);
// PointEventDlg.cpp (PointEventDlg.h does not declare it yet).
extern CCommodityStatusManager g_CommodityStatusManager;

// Actor names in .data with no public symbol: "GuildActor", "\n",
// "CGatewayActor", "Gateway" (the gateway actor's name, [3]).
extern "C" const char *data_aa04ec[4];

static const char *s_guildActorName = "GuildActor";

// objectfactory.h:103 (an inline of objectfactory.h's class body that the
// compiler did not inline; the linker kept this object's copy. It belongs in
// ObjectFactory.h behind a TASKMANAGER_OBJECTFACTORY_INLINE macro.)
IObject* CObjectFactory::CreateObject(const char* name) {
	ObjectFunctorMap::iterator it = m_objectFunctors.find(name);
	if (it != m_objectFunctors.end()) {
		return it->second();
	}
	return 0;
}

// taskmanager.cpp:204
GameGuardMsgHandler::GameGuardMsgHandler(void)
	: m_bPushed(false) {
	InitializeCriticalSection(&m_cs);
}

// taskmanager.cpp:208
GameGuardMsgHandler::~GameGuardMsgHandler(void) {
	DeleteCriticalSection(&m_cs);
}

// taskmanager.cpp:213
void GameGuardMsgHandler::Process(void) {
	if (!m_bPushed) {
		return;
	}

	std::list<_GG_AUTH_DATA> authList;

	EnterCriticalSection(&m_cs);
	std::list<_GG_AUTH_DATA>::iterator it = m_authList.begin();
	for (; it != m_authList.end(); ++it) {
		authList.push_back(*it);
	}
	m_bPushed = false;
	m_authList.clear();
	LeaveCriticalSection(&m_cs);

	it = authList.begin();
	for (; it != authList.end(); ++it) {
		WSendPacket packet(GCT_RESPONSE_GGCSAUTH);
		packet.EncodeBuffer(&*it, sizeof(_GG_AUTH_DATA));
		packet.Send(SEND_TO_DEFAULT);
		CProjectG::Instance()->GameGuardLog("CSAuth2 responded.");
	}
}

// taskmanager.cpp:243
void GameGuardMsgHandler::Push(void *pMsg) {
	EnterCriticalSection(&m_cs);
	m_authList.push_back(*(_GG_AUTH_DATA *)pMsg);
	m_bPushed = true;
	LeaveCriticalSection(&m_cs);
}

// taskmanager.cpp:256
GameGuardMsgHandler g_gameGuardMsgHandler;

// shareddoc.h:467 (an inline of CSharedDoc's class body; sCardStack is
// incomplete in SharedDoc.h, so the copy this unit emitted is defined here)
void CSharedDoc::ClearCardStack(void) {
	m_cardList.clear();
}

// shareddoc.h:551
void CSharedDoc::SetBsUsableTimes(short times) {
	m_userInfo.bsUsableTimes = times;
}

// shareddoc.h:553
void CSharedDoc::SetBsUsableBonusTimes(short times) {
	m_userInfo.bsUsableBonusTimes = times;
}

// shareddoc.h:555
void CSharedDoc::SetBsRemainedBonusTimes(short times) {
	m_userInfo.bsRemainedBonusTimes = times;
}

// shareddoc.h:622
void CSharedDoc::SetFlagBlock(unsigned long flagBlock) {
	m_userInfo.flagBlock = flagBlock;
}

// taskmanager.cpp:260
void RecvRSS0(int)
{
}

// taskmanager.cpp:264
const WRTTI CTask::m_RTTI("CTask", &IObject::m_RTTI);

IObject *CTaskMakeInstance(void) {
	return new CTask;
}

struct __sCTask {
	__sCTask(void) {
		ObjectFactory().AddObjectFunctor(CTaskMakeInstance, "CTask");
	}
};
static __sCTask s_registerCTask;

// taskmanager.cpp:268
IActor *CTask::ms_pDummyActor = ActorFactory().CreateActor("Dummy");

// taskmanager.cpp:506
CTask::CTask(void)
	: m_unknownC(false), m_actorList(16, 16), m_mainActor(NULL), m_pDoc(NULL), m_unknown38(true),
	  m_loadingBackGround(""), m_bProgressBar(true), m_unknown59(true), m_pLoadingFresh(NULL), m_tipDlgState(0) {
	m_stateFlags = 0;
	m_pause = false;
	m_stateFlags = 0;
	m_netSyncTime = 0;
	m_bRecieveWhisper = true;
}

// taskmanager.cpp:516
CTask::~CTask(void) {
	sActor *pActor = m_actorList.Start();
	while (pActor) {
		delete pActor->m_pActor;
		delete[] (char *)pActor;
		pActor = m_actorList.Next();
	}
	m_actorList.Reset();

	if (m_pDoc) {
		delete m_pDoc;
		m_pDoc = NULL;
	}
	if (m_pLoadingFresh) {
		delete m_pLoadingFresh;
		m_pLoadingFresh = NULL;
	}

	if (CFx::Instance()) {
		CFx::Instance()->ClearFxKeeper();
	}
}

// taskmanager.cpp:540
void CTask::SetMainActor(IActor *pActor) {
	m_mainActor = pActor;
}

// taskmanager.cpp:546
bool CTask::OnTipDlgResult(int result, FrForm *pForm) {
	m_pTipForm = 0;

	m_tipDlgState = 0;

	return true;
}

// taskmanager.cpp:560
FrForm *CTask::LoadTipText(int, int) {
	cFile *pFile = g_resrcmng->GetCFile("tip.txt", 0xFFFF);
	if (pFile == NULL) {
		return NULL;
	}

	std::list<TIP_ITEM> tipList;
	char line[0x2000];
	while (pFile->Scan("%n", line)) {
		int len = 0;
		for (int i = 0; line[i]; i++, len++) {
			if (line[i] == '\\' && line[i + 1] == 'n') {
				line[len] = '\n';
				i++;
			} else {
				line[len] = line[i];
			}
		}
		line[len] = 0;

		int lineLen = strlen(line);
		cTokenV token;
		token.Init(line, lineLen);

		char *pNumber;
		token.GetTokenWithoutFail(&pNumber, " \t", 2);
		if (token.GetPos() + 1 > lineLen) {
			CloseCFile(pFile);
			return NULL;
		}

		TIP_ITEM item;
		item.m_number = atoi(pNumber);
		item.m_text = &line[token.GetPos()];

		tipList.push_back(item);
	}
	CloseCFile(pFile);

	int index = rand() % tipList.size() + 1;
	FrForm *pForm = CreateForm<FrForm>(m_pLoadingFresh->GetManager(), this, "TIP", NULL);
	if (index && pForm) {
		std::list<TIP_ITEM>::iterator it;
		for (it = tipList.begin(); it != tipList.end(); ++it) {
			if (index <= 1) {
				break;
			}
			index--;
		}

		pForm->SetMessage(it->m_text.c_str(), false);
		pForm->EnableDrag(false);
		pForm->Open((FRESH_RESULT_CALLBACK)&CTask::OnTipDlgResult, 8);
		return pForm;
	}

	return NULL;
}

// taskmanager.cpp:643
void CTask::SetProgressBar(bool show) {
	m_bProgressBar = show;
}

// taskmanager.cpp:648
void CTask::SetLoadingBackGround(const std::string &backGround) {
	m_loadingBackGround = backGround;
}

// taskmanager.cpp:653
IActor *CTask::AddActor(const char *className, const char *name, int layer, const std::string &str) {
	IActor *pActor = ActorFactory().CreateActor(className);
	if (pActor == NULL) {
		return NULL;
	}

	pActor->m_pDocument = m_pDoc;
	pActor->m_pTask = this;

	sActor *pEntry = (sActor *)new char[strlen(className) + sizeof(sActor)];
	pEntry->m_pActor = pActor;
	pEntry->m_name = name;
	pEntry->m_pActor->RemoveSkipList(layer);

	pEntry->m_unknown4 = false;

	m_actorList.AddItem(pEntry, pEntry->m_name, false);
	if (!str.empty()) {

		pActor->HandleMsg(MsgObject(NULL, 0x2712, (intptr_t)str.c_str(), 0, 0, 0, 0));
	}

	return pActor;
}

// taskmanager.cpp:678
IActor *CTask::GetActor(const char *name) {
	sActor *pActor = m_actorList.Find(name);

	if (pActor == NULL) {
		return NULL;
	}

	return pActor->m_pActor;
}

// taskmanager.cpp:703
void SendPacket(unsigned char value) {
	WSendPacket packet(GCT_REPLAY_OFFLINE);
	packet.Encode1(value);
	packet.Send(SEND_TO_DEFAULT);
}

// taskmanager.cpp:710
void CTask::OnAnotherHand(int nWork, void *pData) {
	if (nWork == 0) {
		sLoadParam *pParam = (sLoadParam *)pData;

		sActor *pActor = pParam->m_pActorList->Start();






		while (pActor) {

			unsigned long startTime = timeGetTime();

			if (((__rtti_obj = this) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI) : false) && OnlinePlay()) {
				switch (CSharedDoc::Instance()->m_roomType) {
				case 4:
				case 5:
				case 6:
				case 9:
				case 10:
					PG_BOOT_TODOF(("taskmanager.cpp:710 empty case %#x (of 4, 5, 6, 9, 10)", (unsigned)(CSharedDoc::Instance()->m_roomType)));
					break;
				default:
					if (pParam->m_progress % 3 == 0) {
						int value;
						if (pParam->m_progress == 23)
							value = 10;
						else
							value = pParam->m_progress / 3 + 2;
						SendPacket(value);
					}
					break;
				}
			}

			pActor->m_pActor->OnLoad();

			unsigned long endTime = timeGetTime();




			pActor = pParam->m_pActorList->Next();
			pParam->m_progress++;
		}






		pParam->m_bLoaded = true;
	}
}

// taskmanager.cpp:768
void CTask::Load(void) {
	g_LoadParam.m_pActorList = &m_actorList;
	g_LoadParam.m_actorCount = m_actorList.GetCount();

	m_pLoadingFresh = new Fresh;
	m_tipDlgState = NULL;

	if (m_loadingBackGround != "") {
		m_pLoadingFresh->Init("loading.xml", false, m_loadingBackGround.c_str());
	} else {
		m_pLoadingFresh->Init("loading.xml", false, NULL);
	}

	m_pLoadingFresh->OpenLayout("LOADING", NULL, true);

	if (m_unknown59) {
		m_pTipForm = LoadTipText(0, 0);
		if (m_pTipForm) {
			m_tipDlgState = DYNAMIC_CAST(FrGaugeBar, m_pTipForm->FindChildByName("progress"));
			if (m_tipDlgState) {
				m_tipDlgState->SetRange(0, g_LoadParam.m_actorCount * 100, 0);
				m_tipDlgState->SetDestPos(100, false);
			}
		}
	}

	if (m_unknownC) {
		OnAnotherHand(0, &g_LoadParam);
	} else {
		m_anotherHand.Work(this, 0, &g_LoadParam, false, 0);
	}
}

// taskmanager.cpp:842
void CTask::ApplyScreenSize(void) {
	if (COption::Instance()) {
		if (m_unknown38) {
			COption::Instance()->vApplyLobbyScreenSize();
		} else {
			COption::Instance()->vApplyGameScreenSize();
		}
	}
}

// taskmanager.cpp:857
void CTask::Init(const char *backGround) {
	if (m_pLoadingFresh) {
		delete m_pLoadingFresh;
		m_pLoadingFresh = NULL;
	}

	sActor *pActor = m_actorList.Start();
	while (pActor) {

		pActor->m_pActor->OnInit();
		pActor = m_actorList.Next();
	}

	if (!IsInited()) {

		m_stateFlags.Enable(2);
		OnInitFinished();
	}

	g_pFresh->GetDesktop()->Init(g_view->GetWidth(), g_view->GetHeight());

	g_pFresh->GetDesktop()->SetWallPaper(backGround, true);

	if (!((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&CGolfTask::m_RTTI) : false)) {

		CChatMsg::Instance()->SetActive(true, true, true);
	}
}

// taskmanager.cpp:884
void CTask::SetInited(bool inited) {
	if (inited) {
		m_stateFlags.Enable(2);
	} else {
		m_stateFlags.Disable(2);
	}
}

// taskmanager.cpp:892
void CTask::PreLoadInit(void) {




	sActor *pActor = m_actorList.Start();
	while (pActor) {

		pActor->m_pActor->OnPreLoadInit();
		pActor = m_actorList.Next();
	}
}

// taskmanager.cpp:906
void CTask::Reload(void) {
	sActor *pActor = m_actorList.Start();
	while (pActor) {

		pActor->m_pActor->OnReload();
		pActor = m_actorList.Next();
	}
}

// taskmanager.cpp:916
void CTask::Destroy(void) {

	g_pFresh->CloseLayout();

	sActor *pActor = m_actorList.Start();
	while (pActor) {


		pActor->m_pActor->OnDestroy();
		pActor = m_actorList.Next();
	}
}

// taskmanager.cpp:930
void CTask::OnProcessWhileLoading(float dt) {
	if (m_pLoadingFresh) {
		m_pLoadingFresh->OnProcess(dt);

		if (data_b79bd0 != g_LoadParam.m_progress) {
			data_b79bd0 = g_LoadParam.m_progress;
			if (m_bProgressBar && m_tipDlgState) {
				m_tipDlgState->SetPos(g_LoadParam.m_progress * 100 - 100);
				m_tipDlgState->SetDestPos(g_LoadParam.m_progress * 100, false);
			}
		}
	}
}

// taskmanager.cpp:952
void CTask::PushMsg(void *pMsg) {
	g_gameGuardMsgHandler.Push(pMsg);
}

// taskmanager.cpp:990
void CTask::PreserveBack(const char *layoutName) {


	for (sActor *pActor = m_actorList.Start(); pActor; pActor = m_actorList.Next())
		pActor->m_pActor->OnPreserveBack(layoutName);
}

// taskmanager.cpp:998
void CTask::RestorePreserved(void) {


	for (sActor *pActor = m_actorList.Start(); pActor; pActor = m_actorList.Next())
		pActor->m_pActor->OnRestorePreserved();
}

// taskmanager.cpp:1006
void CTask::ProcessNetSyncActor(bool active, float elapsed, int frame) {




	sActor *pActor = m_actorList.Start();
	while (pActor) {
		if (!pActor->m_pActor->Skipped()) {


			NetSyncActor *pNetSyncActor = (NetSyncActor *)pActor->m_pActor->DynamicCast(&NetSyncActor::m_RTTI);

			if (pNetSyncActor) {

				pNetSyncActor->NSA_Step(active, elapsed, frame);
			}
		}

		pActor = m_actorList.Next();
	}
}

// taskmanager.cpp:1029
void CTask::Process(float dt) {
	g_gameGuardMsgHandler.Process();

	m_mainActor->OnProcess(dt);

	if (m_pause) {
		return;
	}

	sActor *pActor = m_actorList.Start();
	while (pActor) {
		if (pActor->m_pActor != m_mainActor && !pActor->m_pActor->Skipped()) {
			pActor->m_pActor->OnProcess(dt);
		}
		if (HackingManager::Instance()) {
			HackingManager::Instance()->CheckCrypticValue();
		}
		pActor = m_actorList.Next();
	}

	bool bReset = false;
	if (GolfBall().m_preDataIndex == 0) {
		bReset = true;
	}

	if (dt > 0.0f) {
		m_netSyncTime += dt;
		if (bReset) {
			m_netSyncTime = 0.022f;
		}

		while (m_netSyncTime > 0.02f) {
			if (GolfBall().m_preDataIndex == -1 || GolfBall().m_preDataIndex == GolfBall().m_preDataCount) {
				ProcessNetSyncActor(false, 0.02f, GolfBall().m_preDataIndex);
			} else if (GolfBall().m_preDataIndex == 0) {
				ProcessNetSyncActor(true, 0.02f, GolfBall().m_preDataIndex);
			} else {
				GolfBall();
			}
			m_netSyncTime -= 0.02f;
		}
	}

	if (CSceneManager::Instance() && !CCapturedBg::Instance()->IsCaptured()
		&& !((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&CShopTask::m_RTTI) : false)) {
		CSceneManager::Instance()->Process(dt);
	}
}

// taskmanager.cpp:1136
void CTask::OnDisplayWhileLoading(void) {
	if (m_pLoadingFresh) {
		m_pLoadingFresh->OnDisplay(true);
	}
}

// taskmanager.cpp:1142
void CTask::Display(void) {
	for (int priority = 0; priority <= 5; priority++) {
		void *pos;
		sActor *pActor = m_actorList.Start(pos);
		while (pActor) {
			if (pActor->m_pActor && pActor->m_pActor->GetDisplayPriority() == priority && !pActor->m_pActor->Skipped()) {
				pActor->m_pActor->OnDisplay();
			}
			pActor = m_actorList.Next(pos);
		}

		if (priority == 2) {
			if (!(CCapturedBg::Instance() && CCapturedBg::Instance()->IsCaptured())) {
				if (CSceneManager::Instance()) {
					CSceneManager::Instance()->Prerender(g_view);
					CSceneManager::Instance()->Render(g_view, 0xffffffff);
				}

				if (!m_stateFlags.GetFlag(1)) {
					g_view->Clear(0, 2);
				}
			}
		} else if (priority == 4 || priority == 1) {
			g_view->Flush(0);
		}
	}
}

// taskmanager.cpp:1219
void CTask::SendGlobalMsg(MsgObject msg) {
	IActor *pMainActor = m_mainActor;
	if (pMainActor) {
		pMainActor->HandleMsg(msg);
	}

	void *pos;
	sActor *pActor = m_actorList.Start(pos);
	while (pActor) {
		if (pActor->m_pActor != m_mainActor) {
			if (pActor->m_pActor) {
				pActor->m_pActor->HandleMsg(msg);
			}
		}
		pActor = m_actorList.Next(pos);
	}
}

// taskmanager.cpp:1237
void CTask::SendMsgToMainActor(MsgObject msg) {
	IActor *pMainActor = m_mainActor;
	if (pMainActor) {
		pMainActor->HandleMsg(msg);
	}
}

// The reward record of packet 0x107 (opening a New Year's money envelope,
// 0x14 bytes, packed). Its original name and header are not known.
#pragma pack(push, 1)
struct sEnvelopeReward {
	UNKNOWN(0x0, 0x4);
	unsigned long m_value;               // 0x4, the item's type id, or the pang received when no item has that id
	UNKNOWN(0x8, 0xC);
	int m_pang;                          // 0xC, the player's pang afterwards
	UNKNOWN(0x10, 0x14);
};
#pragma pack(pop)
CHECK_SIZE(sEnvelopeReward, 0x14);
CHECK_FIELD(sEnvelopeReward, m_value, 0x4);
CHECK_FIELD(sEnvelopeReward, m_pang, 0xC);

// CTask::OnPacketCommon case 0x112 (taskmanager.cpp:1383-1387): notice id -> text
struct sNoticeText {
	unsigned long id;
	const char *text;
};

extern "C" {
static const sNoticeText data_a13d4c[3] = {
	// "Your level is too low. Equip it after levelling up."
	{ 0, "\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb7\xb9\xba\xa7\xbe\xf7 \xc8\xc4\xbf\xa1 \xc2\xf8\xbf\xeb\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0." },
	// "Your level is too low. Use it after levelling up."
	{ 1, "\xb7\xb9\xba\xa7\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb7\xb9\xba\xa7\xbe\xf7 \xc8\xc4\xbf\xa1 \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xca\xbd\xc3\xbf\xc0." },
	// "The award ceremony has not finished; the course cannot be changed. Try again later."
	{ 2, "\xbd\xc3\xbb\xf3\xbd\xc4\xc0\xcc \xbe\xc6\xc1\xf7 \xc1\xbe\xb7\xe1\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xc6\xbc\xad, \xc4\xda\xbd\xba\xb8\xa6 \xba\xaf\xb0\xe6 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3 \xc8\xc4 \xba\xaf\xb0\xe6\xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0." },
};
}

// taskmanager.cpp:1246
int CTask::OnPacketCommon(WReceivedPacket &packet) {
	packet.Reset();
	unsigned short type = packet.Decode2();
	g_iRecvPacketType = type;

	switch (g_iRecvPacketType) {
	case 0x1d3:
		// taskmanager.cpp:1259
		{
			unsigned long sendType = packet.Decode4();
			unsigned long value = packet.Decode4();
			// taskmanager.cpp:1262
			WSendPacket send(sendType);
			send.Encode4(CSharedDoc::Instance()->m_userInfo.guildUID);
			send.Encode4(value);
			send.Send(SEND_TO_MSN);
		}
		break;
	case 0x187:
		// taskmanager.cpp:1280
		{
			IActor *pActor = AfxGetTask()->GetActor("PrivateTrade");
			if (!pActor) {
				break;
			}
			// taskmanager.cpp:1283
			CPrivateTrade *pTrade = DYNAMIC_CAST(CPrivateTrade, pActor);
			pTrade->OnPacket(&packet);
		}
		break;
	case 0x19c:
		// taskmanager.cpp:1290
		AfxGetTask()->GetMainActor() << MsgObject(0, 2, 0, 0, 0, 0, 0);
		{
			// taskmanager.cpp:1293
			S5::FrSupplyPackDlg *pDlg = 0;
			FrWnd *pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildForm("FrSupplyPackDlg");
			// taskmanager.cpp:1294
			if (pWnd && ((__rtti_obj = pWnd) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&S5::FrSupplyPackDlg::m_RTTI) : false)) {
				// taskmanager.cpp:1295
				pDlg = DYNAMIC_CAST(S5::FrSupplyPackDlg, pWnd);
			}
			// taskmanager.cpp:1299
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:1300
			if (result == 0) {
				// taskmanager.cpp:1302
				if (pDlg == 0) {
					break;
				}
				// taskmanager.cpp:1304
				pDlg->Close(FrCANCEL, true);
				// taskmanager.cpp:1305
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), NULL, "notify", 0);
				// taskmanager.cpp:1306
				pForm->SetMessage("\xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.", false);
				// taskmanager.cpp:1307
				pForm->Open((FRESH_RESULT_CALLBACK)0, 3);
				// taskmanager.cpp:1309
				break;
			}
			// taskmanager.cpp:1312
			unsigned long serial = packet.Decode4();
			// taskmanager.cpp:1313
			unsigned long typeId = packet.Decode4();
			// taskmanager.cpp:1314
			unsigned long count = packet.Decode4();
			// taskmanager.cpp:1320
			std::list<sItemInfo>::iterator it = std::find(CSharedDoc::Instance()->m_itemInfoList.begin(), CSharedDoc::Instance()->m_itemInfoList.end(), serial);
			// taskmanager.cpp:1322
			if (it != CSharedDoc::Instance()->m_itemInfoList.end()) {
				// taskmanager.cpp:1324
				it->m_count--;
				// taskmanager.cpp:1325
				if (it->m_count <= 0)
					// taskmanager.cpp:1326
					CSharedDoc::Instance()->m_itemInfoList.erase(it);
				// taskmanager.cpp:1328
				AfxGetTask()->GetMainActor() << MsgObject(0, 0xDD, 0, 0, 0, 0, 0);
			}
			// taskmanager.cpp:1332
			if ((typeId & 0xFC000000) != 0x14000000) {
				break;
			}
			// taskmanager.cpp:1335
			std::list<sItemInfo>::iterator gear = std::find(CSharedDoc::Instance()->m_myGearList.begin(), CSharedDoc::Instance()->m_myGearList.end(), typeId);
			// taskmanager.cpp:1339
			if (gear != CSharedDoc::Instance()->m_myGearList.end()) {
				// taskmanager.cpp:1342
				int added = count - gear->m_count;
				gear->m_count = (short)count;
				// taskmanager.cpp:1346
				if (pDlg)
					// taskmanager.cpp:1347
					pDlg->UpdateData(added);
			}
		}
		break;
	case 0x11b:
		{
			// taskmanager.cpp:1361
			unsigned char type = packet.Decode1();
			// taskmanager.cpp:1362
			unsigned char value = packet.Decode1();
			// taskmanager.cpp:1364
			if (type == 3) {
				// taskmanager.cpp:1366
				packet.DecodeBuffer(CSharedDoc::Instance()->m_tutorialData, 0xC);
			}
			// taskmanager.cpp:1369
			if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&CTutorialTask::m_RTTI) : false) {
				// taskmanager.cpp:1371
				AfxPostMsg(0, "TutorialMain", 0x151, type, value, 0, 0);
			}
		}
		break;
	case 0x112:
		{
			// taskmanager.cpp:1380
			unsigned long id = packet.Decode4();
			// taskmanager.cpp:1382
			if (id >= 3) {
				break;
			}
			// taskmanager.cpp:1388
			if (id != data_a13d4c[id].id) {
				break;
			}
			// taskmanager.cpp:1394
			FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			// taskmanager.cpp:1395
			pForm->SetTimeLimit(3.0f);
			// taskmanager.cpp:1396
			pForm->SetMessage(data_a13d4c[id].text, false);
			pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
		}
		break;
	case 0xf0:
		{
			// taskmanager.cpp:1405
			unsigned char value = packet.Decode1();
			// taskmanager.cpp:1406
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x6B, value, 0, 0, 0, 0));
		}
		// taskmanager.cpp:1408
		break;
	case 0x75:
		{
			// taskmanager.cpp:1414
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2B, 1, 0, 0, 0, 0));
			// taskmanager.cpp:1416
			CSharedDoc::Instance()->m_pangPercent = packet.Decode4();
			// taskmanager.cpp:1417
			if (GalleryMode()) {
				// taskmanager.cpp:1419
				CSharedDoc::Instance()->ClearRivalVars(true);
				break;
			}
			// taskmanager.cpp:1424
			CSharedDoc::Instance()->ClearRivalVars(false);
			// taskmanager.cpp:1427
			CSharedDoc *pDoc = CSharedDoc::Instance();
			// taskmanager.cpp:1429
			unsigned int caddieId = pDoc->m_myCaddieId;
			// taskmanager.cpp:1431
			std::map<unsigned int, sCaddieInfo>::iterator it = pDoc->m_caddieMap.find(caddieId);
			// taskmanager.cpp:1433
			if (it != pDoc->m_caddieMap.end()) {
				// taskmanager.cpp:1435
				IFF_STRUCT::sCaddie *pCaddie = ItemManager()->FindCaddie(it->second.typeId);
				// taskmanager.cpp:1437
				if ((it->second.flags & 2) && it->second.days == 0 && pCaddie && pCaddie->fee > 0)
					// taskmanager.cpp:1438
					caddieId = 0;
			}
			// taskmanager.cpp:1446
			WSendPacket send((enumClientPacket)0xC);
			// The equipped character, caddie, club set and ball.
			struct {
				unsigned long characterId;  // 0x0
				unsigned long caddieId;     // 0x4
				unsigned long clubId;       // 0x8
				unsigned long ballId;       // 0xC
			} equip;
			// taskmanager.cpp:1449
			equip.characterId = CSharedDoc::Instance()->m_myCharacterId;
			// taskmanager.cpp:1450
			CSharedDoc::Instance()->m_myCaddieId = caddieId;
			// taskmanager.cpp:1451
			equip.caddieId = caddieId;
			equip.clubId = CSharedDoc::Instance()->m_myClubId;
			// taskmanager.cpp:1452
			equip.ballId = CSharedDoc::Instance()->m_myBallId;
			// taskmanager.cpp:1454
			send.Encode1(7);
			// taskmanager.cpp:1455
			send.EncodeBuffer(&equip, sizeof(equip));
			// taskmanager.cpp:1456
			send.Send(SEND_TO_DEFAULT);
			// taskmanager.cpp:1458
			if (CSharedDoc::Instance()->m_roomInfo.m_roomMode == 0xE) {
				// taskmanager.cpp:1460
				CTaskManager::Instance()->CreateTask("CLobbyTask", "CGolfDoc", false);
				// taskmanager.cpp:1461
				CSharedDoc::Instance()->m_roomInfo.m_gameType = 4;
			}
		}
		// taskmanager.cpp:1463
		break;
	case 0x74:
		{
			// taskmanager.cpp:1469
			sUserInfo userInfo;
			// taskmanager.cpp:1470
			sUserInfoExt userInfoExt;
			// taskmanager.cpp:1476
			packet.Decode1();
			// taskmanager.cpp:1477
			int count = packet.Decode1();
			// taskmanager.cpp:1479
			if (!GalleryMode() && IsMassGame(CSharedDoc::Instance()->m_roomInfo.m_gameType)) {
				// taskmanager.cpp:1481
				CSharedDoc::Instance()->m_userInfo.eventBits.eventFlag3 = packet.Decode1();
				// taskmanager.cpp:1482
				CSharedDoc::Instance()->m_userInfo.eventBits.eventFlag4 = packet.Decode1();
				// taskmanager.cpp:1483
				CSharedDoc::Instance()->m_userInfo.eventBits.eventFlag5 = packet.Decode1();
				// taskmanager.cpp:1485
				if (IsLocalContent((localContentType_t)0x51)) {
					// taskmanager.cpp:1488
					_SYSTEMTIME serverTime;
					packet.DecodeBuffer(&serverTime, sizeof(serverTime));
					// taskmanager.cpp:1489
					CSharedDoc::Instance()->SetServerTime(serverTime);
					// taskmanager.cpp:1491
					CCardManager::Instance()->m_playerIndexMap[0] = 0;
					// taskmanager.cpp:1493
					CSharedDoc *pDoc = CSharedDoc::Instance();
					std::map<unsigned int, sCharacterInfo>::iterator it = pDoc->m_characterMap.find(pDoc->m_myCharacterId);
					// taskmanager.cpp:1494
					if (it != pDoc->m_characterMap.end()) {
						// taskmanager.cpp:1496
						CCardManager::Instance()->m_characterInfo[0] = it->second;
					}
				}
				// taskmanager.cpp:1500
				CSharedDoc::Instance()->SetEquipCharCaddieInfoFromMyInfo();
			} else {
				// taskmanager.cpp:1504
				CPartTidList partTidList;
				// taskmanager.cpp:1510
				int cardIndex = 1;
				if (!IsLocalContent((localContentType_t)0x70) && IsLocalContent((localContentType_t)0x51)) {
					// taskmanager.cpp:1514
					for (int kind = 1; kind < 4; kind++) {
						// taskmanager.cpp:1515
						CSharedDoc::Instance()->m_cardPeriodList[kind].clear();
					}
				}
				// taskmanager.cpp:1519
				for (int i = 0; i < count; i++) {
					// taskmanager.cpp:1521
					sUserInfo *pInfo;
					if (IsLocalContent((localContentType_t)0x70)) {
						// taskmanager.cpp:1523
						packet.DecodeBuffer(&userInfoExt, sizeof(sUserInfoExt));
						// taskmanager.cpp:1524
						userInfo = userInfoExt;
						// taskmanager.cpp:1525
						pInfo = &userInfoExt;
					} else {
						// taskmanager.cpp:1529
						packet.DecodeBuffer(&userInfo, sizeof(sUserInfo));
						// taskmanager.cpp:1530
						pInfo = &userInfo;
					}
					CSharedDoc::Instance()->m_roomPlayers[i] = *pInfo;
					// taskmanager.cpp:1534
					if (IsLocalContent((localContentType_t)0x57)) {
						// taskmanager.cpp:1536
						RequestUccInfo(userInfo);
					}
					// taskmanager.cpp:1539
					if (IsLocalContent((localContentType_t)0x51) && !IsLocalContent((localContentType_t)0x70)) {
						// taskmanager.cpp:1542
						CCardManager::Instance()->m_playerIndexMap[i] = 0xFF;
						// taskmanager.cpp:1545
						_SYSTEMTIME serverTime;
						packet.DecodeBuffer(&serverTime, sizeof(serverTime));
						// taskmanager.cpp:1546
						CSharedDoc::Instance()->SetServerTime(serverTime);
						// taskmanager.cpp:1549
						bool isMe = false;
						if (stricmp(userInfo.m_name, CSharedDoc::Instance()->m_userInfo.nickname) == 0) {
							// taskmanager.cpp:1551
							isMe = true;
						}
						// taskmanager.cpp:1554
						int cardCount = packet.Decode1();
						// taskmanager.cpp:1556
						sSCardAvilityPeriodInfo info;
						memset(&info, 0, sizeof(info));
						// taskmanager.cpp:1562
						for (int card = 0; card < cardCount; card++) {
							// taskmanager.cpp:1564
							packet.DecodeBuffer(&info, sizeof(info));
							// taskmanager.cpp:1565
							if (!isMe) {
								// taskmanager.cpp:1570
								CSharedDoc::Instance()->m_cardPeriodList[cardIndex].push_back(info);
							}
						}
						// taskmanager.cpp:1574
						if (isMe) {
							// taskmanager.cpp:1576
							CCardManager::Instance()->m_playerIndexMap[i] = 0;
							// taskmanager.cpp:1577
							CCardManager::Instance()->m_characterInfo[0] = userInfo.m_characterInfo;
						} else {
							// taskmanager.cpp:1581
							CCardManager::Instance()->m_characterInfo[cardIndex] = userInfo.m_characterInfo;
							// taskmanager.cpp:1582
							CCardManager::Instance()->m_playerIndexMap[i] = (unsigned char)cardIndex;
							cardIndex++;
						}
					}
					// taskmanager.cpp:1590
					partTidList.SetTids(&CSharedDoc::Instance()->m_roomPlayers[i].m_characterInfo, userInfo.m_level);
					// taskmanager.cpp:1592
					IFF_STRUCT::sCaddie *pCaddie = ItemManager()->FindCaddie(CSharedDoc::Instance()->m_roomPlayers[i].m_caddieInfo.typeId);
					// taskmanager.cpp:1594
					if ((pCaddie && CSharedDoc::Instance()->m_roomPlayers[i].m_level < (pCaddie->m_level & 0x7F))
						|| ((CSharedDoc::Instance()->m_roomPlayers[i].m_caddieInfo.flags & 2) && CSharedDoc::Instance()->m_roomPlayers[i].m_caddieInfo.days == 0 && pCaddie->fee > 0)) {
						// taskmanager.cpp:1596
						CSharedDoc::Instance()->m_roomPlayers[i].m_caddieInfo.typeId = 0;
					}
					// taskmanager.cpp:1599
					IFF_STRUCT::sClubSet *pClubSet = ItemManager()->FindClubSet(CSharedDoc::Instance()->m_roomPlayers[i].m_clubSet.typeId);
					// taskmanager.cpp:1601
					if (pClubSet
						&& ((CSharedDoc::Instance()->m_roomPlayers[i].m_level < (pClubSet->m_level & 0x7F) && !(pClubSet->m_level & 0x80))
							|| (CSharedDoc::Instance()->m_roomPlayers[i].m_level > (pClubSet->m_level & 0x7F) && (pClubSet->m_level & 0x80)))) {
						// taskmanager.cpp:1602
						CSharedDoc::Instance()->m_roomPlayers[i].m_clubSet.typeId = 0x10000000;
					}
				}
			}
			// taskmanager.cpp:1606
			std::list<sSlotInfo>::iterator it = CSharedDoc::Instance()->m_roomPlayerList.begin();
			// taskmanager.cpp:1607
			for (; it != CSharedDoc::Instance()->m_roomPlayerList.end(); ++it) {
				// taskmanager.cpp:1609
				if (it->m_attribute & 0x18) {
					// taskmanager.cpp:1611
					it->m_mannerFlag |= 2;
				} else {
					// taskmanager.cpp:1615
					it->m_mannerFlag &= ~2;
				}
			}
			// taskmanager.cpp:1619
			if (IsLocalContent((localContentType_t)0x51))
				// taskmanager.cpp:1620
				CCardManager::Instance()->CalcCardPeriodAndStatus();
		}
		break;
	case 0x50: {
		// taskmanager.cpp:1632
		SendMessage(g_hwnd, 0xADB3, 0, 0);
		// taskmanager.cpp:1640
		FrBongdariShopDlg* pDlg = FrStaticCast<FrBongdariShopDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop"));
		// taskmanager.cpp:1642
		if (pDlg) {
			// taskmanager.cpp:1646
			pDlg->Clear();
		}
		// taskmanager.cpp:1649
		if (CUserInfo::Instance()) {
			// taskmanager.cpp:1650
			CUserInfo::Instance()->Clear();
		}
		// taskmanager.cpp:1653
		if (IsLocalContent((localContentType_t)0x58)) {
			// taskmanager.cpp:1656
			CTHunter::Instance()->SetTreasurePoint(0);
		}
		// taskmanager.cpp:1659
		CIconToolTip::GetInstance()->GetInstance()->ClearToolTip();
		// taskmanager.cpp:1661
		SetCurMap(packet.Decode1());
		// taskmanager.cpp:1662
		SetGameType(packet.Decode1());
		// taskmanager.cpp:1663
		SetHoleType(packet.Decode1());
		// taskmanager.cpp:1664
		SetHoles(packet.Decode1());
		// taskmanager.cpp:1665
		CSharedDoc::Instance()->m_matchTypeId = packet.Decode4();
		// taskmanager.cpp:1666
		SetShotTimeLimit(packet.Decode4());
		// taskmanager.cpp:1667
		SetGameTimeLimit(packet.Decode4());
		// taskmanager.cpp:1670
		if (!IsMassGame()) {
			// taskmanager.cpp:1672
			sUserInfo *pPlayer = CSharedDoc::Instance()->m_roomPlayers;
			for (int i = 0; i < 4; i++, pPlayer++) {
				// taskmanager.cpp:1674
				if (pPlayer->m_guid == MyGuid(false)) {
					break;
				}
			}
		}
		// taskmanager.cpp:1682
		for (unsigned char hole = 0; hole < 18; hole++) {
			// taskmanager.cpp:1684
			CSharedDoc::Instance()->m_holeRandom[hole] = packet.Decode4();
			// taskmanager.cpp:1685
			CSharedDoc::Instance()->m_holePin[hole] = (unsigned char)(CSharedDoc::Instance()->m_holeRandom[hole] % 3);
			// taskmanager.cpp:1687
			if (CSharedDoc::Instance()->m_roomInfo.m_roomMode == 0xE) {
				// taskmanager.cpp:1688
				CSharedDoc::Instance()->m_holeCourse[hole] = packet.Decode1();
			}
			// taskmanager.cpp:1690
			CSharedDoc::Instance()->m_holeOrder[hole] = packet.Decode1();
		}
		// taskmanager.cpp:1694
		memset(CSharedDoc::Instance()->m_courseHoleOn, 0, sizeof(CSharedDoc::Instance()->m_courseHoleOn));
		// taskmanager.cpp:1696
		if (CSharedDoc::Instance()->m_roomInfo.m_roomMode == 0xE) {
			// taskmanager.cpp:1697
			SetCurMap(CSharedDoc::Instance()->m_holeCourse[0]);
		}
		// taskmanager.cpp:1703
		CGimmickContainer *pGimmick = NULL;
		CContentsDoc::Instance()->GetContainer((localContentType_t)0xA3, pGimmick);
		// taskmanager.cpp:1705
		pGimmick->LoadFromPacket(packet);
		// taskmanager.cpp:1709
		if (CSharedDoc::Instance()->m_roomType != 0xA && IsMassGame()) {
			// taskmanager.cpp:1711
			CSharedDoc::Instance()->m_gameStartTime = g_CurrentTime;
		}
		// taskmanager.cpp:1715
		CSharedDoc::Instance()->m_value4D74 = 0;
		CSharedDoc::Instance()->LogGstUpdateHole(0xFFFFFFFF, 0xFF, -1, -1);
		// taskmanager.cpp:1717
		if (IsLocalContent((localContentType_t)0x17)) {
			// taskmanager.cpp:1718
			CSharedDoc::Instance()->SetInitWearHalloweenItem();
		}
		// taskmanager.cpp:1722
		if (CMessengerInfo::Instance()->m_myStatus != MSN::STATUS_BUSY) {
			// taskmanager.cpp:1723
			CMessengerInfo::Instance()->SetMyStatus(MSN::STATUS_ACTIVE);
		}
		// taskmanager.cpp:1725
		for (std::list<sSlotInfo>::iterator it = CSharedDoc::Instance()->m_roomPlayerList.begin(); it != CSharedDoc::Instance()->m_roomPlayerList.end(); ++it) {
			// taskmanager.cpp:1726
			it->m_attribute &= ~4;
		}
		// taskmanager.cpp:1728
		CSharedDoc::Instance()->m_gameOver = false;
		// taskmanager.cpp:1730
		CMouseCursor::Instance()->m_mode = 1;
		// taskmanager.cpp:1731
		CMouseCursor::Instance()->SetActive(false);
		// taskmanager.cpp:1732
		COption::Instance()->vSetMinimumScreenSize(0, 800, 600);
		// taskmanager.cpp:1733
		CTaskManager::Instance()->ChangeTask("CGolfTask", "CGolfDoc", false);
		break;
	}
	case 0x3e: {
		// taskmanager.cpp:1742
		std::string nickName;
		// taskmanager.cpp:1743
		std::string message;
		// taskmanager.cpp:1744
		unsigned char chatType = packet.Decode1();
		// taskmanager.cpp:1745
		std::string text;

		// taskmanager.cpp:1750
		bool bWhisper = (chatType >> 7) != 0;
		// taskmanager.cpp:1751
		chatType &= 0x7F;

		unsigned long color = 0xFFFFFFFF;
		bool removeColor = false;
		// taskmanager.cpp:1754
		packet.DecodeStr(nickName);
		// taskmanager.cpp:1755
		packet.DecodeStr(message);

		// taskmanager.cpp:1757
		WSingleton<CSharedDoc>::Instance()->m_chatManager.FilteringHack(message.c_str(), false);

		bool bIgnore = false;
		// taskmanager.cpp:1760
		switch (chatType) {
		case 0:
			// taskmanager.cpp:1764
			if (strcmp(nickName.c_str(), WSingleton<CSharedDoc>::Instance()->m_userInfo.nickname) == 0) {
				// taskmanager.cpp:1765
				color = 0xFFFFCC00;
			} else {
				// taskmanager.cpp:1768
				std::list<std::string>::iterator it = std::find(WSingleton<CSharedDoc>::Instance()->m_ignoreList.begin(), WSingleton<CSharedDoc>::Instance()->m_ignoreList.end(), nickName);
				// taskmanager.cpp:1769
				if (it != WSingleton<CSharedDoc>::Instance()->m_ignoreList.end()) {
					// taskmanager.cpp:1771
					bIgnore = true;
					// taskmanager.cpp:1772
					break;
				}
			}

			// taskmanager.cpp:1776
			text = nickName + " : \x08" + message;

			// taskmanager.cpp:1780
			removeColor = true;
			if (bWhisper)
				// taskmanager.cpp:1782
				color = 0xFF6EECFB;
			// taskmanager.cpp:1786
			break;

		case 1:
			// taskmanager.cpp:1789
			// "Notice : <nick> is not in this channel."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xc3\xa4\xb3\xce\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			// taskmanager.cpp:1790
			color = 0xFFFF7878;
			// taskmanager.cpp:1791
			break;

		case 5:
			// taskmanager.cpp:1794
			// "Notice : <nick> is not on this server."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			// taskmanager.cpp:1795
			color = 0xFFFF7878;
			// taskmanager.cpp:1796
			break;

		case 2:
			// taskmanager.cpp:1799
			// "Notice : <nick> has left."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.";
			color = 0xFFFF7878;
			// taskmanager.cpp:1803
			WSingleton<CSharedDoc>::Instance()->SetGoStopMode(true);
			// taskmanager.cpp:1804
			break;

		case 8:
			// taskmanager.cpp:1807
			// "Notice : <nick> left, so the strike points go up by 1 point each."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xd8\xbc\xad \xbd\xba\xc6\xae\xb6\xf3\xc0\xcc\xc5\xa9\xc6\xf7\xc0\xce\xc6\xae\xb0\xa1 1\xc6\xf7\xc0\xce\xc6\xae\xbe\xbf \xbf\xc3\xb6\xf3\xb0\xa9\xb4\xcf\xb4\xd9.";
			// taskmanager.cpp:1808
			color = 0xFFFF7878;
			// taskmanager.cpp:1809
			break;

		case 3:
			// taskmanager.cpp:1812
			// "The kick vote was rejected."
			text = "\xb0\xad\xc5\xf0\xbf\xa1 \xb0\xfc\xc7\xd1 \xc5\xf5\xc7\xa5\xb0\xa1 \xba\xce\xb0\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.";
			// taskmanager.cpp:1813
			color = 0xFFFF7878;
			// taskmanager.cpp:1814
			break;

		case 4:
			// taskmanager.cpp:1817
			// "Notice : <nick> cannot receive whispers now."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xb1\xd3\xbc\xd3\xb8\xbb\xc0\xbb \xbc\xf6\xbd\xc5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
			// taskmanager.cpp:1818
			color = 0xFFFF7878;
			// taskmanager.cpp:1819
			break;

		case 6:
			// taskmanager.cpp:1822
			// "Notice : <nick> is not connected."
			text = "\xbe\xcb\xb8\xb2 : " + nickName + " \xb4\xd4\xc0\xba \xc7\xf6\xc0\xe7 \xc1\xa2\xbc\xd3\xc1\xdf\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.";
			// taskmanager.cpp:1823
			color = 0xFFFF7878;
			// taskmanager.cpp:1824
			break;

		case 7:
			// taskmanager.cpp:1827
			text = "\xbe\xcb\xb8\xb2 : " + message;
			// taskmanager.cpp:1828
			color = 0xFFFF7878;
			// taskmanager.cpp:1829
			break;

		case 10:
			// taskmanager.cpp:1846
			// "Notice : <nick> won a <n> pang pouch."
			text = "\xbe\xcb\xb8\xb2 : " + std::string(MakeStr("%s\xb4\xd4\xb2\xb2\xbc\xad %d \xc6\xce \xc1\xd6\xb8\xd3\xb4\xcf\xb8\xa6 \xc8\xb9\xb5\xe6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", nickName.c_str(), packet.Decode4()));
			// taskmanager.cpp:1848
			color = 0xFFFF7878;
			// taskmanager.cpp:1850
			break;

		default:
			// taskmanager.cpp:1853
			text = nickName + " : " + message;
			break;
		}

		// taskmanager.cpp:1857
		if (IsLocalContent((localContentType_t)0x4F) && WSingleton<CSharedDoc>::Instance()->m_playingMode == 2)
			// taskmanager.cpp:1858
			bIgnore = true;

		// taskmanager.cpp:1860
		if (WSingleton<CSharedDoc>::Instance()->m_gameMode != 5) {
			// taskmanager.cpp:1863
			if (!bIgnore) {
				// taskmanager.cpp:1865
				CChatMsg::Instance()->SetChatFacialUser(nickName);
				// taskmanager.cpp:1867
				CChatMsg::Instance()->AddChatMsg(text, color, false, removeColor);
			}
		}
		break;
	}
	case 0x3f:
		// taskmanager.cpp:1875
		{
			std::string text;
			// taskmanager.cpp:1876
			packet.DecodeStr(text);
			// taskmanager.cpp:1877
			CChatMsg::Instance()->AddChatMsg(text, 0xFFFF7878, false, false);
		}
		break;
	case 0x40:
		// taskmanager.cpp:1885
		{
			std::string text;
			// taskmanager.cpp:1886
			packet.DecodeStr(text);
			// taskmanager.cpp:1887
			CNoticeBoard::Instance()->AddNotice(text.c_str(), 1, false);
		}
		break;
	case 0x1d5:
		// taskmanager.cpp:1896
		{
			unsigned long count = packet.Decode4();
			// taskmanager.cpp:1898
			for (unsigned long i = 0; i < count; i++) {
				// taskmanager.cpp:1901
				std::string data("");
				// taskmanager.cpp:1905
				int type = packet.Decode4();
				// taskmanager.cpp:1906
				packet.DecodeStr(data);
				// taskmanager.cpp:1923
				CWinnerNotice::Instance()->SetWinnerData(type, data);
			}
		}
		break;
	case 0x41:
		// taskmanager.cpp:1938
		{
			// The login notice's record (its constructor and destructor are the
			// original's ??0/??1sProtocol of this function).
			struct sProtocol {
				unsigned long level;  // 0x0
				unsigned char sex;    // 0x4, 0 male, 1 female, 2 unknown
				std::string nick;     // 0x8
			} protocol;
			// taskmanager.cpp:1939
			protocol.level = packet.Decode4();
			// taskmanager.cpp:1940
			protocol.sex = packet.Decode1();
			// taskmanager.cpp:1941
			packet.DecodeStr(protocol.nick);
			// taskmanager.cpp:1949
			const char *sexNames[3] = { "\xb3\xb2", "\xbf\xa9", "\xba\xd2\xb8\xed" };
			char text[256];
			sprintf(text, "%s(\xb7\xb9\xba\xa7:%d \xbc\xba\xba\xb0:%s)\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce \xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", protocol.nick.c_str(), protocol.level, sexNames[protocol.sex]);
			// taskmanager.cpp:1951
			CChatMsg::Instance()->AddChatMsg(text, 0xFFFF7878, false, false);
		}
		break;
	case 0x42:
		// taskmanager.cpp:1958
		{
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:1960
			switch (result) {
			case 0xd3:
				// taskmanager.cpp:1964
				{
					FrWnd *pForm = g_pFresh->GetManager()->GetDesktop()->FindChildForm("server");
					// taskmanager.cpp:1965
					if (!pForm || !pForm->IsKindOf(&FrServerDlg::m_RTTI)) {
						break;
					}
					// taskmanager.cpp:1968
					if (packet.Decode1() == 0) {
						// taskmanager.cpp:1971
						((FrServerDlg *)pForm)->FinishAutoLogin();
					} else {
						// taskmanager.cpp:1975
						((FrServerDlg *)pForm)->BeginAutoLogin();
					}
				}
				break;
			case 0xd2:
				// taskmanager.cpp:1982
				{
					unsigned long count = packet.Decode4();
					// taskmanager.cpp:1985
					FrWnd *pForm = g_pFresh->GetManager()->GetDesktop()->FindChildForm("server");
					// taskmanager.cpp:1986
					if (!pForm || !pForm->IsKindOf(&FrServerDlg::m_RTTI)) {
						break;
					}
					// taskmanager.cpp:1989
					((FrServerDlg *)pForm)->AddLoginReplyCount(count);
				}
				break;
			case 0:
				// taskmanager.cpp:1996
				{
					FrWnd *pForm = g_pFresh->GetManager()->GetDesktop()->FindChildForm("server");
					// taskmanager.cpp:1997
					if (pForm && pForm->IsKindOf(&FrServerDlg::m_RTTI)) {
						// taskmanager.cpp:2000
						((FrServerDlg *)pForm)->AddLoginReplyCount(0x15);
					}

					// taskmanager.cpp:2004
					UccManager()->BuildDisplayClothesSet();
					// taskmanager.cpp:2007
					CSharedDoc::Instance()->ClearInventory();

					// taskmanager.cpp:2012
					if (((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CShopTask::m_RTTI) : false) == true) {
						// taskmanager.cpp:2014
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xC5, 0, 0, 0, 0, 0));
					}

					// taskmanager.cpp:2017
					CSharedDoc::Instance()->m_latestVersion = packet.DecodeStr();
					// taskmanager.cpp:2021
					CSharedDoc::Instance()->m_string4D4C = packet.DecodeStr();

					// taskmanager.cpp:2025
					sUserInfoExt userInfoExt;
					// taskmanager.cpp:2026
					sUserInfo userInfo;
					// taskmanager.cpp:2027
					if (IsLocalContent((localContentType_t)0x70)) {
						// taskmanager.cpp:2029
						packet.DecodeBuffer(&userInfoExt, sizeof(sUserInfoExt));
						// taskmanager.cpp:2030
						userInfo = userInfoExt;
						// taskmanager.cpp:2031
						memcpy(CSharedDoc::Instance()->m_userInfoExtBlock, userInfoExt.m_extBlock, sizeof(userInfoExt.m_extBlock));
					} else {
						// taskmanager.cpp:2035
						packet.DecodeBuffer(&userInfo, sizeof(sUserInfo));
					}

					// taskmanager.cpp:2038
					RequestUccInfo(userInfo);

					// taskmanager.cpp:2041
					_SYSTEMTIME serverTime;
					packet.DecodeBuffer(&serverTime, sizeof(serverTime));
					// taskmanager.cpp:2042
					CSharedDoc::Instance()->SetServerTime(serverTime);

					// taskmanager.cpp:2044
					unsigned char firstLogin = packet.Decode1();
					// taskmanager.cpp:2045
					packet.Decode1();

					// taskmanager.cpp:2049
					CSharedDoc::Instance()->SetBsUsableTimes(packet.Decode2());
					// taskmanager.cpp:2050
					CSharedDoc::Instance()->SetBsUsableBonusTimes(packet.Decode2());
					// taskmanager.cpp:2051
					CSharedDoc::Instance()->SetBsRemainedBonusTimes(packet.Decode2());
					// taskmanager.cpp:2053
					if (IsLocalContent((localContentType_t)8)) {
						// taskmanager.cpp:2054
						CSharedDoc::Instance()->SetFlagBlock(packet.Decode4());
					}

					// taskmanager.cpp:2056
					CSharedDoc::Instance()->m_controlServerService = packet.Decode4();
					// taskmanager.cpp:2057
					unsigned long value5060 = packet.Decode4();
					// taskmanager.cpp:2059
					CSharedDoc::Instance()->m_value5060 = value5060;
					// taskmanager.cpp:2061
					unsigned long serverProperty = packet.Decode4();
					// taskmanager.cpp:2069
					CSharedDoc::Instance()->m_serverProperty.Set(serverProperty);

					// taskmanager.cpp:2073
					GUILD_USER_INFO guildUserInfo;
					// taskmanager.cpp:2074
					packet.DecodeBuffer(&guildUserInfo, sizeof(GUILD_USER_INFO));
					// taskmanager.cpp:2075
					IActor *pGuildActor = AfxGetTask()->GetActor(s_guildActorName);
					// taskmanager.cpp:2077
					pGuildActor << MsgObject(0, 0x263, 6, (intptr_t)&guildUserInfo, 0, 0, 0);

					// taskmanager.cpp:2082
					S5::UTIL::SetServerProperty(serverProperty);
					// taskmanager.cpp:2083
					S5::THEME::SetSkinnedTheme(serverProperty);

					// The local player's copy of the user information.
					// taskmanager.cpp:2090
					CSharedDoc::Instance()->m_mySlot = userInfo.m_slot;
					// taskmanager.cpp:2091 (sPangYaUserInfo, 0x10D bytes)
					memcpy(&CSharedDoc::Instance()->m_userInfo, userInfo.m_nickname, 0x10D);
					// taskmanager.cpp:2092 (sPangYaUserStatistics, 0xEB bytes)
					memcpy(&CSharedDoc::Instance()->m_myStatShots, &userInfo.m_statShots, 0xEB);
					// taskmanager.cpp:2093
					CSharedDoc::Instance()->m_myTrophy = userInfo.m_trophy;
					// taskmanager.cpp:2094 (the 0x6C-byte equipment block)
					memcpy(&CSharedDoc::Instance()->m_myCaddieId, &userInfo.m_equipHead, 0x6C);
					// taskmanager.cpp:2098
					memcpy(CSharedDoc::Instance()->m_myMapStatistics, userInfo.m_mapStatistics, sizeof(userInfo.m_mapStatistics));
					// taskmanager.cpp:2099
					memcpy(CSharedDoc::Instance()->m_myMapStatistics2, userInfo.m_mapStatistics2, sizeof(userInfo.m_mapStatistics2));

					// taskmanager.cpp:2102
					NetResourceManager::Instance()->ResetGuildRSS();
					// taskmanager.cpp:2105
					if (userInfo.m_guildUID) {
						// taskmanager.cpp:2113
						NetResourceManager::Instance()->RequestGuildRSS(userInfo.m_guildUID, RecvRSS0);
					}

					// taskmanager.cpp:2117
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 5, 0, 0, 0, 0));
					// taskmanager.cpp:2118
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x42, 0, 0, 0, 0, 0));
					// taskmanager.cpp:2120
					if (firstLogin == 1) {
						// taskmanager.cpp:2121
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x43, 1, 0, 0, 0, 0));
					}

					// taskmanager.cpp:2126
					if (!COption::Instance()->gIsWhisperAllowed()) {
						// taskmanager.cpp:2128
						WSendPacket send(GCT_ALLOW_WHISPER);
						// taskmanager.cpp:2129
						send.Encode1(0);
						// taskmanager.cpp:2130
						send.Send(SEND_TO_DEFAULT);
					}

					// taskmanager.cpp:2133
					if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN)
						&& CMessengerInfo::Instance()->m_serverListRequest == 4) {
						// taskmanager.cpp:2135
						sUserPosition position;
						// taskmanager.cpp:2136
						position.m_userValue = CSharedDoc::Instance()->m_gameServerUID;
						// taskmanager.cpp:2137
						wsprintfA(position.m_channelName, "NONE");
						position.m_roomNumber = 0xFFFF;
						position.m_value2 = 0xFFFFFFFF;
						// taskmanager.cpp:2149
						WSendPacket send(GCR_NOW_POSITION);
						// taskmanager.cpp:2153
						send.EncodeBuffer(&position, sizeof(sUserPosition));
						// taskmanager.cpp:2154
						send.Send(SEND_TO_MSN);
					}

					// taskmanager.cpp:2181
					CSharedDoc::Instance()->SetPeriodDoc();
					// taskmanager.cpp:2182
					CSharedDoc::Instance()->GetPeriodDoc()->Set_PreSettingContentsDate();
				}
				break;
			case 2:
				// taskmanager.cpp:2189
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 6, 0, 0, 0, 0));
				break;
			case 4:
				// taskmanager.cpp:2195
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0xB, 0, 0, 0, 0));
				break;
			case 5:
				// taskmanager.cpp:2201
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 8, 0, 0, 0, 0));
				break;
			case 6:
				// taskmanager.cpp:2207
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 7, 0, 0, 0, 0));
				break;
			case 7:
				// taskmanager.cpp:2213
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 9, 0, 0, 0, 0));
				break;
			case 8:
				// taskmanager.cpp:2219
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0xA, 0, 0, 0, 0));
				break;
			case 0xb:
				// taskmanager.cpp:2225
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0xC, 0, 0, 0, 0));
				// taskmanager.cpp:2226
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbc\xad\xb9\xf6 \xb9\xf6\xc0\xfc\xc0\xcc \xb4\xd9\xb8\xa8\xb4\xcf\xb4\xd9. \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0xe:
				// taskmanager.cpp:2232
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0xE, 0, 0, 0, 0));
				break;
			case 0xd6:
				// taskmanager.cpp:2238
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0xF, 0, 0, 0, 0));
				break;
			case 0xd7:
				// taskmanager.cpp:2244
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0x10, 0, 0, 0, 0));
				break;
			case 0x12:
				// taskmanager.cpp:2250
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0x15, 0, 0, 0, 0));
				break;
			case 0xc9:
				// taskmanager.cpp:2255
				GetActor("Lobby") << MsgObject(0, 0x1E, 0x14, 0, 0, 0, 0);
				// taskmanager.cpp:2256
				GetActor("Lobby") << MsgObject(0, 0x25, (intptr_t)"\xb0\xd4\xc0\xd3\xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
				break;
			case 0x10:
				// taskmanager.cpp:2271
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0x11, 0, 0, 0, 0));
				break;
			case 0x13:
				// taskmanager.cpp:2275
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0x12, 0, 0, 0, 0));
				break;
			case 0xdb:
				// taskmanager.cpp:2279
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0x13, 0, 0, 0, 0));
				break;
			case 3:
			case 0xe0:
				// taskmanager.cpp:2309
				{
					unsigned long blockedTime = packet.Decode4();
					// taskmanager.cpp:2310
					if (blockedTime == 0) {
						break;
					}
					// taskmanager.cpp:2320
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0xD, 0, 0, 0, 0));
					// taskmanager.cpp:2321
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)MakeStr("%s %d", _systemmsg::CMessageManager::Instance()->GetMsg((_systemmsg::eMsgIdentifier)0x76), blockedTime), 0, 0, 0, 0));
				}
				break;
			default:
				// taskmanager.cpp:2329
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1F, 0xD, 0, 0, 0, 0));
				break;
			}
		}
		break;
	case 0x43:
		{
			// taskmanager.cpp:2340
			unsigned short oldLevel = CSharedDoc::Instance()->m_myLevel;
			// taskmanager.cpp:2341
			unsigned long oldExp = CSharedDoc::Instance()->m_myExp;
			// taskmanager.cpp:2344
			packet.DecodeBuffer(&CSharedDoc::Instance()->m_myStatistics, sizeof(sPangYaUserStatistics));
			// taskmanager.cpp:2345
			packet.DecodeBuffer(&CSharedDoc::Instance()->m_myTrophy, sizeof(sTrophyStatistics));
			// taskmanager.cpp:2347
			unsigned char course = packet.Decode1();
			// taskmanager.cpp:2349
			if (course != 0xFF) {
				// taskmanager.cpp:2350
				packet.DecodeBuffer(&CSharedDoc::Instance()->m_myMapStatistics[course], sizeof(sMapStatistics));
			}
			// taskmanager.cpp:2352
			course = packet.Decode1();
			// taskmanager.cpp:2353
			if (course != 0xFF) {
				// taskmanager.cpp:2354
				packet.DecodeBuffer(&CSharedDoc::Instance()->m_myMapStatistics2[course], sizeof(sMapStatistics));
			}
			// taskmanager.cpp:2357
			std::map<unsigned long, sUserInfoTime>::iterator it;
			if (CSharedDoc::Instance()->FindUserInfoTimeGUID(MyGuid(false), it)) {
				// taskmanager.cpp:2360
				it->second.m_userInfo.m_statistics = CSharedDoc::Instance()->m_myStatistics;
				// taskmanager.cpp:2361
				it->second.m_userInfo.m_trophy = CSharedDoc::Instance()->m_myTrophy;
			}
			// taskmanager.cpp:2367
			// The experience gained: the new total minus the old one.
			int gain = (CSharedDoc::Instance()->m_myLevel ? CSharedDoc::Instance()->m_levelExp[CSharedDoc::Instance()->m_myLevel - 1].m_totalExp : 0) + CSharedDoc::Instance()->m_myExp
				- ((oldLevel ? CSharedDoc::Instance()->m_levelExp[oldLevel - 1].m_totalExp : 0) + oldExp);
			// taskmanager.cpp:2369
			if (gain > 0) {
				// taskmanager.cpp:2371
				std::map<unsigned int, sCaddieInfo>::iterator cit = CSharedDoc::Instance()->m_caddieMap.find(CSharedDoc::Instance()->m_myCaddieId);
				// taskmanager.cpp:2372
				if (cit != CSharedDoc::Instance()->m_caddieMap.end()) {
					// taskmanager.cpp:2374
					sCaddieInfo &caddie = cit->second;
					// taskmanager.cpp:2376
					caddie.experience += gain;
					// taskmanager.cpp:2380
					for (unsigned char i = 0; i < 3; i++) {
						if (caddie.level == i && caddie.experience >= CSharedDoc::Instance()->m_caddieLevelExp[i].m_exp) {
							// taskmanager.cpp:2382
							caddie.level++;
							// taskmanager.cpp:2383
							caddie.experience -= CSharedDoc::Instance()->m_caddieLevelExp[i].m_exp;
						}
					}
				}
			}
		}
		break;
	case 0x49:
		// taskmanager.cpp:2394
		// A player's equipment changed (the 0x0B/0x0C kinds): 1 caddie,
		// 2 ball type id, 3 club set, 4 character, 5 mascot, 6 SP item.
		// Another player's, while the avatar chat task waits as the
		// preserved task, goes to that task.
		switch (packet.Decode1()) {
		case 4:
			// taskmanager.cpp:2400
			{
				unsigned long guid = packet.Decode4();
				// taskmanager.cpp:2402
				sCharacterInfo character;
				// taskmanager.cpp:2403
				packet.DecodeBuffer(&character, sizeof(sCharacterInfo));
				// taskmanager.cpp:2405
				if (IsLocalContent((localContentType_t)0x57)) {
					// taskmanager.cpp:2407
					RequestUccInfo(character);
				}
				// taskmanager.cpp:2413
				if (((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false) && guid != MyGuid(false)) {
					// taskmanager.cpp:2414
					AfxGetPreservedTask()->SendMsgToMainActor(MsgObject(0, 0x49, guid, (intptr_t)&character, 0, 0, 0));
				} else {
					// taskmanager.cpp:2416
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x49, guid, (intptr_t)&character, 0, 0, 0));
				}
			}
			break;
		case 1:
			// taskmanager.cpp:2422
			{
				unsigned long guid = packet.Decode4();
				sCaddieInfo caddie;
				// taskmanager.cpp:2425
				packet.DecodeBuffer(&caddie, sizeof(sCaddieInfo));
				// taskmanager.cpp:2427
				if (((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false) && guid != MyGuid(false)) {
					// taskmanager.cpp:2428
					AfxGetPreservedTask()->SendMsgToMainActor(MsgObject(0, 0x4A, guid, (intptr_t)&caddie, 0, 0, 0));
				} else {
					// taskmanager.cpp:2430
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x4A, guid, (intptr_t)&caddie, 0, 0, 0));
				}
			}
			break;
		case 3:
			// taskmanager.cpp:2436
			{
				unsigned long guid = packet.Decode4();
				// The club set's record (0x12 bytes, not typed yet; sUserInfoExt 0xB41).
				unsigned char clubSet[0x12];
				// taskmanager.cpp:2439
				packet.DecodeBuffer(clubSet, sizeof(clubSet));
				// taskmanager.cpp:2441
				if (((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false) && guid != MyGuid(false)) {
					// taskmanager.cpp:2442
					AfxGetPreservedTask()->SendMsgToMainActor(MsgObject(0, 0x4B, guid, (intptr_t)clubSet, 0, 0, 0));
				} else {
					// taskmanager.cpp:2444
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x4B, guid, (intptr_t)clubSet, 0, 0, 0));
				}
			}
			break;
		case 2:
			// taskmanager.cpp:2450
			{
				unsigned long guid = packet.Decode4();
				// taskmanager.cpp:2451
				unsigned long value = packet.Decode4();
				// taskmanager.cpp:2453
				if (((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false) && guid != MyGuid(false)) {
					// taskmanager.cpp:2454
					AfxGetPreservedTask()->SendMsgToMainActor(MsgObject(0, 0x4C, guid, value, 0, 0, 0));
				} else {
					// taskmanager.cpp:2456
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x4C, guid, value, 0, 0, 0));
				}
			}
			break;
		case 5:
			// taskmanager.cpp:2462
			{
				unsigned long guid = packet.Decode4();
				sMascotInfo mascot;
				// taskmanager.cpp:2465
				packet.DecodeBuffer(&mascot, sizeof(sMascotInfo));
				// taskmanager.cpp:2467
				if (((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false) && guid != MyGuid(false)) {
					// taskmanager.cpp:2468
					AfxGetPreservedTask()->SendMsgToMainActor(MsgObject(0, 0x4E, guid, (intptr_t)&mascot, 0, 0, 0));
				} else {
					// taskmanager.cpp:2470
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x4E, guid, (intptr_t)&mascot, 0, 0, 0));
				}
			}
			break;
		case 6:
			// taskmanager.cpp:2475
			{
				unsigned long guid = packet.Decode4();
				// taskmanager.cpp:2476
				unsigned long value = packet.Decode4();
				// taskmanager.cpp:2478
				float rate = 0;
				packet.DecodeBuffer(&rate, sizeof(rate));
				// taskmanager.cpp:2479
				if (((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false) && guid != MyGuid(false)) {
					// taskmanager.cpp:2480
					AfxGetPreservedTask()->SendMsgToMainActor(MsgObject(0, 0x24E, guid, value, (int)rate, 0, 0));
				} else {
					// taskmanager.cpp:2482
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x24E, guid, value, (int)rate, 0, 0));
				}
			}
			break;
		}
		break;
	case 0x4a:
	case 0x7c:
		// taskmanager.cpp:2494
		// Back to the lobby (from a room, or from the avatar chat).
		{
			// taskmanager.cpp:2495
			if (packet.Decode2() != 0xFFFF) {
				// taskmanager.cpp:2497
				AfxPostMsg(0, "AvatarChat", 8, 0, 0, 0, 0);
				// taskmanager.cpp:2498
				AfxPostMsg(0, "AvatarChat", 2, 0, 0, 0, 0);
				break;
			}
			// taskmanager.cpp:2503
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
			// taskmanager.cpp:2505
			CSharedDoc::Instance()->m_mySlot = 0xFFFF;
			// taskmanager.cpp:2506
			CSharedDoc::Instance()->m_userInfo.galleryGuid = 0xFFFFFFFF;
			// taskmanager.cpp:2508
			CProjectG::Instance()->FromGolfToLobbySettings();
			// taskmanager.cpp:2510
			if (!((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&CLobbyTask::m_RTTI) : false)) {
				// taskmanager.cpp:2511
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", 0);
			}
			// taskmanager.cpp:2514
			if (CSharedDoc::Instance()->m_lobbyStateFlag) {
				// taskmanager.cpp:2516
				AfxPostMsg(0, "Lobby", 2, 0, 0, 0, 0);
				// taskmanager.cpp:2517
				AfxPostMsg(0, "Lobby", 0, (intptr_t)"ROOMLIST", 0, 0, 0);
			} else {
				// taskmanager.cpp:2521
				AfxPostMsg(0, "Lobby", 2, 0, 0, 0, 0);
				// taskmanager.cpp:2522
				AfxPostMsg(0, "Lobby", 0, (intptr_t)"TOPPAGE", 0, 0, 0);
			}
			// taskmanager.cpp:2527
			if (WSingleton<CSceneManager>::IsInstantiated()) {
				// taskmanager.cpp:2528
				CSceneManager::Instance()->SetWeather(0, false);
			}
			// The messenger learns the player's new place.
			// taskmanager.cpp:2530
			if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN) && CMessengerInfo::Instance()->GetComplete() == CONNECT_STATUS_COMPLETE) {
				// taskmanager.cpp:2532
				CSharedDoc::Instance()->m_roomInfo.m_roomNumber = 0xFFFF;
				// taskmanager.cpp:2533
				CSharedDoc::Instance()->m_roomInfo.m_gameType = 0xFF;
				// taskmanager.cpp:2535
				sUserPosition position;
				// taskmanager.cpp:2536
				position.m_userValue = CSharedDoc::Instance()->m_gameServerUID;
				// taskmanager.cpp:2537
				position.m_channelId = CSharedDoc::Instance()->m_currentChannel.id;
				// taskmanager.cpp:2538
				wsprintfA(position.m_channelName, "%s", CSharedDoc::Instance()->m_currentChannel.name);
				position.m_roomNumber = 0xFFFF;
				position.m_value2 = 0xFFFFFFFF;
				// taskmanager.cpp:2550
				WSendPacket send(GCR_NOW_POSITION);
				// taskmanager.cpp:2554
				send.EncodeBuffer(&position, sizeof(sUserPosition));
				// taskmanager.cpp:2555
				send.Send(SEND_TO_MSN);
				// taskmanager.cpp:2557
				if (CMessengerInfo::Instance()->m_myStatus != MSN::STATUS_BUSY) {
					// taskmanager.cpp:2558
					CMessengerInfo::Instance()->SetMyStatus(MSN::STATUS_ONLINE);
				}
			}
			// taskmanager.cpp:2562
			CIntrusion::Instance()->FinishGame();
		}
		break;
	case 0x45:
		// taskmanager.cpp:2569
		{
			unsigned char count = packet.Decode1();
			// taskmanager.cpp:2570
			unsigned char kind = packet.Decode1();
			// taskmanager.cpp:2571
			unsigned short lobby = packet.Decode2();
			// taskmanager.cpp:2573
			sRoomInfo room;
			// taskmanager.cpp:2576
			if (lobby != 0xFFFF)
				break;
			// taskmanager.cpp:2580
			std::list<sRoomInfo> &roomList = CSharedDoc::Instance()->m_roomList;
			switch (kind) {
			case 0:
				// The whole list again.
				// taskmanager.cpp:2584
				roomList.clear();
				// taskmanager.cpp:2586
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 6, 0, 0, 0, 0, 0));
				// taskmanager.cpp:2588
				for (unsigned char i = 0; i < count; i++) {
					// taskmanager.cpp:2590
					if (IsLocalContent((localContentType_t)0x5E)) {
						sRoomInfoLegacy legacy;
						// taskmanager.cpp:2592
						memset(&room, 0, sizeof(sRoomInfo));
						// taskmanager.cpp:2593
						packet.DecodeBuffer(&legacy, sizeof(sRoomInfoLegacy));
						// taskmanager.cpp:2596
						room.m_isPublic = legacy.m_isPublic;
						// taskmanager.cpp:2597
						room.m_intrusionAccess = legacy.m_intrusionAccess;
						// taskmanager.cpp:2598
						room.m_state = legacy.m_state;
						// taskmanager.cpp:2601
						room.m_userCount = legacy.m_userCount;
						// taskmanager.cpp:2602
						room.m_userLimit = legacy.m_userLimit;
						room.m_roomNumber = legacy.m_roomNumber;
						// taskmanager.cpp:2607
						room.m_course = legacy.m_course;
						// taskmanager.cpp:2608
						room.m_gameType = legacy.m_gameType;
						// taskmanager.cpp:2609
						room.m_holeCount = legacy.m_holeCount;
						// taskmanager.cpp:2614
						strcpy(room.m_title, legacy.m_title);
						// taskmanager.cpp:2616
						memcpy(&room.m_guild, legacy.m_guildUID, 0x4A);
					// taskmanager.cpp:2619
					} else {
						// taskmanager.cpp:2621
						packet.DecodeBuffer(&room, sizeof(sRoomInfo));
					}
					// taskmanager.cpp:2625
					CSharedDoc::Instance()->m_chatManager.FilteringHack(room.m_title, false);
					// taskmanager.cpp:2627
					if (IsLocalContent((localContentType_t)0x2E))
						// taskmanager.cpp:2628
						CIntrusion::Instance()->OnAccessRoom(room);
					// taskmanager.cpp:2630
					roomList.push_back(room);
					// taskmanager.cpp:2632
					std::list<sRoomInfo>::iterator last = --roomList.end();
					// taskmanager.cpp:2633
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 4, (intptr_t)&*last, 0, 0, 0, 0));
					// taskmanager.cpp:2635
					if (room.m_roomNumber == CSharedDoc::Instance()->m_roomInfo.m_roomNumber) {
						// taskmanager.cpp:2637
						CSharedDoc::Instance()->m_roomInfo = room;
						// taskmanager.cpp:2638
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x10, 0, 0, 0, 0, 0));
					}
				}
				// taskmanager.cpp:2641
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xD, 0, 0, 0, 0, 0));
				// taskmanager.cpp:2643
				break;
			case 1:
				// Rooms added or changed.
				// taskmanager.cpp:2647
				for (unsigned char i = 0; i < count; i++) {
					// taskmanager.cpp:2649
					if (IsLocalContent((localContentType_t)0x5E)) {
						sRoomInfoLegacy legacy;
						// taskmanager.cpp:2651
						memset(&room, 0, sizeof(sRoomInfo));
						// taskmanager.cpp:2652
						packet.DecodeBuffer(&legacy, sizeof(sRoomInfoLegacy));
						// taskmanager.cpp:2655
						room.m_isPublic = legacy.m_isPublic;
						// taskmanager.cpp:2656
						room.m_intrusionAccess = legacy.m_intrusionAccess;
						// taskmanager.cpp:2657
						room.m_state = legacy.m_state;
						// taskmanager.cpp:2660
						room.m_userCount = legacy.m_userCount;
						// taskmanager.cpp:2661
						room.m_userLimit = legacy.m_userLimit;
						room.m_roomNumber = legacy.m_roomNumber;
						// taskmanager.cpp:2666
						room.m_course = legacy.m_course;
						// taskmanager.cpp:2667
						room.m_gameType = legacy.m_gameType;
						// taskmanager.cpp:2668
						room.m_holeCount = legacy.m_holeCount;
						// taskmanager.cpp:2672
						room.m_trophy = legacy.m_trophy;
						// taskmanager.cpp:2673
						strcpy(room.m_title, legacy.m_title);
						// taskmanager.cpp:2675
						memcpy(&room.m_guild, legacy.m_guildUID, 0x4A);
					// taskmanager.cpp:2678
					} else {
						// taskmanager.cpp:2680
						packet.DecodeBuffer(&room, sizeof(sRoomInfo));
					}
					// taskmanager.cpp:2684
					CSharedDoc::Instance()->m_chatManager.FilteringHack(room.m_title, false);
					// taskmanager.cpp:2686
					if (IsLocalContent((localContentType_t)0x2E))
						// taskmanager.cpp:2687
						CIntrusion::Instance()->OnAccessRoom(room);
					// taskmanager.cpp:2690
					std::list<sRoomInfo>::iterator it = std::find(roomList.begin(), roomList.end(), room.m_roomNumber);
					// taskmanager.cpp:2692
					if (it == roomList.end()) {
						// taskmanager.cpp:2694
						roomList.push_back(room);
						// taskmanager.cpp:2695
						std::list<sRoomInfo>::iterator last = --roomList.end();
						// taskmanager.cpp:2696
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 4, (intptr_t)&*last, 0, 0, 0, 0));
					// taskmanager.cpp:2698
					} else {
						// taskmanager.cpp:2700
						*it = room;
					}
					// taskmanager.cpp:2703
					if (room.m_roomNumber == CSharedDoc::Instance()->m_roomInfo.m_roomNumber) {
						// taskmanager.cpp:2705
						CSharedDoc::Instance()->m_roomInfo = room;
						// taskmanager.cpp:2706
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x10, 0, 0, 0, 0, 0));
					}
				}
				// taskmanager.cpp:2710
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xD, 0, 0, 0, 0, 0));
				// taskmanager.cpp:2712
				break;
			case 2:
				// Rooms removed.
				// taskmanager.cpp:2716
				for (unsigned char i = 0; i < count; i++) {
					// taskmanager.cpp:2718
					packet.DecodeBuffer(&room, sizeof(sRoomInfo));
					// taskmanager.cpp:2721
					CSharedDoc::Instance()->m_chatManager.FilteringHack(room.m_title, false);
					// taskmanager.cpp:2723
					if (IsLocalContent((localContentType_t)0x2E))
						// taskmanager.cpp:2724
						CIntrusion::Instance()->OnAccessRoom(room);
					// taskmanager.cpp:2727
					std::list<sRoomInfo>::iterator it = std::find(roomList.begin(), roomList.end(), room.m_roomNumber);
					// taskmanager.cpp:2728
					if (it != roomList.end()) {
						// taskmanager.cpp:2730
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 5, (intptr_t)&*it, sizeof(sRoomInfo), 0, 0, 0));
						// taskmanager.cpp:2731
						roomList.erase(it);
					}
				}
				break;
			case 3:
				// Rooms updated in place.
				// taskmanager.cpp:2739
				for (unsigned char i = 0; i < count; i++) {
					// taskmanager.cpp:2741
					packet.DecodeBuffer(&room, sizeof(sRoomInfo));
					// taskmanager.cpp:2744
					CSharedDoc::Instance()->m_chatManager.FilteringHack(room.m_title, false);
					// taskmanager.cpp:2746
					if (IsLocalContent((localContentType_t)0x2E))
						// taskmanager.cpp:2747
						CIntrusion::Instance()->OnAccessRoom(room);
					// taskmanager.cpp:2765
					std::list<sRoomInfo>::iterator it = std::find(roomList.begin(), roomList.end(), room.m_roomNumber);
					// taskmanager.cpp:2766
					if (it != roomList.end())
						// taskmanager.cpp:2768
						*it = room;
					// taskmanager.cpp:2771
					if (room.m_roomNumber == CSharedDoc::Instance()->m_roomInfo.m_roomNumber) {
						// taskmanager.cpp:2773
						CSharedDoc::Instance()->m_roomInfo = room;
						// taskmanager.cpp:2774
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x10, 0, 0, 0, 0, 0));
					}
				}
				// taskmanager.cpp:2777
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xD, 0, 0, 0, 0, 0));
				// taskmanager.cpp:2783
				break;
			}
		}
		break;
	case 0x44:
		// taskmanager.cpp:2789
		{
			sBriefUserInfo info;
			unsigned char kind = packet.Decode1();
			// taskmanager.cpp:2790
			unsigned char count = packet.Decode1();
			// taskmanager.cpp:2793
			switch (kind) {
			case 4:
				// The whole lobby again: drop the old list, then add as case 5.
				// taskmanager.cpp:2796
				CSharedDoc::Instance()->m_userMap.clear();
				// taskmanager.cpp:2797
				GetActor("Lobby") << MsgObject(0, 0xB, 0, 0, 0, 0, 0);
			case 5:
				// taskmanager.cpp:2801
				for (int i = 0; i < count; i++) {
					// taskmanager.cpp:2803
					packet.DecodeBuffer(&info, sizeof(sBriefUserInfo));
					// taskmanager.cpp:2805
					if (CSharedDoc::Instance()->m_userMap.find(info.uid) == CSharedDoc::Instance()->m_userMap.end()) {
						// taskmanager.cpp:2807
						CSharedDoc::Instance()->m_userMap.insert(std::map<unsigned long, sBriefUserInfo>::value_type(info.uid, info));
						// taskmanager.cpp:2808
						std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(info.uid);
						// taskmanager.cpp:2809
						GetActor("Lobby") << MsgObject(0, 0xA, (intptr_t)&it->second, 0, 0, 0, 0);
					}
				}
				break;
			case 1:
				// taskmanager.cpp:2818
				for (int i = 0; i < count; i++) {
					// taskmanager.cpp:2820
					packet.DecodeBuffer(&info, sizeof(sBriefUserInfo));
					// taskmanager.cpp:2822
					std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(info.uid);
					// taskmanager.cpp:2823
					if (it == CSharedDoc::Instance()->m_userMap.end()) {
						// taskmanager.cpp:2825
						CSharedDoc::Instance()->m_userMap.insert(std::map<unsigned long, sBriefUserInfo>::value_type(info.uid, info));
						// taskmanager.cpp:2826
						it = CSharedDoc::Instance()->m_userMap.find(info.uid);
						// taskmanager.cpp:2827
						GetActor("Lobby") << MsgObject(0, 0xA, (intptr_t)&it->second, 0, 0, 0, 0);
					} else {
						// taskmanager.cpp:2831
						it->second = info;
						// taskmanager.cpp:2833
						if (info.identityBits.administrator) {
							// taskmanager.cpp:2836
							if (info.invisible) {
								// taskmanager.cpp:2839
								if (info.stayInLobby) {
									// taskmanager.cpp:2841
									GetActor("Lobby") << MsgObject(0, 0xA, (intptr_t)&it->second, 0, 0, 0, 0);
								}
							}
						}
					}
				}
				break;
			case 2:
				// taskmanager.cpp:2852
				for (int i = 0; i < count; i++) {
					// taskmanager.cpp:2854
					packet.DecodeBuffer(&info, sizeof(sBriefUserInfo));
					// taskmanager.cpp:2856
					std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(info.uid);
					// taskmanager.cpp:2857
					if (it != CSharedDoc::Instance()->m_userMap.end()) {
						// taskmanager.cpp:2859
						if (info.identityBits.administrator) {
							// taskmanager.cpp:2861
							it->second = info;
						}
						// taskmanager.cpp:2863
						GetActor("Lobby") << MsgObject(0, 0xC, (intptr_t)&it->second, sizeof(sBriefUserInfo), 0, 0, 0);
						// taskmanager.cpp:2864
						if (info.identityBits.administrator) {
							// taskmanager.cpp:2866
							if (!info.stayInLobby) {
								// taskmanager.cpp:2868
								CSharedDoc::Instance()->m_userMap.erase(it);
							}
						} else {
							// taskmanager.cpp:2873
							CSharedDoc::Instance()->m_userMap.erase(it);
						}
					}
				}
				break;
			case 3:
				// taskmanager.cpp:2882
				{
					packet.DecodeBuffer(&info, sizeof(sBriefUserInfo));
					// taskmanager.cpp:2884
					std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(info.uid);
					// taskmanager.cpp:2885
					if (it != CSharedDoc::Instance()->m_userMap.end()) {
						// taskmanager.cpp:2887
						it->second = info;
						// taskmanager.cpp:2888
						CSharedDoc *pDoc = CSharedDoc::Instance();
						if (info.roomNumber != pDoc->m_mySlot || info.roomNumber == 0xFFFF) {
							break;
						}
						// A player of the local player's room: refresh his room entry.
						// taskmanager.cpp:2890
						for (std::list<sSlotInfo>::iterator player = pDoc->m_roomPlayerList.begin(); player != pDoc->m_roomPlayerList.end(); ++player) {
							// taskmanager.cpp:2892
							if (player->m_uid == info.uid) {
								// taskmanager.cpp:2894
								player->m_identity = info.identity;
								// taskmanager.cpp:2895
								player->m_titleTypeId = info.titleTypeId;
								break;
							}
						}
					}
				}
				break;
			}
		}
		break;
	case 0x4b:
		// taskmanager.cpp:2909
		{
			if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_LOGIN)) {
				// taskmanager.cpp:2914
				WSendPacket send(LC_CONNECT_GAMESERVER);
				send.Encode4(CSharedDoc::Instance()->m_gameServerUID);
				send.Send(SEND_TO_LOGIN);
			}
			// taskmanager.cpp:2921
			unsigned char count = packet.Decode1();
			CSharedDoc::Instance()->m_channelList.clear();
			for (int i = 0; i < count; i++) {
				sChannelInfo channel;
				memset(&channel, 0, sizeof(channel));
				packet.DecodeBuffer(&channel, sizeof(channel));
				CSharedDoc::Instance()->m_channelList.push_back(channel);
			}
			// taskmanager.cpp:2935
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1B, 0, 0, 0, 0, 0));
		}
		break;
	case 0x4c:
		// taskmanager.cpp:2942
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
			// taskmanager.cpp:2944
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:2945
			switch (result) {
			case 1:
				// taskmanager.cpp:2949
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1C, 0, 0, 0, 0, 0));
				// taskmanager.cpp:2950
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x20, 0, 0, 0, 0, 0));
				// taskmanager.cpp:2951
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 3, (intptr_t)MakeStr("[%s]\xbf\xa1 \xc0\xd4\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", CSharedDoc::Instance()->m_currentChannel.name), 0xFFFF7878, 0, 0, 0));
				// taskmanager.cpp:2953
				if (CSharedDoc::Instance()->m_currentChannel.flags & 0x4000) {
					// taskmanager.cpp:2954
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 3, (intptr_t)"[EVENT]\xc6\xce\xb9\xe8\xc6\xb2 5\xb0\xe6\xb1\xe2\xb8\xa6 \xc7\xcf\xbd\xc7 \xb6\xa7\xb8\xb6\xb4\xd9 1000\xc6\xce\xc0\xbb \xb5\xe5\xb8\xb3\xb4\xcf\xb4\xd9.", 0xFFFF7878, 0, 0, 0));
				}
				// taskmanager.cpp:2956
				if (IsLocalContent((localContentType_t)0x28)) {
					// taskmanager.cpp:2957
					CMatchHistory::Instance()->LoginRequest(MyId());
				}
				break;
			case 2:
				// taskmanager.cpp:2963
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x22, (intptr_t)"\xc3\xa4\xb3\xce\xc0\xc7 \xc1\xa4\xbf\xf8\xbc\xf6\xb0\xa1 \xc3\xca\xb0\xfa\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:2964
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x39, 0, 0, 0, 0, 0));
				break;
			case 3:
				// taskmanager.cpp:2970
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x22, (intptr_t)"\xc3\xa4\xb3\xce\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:2971
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x39, 0, 0, 0, 0, 0));
				break;
			case 4:
				// taskmanager.cpp:2977
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x22, (intptr_t)"\xc3\xa4\xb3\xce \xc1\xa4\xba\xb8\xb0\xa1 \xc0\xdf\xb8\xf8 \xc0\xfc\xb4\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:2978
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x39, 0, 0, 0, 0, 0));
				break;
			}
		}
		break;
	case 0x4d:
		// taskmanager.cpp:2992
		CSharedDoc::Instance()->ClearCurrentChannel(false);
		// taskmanager.cpp:2994
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:2995
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xEC, 0, 0, 0, 0, 0));
		// taskmanager.cpp:2996
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x3A, 0, 0, 0, 0, 0));
		// taskmanager.cpp:2998
		CSharedDoc::Instance()->m_roomList.clear();
		// taskmanager.cpp:3000
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 6, 0, 0, 0, 0, 0));
		// taskmanager.cpp:3001
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xEB, 0, 0, 0, 0, 0));
		// taskmanager.cpp:3003
		COneLineBoard::Instance()->Clear();
		break;
	case 0xf3:
		// taskmanager.cpp:3011
		CSharedDoc::Instance()->m_lobbyStateFlag = true;
		// taskmanager.cpp:3012
		CSharedDoc::Instance()->m_lobbyStateFlag2 = false;
		// taskmanager.cpp:3014
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:3016
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xEB, 0, 0, 0, 0, 0));
		// taskmanager.cpp:3017
		if (!((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false)) {
			// taskmanager.cpp:3018
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", 0);
			// taskmanager.cpp:3022
			AfxPostMsg(0, "Lobby", 0, (intptr_t)"ROOMLIST", 0, 0, 0);
		} else {
			// taskmanager.cpp:3030
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0, (intptr_t)"ROOMLIST", 0, 0, 0, 0));
		}
		break;
	case 0xf4:
		// taskmanager.cpp:3039
		CSharedDoc::Instance()->m_lobbyStateFlag = false;
		// taskmanager.cpp:3040
		CSharedDoc::Instance()->m_lobbyStateFlag2 = false;
		// taskmanager.cpp:3042
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:3043
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xEB, 0, 0, 0, 0, 0));
		// taskmanager.cpp:3044
		if (!((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false)) {
			// taskmanager.cpp:3045
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", 0);
		}
		// taskmanager.cpp:3047
		AfxPostMsg(0, "Lobby", 0, (intptr_t)"TOPPAGE", 0, 0, 0);
		break;
	case 0xf5:
		// taskmanager.cpp:3056
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:3058
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xEB, 0, 0, 0, 0, 0));
		break;
	case 0x106:
		{
			// taskmanager.cpp:3064
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
			// taskmanager.cpp:3066
			int result = packet.Decode1();
			// taskmanager.cpp:3068
			switch (result) {
			case 0:
				{
					// taskmanager.cpp:3072
					std::string nick;
					// taskmanager.cpp:3073
					packet.DecodeStr(nick);
					// taskmanager.cpp:3075
					if (nick.length() >= 4 && nick.length() <= 16) {
						// taskmanager.cpp:3076
						strcpy(CSharedDoc::Instance()->m_userInfo.nickname, nick.c_str());
					}
					// taskmanager.cpp:3080
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xE7, (intptr_t)nick.c_str(), 0, 0, 0, 0));
					// taskmanager.cpp:3082
					if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN) && CMessengerInfo::Instance()->m_serverListRequest == 4) {
						// taskmanager.cpp:3084
						WSendPacket send(0x22);
						// taskmanager.cpp:3085
						send.EncodeStr(nick);
						// taskmanager.cpp:3086
						send.Send(SEND_TO_MSN);
					}
				}
				// taskmanager.cpp:3089
				break;
			case 2:
				// taskmanager.cpp:3092
				// "This nickname is already in use."
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xb5\xc7\xb0\xed \xc0\xd6\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:3094
				break;
			case 3:
				// taskmanager.cpp:3097
				// "A nickname must be 4-16 letters or 2-8 Korean characters, without spaces."
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xb4\xeb\xc8\xad\xb8\xed\xc0\xba \\c0xffff0000\\c\xbf\xb5\xb9\xae 4 - 16 \xb1\xdb\xc0\xda\\c0xff000000\\c \xb6\xc7\xb4\xc2 \\c0xffff0000\\c\xc7\xd1\xb1\xdb 2 - 8\xb1\xdb\xc0\xda\\c0xff000000\\c\xc0\xc7 \xb1\xe6\xc0\xcc\xb8\xa6 \xb0\xa1\xc1\xae\xbe\xdf \xc7\xcf\xb8\xe7, \\c0xffff0000\\c\xb0\xf8\xb9\xe9\xb9\xae\xc0\xda\\c0xff000000\\c\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:3099
				break;
			case 4:
				// taskmanager.cpp:3102
				// "Not enough cookies. Please top up cookies in the shop."
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xc4\xed\xc5\xb0\\c0xff000000\\c\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.\n\xbb\xf3\xc1\xa1\xbf\xa1\xbc\xad \xc4\xed\xc5\xb0\xb8\xa6 \xc3\xe6\xc0\xfc\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.", 0, 0, 0, 0));
				// taskmanager.cpp:3104
				break;
			case 5:
				// taskmanager.cpp:3108
				// "This nickname is forbidden."
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xbb\xe7\xbf\xeb\xc0\xcc \xb1\xdd\xc1\xf6\xb5\xc8 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:3110
				break;
			default:
				// taskmanager.cpp:3113
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xb5\xc7\xb0\xed \xc0\xd6\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			}
			// taskmanager.cpp:3118
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x40, result, 0, 0, 0, 0));
		}
		// taskmanager.cpp:3120
		break;
	case 0x4e:
		// taskmanager.cpp:3124
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
			// taskmanager.cpp:3126
			unsigned long result = packet.Decode4();
			// taskmanager.cpp:3128
			switch (result) {
			case 0:
				// taskmanager.cpp:3132
				{
					std::string nickname;
					// taskmanager.cpp:3135
					packet.DecodeStr(nickname);
					// taskmanager.cpp:3136
					__int64 cookie = packet.Decode8();
					// taskmanager.cpp:3138
					if (nickname.length() >= 4 && nickname.length() <= 16) {
						// taskmanager.cpp:3139
						strcpy(CSharedDoc::Instance()->m_userInfo.nickname, nickname.c_str());
					}
					// taskmanager.cpp:3146
					if (!IsLocalContent((localContentType_t)0x82)) {
						// taskmanager.cpp:3159
						if (!IsPcBang()) {
							// taskmanager.cpp:3161
							// The nickname-change ticket (0x1A000003) pays for the change; without one, cookies do.
							std::list<sItemInfo>::iterator it = std::find(CSharedDoc::Instance()->m_itemInfoList.begin(), CSharedDoc::Instance()->m_itemInfoList.end(), 0x1A000003);
							// taskmanager.cpp:3163
							if (it == CSharedDoc::Instance()->m_itemInfoList.end()) {
								// taskmanager.cpp:3166
								CSharedDoc::Instance()->m_myBonusCash = cookie;
								// taskmanager.cpp:3167
								CSharedDoc::Instance()->RefreshAllCookie();
							}
							// taskmanager.cpp:3172
							else {
								// taskmanager.cpp:3174
								it->m_count--;
								// taskmanager.cpp:3175
								if (it->m_count <= 0) {
									// taskmanager.cpp:3176
									CSharedDoc::Instance()->m_itemInfoList.erase(it);
								}
							}
						}
					}
					// taskmanager.cpp:3185
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xb4\xeb\xc8\xad\xb8\xed\xc0\xcc \xba\xaf\xb0\xe6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf6\xb1\xdd\xba\xce\xc5\xcd 1\xc1\xd6\xc0\xcf \xc8\xc4 \xc0\xe7 \xba\xaf\xb0\xe6\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
					// taskmanager.cpp:3187
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xE7, (intptr_t)nickname.c_str(), 0, 0, 0, 0));
					// taskmanager.cpp:3190
					if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN) && CMessengerInfo::Instance()->m_serverListRequest == 4) {
						// taskmanager.cpp:3192
						WSendPacket send(GCR_CHANGE_NICK);
						// taskmanager.cpp:3193
						send.EncodeStr(nickname);
						// taskmanager.cpp:3194
						send.Send(SEND_TO_MSN);
						// taskmanager.cpp:3195
					}
					// taskmanager.cpp:3196
				}
				// taskmanager.cpp:3197
				break;
			case 2:
				// taskmanager.cpp:3200
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xb5\xc7\xb0\xed \xc0\xd6\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:3202
				break;
			case 3:
				// taskmanager.cpp:3205
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xb4\xeb\xc8\xad\xb8\xed\xc0\xba \\c0xffff0000\\c\xbf\xb5\xb9\xae 4 - 16 \xb1\xdb\xc0\xda\\c0xff000000\\c \xb6\xc7\xb4\xc2 \\c0xffff0000\\c\xc7\xd1\xb1\xdb 2 - 8\xb1\xdb\xc0\xda\\c0xff000000\\c\xc0\xc7 \xb1\xe6\xc0\xcc\xb8\xa6 \xb0\xa1\xc1\xae\xbe\xdf \xc7\xcf\xb8\xe7, \\c0xffff0000\\c\xb0\xf8\xb9\xe9\xb9\xae\xc0\xda\\c0xff000000\\c\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:3207
				break;
			case 4:
				// taskmanager.cpp:3210
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xc4\xed\xc5\xb0\\c0xff000000\\c\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.\n\xbb\xf3\xc1\xa1\xbf\xa1\xbc\xad \xc4\xed\xc5\xb0\xb8\xa6 \xc3\xe6\xc0\xfc\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.", 0, 0, 0, 0));
				// taskmanager.cpp:3212
				break;
			case 5:
				// taskmanager.cpp:3216
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xbb\xe7\xbf\xeb\xc0\xcc \xb1\xdd\xc1\xf6\xb5\xc8 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:3218
				break;
			case 1:
			case 6:
				// taskmanager.cpp:3222
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xba\xaf\xb0\xe6 \xbf\xe4\xc3\xbb\xc0\xcc \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:3224
				break;
			case 7:
				// taskmanager.cpp:3227
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xb4\xeb\xc8\xad\xb8\xed \xba\xaf\xb0\xe6\xc0\xcc \xc0\xcf\xbd\xc3\xc0\xfb\xc0\xb8\xb7\xce \xc1\xdf\xb4\xdc\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:3229
				break;
			case 9:
				// taskmanager.cpp:3242
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xc0\xcc\xc0\xfc \xb4\xeb\xc8\xad\xb8\xed \xba\xaf\xb0\xe6 \xc8\xc4 1\xc1\xd6\xc0\xcf\xc0\xcc \xc1\xf6\xb3\xaa\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:3244
				break;
			case 11:
				// taskmanager.cpp:3249
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xb4\xeb\xc8\xad\xb8\xed \xb8\xed\xc2\xfb\xc0\xbb \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:3251
				break;
			default:
				// taskmanager.cpp:3255
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xb5\xc7\xb0\xed \xc0\xd6\xb4\xc2 \xb4\xeb\xc8\xad\xb8\xed\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			}
			// taskmanager.cpp:3260
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x40, result, 0, 0, 0, 0));
		}
		break;
	case 0x4f:
		// taskmanager.cpp:3266
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		{
			// taskmanager.cpp:3268
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:3269
			switch (result) {
			case 0:
				// taskmanager.cpp:3273
				CSharedDoc::Instance()->m_userInfo.schoolNo = packet.Decode4();
				// taskmanager.cpp:3274
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc7\xd0\xb1\xb3\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			default:
				// taskmanager.cpp:3280
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc7\xd0\xb1\xb3 \xba\xaf\xb0\xe6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			}
		}
		break;
	case 0x6e:
		// taskmanager.cpp:3290
		{
			sCharacterInfo character;
			// taskmanager.cpp:3292
			CPartTidList tidList;
			// taskmanager.cpp:3295
			int total = packet.Decode2();
			// taskmanager.cpp:3296
			int count = packet.Decode2();
			// taskmanager.cpp:3298
			for (int i = 0; i < count; i++) {
				// taskmanager.cpp:3300
				packet.DecodeBuffer(&character, sizeof(sCharacterInfo));
				// taskmanager.cpp:3305
				tidList.SetTids(&character, CSharedDoc::Instance()->m_myLevel);
				// taskmanager.cpp:3307
				CSharedDoc::Instance()->AddChar(&character);
			}
			// taskmanager.cpp:3309
			if (total == count) {
				// taskmanager.cpp:3311
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2C, 1, 0, 0, 0, 0));
			}
		}
		break;
	case 0x6f:
		// taskmanager.cpp:3324
		{
			int total = packet.Decode2();
			// taskmanager.cpp:3325
			int count = packet.Decode2();
			// taskmanager.cpp:3327
			for (int i = 0; i < count; i++) {
				sCaddieInfo caddie;
				// taskmanager.cpp:3329
				packet.DecodeBuffer(&caddie, sizeof(sCaddieInfo));
				// taskmanager.cpp:3330
				CSharedDoc::Instance()->AddCaddie(&caddie);
				// taskmanager.cpp:3331
				if (!caddie.checkEquip) {
					continue;
				}
				// taskmanager.cpp:3333
				CSharedDoc *pDoc = CSharedDoc::Instance();
				if (pDoc->m_myCaddieId) {
					// taskmanager.cpp:3336
					std::map<unsigned int, sCaddieInfo>::iterator it = pDoc->m_caddieMap.find(pDoc->m_myCaddieId);
					// taskmanager.cpp:3339
					if (it == pDoc->m_caddieMap.end() && pDoc->m_caddieMap.size() > 0) {
						// taskmanager.cpp:3345
						// The equipped caddie is gone: equip the first one.
						pDoc->m_myCaddieId = pDoc->m_caddieMap.begin()->first;
					}
				}
				// taskmanager.cpp:3355
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x68, 1, 0, 0, 0, 0));
			}
			// taskmanager.cpp:3359
			if (total == count) {
				// taskmanager.cpp:3361
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2C, 2, 0, 0, 0, 0));
			}
		}
		break;
	case 0xdf:
		// taskmanager.cpp:3372
		{
			unsigned char count = packet.Decode1();
			// taskmanager.cpp:3374
			for (unsigned char i = 0; i < count; i++) {
				sMascotInfo mascot;
				// taskmanager.cpp:3376
				packet.DecodeBuffer(&mascot, sizeof(sMascotInfo));
				// taskmanager.cpp:3378
				CSharedDoc::Instance()->AddMascot(&mascot);
				// taskmanager.cpp:3379
				if (!mascot.pcBang) {
					continue;
				}
				// A PC-bang mascot shows the message of the options.
				// taskmanager.cpp:3382
				strcpy(mascot.message, COption::Instance()->GetOption()->m_pcBangMascotMsg);
				// taskmanager.cpp:3383
				CSharedDoc::Instance()->AddMascot(&mascot);
				// taskmanager.cpp:3386
				WSendPacket send(GCT_UPDATE_PCBANG_MASCOTMSG);
				// taskmanager.cpp:3387
				send.Encode1(2);
				// taskmanager.cpp:3388
				send.Encode4(mascot.id);
				// taskmanager.cpp:3389
				send.EncodeStr(std::string(mascot.message));
				// taskmanager.cpp:3390
				send.Send(SEND_TO_DEFAULT);
				// taskmanager.cpp:3392
				CSharedDoc *pDoc = CSharedDoc::Instance();
				if (pDoc->m_myMascotId) {
					// taskmanager.cpp:3396
					std::map<unsigned int, sMascotInfo>::iterator it = pDoc->m_myMascotMap.find(pDoc->m_myMascotId);
					// taskmanager.cpp:3398
					if (it == pDoc->m_myMascotMap.end()) {
						// taskmanager.cpp:3400
						pDoc->m_myMascotId = 0;
					}
				}
				// taskmanager.cpp:3406
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x68, 2, 0, 0, 0, 0));
			}
			// taskmanager.cpp:3410
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2C, 0x10, 0, 0, 0, 0));
		}
		break;
	case 0x70:
		// taskmanager.cpp:3418
		{
			sUserEquip equip;
			packet.DecodeBuffer(&equip, sizeof(equip));
			CSharedDoc::Instance()->SetCurEquipInfo(&equip);
		}
		break;
	case 0x71:
		// taskmanager.cpp:3428
		{
			sItemInfo item;
			// taskmanager.cpp:3431
			int total = packet.Decode2();
			// taskmanager.cpp:3432
			int count = packet.Decode2();
			// taskmanager.cpp:3434
			for (int i = 0; i < count; i++) {
				// taskmanager.cpp:3436
				packet.DecodeBuffer(&item, sizeof(sItemInfo));
				// An item the lists already hold is not added again.
				bool isNew = true;
				// taskmanager.cpp:3441
				switch (item.m_typeId >> 26) {
				case 2:
					// taskmanager.cpp:3445
					if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(item.m_typeId)) {
						// taskmanager.cpp:3448
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myItemList.begin(); it != CSharedDoc::Instance()->m_myItemList.end(); it++) {
							// taskmanager.cpp:3450
							if (it->m_typeId == item.m_typeId && strcmp(it->m_uccKey, item.m_uccKey) == 0 && it->m_uccSeq == item.m_uccSeq) {
								// taskmanager.cpp:3453
								isNew = false;
								break;
							}
						}
					}
					// taskmanager.cpp:3461
					{
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myPartsList.begin(); it != CSharedDoc::Instance()->m_myPartsList.end(); it++) {
							// taskmanager.cpp:3463
							if (it->m_serial == item.m_serial) {
								isNew = false;
								break;
							}
						}
					}
					break;
				case 4:
					// taskmanager.cpp:3477
					{
						std::map<unsigned int, sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myItemMap.begin(); it != CSharedDoc::Instance()->m_myItemMap.end(); it++) {
							// taskmanager.cpp:3479
							if (it->second.m_serial == item.m_serial) {
								isNew = false;
								break;
							}
						}
					}
					break;
				case 5:
					// taskmanager.cpp:3493
					{
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); it++) {
							// taskmanager.cpp:3495
							if (it->m_serial == item.m_serial) {
								// taskmanager.cpp:3498
								isNew = false;
								break;
							}
						}
					}
					// taskmanager.cpp:3502
					if (item.m_count > 19000) {
						// taskmanager.cpp:3504
						if (item.m_count > 20000) {
							// taskmanager.cpp:3506
							CSharedDoc::Instance()->m_value5064 = 2;
						} else if (CSharedDoc::Instance()->m_value5064 != 2) {
							// taskmanager.cpp:3512
							CSharedDoc::Instance()->m_value5064 = 1;
						}
					}
					break;
				case 6:
					// taskmanager.cpp:3523
					{
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); it++) {
							// taskmanager.cpp:3525
							if (it->m_serial == item.m_serial) {
								// taskmanager.cpp:3528
								isNew = false;
								break;
							}
						}
					}
					// taskmanager.cpp:3532
					if (item.m_count > 19000) {
						// taskmanager.cpp:3534
						if (item.m_count > 20000) {
							// taskmanager.cpp:3536
							CSharedDoc::Instance()->m_value5064 = 2;
						} else if (CSharedDoc::Instance()->m_value5064 != 2) {
							// taskmanager.cpp:3542
							CSharedDoc::Instance()->m_value5064 = 1;
						}
					}
					break;
				case 8:
					// taskmanager.cpp:3553
					{
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myExtraItemList.begin(); it != CSharedDoc::Instance()->m_myExtraItemList.end(); it++) {
							// taskmanager.cpp:3555
							if (it->m_serial == item.m_serial) {
								isNew = false;
								break;
							}
						}
					}
					break;
				case 9:
					// taskmanager.cpp:3569
					{
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myExtraItemList2.begin(); it != CSharedDoc::Instance()->m_myExtraItemList2.end(); it++) {
							// taskmanager.cpp:3571
							if (it->m_serial == item.m_serial) {
								isNew = false;
								break;
							}
						}
					}
					break;
				case 0xe:
					// taskmanager.cpp:3585
					{
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_mySkinList.begin(); it != CSharedDoc::Instance()->m_mySkinList.end(); it++) {
							// taskmanager.cpp:3587
							if (it->m_serial == item.m_serial) {
								isNew = false;
								break;
							}
						}
					}
					break;
				case 0x1c:
					// taskmanager.cpp:3601
					{
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myExtraItemList3.begin(); it != CSharedDoc::Instance()->m_myExtraItemList3.end(); it++) {
							// taskmanager.cpp:3603
							if (it->m_serial == item.m_serial) {
								isNew = false;
								break;
							}
						}
					}
					break;
				case 0x1d:
					// taskmanager.cpp:3617
					{
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myExtraItemList4.begin(); it != CSharedDoc::Instance()->m_myExtraItemList4.end(); it++) {
							// taskmanager.cpp:3619
							if (it->m_serial == item.m_serial) {
								isNew = false;
								break;
							}
						}
					}
					break;
				case 0x12:
					// taskmanager.cpp:3630
					if (IsLocalContent((localContentType_t)0x56)) {
						// taskmanager.cpp:3634
						std::list<sFurniture_List>::iterator it;
						for (it = CSharedDoc::Instance()->m_furnitureList.begin(); it != CSharedDoc::Instance()->m_furnitureList.end(); it++) {
							// taskmanager.cpp:3636
							if (it->m_id == item.m_serial) {
								isNew = false;
								break;
							}
						}
					}
					break;
				}

				// taskmanager.cpp:3650
				if (!isNew) {
					continue;
				}
				// taskmanager.cpp:3652
				switch (item.m_typeId >> 26) {
				case 2:
					// taskmanager.cpp:3656
					if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(item.m_typeId)) {
						// taskmanager.cpp:3658
						if (item.m_uccStatus & 1) {
							// taskmanager.cpp:3660
							sUccClothes clothes;
							// taskmanager.cpp:3661
							clothes.m_serial = item.m_serial;
							// taskmanager.cpp:3662
							clothes.m_typeId = item.m_typeId;
							// taskmanager.cpp:3663
							strncpy(clothes.m_uccKey, item.m_uccKey, 9);
							// taskmanager.cpp:3664
							strncpy(clothes.m_uccName, item.m_uccName, 0x29);
							// taskmanager.cpp:3665
							clothes.m_period[0] = item.m_reserved18[0]; clothes.m_period[1] = item.m_reserved18[1]; clothes.m_period[2] = item.m_reserved18[2]; clothes.m_period[3] = item.m_reserved18[3];
							clothes.m_downloadCount = 0;
							// taskmanager.cpp:3667
							UccManager()->AddClothes(1, clothes, true, false);
						} else if (item.m_uccStatus & 2) {
							// taskmanager.cpp:3671
							NetResourceManager::Instance()->DownloadUccTempClothes(item.m_typeId, item.m_uccKey, item.m_uccSeq, false);
						}
						// taskmanager.cpp:3674
						CSharedDoc::Instance()->m_myItemList.push_back(item);
					} else {
						// taskmanager.cpp:3678
						CSharedDoc::Instance()->AddMyPartsList(item);
					}
					break;
				case 4:
					// taskmanager.cpp:3683
					CSharedDoc::Instance()->m_myItemMap.insert(std::map<unsigned int, sItemInfo>::value_type(item.m_serial, item));
					break;
				case 5:
					// taskmanager.cpp:3686
					CSharedDoc::Instance()->m_myGearList.push_back(item);
					break;
				case 6:
					// taskmanager.cpp:3689
					CSharedDoc::Instance()->m_itemInfoList.push_back(item);
					break;
				case 8:
					// taskmanager.cpp:3692
					CSharedDoc::Instance()->m_myExtraItemList.push_back(item);
					break;
				case 9:
					// taskmanager.cpp:3695
					CSharedDoc::Instance()->m_myExtraItemList2.push_back(item);
					break;
				case 0xe:
					// taskmanager.cpp:3698
					CSharedDoc::Instance()->m_mySkinList.push_back(item);
					break;
				case 0x1c:
					// taskmanager.cpp:3701
					CSharedDoc::Instance()->m_myExtraItemList3.push_back(item);
					break;
				case 0x1d:
					// taskmanager.cpp:3704
					CSharedDoc::Instance()->m_myExtraItemList4.push_back(item);
					break;
				}
			}
			// taskmanager.cpp:3713
			if (total == count) {
				// taskmanager.cpp:3715
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2C, 4, 0, 0, 0, 0));
			}
			// taskmanager.cpp:3717
			if (IsLocalContent((localContentType_t)0x17)) {
				// taskmanager.cpp:3718
				CSharedDoc::Instance()->SetInitHaveHalloweenItem();
			}
			// taskmanager.cpp:3721
			if (IsLocalContent((localContentType_t)0x57) && AfxGetTask()->GetActor("RealMyRoom")) {
				// taskmanager.cpp:3725
				AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);
			}
		}
		break;
	case 0x17d:
		// taskmanager.cpp:3735
		CSharedDoc::Instance()->ClearCardStack();
		{
			// taskmanager.cpp:3736
			int count = packet.Decode2();
			// taskmanager.cpp:3738
			for (int i = 0; i < count; i++) {
				sCardStack cardStack;
				// taskmanager.cpp:3740
				packet.DecodeBuffer(&cardStack, sizeof(cardStack));
				// taskmanager.cpp:3741
				CSharedDoc::Instance()->AddCardStack(cardStack, false);
			}
		}
		break;
	case 0x12d:
		// taskmanager.cpp:3750
		CSharedDoc::Instance()->m_myCardList.clear();
		break;
	case 0x130:
		// taskmanager.cpp:3760
		{
			int result = packet.Decode4();
			// taskmanager.cpp:3761
			if (result == 0) {
				// taskmanager.cpp:3763
				int count = packet.Decode2();
				// taskmanager.cpp:3764
				for (int i = 0; i < count; i++) {
					sCards card;
					// taskmanager.cpp:3766
					packet.DecodeBuffer(&card, sizeof(card));
					// taskmanager.cpp:3770
					if ((card.m_typeId & 0xFC000000) == 0x7C000000) {
						std::list<sCards> &cardList = CSharedDoc::Instance()->m_myCardList;
						bool found = false;
						// taskmanager.cpp:3774
						std::list<sCards>::iterator it;
						for (it = cardList.begin(); it != cardList.end(); it++) {
							// taskmanager.cpp:3776
							if (it->m_guid == card.m_guid) {
								found = true;
								break;
							}
						}
						if (!found) {
							// taskmanager.cpp:3786
							sCards newCard = card;
							// taskmanager.cpp:3787
							cardList.push_back(newCard);
						}
					} else {
						// taskmanager.cpp:3792
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x25, (intptr_t)"\xc0\xdf\xb8\xf8\xb5\xc8 \xc4\xab\xb5\xe5 \xb5\xa5\xc0\xcc\xc5\xcd \xc6\xd0\xc5\xb6\xc0\xd4\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
					}
				}
				// taskmanager.cpp:3797
				GetActor("RealMyRoom") << MsgObject(0, 0x21A, 0, 0, 0, 0, 0);
			} else if (result == 1) {
				// taskmanager.cpp:3806
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x25, (intptr_t)"\xc4\xab\xb5\xe5 \xc1\xa4\xba\xb8\xb8\xa6 \xb9\xde\xb4\xc2\xb5\xa5 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
			}
		}
		break;
	case 0x12e:
		// taskmanager.cpp:3812
		for (int i = 0; i < 4; i++) {
			// taskmanager.cpp:3813
			CSharedDoc::Instance()->m_cardPeriodList[i].clear();
		}
		break;
	case 0x12f:
		{
			// taskmanager.cpp:3819
			int count = packet.Decode2();
			// taskmanager.cpp:3820
			for (int i = 0; i < count; i++) {
				sSCardAvilityPeriodInfo recv;
				// taskmanager.cpp:3822
				packet.DecodeBuffer(&recv, sizeof(recv));
				// taskmanager.cpp:3826
				if ((recv.typeId & 0xFC000000) == 0x7C000000) {
					// taskmanager.cpp:3843
					sSCardAvilityPeriodInfo info = recv;
					// taskmanager.cpp:3847
					IFF_STRUCT::sCard *pCard = ItemManager()->FindCard(info.typeId);
					// taskmanager.cpp:3848
					if (pCard) {
						// taskmanager.cpp:3850
						info.effect = pCard->m_effectType;
						// taskmanager.cpp:3851
						info.effectValue = pCard->m_effectValue;
					}
					// taskmanager.cpp:3856
					info.kind = (info.typeId >> 22) & 0xF;
					CSharedDoc::Instance()->m_cardPeriodList[0].push_back(info);
				} else {
					// taskmanager.cpp:3862
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x25, (intptr_t)"\xc0\xdf\xb8\xf8\xb5\xc8 \xc4\xab\xb5\xe5 \xb5\xa5\xc0\xcc\xc5\xcd \xc6\xd0\xc5\xb6\xc0\xd4\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
				}
			}
			// taskmanager.cpp:3866
			MsgObject msg(0, 2, 0, 0, 0, 0, 0);
			GetActor("RealMyRoom") << msg;
		}
		break;
	case 0x158:
		{
			// taskmanager.cpp:4003
			unsigned long result = packet.Decode4();
			// taskmanager.cpp:4004
			if (result == 0) {
				// taskmanager.cpp:4006
				sCards card;
				packet.DecodeBuffer(&card, sizeof(sCards));
				// taskmanager.cpp:4009
				std::list<sCards> &cardList = CSharedDoc::Instance()->m_myCardList;
				std::list<sCards>::iterator it;
				for (it = cardList.begin(); it != cardList.end(); it++) {
					// taskmanager.cpp:4012
					if (it->m_typeId == card.m_typeId) {
						// taskmanager.cpp:4014
						it->m_count--;
						// taskmanager.cpp:4015
						if ((int)it->m_count < 1) {
							// taskmanager.cpp:4016
							CSharedDoc::Instance()->m_myCardList.erase(it);
						}
						break;
					}
				}
				// taskmanager.cpp:4024
				// The period entry of the used card. sCards' 0x8-0x37 are not
				// named in SharedDoc.h yet (0x8 parts type id, 0xC parts id,
				// 0x10 slot, 0x18 and 0x28 the start and end SYSTEMTIMEs).
				sSCardAvilityPeriodInfo info;
				info.id = card.m_guid;
				info.typeId = card.m_typeId;
				// taskmanager.cpp:4028
				memcpy(&info.startTime, &card.unknown18[0], sizeof(info.startTime));
				// taskmanager.cpp:4029
				memcpy(&info.endTime, &card.unknown18[4], sizeof(info.endTime));
				info.kind = (card.m_typeId >> 22) & 0xF;
				// taskmanager.cpp:4032
				info.partsTypeId = card.unknown8[0];
				// taskmanager.cpp:4033
				info.partsId = card.unknown8[1];
				// taskmanager.cpp:4034
				info.slot = card.unknown8[2];
				// taskmanager.cpp:4036
				IFF_STRUCT::sCard *pCard = ItemManager()->FindCard(card.m_typeId);
				// taskmanager.cpp:4037
				if (pCard) {
					// taskmanager.cpp:4039
					info.effect = pCard->m_effectType;
					// taskmanager.cpp:4040
					info.effectValue = pCard->m_effectValue;
				}
				// taskmanager.cpp:4043
				if (info.kind == 2) {
					// taskmanager.cpp:4045
					if (pCard->m_effectType == 1) {
						// taskmanager.cpp:4046
						CSharedDoc::Instance()->m_myExp += pCard->m_effectValue;
					}
					// taskmanager.cpp:4049
					if (pCard->m_effectType != 1 && pCard->m_effectType != 4 && pCard->m_effectType != 0x11) {
						// taskmanager.cpp:4051
						if (IsLocalContent((localContentType_t)0x6F)) {
							// taskmanager.cpp:4054
							std::list<sSCardAvilityPeriodInfo> &periodList = CSharedDoc::Instance()->m_cardPeriodList[0];
							for (std::list<sSCardAvilityPeriodInfo>::iterator it = periodList.begin(); it != periodList.end(); it++) {
								// taskmanager.cpp:4059
								if ((it->typeId & 0x3C00000) == 0x800000) {
									// taskmanager.cpp:4061
									if (info.effect == it->effect) {
										// taskmanager.cpp:4063
										periodList.erase(it);
										break;
									}
								}
							}
							// taskmanager.cpp:4068
							CSharedDoc::Instance()->m_cardPeriodList[0].push_back(info);
						} else {
							// taskmanager.cpp:4072
							CSharedDoc::Instance()->m_cardPeriodList[0].push_back(info);
						}
						// taskmanager.cpp:4075
						CCardManager::Instance()->SetSPCardAlarm(true);
					}
					// taskmanager.cpp:4078
				} else if (info.kind == 0 || info.kind == 1) {
					if (info.kind == 1) {
						// taskmanager.cpp:4081
						info.slot -= 4;
					}
					{
						// taskmanager.cpp:4085
						std::list<sSCardAvilityPeriodInfo> &periodList = CSharedDoc::Instance()->m_cardPeriodList[0];
						std::list<sSCardAvilityPeriodInfo>::iterator it;
						for (it = periodList.begin(); it != periodList.end(); it++) {
							// taskmanager.cpp:4087
							if (it->partsId == info.partsId) {
								// taskmanager.cpp:4089
								if (it->kind == info.kind && it->slot == info.slot) {
									// taskmanager.cpp:4091
									periodList.erase(it);
									break;
								}
							}
						}
						// taskmanager.cpp:4098
						CSharedDoc::Instance()->m_cardPeriodList[0].push_back(info);
					}
				}
				// taskmanager.cpp:4103
				CCardManager::Instance()->UpdateCardBuffDlg();
				// taskmanager.cpp:4106
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xDD, 1, 0, 0, 0, 0));
				// taskmanager.cpp:4107
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x219, card.m_typeId, 0, 0, 0, 0));
			} else if (result == 1) {
				// taskmanager.cpp:4112
				unsigned char error = packet.Decode1();
				// taskmanager.cpp:4114
				if (error == 1) {
					// taskmanager.cpp:4116
					AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23,
						(intptr_t)"\xc0\xcc \xc4\xab\xb5\xe5\xc0\xc7 \xc8\xbf\xb0\xfa\xb4\xc2 \xc0\xcc\xb9\xcc \xc0\xfb\xbf\xeb\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
				} else {
					// taskmanager.cpp:4119
					AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x23,
						(intptr_t)"\xc4\xab\xb5\xe5\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xcf\xb4\xc2\xb5\xa5 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0);
				}
			}
			// taskmanager.cpp:4122
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		}
		break;
	case 0x131:
		// taskmanager.cpp:4129
		{
			int count = packet.Decode2();
			// taskmanager.cpp:4131
			for (int i = 0; i < count; i++) {
				sSCardAvilityPeriodInfo info;
				// taskmanager.cpp:4133
				packet.DecodeBuffer(&info, sizeof(info));
				// taskmanager.cpp:4135
				std::list<sSCardAvilityPeriodInfo> &periods = CSharedDoc::Instance()->m_cardPeriodList[0];
				std::list<sSCardAvilityPeriodInfo>::iterator it;
				for (it = periods.begin(); it != periods.end(); it++) {
					// taskmanager.cpp:4138
					if (it->id == info.id) {
						// taskmanager.cpp:4140
						periods.erase(it);
						break;
					}
				}
			}
		}
		break;
	case 0x15a:
		// taskmanager.cpp:4150
		{
			int count = packet.Decode2();
			// taskmanager.cpp:4152
			for (int i = 0; i < count; i++) {
				sSCardAvilityPeriodInfo info;
				// taskmanager.cpp:4154
				packet.DecodeBuffer(&info, sizeof(info));
				// taskmanager.cpp:4155
				CSharedDoc::Instance()->m_cardPeriodList[0].push_back(info);
			}
		}
		break;
	case 0x159:
		// taskmanager.cpp:4162
		{
			int count = packet.Decode2();
			// taskmanager.cpp:4164
			for (int i = 0; i < count; i++) {
				sSCardAvilityPeriodInfo info;
				// taskmanager.cpp:4166
				packet.DecodeBuffer(&info, sizeof(info));
				// taskmanager.cpp:4168
				std::list<sSCardAvilityPeriodInfo> &periods = CSharedDoc::Instance()->m_cardPeriodList[0];
				for (std::list<sSCardAvilityPeriodInfo>::iterator it = periods.begin(); it != periods.end(); it++) {
					// taskmanager.cpp:4171
					if (it->id == info.id) {
						// taskmanager.cpp:4173
						it->partsTypeId = info.partsTypeId;
						// taskmanager.cpp:4174
						it->partsId = info.partsId;
						break;
					}
				}
			}
		}
		break;
	case 0x17f:
		// taskmanager.cpp:4184
		{
			unsigned long oldPartsTypeId = packet.Decode4();
			// taskmanager.cpp:4185
			unsigned long oldPartsId = packet.Decode4();
			// taskmanager.cpp:4186
			unsigned long newPartsTypeId = packet.Decode4();
			// taskmanager.cpp:4187
			unsigned long newPartsId = packet.Decode4();
			// taskmanager.cpp:4189
			std::list<sSCardAvilityPeriodInfo>::iterator it;
			for (it = CSharedDoc::Instance()->m_cardPeriodList[0].begin(); it != CSharedDoc::Instance()->m_cardPeriodList[0].end(); it++) {
				// taskmanager.cpp:4191
				sSCardAvilityPeriodInfo &info = *it;
				// taskmanager.cpp:4192
				if (info.partsTypeId == oldPartsTypeId && info.partsId == oldPartsId) {
					// taskmanager.cpp:4194
					info.partsTypeId = newPartsTypeId;
					// taskmanager.cpp:4195
					info.partsId = newPartsId;
				}
			}
		}
		break;
	case 0x13a: {
		// taskmanager.cpp:4208
		sSCardAvilityPeriodInfo info;
		memset(&info, 0, sizeof(info));
		// taskmanager.cpp:4210
		int result = packet.Decode4();
		// taskmanager.cpp:4211
		if (result == 0) {
			// taskmanager.cpp:4213
			packet.DecodeBuffer(&info, sizeof(info));
			// taskmanager.cpp:4216
			std::list<sSCardAvilityPeriodInfo>& periodList = CSharedDoc::Instance()->m_cardPeriodList[0];
			std::list<sSCardAvilityPeriodInfo>::iterator it;
			for (it = periodList.begin(); it != periodList.end(); it++) {
				// taskmanager.cpp:4224
				if (it->id == info.id) {
					// taskmanager.cpp:4227
					if (it->effect == 0xB) {
						// taskmanager.cpp:4229
						int slot = CCardManager::Instance()->GetCardPeriodSlot();
						// taskmanager.cpp:4230
						if (CSharedDoc::Instance()->GetNumAddItemSlotByMascot() == 1 || slot > 1) {
							// taskmanager.cpp:4232
							CSharedDoc::Instance()->m_equipItem[9] = 0;
						// taskmanager.cpp:4234
						} else if (CSharedDoc::Instance()->GetNumAddItemSlotByMascot() == 0 && slot == 0) {
							// taskmanager.cpp:4236
							CSharedDoc::Instance()->m_equipItem[8] = 0;
							// taskmanager.cpp:4237
							CSharedDoc::Instance()->m_equipItem[9] = 0;
						}
					}
					// taskmanager.cpp:4241
					CSharedDoc::Instance()->m_cardPeriodList[0].erase(it);
					// taskmanager.cpp:4242
					CCardManager::Instance()->UpdateCardBuffDlg();
					// taskmanager.cpp:4243
					break;
				}
			}
		// taskmanager.cpp:4247
		} else if (result == 1) {
			// taskmanager.cpp:4249
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc4\xab\xb5\xe5\xc0\xc7 \xb1\xe2\xb0\xa3\xc1\xa6 \xc6\xaf\xbc\xf6\xb4\xc9\xb7\xc2 \xbb\xe8\xc1\xa6\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
		}
		break;
	}
	case 0x14c: {
		// taskmanager.cpp:4260
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:4261
		FrCardBoxDlg* pCardBox = (FrCardBoxDlg*)g_pFresh->GetManager()->GetDesktop()->FindChildForm("FrCardBoxDlg");
		// taskmanager.cpp:4262
		if (pCardBox) {
			// taskmanager.cpp:4264
			pCardBox->RecievedPacket(&packet);
			break;
		}
		// taskmanager.cpp:4273
		int result = packet.Decode4();
		// taskmanager.cpp:4275
		unsigned long typeIds[4] = { 0, 0, 0, 0 };
		// taskmanager.cpp:4277
		if (result == 0) {
			sCards card;
			// taskmanager.cpp:4300
			packet.DecodeBuffer(&card, sizeof(sCards));
			// taskmanager.cpp:4303
			typeIds[0] = card.m_typeId;
			std::list<sCards>& cardList = CSharedDoc::Instance()->m_myCardList;
			for (std::list<sCards>::iterator it = cardList.begin(); it != cardList.end(); ++it) {
				// taskmanager.cpp:4306
				if (it->m_typeId == card.m_typeId) {
					// taskmanager.cpp:4308
					it->m_count--;
					// taskmanager.cpp:4309
					if ((int)it->m_count < 1) {
						// taskmanager.cpp:4310
						CSharedDoc::Instance()->m_myCardList.erase(it);
					}
					break;
				}
			}
			// taskmanager.cpp:4315
			int newCount = packet.Decode1();
			unsigned int slot = 1;
			// taskmanager.cpp:4318
			for (int i = 0; i < newCount; i++) {
				// taskmanager.cpp:4320
				packet.DecodeBuffer(&card, sizeof(sCards));
				// taskmanager.cpp:4321
				int count = packet.Decode4();
				// taskmanager.cpp:4324
				for (int j = 0; j < count; j++) {
					// taskmanager.cpp:4328
					typeIds[slot] = card.m_typeId;
					if (slot < 3) {
						// taskmanager.cpp:4329
						slot++;
					}
				}
				// taskmanager.cpp:4333
				std::list<sCards>::iterator found = CSharedDoc::Instance()->m_myCardList.begin();
				for (; found != CSharedDoc::Instance()->m_myCardList.end(); ++found) {
					// taskmanager.cpp:4336
					if (found->m_typeId == card.m_typeId) {
						// taskmanager.cpp:4338
						found->m_count += count;
						// taskmanager.cpp:4344
						break;
					}
				}
				if (found == CSharedDoc::Instance()->m_myCardList.end()) {
					// taskmanager.cpp:4345
					CSharedDoc::Instance()->m_myCardList.push_back(card);
				}
			}
			// taskmanager.cpp:4348
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xDD, 1, 0, 0, 0, 0));
			// taskmanager.cpp:4349
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
			// taskmanager.cpp:4350
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x229, typeIds[0], typeIds[1], typeIds[2], typeIds[3], 0));
		// taskmanager.cpp:4352
		} else if (result == 1) {
			// taskmanager.cpp:4354
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
			// taskmanager.cpp:4355
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc4\xab\xb5\xe5\xc6\xd1 \xbf\xc0\xc7\xc2\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
		}
		break;
	}
	case 0x73:
		// taskmanager.cpp:4362
		// "Sent an invalid data packet"
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x25, (intptr_t)"\xC0\xDF\xB8\xF8\xB5\xC8 \xB5\xA5\xC0\xCC\xC5\xCD \xC6\xD0\xC5\xB6\xC0\xBB \xBA\xB8\xB3\xC2\xBD\xC0\xB4\xCF\xB4\xD9", 0, 0, 0, 0));
		break;
	case 0x13d:
		// taskmanager.cpp:4369
		if (!IsLocalContent((localContentType_t)0x5B)) {
			break;
		}
		{
			// taskmanager.cpp:4371
			unsigned char page = packet.Decode1();
			// taskmanager.cpp:4372
			unsigned char pageCount = packet.Decode1();
			// taskmanager.cpp:4373
			unsigned short count = packet.Decode2();
			// taskmanager.cpp:4375
			for (unsigned short i = 0; i < count; i++) {
				sGiftInfo gift;
				// taskmanager.cpp:4378
				packet.DecodeBuffer(&gift, 0xFB);   // the wire record, one byte short of sizeof(sGiftInfo)
				// taskmanager.cpp:4380
				CSharedDoc::Instance()->m_chatManager.FilteringHack(gift.m_message, false);
				// taskmanager.cpp:4381
				CSharedDoc::Instance()->m_giftList.push_back(gift);
			}
			// taskmanager.cpp:4384
			CSharedDoc::Instance()->m_value5055 = page;
			// taskmanager.cpp:4385
			CSharedDoc::Instance()->m_value5056 = pageCount;
			// taskmanager.cpp:4386
			CSharedDoc::Instance()->m_value5058 = count;
			// taskmanager.cpp:4387
			CSharedDoc::Instance()->m_value505A = 1;
			// taskmanager.cpp:4389
			if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&CRealMyRoomTask::m_RTTI) : false) {
				AfxPostMsg(0, "RealMyRoom", 0xC6, 0, 0, 0, 0);
			} else if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false) {
				// taskmanager.cpp:4392
				CProjectG::Instance()->FromGolfToLobbySettings();
				// taskmanager.cpp:4393
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "", true);
				// taskmanager.cpp:4394
				AfxPostMsg(0, "RealMyRoom", 0, (intptr_t)"REALMYROOM", 0, 0, 0);
				// taskmanager.cpp:4395
				AfxPostMsg(0, "RealMyRoom", 0xC6, 0, 0, 0, 0);
				// taskmanager.cpp:4397
				ntAvatarManager::Instance()->SetActive(false);
			} else if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&CTutorialTask::m_RTTI) : false) {
				// taskmanager.cpp:4399
				CSharedDoc::Instance()->m_lobbyStateFlag2 = true;
				// taskmanager.cpp:4400
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "", false);
				// taskmanager.cpp:4401
				AfxPostMsg(0, "RealMyRoom", 0, (intptr_t)"REALMYROOM", 0, 0, 0);
				// taskmanager.cpp:4402
				AfxPostMsg(0, "RealMyRoom", 0xC6, 0, 0, 0, 0);
			} else {
				// taskmanager.cpp:4404
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "", CTaskManager::Instance()->GetPreservedTask() != 0);
				AfxPostMsg(0, "RealMyRoom", 0, (intptr_t)"REALMYROOM", 0, 0, 0);
				AfxPostMsg(0, "RealMyRoom", 0xC6, 0, 0, 0, 0);
			}
		}
		break;
	case 0x78:
		{
			// taskmanager.cpp:4416
			unsigned char kind = packet.Decode1();
			// taskmanager.cpp:4417
			unsigned short perPage = packet.Decode2();
			// taskmanager.cpp:4418
			unsigned short count = packet.Decode2();
			// taskmanager.cpp:4419
			unsigned short total = packet.Decode2();
			// taskmanager.cpp:4426
			CSharedDoc::Instance()->m_value5044 = perPage;
			CSharedDoc::Instance()->m_value5048 = count;
			CSharedDoc::Instance()->m_value504C = total;
			if (kind == 1) {
				// taskmanager.cpp:4429
				CSharedDoc::Instance()->m_giftList.clear();
				// taskmanager.cpp:4430
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x2C, 8, 0, 0, 0, 0));
				// taskmanager.cpp:4432
				for (unsigned short i = 0; i < count; i++) {
					sGiftInfo gift;
					// taskmanager.cpp:4435
					packet.DecodeBuffer(&gift, 0xFB);   // the wire record, one byte short of sizeof(sGiftInfo)
					// taskmanager.cpp:4437
					CSharedDoc::Instance()->m_chatManager.FilteringHack(gift.m_message, false);
					// taskmanager.cpp:4440
					IFF_ITEM_COMMON *pItem = ItemManager()->FindCommonItem(gift.m_typeId);
					// taskmanager.cpp:4441
					if (pItem && pItem->m_active == 1) {
						// taskmanager.cpp:4444
						CSharedDoc::Instance()->m_giftList.push_back(gift);
					}
				}
				// taskmanager.cpp:4447
				if (CSharedDoc::Instance()->m_giftList2.size() != 0) {
					// taskmanager.cpp:4449
					CSharedDoc::Instance()->m_tutorialFinish = 1;
					// taskmanager.cpp:4450
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xAA, 1, 0, 0, 0, 0));
				}
			} else {
				// taskmanager.cpp:4469
				int pages = total / perPage;
				// taskmanager.cpp:4470
				if (pages * perPage < total) {
					// taskmanager.cpp:4472
					pages++;
				}
				// taskmanager.cpp:4475
				if (pages) {
					// taskmanager.cpp:4477
					WSendPacket send((enumClientPacket)0x92);
					// taskmanager.cpp:4478
					send.Encode4(pages);
					// taskmanager.cpp:4479
					send.Send(SEND_TO_DEFAULT);
				} else {
					// taskmanager.cpp:4483
					WSendPacket send((enumClientPacket)0x92);
					// taskmanager.cpp:4484
					send.Encode4(1);
					// taskmanager.cpp:4485
					send.Send(SEND_TO_DEFAULT);
				}
			}
		}
		break;
	case 0x85:
		// taskmanager.cpp:4495
		// taskmanager.cpp:4496
		if (packet.Decode1()) {
			sCaddieInfo caddie;
			// taskmanager.cpp:4499
			packet.DecodeBuffer(&caddie, sizeof(sCaddieInfo));
			// taskmanager.cpp:4500
			CSharedDoc::Instance()->AddCaddie(&caddie);
			// taskmanager.cpp:4501
			CSharedDoc::Instance()->m_myCaddieId = caddie.id;
			// taskmanager.cpp:4502
			CSharedDoc::Instance()->m_roomPlayers[0].m_caddieInfo = caddie;
		}
		break;
	case 0x9d:
		// taskmanager.cpp:4511
		{
			CSharedDoc::Instance()->m_gameServerList.clear();
			// taskmanager.cpp:4512
			unsigned char serverCount = packet.Decode1();
			sGameServerInfo server;
			// taskmanager.cpp:4516
			for (int i = 0; i < serverCount; i++) {
				// taskmanager.cpp:4518
				memset(&server, 0, sizeof(server));
				// taskmanager.cpp:4519
				packet.DecodeBuffer(&server, sizeof(server));
				// taskmanager.cpp:4521
				if (server.flags & 0x1000) {
					continue;
				}
				// taskmanager.cpp:4525
				CSharedDoc::Instance()->m_gameServerList.push_back(server);
			}
			// taskmanager.cpp:4529
			CSharedDoc::Instance()->m_channelList.clear();
			// taskmanager.cpp:4530
			unsigned char channelCount = packet.Decode1();
			sChannelInfo channel;
			// taskmanager.cpp:4533
			for (int j = 0; j < channelCount; j++) {
				// taskmanager.cpp:4534
				packet.DecodeBuffer(&channel, sizeof(channel));
				// taskmanager.cpp:4535
				CSharedDoc::Instance()->m_channelList.push_back(channel);
			}
			// taskmanager.cpp:4538
			if (IsLocalContent((localContentType_t)0x53) || IsLocalContent((localContentType_t)0x54) ||
				IsLocalContent((localContentType_t)0x59)) {
				// taskmanager.cpp:4540
				if (CSharedDoc::Instance()->m_nowGoWithMode == 1) {
					// taskmanager.cpp:4542
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x216, 0, 0, 0, 0, 0));
				}
				// taskmanager.cpp:4544
				else if (CSharedDoc::Instance()->m_nowGoWithMode == 2) {
					// taskmanager.cpp:4546
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x216, 0, 0, 0, 0, 0));
				}
				// taskmanager.cpp:4548
				else if (CSharedDoc::Instance()->m_nowGoWithMode == 3) {
					// taskmanager.cpp:4550
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x216, 0, 0, 0, 0, 0));
				}
				// taskmanager.cpp:4552
				else {
					// taskmanager.cpp:4554
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x19, 0, 0, 0, 0, 0));
					// taskmanager.cpp:4555
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1B, 0, 0, 0, 0, 0));
				}
			} else {
				// taskmanager.cpp:4560
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x19, 0, 0, 0, 0, 0));
				// taskmanager.cpp:4561
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x1B, 0, 0, 0, 0, 0));
			}
		}
		break;
	case 0xfa:
		// taskmanager.cpp:4570
		{
			CMessengerInfo::Instance()->m_serverList.clear();
			// taskmanager.cpp:4571
			MSNServerInfo server;
			unsigned char count = packet.Decode1();
			for (int i = 0; i < count; i++) {
				// taskmanager.cpp:4578
				// Only the sGameServerInfo part comes from the packet.
				memset(&server, 0, sizeof(sGameServerInfo));
				packet.DecodeBuffer(&server, sizeof(sGameServerInfo));
				CMessengerInfo::Instance()->m_serverList.push_back(server);
			}
			// taskmanager.cpp:4585
			CMessengerInfo::Instance()->SetServerListWaitingState(false);
		}
		break;
	case 0x9f:
		// taskmanager.cpp:4591
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		{
			// taskmanager.cpp:4593
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:4594
			if (result == 0) {
				// taskmanager.cpp:4597
				unsigned long roomOwner = packet.Decode4();
				// taskmanager.cpp:4598
				char roomInfo[0x10D];
				packet.DecodeBuffer(roomInfo, 0x10D);
				// taskmanager.cpp:4601
				if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CRealMyRoomTask::m_RTTI) : false) {
					// taskmanager.cpp:4602
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x50, roomOwner, (intptr_t)roomInfo, 0, 0, 0));
				} else {
					// taskmanager.cpp:4604
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x50, roomOwner, (intptr_t)roomInfo, 0, 0, 0));
				}
			} else {
				// taskmanager.cpp:4609
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xaf\xc0\xfa\xb8\xa6 \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
			}
		}
		break;
	case 0xa0:
		// taskmanager.cpp:4616
		CSharedDoc::Instance()->m_rankServerAddress = packet.DecodeStr();
		// taskmanager.cpp:4617
		CSharedDoc::Instance()->m_rankServerPort = (unsigned short)packet.Decode4();
		// taskmanager.cpp:4619
		WNetworkSystem::Instance()->Init(WNetworkSystem::NET_UNIT_RANK);
		// taskmanager.cpp:4621
		WNetworkSystem::Instance()->SendMessage(WNetworkSystem::NET_UNIT_RANK, NetworkUnit::EVENT_NEXT);
		break;
	case 0xab:
		// taskmanager.cpp:4629
		{
			std::string nick;
			// taskmanager.cpp:4634
			packet.DecodeStr(nick);
			// taskmanager.cpp:4635
			unsigned char channelId = packet.Decode1();
			char text[512];
			// taskmanager.cpp:4637
			std::list<sChannelInfo>::iterator it = std::find(CSharedDoc::Instance()->m_channelList.begin(), CSharedDoc::Instance()->m_channelList.end(), channelId);
			// taskmanager.cpp:4638
			if (it == CSharedDoc::Instance()->m_channelList.end()) {
				// taskmanager.cpp:4639
				sprintf(text, "%s\xb4\xd4\xc0\xba [%s]\xbf\xa1\xbc\xad \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", nick.c_str(), CSharedDoc::Instance()->m_curServer.name);
			} else {
				// taskmanager.cpp:4641
				sprintf(text, "%s\xb4\xd4\xc0\xba [%s]\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xd8 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", nick.c_str(), it->name);
			}
			// taskmanager.cpp:4643
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 3, (intptr_t)text, 0xFFFF7878, 0, 0, 0));
		}
		break;
	case 0xc4:
		// taskmanager.cpp:4651
		if (CSharedDoc::Instance()->m_myLevel > 0) {
			// taskmanager.cpp:4653
			if (CSharedDoc::Instance()->m_myStatNoMannerGames > 0)
				// taskmanager.cpp:4654
				CSharedDoc::Instance()->m_myStatNoMannerGames--;
			// taskmanager.cpp:4656
			if (CSharedDoc::Instance()->m_myStatGamesPlayed > 0)
				// taskmanager.cpp:4657
				CSharedDoc::Instance()->m_myStatGamesPlayed--;
		}
		// taskmanager.cpp:4660
		if (CSharedDoc::Instance()->m_myStatTeamHoles > 0)
			// taskmanager.cpp:4661
			CSharedDoc::Instance()->m_myStatTeamHoles--;
		break;
	case 0xb7:
		// taskmanager.cpp:4672
		CSharedDoc::Instance()->m_value4D7E = packet.Decode1();
		// taskmanager.cpp:4674
		if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false) {
			// taskmanager.cpp:4675
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x53, 0, 0, 0, 0, 0);
		}
		break;
	case 0x82:
		{
			// taskmanager.cpp:4684
			unsigned char flags = packet.Decode1();
			// taskmanager.cpp:4685
			unsigned char isGM = flags >> 7;
			// taskmanager.cpp:4686
			unsigned char kind = flags & 0x7F;
			// taskmanager.cpp:4688
			bool isMine = kind == 0;
			// taskmanager.cpp:4691
			unsigned long color = 0xFFD2A2FF;
			if (isGM) {
				// taskmanager.cpp:4693
				color = 0xFF6EECFB;
			}
			// taskmanager.cpp:4698
			std::string nick, message;
			// taskmanager.cpp:4699
			std::string text;
			// taskmanager.cpp:4701
			packet.DecodeStr(nick);
			// taskmanager.cpp:4702
			packet.DecodeStr(message);
			// taskmanager.cpp:4704
			if (isMine) {
				// taskmanager.cpp:4706
				text = MyNick();
				// taskmanager.cpp:4707
				text += ">";
				// taskmanager.cpp:4708
				text += nick;
			} else {
				// taskmanager.cpp:4712
				if (!COption::Instance()->gIsWhisperAllowed()) {
					// taskmanager.cpp:4713
					break;
				}
				// taskmanager.cpp:4715
				std::list<std::string>::iterator it = std::find(CSharedDoc::Instance()->m_ignoreList.begin(), CSharedDoc::Instance()->m_ignoreList.end(), nick);
				// taskmanager.cpp:4716
				if (it != CSharedDoc::Instance()->m_ignoreList.end()) {
					// taskmanager.cpp:4717
					break;
				}
				// taskmanager.cpp:4719
				text = nick + ">" + MyNick();
			}
			// taskmanager.cpp:4722
			CSharedDoc::Instance()->m_chatManager.FilteringHack(message.c_str(), false);
			// taskmanager.cpp:4724
			text += " : \x08'";
			// taskmanager.cpp:4725
			text += message;
			// taskmanager.cpp:4726
			text += "'";
			// taskmanager.cpp:4728
			if (AfxGetTask()->m_bRecieveWhisper) {
				// taskmanager.cpp:4730
				CChatMsg::Instance()->AddChatMsg(text, color, false, true);
			} else {
				// taskmanager.cpp:4735
				WSendPacket send((enumClientPacket)0xE0);
				// taskmanager.cpp:4736
				send.EncodeStr(nick);
				// taskmanager.cpp:4737
				send.Send(SEND_TO_DEFAULT);
			}
		}
		// taskmanager.cpp:4739
		break;
	case 0xb9:
		// taskmanager.cpp:4745
		CSharedDoc::Instance()->m_value4D74 |= packet.Decode4();
		// taskmanager.cpp:4747
		if (CSharedDoc::Instance()->m_value4D74 & 1) {
			// taskmanager.cpp:4749
			std::string msg("\xbe\xcb\xb8\xb2 : \xb8\xf0\xb5\xe7 \xc7\xc3\xb7\xb9\xc0\xcc\xbe\xee\xb0\xa1 \xc0\xcc\xba\xa5\xc6\xae \xc0\xc7\xbb\xf3\xc0\xbb \xc0\xd4\xb0\xed \xc0\xd6\xc0\xb8\xb9\xc7\xb7\xce, 2\xb9\xe8\xc0\xc7 \xc5\xac\xb8\xae\xbe\xee \xba\xb8\xb3\xca\xbd\xba\xb0\xa1 \xc0\xfb\xbf\xeb\xb5\xcb\xb4\xcf\xb4\xd9.");
			// taskmanager.cpp:4750
			CChatMsg::Instance()->AddChatMsg(msg, 0xFFFF7878, false, false);
		}
		break;
	case 0x87:
		// taskmanager.cpp:4766
		{
			unsigned long result = packet.Decode4();
			const char* msg = NULL;
			// taskmanager.cpp:4770
			if (result == 0) {
				// taskmanager.cpp:4773
				msg = "\xc0\xaf\xc0\xfa\xc1\xa4\xba\xb8 \xc1\xb6\xc8\xb8\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.";
			}
			// taskmanager.cpp:4775
			else if (result == 2) {
				// taskmanager.cpp:4778
				msg = "\xbf\xe4\xc3\xbb\xc7\xd1 \xc0\xaf\xc0\xfa\xc0\xc7 \xc1\xa4\xba\xb8\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4";
			}
			// taskmanager.cpp:4780
			else if (result == 3) {
				// taskmanager.cpp:4783
				if (CUserInfo::Instance()->IsVisible())
					// taskmanager.cpp:4784
					CUserInfo::Instance()->Close();
				break;
			}
			if (msg != NULL) {
				// taskmanager.cpp:4790
				if (CUserInfo::Instance()->IsVisible())
					// taskmanager.cpp:4791
					CUserInfo::Instance()->Close();
				// taskmanager.cpp:4793
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)msg, 0, 0, 0, 0));
				// taskmanager.cpp:4794
				break;
			}
			// taskmanager.cpp:4797
			unsigned char season = packet.Decode1();
			// taskmanager.cpp:4798
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:4802
			std::map<unsigned long, sUserInfoTime>::iterator it;
			// taskmanager.cpp:4803
			if (!CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it))
				break;
			// taskmanager.cpp:4805
			it->second.m_time = g_CurrentTime;
			// taskmanager.cpp:4807
			if (!CUserInfo::Instance()->IsVisible())
				break;
			// taskmanager.cpp:4809
			if (!CUserInfo::Instance()->IsRecvedData(season))
				break;
			// taskmanager.cpp:4812
			if (MyUID() != uid)
				// taskmanager.cpp:4819
				CUserInfo::Instance()->SetInfo(uid, 0, true, false, false, false, it->second.m_userInfo.m_name);
			else
				// taskmanager.cpp:4823
				CUserInfo::Instance()->SetInfo(MyUID(), MyGuid(false), true, false, false, false, it->second.m_userInfo.m_name);
			// taskmanager.cpp:4825
			CUserInfo::Instance()->Refresh();
		}
		break;
	case 0x8b:
		// taskmanager.cpp:4834
		{
			unsigned long elapsed = packet.Decode4();
			// taskmanager.cpp:4835
			CSharedDoc::Instance()->m_gameStartTime = g_CurrentTime - elapsed;
		}
		break;
	case 0x91:
		// taskmanager.cpp:4841
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		{
			// taskmanager.cpp:4843
			int result = packet.Decode1();
			// taskmanager.cpp:4845
			switch (result) {
			case 4:
				// taskmanager.cpp:4848
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:4849
				break;
			case 1:
				// taskmanager.cpp:4852
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xb0\xed\xbf\xeb\xb1\xe2\xb0\xa3\xc0\xcc \xbe\xc6\xc1\xf7 \xb3\xb2\xbe\xc6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:4853
				break;
			case 2:
				{
					// taskmanager.cpp:4857
					unsigned int caddieId = packet.Decode4();
					// taskmanager.cpp:4858
					__int64 pang = packet.Decode8();
					// taskmanager.cpp:4860
					std::map<unsigned int, sCaddieInfo>::iterator it = CSharedDoc::Instance()->m_caddieMap.find(caddieId);
					// taskmanager.cpp:4862
					if (it != CSharedDoc::Instance()->m_caddieMap.end())
						// taskmanager.cpp:4863
						it->second.days = 30;
					// taskmanager.cpp:4868
					if (CSharedDoc::Instance()->m_myPang > pang)
						// taskmanager.cpp:4869
						CSharedDoc::Instance()->m_myPang = pang;
					// taskmanager.cpp:4871
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc4\xb3\xb5\xf0\xb8\xa6 \\c0xffff0000\\c\xc7\xd1\xb4\xde\xb0\xa3 \\c0xff000000\\c\xc0\xe7\xb0\xed\xbf\xeb \xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				}
				// taskmanager.cpp:4873
				break;
			case 0:
			case 3:
			case 5:
			case 6:
				// taskmanager.cpp:4879
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\\c0xffff0000\\c\xc4\xb3\xb5\xf0 \xc0\xe7\xb0\xed\xbf\xeb\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			}
			// taskmanager.cpp:4883
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x5C, result, 0, 0, 0, 0));
		}
		break;
	case 0x9b:
		// taskmanager.cpp:4891
				if (!GalleryMode()) {
			// taskmanager.cpp:4892
			break;
		}
		// taskmanager.cpp:4894
		packet.DecodeBuffer(&CSharedDoc::Instance()->m_roomPlayers[0], sizeof(sUserInfo));
		// taskmanager.cpp:4895
		CSharedDoc::Instance()->m_userInfo.galleryGuid = CSharedDoc::Instance()->m_roomPlayers[0].m_guid;
		// taskmanager.cpp:4898
		if (IsLocalContent((localContentType_t)0x57)) {
			// taskmanager.cpp:4900
			RequestUccInfo(CSharedDoc::Instance()->m_roomPlayers[0]);
		}
		break;
	case 0xa5:
		// taskmanager.cpp:4910
		{
			unsigned char itemCount = packet.Decode1();
			std::list<sItemInfo>::iterator it;
			std::map<unsigned int, sCharacterInfo>::iterator ch;
			std::map<unsigned int, sItemInfo>::iterator itemIt;
			// taskmanager.cpp:4915
			for (unsigned char i = 0; i < itemCount; i++) {
				// taskmanager.cpp:4917
				unsigned long typeId = packet.Decode4();
				// taskmanager.cpp:4918
				unsigned long serial = packet.Decode4();
				// taskmanager.cpp:4919
				unsigned short count = packet.Decode2();
				// taskmanager.cpp:4921
				switch (typeId >> 26) {
				case 6:
					{
						// Running out of one of these does not remove it from the list.
						unsigned long keptTypeIds[8];
						// taskmanager.cpp:4935
						keptTypeIds[0] = 0x1A000020;
						// taskmanager.cpp:4936
						keptTypeIds[1] = 0x1A000021;
						// taskmanager.cpp:4937
						keptTypeIds[2] = 0x1A000022;
						// taskmanager.cpp:4938
						keptTypeIds[3] = 0x1A000023;
						// taskmanager.cpp:4939
						keptTypeIds[4] = 0x1A000024;
						// taskmanager.cpp:4940
						keptTypeIds[5] = 0x1A000025;
						// taskmanager.cpp:4941
						keptTypeIds[6] = 0x1A000026;
						// taskmanager.cpp:4943
						keptTypeIds[7] = 0x1A000027;
						// taskmanager.cpp:4948
						bool removable = true;
						// taskmanager.cpp:4950
						for (int k = 0; k < 8; k++) {
							// taskmanager.cpp:4951
							if (keptTypeIds[k] == typeId) {
								// taskmanager.cpp:4952
								removable = false;
								break;
							}
						}
						// taskmanager.cpp:4957
						for (it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {
							// taskmanager.cpp:4990
							if (it->m_serial == serial) {
								// taskmanager.cpp:4992
								it->m_count = count;
								// taskmanager.cpp:4993
								if (count < 1 && removable) {
									// taskmanager.cpp:4994
									CSharedDoc::Instance()->m_itemInfoList.erase(it);
								}
								// taskmanager.cpp:5000
								break;
							}
						}
					}
					break;
				case 5:
					// taskmanager.cpp:5004
					{
						for (it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); ++it) {
							// taskmanager.cpp:5006
							if (it->m_serial == serial) {
								// taskmanager.cpp:5008
								it->m_count = count;
								// taskmanager.cpp:5009
								if (count < 1) {
									// taskmanager.cpp:5011
									CSharedDoc::Instance()->m_myGearList.erase(it);
									// taskmanager.cpp:5012
									CSharedDoc::Instance()->m_myBallId = 0x14000000;
								}
								// taskmanager.cpp:5019
								break;
							}
						}
					}
					break;
				case 29:
					// taskmanager.cpp:5023
					{
						for (it = CSharedDoc::Instance()->m_myExtraItemList4.begin(); it != CSharedDoc::Instance()->m_myExtraItemList4.end(); ++it) {
							// taskmanager.cpp:5026
							if (it->m_serial == serial) {
								// taskmanager.cpp:5028
								it->m_count = count;
								break;
							}
						}
					}
					// taskmanager.cpp:5033
					if (count == 0 && ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CRealMyRoomTask::m_RTTI) : false)) {
						// taskmanager.cpp:5034
						AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x67, 0, 0, 0, 0, 0);
					}
					// taskmanager.cpp:5036
					break;
				case 28:
					// taskmanager.cpp:5042
					{
						for (it = CSharedDoc::Instance()->m_myExtraItemList3.begin(); it != CSharedDoc::Instance()->m_myExtraItemList3.end(); ++it) {
							// taskmanager.cpp:5044
							if (it->m_serial == serial) {
								// taskmanager.cpp:5046
								it->m_count = count;
								// taskmanager.cpp:5049
								if (count < 1) {
									// taskmanager.cpp:5053
									for (ch = CSharedDoc::Instance()->m_characterMap.begin(); ch != CSharedDoc::Instance()->m_characterMap.end(); ++ch) {
										// taskmanager.cpp:5056
										if (((typeId >> 21) & 0x1F) < 5 && ch->second.auxParts.tids[(typeId >> 21) & 0x1F] == typeId) {
											// taskmanager.cpp:5058
											ch->second.auxParts.tids[(typeId >> 21) & 0x1F] = 0;
										}
									}
									// taskmanager.cpp:5061
									CSharedDoc::Instance()->m_myExtraItemList3.erase(it);
								}
								// taskmanager.cpp:5067
								break;
							}
						}
					}
					break;
				case 2:
					// taskmanager.cpp:5071
					{
						for (it = CSharedDoc::Instance()->m_myPartsList.begin(); it != CSharedDoc::Instance()->m_myPartsList.end(); ++it) {
							// taskmanager.cpp:5073
							if (it->m_serial == serial) {
								// taskmanager.cpp:5084
								CSharedDoc::Instance()->m_myPartsList.erase(it);
								// taskmanager.cpp:5089
								break;
							}
						}
					}
					break;
				case 7:
					// taskmanager.cpp:5094
					{
						std::map<unsigned int, sCaddieInfo>::iterator it = CSharedDoc::Instance()->m_caddieMap.find(serial);
						// taskmanager.cpp:5095
						if (it != CSharedDoc::Instance()->m_caddieMap.end()) {
							// taskmanager.cpp:5097
							CSharedDoc::Instance()->m_caddieMap.erase(it);
						}
					}
					// taskmanager.cpp:5100
					break;
				case 4:
					// taskmanager.cpp:5128
					{
						for (itemIt = CSharedDoc::Instance()->m_myItemMap.begin(); itemIt != CSharedDoc::Instance()->m_myItemMap.end(); ++itemIt) {
							// taskmanager.cpp:5130
							if (itemIt->second.m_serial == serial && itemIt->second.m_typeId == typeId) {
								// taskmanager.cpp:5132
								CSharedDoc::Instance()->m_myItemMap.erase(itemIt);
								// taskmanager.cpp:5137
								break;
							}
						}
					}
					break;
				case 31:
					// taskmanager.cpp:5141
					{
						std::list<sCards>::iterator cardIt = CSharedDoc::Instance()->m_myCardList.begin();
						// taskmanager.cpp:5143
						for (; cardIt != CSharedDoc::Instance()->m_myCardList.end(); ++cardIt) {
							// taskmanager.cpp:5145
							if (cardIt->m_guid == serial && cardIt->m_typeId == typeId) {
								// taskmanager.cpp:5147
								if (count < 1) {
									// taskmanager.cpp:5149
									CSharedDoc::Instance()->m_myCardList.erase(cardIt);
								} else {
									// taskmanager.cpp:5153
									cardIt->m_count = count;
								}
								// taskmanager.cpp:5151
								break;
							}
						}
					}
					break;
				}
			}
			// taskmanager.cpp:5165
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xDD, 0, 0, 0, 0, 0));
		}
		break;
	case 0xa7:
		// taskmanager.cpp:5173
		{
			unsigned long infoType = packet.Decode4();
			// taskmanager.cpp:5174
			unsigned long value = packet.Decode4();
			char text[256];
			// taskmanager.cpp:5177
			switch (infoType) {
			case 0:
				// taskmanager.cpp:5180
				sprintf(text, "< SYSTEM INFO > \xc7\xf6\xc0\xe7 \xba\xf1\xb0\xa1 \xbf\xc3 \xc8\xae\xb7\xfc\xc0\xba 1/%u\xc0\xd4\xb4\xcf\xb4\xd9.", value);
				// taskmanager.cpp:5181
				break;
			case 1:
				// taskmanager.cpp:5184
				sprintf(text, "< SYSTEM INFO > \xc7\xf6\xc0\xe7 \xc6\xce\xba\xb8\xb3\xca\xbd\xba\xb4\xc2 %u%%\xc0\xd4\xb4\xcf\xb4\xd9.", value);
				// taskmanager.cpp:5185
				break;
			case 2:
				// taskmanager.cpp:5188
				sprintf(text, "< SYSTEM INFO > \xc7\xf6\xc0\xe7 \xb5\xe5\xb6\xf8\xc8\xae\xb7\xfc\xc0\xba 1/%u\xc0\xd4\xb4\xcf\xb4\xd9.", value);
				// taskmanager.cpp:5189
				break;
			case 3:
				// taskmanager.cpp:5192
				sprintf(text, "< SYSTEM INFO > \xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xb5\xe5\xb7\xd3 \xc8\xae\xb7\xfc\xc0\xba %u%%\xc0\xd4\xb4\xcf\xb4\xd9.", value);
				break;
			}
			// taskmanager.cpp:5196
			if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false) {
				// taskmanager.cpp:5197
				GetActor("Lobby") << MsgObject(0, 3, (intptr_t)text, 0xFFFF7878, 0, 0, 0);
			} else {
				// taskmanager.cpp:5199
				CChatMsg::Instance()->AddChatMsg(text, 0xFFFF7878, false, false);
			}
		}
		break;
	case 0xc6:
		// taskmanager.cpp:5207
		if (IsLocalContent((localContentType_t)0x87)) {
			// taskmanager.cpp:5209
			__int64 pang = packet.Decode8();
			// taskmanager.cpp:5210
			__int64 calc = packet.Decode8();
			// taskmanager.cpp:5212
			if (calc != 0) {
				// taskmanager.cpp:5214
				if (CSharedDoc::Instance()->m_myPang == pang + calc) {
					// taskmanager.cpp:5216
					CSharedDoc::Instance()->m_myPang = pang;
				} else {
					// taskmanager.cpp:5220
					WSendPacket report((enumClientPacket)0x33);
					// taskmanager.cpp:5222
					std::string text;
					char buf[128];
					// taskmanager.cpp:5224
					sprintf(buf, "[Pang Hacker] Old Pangya %I64d, Recive  %I64d , Calculation %I64d", CSharedDoc::Instance()->m_myPang, pang, calc);
					// taskmanager.cpp:5225
					text = buf;
					// taskmanager.cpp:5227
					report.Encode1(3);
					// taskmanager.cpp:5228
					report.EncodeStr(text);
					// taskmanager.cpp:5229
					report.Send(SEND_TO_DEFAULT);
				}
			} else {
				// taskmanager.cpp:5236
				CSharedDoc::Instance()->m_myPang = pang;
			}
		} else {
			// taskmanager.cpp:5241
			__int64 pang = packet.Decode8();
			// taskmanager.cpp:5245
			CSharedDoc::Instance()->m_myPang = pang;
		}
		break;
	case 0xa8:
		// The items of a purchase (after the shop's 0x1D): u16 n, n x
		// sBuyItemResult. The shop and the my-room hand each item to their
		// actor; any other task adds it to CSharedDoc itself.
		{
			// taskmanager.cpp:5265
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
			// taskmanager.cpp:5267
			unsigned short count = packet.Decode2();
			int msg;
			// taskmanager.cpp:5269
			for (unsigned short i = 0; i < count; i++) {
				// taskmanager.cpp:5271
				sBuyItemResult result;
				// taskmanager.cpp:5272
				packet.DecodeBuffer(&result, sizeof(result));
				// taskmanager.cpp:5273
				if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CShopTask::m_RTTI) : false) {
					// taskmanager.cpp:5276
					if (IsLocalContent((localContentType_t)0x14) && result.typeId == 0x1A000042) {
						// taskmanager.cpp:5283
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.m_serial = result.guid;
						item.m_stateBits.kind = result.flag;
						// taskmanager.cpp:5284
						item.m_typeId = 0x1A000042;
						item.m_remainHours = result.period;
						item.m_count = result.count < 1 ? 1 : result.count;
						// taskmanager.cpp:5289
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {
							// taskmanager.cpp:5292
							if (it->m_serial == result.guid) {
								// taskmanager.cpp:5294
								it->m_count = item.m_count;
								break;
							}
						}
						// taskmanager.cpp:5299
						if (it == CSharedDoc::Instance()->m_itemInfoList.end()) {
							// taskmanager.cpp:5304
							item.m_clubStatLevel[1] = packet.Decode2();
							// taskmanager.cpp:5305
							item.m_clubStatLevel[2] = packet.Decode2();
							// taskmanager.cpp:5310
							_SYSTEMTIME endDate;
							packet.DecodeBuffer(&endDate, sizeof(endDate));
							// taskmanager.cpp:5311
							memcpy(item.m_reserved18, &endDate, sizeof(endDate));
							// taskmanager.cpp:5312
							item.m_stateBits.kind = 2;
							// taskmanager.cpp:5314
							CSharedDoc::Instance()->m_itemInfoList.push_back(item);
						}
					} else {
						int shopMsg;
						// taskmanager.cpp:5326
						switch (result.typeId >> 26) {
						case 1: shopMsg = 0xB5; break;    // taskmanager.cpp:5328
						case 2: shopMsg = 0xB6; break;    // taskmanager.cpp:5329
						case 7: shopMsg = 0xB7; break;    // taskmanager.cpp:5330
						case 8: shopMsg = 0xB8; break;    // taskmanager.cpp:5331
						case 6: shopMsg = 0xB9; break;    // taskmanager.cpp:5332
						case 4: shopMsg = 0xBA; break;    // taskmanager.cpp:5333
						case 5: shopMsg = 0xBB; break;    // taskmanager.cpp:5334
						case 0xe: shopMsg = 0xBC; break;  // taskmanager.cpp:5335
						case 0xf: shopMsg = 0xBD; break;  // taskmanager.cpp:5336
						case 0x1c: shopMsg = 0xBE; break; // taskmanager.cpp:5337
						case 0x10: shopMsg = 0xBF; break; // taskmanager.cpp:5338
						case 0x1f: shopMsg = 0x218; break; // taskmanager.cpp:5339
						case 0x12: shopMsg = 0x217; break; // taskmanager.cpp:5340
						default: continue;
						}
						// taskmanager.cpp:5345
						AfxGetTask()->GetActor("Shop") << MsgObject(0, shopMsg, result.guid, result.typeId, result.period, result.flag, (intptr_t)&result);
					}
				// taskmanager.cpp:5348
				} else if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CRealMyRoomTask::m_RTTI) : false) {
					// taskmanager.cpp:5351
					if (IsLocalContent((localContentType_t)0x14) && result.typeId == 0x1A000042) {
						// taskmanager.cpp:5355
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.m_serial = result.guid;
						// taskmanager.cpp:5357
						item.m_stateBits.kind = result.flag;
						// taskmanager.cpp:5358
						item.m_typeId = 0x1A000042;
						item.m_remainHours = result.period;
						// taskmanager.cpp:5359
						item.m_count = result.count < 1 ? 1 : result.count;
						// taskmanager.cpp:5364
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {
							// taskmanager.cpp:5367
							if (it->m_serial == result.guid) {
								// taskmanager.cpp:5369
								it->m_count = item.m_count;
								break;
							}
						}
						// taskmanager.cpp:5374
						if (it == CSharedDoc::Instance()->m_itemInfoList.end()) {
							// taskmanager.cpp:5379
							item.m_clubStatLevel[1] = packet.Decode2();
							// taskmanager.cpp:5380
							item.m_clubStatLevel[2] = packet.Decode2();
							// taskmanager.cpp:5385
							_SYSTEMTIME endDate;
							packet.DecodeBuffer(&endDate, sizeof(endDate));
							// taskmanager.cpp:5386
							memcpy(item.m_reserved18, &endDate, sizeof(endDate));
							// taskmanager.cpp:5387
							item.m_stateBits.kind = 2;
							// taskmanager.cpp:5389
							CSharedDoc::Instance()->m_itemInfoList.push_back(item);
						}
					} else {
						// taskmanager.cpp:5396
						switch (result.typeId >> 26) {
						case 1: msg = 0xC8; break;    // taskmanager.cpp:5398
						case 2: msg = 0xCA; break;    // taskmanager.cpp:5399
						case 7: msg = 0xCB; break;    // taskmanager.cpp:5400
						case 8: msg = 0xCC; break;    // taskmanager.cpp:5401
						case 4: msg = 0xCF; break;    // taskmanager.cpp:5403
						case 0xe: msg = 0xD0; break;  // taskmanager.cpp:5405
						case 5:
						case 6:
						case 0x1c:
						case 0x1d: msg = 0xCE; break; // taskmanager.cpp:5407
						case 0xf: msg = 0xC9; break;  // taskmanager.cpp:5408
						case 0x10: msg = 0xCD; break; // taskmanager.cpp:5409
						case 0x1f: msg = 0xE0; break; // taskmanager.cpp:5410
						}
						// taskmanager.cpp:5417
						if (IsLocalContent((localContentType_t)0x56)) {
							// taskmanager.cpp:5419
							AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, msg, result.guid, result.typeId, result.period, result.flag, (intptr_t)&result);
							// taskmanager.cpp:5421
							if (IsLocalContent((localContentType_t)0x60)) {
								// taskmanager.cpp:5423
								if (msg != 0xC9) {
									// taskmanager.cpp:5424
									AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);
								}
							}
						} else {
							// taskmanager.cpp:5429
							AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, msg, result.guid, result.typeId, result.period, result.flag, (intptr_t)&result);
						}
					}
				} else {
					// taskmanager.cpp:5435
					switch (result.typeId >> 26) {
					case 1: {
						// taskmanager.cpp:5440
						sCharacterInfo character;
						memset(&character, 0, sizeof(character));
						character.id = result.guid;
						character.typeId = result.typeId;
						// taskmanager.cpp:5443
						ItemManager()->GetDefCombo(result.typeId, character.partTypeIds);
						// taskmanager.cpp:5445
						CSharedDoc::Instance()->m_characterMap.insert(std::map<unsigned int, sCharacterInfo>::value_type(character.id, character));
						// taskmanager.cpp:5447
						break;
					}
					case 2: {
						// taskmanager.cpp:5458
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.m_serial = result.guid;
						item.m_typeId = result.typeId;
						// taskmanager.cpp:5462
						if (result.flag == 2) {
							// taskmanager.cpp:5464
							item.m_remainHours = result.period * 24;
							// taskmanager.cpp:5465
							item.m_stateBits.kind = 2;
						}
						// taskmanager.cpp:5467
						CSharedDoc::Instance()->AddMyPartsList(item);
						// taskmanager.cpp:5469
						break;
					}
					case 7: {
						// taskmanager.cpp:5475
						sCaddieInfo caddie;
						memset(&caddie, 0, sizeof(caddie));
						caddie.id = result.guid;
						caddie.typeId = result.typeId;
						// taskmanager.cpp:5477
						caddie.flags |= 2;
						caddie.days = 30;
						// taskmanager.cpp:5480
						if (caddie.typeId >= 0x1C00000C && caddie.typeId <= 0x1C00000E) {
							// taskmanager.cpp:5486
							caddie.level = result.flag;
							caddie.experience = result.period;
						}
						// taskmanager.cpp:5490
						CSharedDoc::Instance()->m_caddieMap.insert(std::map<unsigned int, sCaddieInfo>::value_type(caddie.id, caddie));
						// taskmanager.cpp:5492
						break;
					}
					case 6: {
						// taskmanager.cpp:5497
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						// taskmanager.cpp:5498
						item.m_serial = result.guid;
						// taskmanager.cpp:5500
						item.m_stateBits.kind = result.flag;
						// taskmanager.cpp:5501
						item.m_typeId = result.typeId;
						item.m_count = result.count < 1 ? 1 : result.count;
						// taskmanager.cpp:5507
						if (result.flag) {
							// taskmanager.cpp:5509
							if (result.flag == 3) {
								item.m_remainHours = result.period;
							// taskmanager.cpp:5510
							} else if (result.flag == 4) {
								item.m_remainHours = result.period * 24;
							// taskmanager.cpp:5511
							} else if (result.flag == 5) {
								item.m_remainHours = result.period * 720;
							}
						}
						// taskmanager.cpp:5519
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {
							// taskmanager.cpp:5522
							if (it->m_serial == result.guid) {
								// taskmanager.cpp:5524
								it->m_count = item.m_count;
								break;
							}
						}
						// taskmanager.cpp:5529
						if (it == CSharedDoc::Instance()->m_itemInfoList.end()) {
							// taskmanager.cpp:5532
							if (IsLocalContent((localContentType_t)0x14)) {
								// taskmanager.cpp:5534
								if (item.m_typeId == 0x1A000042) {
									// taskmanager.cpp:5536
									item.m_clubStatLevel[1] = packet.Decode2();
									// taskmanager.cpp:5537
									item.m_clubStatLevel[2] = packet.Decode2();
									// taskmanager.cpp:5539
									item.m_remainHours = result.period;
									// taskmanager.cpp:5544
									_SYSTEMTIME endDate;
									packet.DecodeBuffer(&endDate, sizeof(endDate));
									// taskmanager.cpp:5545
									memcpy(item.m_reserved18, &endDate, sizeof(endDate));
									// taskmanager.cpp:5546
									item.m_stateBits.kind = 2;
								}
							}
							// taskmanager.cpp:5549
							CSharedDoc::Instance()->m_itemInfoList.push_back(item);
						}
						// taskmanager.cpp:5554
						break;
					}
					case 5: {
						// taskmanager.cpp:5562
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); ++it) {
							// taskmanager.cpp:5566
							if (it->m_serial == result.guid) {
								// taskmanager.cpp:5568
								it->m_count = result.count < 1 ? 1 : result.count;
								break;
							}
						}
						// taskmanager.cpp:5573
						if (it == CSharedDoc::Instance()->m_myGearList.end()) {
							// taskmanager.cpp:5576
							sItemInfo item;
							memset(&item, 0, sizeof(item));
							// taskmanager.cpp:5577
							item.m_serial = result.guid;
							// taskmanager.cpp:5579
							item.m_stateBits.kind = result.flag;
							// taskmanager.cpp:5580
							item.m_typeId = result.typeId;
							item.m_count = result.count < 1 ? 1 : result.count;
							// taskmanager.cpp:5582
							CSharedDoc::Instance()->m_myGearList.push_back(item);
						}
						// taskmanager.cpp:5585
						break;
					}
					case 4: {
						// taskmanager.cpp:5589
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						// taskmanager.cpp:5590
						item.m_serial = result.guid;
						item.m_typeId = result.typeId;
						// taskmanager.cpp:5593
						CSharedDoc::Instance()->m_myItemMap.insert(std::map<unsigned int, sItemInfo>::value_type(item.m_serial, item));
						// taskmanager.cpp:5595
						break;
					}
					case 0x1d: {
						// taskmanager.cpp:5598
						IFF_STRUCT::sQuestDrop *questDrop = ItemManager()->FindQuestDrop(result.typeId);
						// taskmanager.cpp:5601
						if (!questDrop)
							break;
						// taskmanager.cpp:5608
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myExtraItemList4.begin(); it != CSharedDoc::Instance()->m_myExtraItemList4.end(); ++it) {
							// taskmanager.cpp:5611
							if (it->m_serial == result.guid) {
								// taskmanager.cpp:5613
								it->m_count++;
								break;
							}
						}
						// taskmanager.cpp:5618
						if (it == CSharedDoc::Instance()->m_myExtraItemList4.end()) {
							// taskmanager.cpp:5622
							sItemInfo item;
							memset(&item, 0, sizeof(item));
							item.m_serial = result.guid;
							// taskmanager.cpp:5623
							item.m_typeId = result.typeId;
							item.m_count = 1;
							// taskmanager.cpp:5626
							CSharedDoc::Instance()->m_myExtraItemList4.push_back(item);
						}
						// taskmanager.cpp:5629
						break;
					}
					case 0x10: {
						// taskmanager.cpp:5635
						sMascotInfo mascot;
						memset(&mascot, 0, sizeof(mascot));
						mascot.id = result.guid;
						mascot.typeId = result.typeId;
						// taskmanager.cpp:5637
						mascot.endTime = result.endDate;
						// taskmanager.cpp:5638
						mascot.remainHours = result.period;
						strcpy(mascot.message, "PANGYA!");
						// taskmanager.cpp:5641
						CSharedDoc::Instance()->AddMascot(&mascot);
						// taskmanager.cpp:5643
						break;
					}
					case 0x1c: {
						// taskmanager.cpp:5649
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.m_serial = result.guid;
						// taskmanager.cpp:5651
						item.m_stateBits.kind = result.flag;
						// taskmanager.cpp:5652
						item.m_typeId = result.typeId;
						item.m_count = result.count < 1 ? 1 : result.count;
						// taskmanager.cpp:5654
						if (result.flag) {
							// taskmanager.cpp:5656
							if (result.flag == 3) {
								item.m_remainHours = result.period;
							// taskmanager.cpp:5657
							} else if (result.flag == 4) {
								item.m_remainHours = result.period * 24;
							// taskmanager.cpp:5658
							} else if (result.flag == 5) {
								item.m_remainHours = result.period * 720;
							}
						}
						// taskmanager.cpp:5664
						std::list<sItemInfo>::iterator it;
						for (it = CSharedDoc::Instance()->m_myExtraItemList3.begin(); it != CSharedDoc::Instance()->m_myExtraItemList3.end(); ++it) {
							// taskmanager.cpp:5667
							if (it->m_serial == result.guid) {
								// taskmanager.cpp:5669
								it->m_count = item.m_count;
								break;
							}
						}
						// taskmanager.cpp:5674
						if (it == CSharedDoc::Instance()->m_myExtraItemList3.end()) {
							// taskmanager.cpp:5676
							CSharedDoc::Instance()->m_myExtraItemList3.push_back(item);
						}
						// taskmanager.cpp:5679
						break;
					}
					case 0x1f: {
						// taskmanager.cpp:5688
						unsigned long guid = result.guid;
						unsigned long typeId = result.typeId;
						int cardCount;
						if (result.count > 0) {
							// taskmanager.cpp:5690
							cardCount = result.count;
						} else {
							// taskmanager.cpp:5694
							cardCount = 1;
						}
						// taskmanager.cpp:5697
						IFF_STRUCT::sCard *card = ItemManager()->FindCard(typeId);
						// taskmanager.cpp:5698
						if (!card)
							break;
						// taskmanager.cpp:5714
						sCards cards;
						memset(&cards, 0, sizeof(cards));
						// taskmanager.cpp:5718
						cards.m_count = cardCount;
						cards.m_guid = guid;
						cards.m_typeId = typeId;
						// taskmanager.cpp:5721
						bool found = false;
						for (std::list<sCards>::iterator it = CSharedDoc::Instance()->m_myCardList.begin(); it != CSharedDoc::Instance()->m_myCardList.end(); ++it) {
							// taskmanager.cpp:5724
							if (it->m_typeId == typeId) {
								// taskmanager.cpp:5726
								it->m_count += cardCount;
								found = true;
								break;
							}
						}
						// taskmanager.cpp:5732
						if (!found)
							CSharedDoc::Instance()->m_myCardList.push_back(cards);
						// taskmanager.cpp:5735
						break;
					}
					case 0xe: {
						// taskmanager.cpp:5740
						sItemInfo item;
						memset(&item, 0, sizeof(item));
						item.m_serial = result.guid;
						// taskmanager.cpp:5742
						item.m_stateBits.kind = result.flag;
						// taskmanager.cpp:5744
						item.m_typeId = result.typeId;
						item.m_remainHours = result.period * 24;
						CSharedDoc::Instance()->m_mySkinList.push_back(item);
						break;
					}
					case 0xf:
						break;
					default:
						continue;
					}
				}
				// taskmanager.cpp:5754
				if (IsAngelWing(result.typeId)) {
					// taskmanager.cpp:5756
					CSharedDoc::Instance()->CheckAngelWing();
				}
				// taskmanager.cpp:5759
				if (IsLocalContent((localContentType_t)0x17) && (result.typeId & 0xFC000000) == 0x8000000 && (result.typeId & 0x1800) == 0x1800) {
					// taskmanager.cpp:5762
					CSharedDoc::Instance()->SetInitHaveHalloweenItem();
				}
			}
		}
		break;
	case 0x93:
		// taskmanager.cpp:5788
		{
			// taskmanager.cpp:5790
			if (packet.Decode2() == 0x111) {
				// taskmanager.cpp:5795
				switch (packet.Decode4()) {
				case 0:
					// taskmanager.cpp:5800
					CSharedDoc::Instance()->m_myPang = packet.Decode8();
					// taskmanager.cpp:5802
					CMessengerInfo::Instance()->Notify1("\xc2\xca\xc1\xf6\xb8\xa6 \xb3\xb2\xb0\xe5\xbd\xc0\xb4\xcf\xb4\xd9. \xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xb4\xd9\xc0\xbd \xb7\xce\xb1\xd7\xc0\xce\xc7\xd2 \xb6\xa7 \xc0\xfc\xb4\xde\xb5\xcb\xb4\xcf\xb4\xd9.");
					break;
				case 3:
					// taskmanager.cpp:5807
					CMessengerInfo::Instance()->Notify1("\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 \xc2\xca\xc1\xf6\xc7\xd4\xc0\xcc \xb0\xa1\xb5\xe6\xc2\xf7\xbc\xad \xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				case 7:
				case 10:
					// taskmanager.cpp:5814
					CMessengerInfo::Instance()->Notify1("\xc0\xda\xbd\xc5\xbf\xa1\xb0\xd4 \xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbe\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				case 12:
					// taskmanager.cpp:5818
					CMessengerInfo::Instance()->Notify1("\xba\xb8\xc0\xaf\xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xcf\xbf\xa9 \xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbe\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				default:
					// taskmanager.cpp:5830
					CMessengerInfo::Instance()->Notify1("\xc2\xca\xc1\xf6 \xba\xb8\xb3\xbb\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
					break;
				}
			}
			// taskmanager.cpp:5838
			WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN);
		}
		break;
	case 0xd3:
		// taskmanager.cpp:6037
		{
			unsigned long value = packet.Decode4();
			// taskmanager.cpp:6039
			if (value) {
				// taskmanager.cpp:6041
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xAD, value, 0, 0, 0, 0));
			}
		}
		break;
	case 0xd4: {
		// taskmanager.cpp:6048
		int result = packet.Decode4();
		// taskmanager.cpp:6051
		FrBongdariShopDlg* pDlg = FrStaticCast<FrBongdariShopDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop"));
		// taskmanager.cpp:6054
		if (pDlg) {
			// taskmanager.cpp:6058
			pDlg->EnableExitButton(true);
		}
		// taskmanager.cpp:6061
		switch (result) {
		case 0: {
			// taskmanager.cpp:6065
			int count = packet.Decode4();
			// taskmanager.cpp:6068
			if (count < 1 || count > 5)
				break;
			// taskmanager.cpp:6071
			sBonusBall balls[5] = { 0 };
			// taskmanager.cpp:6074
			for (int i = 0; i < count; i++) {
				// taskmanager.cpp:6076
				packet.DecodeBuffer(&balls[i], sizeof(sBonusBall));
			}
			// taskmanager.cpp:6080
			__int64 pang = packet.Decode8();
			// taskmanager.cpp:6081
			__int64 bonusCash = packet.Decode8();
			// taskmanager.cpp:6083
			CSharedDoc::Instance()->m_myPang = pang;
			// taskmanager.cpp:6085
			CSharedDoc::Instance()->m_myBonusCash = bonusCash;
			// taskmanager.cpp:6086
			CSharedDoc::Instance()->RefreshAllCookie();
			// taskmanager.cpp:6092
			if (pDlg) {
				// taskmanager.cpp:6096
				pDlg->InitBongdariShop();
				// taskmanager.cpp:6097
				pDlg->SetBonusBall(count, balls);
				// taskmanager.cpp:6098
				pDlg->SetBongdariShop();
				// taskmanager.cpp:6099
				pDlg->SetDataFee(0);
			}
			// taskmanager.cpp:6102
			MsgObject msg(0, 0xE1, 0, 0, 0, 0, 0);
			AfxGetTask()->GetActor("RealMyRoom") << msg;
			break;
		}
		case 2:
			// taskmanager.cpp:6108
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
			break;
		case 3:
			// taskmanager.cpp:6112
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb \xc4\xda\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
			break;
		}
		break;
	}
	case 0xf9:
		// taskmanager.cpp:6135
		CSharedDoc::Instance()->SetBsUsableTimes((short)packet.Decode4());
		// taskmanager.cpp:6136
		CSharedDoc::Instance()->SetBsUsableBonusTimes((short)packet.Decode4());
		{
			// taskmanager.cpp:6139
			FrBongdariShopDlg* pDlg = FrStaticCast<FrBongdariShopDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop"));
			// taskmanager.cpp:6142
			if (pDlg == 0)
				break;
			// taskmanager.cpp:6145
			pDlg->EnableExitButton(true);
			// taskmanager.cpp:6150
			pDlg->ShowDiscountUsableTimes();
			// taskmanager.cpp:6151
			pDlg->UpdateUsableTimes();
		}
		break;
	case 0x2d:
		// taskmanager.cpp:6325
		// taskmanager.cpp:6326
		switch (packet.Decode1()) {
		case 2:
			// taskmanager.cpp:6331
			CMessengerInfo::Instance()->Notify1("\xb8\xde\xbd\xc5\xc0\xfa\xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd1 \xb7\xce\xb1\xd7\xc0\xce\xc1\xa4\xba\xb8\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xee \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
			// taskmanager.cpp:6332
			if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN)) {
				// taskmanager.cpp:6333
				WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_UNIT_MSN);
			}
			break;
		case 0:
			{
				// taskmanager.cpp:6346
				unsigned long uid = packet.Decode4();
				// taskmanager.cpp:6347
				if (uid == MyUID()) {
					// taskmanager.cpp:6353
					CMessengerInfo::Instance()->SetComplete(CONNECT_STATUS_ACCEPTED);
					// taskmanager.cpp:6357
					sUserPosition position;
					// taskmanager.cpp:6358
					position.m_userValue = CSharedDoc::Instance()->m_gameServerUID;
					// taskmanager.cpp:6360
					if (CSharedDoc::Instance()->m_currentChannel.id == 0xFF) {
						// taskmanager.cpp:6361
						position.m_channelId = 0xFF;
					} else {
						// taskmanager.cpp:6363
						position.m_channelId = CSharedDoc::Instance()->m_currentChannel.id;
					}
					// taskmanager.cpp:6365
					wsprintfA(position.m_channelName, "%s", CSharedDoc::Instance()->m_currentChannel.name);
					// taskmanager.cpp:6366
					if (CSharedDoc::Instance()->m_roomInfo.m_roomNumber == 0xFFFF) {
						// taskmanager.cpp:6367
						position.m_roomNumber = 0xFFFF;
					} else {
						// taskmanager.cpp:6369
						position.m_roomNumber = CSharedDoc::Instance()->m_roomInfo.m_roomNumber;
					}
					// taskmanager.cpp:6371
					if (CSharedDoc::Instance()->m_roomInfo.m_gameType == 0xFFFFFFFF) {
						// taskmanager.cpp:6372
						position.m_value2 = 0xFFFFFFFF;
					} else {
						// taskmanager.cpp:6374
						position.m_value2 = CSharedDoc::Instance()->m_roomInfo.m_gameType;
					}
					// taskmanager.cpp:6376
					WSendPacket send(GCR_NOW_POSITION);
					// taskmanager.cpp:6377
					send.EncodeBuffer(&position, sizeof(sUserPosition));
					// taskmanager.cpp:6378
					send.Send(SEND_TO_MSN);
					// taskmanager.cpp:6381
					WSendPacket request(GCR_REQUEST_FRIEND_LIST);
					// taskmanager.cpp:6387
					request.Send(SEND_TO_MSN);
					// taskmanager.cpp:6388
					CMessengerInfo::Instance()->SetComplete(CONNECT_STATUS_POSITION_SENT);
				}
			}
			break;
		}
		break;
	case 0x31:
		// taskmanager.cpp:6397
		if (IsLocalContent((localContentType_t)0x60)) {
			// taskmanager.cpp:6399
			CSharedDoc::Instance()->m_mailList.clear();
			// taskmanager.cpp:6400
			WSendPacket send(GCT_NEW_MAIL_LIST);
			// taskmanager.cpp:6401
			send.Send(SEND_TO_DEFAULT);
		} else {
			// taskmanager.cpp:6405
			CSharedDoc::Instance()->m_giftList.clear();
			// taskmanager.cpp:6406
			WSendPacket send(GCT_NEW_GIFT_LIST);
			// taskmanager.cpp:6407
			send.Send(SEND_TO_DEFAULT);
		}
		break;
	case 0x36:
		{
			// taskmanager.cpp:6414
			WSendPacket send(GCT_NEW_MAIL_LIST);
			// taskmanager.cpp:6415
			send.Send(SEND_TO_DEFAULT);
		}
		break;
	case 0x38:
		{
			// taskmanager.cpp:6422
			int count = packet.Decode4();
			// taskmanager.cpp:6424
			for (int i = 0; i < count; i++) {
				// taskmanager.cpp:6426
				GUILD_USER_MSN_LIST member;
				// taskmanager.cpp:6427
				packet.DecodeBuffer(&member, sizeof(GUILD_USER_MSN_LIST));
				// taskmanager.cpp:6429
				sFriend buddy;
				// taskmanager.cpp:6430
				buddy.value2D = member.value4;
				// taskmanager.cpp:6431
				buddy.uid = member.uid;
				// taskmanager.cpp:6432
				buddy.listFlags |= 2;
				// taskmanager.cpp:6433
				buddy.flags = (unsigned char)((buddy.flags & ~1) | (member.sex & 1));
				// taskmanager.cpp:6434
				strcpy(buddy.nick, member.nick);
				// taskmanager.cpp:6435
				buddy.flags = (unsigned char)((buddy.flags & ~2) | ((member.online & 1) << 1));
				// taskmanager.cpp:6437
				BuddyMGR->AddBuddy(&buddy);
			}
		}
		break;
	case 0x39:
		{
			// taskmanager.cpp:6444
			GUILD_USER_MSN_LIST member;
			// taskmanager.cpp:6445
			packet.DecodeBuffer(&member, sizeof(GUILD_USER_MSN_LIST));
			// taskmanager.cpp:6447
			sFriend buddy;
			// taskmanager.cpp:6451
			buddy.value2D = member.value4;
			buddy.uid = member.uid;
			buddy.listFlags |= 2;
			buddy.flags = (unsigned char)((buddy.flags & ~1) | (member.sex & 1));
			// taskmanager.cpp:6452
			strcpy(buddy.nick, member.nick);
			// taskmanager.cpp:6453
			buddy.flags = (unsigned char)((buddy.flags & ~2) | ((member.online & 1) << 1));
			// taskmanager.cpp:6455
			BuddyMGR->AddBuddy(&buddy);
			// taskmanager.cpp:6459
			IActor *pGuildActor = GetActor(s_guildActorName);
			// taskmanager.cpp:6460
			if (pGuildActor) {
				// taskmanager.cpp:6463
				std::map<unsigned long, sBriefUserInfo>::iterator it;
				if (CSharedDoc::Instance()->FindUserInfoUID(member.uid, it) == 1) {
					// taskmanager.cpp:6465
					sBriefUserInfo &userInfo = it->second;
					// taskmanager.cpp:6469
					GUILD_INFO *pInfo = 0;
					pGuildActor << MsgObject(0, 0x263, 0x21, (intptr_t)&pInfo, 0, 0, 0);
					// taskmanager.cpp:6470
					strncpy(userInfo.guildMark, pInfo->emblem, 0xB);
				}
			}
			// taskmanager.cpp:6475
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)MakeStr("%s \xB4\xD4\xC0\xCC \xB1\xE6\xB5\xE5\xBF\xA1 \xB0\xA1\xC0\xD4\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", member.nick), 0, 0, 0, 0));
		}
		break;
	case 0x3a:
		{
			// taskmanager.cpp:6481
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:6487
			BuddyMGR->DeleteBuddy(uid);
			// taskmanager.cpp:6491
			std::map<unsigned long, sBriefUserInfo>::iterator it;
			if (CSharedDoc::Instance()->FindUserInfoUID(uid, it) == 1) {
				// taskmanager.cpp:6493
				sBriefUserInfo &userInfo = it->second;
				// taskmanager.cpp:6494
				strcpy(userInfo.guildMark, "guildmark");
				// taskmanager.cpp:6495
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)MakeStr("%s \xB4\xD4\xC0\xCC \xB1\xE6\xB5\xE5\xB8\xA6 \xC5\xBB\xC5\xF0\xC7\xCF\xBF\xB4\xBD\xC0\xB4\xCF\xB4\xD9.", userInfo.nick), 0, 0, 0, 0));
			}
		}
		break;
	case 0x3b:
		// taskmanager.cpp:6505
		{
			GUILD_INFO guildInfo;
			// taskmanager.cpp:6506
			packet.DecodeBuffer(&guildInfo, sizeof(GUILD_INFO));
			// taskmanager.cpp:6508
			IActor *pGuildActor = GetActor(s_guildActorName);
			// taskmanager.cpp:6509
			if (pGuildActor) {
				// taskmanager.cpp:6514
				pGuildActor << MsgObject(0, 0x263, 0x16, (intptr_t)&guildInfo, 0, 0, 0);
			}
		}
		break;
	case 0x3c:
		{
			// taskmanager.cpp:6520
			GUILD_INFO guildInfo;
			// taskmanager.cpp:6521
			packet.DecodeBuffer(&guildInfo, sizeof(GUILD_INFO));
			// taskmanager.cpp:6522
			IActor *pGuildActor = GetActor(s_guildActorName);
			// taskmanager.cpp:6523
			if (pGuildActor) {
				// taskmanager.cpp:6528
				pGuildActor << MsgObject(0, 0x263, 0x17, (intptr_t)&guildInfo, 0, 0, 0);
			}
		}
		break;
	case 0x32:
		{
			// taskmanager.cpp:6537
			std::string nick;
			// taskmanager.cpp:6539
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:6540
			nick = packet.DecodeStr();
			// taskmanager.cpp:6542
			sFriend *pBuddy = BuddyMGR->GetBuddy(uid);
			// taskmanager.cpp:6543
			if (pBuddy) {
				// taskmanager.cpp:6545
				strcpy(pBuddy->nick, nick.c_str());
			}
		}
		// taskmanager.cpp:6547
		break;
	case 0x37:
		{
			// taskmanager.cpp:6552
			std::string msg("");
			// taskmanager.cpp:6553
			std::string unknownStr = packet.DecodeStr(); // decoded, never read
			// taskmanager.cpp:6554
			std::string nick = packet.DecodeStr();
			// taskmanager.cpp:6555
			std::string text = packet.DecodeStr();
			// taskmanager.cpp:6567
			std::list<std::string>::iterator it = std::find(CSharedDoc::Instance()->m_ignoreList.begin(), CSharedDoc::Instance()->m_ignoreList.end(), nick);
			// taskmanager.cpp:6568
			if (it != CSharedDoc::Instance()->m_ignoreList.end()) {
				// taskmanager.cpp:6569
				break;
			}
			// taskmanager.cpp:6572
			const char *chatLine;
			GetActor("Lobby") << MsgObject(0, 0x3F, (intptr_t)&chatLine, 0, 0, 0, 0);
			// taskmanager.cpp:6576
			msg = "\xB1\xE6\xB5\xE5:" + nick + "> ";
			// taskmanager.cpp:6579
			CSharedDoc::Instance()->m_chatManager.FilteringHack(text.c_str(), false);
			// taskmanager.cpp:6584
			CChatMsg::Instance()->AddChatMsg(msg, 0xFF5BFFB0, false, true);
		}
		// taskmanager.cpp:6586
		break;
	case 0x2e:
		// taskmanager.cpp:6591
		{
			unsigned short msnType = packet.Decode2();
			// taskmanager.cpp:6592
			switch (msnType) {
			case 0x105:
			case 0x110:
				// taskmanager.cpp:6596
				packet.Decode1();
				// taskmanager.cpp:6598
				break;
			case 0x117:
				{
					// taskmanager.cpp:6625
					int result = packet.Decode4();
					// taskmanager.cpp:6635
					if (result == 0) {
						// taskmanager.cpp:6637
						std::string nick;
						// taskmanager.cpp:6638
						packet.DecodeStr(nick);
						// taskmanager.cpp:6641
						unsigned long uid = packet.Decode4();
						// taskmanager.cpp:6643
						if (((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false)
							|| ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&ntAvatarChatTask::m_RTTI) : false))
							// taskmanager.cpp:6645
							AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x51, (intptr_t)nick.c_str(), uid, 0, 0, 0));
						// taskmanager.cpp:6647
						CMessengerInfo::Instance()->SetOfflineUserNick(nick.c_str(), uid);
					// taskmanager.cpp:6652
					} else if (result == 13) {
						// taskmanager.cpp:6654
						CMessengerInfo::Instance()->Notify1("\xc8\xb8\xbf\xf8\xc5\xbb\xc5\xf0\xc7\xd1 \xbe\xc6\xc0\xcc\xb5\xf0\xc0\xd4\xb4\xcf\xb4\xd9.");
					} else {
						// taskmanager.cpp:6660
						CMessengerInfo::Instance()->Notify1("\xb4\xeb\xc8\xad\xb8\xed\xc0\xcc \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
					}
				}
				break;
			case 0x104:
				{
					// taskmanager.cpp:6675
					unsigned long result = packet.Decode4();
					// taskmanager.cpp:6680
					switch (result) {
					case 0:
						{
							// taskmanager.cpp:6686
							sFriend buddy;
							// taskmanager.cpp:6688
							packet.DecodeBuffer(&buddy, sizeof(sFriend));
							// taskmanager.cpp:6691
							sFriend *pBuddy = BuddyMGR->GetBuddy(buddy.uid);
							// taskmanager.cpp:6693
							if (pBuddy && (pBuddy->listFlags & 2)) {
								// taskmanager.cpp:6695
								pBuddy->listFlags = (pBuddy->listFlags & ~1) | (buddy.listFlags & 1);
								// taskmanager.cpp:6696
								pBuddy->flags = (pBuddy->flags & ~4) | (buddy.flags & 4);
								// taskmanager.cpp:6697
								pBuddy->flags = (pBuddy->flags & ~8) | (buddy.flags & 8);
							// taskmanager.cpp:6699
							} else {
								// taskmanager.cpp:6701
								BuddyMGR->AddBuddy(&buddy);
							}
							// taskmanager.cpp:6704
							CMessengerInfo::Instance()->Notify1("\xc4\xa3\xb1\xb8\xb5\xee\xb7\xcf\xc0\xbb \xbf\xe4\xc3\xbb\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
							// taskmanager.cpp:6705
							CMessengerInfo::Instance()->Refresh();
						}
						// taskmanager.cpp:6707
						break;
					case 11:
						// taskmanager.cpp:6713
						CMessengerInfo::Instance()->Notify1("\xc0\xcc\xb9\xcc \xc4\xa3\xb1\xb8\xb7\xce \xb5\xee\xb7\xcf\xb5\xc7\xbe\xee \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
						// taskmanager.cpp:6715
						break;
					case 2:
						// taskmanager.cpp:6721
						CMessengerInfo::Instance()->Notify1("\xb4\xf5 \xc0\xcc\xbb\xf3 \xc4\xa3\xb1\xb8\xb5\xee\xb7\xcf\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
						// taskmanager.cpp:6723
						break;
					case 3:
						// taskmanager.cpp:6729
						CMessengerInfo::Instance()->Notify1("\xbb\xf3\xb4\xeb\xb9\xe6 \xc4\xa3\xb1\xb8\xbd\xbd\xb7\xd4\xc0\xcc \xb8\xf0\xb5\xce \xc2\xf7 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
						// taskmanager.cpp:6731
						break;
					case 6:
						// taskmanager.cpp:6737
						CMessengerInfo::Instance()->Notify1("\xc0\xda\xb1\xe2 \xc0\xda\xbd\xc5\xc0\xbb \xc4\xa3\xb1\xb8\xb7\xce \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
						// taskmanager.cpp:6739
						break;
					case 7:
						// taskmanager.cpp:6745
						CMessengerInfo::Instance()->Notify1("\xb4\xeb\xc8\xad\xb8\xed\xc0\xcc \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
						// taskmanager.cpp:6747
						break;
					case 8:
						// taskmanager.cpp:6752
						CMessengerInfo::Instance()->Notify1("\xb4\xeb\xc8\xad\xb8\xed\xc0\xcc \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9..");
						// taskmanager.cpp:6754
						break;
					case 4:
						// taskmanager.cpp:6760
						CMessengerInfo::Instance()->Notify1("\xc4\xa3\xb1\xb8 \xc7\xe3\xb0\xa1 \xb4\xeb\xb1\xe2 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.");
						// taskmanager.cpp:6762
						break;
					case 5:
						// taskmanager.cpp:6767
						CMessengerInfo::Instance()->Notify1("\xc4\xa3\xb1\xb8\xc3\xdf\xb0\xa1 \xc8\xc4 \xbd\xc2\xc0\xce \xb0\xc5\xc0\xfd \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
						// taskmanager.cpp:6769
						break;
					default:
						// taskmanager.cpp:6772
						CMessengerInfo::Instance()->Notify1("\xc4\xa3\xb1\xb8 \xb5\xee\xb7\xcf\xc0\xbb \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
						// taskmanager.cpp:6777
						break;
					}
				}
				break;
			case 0x106:
				{
					// taskmanager.cpp:6781
					sFriend buddy;
					// taskmanager.cpp:6783
					packet.DecodeBuffer(&buddy, sizeof(sFriend));
					// taskmanager.cpp:6786
					sFriend *pBuddy = BuddyMGR->GetBuddy(buddy.uid);
					// taskmanager.cpp:6789
					if (pBuddy && (pBuddy->listFlags & 2)) {
						// taskmanager.cpp:6791
						pBuddy->listFlags = (pBuddy->listFlags & ~1) | (buddy.listFlags & 1);
						// taskmanager.cpp:6792
						pBuddy->flags = (pBuddy->flags & ~4) | (buddy.flags & 4);
						// taskmanager.cpp:6793
						pBuddy->flags = (pBuddy->flags & ~8) | (buddy.flags & 8);
					// taskmanager.cpp:6795
					} else {
						// taskmanager.cpp:6797
						BuddyMGR->AddBuddy(&buddy);
					}
					// taskmanager.cpp:6800
					CMessengerInfo::Instance()->Refresh();
					// taskmanager.cpp:6802
					std::string notice("\xbe\xcb\xb8\xb2 : ");
					// taskmanager.cpp:6803
					notice += buddy.nick;
					// taskmanager.cpp:6804
					notice += " \xb4\xd4\xc0\xcc \xc4\xa3\xb1\xb8\xbf\xe4\xc3\xbb\xc0\xbb \xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.";
					// taskmanager.cpp:6806
					CMessengerInfo::Instance()->Notify2(notice.c_str());
				}
				// taskmanager.cpp:6808
				break;
			case 0x113:
				{
					// taskmanager.cpp:6813
					std::string text;
					// taskmanager.cpp:6822
					std::string nick;
					// taskmanager.cpp:6823
					unsigned long uid = packet.Decode4();
					// taskmanager.cpp:6824
					nick = packet.DecodeStr();
					// taskmanager.cpp:6826
					text = packet.DecodeStr();
					// taskmanager.cpp:6827
					if (IsLocalContent((localContentType_t)0x7D)
						// taskmanager.cpp:6829
						&& packet.Decode1() == 1) {
						// taskmanager.cpp:6832
						std::list<std::string>::iterator it = std::find(CSharedDoc::Instance()->m_ignoreList.begin(), CSharedDoc::Instance()->m_ignoreList.end(), nick);
						// taskmanager.cpp:6833
						if (it != CSharedDoc::Instance()->m_ignoreList.end())
							// taskmanager.cpp:6834
							break;
						// taskmanager.cpp:6839
						text = "\xb1\xe6\xb5\xe5:" + nick + "> " + "\x08" + text;
						// taskmanager.cpp:6841
						CSharedDoc::Instance()->m_chatManager.FilteringHack(text.c_str(), false);
						// taskmanager.cpp:6844
						CChatMsg::Instance()->AddChatMsg(text, 0xFF5BFFB0, false, false);
						// taskmanager.cpp:6845
						break;
					}
					// taskmanager.cpp:6852
					sFriend *pBuddy = BuddyMGR->GetBuddy(uid);
					// taskmanager.cpp:6854
					if (pBuddy) {
						// taskmanager.cpp:6856
						if (!(pBuddy->flags & 2)) {
							// taskmanager.cpp:6858
							pBuddy->flags |= 2;
							// taskmanager.cpp:6859
							CMessengerInfo::Instance()->Refresh();
						}
						// taskmanager.cpp:6861
						CMessengerInfo::Instance()->RecvChat(pBuddy, text);
					}
				}
				// taskmanager.cpp:6864
				break;
			case 0x114:
				// taskmanager.cpp:6872
				if (packet.Decode1() == 3) {
					// taskmanager.cpp:6878
					unsigned long uid = packet.Decode4();
					// taskmanager.cpp:6880
					sFriend *pBuddy = BuddyMGR->GetBuddy(uid);
					// taskmanager.cpp:6881
					if (pBuddy) {
						// taskmanager.cpp:6883
						CMessengerInfo::Instance()->RecvChat(pBuddy, "\xb8\xde\xbd\xc3\xc1\xf6\xb8\xa6 \xc0\xfc\xb4\xde\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
						// taskmanager.cpp:6884
						pBuddy->flags &= ~2;
						CMessengerInfo::Instance()->Refresh();
					}
				}
				break;
			case 0x10b:
				// taskmanager.cpp:6942
				if (packet.Decode4() != 0) {
					// taskmanager.cpp:6945
					CMessengerInfo::Instance()->Notify1("\xc4\xa3\xb1\xb8\xbb\xe8\xc1\xa6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
					// taskmanager.cpp:6946
					break;
				} else {
					// taskmanager.cpp:6949
					unsigned long uid = packet.Decode4();
					// taskmanager.cpp:6950
					sFriend *pBuddy = BuddyMGR->GetBuddy(uid);
					// taskmanager.cpp:6952
					if (pBuddy) {
						// taskmanager.cpp:6954
						if (pBuddy->listFlags & 2)
							// taskmanager.cpp:6957
							pBuddy->listFlags &= ~1;
						else
							// taskmanager.cpp:6965
							BuddyMGR->DeleteBuddy(uid);
					}
					CMessengerInfo::Instance()->Refresh();
				}
				break;
			case 0x10c:
				// taskmanager.cpp:6984
				if (packet.Decode4() != 0) {
					// taskmanager.cpp:6987
					CMessengerInfo::Instance()->Notify1("\xc4\xa3\xb1\xb8\xba\xed\xb7\xb0 \xbc\xb3\xc1\xa4\xc0\xbb \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
					// taskmanager.cpp:6988
					break;
				} else {
					// taskmanager.cpp:6993
					sFriend *pBuddy = BuddyMGR->GetBuddy(packet.Decode4());
					// taskmanager.cpp:6995
					if (pBuddy)
						// taskmanager.cpp:6997
						pBuddy->flags |= 0x10;
					CMessengerInfo::Instance()->Refresh();
				}
				break;
			case 0x10d:
				// taskmanager.cpp:7011
				if (packet.Decode4() != 0) {
					// taskmanager.cpp:7014
					CMessengerInfo::Instance()->Notify1("\xba\xed\xb7\xb0 \xc7\xd8\xc1\xf6\xb8\xa6 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
					// taskmanager.cpp:7015
					break;
				} else {
					// taskmanager.cpp:7019
					sFriend *pBuddy = BuddyMGR->GetBuddy(packet.Decode4());
					// taskmanager.cpp:7021
					if (pBuddy) {
						// taskmanager.cpp:7023
						pBuddy->flags &= ~0x10;
						// taskmanager.cpp:7024
						pBuddy->listFlags &= ~4;
					}
					CMessengerInfo::Instance()->Refresh();
				}
				break;
			case 0x119:
				// taskmanager.cpp:7039
				switch (packet.Decode4()) {
				case 0:
					{
						// taskmanager.cpp:7050
						unsigned long uid = packet.Decode4();
						// taskmanager.cpp:7052
						std::string group = packet.DecodeStr();
						// taskmanager.cpp:7054
						CSharedDoc::Instance()->m_chatManager.FilteringHack(group.c_str(), false);
						// taskmanager.cpp:7059
						sFriend *pBuddy = BuddyMGR->GetBuddy(uid);
						// taskmanager.cpp:7061
						if (pBuddy)
							// taskmanager.cpp:7063
							strncpy(pBuddy->group, group.c_str(), 10);
					}
					// taskmanager.cpp:7065
					break;
				case 9:
					// taskmanager.cpp:7069
					CMessengerInfo::Instance()->Notify1("\xc4\xa3\xb1\xb8 \xb5\xee\xb7\xcf\xb5\xc7\xbe\xee\xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
					// taskmanager.cpp:7071
					break;
				case 1:
					// taskmanager.cpp:7075
					CMessengerInfo::Instance()->Notify1("\xba\xb0\xb8\xed\xba\xaf\xb0\xe6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
					// taskmanager.cpp:7077
					break;
				}
				// taskmanager.cpp:7083
				break;
			case 0x10e:
				// taskmanager.cpp:7087
				if (!IsLocalContent((localContentType_t)0x7D)) {
					// taskmanager.cpp:7089
					sFriend buddy;
					// taskmanager.cpp:7090
					packet.DecodeBuffer(&buddy, sizeof(sFriend));
					// taskmanager.cpp:7093
					sFriend *pBuddy = BuddyMGR->GetBuddy(buddy.uid);
					// taskmanager.cpp:7094
					if (!pBuddy)
						break;
					// taskmanager.cpp:7097
					if (buddy.listFlags & 2) {
						// taskmanager.cpp:7103
						if (buddy.listFlags & 1) {
							// taskmanager.cpp:7106
							if (pBuddy->listFlags & 4)
								break;
							// taskmanager.cpp:7110
							pBuddy->listFlags |= 3;
							// taskmanager.cpp:7111
							pBuddy->flags |= 2;
						} else {
							// taskmanager.cpp:7119
							BuddyMGR->AddBuddy(&buddy);
						}
						// taskmanager.cpp:7131
						CMessengerInfo::Instance()->Notify2(MakeStr("%s\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9", buddy.nick));
					} else {
						// taskmanager.cpp:7137
						if ((pBuddy->listFlags & 4) || !(pBuddy->flags & 4))
							break;
						// taskmanager.cpp:7140
						pBuddy->flags |= 2;
						// taskmanager.cpp:7146
						CMessengerInfo::Instance()->Notify2(MakeStr("%s\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9", pBuddy->nick));
					}
					// taskmanager.cpp:7150
					CMessengerInfo::Instance()->Refresh();
					// taskmanager.cpp:7153
					break;
				} else {
					// taskmanager.cpp:7155
					unsigned long uid = packet.Decode4();
					// taskmanager.cpp:7156
					sFriend *pBuddy = BuddyMGR->GetBuddy(uid);
					// taskmanager.cpp:7157
					if (!pBuddy)
						break;
					// taskmanager.cpp:7159
					if (pBuddy->listFlags & 2) {
						// taskmanager.cpp:7165
						if (pBuddy->listFlags & 1) {
							// taskmanager.cpp:7168
							if (pBuddy->listFlags & 4)
								break;
							// taskmanager.cpp:7171
							pBuddy->flags |= 2;
						}
					// taskmanager.cpp:7184
					} else {
						// taskmanager.cpp:7186
						pBuddy->flags |= 2;
						// taskmanager.cpp:7189
						if ((pBuddy->listFlags & 4) || !(pBuddy->flags & 4))
							break;
					}
					// taskmanager.cpp:7195
					CMessengerInfo::Instance()->Notify2(MakeStr("%s\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9", pBuddy->nick));
					// taskmanager.cpp:7199
					CMessengerInfo::Instance()->Refresh();
				}
				break;
			case 0x10f:
				{
					// taskmanager.cpp:7216
					unsigned long uid = packet.Decode4();
					// taskmanager.cpp:7217
					sFriend *pBuddy = BuddyMGR->GetBuddy(uid);
					// taskmanager.cpp:7219
					if (pBuddy) {
						// taskmanager.cpp:7221
						if (!IsLocalContent((localContentType_t)0x7D)
							// taskmanager.cpp:7227
							&& (pBuddy->listFlags & 2)
							// taskmanager.cpp:7229
							&& !(pBuddy->listFlags & 1)) {
							// taskmanager.cpp:7235
							BuddyMGR->DeleteBuddy(uid);
						} else {
							// taskmanager.cpp:7225
							pBuddy->flags &= ~2;
						}
					}
					CMessengerInfo::Instance()->Refresh();
				}
				break;
			case 0x102:
				{
					// taskmanager.cpp:7262
					if (packet.Decode1() == 1)
						// taskmanager.cpp:7265
						BuddyMGR->ClearBuddy();
					// taskmanager.cpp:7282
					packet.Decode2();
					// taskmanager.cpp:7283
					unsigned short count = packet.Decode2();
					// taskmanager.cpp:7285
					sFriend buddy;
					// taskmanager.cpp:7287
					for (int i = 0; i < count; i++) {
						// taskmanager.cpp:7289
						packet.DecodeBuffer(&buddy, sizeof(sFriend));
						// taskmanager.cpp:7290
						if (buddy.uid != CSharedDoc::Instance()->m_myUID)
							// taskmanager.cpp:7292
							BuddyMGR->AddBuddy(&buddy);
					}
					// taskmanager.cpp:7294
					CMessengerInfo::Instance()->SetComplete(CONNECT_STATUS_COMPLETE);
					CMessengerInfo::Instance()->Refresh();
				}
				break;
			case 0x109:
				{
					// taskmanager.cpp:7381
					unsigned long result = packet.Decode4();
					// taskmanager.cpp:7383
					if (result == 9) {
						// taskmanager.cpp:7386
						CMessengerInfo::Instance()->Notify1("\xc4\xa3\xb1\xb8 \xb5\xee\xb7\xcf\xb5\xc7\xbe\xee\xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.");
						break;
					}
					// taskmanager.cpp:7388
					if (result != 0) {
						// taskmanager.cpp:7390
						CMessengerInfo::Instance()->Notify1("\xc4\xa3\xb1\xb8 \xc7\xe3\xbf\xeb\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
						// taskmanager.cpp:7391
						break;
					}
					// taskmanager.cpp:7394
					sFriend *pBuddy = BuddyMGR->GetBuddy(packet.Decode4());
					// taskmanager.cpp:7396
					if (!pBuddy)
						break;
					// taskmanager.cpp:7403
					pBuddy->flags |= 0xC;
					// taskmanager.cpp:7404
					strcpy(pBuddy->group, "Friend");
					// taskmanager.cpp:7406
					CMessengerInfo::Instance()->Notify2(MakeStr("%s\xb4\xd4\xc0\xcc \xc4\xa3\xb1\xb8\xb0\xa1 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9", pBuddy->nick));
					CMessengerInfo::Instance()->Refresh();
				}
				// taskmanager.cpp:7412
				break;
			case 0x10a:
				{
					// taskmanager.cpp:7494
					std::string unused;
					// taskmanager.cpp:7501
					if (packet.Decode4() == 0) {
						// taskmanager.cpp:7506
						sFriend *pBuddy = BuddyMGR->GetBuddy(packet.Decode4());
						// taskmanager.cpp:7508
						if (pBuddy) {
							// taskmanager.cpp:7511
							pBuddy->flags |= 6;
							// taskmanager.cpp:7512
							pBuddy->value29 = 0;
							// taskmanager.cpp:7513
							strcpy(pBuddy->group, "Friend");
							// taskmanager.cpp:7515
							CMessengerInfo::Instance()->Notify2(MakeStr("%s\xb4\xd4\xc0\xcc \xc4\xa3\xb1\xb8\xb0\xa1 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9", pBuddy->nick));
						}
						// taskmanager.cpp:7519
						CMessengerInfo::Instance()->Refresh();
					}
				}
				// taskmanager.cpp:7521
				break;
			case 0x103:
				{
					// taskmanager.cpp:7527
					CSharedDoc::Instance()->m_noteList.clear();
					// taskmanager.cpp:7529
					unsigned char count = packet.Decode1();
					// taskmanager.cpp:7530
					for (unsigned char i = 0; i < count; i++) {
						// taskmanager.cpp:7532
						sNoteInfo note;
						// taskmanager.cpp:7533
						packet.DecodeBuffer(&note, 0x6D);
						// taskmanager.cpp:7535
						CSharedDoc::Instance()->m_chatManager.FilteringHack(note.text, false);
						// taskmanager.cpp:7536
						CSharedDoc::Instance()->m_noteList.push_back(note);
					}
					// taskmanager.cpp:7539
					CMessengerInfo::Instance()->Notify1("\xc2\xca\xc1\xf6\xb0\xa1 \xb5\xb5\xc2\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9");
				}
				// taskmanager.cpp:7541
				break;
			case 0x112:
				// taskmanager.cpp:7545
				switch (packet.Decode1()) {
				case 0:
					// taskmanager.cpp:7550
					packet.Decode1();
					// taskmanager.cpp:7551
					CSharedDoc::Instance()->m_myPang -= 10;
					// taskmanager.cpp:7553
					CMessengerInfo::Instance()->Notify1("\xc2\xca\xc1\xf6\xb8\xa6 \xb3\xb2\xb0\xe5\xbd\xc0\xb4\xcf\xb4\xd9. \xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xb4\xd9\xc0\xbd \xb7\xce\xb1\xd7\xc0\xce\xc7\xd2 \xb6\xa7 \xc0\xfc\xb4\xde\xb5\xcb\xb4\xcf\xb4\xd9.");
					break;
				case 3:
					// taskmanager.cpp:7558
					CMessengerInfo::Instance()->Notify1("\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 \xc2\xca\xc1\xf6\xc7\xd4\xc0\xcc \xb0\xa1\xb5\xe6\xc2\xf7\xbc\xad \xc2\xca\xc1\xf6\xb8\xa6 \xba\xb8\xb3\xbe \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
					// taskmanager.cpp:7559
					break;
				case 1:
					// taskmanager.cpp:7562
					CMessengerInfo::Instance()->Notify1("\xc2\xca\xc1\xf6 \xba\xb8\xb3\xbb\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
					// taskmanager.cpp:7563
					break;
				}
				// taskmanager.cpp:7566
				break;
			case 0x118:
				{
					// taskmanager.cpp:7570
					WNetworkSystem *pNetwork = WNetworkSystem::Instance();
					// taskmanager.cpp:7577
					if (pNetwork->IsConnected(WNetworkSystem::NET_UNIT_MSN))
						// taskmanager.cpp:7579
						pNetwork->ForceShutDown(WNetworkSystem::NET_UNIT_MSN);
					// taskmanager.cpp:7582
					FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
					// taskmanager.cpp:7583
					pForm->SetMessage("\xb4\xd9\xb8\xa5 \xbb\xe7\xbf\xeb\xc0\xda\xb0\xa1 \xc7\xf6\xc0\xe7 \xb0\xe8\xc1\xa4\xc0\xb8\xb7\xce \xc1\xa2\xbc\xd3\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", false);
					// taskmanager.cpp:7584
					pForm->Open((FRESH_RESULT_CALLBACK)&CTask::OnOverlappedDlg, 3);
				}
				// taskmanager.cpp:7586
				break;
			case 0x123:
				{
					// taskmanager.cpp:7593
					packet.Decode4();
					// taskmanager.cpp:7594
					unsigned long uid = packet.Decode4();
					// taskmanager.cpp:7596
					sUserPosition position;
					// taskmanager.cpp:7597
					packet.DecodeBuffer(&position, sizeof(sUserPosition));
					// taskmanager.cpp:7602
					sFriend *pBuddy = BuddyMGR->GetBuddy(uid);
					// taskmanager.cpp:7604
					if (pBuddy)
						// taskmanager.cpp:7606
						memcpy(&pBuddy->extra, &position, sizeof(sUserPosition));
				}
				// taskmanager.cpp:7609
				break;
			case 0x115:
				{
					// taskmanager.cpp:7621
					unsigned long state = packet.Decode4();
					// taskmanager.cpp:7623
					unsigned long uid = packet.Decode4();
					// taskmanager.cpp:7628
					sFriend *pBuddy = BuddyMGR->GetBuddy(uid);
					// taskmanager.cpp:7630
					if (pBuddy) {
						// taskmanager.cpp:7637
						pBuddy->value25 = uid;
						switch (state) {
						case 0:
							// taskmanager.cpp:7642
							pBuddy->flags |= 0x22;
							break;
						case 1:
							// taskmanager.cpp:7649
							pBuddy->flags |= 0x42;
							break;
						case 3:
							// taskmanager.cpp:7656
							pBuddy->flags |= 0x82;
							break;
						case 4:
							// taskmanager.cpp:7664
							pBuddy->flags &= 0x1F;
							// taskmanager.cpp:7665
							if (!(pBuddy->flags & 2))
								// taskmanager.cpp:7670
								CMessengerInfo::Instance()->Notify2(MakeStr("%s\xb4\xd4\xc0\xcc \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9", pBuddy->nick));
							// taskmanager.cpp:7673
							pBuddy->flags |= 2;
							break;
						case 5:
							// taskmanager.cpp:7680
							pBuddy->flags &= 0x9F;
							break;
						}
					}
					// taskmanager.cpp:7690
					CMessengerInfo::Instance()->Refresh();
				}
				// taskmanager.cpp:7691
				break;
			case 0x122:
				{
					// taskmanager.cpp:7694
					WSendPacket send(0x16);
					// taskmanager.cpp:7698
					send.Send(SEND_TO_MSN);
					// taskmanager.cpp:7700
					CMessengerInfo::Instance()->Close(true);
					// taskmanager.cpp:7702
					if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN))
						// taskmanager.cpp:7703
						WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_UNIT_MSN);
					// taskmanager.cpp:7704
					CMessengerInfo::Instance()->Notify2("\xb8\xde\xbd\xc5\xc0\xfa\xbc\xad\xb9\xf6\xbf\xa1 \xc0\xdf\xb8\xf8\xb5\xc8 \xc0\xaf\xc0\xfa \xc1\xa4\xba\xb8\xb0\xa1 \xc0\xfc\xb4\xde\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xe7 \xc1\xa2\xbc\xd3\xc7\xd8\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9");
				}
				// taskmanager.cpp:7705
				break;
			}
			// taskmanager.cpp:7785
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x48, 0, 0, 0, 0, 0));
		}
		// taskmanager.cpp:7787
		break;
	case 0x98:
		// taskmanager.cpp:7792
		CSharedDoc::Instance()->SetIdentity(packet.Decode4());
		// taskmanager.cpp:7796
		if (IsPcBang()) {
			// taskmanager.cpp:7797
			CSharedDoc::Instance()->AccumulateIdentity(0x80);
		}
		{
			// taskmanager.cpp:7799
			CTask* pTask = AfxGetTask();
			if ((__rtti_obj = pTask) != 0 ? ((IObject *)pTask)->IsKindOf(&CGolfTask::m_RTTI) : false) {
				// taskmanager.cpp:7800
				CSharedDoc::Instance()->m_roomPlayers[MyIndex()].m_identity = CSharedDoc::Instance()->m_userInfo.identity;
			}
		}
		break;
	case 0xaf:
		{
			// taskmanager.cpp:7810
			unsigned char count = packet.Decode1();
			// taskmanager.cpp:7811
			std::list<sItemInfo> items;
			// taskmanager.cpp:7813
			for (unsigned char i = 0; i < count; i++) {
				// taskmanager.cpp:7815
				sItemInfo item;
				// taskmanager.cpp:7816
				packet.DecodeBuffer(&item, sizeof(item));
				// taskmanager.cpp:7817
				items.push_back(item);
			}
			// taskmanager.cpp:7821
			std::list<sItemInfo>::iterator it;
			for (it = items.begin(); it != items.end(); it++) {
				// taskmanager.cpp:7824
				std::list<sItemInfo> &myItems = CSharedDoc::Instance()->m_itemInfoList;
				bool found = false;
				std::list<sItemInfo>::iterator myIt;
				for (myIt = myItems.begin(); myIt != myItems.end(); myIt++) {
					// taskmanager.cpp:7826
					if (myIt->m_typeId == it->m_typeId) {
						// taskmanager.cpp:7828
						myIt->m_count = it->m_count;
						found = true;
						break;
					}
				}
				// taskmanager.cpp:7835
				if (!found) {
					myItems.push_back(*it);
				}
			}
			// taskmanager.cpp:7840
			if (CMessengerInfo::Instance()->m_myStatus != MSN::STATUS_BUSY) {
				// taskmanager.cpp:7841
				CMessengerInfo::Instance()->SetMyStatus(MSN::STATUS_ONLINE);
			}
		}
		// taskmanager.cpp:7842
		break;
	case 0xb2:
		// taskmanager.cpp:7847
		{
			unsigned char season = packet.Decode1();
			// taskmanager.cpp:7848
			packet.Decode1();
			// taskmanager.cpp:7849
			unsigned char count = packet.Decode1();
			std::list<sSpecialTrophy> *pTrophies = 0;
			// taskmanager.cpp:7852
			switch (season) {
			case 0:
				// taskmanager.cpp:7856
				pTrophies = &CSharedDoc::Instance()->m_specialTrophies;
				break;
			case 4:
			case 5:
				// taskmanager.cpp:7862
				pTrophies = &CSharedDoc::Instance()->m_mySpecialTrophies;
				// taskmanager.cpp:7864
				break;
			}
			// taskmanager.cpp:7867
			if (pTrophies) {
				// taskmanager.cpp:7869
				for (int i = 0; i < count; i++) {
					sSpecialTrophy trophy;
					// taskmanager.cpp:7872
					packet.DecodeBuffer(&trophy, sizeof(trophy));
					// taskmanager.cpp:7873
					pTrophies->push_back(trophy);
				}
			}
		}
		break;
	case 0x16f:
		// taskmanager.cpp:7881
		{
			unsigned char season = packet.Decode1();
			sTrophyStatistics *pTrophy = 0;
			// taskmanager.cpp:7884
			switch (season) {
			case 0:
				// taskmanager.cpp:7888
				pTrophy = &CSharedDoc::Instance()->m_oldSeasonTrophy;
				break;
			case 4:
			case 5:
				// taskmanager.cpp:7895
				pTrophy = &CSharedDoc::Instance()->m_myTrophy;
				// taskmanager.cpp:7897
				break;
			}
			// taskmanager.cpp:7900
			if (pTrophy) {
				// taskmanager.cpp:7901
				packet.DecodeBuffer(pTrophy, sizeof(sTrophyStatistics));
			}
		}
		break;
	case 0xd6:
		// taskmanager.cpp:7907
		{
			packet.Decode1();
			// taskmanager.cpp:7908
			unsigned char count = packet.Decode1();
			// taskmanager.cpp:7910
			for (int i = 0; i < count; i++) {
				sGuildTrophy trophy;
				// taskmanager.cpp:7913
				packet.DecodeBuffer(&trophy, sizeof(trophy));
				// taskmanager.cpp:7914
				CSharedDoc::Instance()->m_myGuildTrophies.push_back(trophy);
			}
		}
		break;
	case 0xb8:
		// taskmanager.cpp:7922
		{
			_SYSTEMTIME time;
			packet.DecodeBuffer(&time, sizeof(time));
			CSharedDoc::Instance()->SetServerTime(time);
		}
		break;
	case 0xc7:
		// taskmanager.cpp:7948
		{
			std::string name = packet.DecodeStr();
			// taskmanager.cpp:7949
			std::string message = packet.DecodeStr();
			// taskmanager.cpp:7951
			CSharedDoc::Instance()->m_chatManager.FilteringHack(message.c_str(), false);
			// taskmanager.cpp:7953
			COneLineBoard::Instance()->AddOnelineMsg(name, message);
		}
		break;
	case 0xc9:
		// taskmanager.cpp:7961
		// taskmanager.cpp:7962
		if (packet.Decode4() != 0) {
	case 0x94:
			// taskmanager.cpp:7965
			CSharedDoc::Instance()->m_myBonusCash = packet.Decode8();
			// taskmanager.cpp:7966
			CSharedDoc::Instance()->RefreshAllCookie();
		} else {
			// taskmanager.cpp:7974
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xba\xb8\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xb4\xc2 \xc4\xed\xc5\xb0\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
		}
		break;
	case 0xc8:
		// taskmanager.cpp:7983
		{
			unsigned short param1 = packet.Decode2();
			// taskmanager.cpp:7984
			unsigned long param2 = packet.Decode4();
			// taskmanager.cpp:7986
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x5D, param1, param2, 0, 0, 0));
		}
		break;
	case 0xcc:
		// taskmanager.cpp:7992
		{
			unsigned char doubled = packet.Decode1();
			// taskmanager.cpp:7994
			if (doubled) {
				// taskmanager.cpp:7995
				CSharedDoc::Instance()->m_pangRate *= 2.0f;
			}
			// taskmanager.cpp:7997
			unsigned short count = packet.Decode2();
			// taskmanager.cpp:7999
			CSharedDoc::Instance()->m_gainedItemList[0].clear();
			// taskmanager.cpp:8020
			for (int i = 0; i < count; i++) {
				// taskmanager.cpp:8021
				CSharedDoc::Instance()->m_gainedItemList[0].push_back(packet.Decode4());
			}
		}
		break;
	case 0xf8:
		// taskmanager.cpp:8030
		for (int i = 0; i < 4; i++) {
			// taskmanager.cpp:8031
			CSharedDoc::Instance()->m_gainedItemList[i].clear();
		}
		{
			// taskmanager.cpp:8034
			int count = packet.Decode2();
			// taskmanager.cpp:8035
			for (int n = 0; n < count; n++) {
				// taskmanager.cpp:8037
				unsigned long guid = packet.Decode4();
				// taskmanager.cpp:8038
				unsigned char doubleRate = packet.Decode1();
				// taskmanager.cpp:8039
				unsigned short itemCount = packet.Decode2();
				// taskmanager.cpp:8041
				if (guid == MyGuid(false)) {
					// taskmanager.cpp:8043
					if (doubleRate)
						// taskmanager.cpp:8044
						CSharedDoc::Instance()->m_pangRate *= 2.0f;
				}
				// taskmanager.cpp:8047
				int index = -1;
				// taskmanager.cpp:8048
				for (int i = 0; i < 4; i++) {
					// taskmanager.cpp:8050
					if (guid == CSharedDoc::Instance()->m_roomPlayers[i].m_guid) {
						// taskmanager.cpp:8052
						index = i;
						break;
					}
				}
				// taskmanager.cpp:8057
				for (int j = 0; j < itemCount; j++) {
					// taskmanager.cpp:8059
					unsigned long typeId = packet.Decode4();
					// taskmanager.cpp:8074
					if (index != -1 && typeId != 0)
						// taskmanager.cpp:8076
						CSharedDoc::Instance()->m_gainedItemList[index].push_back(typeId);
				}
			}
		}
		break;
	case 0x1ae:
		// taskmanager.cpp:8089
		{
			BYTE enable = packet.Decode1();
			if (enable) {
				DWORD interval = packet.Decode4();
				CTaskManager::Instance()->SetWork(new CWorkAliveCheck(interval));
			} else {
				CTaskManager::Instance()->ReleaseWork();
			}
		}
		break;
	case 0xd5:
		// taskmanager.cpp:8107
		_GGC_CSAuth2(packet);
		break;
	case 0xcf:
		// taskmanager.cpp:8116
		CSharedDoc::Instance()->m_newMapEvent = true;
		// taskmanager.cpp:8117
		CSharedDoc::Instance()->m_newMapEventFlags = packet.Decode1();
		// taskmanager.cpp:8119
		if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false) {
			// taskmanager.cpp:8120
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0xB2, 0, 0, 0, 0, 0);
		}
		break;
	case 0xd0:
		// taskmanager.cpp:8128
		CSharedDoc::Instance()->m_flag4D84 = true;
		// taskmanager.cpp:8130
		if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false) {
			// taskmanager.cpp:8131
			AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0xB3, 0, 0, 0, 0, 0);
		}
		break;
	case 0xd8:
		// taskmanager.cpp:8139
		CSharedDoc::Instance()->m_controlServerService = packet.Decode4();
		// taskmanager.cpp:8141
		if (IsLocalContent((localContentType_t)0x56)) {
			// taskmanager.cpp:8143
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x22F, 0, 0, 0, 0, 0);
		}
		break;
	case 0xdb: {
		// taskmanager.cpp:8150
		int result = packet.Decode4();
		// taskmanager.cpp:8153
		FrScratchDlg* pDlg = FrStaticCast<FrScratchDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("scratchdlg"));
		// taskmanager.cpp:8161
		if (result == 0) {
			// taskmanager.cpp:8165
			int count = packet.Decode4();
			// taskmanager.cpp:8167
			sBonusBall items[11] = { 0 };
			// taskmanager.cpp:8169
			for (int i = 0; i < count; i++) {
				// taskmanager.cpp:8171
				packet.DecodeBuffer(&items[i], sizeof(sBonusBall));
			}
			// taskmanager.cpp:8175
			if (pDlg == 0)
				break;
			// taskmanager.cpp:8177
			pDlg->EnableNextPageButton(true);
			// taskmanager.cpp:8178
			pDlg->SetScratchItem(count, items);
			// taskmanager.cpp:8179
			pDlg->InitChangeScratchCardNumber(0);
			// taskmanager.cpp:8181
			if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CRealMyRoomTask::m_RTTI) : false) {
				// taskmanager.cpp:8183
				AfxPostMsg(0, "RealMyRoom", 0xE1, 0, 0, 0, 0);
			}
		} else if (result == 0x1B) {
			// taskmanager.cpp:8190
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbd\xba\xc5\xa9\xb7\xa1\xc4\xa1\xb8\xa6 \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc3\xb4\xc2\xb5\xa5 \xba\xd2\xc6\xed\xc0\xbb \xb5\xe5\xb7\xc1 \xc1\xcb\xbc\xdb\xc7\xd5\xb4\xcf\xb4\xd9. \xc0\xe7\xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbd\xc3\xb8\xe9 \xc1\xa4\xbb\xf3\xc0\xfb\xc0\xb8\xb7\xce \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
		} else {
			// taskmanager.cpp:8195
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbd\xba\xc5\xa9\xb7\xa1\xc4\xa1 \xc0\xcc\xbf\xeb \xbf\xc0\xb7\xf9\xb0\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
		}
		break;
	}
	case 0xdc: {
		// taskmanager.cpp:8203
		int result = packet.Decode4();
		// taskmanager.cpp:8206
		FrScratchDlg* pDlg = FrStaticCast<FrScratchDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("scratchdlg"));
		// taskmanager.cpp:8209
		if (pDlg) {
			// taskmanager.cpp:8212
			pDlg->EnableScratchSerialNumber(true);
		}
		// taskmanager.cpp:8215
		switch (result) {
		case 0: {
			// taskmanager.cpp:8219
			int response = packet.Decode4();
			// taskmanager.cpp:8220
			unsigned long serial = packet.Decode4();
			// taskmanager.cpp:8223
			if (pDlg == 0)
				break;
			// taskmanager.cpp:8225
			pDlg->ResponseScratchSerialNumber(response, serial);
			// taskmanager.cpp:8228
			if (response == 0) {
				// taskmanager.cpp:8229
				pDlg->InitChangeScratchCardNumber(1);
			}
			break;
		}
		case 1:
			// taskmanager.cpp:8235
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xb5\xc8 \xbd\xc3\xb8\xae\xbe\xf3 \xb9\xf8\xc8\xa3\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
			break;
		case 2:
			// taskmanager.cpp:8240
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xbd\xc3\xb8\xae\xbe\xf3 \xb9\xf8\xc8\xa3 \xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
			break;
		case 3:
			// taskmanager.cpp:8245
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xaf\xc8\xbf\xb1\xe2\xb0\xa3\xc0\xcc \xc1\xf6\xb3\xad \xbd\xc3\xb8\xae\xbe\xf3 \xb9\xf8\xc8\xa3\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
			break;
		case 4:
			// taskmanager.cpp:8250
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc3\xd6\xb4\xeb \xc1\xf6\xb1\xde \xc8\xbd\xbc\xf6\xb8\xa6 \xc3\xca\xb0\xfa\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
			break;
		default:
			// taskmanager.cpp:8254
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbf\xc3\xb9\xd9\xb8\xa5 \xc4\xed\xc6\xf9 \xb9\xf8\xc8\xa3\xb0\xa1 \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
			break;
		}
		break;
	}
	case 0xdd:
		// taskmanager.cpp:8261
		{
			sItemInfo item;
			// taskmanager.cpp:8263
			packet.DecodeBuffer(&item, sizeof(item));
			// taskmanager.cpp:8265
			if (item.m_serial == 0)
				break;
			// taskmanager.cpp:8266
			if (item.m_typeId == 0)
				break;
			// taskmanager.cpp:8267
			if (item.m_count <= 0)
				break;
			std::list<sItemInfo> &itemList = CSharedDoc::Instance()->m_itemInfoList;
			bool found = false;
			// taskmanager.cpp:8273
			std::list<sItemInfo>::iterator it;
			for (it = itemList.begin(); it != itemList.end(); ++it) {
				// taskmanager.cpp:8275
				if (it->m_serial == item.m_serial
					// taskmanager.cpp:8276
					&& it->m_typeId == item.m_typeId) {
					// taskmanager.cpp:8278
					it->m_count = item.m_count;
					found = true;
					break;
				}
			}
			// taskmanager.cpp:8286
			if (!found)
				itemList.push_back(item);
		}
		break;
	case 0xde:
		// taskmanager.cpp:8295
		CSharedDoc::Instance()->m_latestVersion = packet.DecodeStr();
		break;
	case 0x16d:
		// taskmanager.cpp:8315
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc6\xce\xbe\xdf\xb8\xb6\xc4\xcf \xb9\xd7 \xc1\xf8\xbf\xad\xb4\xeb \xb1\xe2\xb4\xc9\xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
		break;
	case 0x164:
		// taskmanager.cpp:8328
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:8330
		if (!IsLocalContent((localContentType_t)0x66))
			break;
		{
			// taskmanager.cpp:8332
			int result = packet.Decode2();
			// taskmanager.cpp:8333
			unsigned char request = packet.Decode1();
			// taskmanager.cpp:8334
			unsigned char placed = packet.Decode1();
			// taskmanager.cpp:8336
			if (result == 1) {
				// taskmanager.cpp:8338
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x22f, 0, 0, 0, 0, 0));
				// taskmanager.cpp:8340
				if (placed) {
					// taskmanager.cpp:8342
					if (request == 1)
						// taskmanager.cpp:8344
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb \xb9\xe8\xc4\xa1 \xbc\xba\xb0\xf8", 0, 0, 0, 0));
					else
						// taskmanager.cpp:8348
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xc1\xa6\xb0\xc5\xb5\xc8 \xbb\xf3\xc5\xc2\xb7\xce \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc6\xce\xbe\xdf \xb8\xb6\xc4\xcf\xbf\xa1 \xb5\xee\xb7\xcf\xb5\xc7\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				} else {
					// taskmanager.cpp:8353
					if (request == 1)
						// taskmanager.cpp:8355
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xb9\xe8\xc4\xa1\xb5\xc8 \xbb\xf3\xc5\xc2\xb7\xce \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc6\xce\xbe\xdf \xb8\xb6\xc4\xcf\xbf\xa1 \xb5\xee\xb7\xcf \xb5\xcb\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
					else
						// taskmanager.cpp:8359
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xc1\xa6\xb0\xc5\xb5\xc8 \xbb\xf3\xc5\xc2\xb7\xce \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc6\xce\xbe\xdf \xb8\xb6\xc4\xcf\xbf\xa1 \xb5\xee\xb7\xcf\xb5\xc7\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				}
			} else {
				// taskmanager.cpp:8365
				if (request == 1)
					// taskmanager.cpp:8367
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb \xb9\xe8\xc4\xa1 \xbd\xc7\xc6\xd0", 0, 0, 0, 0));
				else
					// taskmanager.cpp:8371
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb \xc1\xa6\xb0\xc5 \xbd\xc7\xc6\xd0", 0, 0, 0, 0));
			}
		}
		break;
	case 0x16c:
		// taskmanager.cpp:8380
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:8381
		if (!IsLocalContent((localContentType_t)0x66)) {
			break;
		}
		{
			// taskmanager.cpp:8387
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:8388
			unsigned long ownerUID = packet.Decode4();
			// taskmanager.cpp:8389
			unsigned char success = packet.Decode1();
			// taskmanager.cpp:8391
			if (success != 1) {
				break;
			}
			// taskmanager.cpp:8393
			if (MyUID() == uid) {
				// taskmanager.cpp:8395
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xda\xbd\xc5\xc0\xc7 \xc1\xf8\xbf\xad\xb4\xeb\xb4\xc2 \xb1\xb8\xb0\xe6\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:8397
				break;
			}
			// taskmanager.cpp:8403
			std::list<sStall> &stallList = CSharedDoc::Instance()->m_listStallItem;
			// taskmanager.cpp:8405
			sStall stall;
			// taskmanager.cpp:8406
			for (std::list<sStall>::iterator it = stallList.begin(); it != stallList.end(); it++) {
				// taskmanager.cpp:8408
				if (it->ownerUID == ownerUID) {
					// taskmanager.cpp:8412
					CSharedDoc::Instance()->SetStallItem(*it);
					break;
				}
			}
			// taskmanager.cpp:8417
			CSharedDoc::Instance()->SetTradeMode(1);
			// taskmanager.cpp:8419
			if (CSharedDoc::Instance()->m_stallItem.ownerUID) {
				// taskmanager.cpp:8421
				FrWnd *pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("offlinetradedlg");
				// taskmanager.cpp:8422
				if (FrStaticCast<COfflineShopVisitorDlg>(pWnd)) {
					break;
				}
				// taskmanager.cpp:8424
				COfflineShopVisitorDlg *pDlg = CreateForm<COfflineShopVisitorDlg>(g_pFresh->GetManager(), this, "offlinetradedlg", 0);
				// taskmanager.cpp:8426
				if (pDlg == 0) {
					break;
				}
				// taskmanager.cpp:8429
				pDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
				// taskmanager.cpp:8430
				pDlg->OpenTradeShop(CSharedDoc::Instance()->m_myRoomOwnerUID);
				// taskmanager.cpp:8431
				pDlg->SetListTradeItem(false);
				// taskmanager.cpp:8432
				pDlg->SetFadeout(false);
			} else {
				// taskmanager.cpp:8438
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
			}
		}
		break;
	case 0x167:
		// taskmanager.cpp:8451
		{
			unsigned long roomOwnerUID = packet.Decode4();
			// taskmanager.cpp:8452
			unsigned long stallOwnerUID = packet.Decode4();
			// taskmanager.cpp:8453
			int slot = packet.Decode1();
			// taskmanager.cpp:8457
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
			// taskmanager.cpp:8459
			CSharedDoc *pDoc = CSharedDoc::Instance();
			std::list<sStall> *pStallList = pDoc->GetListStallItem();
			sStall *pStall = pDoc->GetStallItem();
			// taskmanager.cpp:8462
			if (pDoc->m_myRoomOwnerUID != roomOwnerUID) {
				break;
			}
			// taskmanager.cpp:8464
			if (pStall->ownerUID == stallOwnerUID) {
				// taskmanager.cpp:8466
				if (pStall->slots[slot - 1].item.count == 0)
					// taskmanager.cpp:8467
					memset(&pStall->slots[slot - 1].item, 0, sizeof(sTradeItem));
				// taskmanager.cpp:8469
				CSharedDoc::Instance()->SetStallItem(*pStall);
				// taskmanager.cpp:8470
				CSharedDoc::Instance()->ChageListStallitem(pStall);
			} else {
				// taskmanager.cpp:8475
				if (pStallList == 0) {
					break;
				}
				// taskmanager.cpp:8479
				std::list<sStall>::iterator it = pStallList->begin();
				if (it == pStallList->end()) {
					break;
				}
				// taskmanager.cpp:8481
				if (it->ownerUID != stallOwnerUID) {
					break;
				}
				// taskmanager.cpp:8483
				if (it->open <= 0) {
					break;
				}
				// taskmanager.cpp:8485
				if (it->slots[0].used != slot) {
					break;
				}
				// taskmanager.cpp:8487
				memset(&it->slots[0].item, 0, sizeof(sTradeItem));
			}
			// taskmanager.cpp:8502
			if (MyUID() == CSharedDoc::Instance()->m_myRoomOwnerUID) {
				// taskmanager.cpp:8505
				FrWnd *pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradeeditdlg");
				// taskmanager.cpp:8508
				COfflineShopEditDlg *pEditDlg = FrStaticCast<COfflineShopEditDlg>(pWnd);
				if (pEditDlg)
					// taskmanager.cpp:8516
					pEditDlg->SetListTradeItem();
				// taskmanager.cpp:8520
				pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradebuydlg");
				// taskmanager.cpp:8522
				FrOfflineTradeFinishEditDlg *pFinishDlg = FrStaticCast<FrOfflineTradeFinishEditDlg>(pWnd);
				if (pFinishDlg)
					// taskmanager.cpp:8526
					pFinishDlg->SetListTradeItem((std::list<sTradeItem> *)0);
				// taskmanager.cpp:8530
				pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("offlinetradedlg");
				// taskmanager.cpp:8532
				COfflineShopDlg *pShopDlg = FrStaticCast<COfflineShopDlg>(pWnd);
				if (pShopDlg == 0) {
					break;
				}
				// taskmanager.cpp:8535
				pShopDlg->OpenTradeShop(CSharedDoc::Instance()->m_myRoomOwnerUID);
				// taskmanager.cpp:8536
				pShopDlg->SetListTradeItem(false);
			} else {
				// taskmanager.cpp:8542
				FrWnd *pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradebuydlg");
				// taskmanager.cpp:8544
				FrOfflineTradeFinishEditDlg *pFinishDlg = FrStaticCast<FrOfflineTradeFinishEditDlg>(pWnd);
				if (pFinishDlg)
					// taskmanager.cpp:8548
					pFinishDlg->Close(true);
				// taskmanager.cpp:8551
				pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("offlinetradedlg");
				// taskmanager.cpp:8553
				COfflineShopVisitorDlg *pVisitorDlg = FrStaticCast<COfflineShopVisitorDlg>(pWnd);
				if (pVisitorDlg == 0) {
					break;
				}
				// taskmanager.cpp:8556
				pVisitorDlg->OpenTradeShop(CSharedDoc::Instance()->m_myRoomOwnerUID);
				// taskmanager.cpp:8557
				pVisitorDlg->SetListTradeItem(false);
			}
		}
		break;
	case 0x161:
		// taskmanager.cpp:8566
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:8568
		if (!IsLocalContent((localContentType_t)0x66)) {
			break;
		}
		{
			// taskmanager.cpp:8570
			unsigned long result = packet.Decode4();
			// taskmanager.cpp:8571
			unsigned long error = packet.Decode4();
			// taskmanager.cpp:8573
			if (result == 1) {
				// taskmanager.cpp:8575
				__int64 pang = packet.Decode8();
				// taskmanager.cpp:8576
				CSharedDoc::Instance()->m_myPang = pang;
				// taskmanager.cpp:8578
				if (MyUID() == CSharedDoc::Instance()->m_myRoomOwnerUID) {
					break;
				}
				// taskmanager.cpp:8580
				WSendPacket send(0x21);
				// taskmanager.cpp:8582
				send.EncodeStr(MyNick());
				// taskmanager.cpp:8583
				send.Encode4(MyUID());
				// taskmanager.cpp:8588
				send.Send(SEND_TO_DEFAULT);
				// taskmanager.cpp:8589
				break;
			}
			// taskmanager.cpp:8593
			switch (error) {
			case 0:
				// taskmanager.cpp:8595
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb \xc0\xcc\xbf\xeb\xbf\xa1 \xc1\xa6\xc0\xe7\xb8\xa6 \xb9\xde\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 1:
				// taskmanager.cpp:8596
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb \xb0\xfc\xb7\xc3 \xbc\xba\xb0\xf8", 0, 0, 0, 0));
				break;
			case 2:
				// taskmanager.cpp:8597
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb \xb0\xfc\xb7\xc3 \xbd\xc7\xc6\xd0", 0, 0, 0, 0));
				break;
			case 3:
				// taskmanager.cpp:8598
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb \xb5\xee\xb7\xcf\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(Error DB)", 0, 0, 0, 0));
				break;
			case 4:
				// taskmanager.cpp:8599
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xb5\xee\xb7\xcf \xbc\xba\xb0\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. \xb0\xe1\xb0\xfa\xb0\xaa\xc0\xbb \xb9\xde\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 5:
				// taskmanager.cpp:8600
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xb8\xa6 \xbc\xd2\xc0\xaf\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. \xb1\xb8\xb8\xc5 \xc8\xc4 \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.", 0, 0, 0, 0));
				break;
			case 6:
				// taskmanager.cpp:8601
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 \xbd\xbd\xb7\xd4 \xb5\xa5\xc0\xcc\xc5\xb8\xb8\xa6 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 7:
				// taskmanager.cpp:8602
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 8:
				// taskmanager.cpp:8603
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xcc\xb9\xcc \xbf\xb5\xb1\xb8 \xbd\xbd\xb7\xd4\xc0\xb8\xb7\xce \xbf\xac\xc0\xe5\xb9\xd7 \xb1\xb8\xb8\xc5\xc7\xd2\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 9:
				// taskmanager.cpp:8604
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc4\xed\xc5\xb0\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0xA:
				// taskmanager.cpp:8605
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xb5\xee\xb7\xcf\xc7\xcf\xb7\xc1\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb0\xb3\xbc\xf6\xb0\xa1 \xbc\xd2\xc0\xaf\xc7\xd1 \xbc\xf6\xb7\xae\xba\xb8\xb4\xd9 \xb8\xb9\xbe\xc6\xbc\xad \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ", 0, 0, 0, 0));
				break;
			case 0xC:
				// taskmanager.cpp:8606
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xb5\xee\xb7\xcf\xc7\xcf\xb7\xc1\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0xD:
				// taskmanager.cpp:8607
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbd\xbd\xb7\xd4\xbf\xa1 \xc0\xcc\xb9\xcc \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xb5\xee\xb7\xcf \xb5\xc7\xbe\xee\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xb5\xee\xb7\xcf \xc7\xd8\xc1\xf6 \xc0\xcc\xc8\xc4 \xb4\xd9\xbd\xc3 \xb5\xee\xb7\xcf\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4", 0, 0, 0, 0));
				break;
			case 0xE:
				// taskmanager.cpp:8608
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0xF:
				// taskmanager.cpp:8609
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xcc\xb9\xcc \xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0x10:
				// taskmanager.cpp:8610
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbd\xbd\xb7\xd4 \xb1\xe2\xb0\xa3\xc0\xcc \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0x11:
				// taskmanager.cpp:8611
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb7\xae\xc0\xbb \xc3\xca\xb0\xfa\xc7\xcf\xb0\xd4 \xb5\xcb\xb4\xcf\xb4\xd9. \xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb0\xb9\xbc\xf6\xb8\xa6 \xb4\xd9\xbd\xc3 \xc8\xae\xc0\xce\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0x12:
				// taskmanager.cpp:8612
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc6\xc7\xb8\xc5\xc0\xda\xc0\xc7 \xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xc6\xed\xc1\xfd \xb8\xf0\xb5\xe5\xb7\xce \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xb1\xb8\xb8\xc5\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0xB:
			case 0x13: {
				// taskmanager.cpp:8617
				if (MyUID() == CSharedDoc::Instance()->m_myRoomOwnerUID) {
					// taskmanager.cpp:8620
					FrTradeEditDlg *pEditDlg = FrStaticCast<FrTradeEditDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradeeditdlg"));
					// taskmanager.cpp:8623
					if (pEditDlg) {
						// taskmanager.cpp:8630
						pEditDlg->Close(true);
					}
					// taskmanager.cpp:8634
					FrTradeBuyDlg *pBuyDlg = FrStaticCast<FrTradeBuyDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradebuydlg"));
					// taskmanager.cpp:8636
					if (pBuyDlg) {
						pBuyDlg->Close(true);
					}
				} else {
					// taskmanager.cpp:8662
					FrTradeBuyDlg *pBuyDlg = FrStaticCast<FrTradeBuyDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradebuydlg"));
					// taskmanager.cpp:8664
					if (pBuyDlg) {
						// taskmanager.cpp:8668
						pBuyDlg->Close(true);
					}
				}
				// taskmanager.cpp:8671
				FrWnd *pShopDlg = FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("offlinetradedlg"));
				// taskmanager.cpp:8674
				if (pShopDlg) {
					// taskmanager.cpp:8678
					pShopDlg->Close(true);
				}
				// taskmanager.cpp:8687
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xc0\xc7 \xc1\xa4\xba\xb8\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf8\xbf\xad\xb4\xeb\xb8\xa6 \xb4\xd9\xbd\xc3 \xbf\xad\xbe\xee \xc1\xd6\xbc\xbc\xbf\xe4", 0, 0, 0, 0));
				// taskmanager.cpp:8694
				break;
			}
			}
		}
		break;
	case 0x165:
	case 0x166:
		{
			// taskmanager.cpp:8698
			int maxPage = packet.Decode2();
			// taskmanager.cpp:8699
			int count = packet.Decode2();
			// taskmanager.cpp:8705
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
			// taskmanager.cpp:8707
			if (count != 0) {
				// taskmanager.cpp:8709
				CTask *task = AfxGetTask();
				// taskmanager.cpp:8710
				((CTaskMain *)task->m_mainActor)->CreateMaketDlg();
				// taskmanager.cpp:8712
				FrWnd *pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("offlinetraderanking");
				// taskmanager.cpp:8714
				CSharedDoc::Instance()->ClearMaketList();
				// taskmanager.cpp:8716
				for (int i = 0; i < count; i++) {
					// One sale entry, decoded into a static buffer.
					static sSaleItem s_saleItem;
					// taskmanager.cpp:8719
					packet.DecodeBuffer(&s_saleItem, sizeof(sSaleItem));
					// taskmanager.cpp:8722
					CSharedDoc::Instance()->AddMaketListItem(s_saleItem);
				}
				// taskmanager.cpp:8727
				COfflineTradeDlg *pDlg = FrStaticCast<COfflineTradeDlg>(pWnd);
				if (pDlg) {
					// taskmanager.cpp:8745
					pDlg->SetListItem();
					// taskmanager.cpp:8746
					pDlg->SetPageData(-1, maxPage);
				}
			} else {
				// taskmanager.cpp:8753
				FrWnd *pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("offlinetraderanking");
				// taskmanager.cpp:8755
				CSharedDoc::Instance()->ClearMaketList();
				// taskmanager.cpp:8756
				COfflineTradeDlg *pDlg = FrStaticCast<COfflineTradeDlg>(pWnd);
				if (pDlg) {
					// taskmanager.cpp:8762
					pDlg->SetListItem();
					// taskmanager.cpp:8763
					pDlg->SetPageData(1, 1);
				}
				// taskmanager.cpp:8767
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xb8\xb6\xc4\xcf\xbf\xa1\xbc\xad \xb0\xcb\xbb\xf6\xb5\xc8 \xbb\xf3\xc7\xb0\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
			}
		}
		// taskmanager.cpp:8775
		break;
	case 0x162:
	case 0x163:
		// taskmanager.cpp:8780
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:8782
		if (!IsLocalContent((localContentType_t)0x66)) {
			break;
		}
		{
			// taskmanager.cpp:8784
			unsigned long result = packet.Decode4();
			// taskmanager.cpp:8785
			unsigned long error = packet.Decode4();
			// taskmanager.cpp:8787
			if (result == 1) {
				if (type == 0x162) {
					// taskmanager.cpp:8789
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb5\xee\xb7\xcf \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
					// taskmanager.cpp:8791
					break;
				} else if (type == 0x163) {
					// taskmanager.cpp:8793
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc1\xa6\xb0\xc5 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
					// taskmanager.cpp:8795
					break;
				}
			}
			// taskmanager.cpp:8797
			switch (error) {
			case 0:
				// taskmanager.cpp:8799
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb \xc0\xcc\xbf\xeb\xbf\xa1 \xc1\xa6\xc0\xe7\xb8\xa6 \xb9\xde\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 1:
				// taskmanager.cpp:8800
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb \xb0\xfc\xb7\xc3 \xbc\xba\xb0\xf8", 0, 0, 0, 0));
				break;
			case 2:
				// taskmanager.cpp:8801
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb \xb0\xfc\xb7\xc3 \xbd\xc7\xc6\xd0", 0, 0, 0, 0));
				break;
			case 3:
				// taskmanager.cpp:8802
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb \xb5\xee\xb7\xcf\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.(Error DB)", 0, 0, 0, 0));
				break;
			case 4:
				// taskmanager.cpp:8803
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xb5\xee\xb7\xcf \xbc\xba\xb0\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. \xb0\xe1\xb0\xfa\xb0\xaa\xc0\xbb \xb9\xde\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 5:
				// taskmanager.cpp:8804
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xb8\xa6 \xbc\xd2\xc0\xaf\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. \xb1\xb8\xb8\xc5 \xc8\xc4 \xc0\xcc\xbf\xeb\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.", 0, 0, 0, 0));
				break;
			case 6:
				// taskmanager.cpp:8805
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 \xbd\xbd\xb7\xd4 \xb5\xa5\xc0\xcc\xc5\xb8\xb8\xa6 \xb0\xa1\xc1\xf6\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 7:
				// taskmanager.cpp:8806
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 8:
				// taskmanager.cpp:8807
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xcc\xb9\xcc \xbf\xb5\xb1\xb8 \xbd\xbd\xb7\xd4\xc0\xb8\xb7\xce \xbf\xac\xc0\xe5\xb9\xd7 \xb1\xb8\xb8\xc5\xc7\xd2\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 9:
				// taskmanager.cpp:8808
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc4\xed\xc5\xb0\xb0\xa1 \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0xA:
				// taskmanager.cpp:8809
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xb5\xee\xb7\xcf\xc7\xcf\xb7\xc1\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb0\xb3\xbc\xf6\xb0\xa1 \xbc\xd2\xc0\xaf\xc7\xd1 \xbc\xf6\xb7\xae\xba\xb8\xb4\xd9 \xb8\xb9\xbe\xc6\xbc\xad \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. ", 0, 0, 0, 0));
				break;
			case 0xC:
				// taskmanager.cpp:8810
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xb5\xee\xb7\xcf\xc7\xcf\xb7\xc1\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0xD:
				// taskmanager.cpp:8811
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbd\xbd\xb7\xd4\xbf\xa1 \xc0\xcc\xb9\xcc \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xb5\xee\xb7\xcf \xb5\xc7\xbe\xee\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xb5\xee\xb7\xcf \xc7\xd8\xc1\xf6 \xc0\xcc\xc8\xc4 \xb4\xd9\xbd\xc3 \xb5\xee\xb7\xcf\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4", 0, 0, 0, 0));
				break;
			case 0xE:
				// taskmanager.cpp:8812
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0xF:
				// taskmanager.cpp:8813
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xcc\xb9\xcc \xc1\xf8\xbf\xad\xb4\xeb\xbf\xa1 \xb5\xee\xb7\xcf\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0x10:
				// taskmanager.cpp:8814
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbd\xbd\xb7\xd4 \xb1\xe2\xb0\xa3\xc0\xcc \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0x11:
				// taskmanager.cpp:8815
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb7\xae\xc0\xbb \xc3\xca\xb0\xfa\xc7\xcf\xb0\xd4 \xb5\xcb\xb4\xcf\xb4\xd9. \xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xb0\xb9\xbc\xf6\xb8\xa6 \xb4\xd9\xbd\xc3 \xc8\xae\xc0\xce\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0x12:
				// taskmanager.cpp:8816
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc6\xc7\xb8\xc5\xc0\xda\xc0\xc7 \xc1\xf8\xbf\xad\xb4\xeb\xb0\xa1 \xc6\xed\xc1\xfd \xb8\xf0\xb5\xe5\xb7\xce \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xb1\xb8\xb8\xc5\xb0\xa1 \xba\xd2\xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				break;
			case 0xB: {
				// taskmanager.cpp:8820
				if (MyUID() == CSharedDoc::Instance()->m_myRoomOwnerUID) {
					// taskmanager.cpp:8823
					FrTradeEditDlg *pEditDlg = FrStaticCast<FrTradeEditDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradeeditdlg"));
					// taskmanager.cpp:8826
					if (pEditDlg) {
						// taskmanager.cpp:8833
						pEditDlg->Close(true);
					}
					// taskmanager.cpp:8837
					FrTradeBuyDlg *pBuyDlg = FrStaticCast<FrTradeBuyDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradebuydlg"));
					// taskmanager.cpp:8839
					if (pBuyDlg) {
						pBuyDlg->Close(true);
					}
				} else {
					// taskmanager.cpp:8865
					FrTradeBuyDlg *pBuyDlg = FrStaticCast<FrTradeBuyDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradebuydlg"));
					// taskmanager.cpp:8867
					if (pBuyDlg) {
						// taskmanager.cpp:8871
						pBuyDlg->Close(true);
					}
				}
				// taskmanager.cpp:8874
				FrWnd *pShopDlg = FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("offlinetradedlg"));
				// taskmanager.cpp:8876
				if (pShopDlg) {
					// taskmanager.cpp:8880
					pShopDlg->Close(true);
				}
				// taskmanager.cpp:8885
				WSendPacket send((enumClientPacket)0xC4);
				// taskmanager.cpp:8886
				send.Encode4(CSharedDoc::Instance()->m_myRoomOwnerUID);
				// taskmanager.cpp:8887
				send.Send(SEND_TO_DEFAULT);
				// taskmanager.cpp:8889
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc1\xf8\xbf\xad\xb4\xeb\xc0\xc7 \xc1\xa4\xba\xb8\xb0\xa1 \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9. \xc1\xf8\xbf\xad\xb4\xeb\xb8\xa6 \xb4\xd9\xbd\xc3 \xbf\xad\xbe\xee \xc1\xd6\xbc\xbc\xbf\xe4", 0, 0, 0, 0));
				break;
			}
			}
		}
		break;
	case 0x169:
		// taskmanager.cpp:8902
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:8904
		if (!IsLocalContent((localContentType_t)0x66)) {
			break;
		}
		{
			// taskmanager.cpp:8907
			packet.Decode4();
			// taskmanager.cpp:8908
			unsigned char slot = packet.Decode1();
			// taskmanager.cpp:8910
			static sTradeItem item;
			// taskmanager.cpp:8912
			packet.DecodeBuffer(&item, sizeof(sTradeItem));
			// taskmanager.cpp:8915
			sStall *pStall = CSharedDoc::Instance()->GetStallItem();
			// taskmanager.cpp:8918
			if (item.typeId == 0 && pStall->slots[slot - 1].reserved) {
				FILETIME expireTime;
				FILETIME serverTime;
				// taskmanager.cpp:8922
				SystemTimeToFileTime(&pStall->slots[slot - 1].expireTime, &expireTime);
				// taskmanager.cpp:8923
				SystemTimeToFileTime(&CSharedDoc::Instance()->GetServerTime(), &serverTime);
				// taskmanager.cpp:8930
				if ((((__int64)expireTime.dwHighDateTime - serverTime.dwHighDateTime) * 0x100000000 - serverTime.dwLowDateTime + expireTime.dwLowDateTime) / 60 / 60 / 10000000 <= 0)
					// taskmanager.cpp:8931
					pStall->slots[slot - 1].reserved = 0;
			}
			// taskmanager.cpp:8940
			pStall->slots[slot - 1].item = item;
			// taskmanager.cpp:8958
			CSharedDoc::Instance()->SetStallItem(*pStall);
			// taskmanager.cpp:8960
			CSharedDoc::Instance()->ChageListStallitem(pStall);
			// taskmanager.cpp:8962
			FrWnd *pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradeeditdlg");
			// taskmanager.cpp:8965
			COfflineShopEditDlg *pEditDlg = FrStaticCast<COfflineShopEditDlg>(pWnd);
			if (pEditDlg) {
				// taskmanager.cpp:8969
				pEditDlg->m_closing = 1;
				// taskmanager.cpp:8970
				pEditDlg->Close(true);
				// taskmanager.cpp:8985
				COfflineShopDlg *pShopDlg = CreateForm<COfflineShopDlg>(g_pFresh->GetManager(), this, "offlinetradedlg", 0);
				// taskmanager.cpp:8987
				if (pShopDlg) {
					// taskmanager.cpp:8989
					pShopDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
					// taskmanager.cpp:8990
					pShopDlg->OpenTradeShop(CSharedDoc::Instance()->m_myRoomOwnerUID);
					// taskmanager.cpp:8991
					pShopDlg->SetListTradeItem(false);
					// taskmanager.cpp:8992
					pShopDlg->SetFadeout(false);
				}
			}
			// taskmanager.cpp:8996
			pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradebuydlg");
			// taskmanager.cpp:8998
			FrOfflineTradeFinishEditDlg *pFinishDlg = FrStaticCast<FrOfflineTradeFinishEditDlg>(pWnd);
			if (pFinishDlg)
				// taskmanager.cpp:9002
				pFinishDlg->SetListTradeItem((std::list<sTradeItem> *)0);
			// taskmanager.cpp:9006
			pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("offlinetradedlg");
			// taskmanager.cpp:9008
			COfflineShopDlg *pShopDlg = FrStaticCast<COfflineShopDlg>(pWnd);
			if (pShopDlg == 0) {
				break;
			}
			// taskmanager.cpp:9011
			pShopDlg->OpenTradeShop(CSharedDoc::Instance()->m_myRoomOwnerUID);
			// taskmanager.cpp:9012
			pShopDlg->SetListTradeItem(false);
		}
		break;
	case 0xe1:
		// taskmanager.cpp:9020
		{
			int result = packet.Decode4();
			// taskmanager.cpp:9022
			if (result == 1) {
				// taskmanager.cpp:9024
				std::string nick;
				// taskmanager.cpp:9025
				packet.DecodeStr(nick);
				// taskmanager.cpp:9026
				GetActor("AvatarChat") << MsgObject(0, 0xe8, (intptr_t)nick.c_str(), result, 0, 0, 0);
				// taskmanager.cpp:9028
				if (strcmp(nick.c_str(), MyNick()) == 0)
					// taskmanager.cpp:9030
					CSharedDoc::Instance()->SetStateTrade((eStateTrade)1);
				// taskmanager.cpp:9032
			}
			// taskmanager.cpp:9034
			SetTradeResultCode(result, 0xe1, NULL);
		}
		break;
	case 0xe2:
		// taskmanager.cpp:9040
		{
			int result = packet.Decode4();
			// taskmanager.cpp:9042
			if (result == 1) {
				// taskmanager.cpp:9044
				std::string nick;
				// taskmanager.cpp:9046
				packet.DecodeStr(nick);
				// taskmanager.cpp:9047
				unsigned long uid = packet.Decode4();
				// taskmanager.cpp:9049
				GetActor("AvatarChat") << MsgObject(0, 0xe8, (intptr_t)nick.c_str(), 0, 0, 0, 0);
				// taskmanager.cpp:9052
				if (uid == CSharedDoc::Instance()->m_tradeUID || uid == MyUID()) {
					// taskmanager.cpp:9054
					CSharedDoc::Instance()->InitTradeData();
					// taskmanager.cpp:9056
					FrTradeDlg *pTradeDlg = FrStaticCast<FrTradeDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradedlg"));
					// taskmanager.cpp:9058
					if (pTradeDlg) {
						// taskmanager.cpp:9064
						pTradeDlg->Close(true);
					}
					// taskmanager.cpp:9069
					if (strcmp(nick.c_str(), MyNick()) == 0) {
						// taskmanager.cpp:9071
						FrTradeEditDlg *pEditDlg = FrStaticCast<FrTradeEditDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradeeditdlg"));
						// taskmanager.cpp:9073
						if (pEditDlg) {
							// taskmanager.cpp:9079
							pEditDlg->Close(true);
						}
						// taskmanager.cpp:9084
					} else {
						// taskmanager.cpp:9087
						result = 3;
						CSharedDoc::Instance()->InitTradeData();
						// taskmanager.cpp:9089
						FrTradeBuyDlg *pBuyDlg = FrStaticCast<FrTradeBuyDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradebuydlg"));
						// taskmanager.cpp:9091
						if (pBuyDlg) {
							// taskmanager.cpp:9097
							FrWnd *pNotify = g_pFresh->GetManager()->GetDesktop()->FindChildByName("notify_okcancel");
							// taskmanager.cpp:9098
							if (pNotify)
								// taskmanager.cpp:9100
								pNotify->Close(true);
							// taskmanager.cpp:9104
							pBuyDlg->Close(true);
						}
					}
				}
				// taskmanager.cpp:9108
			}
			// taskmanager.cpp:9110
			SetTradeResultCode(result, 0xe2, NULL);
		}
		break;
	case 0xe3:
		// taskmanager.cpp:9116
		{
			int result = packet.Decode4();
			// taskmanager.cpp:9118
			if (result == 1) {
				// taskmanager.cpp:9120
				std::string nick;
				// taskmanager.cpp:9122
				packet.DecodeStr(nick);
				// taskmanager.cpp:9123
				unsigned long uid = packet.Decode4();
				// taskmanager.cpp:9125
				GetActor("AvatarChat") << MsgObject(0, 0xe8, (intptr_t)nick.c_str(), 2, 0, 0, 0);
				// taskmanager.cpp:9128
				if (strcmp(nick.c_str(), MyNick()) == 0) {
					// taskmanager.cpp:9130
					CSharedDoc::Instance()->SetStateTrade(STATE_TRADE_READY);
					// taskmanager.cpp:9132
					FrTradeDlg *pTradeDlg = FrStaticCast<FrTradeDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradedlg"));
					// taskmanager.cpp:9134
					if (pTradeDlg) {
						// taskmanager.cpp:9138
						pTradeDlg->Close(true);
					}
					// taskmanager.cpp:9141
					FrTradeEditDlg *pEditDlg = FrStaticCast<FrTradeEditDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradeeditdlg"));
					// taskmanager.cpp:9143
					if (!pEditDlg) {
						// taskmanager.cpp:9145
						pEditDlg = CreateForm<FrTradeEditDlg>(g_pFresh->GetManager(), this, "tradeeditdlg", 0);
						// taskmanager.cpp:9147
						if (pEditDlg) {
							// taskmanager.cpp:9149
							pEditDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
							// taskmanager.cpp:9150
							pEditDlg->SetFadeout(false);
						}
					}
					// taskmanager.cpp:9155
				} else if (uid == CSharedDoc::Instance()->m_tradeUID) {
					// taskmanager.cpp:9157
					result = 4;
					// taskmanager.cpp:9158
					CSharedDoc::Instance()->InitTradeData();
					// taskmanager.cpp:9160
					FrTradeDlg *pTradeDlg = FrStaticCast<FrTradeDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradedlg"));
					// taskmanager.cpp:9162
					if (pTradeDlg) {
						// taskmanager.cpp:9166
						pTradeDlg->Close(true);
					}
					// taskmanager.cpp:9169
					FrTradeBuyDlg *pBuyDlg = FrStaticCast<FrTradeBuyDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradebuydlg"));
					// taskmanager.cpp:9171
					if (pBuyDlg) {
						// taskmanager.cpp:9175
						pBuyDlg->Close(true);
					}
				}
				// taskmanager.cpp:9179
				SetTradeResultCode(result, 0xe3, NULL);
				// taskmanager.cpp:9180
			} else {
				// taskmanager.cpp:9183
				SetTradeResultCode(result, 0xe3, NULL);
			}
		}
		break;
	case 0xe4:
		{
			// taskmanager.cpp:9191
			eTypeSalesTrade saleType = (eTypeSalesTrade)0;
			// taskmanager.cpp:9194
			int result = packet.Decode4();
			// taskmanager.cpp:9196
			if (result == 1) {
				// taskmanager.cpp:9199
				CSharedDoc::Instance()->InitTradeData();
				// taskmanager.cpp:9200
				CSharedDoc::Instance()->SetStateTrade((eStateTrade)4);
				// taskmanager.cpp:9204
				saleType = (eTypeSalesTrade)packet.Decode4();
				// taskmanager.cpp:9207
				std::string title("");
				// taskmanager.cpp:9209
				char nick[0x16] = {0};
				// taskmanager.cpp:9210
				packet.DecodeBuffer(nick, sizeof(nick));
				// taskmanager.cpp:9211
				packet.DecodeStr(title);
				// taskmanager.cpp:9213
				unsigned long uid = packet.Decode4();
				// taskmanager.cpp:9214
				CSharedDoc::Instance()->SetTradeUID(uid);
				// taskmanager.cpp:9215
				CSharedDoc::Instance()->SetTradeTitle(title);
				// taskmanager.cpp:9219
				sTradeItem item;
				// taskmanager.cpp:9220
				int count = packet.Decode4();
				// taskmanager.cpp:9222
				for (int i = 0; i < count; i++) {
					// taskmanager.cpp:9224
					packet.DecodeBuffer(&item, sizeof(sTradeItem));
					// taskmanager.cpp:9226
					if (!CSharedDoc::Instance()->AddListTradeItem(item)) {
						// taskmanager.cpp:9229
						result = 0;
						break;
					}
				}
			}
			// taskmanager.cpp:9235
			SetTradeResultCode(result, 0xE4, 0);
			// taskmanager.cpp:9237
			FrTradeDlg *pDlg = FrStaticCast<FrTradeDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradedlg"));
			// taskmanager.cpp:9240
			if (!pDlg) {
				// taskmanager.cpp:9246
				pDlg = CreateForm<FrTradeDlg>(g_pFresh->GetManager(), this, "tradedlg", 0);
				// taskmanager.cpp:9248
				if (!pDlg)
					break;
				// taskmanager.cpp:9250
				pDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
				// taskmanager.cpp:9251
				pDlg->SetFadeout(false);
			}
			// taskmanager.cpp:9255
			if (pDlg) {
				// taskmanager.cpp:9257
				if (result == 1) {
					// taskmanager.cpp:9260
					pDlg->SetPackageSale(saleType);
					// taskmanager.cpp:9262
					pDlg->OpenTradeShop(CSharedDoc::Instance()->m_tradeUID);
					// taskmanager.cpp:9263
					pDlg->SetListTradeItem(false);
				} else {
					// taskmanager.cpp:9267
					pDlg->Close(true);
				}
			}
		}
		break;
	case 0xe5:
		// taskmanager.cpp:9275
		{
			int result = packet.Decode4();
			// taskmanager.cpp:9277
			if (result == 1) {
				// taskmanager.cpp:9279
				CSharedDoc::Instance()->InitTradeData();
			}
			// taskmanager.cpp:9282
			SetTradeResultCode(result, 0xE5, 0);
		}
		break;
	case 0xe6:
		{
			// taskmanager.cpp:9288
			int result = packet.Decode4();
			// taskmanager.cpp:9289
			std::string title;
			// taskmanager.cpp:9291
			if (result == 1) {
				// taskmanager.cpp:9293
				packet.DecodeStr(title);
				// taskmanager.cpp:9294
				unsigned long uid = packet.Decode4();
				// taskmanager.cpp:9296
				std::string nick;
				// taskmanager.cpp:9298
				packet.DecodeStr(nick);
				// taskmanager.cpp:9301
				CTask *pTask = ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false) ? AfxGetTask() : AfxGetPreservedTask();
				// taskmanager.cpp:9303
				if (pTask) {
					// taskmanager.cpp:9305
					pTask->GetActor("AvatarChat") << MsgObject(0, 0xe9, (intptr_t)nick.c_str(), (intptr_t)title.c_str(), 0, 0, 0);
				}
				// taskmanager.cpp:9308
				if (CSharedDoc::Instance()->m_stateTrade != STATE_TRADE_NONE) {
					// taskmanager.cpp:9310
					if (uid == CSharedDoc::Instance()->m_tradeUID || uid == MyUID()) {
						// taskmanager.cpp:9312
						CSharedDoc::Instance()->SetTradeTitle(title);
						// taskmanager.cpp:9314
						FrTradeDlg *pDlg = FrStaticCast<FrTradeDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradedlg"));
						// taskmanager.cpp:9317
						if (pDlg) {
							// taskmanager.cpp:9321
							pDlg->UpdateTradeTitle();
						}
						// taskmanager.cpp:9324
						FrTradeEditDlg *pEditDlg = FrStaticCast<FrTradeEditDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradeeditdlg"));
						// taskmanager.cpp:9326
						if (pEditDlg) {
							// taskmanager.cpp:9330
							pEditDlg->UpdateTradeTitle();
							// taskmanager.cpp:9331
							pEditDlg->UpdateTradeTitleEdit();
						}
					}
				}
				// taskmanager.cpp:9335
			} else {
				// taskmanager.cpp:9338
				CSharedDoc::Instance()->SetTradeTitle(title);
			}
			// taskmanager.cpp:9341
			SetTradeResultCode(result, 0xE6, 0);
			// taskmanager.cpp:9342
		}
		break;
	case 0xe7:
		// taskmanager.cpp:9347
		{
			int result = packet.Decode4();
			// taskmanager.cpp:9348
			int count = packet.Decode4();
			// taskmanager.cpp:9350
			if (result == 1)
				// taskmanager.cpp:9352
				CSharedDoc::Instance()->SetCountVisitor(count);
			// taskmanager.cpp:9355
			SetTradeResultCode(result, 0xE7, 0);
			// taskmanager.cpp:9357
			FrTradeDlg *pDlg = FrStaticCast<FrTradeDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradedlg"));
			// taskmanager.cpp:9359
			if (pDlg == 0)
				break;
			// taskmanager.cpp:9363
			pDlg->UpdateCountVisitor();
		}
		break;
	case 0xe8:
		// taskmanager.cpp:9370
		{
			int result = packet.Decode4();
			// taskmanager.cpp:9373
			__int64 income = packet.Decode8();
			// taskmanager.cpp:9374
			if (result == 1)
				// taskmanager.cpp:9376
				CSharedDoc::Instance()->SetTradeIncome(income);
			// taskmanager.cpp:9378
			SetTradeResultCode(result, 0xE8, 0);
		}
		break;
	case 0xe9:
		{
			// taskmanager.cpp:9388
			eTypeSalesTrade saleType = (eTypeSalesTrade)0;
			int result = packet.Decode4();
			// taskmanager.cpp:9390
			if (result == 1) {
				// taskmanager.cpp:9392
				CSharedDoc::Instance()->SetStateTrade((eStateTrade)result);
				// taskmanager.cpp:9393
				CSharedDoc::Instance()->ClearListTradeItem();
				// taskmanager.cpp:9397
				saleType = (eTypeSalesTrade)packet.Decode4();
				// taskmanager.cpp:9400
				char title[0x16] = {0};
				// taskmanager.cpp:9401
				packet.DecodeBuffer(title, sizeof(title));
				// taskmanager.cpp:9402
				unsigned long uid = packet.Decode4();
				// taskmanager.cpp:9403
				CSharedDoc::Instance()->SetTradeUID(uid);
				// taskmanager.cpp:9405
				GetActor("AvatarChat") << MsgObject(0, 0xe8, (intptr_t)title, 1, 0, 0, 0);
				// taskmanager.cpp:9409
				sTradeItem item;
				// taskmanager.cpp:9410
				int count = packet.Decode4();
				// taskmanager.cpp:9412
				for (int i = 0; i < count; i++) {
					// taskmanager.cpp:9414
					packet.DecodeBuffer(&item, sizeof(sTradeItem));
					// taskmanager.cpp:9416
					if (!CSharedDoc::Instance()->AddListTradeItem(item)) {
						// taskmanager.cpp:9419
						result = 0;
						break;
					}
				}
			}
			// taskmanager.cpp:9425
			SetTradeResultCode(result, 0xE9, 0);
			// taskmanager.cpp:9427
			if (result != 1)
				break;
			// taskmanager.cpp:9429
			FrWnd *pEditDlg = FrStaticCast<FrWnd>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradeeditdlg"));
			// taskmanager.cpp:9431
			if (pEditDlg) {
				// taskmanager.cpp:9435
				pEditDlg->Close(true);
			}
			// taskmanager.cpp:9438
			FrTradeDlg *pDlg = FrStaticCast<FrTradeDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradedlg"));
			// taskmanager.cpp:9440
			if (pDlg)
				break;
			// taskmanager.cpp:9442
			pDlg = CreateForm<FrTradeDlg>(g_pFresh->GetManager(), this, "tradedlg", 0);
			// taskmanager.cpp:9444
			if (!pDlg)
				break;
			// taskmanager.cpp:9446
			pDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
			// taskmanager.cpp:9447
			pDlg->SetFadeout(false);
			// taskmanager.cpp:9450
			pDlg->SetPackageSale(saleType);
			// taskmanager.cpp:9453
			pDlg->OpenTradeShop(MyUID());
		}
		break;
	case 0x1d7:
		// taskmanager.cpp:9463
		{
			Cm::eErrorCode err = (Cm::eErrorCode)packet.Decode4();
			// taskmanager.cpp:9464
			if (!IS_SUCCESSFUL(err)) {
				// taskmanager.cpp:9466
				GetActor(data_aa04ec[3]) << MsgObject(0, 0x273, 0, err, 0, 0, 0);
				return 1;
			}
			// Leaves no code; the original's only trace is the xtAlways<0>::Get
			// COMDAT, its exact place in these cases is not evidenced.
			xtASSERT(IS_SUCCESSFUL(err));
			// taskmanager.cpp:9470
			_gateway::sTradeSellParam param;
			param.result = 1;
			// taskmanager.cpp:9472
			packet.Decode8();
			// taskmanager.cpp:9473
			param.pang = packet.Decode8();
			// taskmanager.cpp:9474
			unsigned char count = packet.Decode1();
			// taskmanager.cpp:9476
			param.itemList.reserve(count);
			// taskmanager.cpp:9478
			for (unsigned char i = 0; i < count; i++) {
				// taskmanager.cpp:9480
				sTradeItem item;
				// taskmanager.cpp:9481
				packet.DecodeBuffer(&item, sizeof(sTradeItem));
				// taskmanager.cpp:9482
				param.itemList.push_back(item);
			}
			// taskmanager.cpp:9485
			GetActor(data_aa04ec[3]) << MsgObject(0, 0x273, 2, (intptr_t)&param, 0, 0, 0);
		}
		// taskmanager.cpp:9486
		break;
	case 0x1d6:
		// taskmanager.cpp:9491
		{
			Cm::eErrorCode err = (Cm::eErrorCode)packet.Decode4();
			// taskmanager.cpp:9492
			if (!IS_SUCCESSFUL(err)) {
				// taskmanager.cpp:9494
				GetActor(data_aa04ec[3]) << MsgObject(0, 0x273, 0, err, 0, 0, 0);
				return 1;
			}
			// taskmanager.cpp:9498
			_gateway::sTradeBuyParam param;
			param.result = 1;
			// taskmanager.cpp:9500
			param.pang = packet.Decode8();
			// taskmanager.cpp:9501
			unsigned char count = packet.Decode1();
			// taskmanager.cpp:9502
			param.itemList.reserve(count);
			// taskmanager.cpp:9503
			for (unsigned char i = 0; i < count; i++) {
				// taskmanager.cpp:9505
				_gateway::sTradeBuyBase item;
				// taskmanager.cpp:9507
				packet.DecodeBuffer(&item.item, sizeof(sTradeItem));
				// taskmanager.cpp:9508
				item.countType = packet.Decode1();
				// taskmanager.cpp:9509
				packet.DecodeBuffer(&item.info, sizeof(sItemInfo));
				// taskmanager.cpp:9511
				param.itemList.push_back(item);
			}
			// taskmanager.cpp:9514
			GetActor(data_aa04ec[3]) << MsgObject(0, 0x273, 1, (intptr_t)&param, 0, 0, 0);
		}
		// taskmanager.cpp:9515
		break;
	case 0x1d8:
		// taskmanager.cpp:9520
		{
			Cm::eErrorCode err = (Cm::eErrorCode)packet.Decode4();
			// taskmanager.cpp:9521
			if (!IS_SUCCESSFUL(err)) {
				// taskmanager.cpp:9523
				GetActor(data_aa04ec[3]) << MsgObject(0, 0x273, 0, err, 0, 0, 0);
				return 1;
			}
			// taskmanager.cpp:9527
			_gateway::sTradeMinusParam param;
			// taskmanager.cpp:9529
			packet.DecodeStr(param.ownerName);
			// taskmanager.cpp:9530
			param.shopUID = packet.Decode4();
			// taskmanager.cpp:9531
			unsigned char count = packet.Decode1();
			// taskmanager.cpp:9533
			param.itemList.reserve(count);
			// taskmanager.cpp:9534
			for (unsigned char i = 0; i < count; i++) {
				// taskmanager.cpp:9536
				sTradeItem item;
				// taskmanager.cpp:9537
				packet.DecodeBuffer(&item, sizeof(sTradeItem));
				// taskmanager.cpp:9538
				param.itemList.push_back(item);
			}
			// taskmanager.cpp:9540
			param.shopState = packet.Decode4();
			// taskmanager.cpp:9542
			GetActor(data_aa04ec[3]) << MsgObject(0, 0x273, 3, (intptr_t)&param, 0, 0, 0);
		}
		// taskmanager.cpp:9543
		break;
	case 0xea:
		// taskmanager.cpp:9550
		{
			int result = packet.Decode4();
			// taskmanager.cpp:9551
			sTradeItem tradeItem;
			unsigned char kind = 0;
			__int64 pang;
			// taskmanager.cpp:9556
			if (result == 1) {
				// taskmanager.cpp:9558
				kind = packet.Decode1();
				// taskmanager.cpp:9559
				pang = packet.Decode8();
				// taskmanager.cpp:9561
				packet.DecodeBuffer(&tradeItem, sizeof(sTradeItem));
				// taskmanager.cpp:9570
				if (kind == 1) {
					// taskmanager.cpp:9573
					switch (tradeItem.typeId >> 26) {
					case 6:
						// taskmanager.cpp:9579
						{
							std::list<sItemInfo>::iterator it;
							for (it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); it++) {
								// taskmanager.cpp:9581
								if (it->m_serial == tradeItem.guid) {
									if (it->m_typeId == tradeItem.typeId) {
										if (it->m_count > (int)tradeItem.count)
											it->m_count -= (short)tradeItem.count;
										else if (it->m_count == (int)tradeItem.count)
											CSharedDoc::Instance()->m_itemInfoList.erase(it);
									}
									break;
								}
							}
						}
						break;
					case 5:
						// taskmanager.cpp:9604
						{
							std::list<sItemInfo>::iterator it;
							for (it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); it++) {
								// taskmanager.cpp:9606
								if (it->m_serial == tradeItem.guid) {
									if (it->m_typeId == tradeItem.typeId) {
										if (it->m_count > (int)tradeItem.count)
											it->m_count -= (short)tradeItem.count;
										else if (it->m_count == (int)tradeItem.count)
											CSharedDoc::Instance()->m_myGearList.erase(it);
									}
									break;
								}
							}
						}
						break;
					case 2:
						// taskmanager.cpp:9630
						if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(tradeItem.typeId)) {
							// taskmanager.cpp:9632
							{
								std::list<sItemInfo>::iterator it;
								for (it = CSharedDoc::Instance()->m_myItemList.begin(); it != CSharedDoc::Instance()->m_myItemList.end(); it++) {
									// taskmanager.cpp:9637
									if (it->m_serial == tradeItem.guid && it->m_typeId == tradeItem.typeId
										&& strcmpi(it->m_uccKey, tradeItem.uccIdx) == 0 && it->m_uccSeq == tradeItem.uccSeq) {
										// taskmanager.cpp:9639
										CSharedDoc::Instance()->m_myItemList.erase(it);
										// taskmanager.cpp:9640
										if (IsLocalContent((localContentType_t)0x6F))
											// taskmanager.cpp:9641
											CSharedDoc::Instance()->DeleteCardToPartsAttach(tradeItem.guid);
										break;
									}
								}
							}
						// taskmanager.cpp:9646
						} else {
							// taskmanager.cpp:9648
							{
								std::list<sItemInfo>::iterator it;
								for (it = CSharedDoc::Instance()->m_myPartsList.begin(); it != CSharedDoc::Instance()->m_myPartsList.end(); it++) {
									// taskmanager.cpp:9650
									if (it->m_serial == tradeItem.guid) {
										// taskmanager.cpp:9652
										if (it->m_typeId == tradeItem.typeId) {
											// taskmanager.cpp:9654
											CSharedDoc::Instance()->m_myPartsList.erase(it);
											// taskmanager.cpp:9655
											if (IsLocalContent((localContentType_t)0x6F))
												// taskmanager.cpp:9656
												CSharedDoc::Instance()->DeleteCardToPartsAttach(tradeItem.guid);
										}
										break;
									}
								}
							}
						}
						// taskmanager.cpp:9664
						break;
					case 4:
						// taskmanager.cpp:9670
						{
							std::map<unsigned int, sItemInfo>::iterator it;
							for (it = CSharedDoc::Instance()->m_myItemMap.begin(); it != CSharedDoc::Instance()->m_myItemMap.end(); it++) {
								// taskmanager.cpp:9672
								if (it->second.m_serial == tradeItem.guid) {
									// taskmanager.cpp:9674
									if (it->second.m_typeId == tradeItem.typeId)
										// taskmanager.cpp:9676
										CSharedDoc::Instance()->m_myItemMap.erase(it);
									break;
								}
							}
						}
						// taskmanager.cpp:9682
						break;
					case 28:
						// taskmanager.cpp:9688
						{
							std::list<sItemInfo>::iterator it;
							for (it = CSharedDoc::Instance()->m_myExtraItemList3.begin(); it != CSharedDoc::Instance()->m_myExtraItemList3.end(); it++) {
								// taskmanager.cpp:9690
								if (it->m_serial == tradeItem.guid) {
									// taskmanager.cpp:9692
									if (it->m_typeId == tradeItem.typeId) {
										// taskmanager.cpp:9694
										if (it->m_count > (int)tradeItem.count)
											// taskmanager.cpp:9696
											it->m_count -= (short)tradeItem.count;
										// taskmanager.cpp:9698
										else if (it->m_count == (int)tradeItem.count)
											// taskmanager.cpp:9700
											CSharedDoc::Instance()->m_myExtraItemList3.erase(it);
									}
									break;
								}
							}
						}
						// taskmanager.cpp:9707
						break;
					case 7:
						// taskmanager.cpp:9711
						{
							std::map<unsigned int, sCaddieInfo>::iterator it = CSharedDoc::Instance()->m_caddieMap.find(tradeItem.guid);
							// taskmanager.cpp:9713
							if (it != CSharedDoc::Instance()->m_caddieMap.end())
								// taskmanager.cpp:9715
								CSharedDoc::Instance()->m_caddieMap.erase(it);
						}
						// taskmanager.cpp:9718
						break;
					case 31:
						// taskmanager.cpp:9726
						{
							std::list<sCards>::iterator it;
							for (it = CSharedDoc::Instance()->m_myCardList.begin(); it != CSharedDoc::Instance()->m_myCardList.end(); it++) {
								// taskmanager.cpp:9728
								if (it->m_typeId == tradeItem.typeId) {
									// taskmanager.cpp:9730
									it->m_count -= tradeItem.count;
									// taskmanager.cpp:9731
									if (it->m_count < 1)
										// taskmanager.cpp:9732
										CSharedDoc::Instance()->m_myCardList.erase(it);
									break;
								}
							}
						}
						break;
					}
					// taskmanager.cpp:9741
					CSharedDoc::Instance()->m_myPang += pang;
					// taskmanager.cpp:9742
					CSharedDoc::Instance()->AddTradeIncome(pang);
				// taskmanager.cpp:9744
				} else if (kind == 0) {
					// taskmanager.cpp:9746
					unsigned char addType = packet.Decode1();
					// taskmanager.cpp:9747
					sItemInfo itemInfo;
					// taskmanager.cpp:9748
					sCaddieInfo caddieInfo = {0};
					// taskmanager.cpp:9749
					sCards card = {0};
					// taskmanager.cpp:9751
					switch (addType) {
					case 1:
						// taskmanager.cpp:9755
						packet.DecodeBuffer(&itemInfo, sizeof(sItemInfo));
						// taskmanager.cpp:9757
						switch (itemInfo.m_typeId >> 26) {
						case 5:
							// taskmanager.cpp:9763
							{
								std::list<sItemInfo>::iterator it;
								for (it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); it++) {
									// taskmanager.cpp:9765
									if (it->m_serial == itemInfo.m_serial)
										break;
								}
								// taskmanager.cpp:9771
								if (it != CSharedDoc::Instance()->m_myGearList.end())
									// taskmanager.cpp:9773
									it->m_count += itemInfo.m_count;
								// taskmanager.cpp:9775
								else
									// taskmanager.cpp:9777
									CSharedDoc::Instance()->m_myGearList.push_back(itemInfo);
							}
							break;
						case 6:
							// taskmanager.cpp:9789
							{
								std::list<sItemInfo>::iterator it;
								for (it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); it++) {
									// taskmanager.cpp:9794
									if (it->m_serial == itemInfo.m_serial)
										break;
								}
								// taskmanager.cpp:9800
								if (it != CSharedDoc::Instance()->m_itemInfoList.end())
									// taskmanager.cpp:9802
									it->m_count += itemInfo.m_count;
								// taskmanager.cpp:9804
								else
									// taskmanager.cpp:9806
									CSharedDoc::Instance()->m_itemInfoList.push_back(itemInfo);
							}
							// taskmanager.cpp:9809
							break;
						case 28:
							// taskmanager.cpp:9815
							{
								std::list<sItemInfo>::iterator it;
								for (it = CSharedDoc::Instance()->m_myExtraItemList3.begin(); it != CSharedDoc::Instance()->m_myExtraItemList3.end(); it++) {
									// taskmanager.cpp:9817
									if (it->m_serial == itemInfo.m_serial)
										break;
								}
								// taskmanager.cpp:9823
								if (it != CSharedDoc::Instance()->m_myExtraItemList3.end())
									// taskmanager.cpp:9825
									it->m_count += itemInfo.m_count;
								// taskmanager.cpp:9827
								else
									// taskmanager.cpp:9829
									CSharedDoc::Instance()->m_myExtraItemList3.push_back(itemInfo);
							}
							// taskmanager.cpp:9832
							break;
						}
						// taskmanager.cpp:9845
						break;
					case 2:
					case 3:
						// taskmanager.cpp:9850
						packet.DecodeBuffer(&itemInfo, sizeof(sItemInfo));
						// taskmanager.cpp:9852
						switch (itemInfo.m_typeId >> 26) {
						case 2:
							// taskmanager.cpp:9856
							if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(itemInfo.m_typeId)) {
								// taskmanager.cpp:9858
								CSharedDoc::Instance()->m_myItemList.push_back(itemInfo);
								// taskmanager.cpp:9859
								UccManager()->AddClothes(1, itemInfo.m_serial, itemInfo.m_typeId, itemInfo.m_uccKey, false, false);
							// taskmanager.cpp:9861
							} else {
								// taskmanager.cpp:9863
								CSharedDoc::Instance()->AddMyPartsList(itemInfo);
							}
							// taskmanager.cpp:9866
							break;
						// taskmanager.cpp:9867
						case 4: CSharedDoc::Instance()->m_myItemMap.insert(std::pair<const unsigned int, sItemInfo>(itemInfo.m_serial, itemInfo)); break;
						// taskmanager.cpp:9868
						case 5: CSharedDoc::Instance()->m_myGearList.push_back(itemInfo); break;
						// taskmanager.cpp:9869
						case 6: CSharedDoc::Instance()->m_itemInfoList.push_back(itemInfo); break;
						// taskmanager.cpp:9870
						case 28: CSharedDoc::Instance()->m_myExtraItemList3.push_back(itemInfo); break;
						}
						// taskmanager.cpp:9873
						break;
					case 4:
						// taskmanager.cpp:9877
						packet.DecodeBuffer(&caddieInfo, sizeof(sCaddieInfo));
						// taskmanager.cpp:9878
						CSharedDoc::Instance()->AddCaddie(&caddieInfo);
						// taskmanager.cpp:9880
						break;
					case 5:
						// taskmanager.cpp:9884
						if (IsLocalContent((localContentType_t)0x51)) {
							// taskmanager.cpp:9887
							packet.DecodeBuffer(&card, sizeof(sCards));
							// taskmanager.cpp:9888
							bool found = false;
							for (std::list<sCards>::iterator it = CSharedDoc::Instance()->m_myCardList.begin(); it != CSharedDoc::Instance()->m_myCardList.end(); it++) {
								// taskmanager.cpp:9890
								if (it->m_typeId == tradeItem.typeId) {
									// taskmanager.cpp:9892
									it->m_count += card.m_count;
									found = true;
									// taskmanager.cpp:9897
									break;
								}
							}
							if (!found)
								// taskmanager.cpp:9898
								CSharedDoc::Instance()->m_myCardList.push_back(card);
						}
						break;
					}
					// taskmanager.cpp:9904
					CSharedDoc::Instance()->m_myPang = pang;
				}
			}
			// taskmanager.cpp:9910
			FrWnd *pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradedlg");
			// taskmanager.cpp:9912
			FrTradeDlg *pTradeDlg = FrStaticCast<FrTradeDlg>(pWnd);
			if (pTradeDlg)
				// taskmanager.cpp:9916
				pTradeDlg->EnableSendingServer(false);
			// taskmanager.cpp:9919
			if (kind == 1) {
				// taskmanager.cpp:9921
				IFF_ITEM_COMMON *pCommon = ItemManager()->FindCommonItem(tradeItem.typeId);
				// taskmanager.cpp:9923
				if (pCommon) {
					// taskmanager.cpp:9925
					std::string msg;
					// taskmanager.cpp:9927
					if ((int)tradeItem.count > 1) {
						// taskmanager.cpp:9928
						msg = MakeStr("%s\xc0\xcc(\xb0\xa1) %d\xb0\xb3 \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", pCommon->m_name, tradeItem.count);
					// taskmanager.cpp:9929
					} else {
						// taskmanager.cpp:9931
						sItemInfo *pInfo = GetMyItemInfo(tradeItem.guid);
						// taskmanager.cpp:9932
						sUccClothes *pClothes = UccManager()->FindClothes(tradeItem.guid);
						// taskmanager.cpp:9933
						if (IsLocalContent((localContentType_t)0x57) && pInfo && (pInfo->m_uccStatus & 1) && pClothes && IsUccClothes(tradeItem.typeId))
							// taskmanager.cpp:9935
							msg = MakeStr("%s\xc0\xcc(\xb0\xa1) \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", pClothes->m_uccName);
						// taskmanager.cpp:9937
						else
							// taskmanager.cpp:9939
							msg = MakeStr("%s\xc0\xcc(\xb0\xa1) \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", pCommon->m_name);
					}
					// taskmanager.cpp:9943
					CChatMsg::Instance()->AddChatMsg(msg, 0xFFFF, true, false);
				// taskmanager.cpp:9944
				}
				// taskmanager.cpp:9946
				SetTradeResultCode(result, 0xE1, NULL);
				break;
			}
			// taskmanager.cpp:9950
			SetTradeResultCode(result, 0xEA, &tradeItem);
		}
		break;
	case 0xeb:
		// taskmanager.cpp:9957
		{
			std::string nick;
			// taskmanager.cpp:9958
			sTradeItem item;
			// taskmanager.cpp:9960
			packet.DecodeStr(nick);
			// taskmanager.cpp:9961
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:9963
			packet.DecodeBuffer(&item, sizeof(sTradeItem));
			// taskmanager.cpp:9965
			if (uid == CSharedDoc::Instance()->m_tradeUID) {
				// taskmanager.cpp:9967
				unsigned long state = packet.Decode4();
				// taskmanager.cpp:9969
				if (state == 3)
					// taskmanager.cpp:9970
					CSharedDoc::Instance()->SetStateTrade((eStateTrade)state);
				// taskmanager.cpp:9973
				FrWnd *pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradebuydlg");
				// taskmanager.cpp:9975
				FrTradeBuyDlg *pBuyDlg = FrStaticCast<FrTradeBuyDlg>(pWnd);
				if (pBuyDlg) {
					// taskmanager.cpp:9980
					if (!pBuyDlg->IsClosing()) {
						// taskmanager.cpp:9982
						sTradeItem *pBuyItem = pBuyDlg->GetBuyItem();
						// taskmanager.cpp:9984
						if (pBuyItem) {
							// taskmanager.cpp:9986
							if (pBuyItem->index == item.index
								// taskmanager.cpp:9987
								&& pBuyItem->guid == item.guid) {
								// taskmanager.cpp:9989
								if (CSharedDoc::Instance()->GetSizeTradeItem() > 1) {
									// taskmanager.cpp:9991
									if (pBuyItem->count == item.count)
										// taskmanager.cpp:9992
										CChatMsg::Instance()->AddChatMsg("\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xc5 \xbb\xf3\xc7\xb0\xc0\xba \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFFFF, true, false);
									// taskmanager.cpp:9993
									else
										// taskmanager.cpp:9994
										CChatMsg::Instance()->AddChatMsg("\xbc\xb1\xc5\xc3\xc7\xcf\xbd\xc5 \xbb\xf3\xc7\xb0\xc0\xc7 \xbc\xf6\xb7\xae\xc0\xcc \xba\xaf\xb0\xe6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFFFF, true, false);
								}
								// taskmanager.cpp:9997
								pBuyDlg->Close(true);
								// taskmanager.cpp:10000
								FrWnd *pNotifyWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("notify_okcancel");
								// taskmanager.cpp:10002
								FrForm *pNotify = FrStaticCast<FrForm>(pNotifyWnd);
								if (pNotify)
									// taskmanager.cpp:10006
									pNotify->Close(true);
							}
						}
					}
				}
				// taskmanager.cpp:10014
				FrWnd *pTradeWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName("tradedlg");
				// taskmanager.cpp:10017
				FrTradeDlg *pTradeDlg = FrStaticCast<FrTradeDlg>(pTradeWnd);
				if (pTradeDlg)
					// taskmanager.cpp:10024
					pTradeDlg->SetListTradeItem(true);
				// taskmanager.cpp:10027
				CSharedDoc::Instance()->MinusListTradeItem(item);
				// taskmanager.cpp:10029
				if (pTradeDlg)
					// taskmanager.cpp:10031
					pTradeDlg->SetListTradeItem(false);
				// taskmanager.cpp:10034
				if (CSharedDoc::Instance()->m_stateTrade == 3) {
					// taskmanager.cpp:10036
					if (strcmp(nick.c_str(), MyNick()) != 0) {
						// taskmanager.cpp:10038
						if (pTradeDlg)
							// taskmanager.cpp:10039
							pTradeDlg->Close(true);
						// taskmanager.cpp:10041
					} else if (pTradeDlg) {
						// taskmanager.cpp:10045
						pTradeDlg->ShowStaticOutOfStock(true);
						// taskmanager.cpp:10046
						pTradeDlg->EnableHideBn(false);
						// taskmanager.cpp:10048
					} else {
						// taskmanager.cpp:10050
						FrTradeDlg *pNewDlg = CreateForm<FrTradeDlg>(g_pFresh->GetManager(), this, "tradedlg", 0);
						// taskmanager.cpp:10052
						if (pNewDlg) {
							// taskmanager.cpp:10054
							pNewDlg->Open((FRESH_RESULT_CALLBACK)0, 0xC0);
							// taskmanager.cpp:10055
							pNewDlg->OpenTradeShop(CSharedDoc::Instance()->m_tradeUID);
							// taskmanager.cpp:10056
							pNewDlg->SetFadeout(false);
						}
					}
					// taskmanager.cpp:10061
					CChatMsg::Instance()->AddChatMsg("\xb8\xf0\xb5\xe7 \xbb\xf3\xc7\xb0\xc0\xcc \xc6\xc7\xb8\xc5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFFFF, true, false);
				}
			}
			// taskmanager.cpp:10065
			if (CSharedDoc::Instance()->m_stateTrade == 3)
				// taskmanager.cpp:10067
				GetActor("AvatarChat") << MsgObject(0, 0xe8, (intptr_t)nick.c_str(), 3, 0, 0, 0);
			// taskmanager.cpp:10069
		}
		break;
	case 0xec:
		// taskmanager.cpp:10074
		{
			unsigned char blocked = packet.Decode1();
			// taskmanager.cpp:10076
			SetTradeResultCode(0x0F, blocked, 0);
			// taskmanager.cpp:10078
			if (blocked) {
				// taskmanager.cpp:10079
				CChatMsg::Instance()->AddChatMsg("\xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1 \xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF37, true, false);
			} else {
				// taskmanager.cpp:10081
				CChatMsg::Instance()->AddChatMsg("\xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7\xc1\xdf\xc0\xce \xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xb3\xc0\xce\xbb\xf3\xc1\xa1\xc0\xbb \xc0\xcc\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0xFF37, true, false);
			}
		}
		break;
	case 0xed:
		// taskmanager.cpp:10087
		{
			std::string msg = "\xc1\xb6\xb0\xc7\xc0\xcc \xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.";
			// taskmanager.cpp:10089
			sRecycleItem item;
			memset(&item, 0, sizeof(item));
			// taskmanager.cpp:10091
			std::list<sRecycleItem> items;
			// taskmanager.cpp:10092
			items.clear();
			// taskmanager.cpp:10095
			short index = 0;
			int result = packet.Decode4();
			// taskmanager.cpp:10096
			switch (result) {
			case 0: {
				// taskmanager.cpp:10101
				index = packet.Decode2();
				// taskmanager.cpp:10103
				unsigned char count = packet.Decode1();
				// taskmanager.cpp:10104
				for (unsigned char i = 0; i < count; i++) {
					// taskmanager.cpp:10110
					packet.DecodeBuffer(&item, sizeof(item));
					// taskmanager.cpp:10113
					items.push_back(item);
				}
				// taskmanager.cpp:10118
				break;
			}
			case 2:
				// taskmanager.cpp:10121
				msg = "\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.";
				// taskmanager.cpp:10123
				break;
			case 3:
				// taskmanager.cpp:10126
				msg = "\xbe\xc6\xc0\xcc\xc5\xdb \xba\xb8\xc0\xaf \xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce \xc1\xb6\xc7\xd5\xc0\xbb \xc7\xd2\xbc\xf6\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.";
				// taskmanager.cpp:10167
				break;
			case 4:
				// taskmanager.cpp:10170
				msg = "\xc0\xcc\xb9\xcc \xbc\xd2\xc0\xaf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.";
				// taskmanager.cpp:10172
				break;
			default:
				// taskmanager.cpp:10176
				msg = "\xc1\xb6\xb0\xc7\xc0\xcc \xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.";
				break;
			}
			// taskmanager.cpp:10182
			FrRecycleDlg *pDlg = FrStaticCast<FrRecycleDlg>(g_pFresh->GetManager()->GetDesktop()->FindChildByName("recycledlg"));
			// taskmanager.cpp:10183
			if (pDlg) {
				// taskmanager.cpp:10187
				if (result == 0) {
					// taskmanager.cpp:10189
					pDlg->SetResultRecycleItem(true, items, index);
					// taskmanager.cpp:10190
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xE1, 0, 0, 0, 0, 0));
				} else {
					// taskmanager.cpp:10194
					pDlg->SetResultRecycleItem(false, items, index);
					// taskmanager.cpp:10195
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)msg.c_str(), 0, 0, 0, 0));
				}
			}
			// taskmanager.cpp:10198
		}
		break;
	case 0x69:
		// taskmanager.cpp:10205
		{
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:10206
			switch (result) {
			case 4:
				{
					// taskmanager.cpp:10210
					unsigned char kind = packet.Decode1();
					// taskmanager.cpp:10211
					switch (kind) {
					case 0:
						{
							// taskmanager.cpp:10215
							sCharacterInfo characterInfo;
							// taskmanager.cpp:10216
							packet.DecodeBuffer(&characterInfo, sizeof(sCharacterInfo));
							// taskmanager.cpp:10217
							CSharedDoc::Instance()->m_characterMap[characterInfo.id] = characterInfo;
							// taskmanager.cpp:10218
							CSharedDoc::Instance()->BuildMyPartTidList(characterInfo);
							// taskmanager.cpp:10219
							CTask *pTask = AfxGetTask();
							// taskmanager.cpp:10220
							if ((__rtti_obj = pTask) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CRealMyRoomTask::m_RTTI) : false)
								// taskmanager.cpp:10221
								pTask->SendMsgToMainActor(MsgObject(0, 0x26C, 0, 0, 0, 0, 0));
							// taskmanager.cpp:10222
							AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x49, CSharedDoc::Instance()->m_userInfo.guid, (intptr_t)&characterInfo, 0, 0, 0));
						}
						// taskmanager.cpp:10225
						break;
					case 2:
						{
							// taskmanager.cpp:10229
							unsigned long equipItem[10] = { 0 };
							// taskmanager.cpp:10230
							packet.DecodeBuffer(equipItem, sizeof(equipItem));
							// taskmanager.cpp:10232
							memcpy(CSharedDoc::Instance()->m_equipItem, equipItem, sizeof(equipItem));
						}
						// taskmanager.cpp:10237
						break;
					case 5:
						{
							// taskmanager.cpp:10241
							CSharedDoc::Instance()->m_myCharacterId = packet.Decode4();
							// taskmanager.cpp:10243
							CSharedDoc *pDoc = CSharedDoc::Instance();
							std::map<unsigned int, sCharacterInfo>::iterator it = pDoc->m_characterMap.find(pDoc->m_myCharacterId);
							// taskmanager.cpp:10244
							AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x49, pDoc->m_userInfo.guid, (intptr_t)&it->second, 0, 0, 0));
						}
						// taskmanager.cpp:10246
						break;
					case 4:
						{
							// taskmanager.cpp:10251
							packet.DecodeBuffer(CSharedDoc::Instance()->m_equipSkin, sizeof(CSharedDoc::Instance()->m_equipSkin));
							// taskmanager.cpp:10254
							std::map<unsigned long, sUserInfoTime>::iterator it;
							if (CSharedDoc::Instance()->FindUserInfoTimeGUID(MyGuid(false), it))
								// taskmanager.cpp:10257
								memcpy(it->second.m_userInfo.m_skinTypeIds, CSharedDoc::Instance()->m_equipSkin, sizeof(CSharedDoc::Instance()->m_equipSkin));
						}
						// taskmanager.cpp:10267
						break;
					case 8:
						{
							// taskmanager.cpp:10272
							sMascotInfo mascotInfo;
							packet.DecodeBuffer(&mascotInfo, sizeof(sMascotInfo));
							// taskmanager.cpp:10274
							std::map<unsigned int, sMascotInfo> &mascotMap = CSharedDoc::Instance()->m_myMascotMap;
							std::map<unsigned int, sMascotInfo>::iterator it = mascotMap.find(mascotInfo.id);
							// taskmanager.cpp:10275
							if (it != mascotMap.end())
								// taskmanager.cpp:10276
								mascotMap[mascotInfo.id] = mascotInfo;
						}
						// taskmanager.cpp:10278
						break;
					case 1:
					case 3:
						break;
					default:
						// taskmanager.cpp:10301
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbe\xcb \xbc\xf6 \xbe\xf8\xb4\xc2 \xb8\xed\xb7\xc9 \xc4\xda\xb5\xe5", 0, 0, 0, 0));
						// taskmanager.cpp:10305
						break;
					}
				}
				break;
			case 1:
				// taskmanager.cpp:10308
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"DB \xbf\xa1\xb7\xaf\xb7\xce \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
				// taskmanager.cpp:10309
				break;
			case 5:
				// taskmanager.cpp:10312
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xe5\xc2\xf8\xc0\xbb \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
				// taskmanager.cpp:10313
				break;
			case 0:
				// taskmanager.cpp:10316
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb \xc4\xda\xb5\xe5\xc0\xd4\xb4\xcf\xb4\xd9", 0, 0, 0, 0));
				// taskmanager.cpp:10317
				break;
			default:
				// taskmanager.cpp:10320
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbe\xcb \xbc\xf6 \xbe\xf8\xb4\xc2 \xb0\xe1\xb0\xfa \xc4\xda\xb5\xe5", 0, 0, 0, 0));
				break;
			}
			// taskmanager.cpp:10324
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		}
		// taskmanager.cpp:10326
		break;
	case 0x15f:
		// taskmanager.cpp:10331
		{
			int count = packet.Decode4();
			// taskmanager.cpp:10332
			char text[1024] = "\0";
			// taskmanager.cpp:10333
			if (count > 0) {
				// taskmanager.cpp:10335
				wsprintf(text, "%d\xb0\xb3\xc0\xc7 \xc6\xed\xc1\xf6\xb0\xa1 \xba\xb8\xb0\xfc \xb1\xe2\xb0\xa3\xc0\xbb \xc3\xca\xb0\xfa\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.\n\xb7\xce\xb1\xd7\xbe\xc6\xbf\xf4 \xc8\xa4\xc0\xba \xbc\xad\xb9\xf6 \xc0\xcc\xb5\xbf\xc0\xbb \xc7\xcf\xb1\xe2 \xc0\xfc\xbf\xa1 \xb1\xe2\xb0\xa3\xc0\xcc \xc1\xf6\xb3\xad \xb8\xde\xc0\xcf\xbf\xa1 \xc6\xf7\xc7\xd4\xb5\xc8 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb\n\xbf\xc5\xb0\xdc\xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.\n", count);
				// taskmanager.cpp:10336
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)text, 0, 0, 0, 0));
			}
		}
		break;
	case 0xef:
		{
			// taskmanager.cpp:10342
			CProjectG::Instance()->m_loginDlgFlag = true;
			// taskmanager.cpp:10344
			CSharedDoc::Instance()->m_list4B64.clear();
			// taskmanager.cpp:10345
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x68, 0, 0, 0, 0, 0));
			// taskmanager.cpp:10347
			if (((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CShopTask::m_RTTI) : false) == true) {
				// taskmanager.cpp:10349
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xC5, 0, 0, 0, 0, 0));
			}
			// taskmanager.cpp:10352
			unsigned short buffer[0x200] = {0};
			// taskmanager.cpp:10399
			if (!IsLocalContent((localContentType_t)0x53) && !IsLocalContent((localContentType_t)0x5A))
				break;
			// taskmanager.cpp:10404
			unsigned char server = 0xFF;
			unsigned short room = 0xFFFF;
			unsigned long uid = 0xFFFFFFFF;
			if (CSharedDoc::Instance()->GetDirectMoveRoomInfo(&server, &room, &uid) == true) {
				// taskmanager.cpp:10406
				if (room == 0xFFFF && IsLocalContent((localContentType_t)0x5A)) {
					// taskmanager.cpp:10408
					std::list<sChannelInfo>::iterator it = std::find(CSharedDoc::Instance()->m_channelList.begin(), CSharedDoc::Instance()->m_channelList.end(), server);
					// taskmanager.cpp:10409
					sChannelInfo channel = *it;
					// taskmanager.cpp:10411
					CSharedDoc::Instance()->m_selectedChannel = channel;
					// taskmanager.cpp:10414
					WSendPacket send(GCT_ENTER_CHANNEL);
					// taskmanager.cpp:10415
					send.Encode1(channel.id);
					// taskmanager.cpp:10416
					send.Send(SEND_TO_DEFAULT);
					// taskmanager.cpp:10418
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 1, 0, 0, 0, 0, 0));
					// taskmanager.cpp:10420
					if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_UNIT_MSN)
						&& CMessengerInfo::Instance()->m_serverListRequest == 4) {
						// taskmanager.cpp:10423
						const sChannelInfo *pSelected = &CSharedDoc::Instance()->m_selectedChannel;
						// taskmanager.cpp:10425
						CSharedDoc::Instance()->m_roomInfo.m_roomNumber = 0xFFFF;
						// taskmanager.cpp:10426
						CSharedDoc::Instance()->m_roomInfo.m_gameType = 0xFF;
						// taskmanager.cpp:10428
						sUserPosition position;
						// taskmanager.cpp:10429
						position.m_userValue = CSharedDoc::Instance()->m_gameServerUID;
						// taskmanager.cpp:10430
						position.m_channelId = pSelected->id;
						// taskmanager.cpp:10431
						wsprintfA(position.m_channelName, pSelected->name);
						position.m_roomNumber = 0xFFFF;
						position.m_value2 = 0xFFFFFFFF;
						// taskmanager.cpp:10443
						WSendPacket msnPacket(GCR_NOW_POSITION);
						// taskmanager.cpp:10447
						msnPacket.EncodeBuffer(&position, sizeof(sUserPosition));
						// taskmanager.cpp:10448
						msnPacket.Send(SEND_TO_MSN);
					}
				} else {
					// taskmanager.cpp:10456
					WSendPacket send((enumClientPacket)0xAC);
					// taskmanager.cpp:10457
					send.Encode1(server);
					// taskmanager.cpp:10458
					send.Encode2(room);
					// taskmanager.cpp:10459
					send.Send(SEND_TO_DEFAULT);
				}
			}
			// taskmanager.cpp:10463
			CSharedDoc::Instance()->ClearDirectMoveRoomInfo();
			// taskmanager.cpp:10464
			CSharedDoc::Instance()->SetGoWithModeEnd();
		}
		break;
	case 0xf6:
		// taskmanager.cpp:10471
		switch (packet.Decode1()) {
		case 0:
			{
				// taskmanager.cpp:10475
				short minutes = packet.Decode2();
				// taskmanager.cpp:10476
				CNoticeBoard::Instance()->AddNotice(MakeStr("[\xb0\xf8\xc1\xf6] \xc0\xcc \xbc\xad\xb9\xf6\xb4\xc2 %d\xba\xd0\xc8\xc4\xbf\xa1 \xc0\xda\xb5\xbf \xc1\xbe\xb7\xe1\xb5\xc9 \xbf\xb9\xc1\xa4\xc0\xd4\xb4\xcf\xb4\xd9.", minutes), 1, false);
			}
			break;
		case 1:
			{
				// taskmanager.cpp:10481
				short seconds = packet.Decode2();
				// taskmanager.cpp:10482
				CChatMsg::Instance()->AddChatMsg(MakeStr("\xbe\xcb\xb8\xb2 : \xbc\xad\xb9\xf6\xb0\xa1 %d\xc3\xca\xc8\xc4\xbf\xa1 \xc0\xda\xb5\xbf \xc1\xbe\xb7\xe1\xb5\xcb\xb4\xcf\xb4\xd9.", seconds), 0xFFFF7878, false, false);
			}
			break;
		}
		break;
	case 0x1f5:
		// taskmanager.cpp:10525
		if (CRankingInfo::Instance()->GetDlg() == NULL)
			break;
		{
			// taskmanager.cpp:10529
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:10530
			unsigned char rankType = packet.Decode1();
			// taskmanager.cpp:10531
			unsigned char subType = packet.Decode1();
			// taskmanager.cpp:10532
			unsigned char termType = packet.Decode1();
			// taskmanager.cpp:10533
			unsigned char classType = packet.Decode1();
			// taskmanager.cpp:10534
			int pageNum = packet.Decode4();
			// taskmanager.cpp:10535
			int pageTotal = packet.Decode4();
			// taskmanager.cpp:10536
			FrRankingDlg::SetRankTypes(rankType, subType, termType, classType, pageNum, pageTotal);
			// taskmanager.cpp:10538
			if (result != 0) {
				// taskmanager.cpp:10540
				CRankingInfo::Instance()->GetDlg()->SetBottomMsg("\xb4\xf5 \xc0\xcc\xbb\xf3 \xb7\xa9\xc5\xb7 \xb1\xe2\xb7\xcf\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
				// taskmanager.cpp:10542
				if (result == 2)
					break;
				// taskmanager.cpp:10544
				CRankingInfo::Instance()->GetDlg()->m_rankList.clear();
				// taskmanager.cpp:10545
				CRankingInfo::Instance()->GetDlg()->m_myInfo.uid = 0xFFFFFFFF;
				// taskmanager.cpp:10546
				CRankingInfo::Instance()->GetDlg()->ClearRankList();
				// taskmanager.cpp:10547
				CRankingInfo::Instance()->GetDlg()->UpdateMyInfo();
				break;
			}
			// taskmanager.cpp:10553
			CRankingInfo::Instance()->GetDlg()->m_rankList.clear();
			// taskmanager.cpp:10555
			unsigned short count = packet.Decode2();
			// taskmanager.cpp:10556
			for (unsigned short i = 0; i < count; i++) {
				// taskmanager.cpp:10558
				ranktype::sListInfo info;
				// taskmanager.cpp:10559
				info.uid = packet.Decode4();
				// taskmanager.cpp:10560
				info.rank = packet.Decode4();
				// taskmanager.cpp:10561
				info.prevRank = packet.Decode4();
				// taskmanager.cpp:10562
				info.record = packet.Decode4();
				// taskmanager.cpp:10563
				info.level = packet.Decode2();
				// taskmanager.cpp:10564
				info.extraByte = packet.Decode1();
				// taskmanager.cpp:10565
				info.nickname = packet.DecodeStr();
				// taskmanager.cpp:10566
				info.text30 = packet.DecodeStr();
				// taskmanager.cpp:10568
				CRankingInfo::Instance()->GetDlg()->m_rankList.push_back(info);
			}
			// taskmanager.cpp:10572
			FrRankingDlg* pDlg = CRankingInfo::Instance()->GetDlg();
			// taskmanager.cpp:10574
			unsigned char myResult = packet.Decode1();
			// taskmanager.cpp:10575
			if (myResult == 0) {
				// taskmanager.cpp:10577
				pDlg->m_myInfo.uid = packet.Decode4();
				// taskmanager.cpp:10578
				pDlg->m_myInfo.rank = packet.Decode4();
				// taskmanager.cpp:10579
				pDlg->m_myInfo.prevRank = packet.Decode4();
				// taskmanager.cpp:10580
				pDlg->m_myInfo.record = packet.Decode4();
				// taskmanager.cpp:10581
				pDlg->m_myInfo.level = packet.Decode2();
				// taskmanager.cpp:10582
				pDlg->m_myInfo.extraByte = packet.Decode1();
				// taskmanager.cpp:10583
				pDlg->m_myInfo.nickname = packet.DecodeStr();
				// taskmanager.cpp:10584
				pDlg->m_myInfo.text30 = packet.DecodeStr();
			}
			// taskmanager.cpp:10586
			else if (myResult == 1) {
				// taskmanager.cpp:10588
				pDlg->m_myInfo.uid = 0xFFFFFFFF;
			}
			// taskmanager.cpp:10591
			CRankingInfo::Instance()->GetDlg()->UpdateRankList();
			// taskmanager.cpp:10592
			CRankingInfo::Instance()->GetDlg()->UpdateMyInfo();
		}
		break;
	case 0x1f6:
		// taskmanager.cpp:10600
		if (CRankingInfo::Instance()->GetDlg() == NULL)
			break;
		{
			// taskmanager.cpp:10604
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:10605
			if (result == 2) {
				// taskmanager.cpp:10607
				CRankingInfo::Instance()->OnNotify("\xc0\xaf\xc0\xfa \xc1\xa4\xba\xb8\xb8\xa6 \xb0\xa1\xc1\xae\xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 2);
				break;
			}
			// taskmanager.cpp:10610
			if (result == 1) {
				// taskmanager.cpp:10612
				CRankingInfo::Instance()->GetDlg()->SetBottomMsg("\xbc\xf8\xc0\xa7\xb3\xbb\xbf\xa1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
				break;
			}
			// taskmanager.cpp:10617
			packet.DecodeBuffer(&CRankingInfo::Instance()->GetDlg()->m_userInfo, sizeof(ranktype::sRankUserInfo));
			// taskmanager.cpp:10619
			if (IsLocalContent((localContentType_t)0x57))
				// taskmanager.cpp:10621
				RequestUccInfo(CRankingInfo::Instance()->GetDlg()->m_userInfo);
			// taskmanager.cpp:10625
			CRankingInfo::Instance()->GetDlg()->UpdateUserInfo();
		}
		break;
	case 0x1f7:
		// taskmanager.cpp:10630
		CRankingInfo::Instance()->OnNotify("\xbc\xad\xb9\xf6\xbf\xa1\xbc\xad \xb7\xa9\xc5\xb7 \xc1\xa4\xba\xb8\xb8\xa6 \xb4\xd9\xbd\xc3 \xc1\xa4\xb7\xc4\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 2);
		break;
	case 0x1f8:
		// taskmanager.cpp:10637
		{
			FrRankingDlg* pDlg = CRankingInfo::Instance()->GetDlg();
			if (pDlg == NULL)
				break;
			// taskmanager.cpp:10643
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:10644
			if (result != 0) {
				// taskmanager.cpp:10646
				pDlg->m_myInfo.uid = 0xFFFFFFFF;
				// taskmanager.cpp:10647
				CRankingInfo::Instance()->OnNotify("\xbc\xf8\xc0\xa7\xb3\xbb\xbf\xa1 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 2);
				break;
			}
			// taskmanager.cpp:10652
			unsigned char rankType = packet.Decode1();
			// taskmanager.cpp:10653
			unsigned char subType = packet.Decode1();
			// taskmanager.cpp:10654
			unsigned char termType = packet.Decode1();
			// taskmanager.cpp:10655
			unsigned char classType = packet.Decode1();
			// taskmanager.cpp:10656
			int pageNum = packet.Decode4();
			// taskmanager.cpp:10657
			int pageTotal = packet.Decode4();
			// taskmanager.cpp:10658
			FrRankingDlg::SetRankTypes(rankType, subType, termType, classType, pageNum, pageTotal);
			// taskmanager.cpp:10661
			CRankingInfo::Instance()->GetDlg()->m_rankList.clear();
			// taskmanager.cpp:10662
			unsigned short count = packet.Decode2();
			// taskmanager.cpp:10663
			for (unsigned short i = 0; i < count; i++) {
				// taskmanager.cpp:10665
				ranktype::sListInfo info;
				// taskmanager.cpp:10666
				info.uid = packet.Decode4();
				// taskmanager.cpp:10667
				info.rank = packet.Decode4();
				// taskmanager.cpp:10668
				info.prevRank = packet.Decode4();
				// taskmanager.cpp:10669
				info.record = packet.Decode4();
				// taskmanager.cpp:10670
				info.level = packet.Decode2();
				// taskmanager.cpp:10671
				info.extraByte = packet.Decode1();
				// taskmanager.cpp:10672
				info.nickname = packet.DecodeStr();
				// taskmanager.cpp:10673
				info.text30 = packet.DecodeStr();
				// taskmanager.cpp:10675
				CRankingInfo::Instance()->GetDlg()->m_rankList.push_back(info);
			}
			// taskmanager.cpp:10677
			unsigned short selected = packet.Decode2();
			// taskmanager.cpp:10679
			CRankingInfo::Instance()->GetDlg()->UpdateRankList();
			// taskmanager.cpp:10680
			CRankingInfo::Instance()->GetDlg()->SelectRankList(selected);
		}
		break;
	case 0xfc:
		// taskmanager.cpp:10689
		{
			unsigned long result = packet.Decode4();
			// taskmanager.cpp:10692
			if (g_pPointEventDlg) {
				// taskmanager.cpp:10694
				g_pPointEventDlg->ResultMessage(result);
			}
		}
		break;
	case 0xfb:
		// taskmanager.cpp:10701
		{
			unsigned long points = packet.Decode4();
			// taskmanager.cpp:10702
			CSharedDoc::Instance()->m_userInfo.pointEventPoints = points;
		}
		break;
	case 0xfd:
		// taskmanager.cpp:10708
		{
			unsigned long count = packet.Decode4();
			// taskmanager.cpp:10710
			unsigned long totals[10] = { 0 };
			// taskmanager.cpp:10712
			packet.DecodeBuffer(totals, count * sizeof(unsigned long));
			// taskmanager.cpp:10714
			for (unsigned long i = 0; i < count; i++) {
				// taskmanager.cpp:10716
				g_CommodityStatusManager.UpdateTotalCount((unsigned short)i, totals[i]);
			}
		}
		break;
	case 0xfe:
		// taskmanager.cpp:10723
		{
			unsigned long count = packet.Decode4();
			// taskmanager.cpp:10725
			// One commodity's state as the server sends it (12 bytes).
			struct sCommodityState {
				unsigned long status;   // 0x0, CCommodityStatusManager::UpdateStatus
				unsigned long myCount;  // 0x4, CCommodityStatusManager::UpdateMyCount
				PADDING(0x8, 0xC);
			} states[10] = { 0 };
			// taskmanager.cpp:10727
			packet.DecodeBuffer(states, count * sizeof(sCommodityState));
			// taskmanager.cpp:10729
			for (unsigned long i = 0; i < count; i++) {
				// taskmanager.cpp:10731
				g_CommodityStatusManager.UpdateStatus((unsigned short)i, states[i].status);
				// taskmanager.cpp:10732
				g_CommodityStatusManager.UpdateMyCount((unsigned short)i, states[i].myCount);
			}
		}
		break;
	case 0x10b:
		// taskmanager.cpp:10740
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		{
			// taskmanager.cpp:10742
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:10746
			CGiftBoxEvent *pEvent = 0;
			CContentsDoc::Instance()->GetContainer((localContentType_t)0x1D, pEvent);
			// taskmanager.cpp:10748
			if (result == 0) {
				// taskmanager.cpp:10750
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbc\xb1\xb9\xb0\xbb\xf3\xc0\xda \xbf\xad\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:10752
				break;
			}
			if (pEvent->GetOpenedBoxTID()) {
				// taskmanager.cpp:10755
				_RemoveWareHouseItem(pEvent->GetOpenedBoxTID(), AfxGetTask()->GetActor("RealMyRoom"), 1);
				// taskmanager.cpp:10758
				struct sOpenedGift {
					unsigned long type;   // 0x0, what the box held (3 pang, 4 coupon, 5/6 an item sent to the gift box)
					unsigned long count;  // 0x4, the amount shown in the message
					unsigned long value8; // 0x8, meaning not known
					unsigned long pang;   // 0xC, the player's new pang (type 3)
					unsigned long value10; // 0x10, meaning not known
				} gift = { 0 };
				// taskmanager.cpp:10759
				packet.DecodeBuffer(&gift, sizeof(gift));
				// taskmanager.cpp:10762
				std::string msg;
				// taskmanager.cpp:10763
				switch (gift.type) {
				case 5:
					// taskmanager.cpp:10766
					_ReFreshMyRoomGift(false, "@HanBit");
					// taskmanager.cpp:10767
					if (pEvent->GetOpenedBoxTID() == 0x1A00003E) {
						// taskmanager.cpp:10769
						msg = "\xc6\xce\xbe\xdf 3\xc1\xd6\xb3\xe2 \xb1\xe2\xb3\xe4 \xbe\xc6\xc1\xee\xc5\xd8 10\xb0\xb3\xb8\xa6 \xbc\xb1\xb9\xb0\xc7\xd4\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.";
					}
					// taskmanager.cpp:10772
					break;
				case 6:
					// taskmanager.cpp:10775
					_ReFreshMyRoomGift(false, "@HanBit");
					// taskmanager.cpp:10776
					msg = "\xb8\xb6\xbd\xba\xc4\xda\xc6\xae(1\xc0\xcf)\xb4\xc2 \xbc\xb1\xb9\xb0\xc7\xd4\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.";
					// taskmanager.cpp:10777
					break;
				case 4:
					// taskmanager.cpp:10781
					msg = "\xc4\xed\xc6\xf9\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. \xc4\xed\xc6\xf9\xc7\xd4\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.";
					// taskmanager.cpp:10783
					break;
				case 3: {
					// taskmanager.cpp:10788
					char text[64] = { 0 };
					sprintf(text, "%d \xc6\xce\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9. ", gift.count);
					// taskmanager.cpp:10789
					msg = text;
					// taskmanager.cpp:10790
					CSharedDoc::Instance()->m_myPang = gift.pang;
					// taskmanager.cpp:10792
					break;
				}
				default:
					// taskmanager.cpp:10795
					msg = "\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.";
					break;
				}
				// taskmanager.cpp:10799
				pEvent->SetOpenedBoxTID(0);
				// taskmanager.cpp:10801
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x26, (intptr_t)msg.c_str(), gift.count, 0, 0, 0));
			} else {
				// taskmanager.cpp:10805
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbc\xb1\xb9\xb0\xbb\xf3\xc0\xda \xbf\xad\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
			}
		}
		break;
	case 0x107:
		// taskmanager.cpp:10812
		{
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
			// taskmanager.cpp:10814
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:10816
			if (result == 0) {
				// taskmanager.cpp:10818
				// "Failed to open the New Year's money envelope."
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbc\xbc\xb9\xe8\xb5\xb7\xba\xc0\xc5\xf5 \xbf\xad\xb1\xe2\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
				// taskmanager.cpp:10820
				break;
			}
			// taskmanager.cpp:10823
			_RemoveWareHouseItem(0x1A00003B, AfxGetTask()->GetActor("RealMyRoom"), 1);
			// taskmanager.cpp:10826
			sEnvelopeReward reward;
			memset(&reward, 0, sizeof(reward));
			// taskmanager.cpp:10828
			packet.DecodeBuffer(&reward, sizeof(reward));
			// taskmanager.cpp:10830
			IFF_ITEM_COMMON *pItem = ItemManager()->FindCommonItem(reward.m_value);
			// taskmanager.cpp:10832
			if (pItem) {
				// taskmanager.cpp:10833
				// "You received an item."
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x26, (intptr_t)"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.", reward.m_value, 0, 0, 0));
				// taskmanager.cpp:10834
				break;
			}
			// taskmanager.cpp:10835
			// "You received pang."
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x26, (intptr_t)"\xc6\xce\xc0\xbb \xb9\xde\xc0\xb8\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.", reward.m_value, 0, 0, 0));
			// taskmanager.cpp:10838
			if (reward.m_value != 0) {
				// taskmanager.cpp:10840
				CSharedDoc::Instance()->m_myPang = reward.m_pang;
			}
		}
		break;
	case 0xf7:
		{
			// taskmanager.cpp:10850
			sGameServerInfo server;
			packet.DecodeBuffer(&server, sizeof(server));
			// taskmanager.cpp:10851
			CSharedDoc::Instance()->m_curServer = server;
		}
		break;
	case 0xce:
		// taskmanager.cpp:10859
		{
			unsigned char type = packet.Decode1();
			// taskmanager.cpp:10863
			int total = packet.Decode2();
			// taskmanager.cpp:10864
			int count = packet.Decode2();
			// taskmanager.cpp:10866
			for (int i = 0; i < count; i++) {
				// taskmanager.cpp:10868
				CSharedDoc::Instance()->m_list1D0.push_back(packet.Decode4());
				// taskmanager.cpp:10870
				if (type == 2 && CSharedDoc::Instance() != NULL)
					// taskmanager.cpp:10872
					CSharedDoc::Instance()->m_list1DC.push_back(packet.Decode2());
			}
			// taskmanager.cpp:10876
			if (total != count)
				break;
			// taskmanager.cpp:10878
			CSharedDoc::Instance()->RefreshItemListFromGuidList(type);
			// taskmanager.cpp:10880
			if (type == 2 && ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CRealMyRoomTask::m_RTTI) : false))
				// taskmanager.cpp:10882
				GetActor("RealMyRoom") << MsgObject(0, 0x67, 0, 0, 0, 0, 0);
		}
		break;
	case 0xc3:
		{
			// taskmanager.cpp:10892
			unsigned char success = packet.Decode1();
			// taskmanager.cpp:10893
			unsigned long typeId = packet.Decode4();
			// taskmanager.cpp:10894
			int count = packet.Decode4();
			// taskmanager.cpp:10897
			IFF_ITEM_COMMON *pItem = ItemManager()->FindCommonItem(typeId);
			// taskmanager.cpp:10898
			if (!success) {
				// taskmanager.cpp:10900
				// "Could not delete the %s item.."
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)MakeStr("%s \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xbb\xe8\xc1\xa6\xc7\xcf\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9..", pItem ? pItem->m_name : NULL), 0, 0, 0, 0));
				// taskmanager.cpp:10901
				break;
			}
			// taskmanager.cpp:10904
			switch (typeId >> 26) {
			case 6:
				{
					// taskmanager.cpp:10908
					packet.Decode4();
					// taskmanager.cpp:10911
					std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_itemInfoList.begin();
					while (it != CSharedDoc::Instance()->m_itemInfoList.end()) {
						// taskmanager.cpp:10913
						if (it->m_typeId == typeId) {
							// taskmanager.cpp:10915
							it->m_count -= count;
							// taskmanager.cpp:10916
							if (it->m_count < 1) {
								// taskmanager.cpp:10917
								CSharedDoc::Instance()->m_itemInfoList.erase(it);
							}
							break;
						}
						// taskmanager.cpp:10922
						++it;
					}
					// taskmanager.cpp:10927
					// "%d %s items were used up."
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)MakeStr("%d\xb0\xb3\xc0\xc7 %s \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbc\xd2\xb8\xf0\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", count, pItem ? pItem->m_name : NULL), 0, 0, 0, 0));
				}
				break;
			case 0xE:
				// taskmanager.cpp:10944
				for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_mySkinList.begin(); it != CSharedDoc::Instance()->m_mySkinList.end(); ++it) {
					// taskmanager.cpp:10946
					if (it->m_typeId == typeId) {
						// taskmanager.cpp:10948
						CSharedDoc::Instance()->m_mySkinList.erase(it);
						break;
					}
				}
				// taskmanager.cpp:10953
				// "The %s item was deleted."
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)MakeStr("%s \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbb\xe8\xc1\xa6\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", pItem ? pItem->m_name : NULL), 0, 0, 0, 0));
				break;
			}
			// taskmanager.cpp:10958
			if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CRealMyRoomTask::m_RTTI) : false) {
				// taskmanager.cpp:10960
				GetActor("RealMyRoom") << MsgObject(0, 0xDD, typeId, count, 0, 0, 0);
			}
			// taskmanager.cpp:10962
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		}
		break;
	case 0x15e:
		// taskmanager.cpp:10968
		CSharedDoc::Instance()->m_mailList.clear();
		{
			// taskmanager.cpp:10969
			static sMailInfoBrief mail;
			// taskmanager.cpp:10971
			unsigned long count = packet.Decode4();
			// taskmanager.cpp:10973
			for (unsigned long i = 0; i < count; i++) {
				// taskmanager.cpp:10975
				packet.DecodeBuffer(&mail, 0xC2); // the packet's record stops short of the struct's last two bytes
				// taskmanager.cpp:10981
				CSharedDoc::Instance()->m_mailList.push_back(mail);
			}
			// taskmanager.cpp:10984
			if (CSharedDoc::Instance()->m_mailList.size() != 0) {
				// taskmanager.cpp:10986
				CSharedDoc::Instance()->m_tutorialFinish = 1;
				// taskmanager.cpp:10987
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xAA, 1, 0, 0, 0, 0));
			}
		}
		break;
	case 0x108:
		// taskmanager.cpp:10993
		if (CSharedDoc::Instance()->m_value5050 == 1) {
			// taskmanager.cpp:10996
			CSharedDoc::Instance()->m_giftList.clear();
			// taskmanager.cpp:10997
			CSharedDoc::Instance()->m_value5050 = 1;
			// taskmanager.cpp:10998
			WSendPacket send((enumClientPacket)0x92);
			// taskmanager.cpp:10999
			send.Encode4(CSharedDoc::Instance()->m_value5050);
			// taskmanager.cpp:11000
			send.Send(SEND_TO_DEFAULT);
			// taskmanager.cpp:11001
		}
		{
			// taskmanager.cpp:11003
			unsigned short count = packet.Decode2();
			// taskmanager.cpp:11004
			if (count <= 0)
				break;
			// taskmanager.cpp:11006
			CSharedDoc::Instance()->m_giftList2.clear();
			// taskmanager.cpp:11009
			for (unsigned short i = 0; i < count; i++) {
				sGiftInfo gift;
				// taskmanager.cpp:11012
				packet.DecodeBuffer(&gift, 0xFB);   // the wire record, one byte short of sizeof(sGiftInfo)
				// taskmanager.cpp:11014
				CSharedDoc::Instance()->m_chatManager.FilteringHack(gift.m_message, false);
				// taskmanager.cpp:11017
				IFF_ITEM_COMMON *pItem = ItemManager()->FindCommonItem(gift.m_typeId);
				// taskmanager.cpp:11018
				if (pItem && pItem->m_active == 1)
					// taskmanager.cpp:11020
					CSharedDoc::Instance()->m_giftList2.push_back(gift);
			}
			// taskmanager.cpp:11023
			if (CSharedDoc::Instance()->m_giftList2.size() != 0) {
				// taskmanager.cpp:11025
				CSharedDoc::Instance()->m_tutorialFinish = 1;
				// taskmanager.cpp:11026
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0xAA, 1, 0, 0, 0, 0));
			}
		}
		break;
	case 0x109:
		// taskmanager.cpp:11036
		{
			unsigned long usable = packet.Decode4();
			// taskmanager.cpp:11037
			unsigned long remained = packet.Decode4();
			// taskmanager.cpp:11038
			packet.Decode4();
			// taskmanager.cpp:11040
			CSharedDoc::Instance()->SetBsUsableBonusTimes((short)usable);
			// taskmanager.cpp:11041
			CSharedDoc::Instance()->SetBsRemainedBonusTimes((short)remained);
			// taskmanager.cpp:11043
			switch (CSharedDoc::Instance()->m_pointEventState) {
			case 1:
				// taskmanager.cpp:11047
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x200, 0, 0, 0, 0, 0));
				// taskmanager.cpp:11049
				CSharedDoc::Instance()->m_pointEventState = 0;
				break;
			case 2:
				// taskmanager.cpp:11055
				if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false)
					// taskmanager.cpp:11057
					AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x201, 0, 0, 0, 0, 0);
				// taskmanager.cpp:11060
				CSharedDoc::Instance()->m_pointEventState = 0;
				break;
			}
		}
		break;
	case 0x10e:
		// taskmanager.cpp:11068
		{
			unsigned long result = packet.Decode4();
			// taskmanager.cpp:11070
			switch (result) {
			case 0:
				// taskmanager.cpp:11075
				if (!IsAdministrator(0xFFFFFFFF)) {
					// taskmanager.cpp:11077
					WSendPacket send(0x21);
					// taskmanager.cpp:11079
					send.EncodeStr(MyNick());
					// taskmanager.cpp:11080
					send.Encode4(MyUID());
					// taskmanager.cpp:11085
					send.Send(SEND_TO_MSN);
					// taskmanager.cpp:11086
				}
				break;
			case 2:
				// taskmanager.cpp:11097
				if (IsAdministrator(0xFFFFFFFF) != 1) {
					// taskmanager.cpp:11099
					int prizeCount = packet.Decode4();
					// taskmanager.cpp:11101
					std::list<sPrize*> prizeList;
					// taskmanager.cpp:11106
					for (int i = 0; i < prizeCount; i++) {
						// taskmanager.cpp:11108
						sPrize *pPrize = new sPrize;
						// taskmanager.cpp:11109
						memset(pPrize, 0, sizeof(sPrize));
						// taskmanager.cpp:11111
						packet.DecodeBuffer(pPrize, sizeof(sPrize));
						// taskmanager.cpp:11112
						prizeList.push_back(pPrize);
					}
					// taskmanager.cpp:11115
					char *message = new char[0x200];
					// taskmanager.cpp:11116
					memset(message, 0, 0x200);
					int number = 0;
					// taskmanager.cpp:11120
					std::list<sPrize*>::iterator it;
					for (it = prizeList.begin(); it != prizeList.end(); ++it) {
						// taskmanager.cpp:11122
						sPrize *pPrize = *it;
						// taskmanager.cpp:11124
						if (pPrize == NULL)
							continue;
						// taskmanager.cpp:11129
						char itemName[0x40] = { 0, };
						char line[0x100] = { 0, };
						number++;
						// taskmanager.cpp:11131
						IFF_ITEM_COMMON *pItem = ItemManager()->FindCommonItem(pPrize->m_itemTypeId);
						// taskmanager.cpp:11132
						strcpy(itemName, pItem->m_name);
						// taskmanager.cpp:11134
						if (pPrize->m_type == 0) {
							// taskmanager.cpp:11136
							if (pPrize->m_firstRank == pPrize->m_lastRank)
								// taskmanager.cpp:11138
								sprintf(line, "%2d. %d\xb5\xee %s %d\xb0\xb3\n", number, pPrize->m_firstRank, itemName, pPrize->m_itemCount);
							// taskmanager.cpp:11140
							else
								// taskmanager.cpp:11142
								sprintf(line, "%2d. %d\xb5\xee\xba\xce\xc5\xcd %d\xb5\xee %s %d\xb0\xb3\n", number, pPrize->m_firstRank, pPrize->m_lastRank, itemName, pPrize->m_itemCount);
							// taskmanager.cpp:11145
						} else {
							// taskmanager.cpp:11147
							char awardName[0x20] = { 0, };
							// taskmanager.cpp:11149
							switch (pPrize->m_type) {
							case 1:
								// taskmanager.cpp:11152
								{ const char *name = "\xba\xa3\xbd\xba\xc6\xae \xbd\xba\xc7\xc7\xb4\xf5"; strcpy(awardName, name); }
								// taskmanager.cpp:11153
								break;
							case 2:
								// taskmanager.cpp:11156
								{ const char *name = "\xba\xa3\xbd\xba\xc6\xae \xb7\xaf\xb3\xca"; strcpy(awardName, name); }
								// taskmanager.cpp:11157
								break;
							case 3:
								// taskmanager.cpp:11160
								{ const char *name = "\xba\xa3\xbd\xba\xc6\xae \xb8\xae\xc4\xbf\xb9\xf6\xb8\xae"; strcpy(awardName, name); }
								// taskmanager.cpp:11161
								break;
							case 4:
								// taskmanager.cpp:11164
								{ const char *name = "\xc7\xe0\xbf\xee\xbb\xf3"; strcpy(awardName, name); }
								// taskmanager.cpp:11165
								break;
							case 5:
								// taskmanager.cpp:11168
								{ const char *name = "\xba\xa3\xbd\xba\xc6\xae \xb7\xd5\xc6\xdb\xc5\xcd"; strcpy(awardName, name); }
								// taskmanager.cpp:11169
								break;
							case 6:
								// taskmanager.cpp:11172
								{ const char *name = "\xba\xa3\xbd\xba\xc6\xae \xc4\xa8\xc0\xce"; strcpy(awardName, name); }
								break;
							}
							// taskmanager.cpp:11175
							sprintf(line, "%2d. %s %s %d\xb0\xb3\n", number, awardName, itemName, pPrize->m_itemCount);
						}
						// taskmanager.cpp:11177
						strcat(message, line);
					}
					// taskmanager.cpp:11181
					FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
					// taskmanager.cpp:11182
					pForm->SetTimeLimit(30.0f);
					// taskmanager.cpp:11184
					pForm->SetMessage(message, false);
					// taskmanager.cpp:11186
					pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
					// taskmanager.cpp:11188
					for (it = prizeList.begin(); it != prizeList.end(); ++it) {
						// taskmanager.cpp:11190
						if (*it != NULL) {
							// taskmanager.cpp:11192
							delete *it;
							// taskmanager.cpp:11193
							*it = NULL;
						}
					}
					// taskmanager.cpp:11196
					prizeList.clear();
					// taskmanager.cpp:11197
				}
				break;
			}
		}
		break;
	case 0x10c:
		// taskmanager.cpp:11206
		{
			sUserMatchHistory history[5];
			// taskmanager.cpp:11207
			packet.DecodeBuffer(history, sizeof(history));
			// taskmanager.cpp:11208
			CMatchHistory::Instance()->SetMatchHistory(history);
		}
		break;
	case 0x10f:
		// taskmanager.cpp:11214
		{
			unsigned long flagBlock = packet.Decode4();
			// taskmanager.cpp:11216
			CSharedDoc::Instance()->SetFlagBlock(flagBlock);
			// taskmanager.cpp:11218
			if (CSharedDoc::Instance()->m_userInfo.flagBlock & 0x10) {
				// taskmanager.cpp:11220
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				// taskmanager.cpp:11221
				pForm->SetTimeLimit(12.0f);
				// taskmanager.cpp:11222
				pForm->SetMessage("\xc7\xf6\xc0\xe7 \xc0\xfc\xb1\xa4\xc6\xc7 \xc0\xcc\xbf\xeb \xc1\xa6\xc0\xe7\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xfc\xb1\xa4\xc6\xc7\xc0\xbb \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", false);
				// taskmanager.cpp:11226
				pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
			}
		}
		break;
	case 0x10d:
		// taskmanager.cpp:11234
		{
			sLevelUpDone levelUp;
			// taskmanager.cpp:11235
			packet.DecodeBuffer(&levelUp, sizeof(levelUp));
			// taskmanager.cpp:11237
			if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI) : false) {
				// taskmanager.cpp:11239
				GetActor("Score") << MsgObject(0, 0x12D, (intptr_t)&levelUp, 0, 0, 0, 0);
			// taskmanager.cpp:11241
			} else if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false) {
				// taskmanager.cpp:11243
				GetActor("Lobby") << MsgObject(0, 0x12D, (intptr_t)&levelUp, 0, 0, 0, 0);
			// taskmanager.cpp:11247
			} else if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CRealMyRoomTask::m_RTTI) : false) {
				// taskmanager.cpp:11249
				GetActor("RealMyRoom") << MsgObject(0, 0x12D, (intptr_t)&levelUp, 0, 0, 0, 0);
			}
		}
		break;
	case 0x111:
		// taskmanager.cpp:11257
		if (IsLocalContent((localContentType_t)0x2E)) {
			// taskmanager.cpp:11259
			IActor *pLobby = AfxGetTask()->GetActor("Lobby");
			// taskmanager.cpp:11260
			if (pLobby) {
				// taskmanager.cpp:11261
				pLobby << MsgObject(0, 2, 0, 0, 0, 0, 0);
			}
			// taskmanager.cpp:11263
			CIntrusion::Instance()->PacketAnalysis(packet);
		}
		break;
	case 0x10a:
		// taskmanager.cpp:11270
		g_iRecvDisconCode = packet.Decode4();
		break;
	case 0x110:
		// taskmanager.cpp:11279
		if (packet.Decode1()) {
			// taskmanager.cpp:11281
			CSharedDoc::Instance()->m_pointEventFlag = IsLocalContent((localContentType_t)0x15) != false;
		} else {
			// taskmanager.cpp:11292
			CSharedDoc::Instance()->m_pointEventFlag = false;
		}
		break;
	case 0x118:
		// taskmanager.cpp:11300
		if (IsLocalContent((localContentType_t)0x14)) {
			// taskmanager.cpp:11302
			switch (packet.Decode1()) {
			case 1:
				// taskmanager.cpp:11305
				CSharedDoc::Instance()->m_value506C = 1;
				break;
			case 0:
			default:
				// taskmanager.cpp:11313
				CSharedDoc::Instance()->m_value506C = 0;
				break;
			case 2:
				// taskmanager.cpp:11318
				CSharedDoc::Instance()->m_value506C = 2;
				break;
			}
		} else {
			// taskmanager.cpp:11323
			CSharedDoc::Instance()->m_value506C = 0;
		}
		break;
	case 0x119: {
		// taskmanager.cpp:11472
		CWorldTourEvent *pEvent = 0;
		CContentsDoc::Instance()->GetContainer((localContentType_t)0x1E, pEvent);
		// taskmanager.cpp:11474
		if (pEvent) {
			// taskmanager.cpp:11476
			if (IsLocalContent((localContentType_t)0x1E)) {
				// taskmanager.cpp:11478
				pEvent->SetMapFlag(packet.Decode4());
				// taskmanager.cpp:11480
				if (packet.Decode1()) {
					// taskmanager.cpp:11482
					pEvent->SetGotGift(true);
				// taskmanager.cpp:11484
				} else {
					// taskmanager.cpp:11486
					pEvent->SetGotGift(false);
				}
				// taskmanager.cpp:11491
				pEvent->SetCanGift(false);
				// taskmanager.cpp:11492
				if (pEvent->GetMapFlag() == 0xFFFF && !pEvent->GetGotGift()) {
					// taskmanager.cpp:11494
					pEvent->SetCanGift(true);
				}
			} else {
				// taskmanager.cpp:11499
				pEvent->SetMapFlag(0);
				// taskmanager.cpp:11500
				pEvent->SetGotGift(false);
				// taskmanager.cpp:11501
				pEvent->SetCanGift(false);
			}
		}
		break;
	}
	case 0x11a: {
		// taskmanager.cpp:11511
		CWorldTourEvent *pEvent = 0;
		CContentsDoc::Instance()->GetContainer((localContentType_t)0x1E, pEvent);
		// taskmanager.cpp:11513
		if (pEvent) {
			// taskmanager.cpp:11515
			if (packet.Decode1() == 0) {
				// taskmanager.cpp:11519
				pEvent->SetMapFlag(0xFFFF);
				// taskmanager.cpp:11520
				pEvent->SetGotGift(false);
				// taskmanager.cpp:11521
				pEvent->SetCanGift(true);
			} else {
				// taskmanager.cpp:11526
				WSendPacket send(0x27);
				// taskmanager.cpp:11528
				send.Encode4(MyUID());
				// taskmanager.cpp:11529
				send.Encode4(CSharedDoc::Instance()->m_lastGiftReceiverUID);
				// taskmanager.cpp:11534
				send.Send(SEND_TO_MSN);
			}
		}
		break;
	}
	case 0x11f: {
		// taskmanager.cpp:11570
		CHalloweenEvent *pEvent = 0;
		CContentsDoc::Instance()->GetContainer((localContentType_t)0x21, pEvent);
		// taskmanager.cpp:11572
		if (pEvent) {
			// taskmanager.cpp:11574
			pEvent->SetMapFlag(packet.Decode4());
			// taskmanager.cpp:11575
			pEvent->SetGiftCnt(packet.Decode4());
		}
		break;
	}
	case 0x120:
		// taskmanager.cpp:11582
		if (IsLocalContent((localContentType_t)0x21)) {
			// taskmanager.cpp:11585
			CHalloweenEvent *pEvent = 0;
			CContentsDoc::Instance()->GetContainer((localContentType_t)0x21, pEvent);
			// taskmanager.cpp:11587
			if (pEvent) {
				// taskmanager.cpp:11589
				pEvent->SetGiftCnt(packet.Decode4());
				// taskmanager.cpp:11590
				pEvent->SetShowGiftDlg(true);
			}
			// taskmanager.cpp:11594
			WSendPacket send(0x21);
			// taskmanager.cpp:11596
			send.EncodeStr(MyNick());
			// taskmanager.cpp:11597
			send.Encode4(MyUID());
			// taskmanager.cpp:11602
			send.Send(SEND_TO_MSN);
		}
		break;
	case 0x122:
		// taskmanager.cpp:11617
		switch (packet.Decode4()) {
		case 0:
			// taskmanager.cpp:11656
			CTHunter::Instance()->SetUsedTikiReport(true);
			// taskmanager.cpp:11657
			if (!IsLocalContent((localContentType_t)0x58))
				break;
			// taskmanager.cpp:11660
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x6c, 1, 0, 0, 0, 0));
			// taskmanager.cpp:11663
			break;
		case 1:
			// taskmanager.cpp:11667
			AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x21b, 0, 0, 0, 0, 0));
			// taskmanager.cpp:11669
			break;
		case 2: {
			// taskmanager.cpp:11675
			FrImageNoticeDlg *pDlg = CreateForm<FrImageNoticeDlg>(g_pFresh->GetManager(), this, "imagenoticedlg", 0);
			// taskmanager.cpp:11678
			pDlg->SetCaption("\xbd\xc5\xb1\xd4\xb0\xa1\xc0\xd4 \xc0\xcc\xba\xa5\xc6\xae");
			// taskmanager.cpp:11680
#ifdef PG_PORT
			{ WRect pg_rect(-1.0f, -1.0f, 301.0f, 309.0f); pDlg->SetDlgSize(pg_rect); }
#else
			pDlg->SetDlgSize(WRect(-1.0f, -1.0f, 301.0f, 309.0f));
#endif
			// taskmanager.cpp:11685
#ifdef PG_PORT
			{ WRect pg_rect(0.0f, 0.0f, 256.0f, 220.0f); pDlg->SetBackGroundImage(0, 0, pg_rect, "event_new_02_a", true); }
#else
			pDlg->SetBackGroundImage(0, 0, WRect(0.0f, 0.0f, 256.0f, 220.0f), "event_new_02_a", true);
#endif
			// taskmanager.cpp:11691
#ifdef PG_PORT
			{ WRect pg_rect(256.0f, 0.0f, 314.0f, 247.0f); pDlg->SetBackGroundImage(2, 0, pg_rect, "event_new_02_b", true); }
#else
			pDlg->SetBackGroundImage(2, 0, WRect(256.0f, 0.0f, 314.0f, 247.0f), "event_new_02_b", true);
#endif
			// taskmanager.cpp:11694
#ifdef PG_PORT
			{ WRect pg_rect(114.0f, 247.0f, 0.0f, 0.0f); pDlg->SetOkButton(pg_rect, true); }
#else
			pDlg->SetOkButton(WRect(114.0f, 247.0f, 0.0f, 0.0f), true);
#endif
			// taskmanager.cpp:11696
			pDlg->SetDesc("8\xbf\xf9 31\xc0\xcf \xc6\xce\xbe\xdf \xc8\xa8\xc6\xe4\xc0\xcc\xc1\xf6\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.");
			// taskmanager.cpp:11698
			pDlg->Open((FRESH_RESULT_CALLBACK)0, 0);
			break;
		}
		case 3: {
			// taskmanager.cpp:11707
			FrImageNoticeDlg *pDlg = CreateForm<FrImageNoticeDlg>(g_pFresh->GetManager(), this, "imagenoticedlg", 0);
			// taskmanager.cpp:11710
			pDlg->SetCaption("\xbd\xc5\xb1\xd4\xb0\xa1\xc0\xd4 \xc0\xcc\xba\xa5\xc6\xae");
			// taskmanager.cpp:11712
#ifdef PG_PORT
			{ WRect pg_rect(-1.0f, -1.0f, 342.0f, 334.0f); pDlg->SetDlgSize(pg_rect); }
#else
			pDlg->SetDlgSize(WRect(-1.0f, -1.0f, 342.0f, 334.0f));
#endif
			// taskmanager.cpp:11717
#ifdef PG_PORT
			{ WRect pg_rect(0.0f, 0.0f, 256.0f, 256.0f); pDlg->SetBackGroundImage(0, 0, pg_rect, "event_new_03_a", true); }
#else
			pDlg->SetBackGroundImage(0, 0, WRect(0.0f, 0.0f, 256.0f, 256.0f), "event_new_03_a", true);
#endif
			// taskmanager.cpp:11723
#ifdef PG_PORT
			{ WRect pg_rect(256.0f, 0.0f, 333.0f, 256.0f); pDlg->SetBackGroundImage(2, 0, pg_rect, "event_new_03_b", true); }
#else
			pDlg->SetBackGroundImage(2, 0, WRect(256.0f, 0.0f, 333.0f, 256.0f), "event_new_03_b", true);
#endif
			// taskmanager.cpp:11726
#ifdef PG_PORT
			{ WRect pg_rect(127.0f, 273.0f, 0.0f, 0.0f); pDlg->SetOkButton(pg_rect, true); }
#else
			pDlg->SetOkButton(WRect(127.0f, 273.0f, 0.0f, 0.0f), true);
#endif
			// taskmanager.cpp:11728
			pDlg->SetDesc("8\xbf\xf9 31\xc0\xcf \xc6\xce\xbe\xdf \xc8\xa8\xc6\xe4\xc0\xcc\xc1\xf6\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.");
			// taskmanager.cpp:11730
			pDlg->Open((FRESH_RESULT_CALLBACK)0, 0);
			{
				// taskmanager.cpp:11733
				WSendPacket send(0x21);
				// taskmanager.cpp:11735
				send.EncodeStr(MyNick());
				// taskmanager.cpp:11736
				send.Encode4(MyUID());
				// taskmanager.cpp:11741
			}
			break;
		}
		case 4: {
			// taskmanager.cpp:11747
			unsigned long guid = packet.Decode4();
			// taskmanager.cpp:11748
			if (guid == CSharedDoc::Instance()->m_userInfo.guid) {
				// taskmanager.cpp:11750
				FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
				// taskmanager.cpp:11751
				pForm->SetTimeLimit(2.0f);
				// taskmanager.cpp:11752
				pForm->SetMessage("\xc7\xf6\xc0\xe7 \xc0\xe5\xc2\xf8\xb5\xc8 \xbe\xc6\xc1\xee\xc5\xd8\xc0\xbb \xb8\xf0\xb5\xce \xbc\xd2\xb8\xf0\xc7\xcf\xbf\xb4\xc0\xb8\xb9\xc7\xb7\xce, \xc0\xe5\xc2\xf8 \xbe\xc6\xc1\xee\xc5\xd8\xc0\xbb \xb1\xe2\xba\xbb \xbe\xc6\xc1\xee\xc5\xd8\xc0\xb8\xb7\xce \xb1\xb3\xc3\xbc \xc7\xd5\xb4\xcf\xb4\xd9.", false);
				// taskmanager.cpp:11753
				pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
				// taskmanager.cpp:11755
				CSharedDoc::Instance()->m_myBallId = 0x14000000;
			}
			// taskmanager.cpp:11758
			for (int i = 0; i < 4; i++) {
				// taskmanager.cpp:11761
				if (guid == CSharedDoc::Instance()->m_roomPlayers[i].m_guid) {
					// taskmanager.cpp:11764
					CSharedDoc::Instance()->m_roomPlayers[i].m_ballId = 0x14000000;
					AfxGetTask()->GetActor("ball") << MsgObject(0, 0x4d, 0x14000000, i, 0, 0, 0);
				}
			}
			break;
		}
		case 5: {
			// taskmanager.cpp:11772
			std::string nick = packet.DecodeStr();
			// taskmanager.cpp:11773
			char text[0x80] = { 0, };
			// taskmanager.cpp:11774
			sprintf(text, "\xc7\xf6\xc0\xe7 \xc7\xc3\xb7\xb9\xc0\xcc\xc1\xdf\xc0\xce \xb0\xd4\xc0\xd3\xbf\xa1\xbc\xad '%s'\xb4\xd4\xbf\xa1 \xc0\xc7\xc7\xd8 \xc6\xf8\xc1\xd7\xc0\xcc \xc5\xcd\xc1\xb3\xbd\xc0\xb4\xcf\xb4\xd9.", nick.c_str());
			// taskmanager.cpp:11775
			CChatMsg::Instance()->AddChatMsg(text, 0xFFFF0000, true, false);
			// taskmanager.cpp:11776
			break;
		}
		case 0xb: {
			// taskmanager.cpp:11780
			std::string msg = packet.DecodeStr();
			// taskmanager.cpp:11781
			CMessengerInfo::Instance()->Notify2(MakeStr("%s", msg.c_str()));
			// taskmanager.cpp:11782
			break;
		}
		case 8:
			// taskmanager.cpp:11786
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x122, 1, 0, 0, 0);
			// taskmanager.cpp:11788
			break;
		case 9:
			// taskmanager.cpp:11791
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0x228, 0x122, 0, 0, 0, 0);
			// taskmanager.cpp:11808
			break;
		}
		break;
	case 0x129:
		// taskmanager.cpp:11863
		if (!IsLocalContent((localContentType_t)0x58)) {
			// taskmanager.cpp:11865
			return 0;
		}
		{
			// taskmanager.cpp:11869
			unsigned char mode = packet.Decode1();
			// taskmanager.cpp:11871
			sTreasureItem item;
			// taskmanager.cpp:11872
			switch (mode) {
			case 0:
				// taskmanager.cpp:11876
				packet.DecodeBuffer(&item, sizeof(item));
				// taskmanager.cpp:11878
				CTHunter::Instance()->SetTHunterGauge(item.type, item.gauge);
				break;
			case 1: {
				// taskmanager.cpp:11883
				unsigned char count = packet.Decode1();
				// taskmanager.cpp:11884
				for (int i = 0; i < count; i++) {
					// taskmanager.cpp:11886
					packet.DecodeBuffer(&item, sizeof(item));
					// taskmanager.cpp:11888
					CTHunter::Instance()->SetTHunterGauge(item.type, item.gauge);
				}
				break;
			}
			}
		}
		break;
	case 0x12a:
		// taskmanager.cpp:11898
		if (!IsLocalContent((localContentType_t)0x58)) {
			// taskmanager.cpp:11900
			return 0;
		}
		// taskmanager.cpp:11902
		switch (GetGameType()) {
		case 0:
		case 1:
		case 4:
		case 5: {
			// taskmanager.cpp:11910
			unsigned long point = packet.Decode4();
			// taskmanager.cpp:11911
			CTHunter::Instance()->SetTreasurePoint(point);
			// taskmanager.cpp:11913
			GetActor("Score") << MsgObject(0, 0x12F, 4, 0, 0, 0, 0);
			break;
		}
		}
		break;
	case 0x12b: {
		// taskmanager.cpp:11925
		std::string strTemp; // constructed and destroyed unused in the original
		// taskmanager.cpp:11927
		std::vector<sTreasureHunt> repository;
		// taskmanager.cpp:11928
		sTreasureHunt hunt;
		// taskmanager.cpp:11930
		unsigned char count = packet.Decode1();
		// taskmanager.cpp:11935
		repository.reserve(count);
		// taskmanager.cpp:11938
		for (unsigned char i = 0; i < count; i++) {
			// taskmanager.cpp:11940
			packet.DecodeBuffer(&hunt, sizeof(sTreasureHunt));
			// taskmanager.cpp:11942
			repository.push_back(hunt);
		}
		// taskmanager.cpp:11944
		CTHunter::Instance()->ReceiveRepository(repository);
		// taskmanager.cpp:11946
		CTHunter::Instance()->SetReceiveGiftPacket(true);
		// taskmanager.cpp:11948
		CSharedDoc::Instance()->SetAppGiftDownloadState(CSharedDoc::APP_GIFT_DOWNLOAD_READY);
		break;
	}
	case 0x12c:
		// taskmanager.cpp:11954
		if (!IsLocalContent((localContentType_t)0x67))
			break;
		{
			// taskmanager.cpp:11959
			std::vector<sTreasureGift> gifts;
			// taskmanager.cpp:11960
			sTreasureGift gift;
			// taskmanager.cpp:11961
			unsigned char count = packet.Decode1();
			// taskmanager.cpp:11963
			for (unsigned char i = 0; i < count; i++) {
				// taskmanager.cpp:11965
				packet.DecodeBuffer(&gift, sizeof(gift));
				// taskmanager.cpp:11966
				gifts.push_back(gift);
			}
			// taskmanager.cpp:11968
			CTHunter::Instance()->ReceiveAllGift(gifts);
			// taskmanager.cpp:11969
			CTHunter::Instance()->UpdateItemList("");
			// taskmanager.cpp:11970
		}
		break;
	case 0x138:
		// taskmanager.cpp:11975
		if (IsLocalContent((localContentType_t)0x59)) {
			// taskmanager.cpp:11978
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:11980
			if (result == 0) {
				// taskmanager.cpp:11986
				AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x23, (intptr_t)"\xb0\xfc\xbd\xc9\xbb\xe7 \xbe\xf7\xb5\xa5\xc0\xcc\xc6\xae\xb8\xa6 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
			}
		}
		break;
	case 0x126:
		// taskmanager.cpp:11994, the self-design (UCC) replies
		{
			unsigned char sub = packet.Decode1();
			// taskmanager.cpp:11996
			switch (sub) {
			case 0:
				// taskmanager.cpp:12009, the upload result
				{
					unsigned char result = packet.Decode1();
					// taskmanager.cpp:12010
					unsigned long serial = packet.Decode4();
					// taskmanager.cpp:12011
					unsigned long typeId = packet.Decode4();
					char uccKey[9];
					char uccName[0x29];
					// taskmanager.cpp:12014
					strncpy(uccKey, packet.DecodeStr().c_str(), 9);
					// taskmanager.cpp:12015
					strncpy(uccName, packet.DecodeStr().c_str(), 0x29);
					// taskmanager.cpp:12018
					GetActor("RealMyRoom") << MsgObject(0, 0x214, result, (intptr_t)&serial, (intptr_t)&typeId, (intptr_t)uccKey, (intptr_t)uccName);
				}
				break;
			case 1:
				// taskmanager.cpp:12024, a self-designed item to add or refresh
				{
					sItemInfo item;
					// taskmanager.cpp:12027
					packet.Decode4();
					// taskmanager.cpp:12028
					char uccKey[9];
					strncpy(uccKey, packet.DecodeStr().c_str(), 9);
					// taskmanager.cpp:12029
					unsigned char slot = packet.Decode1();
					// taskmanager.cpp:12030
					packet.DecodeBuffer(&item, sizeof(sItemInfo));
					// taskmanager.cpp:12033
					if (slot == 1) {
						// taskmanager.cpp:12036
						CSharedDoc *pDoc = CSharedDoc::Instance();
						bool exists = false;
						// taskmanager.cpp:12038
						for (std::list<sItemInfo>::iterator it = pDoc->m_myItemList.begin(); it != pDoc->m_myItemList.end(); it++) {
							// taskmanager.cpp:12042
							if (it->m_typeId == item.m_typeId && strcmp(it->m_uccKey, item.m_uccKey) == 0 && it->m_uccSeq == item.m_uccSeq) {
								exists = true;
								break;
							}
						}
						if (!exists) {
							// taskmanager.cpp:12051
							pDoc->m_myItemList.push_back(item);
							// taskmanager.cpp:12055
							GetActor("RealMyRoom") << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);
						}
					}
					// taskmanager.cpp:12061
					if (item.m_uccStatus & 1) {
						// taskmanager.cpp:12064
						sUccClothes *pClothes = UccManager()->FindClothes(item.m_serial);
						// taskmanager.cpp:12065
						if (pClothes) {
							// taskmanager.cpp:12067
							pClothes->m_period[0] = item.m_reserved18[0]; pClothes->m_period[1] = item.m_reserved18[1]; pClothes->m_period[2] = item.m_reserved18[2]; pClothes->m_period[3] = item.m_reserved18[3];
							// taskmanager.cpp:12068
							size_t nameLen = strlen(item.m_uccName);
							if (nameLen)
								// taskmanager.cpp:12070
								strcpy(pClothes->m_uccName, item.m_uccName);
						} else {
							// taskmanager.cpp:12075
							sUccClothes clothes;
							// taskmanager.cpp:12076
							clothes.m_serial = item.m_serial;
							// taskmanager.cpp:12077
							clothes.m_period[0] = item.m_reserved18[0]; clothes.m_period[1] = item.m_reserved18[1]; clothes.m_period[2] = item.m_reserved18[2]; clothes.m_period[3] = item.m_reserved18[3];
							clothes.m_downloadCount = 0;
							clothes.m_pIcon = 0;
							// taskmanager.cpp:12080
							size_t nameLen = strlen(item.m_uccName);
							if (nameLen)
								// taskmanager.cpp:12082
								strcpy(clothes.m_uccName, item.m_uccName);
							// taskmanager.cpp:12084
							clothes.m_typeId = item.m_typeId;
							// taskmanager.cpp:12085
							strncpy(clothes.m_uccKey, item.m_uccKey, 9);
							// taskmanager.cpp:12087
							UccManager()->AddClothes(slot, clothes, false, false);
						}
					}
				}
				break;
			case 2:
				// taskmanager.cpp:12095, the copy result
				{
					unsigned long srcTypeId = packet.Decode4();
					// taskmanager.cpp:12097
					char srcUccKey[9];
					strncpy(srcUccKey, packet.DecodeStr().c_str(), 9);
					// taskmanager.cpp:12098
					unsigned short srcSeq = packet.Decode2();
					// taskmanager.cpp:12100
					unsigned long oldSerial = packet.Decode4();
					// taskmanager.cpp:12102
					unsigned long newSerial = packet.Decode4();
					// taskmanager.cpp:12103
					unsigned long newTypeId = packet.Decode4();
					// taskmanager.cpp:12105
					char newUccKey[9];
					strncpy(newUccKey, packet.DecodeStr().c_str(), 9);
					// taskmanager.cpp:12106
					unsigned short newSeq = packet.Decode2();
					// taskmanager.cpp:12108
					unsigned char result = packet.Decode1();
					// taskmanager.cpp:12110
					if (result == 1) {
						// taskmanager.cpp:12113
						UccManager()->FindClothes(srcTypeId, srcUccKey);
						// taskmanager.cpp:12114
						sUccClothes clothes;
						// taskmanager.cpp:12115
						clothes.m_typeId = newTypeId;
						// taskmanager.cpp:12116
						clothes.m_serial = newSerial;
						// taskmanager.cpp:12117
						strncpy(clothes.m_uccKey, newUccKey, 9);
						// taskmanager.cpp:12118
						UccManager()->AddClothes(1, clothes, false, false);
						// taskmanager.cpp:12122
						FrUccCopyCompletedDlg *pDlg = CreateForm<FrUccCopyCompletedDlg>(g_pFresh->GetManager(), this, "ucc_copy_completed", 0);
						// taskmanager.cpp:12125
						pDlg->Init(srcTypeId, srcUccKey);
						// taskmanager.cpp:12126
						pDlg->Open((FRESH_RESULT_CALLBACK)0, WPoint(453.0f, 189.0f), 1);
						// taskmanager.cpp:12130
						sItemInfo *pSrcItem = GetMyItemInfo(srcTypeId, srcUccKey, srcSeq);
						// taskmanager.cpp:12133
						CSharedDoc *pDoc = CSharedDoc::Instance();
						// taskmanager.cpp:12134
						for (std::list<sItemInfo>::iterator it = pDoc->m_myItemList.begin(); it != pDoc->m_myItemList.end(); it++) {
							// taskmanager.cpp:12137
							if (it->m_serial == oldSerial) {
								// taskmanager.cpp:12139
								it->m_serial = newSerial;
								// taskmanager.cpp:12140
								it->m_typeId = newTypeId;
								// taskmanager.cpp:12141
								strncpy(it->m_uccKey, newUccKey, 9);
								// taskmanager.cpp:12142
								it->m_uccSeq = newSeq;
								// taskmanager.cpp:12144
								it->m_uccStatus &= ~2;
								// taskmanager.cpp:12145
								it->m_uccStatus |= 1;
								// taskmanager.cpp:12146, the copier's nickname
								strncpy(it->m_reserved5E, CSharedDoc::Instance()->m_userInfo.nickname, 0x16);
								// taskmanager.cpp:12147
								strncpy(it->m_uccName, pSrcItem ? pSrcItem->m_uccName : "\xc1\xa4\xba\xb8\xb8\xa6 \xb9\xde\xbe\xc6\xbf\xc0\xb4\xc2 \xc1\xdf...", 0x29);
								break;
							}
						}
						// taskmanager.cpp:12153
						AfxGetTask()->GetActor("RealMyRoom") << MsgObject(0, 0xE1, 0, 0, 0, 0, 0);
					} else {
						// taskmanager.cpp:12157
						AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x23, (intptr_t)"\xbc\xbf\xc7\xc1\xb5\xf0\xc0\xda\xc0\xce \xc0\xc7\xbb\xf3 \xba\xb9\xc1\xa6\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0));
					}
				}
				break;
			case 3:
				// taskmanager.cpp:12164, the temporary save result
				{
					unsigned long typeId = packet.Decode4();
					// taskmanager.cpp:12166
					char uccKey[9];
					memset(uccKey, 0, sizeof(uccKey));
					// taskmanager.cpp:12167
					strncpy(uccKey, packet.DecodeStr().c_str(), 9);
					// taskmanager.cpp:12168
					unsigned char result = packet.Decode1();
					// taskmanager.cpp:12170
					if (result == 1) {
						// taskmanager.cpp:12173
						CSharedDoc *pDoc = CSharedDoc::Instance();
						// taskmanager.cpp:12174
						for (std::list<sItemInfo>::iterator it = pDoc->m_myItemList.begin(); it != pDoc->m_myItemList.end(); it++) {
							// taskmanager.cpp:12177
							if (it->m_typeId == typeId && strcmpi(it->m_uccKey, uccKey) == 0) {
								// taskmanager.cpp:12179
								it->m_uccStatus |= 2;
								break;
							}
						}
					}
					// taskmanager.cpp:12185
					GetActor("RealMyRoom") << MsgObject(0, 0x215, result, (intptr_t)&typeId, (intptr_t)uccKey, 0, 0);
				}
				break;
			}
		}
		break;
	case 0x13c:
		// taskmanager.cpp:12194
		{
			CMatchingSystem *pMatching = CMatchingSystem::Instance();
			// taskmanager.cpp:12195
			if (pMatching) {
				// taskmanager.cpp:12197
				unsigned char count = packet.Decode1();
				// taskmanager.cpp:12199
				if (count > 0) {
					// taskmanager.cpp:12202
					for (int i = 0; i < count; i++) {
						sIntrest intrest;
						// taskmanager.cpp:12204
						packet.DecodeBuffer(&intrest, sizeof(intrest));
						// taskmanager.cpp:12206
						pMatching->SetDisplayIntrest(intrest);
					}
				}
			}
		}
		break;
	case 0x14f:
		// taskmanager.cpp:12214
		{
			packet.Decode1();
			// taskmanager.cpp:12215
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:12219
			std::map<unsigned long, sUserInfoTime>::iterator it;
			int found = CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it);
			// taskmanager.cpp:12220
			if (!found) {
				// taskmanager.cpp:12223
				sUserInfoTime info;
				// taskmanager.cpp:12224
				memset(&info, 0, sizeof(info));
				// taskmanager.cpp:12226
				if (CSharedDoc::Instance()->InsertUserInfoTime(uid, info))
					// taskmanager.cpp:12227
					found = CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it);
			}
			// taskmanager.cpp:12230
			if (found) {
				// taskmanager.cpp:12232
				unsigned short slot = packet.Decode2();
				// taskmanager.cpp:12234
				packet.DecodeBuffer(it->second.m_userInfo.m_nickname, 0x10D);
				// taskmanager.cpp:12236
				it->second.m_userInfo.m_slot = slot;
				// taskmanager.cpp:12237
				it->second.m_guildMyPercent = packet.Decode4();
			}
		}
		break;
	case 0x150:
		// taskmanager.cpp:12244
		{
			unsigned char mode = packet.Decode1();
			// taskmanager.cpp:12245
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:12249
			std::map<unsigned long, sUserInfoTime>::iterator it;
			int found = CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it);
			// taskmanager.cpp:12250
			if (!found) {
				// taskmanager.cpp:12253
				sUserInfoTime info;
				// taskmanager.cpp:12254
				memset(&info, 0, sizeof(info));
				// taskmanager.cpp:12256
				if (CSharedDoc::Instance()->InsertUserInfoTime(uid, info)) {
					// taskmanager.cpp:12257
					found = CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it);
				}
			}
			// taskmanager.cpp:12261
			sPangYaUserStatistics statistics;
			packet.DecodeBuffer(&statistics, sizeof(statistics));
			// taskmanager.cpp:12263
			if (CSharedDoc::Instance()->m_myUID == uid) {
				// taskmanager.cpp:12265
				if (mode == 0) {
					// taskmanager.cpp:12268
					CSharedDoc::Instance()->m_oldSeasonStatistics = statistics;
				} else {
					// taskmanager.cpp:12269
					CSharedDoc::Instance()->m_myStatistics = statistics;
				}
			}
			// taskmanager.cpp:12273
			if (found) {
				// taskmanager.cpp:12275
				if (mode == 5) {
					// taskmanager.cpp:12276
					it->second.m_userInfo.m_statistics = statistics;
				} else if (mode == 0) {
					// taskmanager.cpp:12278
					it->second.m_seasonStatistics = statistics;
				}
			}
		}
		break;
	case 0x14e:
		// taskmanager.cpp:12294
		{
			packet.Decode1();
			// taskmanager.cpp:12295
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:12299
			std::map<unsigned long, sUserInfoTime>::iterator it;
			int found = CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it);
			// taskmanager.cpp:12301
			if (found) {
				// taskmanager.cpp:12303
				packet.DecodeBuffer(&it->second.m_userInfo.m_equipHead, 0x6C);
			}
		}
		break;
	case 0x155:
		// taskmanager.cpp:12309
		{
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:12313
			std::map<unsigned long, sUserInfoTime>::iterator it;
			int found = CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it);
			// taskmanager.cpp:12314
			if (found) {
				// taskmanager.cpp:12317
				GUILD_INFO guildInfo;
				// taskmanager.cpp:12318
				packet.DecodeBuffer(&guildInfo, 0x129);
				// taskmanager.cpp:12320
				it->second.m_guildPang = guildInfo.value19;
				// taskmanager.cpp:12321
				if (uid == MyUID()) {
					// taskmanager.cpp:12323
					it->second.m_guildMyPercent = guildInfo.value1D;
				}
				// taskmanager.cpp:12325
				strcpy(it->second.m_userInfo.m_guildMark, guildInfo.emblem);
				// taskmanager.cpp:12326
				strcpy(it->second.m_userInfo.m_guildName, guildInfo.name);
			}
		}
		break;
	case 0x153:
		// taskmanager.cpp:12344
		{
			unsigned char season = packet.Decode1();
			// taskmanager.cpp:12345
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:12346
			unsigned short count = packet.Decode2();
			// taskmanager.cpp:12350
			std::map<unsigned long, sUserInfoTime>::iterator it;
			int found = CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it);
			// taskmanager.cpp:12351
			if (found) {
				sGuildTrophy **ppArray = NULL;
				int *pCount = NULL;
				// taskmanager.cpp:12356
				if (season == 5) {
					// taskmanager.cpp:12358
					ppArray = &it->second.m_pArray2;
					// taskmanager.cpp:12359
					pCount = &it->second.m_guildTrophyCount;
				}
				// taskmanager.cpp:12361
				else if (season == 0) {
					// taskmanager.cpp:12363
					ppArray = &it->second.m_pArray4;
					// taskmanager.cpp:12364
					pCount = &it->second.m_seasonGuildTrophyCount;
				}
				// taskmanager.cpp:12368
				if (ppArray) {
					// taskmanager.cpp:12370
					if (*ppArray) { delete[] *ppArray; *ppArray = NULL; }
					// taskmanager.cpp:12377
					if (count) {
						// taskmanager.cpp:12379
						sGuildTrophy *pArray = new sGuildTrophy[count];
						// taskmanager.cpp:12381
						for (int i = 0; i < count; i++) {
							// taskmanager.cpp:12383
							packet.DecodeBuffer(&pArray[i], 4);
						}
						// taskmanager.cpp:12389 (tail merged with case 0x152's)
						*ppArray = pArray;
					}
					// taskmanager.cpp:12393 (tail merged with case 0x152's)
					*pCount = count;
				}
			}
		}
		break;
	case 0x151:
		// taskmanager.cpp:12399
		{
			unsigned char mode = packet.Decode1();
			// taskmanager.cpp:12400
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:12404
			std::map<unsigned long, sUserInfoTime>::iterator it;
			int found = CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it);
			// taskmanager.cpp:12405
			if (found) {
				// taskmanager.cpp:12408
				sTrophyStatistics trophy;
				packet.DecodeBuffer(&trophy, sizeof(trophy));
				// taskmanager.cpp:12410
				if (mode == 5) {
					// taskmanager.cpp:12412
					it->second.m_userInfo.m_trophy = trophy;
				} else if (mode == 0) {
					// taskmanager.cpp:12416
					it->second.m_trophyStatistics = trophy;
				}
			}
		}
		break;
	case 0x152:
		// taskmanager.cpp:12425
		{
			unsigned char season = packet.Decode1();
			// taskmanager.cpp:12426
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:12427
			unsigned short count = packet.Decode2();
			// taskmanager.cpp:12431
			std::map<unsigned long, sUserInfoTime>::iterator it;
			int found = CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it);
			// taskmanager.cpp:12432
			if (found) {
				sSpecialTrophy **ppArray = NULL;
				int *pCount = NULL;
				// taskmanager.cpp:12437
				if (season == 5) {
					// taskmanager.cpp:12439
					ppArray = &it->second.m_pArray1;
					// taskmanager.cpp:12440
					pCount = &it->second.m_specialTrophyCount;
				}
				// taskmanager.cpp:12442
				else if (season == 0) {
					// taskmanager.cpp:12444
					ppArray = &it->second.m_pArray3;
					// taskmanager.cpp:12445
					pCount = &it->second.m_seasonSpecialTrophyCount;
				}
				// taskmanager.cpp:12448
				if (ppArray) {
					// taskmanager.cpp:12450
					if (*ppArray) { delete[] *ppArray; *ppArray = NULL; }
					// taskmanager.cpp:12457
					if (count) {
						// taskmanager.cpp:12459
						sSpecialTrophy *pArray = new sSpecialTrophy[count];
						// taskmanager.cpp:12462
						for (int i = 0; i < count; i++) {
							sSpecialTrophy trophy;
							// taskmanager.cpp:12464
							packet.DecodeBuffer(&trophy, 0xC);
							// taskmanager.cpp:12465
							pArray[i] = trophy;
						}
						// taskmanager.cpp:12468
						*ppArray = pArray;
					}
					// taskmanager.cpp:12472
					*pCount = count;
				}
			}
		}
		break;
	case 0x154:
		// taskmanager.cpp:12480
		{
			unsigned char mode = packet.Decode1();
			// taskmanager.cpp:12481
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:12482
			unsigned short count = packet.Decode2();
			// taskmanager.cpp:12485
			if (count == 0) {
				break;
			}
			// taskmanager.cpp:12488
			std::map<unsigned long, sUserInfoTime>::iterator it;
			if (CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it)) {
				// taskmanager.cpp:12490
				sMapStatistics *dest = 0;
				// taskmanager.cpp:12491
				sMapStatistics maps[20];
				// taskmanager.cpp:12493
				int size = count * sizeof(sMapStatistics);
				// taskmanager.cpp:12494
				packet.DecodeBuffer(maps, size);
				// taskmanager.cpp:12498
				if (mode == 5) dest = it->second.m_userInfo.m_mapStatistics;
				// taskmanager.cpp:12499
				else if (mode == 0) dest = it->second.m_mapStatistics;
				// taskmanager.cpp:12500
				else if (mode == 10) dest = it->second.m_classicMapStatistics;
				// taskmanager.cpp:12501
				else if (mode == 0x33) dest = it->second.m_userInfo.m_mapStatistics2;
				// taskmanager.cpp:12503
				if (dest) {
					// taskmanager.cpp:12504
					memcpy(dest, maps, size);
				}
			}
		}
		break;
	case 0x156:
		// taskmanager.cpp:12511
		{
			unsigned long uid = packet.Decode4();
			// taskmanager.cpp:12515
			std::map<unsigned long, sUserInfoTime>::iterator it;
			int found = CSharedDoc::Instance()->FindUserInfoTimeUID(uid, it);
			// taskmanager.cpp:12516
			if (found) {
				// taskmanager.cpp:12518
				sCharacterInfo info;
				// taskmanager.cpp:12519
				packet.DecodeBuffer(&info, sizeof(info));
				// taskmanager.cpp:12520
				it->second.m_userInfo.m_characterInfo = info;
				// taskmanager.cpp:12522
				CSharedDoc::Instance()->SetUserInfoMod(0);
				// taskmanager.cpp:12525
				if (IsLocalContent((localContentType_t)0x57)) {
					// taskmanager.cpp:12527
					RequestUccInfo(info);
				}
			}
		}
		break;
	case 0x14b:
		// taskmanager.cpp:12541
		{
			unsigned char type = packet.Decode1();
			// taskmanager.cpp:12543
			switch (type) {
			case 0:
				{
					// taskmanager.cpp:12547
					unsigned char content = packet.Decode1();
					// taskmanager.cpp:12548
					unsigned long serial = packet.Decode4();
					// taskmanager.cpp:12549
					std::string key = packet.DecodeStr();
					// taskmanager.cpp:12550
					unsigned char result = packet.Decode1();
					// taskmanager.cpp:12551
					SecurityKeyManager()->ResCreateKey((eSKContent)content, serial, key, result);
				}
				break;
			case 1:
				{
					// taskmanager.cpp:12557
					unsigned char content = packet.Decode1();
					// taskmanager.cpp:12558
					unsigned long serial = packet.Decode4();
					// taskmanager.cpp:12559
					std::string key = packet.DecodeStr();
					// taskmanager.cpp:12560
					unsigned char result = packet.Decode1();
					// taskmanager.cpp:12561
					SecurityKeyManager()->ResCheckKey((eSKContent)content, serial, key, result);
				}
				break;
			}
		}
		break;
	case 0x15b:
		// taskmanager.cpp:12569
		{
			unsigned char roomCount = packet.Decode1();
			// taskmanager.cpp:12570
			unsigned short filter = packet.Decode2();
			// taskmanager.cpp:12572
			char text[0x100] = { 0, };
			char serverName[0x28] = { 0, };
			char serverType[0x28] = { 0, };
			char courseName[0x28] = { 0, };
			// taskmanager.cpp:12577
			wsprintfA(text, "\xc3\xd1 \xb0\xb3\xbc\xb3\xb5\xc8 \xb9\xe6 \xb0\xb3\xbc\xf6 %d", roomCount);
			// taskmanager.cpp:12578
			CChatMsg::Instance()->AddChatMsg(text, 0xFFFF7878, false, false);
			// taskmanager.cpp:12579
			for (int i = 0; i < roomCount; i++) {
				sRoomInfoLegacy room;
				// taskmanager.cpp:12581
				packet.DecodeBuffer(&room, sizeof(sRoomInfoLegacy));
				// taskmanager.cpp:12582
				if (!room.m_isPublic && !room.m_intrusionAccess && filter == 4 && !room.m_state)
					continue;
				// taskmanager.cpp:12585
				wsprintfA(serverName, "%d", room.m_serverId);
				// taskmanager.cpp:12586
				std::list<sGameServerInfo> &serverList = CSharedDoc::Instance()->m_gameServerList;
				for (std::list<sGameServerInfo>::iterator it = serverList.begin(); it != serverList.end(); ++it) {
					// taskmanager.cpp:12588
					if (it->id == room.m_serverId) {
						// taskmanager.cpp:12590
						wsprintfA(serverName, "%s", it->name);
						break;
					}
				}
				// taskmanager.cpp:12595
				switch (room.m_serverType) {
				case 0:
					// taskmanager.cpp:12598
					wsprintfA(serverType, "%s", "\xb7\xe7\xc5\xb0");
					break;
				case 1:
					// taskmanager.cpp:12601
					wsprintfA(serverType, "%s", "\xc3\xca\xba\xb8");
					break;
				case 2:
					// taskmanager.cpp:12604
					wsprintfA(serverType, "%s", "\xc1\xdf\xb1\xde");
					break;
				case 3:
					// taskmanager.cpp:12607
					wsprintfA(serverType, "%s", "\xc0\xda\xc0\xaf" "1");
					break;
				case 4:
					// taskmanager.cpp:12610
					wsprintfA(serverType, "%s", "\xc0\xda\xc0\xaf" "2");
					break;
				default:
					// taskmanager.cpp:12613
					wsprintfA(serverType, "%d", room.m_serverType);
					break;
				}
				// taskmanager.cpp:12618
				switch (room.m_course) {
				case 0:
					// taskmanager.cpp:12621
					wsprintfA(courseName, "%s", "\xba\xed\xb7\xe7\xb6\xf3\xb1\xba");
					break;
				case 1:
					// taskmanager.cpp:12624
					wsprintfA(courseName, "%s", "\xba\xed\xb7\xe7\xbf\xf6\xc5\xcd");
					break;
				case 2:
					// taskmanager.cpp:12627
					wsprintfA(courseName, "%s", "\xbc\xbc\xc7\xc7\xbe\xc6\xc0\xa9\xb5\xe5");
					break;
				case 3:
					// taskmanager.cpp:12630
					wsprintfA(courseName, "%s", "\xc0\xa9\xb5\xe5\xc8\xfa");
					break;
				case 4:
					// taskmanager.cpp:12633
					wsprintfA(courseName, "%s", "\xc0\xa7\xc1\xee\xc0\xa7\xc1\xee");
					break;
				case 5:
					// taskmanager.cpp:12636
					wsprintfA(courseName, "%s", "\xbf\xfe\xbd\xba\xc6\xae\xc0\xa7\xc1\xee");
					break;
				case 6:
					// taskmanager.cpp:12639
					wsprintfA(courseName, "%s", "\xba\xed\xb7\xe7\xb9\xae");
					break;
				case 7:
					// taskmanager.cpp:12642
					wsprintfA(courseName, "%s", "\xbd\xc7\xba\xf1\xbe\xc6\xc4\xb3\xb3\xed");
					break;
				case 8:
					// taskmanager.cpp:12645
					wsprintfA(courseName, "%s", "\xbe\xc6\xc0\xcc\xbd\xba\xc4\xb3\xb3\xed");
					break;
				case 9:
					// taskmanager.cpp:12648
					wsprintfA(courseName, "%s", "\xc8\xad\xc0\xcc\xc6\xae\xc0\xa7\xc1\xee");
					break;
				case 10:
					// taskmanager.cpp:12651
					wsprintfA(courseName, "%s", "\xbb\xfe\xc0\xcc\xb4\xd7\xbb\xf7\xb5\xe5");
					break;
				case 11:
					// taskmanager.cpp:12654
					wsprintfA(courseName, "%s", "\xc7\xce\xc5\xa9\xc0\xa9\xb5\xe5");
					break;
				case 13:
					// taskmanager.cpp:12657
					wsprintfA(courseName, "%s", "\xb5\xf6\xc0\xce\xc6\xe4\xb8\xa3\xb3\xeb");
					break;
				case 14:
					// taskmanager.cpp:12660
					wsprintfA(courseName, "%s", "\xbe\xc6\xc0\xcc\xbd\xba\xbd\xba\xc6\xc4");
					break;
				case 15:
					// taskmanager.cpp:12663
					wsprintfA(courseName, "%s", "\xb7\xce\xbd\xba\xc6\xae\xbd\xc3\xbf\xfe\xc0\xcc");
					break;
				case 16:
					// taskmanager.cpp:12666
					wsprintfA(courseName, "%s", "\xc0\xcc\xbd\xba\xc5\xcf\xb9\xeb\xb8\xae");
					break;
				case 17:
					// taskmanager.cpp:12670
					wsprintfA(courseName, "%s", "\xbd\xba\xc6\xf3\xbc\xc8\xbc\xc5\xc7\xc3");
					break;
				case 19:
					// taskmanager.cpp:12681
					wsprintfA(courseName, "%s", "\xc0\xa7\xc1\xee\xbd\xc3\xc6\xbc");
					break;
				case 127:
					// taskmanager.cpp:12686
					wsprintfA(courseName, "%s", "\xb7\xa3\xb4\xfd");
					break;
				default:
					// taskmanager.cpp:12689
					wsprintfA(courseName, "%d", room.m_course);
					break;
				}
				// taskmanager.cpp:12692
				wsprintfA(text, "%s %s %d\xb9\xf8\xb9\xe6 %s %d\xc8\xa6", serverName, serverType, room.m_roomNumber, courseName, room.m_holeCount);
				// taskmanager.cpp:12693
				CChatMsg::Instance()->AddChatMsg(text, 0xFFFF7878, false, false);
			}
		}
		break;
	case 0x123:
		// taskmanager.cpp:12699
		if (IsLocalContent((localContentType_t)0x56)) {
			// taskmanager.cpp:12701
			unsigned long result = packet.Decode4();
			// taskmanager.cpp:12703
			if (result) {
				// taskmanager.cpp:12706
				sRealMyRoomAuthority authority;
				packet.DecodeBuffer(&authority, sizeof(authority));
				// taskmanager.cpp:12709
				CSharedDoc::Instance()->m_realMyRoomUID = authority.ownerUID;
				// taskmanager.cpp:12710
				CSharedDoc::Instance()->m_authSetting = authority.bPrivate;
				// taskmanager.cpp:12711
				CSharedDoc::Instance()->m_authReserved = authority.reserved;
				// taskmanager.cpp:12712
				strcpy(CSharedDoc::Instance()->m_authPassword, authority.password);
				// taskmanager.cpp:12717
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x21f, 0, 0, 0, 0, 0));
			} else {
				// taskmanager.cpp:12724
				if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsExactKindOf(&CRealMyRoomTask::m_RTTI) : false) {
					// taskmanager.cpp:12725
					AfxPostMsg(0, "RealMyRoom", 0x228, 0x123, 0, 0, 0);
				} else {
					// taskmanager.cpp:12727
					AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x228, 0x123, 0, 0, 0, 0));
				}
			}
		}
		break;
	case 0x186:
		// taskmanager.cpp:12738
		{
			int result = packet.Decode4();
			// taskmanager.cpp:12739
			if (result == 2) {
				// taskmanager.cpp:12741
				int kind = packet.Decode4();
				// taskmanager.cpp:12742
				if (kind == 1) {
					// taskmanager.cpp:12744
					unsigned long typeId = packet.Decode4();
					// taskmanager.cpp:12745
					_RemoveWareHouseItem(typeId, AfxGetTask()->GetActor("RealMyRoom"), 1);
				}
			}
			// taskmanager.cpp:12749
			packet.Reset();
			// taskmanager.cpp:12750
			CItemBuff::ProcessPacket(packet);
		}
		break;
	case 0x188:
		// taskmanager.cpp:12756
		if (IsLocalContent((localContentType_t)0x78) == true) {
			// taskmanager.cpp:12758
			unsigned long value = packet.Decode4();
			// taskmanager.cpp:12760
			if ((__rtti_obj = AfxGetTask()) != 0 ? ((IObject *)__rtti_obj)->IsKindOf(&CLobbyTask::m_RTTI) : false) {
				// taskmanager.cpp:12762
				AfxGetTask()->GetActor("Lobby") << MsgObject(0, 0x248, value, 0, 0, 0, 0);
			}
		}
		break;
	case 0x189:
		// taskmanager.cpp:12770
		if (IsLocalContent((localContentType_t)0x78) == true) {
			// taskmanager.cpp:12772
			unsigned long typeId = packet.Decode4();
			// taskmanager.cpp:12773
			_RemoveWareHouseItem(typeId, AfxGetTask()->GetActor("Lobby"), 100);
			// taskmanager.cpp:12775
			FrForm *pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", 0);
			// taskmanager.cpp:12776
			pForm->SetTimeLimit(3.0f);
			// taskmanager.cpp:12784
			// "You received an event item. Check it in My Room."
			pForm->SetMessage("\xC0\xCC\xBA\xA5\xC6\xAE \xBE\xC6\xC0\xCC\xC5\xDB\xC0\xBB \xB9\xDE\xBE\xD2\xBD\xC0\xB4\xCF\xB4\xD9. \xB8\xB6\xC0\xCC\xB7\xEB\xBF\xA1\xBC\xAD \xC8\xAE\xC0\xCE\xC7\xCF\xBC\xBC\xBF\xE4.", false);
			// taskmanager.cpp:12789
			pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
		}
		break;
	case 0x18d:
		// taskmanager.cpp:12794
		{
			unsigned char result = packet.Decode1();
			// taskmanager.cpp:12796
			FrCardRemoveResult* pForm = CreateForm<FrCardRemoveResult>(g_pFresh->GetManager(), this, "card_remove_result", 0);
			// taskmanager.cpp:12797
			if (pForm == NULL)
				break;
			// taskmanager.cpp:12803
			if (result == 1) {
				// taskmanager.cpp:12806
				unsigned long partsId = packet.Decode4();
				// taskmanager.cpp:12807
				unsigned long removerId = packet.Decode4();
				// taskmanager.cpp:12808
				if (FrCardRemove::IsCardRemoveItem(removerId) == true) {
					// taskmanager.cpp:12811
					pForm->SetRemover(removerId);
					// taskmanager.cpp:12814
					_RemoveWareHouseItem(removerId, AfxGetTask()->GetActor("RealMyRoom"), 1);
					// taskmanager.cpp:12817
					if (partsId != 0) {
						// taskmanager.cpp:12819
						std::list<unsigned long> cards;
						// taskmanager.cpp:12820
						if (S5::UTIL::ErasePartsCardAll(partsId, cards)) {
							// taskmanager.cpp:12822
							for (std::list<unsigned long>::iterator it = cards.begin(); it != cards.end(); ++it)
								// taskmanager.cpp:12823
								pForm->PushList(*it);
						}
						// taskmanager.cpp:12826
						pForm->SetResult((FrCardRemoveResult::eRESULT)1);
					} else {
						// taskmanager.cpp:12832
						pForm->SetResult((FrCardRemoveResult::eRESULT)2);
					}
				} else {
					// taskmanager.cpp:12838
					pForm->SetResult((FrCardRemoveResult::eRESULT)0);
				}
			} else if (result == 0) {
				// taskmanager.cpp:12843
				unsigned char error = packet.Decode1();
				// taskmanager.cpp:12846
				if (error != 9)
					// taskmanager.cpp:12848
					pForm->SetResult((FrCardRemoveResult::eRESULT)0);
				else
					// taskmanager.cpp:12852
					pForm->SetResult((FrCardRemoveResult::eRESULT)3);
			}
			// taskmanager.cpp:12856
			pForm->SetCaption("\xbb\xe7\xbf\xeb\xb0\xe1\xb0\xfa");
			// taskmanager.cpp:12857
			pForm->Open((FRESH_RESULT_CALLBACK)0, 0);
		}
		break;
	case 0x18b:
		// taskmanager.cpp:12863
		if (IsLocalContent((localContentType_t)0x79)) {
		}
		break;
	case 0x198:
		// taskmanager.cpp:12893
		SetMyOutsideCash(packet.Decode8());
		// taskmanager.cpp:12895
		CSharedDoc::Instance()->RefreshAllCookie();
		break;
	case 0x19a:
		// taskmanager.cpp:12902
		{
			unsigned char course = packet.Decode1();
			// taskmanager.cpp:12903
			unsigned long count = packet.Decode4();
			// taskmanager.cpp:12905
			std::map<unsigned char, sMapStatistics> &stats = CSharedDoc::Instance()->GetMyPastMapStat(course);
			// taskmanager.cpp:12906
			for (unsigned long i = 0; i < count; i++) {
				// taskmanager.cpp:12908
				unsigned char key = packet.Decode1();
				sMapStatistics stat;
				// taskmanager.cpp:12910
				packet.DecodeBuffer(&stat, sizeof(stat));
				// taskmanager.cpp:12911
				stats.insert(std::pair<const unsigned char, sMapStatistics>(key, stat));
			}
		}
		break;
	case 0x1a1:
		// taskmanager.cpp:13048
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:13050
		if (IsLocalContent((localContentType_t)0x8E)) {
			// taskmanager.cpp:13055
			unsigned long count = packet.Decode4();
			// taskmanager.cpp:13056
			for (unsigned long i = 0; i < count; i++) {
				// taskmanager.cpp:13058
				unsigned long value = packet.Decode4();
				// taskmanager.cpp:13059
				AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x252, (intptr_t)&value, 0, 0, 0, 0));
			}
		}
		break;
	case 0x1cf:
		// taskmanager.cpp:13350
		{
			unsigned long param1 = packet.Decode4();
			// taskmanager.cpp:13351
			unsigned long param2 = packet.Decode4();
			// taskmanager.cpp:13353
			AfxGetTask()->GetActor(data_aa04ec[3]) << MsgObject(0, 0x26D, 0xF, param1, param2, 0, 0);
		}
		break;
	case 0x1d0:
		// taskmanager.cpp:13359
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 2, 0, 0, 0, 0, 0));
		// taskmanager.cpp:13360
		AfxGetTask()->GetActor(data_aa04ec[3]) << MsgObject(0, 0x26D, 0x11, 0, 0, 0, 0);
		break;
	case 0x30:
	case 0x60:
	case 0xff:
	case 0x11e:
	case 0x13b:
	case 0x19e:
		PG_BOOT_TODOF(("taskmanager.cpp:13359 empty case %#x (of 0x30, 0x60, 0xff, 0x11e, 0x13b, 0x19e)", (unsigned)(g_iRecvPacketType)));
		break;
	default:
		// taskmanager.cpp:13946
		return 0;
	}
	// taskmanager.cpp:13954
	return 1;
}

// taskmanager.cpp:13964
int CTask::_RemoveWareHouseItem(unsigned long typeId, IActor *pActor, int count) {
	unsigned long serial = 0;
	std::list<sItemInfo>::iterator it;

	if (typeId == 0x1A000042 && IsLocalContent((localContentType_t)0x14)) {
		AfxGetTask()->PassMsgToActor("RealMyRoom", MsgObject(0, 0x204, (intptr_t)&serial, 0, 0, 0, 0));
		for (it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {
			if (it->m_serial == serial) {
				break;
			}
		}
	} else {
		for (it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {
			if (it->m_typeId == typeId) {
				break;
			}
		}
	}

	if (it == CSharedDoc::Instance()->m_itemInfoList.end()) {
		return 0;
	}

	int removed;
	for (removed = 0; removed < count; removed++) {
		it->m_count--;
		if (it->m_count <= 0) {
			CSharedDoc::Instance()->m_itemInfoList.erase(it);
			break;
		}
	}

	pActor << MsgObject(0, 0xDD, 0, 0, 0, 0, 0);

	return removed;
}

// taskmanager.cpp:14002
int CTask::_RemoveWareHouseItem(unsigned long typeId, const char *actorName, int count) {
	return _RemoveWareHouseItem(typeId, AfxGetTask()->GetActor(actorName), count);
}

// taskmanager.cpp:14013
void CTask::_ReFreshMyRoomGift(bool bSend, std::string name) {
	if (bSend) {
		WSendPacket packet(GCR_YOU_GOT_GIFT);
		packet.EncodeStr(name);
		packet.Encode4(MyUID());
		packet.Send(SEND_TO_MSN);
	} else {
		CSharedDoc::Instance()->m_giftList.clear();
		WSendPacket packet(GCT_NEW_GIFT_LIST);
		packet.Send(SEND_TO_DEFAULT);
	}
}

// taskmanager.cpp:14039
bool CTask::OnOverlappedDlg(int result, FrForm *pForm) {
	g_bQuit = true;
	return true;
}

// taskmanager.cpp:14051
CTaskManager::CTaskManager(void)
	: m_bReservedTask(false), m_reservedPreserve(false), m_reservedTaskName(NULL), m_reservedParam(NULL),
	  m_currentTask(NULL), m_preservedTask(NULL), m_pBackGround(NULL), m_pWork(NULL) {
	m_curHistory = m_history.begin();

	m_naviState = 0;
}

// taskmanager.cpp:14057
CTaskManager::~CTaskManager(void) {
	ClearTask();

	if (m_preservedTask) {

		m_preservedTask->Destroy();
		delete m_preservedTask;
		m_preservedTask = NULL;
	}

	if (m_pBackGround) {
		delete m_pBackGround;
		m_pBackGround = NULL;
	}
}

// taskmanager.cpp:14071
void CTaskManager::CreateTask(const char *taskName, const char *param, bool preserve) {
	if (preserve) {
		if (m_preservedTask == NULL) {
			m_preservedTask = m_currentTask;
			m_preservedTask->PreserveBack(taskName);
			m_currentTask = NULL;
		}
		ClearTask();
	} else {
		ClearTask();

		if (m_preservedTask) {
			if (stricmp(m_preservedTask->GetRTTI()->GetName(), taskName) == 0) {
				m_currentTask = m_preservedTask;
				m_currentTask->RestorePreserved();

				m_preservedTask = NULL;
				m_bReservedTask = false;
				m_reservedTaskName = NULL;
				m_reservedParam = NULL;
				if (m_pBackGround) {
					m_pBackGround->ResetScreenSize();
				}
				return;
			}

			m_preservedTask->Destroy();
			delete m_preservedTask;
			m_preservedTask = NULL;
		}
	}

	m_currentTask = (CTask *)ObjectFactory().CreateObject(taskName);

	m_currentTask->ApplyScreenSize();

	if (m_pBackGround) {
		m_pBackGround->ResetScreenSize();
	}

	if (param && *param) {
		// The type test casts the created object itself, not __rtti_obj.
		IObject *pDoc = ObjectFactory().CreateObject(param);
		m_currentTask->m_pDoc = DYNAMIC_CAST(CTaskDoc, pDoc);
		if (m_currentTask->m_pDoc) {
			m_currentTask->m_pDoc->m_pTask = m_currentTask;
		}
	} else {
		m_currentTask->m_pDoc = NULL;
	}

	m_currentTask->Register();
	m_currentTask->PreLoadInit();

	g_LoadParam.m_progress = 0;
	g_LoadParam.m_bLoaded = false;
	g_LoadParam.m_bInited = false;
	m_reservedPreserve = preserve;

	m_bReservedTask = false;
	m_reservedTaskName = NULL;
	m_reservedParam = NULL;
	m_currentTask->Load();
}

// taskmanager.cpp:14162
void CTaskManager::ChangeTask(const char *taskName, const char *param, bool preserve) {
	CMessengerInfo::Instance()->CloseWindow();

	m_reservedTaskName = taskName;
	m_reservedParam = param;

	m_bReservedTask = true;
	m_reservedPreserve = preserve;

	g_audio->m_fadeOutTime = 1.0f;

	if (m_pBackGround) { delete m_pBackGround; m_pBackGround = NULL; }
	m_pBackGround = new CBackGround(NULL, true);
	m_backGroundAlpha = 1.0f;
}

// taskmanager.cpp:14180
void CTaskManager::PreProcess(void) {
	if (m_bReservedTask) {
		CreateTask(m_reservedTaskName, m_reservedParam, m_reservedPreserve);
	}
}

// taskmanager.cpp:14187
void CTaskManager::ProcessTask(float dt) {
	if (m_bReservedTask) {
		return;
	}

	if (m_pBackGround) {
		float step = (m_currentTask->m_unknownC ? 0.8f : 1.0f) * dt;
		if (step > 0.04f) {
			step = 0.04f;
		}

		m_backGroundAlpha -= step;
		if (m_backGroundAlpha < 0.0f) {
			m_backGroundAlpha = 0.0f;
			delete m_pBackGround;
			m_pBackGround = NULL;
		}
	}

	if (!g_LoadParam.m_bLoaded) {
		m_currentTask->OnProcessWhileLoading(dt);

		Sleep(30);
		return;
	}

	if (!g_LoadParam.m_bInited) {
		m_currentTask->Init(NULL);
		g_LoadParam.m_bInited = true;
	}

	while (!m_postMsgQueue.empty()) {
		sPostMsg &postMsg = m_postMsgQueue.front();
		m_currentTask->PassMsgToActor(postMsg.m_target.c_str(), postMsg.m_msg);
		m_postMsgQueue.pop();
	}

	m_currentTask->Process(dt);
}

// taskmanager.cpp:14240
void CTaskManager::DisplayTask(void) const {
	if (!g_LoadParam.m_bInited) {
		m_currentTask->OnDisplayWhileLoading();
	} else {
		if (!m_bReservedTask) {
			m_currentTask->Display();
		}
	}

	if (m_pBackGround) {
		m_pBackGround->Draw(((int)(m_backGroundAlpha * 255.0f) << 24) | 0xFFFFFF);
	}
}

// taskmanager.cpp:14257
void CTaskManager::ClearTask(void) {
	if (CCapturedBg::Instance()) {
		CCapturedBg::Instance()->Reset();
	}

	if (m_currentTask) {
		m_currentTask->Destroy();
		delete m_currentTask;
		m_currentTask = NULL;
	}

	if (HackingManager::Instance()) {
		HackingManager::Instance()->ClearCrypticList();
	}
}

// taskmanager.cpp:14280
CTask *CTaskManager::GetCurrentTask(void) const {
	return m_currentTask;
}

// taskmanager.cpp:14287
int CTaskManager::PostMsg(const IActor *pSender, const char *pTarget, int msg, intptr_t param1, intptr_t param2, intptr_t param3, unsigned long param4) {
	sPostMsg postMsg;

	postMsg.m_msg.m_pSender = pSender;
	postMsg.m_msg.m_msg = msg;
	postMsg.m_msg.m_param1 = param1;
	postMsg.m_msg.m_param2 = param2;
	postMsg.m_msg.m_param3 = param3;
	postMsg.m_msg.m_reserved = param4;
	postMsg.m_target = pTarget;

	m_postMsgQueue.push(postMsg);
	return 0;
}

// taskmanager.cpp:14305
CTaskDoc::CTaskDoc(void) {
}

// taskmanager.cpp:14309
CTaskDoc::~CTaskDoc(void) {
}

// taskmanager.cpp:14313
CTask *CTaskDoc::GetTask(void) const {
	return m_pTask;
}

// taskmanager.cpp:14324
void CTaskManager::NaviOpenLayout(const char *taskName, const char *docName, const char *actorName, const char *layoutName) {
	switch (m_naviState) {
	case 1:
		--m_curHistory;
		break;
	case 0: {
		std::list<history_t>::iterator it = m_curHistory;
		if (++it != m_history.end()) {
			if (stricmp((*it).m_layoutName, layoutName) == 0) {
				++m_curHistory;
				break;
			}
			m_history.erase(++m_curHistory, m_history.end());
			NaviAddHistory(taskName, docName, actorName, layoutName);
		} else {
			NaviAddHistory(taskName, docName, actorName, layoutName);
		}
		break;
	}
	}

	CTask *task = AfxGetTask();
	((CTaskMain *)task->m_mainActor)->CheckNaviButtons();

	m_naviState = 0;
}

// taskmanager.cpp:14353
void CTaskManager::NaviAddHistory(const char *taskName, const char *docName, const char *actorName, const char *layoutName) {
	history_t history;
	history.m_taskName = taskName;

	history.m_docName = docName;
	history.m_layoutName = layoutName;
	history.m_actorName = actorName;

	m_curHistory = m_history.insert(m_history.end(), history);
}

// taskmanager.cpp:14368
void CTaskManager::NaviClearHistory(void) {
	m_history.clear();
}

// taskmanager.cpp:14373
void CTaskManager::NaviGoForward(void) {
	std::list<history_t>::iterator it = m_curHistory;

	if (it != m_history.end()) {
		if (++it != m_history.end()) {
			history_t history = *it;

			sActor *actor = AfxGetTask()->m_actorList.Find(history.m_actorName);
			if (!actor || !actor->m_pActor) {
				CTaskManager::Instance()->ChangeTask(history.m_taskName, history.m_docName, false);
			}

			CTaskManager::Instance()->PostMsg(NULL, history.m_actorName, 0, (intptr_t)history.m_layoutName, 1, 0, 0);
			m_naviState = 0;
		}
	}
}

// taskmanager.cpp:14393
void CTaskManager::NaviGoBackward(void) {
	std::list<history_t>::iterator it = m_curHistory;

	if (it != m_history.begin()) {
		history_t history = *--it;

		sActor *actor = AfxGetTask()->m_actorList.Find(history.m_actorName);
		if (!actor || !actor->m_pActor) {
			CTaskManager::Instance()->ChangeTask(history.m_taskName, history.m_docName, false);
		}

		CTaskManager::Instance()->PostMsg(NULL, history.m_actorName, 0, (intptr_t)history.m_layoutName, 1, 0, 0);
		m_naviState = 1;
	}
}

// taskmanager.cpp:14411
bool CTaskManager::IsNaviForwardEnable(void) {
	std::list<history_t>::iterator it = m_curHistory;

	return ++it != m_history.end() ? true : false;
}

// taskmanager.cpp:14421
bool CTaskManager::IsNaviBackwardEnable(void) {
	return m_curHistory != m_history.begin() ? true : false;
}

// taskmanager.cpp:14429
bool CTaskManager::IsNaviForwardSameHistory(const char *layoutName) {
	std::list<history_t>::iterator it = m_curHistory;

	return stricmp((*++it).m_layoutName, layoutName) == 0 ? true : false;
}

// taskmanager.cpp:14439
void CTaskManager::NaviDisplayDebugInfo(void) {
	float y = 200.0f;
	int index = 0;

	for (std::list<history_t>::iterator it = m_history.begin(); it != m_history.end(); ++it) {
		history_t history = *it;
		const char *mark;
		if (it == m_curHistory) {
			mark = "*";
		} else {
			mark = "";
		}

		TextOut(g_view, 5.0f, y, "%d: (%s)%s%s", index, history.m_actorName, history.m_layoutName, mark);
		y += 15.0f;
		index++;
	}
}

// taskmanager.cpp:14465
void CTaskManager::AddNewItemToDoc(unsigned int type, WReceivedPacket &packet) {
	switch (type) {
	case 1: {
		sCharacterInfo character;
		packet.DecodeBuffer(&character, sizeof(sCharacterInfo));
		ItemManager()->GetDefCombo(character.typeId, character.partTypeIds);

		CSharedDoc::Instance()->m_characterMap.insert(std::map<unsigned int, sCharacterInfo>::value_type(character.id, character));
		break;
	}
	case 2: {
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(sItemInfo));

		if (IsLocalContent((localContentType_t)0x57) && IsUccClothes(item.m_typeId)) {

			CSharedDoc::Instance()->m_myItemList.push_back(item);

		} else {

			CSharedDoc::Instance()->AddMyPartsList(item);
		}

		if (IsAngelWing(item.m_typeId)) {

			CSharedDoc::Instance()->CheckAngelWing();
		}

		if (IsLocalContent((localContentType_t)0x17)) {

			if ((item.m_typeId & 0x1800) == 0x1800) {

				CSharedDoc::Instance()->SetInitHaveHalloweenItem();
			}
		}
		break;
	}
	case 4: {
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(sItemInfo));
		CSharedDoc::Instance()->m_myItemMap.insert(std::map<unsigned int, sItemInfo>::value_type(item.m_serial, item));
		break;
	}
	case 5: {
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(sItemInfo));

		std::list<sItemInfo>::iterator it;
		for (it = CSharedDoc::Instance()->m_myGearList.begin(); it != CSharedDoc::Instance()->m_myGearList.end(); ++it) {
			if (it->m_serial == item.m_serial) {
				it->m_count = item.m_count < 1 ? 1 : item.m_count;
				break;
			}
		}

		if (it == CSharedDoc::Instance()->m_myGearList.end()) {
			CSharedDoc::Instance()->m_myGearList.push_back(item);
		}
		break;
	}
	case 6: {
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(sItemInfo));

		if (item.m_stateBits.kind != 0) {
			if (item.m_stateBits.kind == 3) {
				item.m_remainHours = item.m_count;
			} else if (item.m_stateBits.kind == 4) {
				item.m_remainHours = item.m_count * 24;
			} else if (item.m_stateBits.kind == 5) {
				item.m_remainHours = item.m_count * 720;
			}
		} else {
			for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_itemInfoList.begin(); it != CSharedDoc::Instance()->m_itemInfoList.end(); ++it) {
				if (it->m_serial == item.m_serial) {
					it->m_count = item.m_count < 1 ? 1 : item.m_count;
					it->m_remainHours += item.m_remainHours;
					return;
				}
			}
		}

		CSharedDoc::Instance()->m_itemInfoList.push_back(item);
		break;
	}
	case 7: {
		sCaddieInfo caddie;
		packet.DecodeBuffer(&caddie, sizeof(sCaddieInfo));
		CSharedDoc::Instance()->m_caddieMap.insert(std::map<unsigned int, sCaddieInfo>::value_type(caddie.id, caddie));
		break;
	}
	case 8: {
		int caddieId = 0;
		int cadItemTypeId = 0;
		int hours = 0;
		packet.DecodeBuffer(&caddieId, 4);
		packet.DecodeBuffer(&cadItemTypeId, 4);
		packet.DecodeBuffer(&hours, 4);

		std::map<unsigned int, sCaddieInfo>::iterator it = CSharedDoc::Instance()->m_caddieMap.find(caddieId);
		if (it == CSharedDoc::Instance()->m_caddieMap.end()) {
			break;
		}

		it->second.cadItemTypeId = cadItemTypeId;
		it->second.hours = hours;

		if (it->second.days < hours) {

			it->second.days = hours;
		}
		break;
	}
	case 14: {
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(sItemInfo));
		item.m_remainHours = item.m_count * 24;

		for (std::list<sItemInfo>::iterator it = CSharedDoc::Instance()->m_mySkinList.begin(); it != CSharedDoc::Instance()->m_mySkinList.end(); ++it) {
			if (it->m_typeId == item.m_typeId || it->m_serial == item.m_serial) {
				return;
			}
		}

		CSharedDoc::Instance()->m_mySkinList.push_back(item);
		break;
	}
	case 15: {
		unsigned long hairStyleId = 0;
		packet.DecodeBuffer(&hairStyleId, 4);

		sCharacterInfo character;
		packet.DecodeBuffer(&character, sizeof(sCharacterInfo));
		ItemManager()->GetDefCombo(character.typeId, character.partTypeIds);

		CSharedDoc::Instance()->m_characterMap[CSharedDoc::Instance()->m_myCharacterId];

		IFF_STRUCT::sHairStyle *pHairStyle = ItemManager()->FindHairStyle(hairStyleId);
		if (pHairStyle == NULL) {
			break;
		}

		for (std::map<unsigned int, sCharacterInfo>::iterator it = CSharedDoc::Instance()->m_characterMap.begin(); it != CSharedDoc::Instance()->m_characterMap.end(); ++it) {

			if (it->second.typeId == (pHairStyle->m_style | 0x4000000)) {

				it->second.hairColor = pHairStyle->m_color;
				break;
			}
		}
		break;
	}
	case 16: {
		sMascotInfo mascot;
		packet.DecodeBuffer(&mascot, sizeof(sMascotInfo));
		strcpy(mascot.message, "PANGYA!");

		CSharedDoc::Instance()->AddMascot(&mascot);
		break;
	}
	case 28: {
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(sItemInfo));

		std::list<sItemInfo>::iterator it;
		for (it = CSharedDoc::Instance()->m_myExtraItemList3.begin(); it != CSharedDoc::Instance()->m_myExtraItemList3.end(); ++it) {
			if (it->m_serial == item.m_serial) {
				it->m_count = item.m_count < 1 ? 1 : item.m_count;
				break;
			}
		}

		if (it == CSharedDoc::Instance()->m_myExtraItemList3.end()) {
			CSharedDoc::Instance()->m_myExtraItemList3.push_back(item);
		}
		break;
	}
	case 29: {
		sItemInfo item;
		packet.DecodeBuffer(&item, sizeof(sItemInfo));

		IFF_STRUCT::sQuestDrop *pQuestDrop = ItemManager()->FindQuestDrop(item.m_typeId);
		if (pQuestDrop == NULL) {
			break;
		}

		std::list<sItemInfo>::iterator it;
		for (it = CSharedDoc::Instance()->m_myExtraItemList4.begin(); it != CSharedDoc::Instance()->m_myExtraItemList4.end(); ++it) {
			if (it->m_serial == item.m_serial) {
				it->m_count++;
				break;
			}
		}

		if (it == CSharedDoc::Instance()->m_myExtraItemList4.end()) {
			CSharedDoc::Instance()->m_myExtraItemList4.push_back(item);
		}
		break;
	}
	case 31: {
		sCards card;
		packet.DecodeBuffer(&card, sizeof(sCards));

		IFF_STRUCT::sCard *pCard = ItemManager()->FindCard(card.m_typeId);
		if (pCard == NULL) {
			break;
		}

		for (std::list<sCards>::iterator it = CSharedDoc::Instance()->m_myCardList.begin(); it != CSharedDoc::Instance()->m_myCardList.end(); ++it) {

			if (it->m_typeId == card.m_typeId) {

				it->m_count += card.m_count < 1 ? 1 : card.m_count;
				return;
			}
		}

		CSharedDoc::Instance()->m_myCardList.push_back(card);
		break;
	}
	}
}

class CNoticeBoard;
class CMatchingSystem;
class CEventThread;
class CWinnerNotice;

// Instantiates the singleton accessors this unit emits (the linker kept this
// unit's copies; the callers are in other units).
template CNoticeBoard* WSingleton<CNoticeBoard>::Instance();
template CMatchingSystem* WSingleton<CMatchingSystem>::Instance();
template CEventThread* WSingleton<CEventThread>::Instance();
template CWinnerNotice* WSingleton<CWinnerNotice>::Instance();
