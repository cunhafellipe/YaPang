// The JP 983 lobby's 3D mascot actor LobbyMascot ("LobbyMascot",
// JpLobbyMascot.h), from the JP exe's bodies (node scripts/disasm.ts --jp
// 0x<va>). Registered by its static registrar (0x565150); the JP
// CLobbyTask::Register 0xabfcc0 adds it as "LobbyMascot".
#include "wmath.h"

#include "JpLobbyMascot.h"

#include "GolfDoc.h"
#include "MascotInfo.h"
#include "polysoup.h"
#include "commonutil.h"
#include "wpuppet.h"
#include "scenemanager.h"
#include "SharedDoc.h"
#include "TaskManager.h"
#include "actor.h"

// 0xe751a4 (initializer 0xcce7a0)
const WRTTI LobbyMascot::m_RTTI("LobbyMascot", &IActor::m_RTTI);

// lib: 0x563e40 (std::tr1::bind of Load), 0x564740 (std::tr1::function
// assign), 0x564ce0 (the +0x20 map's erase in the destructor)

// 0x563860
void LobbyMascot::OnLoad(void) {
}

// 0x563870
void LobbyMascot::OnDisplay(void) {
	if (m_mascot != NULL) {
		m_mascot->Display();
	}
}

// 0x563c10
void LobbyMascot::OnAnotherHand(int nWork, void* pData) {
	(void)nWork;
	(void)pData;
	m_request.m_load();
	m_request.m_built = true;
}

// 0x563ef0
void LobbyMascot::SetPositionFromScenePoint(const char* name) {
	// JP CSharedDoc +0x11024's +0x8 (as LobbyPlayer 0x566350).
	WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->FindPoint(name);
	if (point != NULL) {
		m_position = point->m_pos;
	}
}

// 0x563f30
bool LobbyMascot::AttachMascot() {
	if (m_mascot == NULL || !m_mascot->SetMotionByName("Default")) {  // 0xd655fc
		return false;
	}
	m_mascot->SetPosition(&m_position);
	m_mascot->m_motionPlaying = true;
	m_mascot->m_motionHold = true;
	CSceneManager::Instance()->RegisterElement((eObjType)4, NULL, NULL, m_mascot->m_name.c_str(), 0x18, NULL);
	CSceneManager::Instance()->SetPuppet(m_mascot->m_name.c_str(), m_mascot->m_puppet);
	return true;
}

// 0x563fc0
void LobbyMascot::ResetMascot() {
	if (m_mascot != NULL) {
		CSceneManager::Instance()->DeleteElement(m_mascot->m_name.c_str());
	}
	if (m_mascot != NULL) {
		delete m_mascot;
		m_mascot = NULL;
	}
	m_mascot = NULL;
	m_attachPending = false;
	m_mascotId = 0;
	m_request.m_built = true;
	if (m_hand.IsWorking(0)) {
		m_hand.WaitUntilWorkDone(0xffffffff);
	}
	Jp_ReleaseThreadResources(m_hand.GetThreadID());
}

// 0x564340
void LobbyMascot::OnProcess(float dt) {
	if (m_request.m_requested) {
		m_request.m_requested = false;
		if (!m_hand.IsWorking(0)) {
			m_hand.Work(this, 0, NULL, true, -1);
		}
	}
	if (m_mascot != NULL && m_attachPending) {
		AttachMascot();
		m_attachPending = false;
	}
	if (m_mascot != NULL) {
		m_mascot->Process(dt);
	}
}

// 0x5643e0
void LobbyMascot::OnDestroy(void) {
	ResetMascot();
	m_attachPending = true;
	m_position = WVector::ZERO;
}

