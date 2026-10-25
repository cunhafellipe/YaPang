// Original: d:\build\custom\temp\code\client\projectg\lobbybg.cpp
#include "lobbybg.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "TaskManager.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "ClientSetting.h"
#include "scenemanager.h"
#include "Shadow.h"
#include "sky.h"
#include "wind.h"
#include "wresrcmng.h"
#include "wview.h"
#include "fx.h"
#include "commonutil.h"
#include "quadtree.h"

extern bool g_bQuit;
extern unsigned long g_CurrentTime;
extern LightSet g_lightset;
#ifdef JP_PAKS
extern WMatrix g_camera;

// JP ntLobbyBg +0x5c (a bool): a bar-state camera move (message 0x17f) is
// running; OnProcess 0xab1d90 ends it with message 0x180 once the +0x38
// camera animator is done. The JP object is larger than the KR 0x18 (the
// animator, +0x40 the camera's name), so the port keeps the state here (the
// lobby has one ntLobbyBg). Without the animator the port's move lasts one
// whole OnProcess: 1 = started, 2 = one OnProcess seen; the next one ends
// it. The desktop's "Lobby" 0x88 (posted, delivered at the start of the
// next frame) then always comes before the 0x180, as during the JP's
// several-frame move.
static int s_jpCameraMove = 0;

// JP 0xe36a90 (initialized by 0xcec500): the JP lobby's terrace map. The JP
// ntLobbyBg::OnLoad 0xab23d0 loads it whatever the lobby course option is.
static const char s_jpLobbyMapFile[] = "lobby_summer_night.gbin";
// JP ntLobbyBg +0x18 / +0x1c (ctor 0xab20f0: 0, "blue"): the stars index
// and the name of the sky, wave, river and cloud files, set by message
// 0x17a (CLobbyMain 0xab9720 sends 6, "moon"). Kept here like
// s_jpCameraMove (the KR object is 0x18 bytes).
static int s_jpSkyIndex = 0;
static std::string s_jpSkyName("blue");

// JP 0xab1bd0 (ntLobbyBg::LoadBg, the shape of CRealMyRoomMain::LoadBg):
// the sky (created once), wave, river and clouds of s_jpSkyName.
static void Jp_LobbyBg_LoadBg(void) {
	if (GOLFDOC()->m_pSky == NULL)
		GOLFDOC()->m_pSky = new CSky;

	GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", s_jpSkyName.c_str()));
	GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", s_jpSkyName.c_str()));

	if (GOLFDOC()->m_pSky) {
		GOLFDOC()->m_pSky->LoadSky(s_jpSkyName.c_str(), 0.0f);
		GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", s_jpSkyName.c_str()));
		GOLFDOC()->m_pPolySoup->MakeDataStructure();
		GOLFDOC()->m_pSky->GenerateStars(s_jpSkyIndex);
		GOLFDOC()->m_pSky->GenerateMoons();
		GOLFDOC()->m_pSky->GenerateLensFlare();
	}

	CShadowManager::Instance()->UpdateLight("sun", g_lightset);

	CSceneManager::Instance()->Load();
}
#endif

// lobbybg.cpp:41 (run-time type and factory registration of ntLobbyBg)
const WRTTI ntLobbyBg::m_RTTI("ntLobbyBg", &IActor::m_RTTI);

IObject* ntLobbyBgMakeInstance(void) {
	return new ntLobbyBg;
}

struct __impntLobbyBg {
	__impntLobbyBg(void) {
		ObjectFactory().AddObjectFunctor(ntLobbyBgMakeInstance, "ntLobbyBg");
		ActorFactory().AddActorClass("ntLobbyBg");
	}
};
static __impntLobbyBg s_impntLobbyBg;

// lobbybg.cpp:45
void ntLobbyBg::OnPreLoadInit(void) {
	new CShadowManager(g_resrcmng, COption::Instance()->vIsShadowEnabled());

	if (LOBBYBG_MAP == 13) {
		g_view->SetClip(0.1f, 26214.4f, true);
	}
}

// lobbybg.cpp:60
#ifdef JP_PAKS
// JP 0xab23d0. Not ported: the +0x38 camera animator (new 0x20, ctor
// 0x4864b0, see s_jpCameraMove) and the handles of the "lobby_post01.pet" /
// "lobby_post02.pet" signs (WPolySoup 0x92c0a0 -> +0x68 / +0x6c, +0x5d set)
// whose textures messages 0x182 / 0x183 change (0xab1b10).
void ntLobbyBg::OnLoad(void) {
	if (CFx::Instance()) {
		CFx::Instance()->SetActive(false);
	}

	if (!GOLFDOC()->m_pPolySoup->Load(s_jpLobbyMapFile, false)) {
		g_bQuit = true;
		return;
	}

	Jp_LobbyBg_LoadBg();

	GOLFDOC()->m_pPolySoup->EndLoad(false);

	if (CFx::Instance()) {
		CFx::Instance()->SetActive(true);
	}
}
#else
void ntLobbyBg::OnLoad(void) {
	if (CFx::Instance()) {
		CFx::Instance()->SetActive(false);
	}

	if (!GOLFDOC()->m_pPolySoup->Load(MakeStr("%s_rank.gbin", LOBBYBG_MAPNAME), false)) {
		g_bQuit = true;
		return;
	}

	GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", LOBBYBG_MAPNAME));
	GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", LOBBYBG_MAPNAME));

	GOLFDOC()->m_pSky = new CSky;
	GOLFDOC()->m_pSky->LoadSky(LOBBYBG_MAPNAME, 0.0f);
	GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", LOBBYBG_MAPNAME));

	GOLFDOC()->m_pPolySoup->MakeDataStructure();

	if (GOLFDOC()->m_pSky) {
		GOLFDOC()->m_pSky->GenerateStars(LOBBYBG_MAP);
		GOLFDOC()->m_pSky->GenerateLensFlare();
		GOLFDOC()->m_pSky->GenerateMoons();
	}

	CShadowManager::Instance()->UpdateLight("sun", g_lightset);

	CSceneManager::Instance()->Load();

	GOLFDOC()->m_pPolySoup->EndLoad(false);

	if (CFx::Instance()) {
		CFx::Instance()->SetActive(true);
	}
}
#endif

