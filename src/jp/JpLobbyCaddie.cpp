// The JP 983 lobby's 3D caddie actor LobbyCaddie ("LobbyCaddie",
// JpLobbyCaddie.h), from the JP exe's bodies (node scripts/disasm.ts --jp
// 0x<va>). Registered by its static registrar (0x561670); the JP
// CLobbyTask::Register 0xabfcc0 adds it as "LobbyCaddie".
#include "wmath.h"

#include "JpLobbyCaddie.h"

#include "JpLobbyGlue.h"
#include "JpFxInterpreter.h"
#include "JpTooltipWnd.h"

#include <string.h>

#include "GolfDoc.h"
#include "ItemManager.h"
#include "globalgamedefine.h"
#include "polysoup.h"
#include "commonutil.h"
#include "wpuppet.h"
#include "RFLobbyDesktop.h"
#include "scenemanager.h"
#include "SharedDoc.h"
#include "InputManager.h"
#include "RFManager.h"
#include "TaskManager.h"
#include "actor.h"

// 0xe7518c (initializer 0xcce700)
const WRTTI LobbyCaddie::m_RTTI("LobbyCaddie", &IActor::m_RTTI);

// The CP949 names the caddie plays.
static const char kCaddieWaitMotion[] = "\xB1\xE2\xB4\xD9\xB8\xAE\xB1\xE2";  // "기다리기" (0xd0a3f4)
static const char kCaddieDefaultMotion[] = "\xB1\xE2\xBA\xBB";               // "기본" (0xcfed88)
// 0xe0c02c: the caddie attach's sequence.
static const char* const kCaddieTalkSequence = "Talk_Room.seq";  // 0xd0a394

// lib: 0x560ed0 / 0x561040 / 0x560b40 (std::tr1::shared_ptr<IDummyPet>
// construct, assign, destroy), 0x560160 (std::tr1::bind of Load), 0x5610b0
// (std::tr1::function assign)

// Sends a parameterless message to the current task's "Lobby" actor (the
// inlined CTask::GetActor + IActor::operator<< of the loader).
static void SendToLobby(int msg) {
	IActor* lobby = AfxGetTask()->GetActor("Lobby");
	if (lobby != NULL) {
		lobby << MsgObject(NULL, msg, 0, 0, 0, 0, 0);
	}
}

// RFLobbyDesktop 0x55fe60 / 0x55fea0 (desktop members, placed next to this
// class's code; written here on GetLobbyState until the desktop header has
// them, as JpLobbyPlayer.cpp does): the lobby states 5-7, and the states 3
// and 6.
static bool IsDesktopState567(RFLobbyDesktop* desktop) {
	int32_t state = desktop->GetLobbyState();
	if (state == 5 || state == 6 || state == 7) {
		return true;
	}
	return false;
}
static bool IsDesktopState36(RFLobbyDesktop* desktop) {
	int32_t state = desktop->GetLobbyState();
	if (state == 3 || state == 6) {
		return true;
	}
	return false;
}

// 0x63f390
sCaddieInfo* Jp_SharedDoc_FindMyCaddie(unsigned long id) {
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (id == 0xffffffff) {
		// JP CSharedDoc +0xa26: the KR m_myCaddieId.
		id = doc->m_myCaddieId;
	}
	std::map<unsigned int, sCaddieInfo>::iterator it = doc->m_caddieMap.find(id);
	if (it != doc->m_caddieMap.end() && (it->second.typeId & 0xfc000000) == 0x1c000000) {
		return &it->second;
	}
	return NULL;
}

// 0x55fee0
void LobbyCaddie::OnLoad(void) {
}

// 0x560080
void LobbyCaddie::OnAnotherHand(int nWork, void* pData) {
	(void)nWork;
	(void)pData;
	m_request.m_load();
	m_request.m_built = true;
}

// 0x5600b0
void LobbyCaddie::RequestBuild(uint32_t typeId, int caddieId) {
	if (m_hand.IsWorking(0)) {
		return;
	}
	m_request.m_arg = caddieId;
	m_request.m_characterId = (int)typeId;
	m_request.m_built = false;
	m_request.m_requested = true;
}