// 0x564610
bool LobbyMascot::Build(int mascotId) {
	if (m_mascot != NULL) {
		return false;
	}
	IDummyPet* mascot = new IDummyPet(5);
	if (mascot == NULL) {
		return false;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	// JP CSharedDoc +0xa8e: the KR m_myMascotId; JP 0x63dc00: the KR
	// GetMyMascotInfo.
	sMascotInfo* info = doc->GetMyMascotInfo(mascotId != 0 ? (unsigned long)mascotId : doc->m_myMascotId);
	if (info == NULL) {
		delete mascot;
		return false;
	}
	// JP CSharedDoc +0x8cb: the KR MyUID() (Jp_GameState_AvatarNameKey).
	mascot->m_name = MakeStr("LOBBY_MASCOT_%d_%d", Jp_GameState_AvatarNameKey(), info->typeId);
	if (mascot->BuildMascot(info->typeId, true)) {
		EnterCriticalSection(&m_lock);
		m_mascot = mascot;
		LeaveCriticalSection(&m_lock);
	}
	// The JP returns true (and keeps no pointer to the pet) when the build
	// fails.
	return true;
}

// 0x564820
bool LobbyMascot::LoadMascot() {
	bool result = Build(m_request.m_mascotId);
	m_attachPending = result;
	if (result) {
		m_mascotId = m_request.m_mascotId;
	}
	return result;
}

// 0x564890
bool LobbyMascot::Load() {
	if (m_request.m_built) {
		return false;
	}
	LoadMascot();
	Sleep(15);
	return true;
}

// 0x5648f0
void LobbyMascot::OnPreLoadInit(void) {
	m_request.m_load = std::bind(&LobbyMascot::Load, this);  // bind 0x563e40, function assign 0x564740
	m_request.m_built = false;
	m_request.m_mascotId = 0;
}

// 0x5650b0 (scalar deleting destructor; body 0x564e60): the hand, the
// request, the +0x20 map and IActor are destroyed implicitly; the mascot is
// not deleted.
LobbyMascot::~LobbyMascot() {
	DeleteCriticalSection(&m_lock);
}

// 0x564fb0
LobbyMascot::LobbyMascot()
	: m_mascot(NULL),
	  m_mascotId(0),
	  m_position(WVector::ZERO) {
	m_attachPending = true;
	m_unknown48 = 0;
	InitializeCriticalSection(&m_lock);
}

// 0x5650e0
IObject* LobbyMascotMakeInstance(void) {
	return new LobbyMascot;
}

// 0x565150: the registration object's constructor (the KR IMPLEMENT
// pattern of LobbyBg.cpp: ObjectFactory 0xa4c0d0 / AddObjectFunctor
// 0xa4bcf0, ActorFactory 0xa471a0 / AddActorClass 0xa46aa0; the static's
// initializer is 0xcce7c0).
struct __impLobbyMascot {
	__impLobbyMascot(void) {
		ObjectFactory().AddObjectFunctor(LobbyMascotMakeInstance, "LobbyMascot");
		ActorFactory().AddActorClass("LobbyMascot");
	}
};
static __impLobbyMascot s_impLobbyMascot;

// 0x565180
void LobbyMascot::HandleMsg(const MsgObject& msg) {
	switch (msg.m_msg) {
	// 0x180: the mascot's motion reset.
	case 0x180:
		if (m_mascot != NULL) {
			m_mascot->ResetMotion(0, 0, true);
		}
		break;
	case 0x184:
		OnDestroy();
		break;
	// 0x1aa: build the owned mascot (param1: its id, by value; 0: the
	// equipped one).
	case 0x1aa: {
		if (WisEqual(m_position, WVector::ZERO, 1e-5f)) {  // 0xd0a5a8
			SetPositionFromScenePoint("mascot");
		}
		int mascotId = (int)msg.m_param1;
		if (mascotId == 0) {
			// JP CSharedDoc +0xa8e: the KR m_myMascotId.
			mascotId = (int)WSingleton<CSharedDoc>::Instance()->m_myMascotId;
		}
		if (m_mascotId == mascotId) {
			break;
		}
		m_request.m_mascotId = mascotId;
		m_request.m_built = false;
		m_request.m_requested = true;
		break;
	}
	case 0x1ab:
		ResetMascot();
		break;
	// 0x1ce / 0x1cf: the shop's ChangeMainTab: the mascot shown / hidden.
	case 0x1ce:
		if (m_mascot != NULL && m_mascot->m_puppet != NULL) {
			m_mascot->m_puppet->SetAlpha((unsigned char)0xff);
		}
		break;
	case 0x1cf:
		if (m_mascot != NULL && m_mascot->m_puppet != NULL) {
			m_mascot->m_puppet->SetAlpha((unsigned char)0);
		}
		break;
	}
}