// lobbybg.cpp:104
void ntLobbyBg::OnInit(void) {
	CSceneManager::Instance()->UpdateFog(LOBBYBG_MAPNAME);

	Wind().CheckSpecialWind();
}

// lobbybg.cpp:112
void ntLobbyBg::OnDestroy(void) {
	if (CShadowManager::Instance()) {
		delete CShadowManager::Instance();
	}
}

// lobbybg.cpp:118
void ntLobbyBg::HandleMsg(const MsgObject& msg) {
	if ((unsigned long)msg.m_reserved > g_CurrentTime) {
		StoreDelayedMsg(msg);
		return;
	}

	switch (msg.m_msg) {
	case 317:
		GOLFDOC()->m_pSky->GenerateClouds(50, Wind().GetGlobalIntensity());
		break;

	case 318:
		GOLFDOC()->m_pSky->LoadSky(LOBBYBG_MAPNAME, 0.0f);
		GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", LOBBYBG_MAPNAME));
		GOLFDOC()->m_pSky->GenerateStars(LOBBYBG_MAP);
		GOLFDOC()->m_pSky->GenerateMoons();
		GOLFDOC()->m_pSky->GenerateLensFlare();
		break;

	case 319:
		GOLFDOC()->m_pSky->SetColor(msg.m_param1);
		break;

	case 320:
		DoReLoad();
		break;

	case 280:
		GOLFDOC()->m_pPolySoup->SetRenderMode(msg.m_param1);
		break;

	case 321:
		if (GOLFDOC()->m_pPolySoup) {
			GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
		}
		break;
#ifdef JP_PAKS
	// JP 0xab25f5: the stars index and the sky name (param1, param2);
	// param3 nonzero reloads the sky at once (LoadBg 0xab1bd0).
	case 0x17A:
		s_jpSkyIndex = msg.m_param1;
		s_jpSkyName = msg.m_param2 ? (const char*)msg.m_param2 : "";
		if (msg.m_param3)
			Jp_LobbyBg_LoadBg();
		break;

	// JP 0xab2683: RFLobbyDesktop::UpdateLobbyCamera 0x5f8020 moves the
	// camera to a bar state's camera (param1 the camera, param2 the camera
	// to start from or "", param3 nonzero = animated). The same name ends at
	// once with 0x180. Animated, the JP runs its +0x38 camera animator
	// (0x485c90 / 0x485d50 / 0x485d60 / 0x485c80, from param2's camera or
	// g_camera to param1's) and OnProcess sends 0x180 when it is done; that
	// animator is not ported: the camera is set at once (the JP's
	// non-animated path) and the move ends one OnProcess later (see
	// s_jpCameraMove), so 0x180 arrives after the desktop's m_busy is set
	// and after its posted 0x88.
	case 0x17F: {
		std::string name(msg.m_param1 ? (const char*)msg.m_param1 : "");
		std::string from(msg.m_param2 ? (const char*)msg.m_param2 : "");
		bool animated = msg.m_param3 != 0;
		s_jpCameraMove = 0;
		if (name == from) {
			HandleMsg(MsgObject(0, 0x180, 0, 0, 0, 0, 0));
			break;
		}
		WPolySoup::sCamera* camera = GOLFDOC()->m_pPolySoup->FindCamera(name.c_str());
		if (camera == NULL)
			break;
		g_camera = camera->m_matrix;
		g_view->SetFOV(camera->m_fov * 0.017453292f);
		if (animated)
			s_jpCameraMove = 1;
		else
			HandleMsg(MsgObject(0, 0x180, 0, 0, 0, 0, 0));
		break;
	}

	// JP 0xab285e: the move ended; the task's main actor (CLobbyMain) hands
	// it to RFLobbyDesktop::HandleMsg 0x180 through HandleMsgCommon.
	case 0x180:
		s_jpCameraMove = 0;
		AfxGetTask()->SendMsgToMainActor(MsgObject(0, 0x180, 0, 0, 0, 0, 0));
		break;
#endif
	}
}

// lobbybg.cpp:162
ntLobbyBg::ntLobbyBg(void) {
	m_priority = 1;
}

// lobbybg.cpp:168
void ntLobbyBg::DoReLoad(void) {
	SendMsg(this, "Ambient", 500, 0, 0, 0, 0);

	OnLoad();

	CTaskManager *pTaskManager = CTaskManager::Instance();
	(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->Init(0);
}

// lobbybg.cpp:178
void ntLobbyBg::SetSkyColor(unsigned long color) {
	if (GOLFDOC()->m_pSky) {
		GOLFDOC()->m_pSky->SetColor(color);
	}
}

// lobbybg.cpp:275
void ntLobbyBg::OnProcess(float elapsed) {
	GetPVS().PreProcess(elapsed, false);
#ifdef JP_PAKS
	// JP 0xab1dd8: a finished camera move sends this actor 0x180 (the
	// animator's done test 0x485e80; no animator in the port, see 0x17F).
	if (s_jpCameraMove == 1)
		s_jpCameraMove = 2;
	else if (s_jpCameraMove == 2)
		HandleMsg(MsgObject(0, 0x180, 0, 0, 0, 0, 0));
#endif
}