// 0x560290
void LobbyCaddie::OnDisplay(void) {
	if (m_caddie.get() != NULL) {
		m_caddie.get()->Display();
	}
}

// 0x5602a0
void LobbyCaddie::SetPositionFromScenePoint(const char* name) {
	// JP CSharedDoc +0x11024's +0x8 (as LobbyPlayer 0x566350).
	WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->FindPoint(name);
	if (point != NULL) {
		m_position = point->m_pos;
	}
}

// 0x560330
void LobbyCaddie::RotateCaddie(float angle) {
	IDummyPet* caddie = m_caddie.get();
	if (caddie == NULL) {
		return;
	}
	// Axis 1: y.
	WMatrix rotation = RotMat(angle, 1);
	m_caddie.get()->m_matrix = rotation * caddie->m_matrix;
}

// 0x5603c0
bool LobbyCaddie::ProcessFade(float dt) {
	IDummyPet* caddie = m_caddie.get();
	if (caddie == NULL || caddie->m_puppet == NULL) {
		return false;
	}
	float from = m_fadeOut ? 255.0f : 0.0f;  // 0xcfec5c
	float to = m_fadeOut ? 0.0f : 255.0f;
	float alpha = Jp_EaseInQuartic(from, to, m_fadeTime, 0.7f);  // 0xd0a3ac
	m_fadeTime += dt;
	if (!m_fadeOut) {
		if (!(alpha < to)) {
			alpha = to;
			m_fadeOut = true;  // one word store (0x560463): the fade ends,
			m_fadeIn = false;  // m_fadeIn is cleared with it
		}
	} else if (alpha <= to) {
		alpha = to;
		m_fadeOut = false;  // one word store (0x560453)
		m_fadeIn = false;
	}
	m_caddie.get()->m_puppet->SetAlpha((unsigned char)(int)alpha);
	return m_fadeOut;
}

// 0x5604b0
bool LobbyCaddie::ProcessMouse(float dt) {
	(void)dt;
	if (!g_jpTooltipFollowEnabled) {
		return false;
	}
	int buttons = Jp_RefreshScene_GetMouseButtons();
	bool released = (buttons & 2) != 0;
	if (released) {
		m_dragging = false;
	}
	if (m_paused || m_caddie.get() == NULL) {
		return false;
	}
	if (!WSingleton<RFSceneManager>::IsInstantiated() || !Jp_RefreshScene_HasInput()) {
		return false;
	}
	if (g_pFresh == NULL || g_pFresh->GetManager() == NULL) {
		return false;
	}
	RFLobbyDesktop* desktop = RFLobbyDesktop::s_instance;
	if (desktop != NULL && (desktop->IsShopState() || IsDesktopState567(desktop)) && !IsDesktopState36(desktop)) {
		return false;
	}
	WVector mouse = WSingleton<RFSceneManager>::Instance()->GetMousePosition();
	WVector delta = g_input->GetMouseDelta();
	if (!Jp_RefreshScene_IsOverWindow(mouse.x, mouse.y)) {
		bool hit = false;
		if (!m_dragging) {
			// 0x565a90 (LobbyPlayer's IsAvatarUnderMouse, inlined here).
			hit = Jp_LobbyPlayer_IsMouseFree(mouse) && Jp_LobbyPlayer_HitTest(mouse, m_position);
		}
		if (m_dragging || hit) {
			if (buttons & 1) {
				m_dragging = true;
			}
			if (m_dragging) {
				Jp_RefreshScene_SetMouseCursor(0xe, true);
				if ((buttons & 0x100) != 0) {
					// 0xe7517c (guard 0xe75188): the previous frame's delta.
					static WVector s_previousDelta(0.0f, 0.0f, 0.0f);
					float average = (float)((s_previousDelta.x + delta.x) * 0.5);  // 0xd66ca0
					s_previousDelta = delta;
					RotateCaddie((float)(average * 0.0069813169));  // 0xd0a3d0: 0.4 degree in radians
				}
				return true;
			}
			if (released) {
				m_dragging = false;
				return true;
			}
			if ((buttons & 4) != 0 && m_caddie.get()->SetMotionByName(kCaddieDefaultMotion)) {
				IDummyPet* caddie = m_caddie.get();
				caddie->SetPosition(&m_position);
				m_caddie.get()->m_motionPlaying = true;
				m_caddie.get()->m_motionHold = true;
				RotateCaddie(0.6f);  // 0xd0a3a4
			}
			Jp_RefreshScene_SetMouseCursor(1, true);
			return true;
		}
	}
	if (!Jp_LobbyDesktop_GetShopCameraZoom()) {
		Jp_RefreshScene_SetMouseCursor(0, false);
	}
	return false;
}

// 0x560a60
bool LobbyCaddie::AttachCaddie() {
	IDummyPet* caddie = m_caddie.get();
	if (caddie == NULL || !caddie->SetMotionByName(kCaddieWaitMotion)) {
		return false;
	}
	RFLobbyDesktop* desktop = RFLobbyDesktop::s_instance;
	if (desktop != NULL && !desktop->IsShopState()) {
		m_caddie.get()->m_effectHost->PlaySequence(kCaddieTalkSequence, &m_position);
	}
	m_caddie.get()->SetPosition(&m_position);
	m_caddie.get()->m_motionPlaying = true;
	m_caddie.get()->m_motionHold = true;
	m_fadeTime = 0.0f;
	m_fadeOut = true;
	m_fadeIn = false;
	RotateCaddie(0.6f);  // 0xd0a3a4
	CSceneManager::Instance()->RegisterElement((eObjType)4, NULL, NULL, m_caddie.get()->m_name.c_str(), 0x18, NULL);
	CSceneManager::Instance()->SetPuppet(m_caddie.get()->m_name.c_str(), m_caddie.get()->m_puppet);
	return true;
}

// 0x560d70
void LobbyCaddie::OnProcess(float dt) {
	if (m_request.m_requested) {
		m_request.m_requested = false;
		if (!m_hand.IsWorking(0)) {
			m_hand.Work(this, 0, NULL, true, -1);
		}
	}
	if (m_caddie.get() != NULL && m_attachPending) {
		AttachCaddie();
		m_attachPending = false;
	}
	if (m_caddie.get() != NULL) {
		m_caddie.get()->Process(dt);
		bool zoom = ProcessMouse(dt);
		if (RFLobbyDesktop::s_instance != NULL) {
			Jp_LobbyDesktop_SetCaddieCameraZoom(zoom);
		}
	}
	if (m_fadeIn) {
		ProcessFade(dt);
	}
}

// 0x560ff0 (scalar deleting destructor; body 0x560c90): the hand, the
// request, the caddie pointer and IActor are destroyed implicitly.
LobbyCaddie::~LobbyCaddie() {
	DeleteCriticalSection(&m_lock);
}

// 0x561180
LobbyCaddie::LobbyCaddie()
	: m_caddie(static_cast<IDummyPet*>(NULL)),
	  m_position(WVector::ZERO) {
	m_fadeTime = 0.0f;
	m_attachPending = true;
	m_caddieTid = 0;
	m_caddieId = 0;
	m_fadeOut = false;
	m_fadeIn = false;
	m_paused = false;
	m_dragging = false;
	m_unknown48 = 0;
	InitializeCriticalSection(&m_lock);
}

// 0x561240
bool LobbyCaddie::Build(int caddieId, uint32_t typeId) {
	if (m_caddie.get() != NULL) {
		return false;
	}
	IDummyPet* caddie = new IDummyPet(0);
	if (caddie == NULL) {
		return false;
	}
	CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
	if (caddieId == -1) {
		caddieId = 0;
	}
	if (caddieId == 0 && typeId == 0) {
		// JP CSharedDoc +0xa26: the KR m_myCaddieId.
		caddieId = (int)doc->m_myCaddieId;
	}
	sCaddieInfo* info = Jp_SharedDoc_FindMyCaddie((unsigned long)caddieId);
	if (info != NULL) {
		// JP CSharedDoc +0x8cb: the KR MyUID() (Jp_GameState_AvatarNameKey).
		caddie->m_name = MakeStr("LOBBY_CADDIE_%d_%d", Jp_GameState_AvatarNameKey(), info->typeId);
		if (!caddie->BuildCaddie(info->typeId, info->cadItemTypeId, true)) {
			delete caddie;
			return false;
		}
		m_caddieId = caddieId;
		m_caddieTid = info->typeId;
	} else {
		if (typeId == 0) {
			delete caddie;
			return false;
		}
		caddie->m_name = MakeStr("LOBBY_CADDIE_%d_%d", Jp_GameState_AvatarNameKey(), typeId);
		// JP CSharedDoc +0x20c: the item manager.
		uint32_t cadItemTypeId;
		if (ItemManager()->FindCaddie(typeId) != NULL) {
			cadItemTypeId = 0;
		} else {
			// A caddie item: its caddie is the type id built from the item's
			// two 5-bit fields.
			IFF_STRUCT::sCadItem* item = ItemManager()->FindCadItem(typeId);
			IFF_STRUCT::sCaddie* owner = ItemManager()->FindCaddie((((typeId >> 21) & 0x1f) + ((typeId >> 16) & 0x1f)) | 0x1c000000);
			typeId = owner->m_typeId;
			cadItemTypeId = item->m_typeId;
		}
		if (!caddie->BuildCaddie(typeId, cadItemTypeId, true)) {
			delete caddie;
			return false;
		}
		m_caddieTid = typeId;
		m_caddieId = 0;
	}
	EnterCriticalSection(&m_lock);
	m_caddie = RF_SHARED_PTR<IDummyPet>(caddie);
	m_attachPending = true;
	LeaveCriticalSection(&m_lock);
	return true;
}

// 0x561490
void LobbyCaddie::ResetCaddie(int unused) {
	(void)unused;
	if (m_caddie.get() != NULL) {
		CSceneManager::Instance()->DeleteElement(m_caddie.get()->m_name.c_str());
	}
	m_caddie.reset();
	m_caddie = RF_SHARED_PTR<IDummyPet>(static_cast<IDummyPet*>(NULL));
	m_attachPending = false;
	m_caddieTid = 0;
	m_caddieId = 0;
	m_request.m_built = true;
	if (m_hand.IsWorking(0)) {
		m_hand.WaitUntilWorkDone(0xffffffff);
	}
	Jp_ReleaseThreadResources(m_hand.GetThreadID());
}

// 0x561600
IObject* LobbyCaddieMakeInstance(void) {
	return new LobbyCaddie;
}

// 0x561670: the registration object's constructor (the KR IMPLEMENT
// pattern of LobbyBg.cpp: ObjectFactory 0xa4c0d0 / AddObjectFunctor
// 0xa4bcf0, ActorFactory 0xa471a0 / AddActorClass 0xa46aa0; the static's
// initializer is 0xcce720).
struct __impLobbyCaddie {
	__impLobbyCaddie(void) {
		ObjectFactory().AddObjectFunctor(LobbyCaddieMakeInstance, "LobbyCaddie");
		ActorFactory().AddActorClass("LobbyCaddie");
	}
};
static __impLobbyCaddie s_impLobbyCaddie;

// 0x5616a0
void LobbyCaddie::OnDestroy(void) {
	ResetCaddie(0);
	m_attachPending = true;
	m_position = WVector::ZERO;
}

// 0x5616d0
void LobbyCaddie::LoadCaddie() {
	SendToLobby(0x53);
	Build(m_request.m_arg, (uint32_t)m_request.m_characterId);
	SendToLobby(0x54);
}

// 0x561790
bool LobbyCaddie::Load() {
	if (m_request.m_built) {
		return false;
	}
	LoadCaddie();
	Sleep(15);
	return true;
}

// 0x5617b0
void LobbyCaddie::OnPreLoadInit(void) {
	m_request.m_load = std::bind(&LobbyCaddie::Load, this);  // bind 0x560160, function assign 0x5610b0
	m_request.m_built = false;
	m_request.m_arg = 0;
	m_request.m_characterId = 0;
}

// 0x561820
void LobbyCaddie::HandleMsg(const MsgObject& msg) {
	switch (msg.m_msg) {
	// 0x180: the caddie's motion reset.
	case 0x180:
		if (m_caddie.get() != NULL) {
			m_caddie.get()->ResetMotion(0, 0, true);
		}
		break;
	case 0x184:
		OnDestroy();
		break;
	// 0x193: the caddie equip change (param1 int*: the new caddie id, param2
	// passed on in message 0x49 to the task's main actor).
	case 0x193: {
		int caddieId = msg.m_param1 != 0 ? *reinterpret_cast<const int*>(msg.m_param1) : 0;
		intptr_t param2 = msg.m_param2;
		// JP CSharedDoc +0xa26: the KR m_myCaddieId.
		if (caddieId == (int)WSingleton<CSharedDoc>::Instance()->m_myCaddieId) {
			break;
		}
		static_cast<IActor*>(this) << MsgObject(NULL, 0x1a6, 0, 0, 0, 0, 0);
		if (caddieId != 0) {
			static_cast<IActor*>(this) << MsgObject(NULL, 0x1a5, reinterpret_cast<intptr_t>(&caddieId), 0, 0, 0, 0);
		}
		AfxGetTask()->PassMsgToMainActor(MsgObject(NULL, 0x49, 0, param2, 0, 0, 0));
		break;
	}
	// 0x1a5: build the owned caddie (param1 int*: its id; none: the
	// equipped one).
	case 0x1a5:
		if (WisEqual(m_position, WVector::ZERO, 1e-5f)) {  // 0xd0a46c
			SetPositionFromScenePoint("Caddie");
		}
		if (msg.m_param1 != 0) {
			RequestBuild(0, *reinterpret_cast<const int*>(msg.m_param1));
		} else {
			RequestBuild(0, (int)WSingleton<CSharedDoc>::Instance()->m_myCaddieId);
		}
		break;
	case 0x1a6:
		ResetCaddie(0);
		break;
	// 0x1a7: the caddie preview (param1 uint32_t*: the caddie or caddie
	// item type id; none: the equipped caddie's).
	case 0x1a7: {
		uint32_t typeId = msg.m_param1 != 0 ? *reinterpret_cast<const uint32_t*>(msg.m_param1) : 0;
		if ((uint32_t)m_caddieTid == typeId) {
			break;
		}
		if (typeId == 0) {
			sCaddieInfo* info = Jp_SharedDoc_FindMyCaddie(WSingleton<CSharedDoc>::Instance()->m_myCaddieId);
			if (info != NULL) {
				typeId = info->typeId;
			}
		}
		if (m_caddieTid != 0) {
			ResetCaddie(1);
		}
		if (typeId != 0 && typeId != 0xffffffff) {
			RequestBuild(typeId, 0);
		}
		break;
	}
	case 0x1a8:
		ResetCaddie(1);
		break;
	// 0x1a9: rebuild the equipped caddie.
	case 0x1a9:
		static_cast<IActor*>(this) << MsgObject(NULL, 0x1a6, 0, 0, 0, 0, 0);
		static_cast<IActor*>(this) << MsgObject(NULL, 0x1a5, reinterpret_cast<intptr_t>(&WSingleton<CSharedDoc>::Instance()->m_myCaddieId), 0, 0, 0, 0);
		break;
	// 0x1ce / 0x1cf: the shop's ChangeMainTab: the caddie fully shown.
	case 0x1ce:
	case 0x1cf:
		if (m_caddie.get() != NULL && m_caddie.get()->m_puppet != NULL) {
			m_caddie.get()->m_puppet->SetAlpha((unsigned char)0xff);
		}
		break;
	case 0x2ae:
		m_paused = true;
		break;
	case 0x2af:
		m_paused = false;
		break;
	}
}
