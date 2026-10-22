// Original: d:\build\custom\temp\code\client\projectg\avatar_main.cpp
#define AVATAR_MAIN_SPECIALPRIZEITEM_INLINE
#include "NetworkSystem.h"
#include <string.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include "avatar_main.h"
#include "avatar_avatar.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "TaskManager.h"
#include "SoundManager.h"
#include "JukeBoxDlg.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "quadtree.h"
#include "ClientSetting.h"
#include "fresh.h"
#include "fredit.h"
#include "Shadow.h"
#include "ItemManager.h"
#include "wresrcmng.h"
#include "mousecursor.h"
#include "scenemanager.h"
#include "ChatMsg.h"
#include "InputManager.h"
#include "frwndmanager.h"
#include "frdesktop.h"
#include "UccManager.h"
#include "woverlay.h"
#include "fx.h"
#include "wfont.h"
#include "wview.h"
#include "fxsequence.h"
#include "s5/specialprize/specialprizeitem.h"
#include "UccLibrary.h"
#include "bitmap.h"
#include "balloontext.h"
#include "avatar_task.h"
#include "AvatarUserListDlg.h"
#include "frcombobox.h"
#include "frframe.h"
#include "AvatarChatLogDlg.h"
#include "frscrollbar.h"
#include "user_info.h"
#include "polysoup.h"
#include "sky.h"
#include "wind.h"
#include "messengerdlg.h"
#include "commonutil.h"
#include "s5/Utilities.h"
#include "pg_boot_todo.h"
#include "ProjectG.h"
#include "QuitWindow.h"
#include "frgraphicinterface.h"
#include "client/Wangreal/include/wutil.h"

// The original's max compiles as `a < b ? b : a` (wutil Max uses >=); its
// header is unknown, as for river.cpp's MaxOf.
template <class T> T MaxOf(T a, T b) { return a < b ? b : a; }

extern bool g_SALoadingState;
extern WMatrix g_camera;
extern bool g_bQuit;          // ProjectG.cpp
extern unsigned long g_CurrentTime;
extern LightSet g_lightset;

float Random(float min, float max, int seed); // commonutil.cpp:1106

// avatar_main.cpp:103 (run-time type and factory registration of ntAvatarChatMain)
const WRTTI ntAvatarChatMain::m_RTTI("ntAvatarChatMain", &CTaskMain::m_RTTI);

IObject *ntAvatarChatMainMakeInstance(void) {
	return new ntAvatarChatMain;
}

struct __impntAvatarChatMain {
	__impntAvatarChatMain(void) {
		ObjectFactory().AddObjectFunctor(ntAvatarChatMainMakeInstance, "ntAvatarChatMain");
		ActorFactory().AddActorClass("ntAvatarChatMain");
	}
};
static __impntAvatarChatMain s_impntAvatarChatMain;

// avatar_main.cpp:106
ntAvatarChatMain::ntAvatarChatMain(void) {
	m_priority = 4;

	m_pPVS = 0;
	m_pPVS = &::GetPVS();

	m_preservedMap = 0;
	m_preservedHole = 0;

	m_decalTime = 0.0f;
	m_cameraOffset = WVector::ZERO;
	m_cameraAngle = WVector(-0.48869219f, 0.0f, 0.0f);
	m_cameraDistance = 28.800001f;
	m_cameraDistanceGoal = 28.800001f;
	m_cameraClipDistance = 28.800001f;
	m_cameraHeight = 4.6f;
	m_cameraHeightGoal = 4.6f;
	m_idleTime = 0.0f;
	m_idleMark = 0.0f;
	m_cameraRayHit = 1.0f;
	m_topViewMode = false;
	m_topViewHeight = 256.0f;
	m_showTargetDecal = false;

	m_glowBallTexture = g_resrcmng->LoadTexture("~GlowBall.jpg", 0, 0, 0);
	m_initFlag = false;

	new ntBalloonTextManager;
	ntBalloonTextManager::Instance()->Init();

	m_pTitleFont = g_resrcmng->GetTitleFont();

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.fonth = 16;
	info.fontw = 16;

	char *fontName = "[font_wind.jpg";
	info.filename = &fontName;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:, ";
	m_pTitleFont->Create(&info);

	InitUI();

	m_spItemChanged = false;
	m_targetUid = 0xFFFFFFFF;
}

// avatar_main.cpp:176
ntAvatarChatMain::~ntAvatarChatMain(void) {
	g_audio->StopSfx("AmbientSnd");

	ntBalloonTextManager::Instance()->Shutdown();
	if (ntBalloonTextManager::IsInstantiated()) delete ntBalloonTextManager::Instance();

	if (g_resrcmng && m_glowBallTexture) { g_resrcmng->Release(m_glowBallTexture); m_glowBallTexture = 0; }

	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = 0; }

	for (int i = 0; i < 10; i++) {
		ClearTexCache(m_portraitBitmaps[i]);
		delete m_portraitBitmaps[i];
		m_portraitBitmaps[i] = 0;
	}
}

// avatar_main.cpp:198
void ntAvatarChatMain::OnPreLoadInit(void) {
	CTaskMain::OnPreLoadInit();
	new CShadowManager(g_resrcmng, COption::Instance()->vIsShadowEnabled());

	GOLFDOC()->MakePlayer(1);
}

// avatar_main.cpp:310
void ntAvatarChatMain::OnLoad(void) {
	sSlotInfo slotInfo;
	sCharacterInfo charInfo;

	CTaskMain::OnLoad();

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		SetCurMap(CSharedDoc::Instance()->m_roomInfo.m_course);

		LoadBg(CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0, CSharedDoc::Instance()->m_roomInfo.m_holeCount);

		WPolySoup::sCamera *camera = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
		if (camera) {
			g_camera = camera->m_matrix;
			g_view->SetFOV(camera->m_fov * g_DEGTORAD);
		}

		g_audio->StopBGM(false);
		if (GetCurMapProp() && GetCurMapProp()->m_ambientSound[0]) {
			g_audio->StopSfx("AmbientSnd");
			g_audio->PlaySfx(GetCurMapProp()->m_ambientSound, 0, 1, "AmbientSnd", 0, 0.5f, 200.0f);
		}

		CSceneManager::Instance()->SetWeather(0, false);

		ntAvatarManager::Instance()->FreeAllAvatars();
	} else {
		SetCurMap(COption::Instance()->dGetMap());

		LoadBg(CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0, COption::Instance()->dGetStartHole());

		WPolySoup::sCamera *camera = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
		if (camera) {
			g_camera = camera->m_matrix;
			g_view->SetFOV(camera->m_fov * g_DEGTORAD);
		}

		g_audio->StopBGM(false);
		if (GetCurMapProp() && GetCurMapProp()->m_ambientSound[0]) {
			g_audio->StopSfx("AmbientSnd");
			g_audio->PlaySfx(GetCurMapProp()->m_ambientSound, 0, 1, "AmbientSnd", 0, 0.5f, 200.0f);
		}

		CSceneManager::Instance()->SetWeather(COption::Instance()->dGetWeather(), false);

		ntAvatarManager::Instance()->FreeAllAvatars();

		// "\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb": the chat room's start point; "\xbd\xc3\xc0\xdb\xc1\xa1": the course's
		WPolySoup::sPoint *point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));
		if (point == 0) {
			point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xbd\xc3\xc0\xdb\xc1\xa1"));
		}

		WVector pos;
		pos = GetPVS().GetGroundPoint(point->m_pos, false, 0, false);

		memset(&slotInfo, 0, sizeof(slotInfo));
		slotInfo.m_angle = Random(0.0f, 6.2831855f, -1);
		slotInfo.m_attribute = (slotInfo.m_attribute & ~0x18) | 0x08;  // bits 3-4: 1 (the room master)
		slotInfo.m_uid = 0;
		slotInfo.m_titleTypeId = 0;
		slotInfo.m_level = 1;
		rand();
		slotInfo.m_x = pos.x;
		slotInfo.m_z = pos.z;
		slotInfo.m_attributeBits.userFlag = 0;
		strcpy(slotInfo.m_nickname, "jufoot");
		slotInfo.m_characterTypeId = COption::Instance()->dGetDefaultCharacter() | 0x4000000;
		CSharedDoc::Instance()->m_roomPlayerList.push_back(slotInfo);

		memset(&charInfo, 0, sizeof(charInfo));
		charInfo.typeId = slotInfo.m_characterTypeId;

		ntAvatarManager::Instance()->m_pMyAvatar = ntAvatarManager::Instance()->CreateAvatar(slotInfo, charInfo, this);

		static_cast<ntAvatarChatTask *>(GetTask())->AddAvatarBurden(ntAvatarBurden(0, 0, charInfo, 0));

		for (int i = 1; i < COption::Instance()->dGetNumPlayers(); i++) {
			point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));
			if (point == 0) {
				point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xbd\xc3\xc0\xdb\xc1\xa1"));
			}

			pos = point->m_pos;
			pos.x += Random(-16.0f, 16.0f, -1);
			pos.z += Random(-16.0f, 16.0f, -1);
			pos = GetPVS().GetGroundPoint(pos, false, 0, false);

			memset(&slotInfo, 0, sizeof(slotInfo));
			slotInfo.m_angle = Random(0.0f, 6.2831855f, -1);
			slotInfo.m_attribute &= ~0x18;
			slotInfo.m_uid = i;
			slotInfo.m_titleTypeId = 0;
			slotInfo.m_level = 1;
			rand();
			slotInfo.m_x = pos.x;
			slotInfo.m_z = pos.z;
			slotInfo.m_attributeBits.userFlag = 0;
			strcpy(slotInfo.m_nickname, MakeStr("dummy%2d", i));
			slotInfo.m_characterTypeId = rand() % 7 | 0x4000000;
			CSharedDoc::Instance()->m_roomPlayerList.push_back(slotInfo);

			memset(&charInfo, 0, sizeof(charInfo));
			charInfo.typeId = slotInfo.m_characterTypeId;

			_util::GenerateRandomTidParts(charInfo.typeId, charInfo.partTypeIds, charInfo.auxParts.tids);

			ntAvatarManager::Instance()->CreateAvatar(slotInfo, charInfo, this);

			static_cast<ntAvatarChatTask *>(GetTask())->AddAvatarBurden(ntAvatarBurden(i, i, charInfo, 0));
		}
	}

	GOLFDOC()->m_pSky->GenerateClouds(50, Wind().GetGlobalIntensity());

	Wind().Init(0xFF, 0xFF, 0, 1);

	Wind().CheckSpecialWind();
	Wind().SetWind(WVector(1.0f, 0.0f, 1.0f));
	GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
	CSceneManager::Instance()->UpdateWind();

	m_genderIcons[0] = g_resrcmng->GetOverlay("i_male.tga", 0);
	m_genderIcons[1] = g_resrcmng->GetOverlay("i_female.tga", 0);
	m_genderIcons[2] = g_resrcmng->GetOverlay("i_male_02.tga", 0);
	m_genderIcons[3] = g_resrcmng->GetOverlay("i_female_02.tga", 0);
	m_genderIcons[4] = g_resrcmng->GetOverlay("i_male_03.tga", 0);
	m_genderIcons[5] = g_resrcmng->GetOverlay("i_female_03.tga", 0);

	m_mannerIcons[0] = g_resrcmng->GetOverlay("i_male_manner.tga", 0);
	m_mannerIcons[1] = g_resrcmng->GetOverlay("i_female_manner.tga", 0);

	m_angelIcons[0] = g_resrcmng->GetOverlay("i_male_angel.tga", 0);
	m_angelIcons[1] = g_resrcmng->GetOverlay("i_female_angel.tga", 0);

	m_diveIcon = g_resrcmng->GetOverlay("dive.tga", 0);
	m_stateIcon01 = g_resrcmng->GetOverlay("state_icon_01.tga", 0);
	m_stateIcon02 = g_resrcmng->GetOverlay("state_icon_02.tga", 0);
	m_stateIcon03 = g_resrcmng->GetOverlay("state_icon_03.tga", 0);
	m_stateIcon04 = g_resrcmng->GetOverlay("state_icon_04.tga", 0);
	m_stateIcon05 = g_resrcmng->GetOverlay("state_icon_05.tga", 0);
	m_stateIcon06 = g_resrcmng->GetOverlay("state_icon_06.tga", 0);
	m_stateIcon01N = g_resrcmng->GetOverlay("stateicon_01_n.tga", 0);
	m_stateIcon02N = g_resrcmng->GetOverlay("stateicon_02_n.tga", 0);
	m_stateIcon03N = g_resrcmng->GetOverlay("stateicon_03_n.tga", 0);
	m_stateIcon04N = g_resrcmng->GetOverlay("stateicon_04_n.tga", 0);

	m_emotiObject = g_resrcmng->GetOverlay("[emoti_object.png", 0);

	for (unsigned int bar = 0; bar < 9; bar++)
		m_stateBars[bar] = g_resrcmng->GetOverlay(MakeStr("statebar_%02d.tga", bar + 1), 0);

	m_shadowTexture = g_resrcmng->LoadTexture("[shadow.jpg", 0, 0, 0);

	// The original toggles CSceneManager::m_overlapCamera (0x128) directly (no ToggleTransparent
	// copy in this unit; ntAvatarChatMain is presumably a friend of CSceneManager).
	CSceneManager::Instance()->ToggleTransparent();

	if (CMessengerInfo::Instance()->m_myStatus != 3)
		CMessengerInfo::Instance()->SetMyStatus((MSN::eStatusType)0);
}

// avatar_main.cpp:508
void ntAvatarChatMain::DoReLoad(void) {
	SendMsg(this, "Ambient", 500, 0, 0, 0, 0);

	OnLoad();

	CTaskManager *pTaskManager = CTaskManager::Instance();
	(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->Init(0);
}

// avatar_main.cpp:517
void ntAvatarChatMain::OnInit(void) {
	CTaskMain::OnInit();

	SetBGMNone();

	CalcUIOffset(false);

	if (CMouseCursor::IsInstantiated()) {
		CMouseCursor::Instance()->SetActive(true);
		CMouseCursor::Instance()->m_mode = 2;
	}

	CSceneManager::Instance()->UpdateFog(true);
	CSceneManager::Instance()->RegisterElement((eObjType)4, 0, this, "\xbe\xc6\xb9\xd9\xc5\xb8\xb8\xde\xc0\xce", 0, 0);

	if (CChatMsg::Instance()) {
		CChatMsg::Instance()->Close(true, true);
		CChatMsg::Instance()->EnableCaretMove(true);
	}

	if (!CChatMsg::Instance()->IsOpened()) {
		g_pFresh->GetManager()->SetExclusiveKey(false);
	}

	FrJukeBoxDlg::Init();

	FrJukeBoxDlg::PlayWithMap(GetCurMap());

	g_input->Reset();
}

// avatar_main.cpp:550
void ntAvatarChatMain::OnDestroy(void) {
	CTaskMain::OnDestroy();

	if (CShadowManager::Instance()) {
		delete CShadowManager::Instance();
	}

	for (int gender = 0; gender < 6; gender++) {
		if (g_resrcmng && m_genderIcons[gender]) { g_resrcmng->Release(m_genderIcons[gender]); m_genderIcons[gender] = 0; }
	}
	for (int manner = 0; manner < 2; manner++) {
		if (g_resrcmng && m_mannerIcons[manner]) { g_resrcmng->Release(m_mannerIcons[manner]); m_mannerIcons[manner] = 0; }
	}
	for (int angel = 0; angel < 2; angel++) {
		if (g_resrcmng && m_angelIcons[angel]) { g_resrcmng->Release(m_angelIcons[angel]); m_angelIcons[angel] = 0; }
	}

	if (g_resrcmng && m_diveIcon) { g_resrcmng->Release(m_diveIcon); m_diveIcon = 0; }
	if (g_resrcmng && m_stateIcon01) { g_resrcmng->Release(m_stateIcon01); m_stateIcon01 = 0; }
	if (g_resrcmng && m_stateIcon02) { g_resrcmng->Release(m_stateIcon02); m_stateIcon02 = 0; }
	if (g_resrcmng && m_stateIcon03) { g_resrcmng->Release(m_stateIcon03); m_stateIcon03 = 0; }
	if (g_resrcmng && m_stateIcon04) { g_resrcmng->Release(m_stateIcon04); m_stateIcon04 = 0; }
	if (g_resrcmng && m_stateIcon05) { g_resrcmng->Release(m_stateIcon05); m_stateIcon05 = 0; }
	if (g_resrcmng && m_stateIcon06) { g_resrcmng->Release(m_stateIcon06); m_stateIcon06 = 0; }
	if (g_resrcmng && m_stateIcon01N) { g_resrcmng->Release(m_stateIcon01N); m_stateIcon01N = 0; }
	if (g_resrcmng && m_stateIcon02N) { g_resrcmng->Release(m_stateIcon02N); m_stateIcon02N = 0; }
	if (g_resrcmng && m_stateIcon03N) { g_resrcmng->Release(m_stateIcon03N); m_stateIcon03N = 0; }
	if (g_resrcmng && m_stateIcon04N) { g_resrcmng->Release(m_stateIcon04N); m_stateIcon04N = 0; }
	if (g_resrcmng && m_emotiObject) { g_resrcmng->Release(m_emotiObject); m_emotiObject = 0; }

	for (int bar = 0; bar < 9; bar++) {
		if (g_resrcmng && m_stateBars[bar]) { g_resrcmng->Release(m_stateBars[bar]); m_stateBars[bar] = 0; }
	}

	if (g_resrcmng && m_shadowTexture) { g_resrcmng->Release(m_shadowTexture); m_shadowTexture = 0; }

	// The guids of the avatars still in the lounge, each sent message 0xE5.
	int guids[30];
	int count = 0;
	for (int slot = 0; slot < 30 && count < ntAvatarManager::Instance()->m_avatarCount; slot++) {
		if (ntAvatarManager::Instance()->m_avatars[slot]) {
			guids[count++] = ntAvatarManager::Instance()->m_avatars[slot]->m_guid;
		}
	}

	for (int msgIndex = 0; msgIndex < count; msgIndex++) {
		this << MsgObject(this, 0xE5, guids[msgIndex], 0, 0, 0, 0);
	}

	g_pFresh->GetManager()->SetExclusiveKey(true);
}

// avatar_main.cpp:604
void ntAvatarChatMain::OnPreserveBack(const char *nextTask) {
	if (strcmp(nextTask, "CRealMyRoomTask") == 0) {
		ntAvatarManager::Instance()->m_pMyAvatar->SetState(ntAvatarManager::Instance()->m_pMyAvatar->GetState() | 2);
	} else if (strcmp(nextTask, "CShopTask") == 0) {
		ntAvatarManager::Instance()->m_pMyAvatar->SetState(ntAvatarManager::Instance()->m_pMyAvatar->GetState() | 4);
	}

	ntAvatarManager::Instance()->m_pMyAvatar->Stop();

	g_audio->StopSfx("AmbientSnd");

	FrJukeBoxDlg::Preserve();

	m_preservedMap = GetCurMap();
	m_preservedHole = GOLFDOC()->m_currentHole;

	SetPVSIndex(1);

	ResetFrWndPointers();

	CTaskMain::OnPreserveBack(nextTask);
}

// avatar_main.cpp:632
void ntAvatarChatMain::OnRestorePreserved(void) {
	CTaskMain::OnRestorePreserved();

	g_lightset = m_savedLightSet;

	SetPVSIndex(0);

	SetCurMap(m_preservedMap);
	GOLFDOC()->m_currentHole = m_preservedHole;
	COption::Instance()->vSetMinimumScreenSize(1, 800, 600);
	COption::Instance()->vApplyGameScreenSize();
	ntAvatarManager::Instance()->m_active = true;

	std::map<unsigned int, sCharacterInfo>::iterator it = CSharedDoc::Instance()->m_characterMap.find(CSharedDoc::Instance()->m_myCharacterId);
	if (it != CSharedDoc::Instance()->m_characterMap.end()) {
		// re-sends the equipped character (equipment change, kind 4)
		WSendPacket packet((enumClientPacket)0x0C);
		packet.Encode1(4);
		packet.Encode4(it->second.id);
		packet.Send(SEND_TO_DEFAULT);
	}

	ntAvatarManager::Instance()->ResetAvatarPositionAngle();

	if (CSharedDoc::Instance()->m_pCurCourse && CSharedDoc::Instance()->m_pCurCourse->m_ambientSound[0]) {
		g_audio->PlaySfx(CSharedDoc::Instance()->m_pCurCourse->m_ambientSound, 0, 1, "AmbientSnd", 0, 0.5f, 200.0f);
	}

	SetBGMNone();

	CalcUIOffset(false);

	if (CMouseCursor::IsInstantiated()) {
		CMouseCursor::Instance()->SetActive(true);
		CMouseCursor::Instance()->m_mode = 2;
	}

	FrJukeBoxDlg::Restore();

	ntAvatarManager::Instance()->m_pMyAvatar->SetState(ntAvatarManager::Instance()->m_pMyAvatar->GetState() & ~6);
}

// avatar_main.cpp:685
void ntAvatarChatMain::LoadBg(const char *name, int hole) {
	m_bgName = name;
	GOLFDOC()->m_currentHole = hole;

	if (CFx::Instance()) {
		CFx::Instance()->SetActive(false);
	}

	if (!GOLFDOC()->m_pPolySoup->Load(MakeStr("%s_%02d.gbin", name, GOLFDOC()->m_currentHole), true)) {
		g_bQuit = true;
		return;
	}

	m_savedLightSet = g_lightset;

	GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", name));
	GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", name));

	GOLFDOC()->m_pSky = new CSky;
	GOLFDOC()->m_pSky->LoadSky(name, 0.0f);
	GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", name));

	GOLFDOC()->m_pPolySoup->MakeDataStructure();

	if (GOLFDOC()->m_pSky) {
		GOLFDOC()->m_pSky->GenerateStars(-1);
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

// avatar_main.cpp:733
void ntAvatarChatMain::CheckUIPosition(void) {
	static float s_barY = COption::Instance()->gGetBar_Y();

	if (COption::Instance()->gGetBar_Y() != s_barY) {
		CalcUIOffset(false);
		s_barY = COption::Instance()->gGetBar_Y();

		g_pFresh->OpenLayout("AVATARCHAT_MAIN", this, false);

		if (ntAvatarManager::Instance()->m_pMyAvatar->GetState() & 0x80) {
			EnableUnderBar(false);
		}
	}
}

// avatar_main.cpp:771
void ntAvatarChatMain::ToggleTopViewMode(void) {
	m_topViewMode = !m_topViewMode;
	if (m_topViewMode) {
		GOLFDOC()->GameMode() = 0x200;

		Waabb box;
		m_pPVS->GetAABB(box);

		if (ntAvatarManager::Instance()->m_pMyAvatar) {
			m_topViewCenter = ntAvatarManager::Instance()->m_pMyAvatar->m_position;
		} else {
			m_topViewCenter.x = (box.min.x + box.max.x) * 0.5f;
			m_topViewCenter.y = box.min.y;
			m_topViewCenter.z = (box.min.z + box.max.z) * 0.5f;
		}
		m_topViewHeight = 256.0f;

		CSceneManager::Instance()->SetFog(false);
	} else {
		GOLFDOC()->GameMode() = 0;
		CSceneManager::Instance()->SetFog(true);
	}
}

// avatar_main.cpp:802
void ntAvatarChatMain::OnProcess(float elapsed) {
	static WVector s_keyMoveDir = WVector::UNIT_POS_Z;  // 0xAF3E5C, the keyboard walking direction
	static WVector s_cameraFront = WVector::UNIT_POS_X; // 0xAF3E50, the camera's flat front (line 858)
	static bool s_flagAF3E4C = false;   // 0xAF3E4C, cleared by the right-button branch (line 966)
	static bool s_moving = false;       // 0xAF3E4D, the own avatar walks to a target (shows the target decal)
	static bool s_keyMoving = false;    // 0xAF3E4E, the own avatar walks by the keyboard

	CheckUIPosition();

	CTaskMain::OnProcess(elapsed);

	GetPVS().PreProcess(elapsed, false);

	ntBalloonTextManager::Instance()->Process(elapsed);

	if (!ntAvatarManager::Instance()->m_pMyAvatar) {
		return;
	}

	if (g_pFresh->GetManager()->GetDesktop()->m_nFlags.GetFlag(0x8)) {
		int leftButton = g_input->GetButton(LEFT_BUTTON);
		int rightButton = g_input->GetButton(RIGHT_BUTTON);
		int middleButton = g_input->GetButton(MIDDLE_BUTTON);
		WVector mouseDelta;
		WVector target;
		int groundIndex;
		mouseDelta = g_input->GetMouseDelta();

		WVector mouse = g_input->GetMousePoint();
		WVector rayFrom, rayDir;
		g_view->GetScreen2World(mouse.x, mouse.y, rayFrom, rayDir);

		if (m_rightMenu) {
			// Any click outside the right menu closes it.
			if (!CMouseCursor::Instance()->InArea(m_rightMenu->GetRect())) {
				if (leftButton == 1 || rightButton == 1) {
					m_rightMenu->SetVisible(false);
				}
			}
		}

		float topViewSpeed = MaxOf(0.1f, m_topViewHeight * 0.0010416667f);

		if (!s_keyMoving) {
			s_cameraFront = g_view->camera.xa;
			s_cameraFront = s_cameraFront.Normalize();

			s_keyMoveDir = WCrossProduct(s_cameraFront, WVector::UNIT_POS_Y);
			s_keyMoveDir = s_keyMoveDir.Normalize();
		}

		if (CMouseCursor::Instance()->GetArea() == 0) {
			if (!m_topViewMode) {
				ntAvatarManager::Instance()->m_pLitAvatar = ntAvatarManager::Instance()->AvatarByRayIntersect(rayFrom, rayDir);
				ntAvatarManager::Instance()->m_pHighlightAvatar = ntAvatarManager::Instance()->CheckClick(mouse.x, mouse.y);

				if (ntAvatarManager::Instance()->m_pLitAvatar || ntAvatarManager::Instance()->m_pHighlightAvatar) {
					CMouseCursor::Instance()->SetCursor(1);
				} else {
					CMouseCursor::Instance()->SetCursor(0);
				}
			}

			bool inRightMenu = false;
			if (m_rightMenu && m_rightMenu->IsVisible()) {
				WRect menuRect(m_rightMenu->GetRect());

				if (menuRect.IsInRect(mouse.x, mouse.y)) {
					inRightMenu = true;
				}
			}

			if (!inRightMenu) {
				if (leftButton == 1) {
					ntAvatar *pClicked;
					// A click on an avatar picks it (and opens its trade
					// shop); a click on the ground walks there.
					if (!m_topViewMode && (pClicked = ntAvatarManager::Instance()->CheckClick(mouse.x, mouse.y)) != 0) {
						strcpy(m_targetNickName, pClicked->GetNickName());
						m_targetUid = pClicked->GetUID();

						if (pClicked->GetState() & 0x80) {
							if (pClicked != ntAvatarManager::Instance()->m_pMyAvatar) {
								OnEnterTradeShop();
							} else {
								OnOpenTradeShop();
							}
						}
					} else {
						WVector groundTarget = WVector(GetPVS().GetRayIntersection(rayFrom, rayDir));

						if (!ntAvatarManager::Instance()->m_pMyAvatar->IsHoldState()) {
							groundTarget = GetPVS().GetGroundPoint(groundTarget, true, &groundIndex, true);

							if (groundIndex >= 0) {
								s_moving = true;
								ntAvatarManager::Instance()->m_pMyAvatar->SetTargetPosition(groundTarget);
								ntAvatarManager::Instance()->m_pMyAvatar->SetAction(0);
								ntAvatarManager::Instance()->m_pMyAvatar->Follow(0);
							}
						}
					}
				}

				if (rightButton == 1) {
					if (!m_topViewMode) {
						if (ntAvatarManager::Instance()->m_pLitAvatar) {
							strcpy(m_targetNickName, ntAvatarManager::Instance()->m_pLitAvatar->GetNickName());
							m_targetUid = ntAvatarManager::Instance()->m_pLitAvatar->GetUID();
						} else {
							m_targetNickName[0] = 0;
							m_targetUid = 0xFFFFFFFF;
							m_menuNickName[0] = 0;
						}
					}
					s_flagAF3E4C = false;
				} else if (rightButton == 3) {
					// Dragging with the right button turns the camera.
					if (!m_topViewMode) {
						static WVector s_lastMouseDelta(0.0f, 0.0f, 0.0f); // 0xAF3E40

						float turnX = (mouseDelta.y + s_lastMouseDelta.y) * 0.5f;
						float turnY = (s_lastMouseDelta.y + mouseDelta.x) * 0.5f;
						m_cameraAngle.y -= turnY * 0.008f;
						m_cameraAngle.x -= turnX * 0.008f;
						s_lastMouseDelta = mouseDelta;

						if (m_cameraAngle.x < -1.5707964f) {
							m_cameraAngle.x = -1.5707964f;
						}
						if (m_cameraAngle.x > 0.78539819f) {
							m_cameraAngle.x = 0.78539819f;
						}

						m_headingCameraAngle = m_cameraAngle;
						m_idleTime = 0.0f;
					}

					if (!WisZero(mouseDelta.x, 1e-5f) || !WisZero(mouseDelta.y, 1e-5f)) {
						s_flagAF3E4C = true;
					}

					g_input->FixPointer(true);
					CMouseCursor::Instance()->Toggle(false);
				} else if (rightButton == 2) {
					// A right click without a drag, released on the avatar it
					// was pressed on, opens the menu.
					if (!m_topViewMode && !s_flagAF3E4C && ntAvatarManager::Instance()->m_pLitAvatar
						&& strcmp(ntAvatarManager::Instance()->m_pLitAvatar->GetNickName(), m_targetNickName) == 0) {
						PopupRightMenu((int)g_input->GetMousePoint().x, (int)g_input->GetMousePoint().y);
					}

					g_input->FixPointer(false);
					CMouseCursor::Instance()->Toggle(true);
				} else {
					g_input->FixPointer(false);
					CMouseCursor::Instance()->Toggle(true);
				}

				// Both buttons held walk forward.
				if (g_input->GetButton(LEFT_BUTTON) && g_input->GetButton(RIGHT_BUTTON)) {
					if (!m_topViewMode && !CChatMsg::Instance()->IsOpenning()) {
						target = ntAvatarManager::Instance()->m_pMyAvatar->m_position;
						target += s_keyMoveDir * 4.1f;

						if (!WisEqual(target, ntAvatarManager::Instance()->m_pMyAvatar->m_position, 1e-5f)
							&& !ntAvatarManager::Instance()->m_pMyAvatar->IsHoldState()) {
							s_moving = false;

							ntAvatarManager::Instance()->m_pMyAvatar->SetTargetPosition(target);
							ntAvatarManager::Instance()->m_pMyAvatar->SetAction(0);
							ntAvatarManager::Instance()->m_pMyAvatar->Follow(0);
						}
					}
				}

				if (middleButton == 1) {
					ToggleTopViewMode();
				}

				// The wheel zooms, unless a scroll bar has it.
				if (!g_pFresh->GetManager()->GetWheelFocus()) {
					if (!m_topViewMode) {
						m_cameraDistanceGoal = Between(10.88f, m_cameraDistanceGoal - mouseDelta.z * g_input->GetSensitivity() * 0.021f, 64.0f);
					} else {
						m_topViewHeight = Between(25.6f, m_topViewHeight - mouseDelta.z * g_input->GetSensitivity() * topViewSpeed * 0.4f, 960.0f);
					}
				}
			}
		}

		// The top view pans when the pointer touches a screen edge.
		if (m_topViewMode) {
			if (g_input->GetMousePoint().x < 5.0f) {
				m_topViewCenter.x -= topViewSpeed * elapsed * 800.0f;
			}
			if (g_view->GetWidth() - 6.0f < g_input->GetMousePoint().x) {
				m_topViewCenter.x += topViewSpeed * elapsed * 800.0f;
			}
			if (g_input->GetMousePoint().y < 5.0f) {
				m_topViewCenter.z += topViewSpeed * elapsed * 800.0f;
			}
			if (g_view->GetHeight() - 6.0f < g_input->GetMousePoint().y) {
				m_topViewCenter.z -= topViewSpeed * elapsed * 800.0f;
			}
		}

		if (!g_pFresh->GetManager()->IsKeyExclusive()) {
			if (g_input->GetDown("ENTER", true) || g_input->GetDown("PADENTER", true)) {
				if (CChatMsg::Instance()->GetOpenState() == 0) {
					CChatMsg::Instance()->Open(true, true);
				}
			}

			bool shift;
			if (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true)) {
				shift = true;
			} else {
				shift = false;
			}
			bool control;
			if (g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) {
				control = true;
			} else {
				control = false;
			}

			target = ntAvatarManager::Instance()->m_pMyAvatar->m_position;

			// The direction keys are named "up", "down", "left", "right" in CP949.
			if (!m_topViewMode && !CChatMsg::Instance()->IsOpenning()) {
				// They walk, or turn the camera with control held.
				if (!control) {
					if (g_input->Get("\xbb\xf3", true)) {
						target += s_keyMoveDir * 4.1f;
					}
					if (g_input->Get("\xc7\xcf", true)) {
						target += -s_keyMoveDir * 4.1f;
					}
					if (g_input->Get("\xc1\xc2", true)) {
						target += -s_cameraFront * 4.1f;
					}
					if (g_input->Get("\xbf\xec", true)) {
						target += s_cameraFront * 4.1f;
					}

					target = GetPVS().GetGroundPoint(target, true, &groundIndex, true);
					if (groundIndex < 0) {
						target = ntAvatarManager::Instance()->m_pMyAvatar->m_position;
					}
				} else {
					float turnY = 0.0f;
					float turnX = 0.0f;

					if (g_input->Get("\xbb\xf3", true)) {
						turnX = elapsed * -0.52359879f;
					}
					if (g_input->Get("\xc7\xcf", true)) {
						turnX += elapsed * 0.52359879f;
					}
					if (g_input->Get("\xc1\xc2", true)) {
						turnY = elapsed * -0.52359879f;
					}
					if (g_input->Get("\xbf\xec", true)) {
						turnY += elapsed * 0.52359879f;
					}

					if (!WisZero(turnY, 1e-5f) || !WisZero(turnX, 1e-5f)) {
						m_cameraAngle.y -= turnY;
						m_cameraAngle.x -= turnX;

						if (m_cameraAngle.x < -1.5707964f) {
							m_cameraAngle.x = -1.5707964f;
						}
						if (m_cameraAngle.x > 0.78539819f) {
							m_cameraAngle.x = 0.78539819f;
						}

						m_headingCameraAngle = m_cameraAngle;
						m_idleTime = 0.0f;
					}
				}

				if (g_input->Get("HOME", true)) {
					m_cameraDistanceGoal = Between(10.88f, m_cameraDistanceGoal - elapsed * 16.8f, 64.0f);
				}

				if (g_input->Get("END", true)) {
					m_cameraDistanceGoal = Between(10.88f, m_cameraDistanceGoal + elapsed * 16.8f, 64.0f);
				}

				if (g_input->GetDown("PAGE_DOWN", true)) {
					if (!ntAvatarManager::Instance()->m_pMyAvatar->IsHoldState()) {
						if (ntAvatarManager::Instance()->m_pMyAvatar->GetAction() != 2) {
							if (ntAvatarManager::Instance()->m_pMyAvatar->GetAction() == 1) {
								ntAvatarManager::Instance()->m_pMyAvatar->SetAction(2);
							} else {
								ntAvatarManager::Instance()->m_pMyAvatar->Stop();
								ntAvatarManager::Instance()->m_pMyAvatar->SetAction(1);
							}
						}
					}
				}

				if (g_input->GetDown("PAGE_UP", true)) {
					if (!ntAvatarManager::Instance()->m_pMyAvatar->IsHoldState()) {
						if (ntAvatarManager::Instance()->m_pMyAvatar->GetAction() == 1 || ntAvatarManager::Instance()->m_pMyAvatar->GetAction() == 2) {
							if (ntAvatarManager::Instance()->m_pMyAvatar->GetAction() == 1) {
								ntAvatarManager::Instance()->m_pMyAvatar->SetAction(0);
							} else {
								ntAvatarManager::Instance()->m_pMyAvatar->SetAction(1);
							}
						}
					}
				}
			} else {
				// The top view: the direction keys pan, shift with up/down zooms.
				if (g_input->Get("\xbb\xf3", true) && !shift) {
					m_topViewCenter.z += topViewSpeed * elapsed * 800.0f;
				}
				if (g_input->Get("\xc7\xcf", true) && !shift) {
					m_topViewCenter.z -= topViewSpeed * elapsed * 800.0f;
				}
				if (g_input->Get("\xc1\xc2", true)) {
					m_topViewCenter.x -= topViewSpeed * elapsed * 800.0f;
				}
				if (g_input->Get("\xbf\xec", true)) {
					m_topViewCenter.x += topViewSpeed * elapsed * 800.0f;
				}

				if (g_input->Get("HOME", true) || (shift && g_input->Get("\xbb\xf3", true))) {
					m_topViewHeight = Between(25.6f, m_topViewHeight - topViewSpeed * elapsed * 1200.0f, 960.0f);
				}
				if (g_input->Get("END", true) || (shift && g_input->Get("\xc7\xcf", true))) {
					m_topViewHeight = Between(25.6f, m_topViewHeight + topViewSpeed * elapsed * 1200.0f, 960.0f);
				}
			}

			if (g_input->GetDown("PAD_0", true) || g_input->GetDown("NUM_0", true)) {
				ToggleTopViewMode();
			}

			if (!WisEqual(target, ntAvatarManager::Instance()->m_pMyAvatar->m_position, 1e-5f)
				&& !ntAvatarManager::Instance()->m_pMyAvatar->IsHoldState()) {
				s_moving = false;
				s_keyMoving = true;

				ntAvatarManager::Instance()->m_pMyAvatar->SetTargetPosition(target);
				ntAvatarManager::Instance()->m_pMyAvatar->SetAction(0);
				ntAvatarManager::Instance()->m_pMyAvatar->Follow(0);
			} else {
				s_keyMoving = false;
			}
		}
	} else {
		if (m_rightMenu->IsVisible()) {
			m_rightMenu->SetVisible(false);
		}
		g_input->FixPointer(false);
		CMouseCursor::Instance()->Toggle(true);
	}

	if (ntAvatarManager::Instance()->m_pMyAvatar->IsTargetPositionChanged(false)) {
		WVector dir;
		if (ntAvatarManager::Instance()->m_pMyAvatar->IsFollowing() && ntAvatarManager::Instance()->m_pMyAvatar->GetFollowingAvatar()) {
			ntAvatar *pMyAvatar = ntAvatarManager::Instance()->m_pMyAvatar;
			dir = pMyAvatar->GetFollowingAvatar()->m_position - pMyAvatar->m_position;
			s_moving = true;
		} else if (s_keyMoving) {
			dir = s_keyMoveDir;
		} else {
			dir = ntAvatarManager::Instance()->m_pMyAvatar->GetTargetPosition() - ntAvatarManager::Instance()->m_pMyAvatar->m_position;
			m_idleTime = 0.0f;
		}

		dir.Normalize();

		m_idleMark = (float)atan2(dir.z, dir.x) + 4.712389f;
		m_headingCameraAngle = m_cameraAngle;
	}

	ntAvatarManager::Instance()->Process(elapsed);

	if (ntAvatarManager::Instance()->m_pMyAvatar->GetMotion() == 1) {
		m_idleTime += elapsed;
	}
	if (ntAvatarManager::Instance()->m_pMyAvatar->GetMotion() == 2 || ntAvatarManager::Instance()->m_pMyAvatar->GetMotion() == 3) {
		m_cameraHeightGoal = ntAvatarManager::Instance()->m_pMyAvatar->GetMySpItem((eSPAVILITYTYPE)0).getvalue() * 1.9f;
	} else {
		m_cameraHeightGoal = ntAvatarManager::Instance()->m_pMyAvatar->GetMySpItem((eSPAVILITYTYPE)0).getvalue() * 4.6f;
	}

	if (IsEqual(ntAvatarManager::Instance()->m_pMyAvatar->GetTargetPosition().x, ntAvatarManager::Instance()->m_pMyAvatar->m_position.x)
		&& IsEqual(ntAvatarManager::Instance()->m_pMyAvatar->GetTargetPosition().z, ntAvatarManager::Instance()->m_pMyAvatar->m_position.z)) {
		s_moving = false;
	}

	m_showTargetDecal = s_moving;

	ProcessCamera(elapsed);
	ProcessUI(elapsed);

	m_decalTime += elapsed;

	if (IsLocalContent((localContentType_t)0x57)) {
		RefreshAllUccClothes();
	}
}

// avatar_main.cpp:1379
void ntAvatarChatMain::ProcessCamera(float elapsed) {
	WMatrix camera;

	if (m_spItemChanged && g_SALoadingState) {
		m_cameraDistanceGoal = ntAvatarManager::Instance()->m_pMyAvatar->GetMySpItem((eSPAVILITYTYPE)0).getvalue() * 28.800001f;
		m_spItemChanged = false;
	}

	if (m_topViewMode) {
		g_view->SetClip(8.0f, 8192.0f, false);
		g_view->SetFOV(1.5707964f);
		g_view->UpdateCamera();

		camera = RotMat(WVector(-1.5707964f, 0.0f, 0.0f));
		camera.pivot = GetPVS().GetGroundPoint(m_topViewCenter, false, 0, false);
		camera.pivot.y += m_topViewHeight;
	} else {
		g_view->SetClip(1.0f, 4096.0f, false);
		g_view->SetFOV(1.0471976f);
		g_view->UpdateCamera();

		// Ease the distance and the height toward their goals.
		if (!(WAbsBySign(m_cameraDistance - m_cameraDistanceGoal) < 1e-5f)) {
			float step = elapsed * 46.0f;
			if (m_cameraDistanceGoal > m_cameraDistance) {
				m_cameraDistance += step;
				if (m_cameraDistance > m_cameraDistanceGoal) {
					m_cameraDistance = m_cameraDistanceGoal;
				}
			} else {
				m_cameraDistance -= step;
				if (m_cameraDistance < m_cameraDistanceGoal) {
					m_cameraDistance = m_cameraDistanceGoal;
				}
			}
		}

		camera = RotMat(WVector(m_cameraAngle.x, m_cameraAngle.y, m_cameraAngle.z));

		if (!(WAbsBySign(m_cameraHeight - m_cameraHeightGoal) < 1e-5f)) {
			float step = elapsed * 5.0f;
			if (m_cameraHeightGoal > m_cameraHeight) {
				m_cameraHeight += step;
				if (m_cameraHeight > m_cameraHeightGoal) {
					m_cameraHeight = m_cameraHeightGoal;
				}
			} else {
				m_cameraHeight -= step;
				if (m_cameraHeight < m_cameraHeightGoal) {
					m_cameraHeight = m_cameraHeightGoal;
				}
			}
		}

		const WVector &avatarPos = ntAvatarManager::Instance()->m_pMyAvatar->m_position;
		WVector pos = avatarPos;
		pos.y += m_cameraHeight;

		// Pull the camera in when the ground or an object is between it and
		// the avatar, and let it back out slowly.
		WVector dir;
		dir = camera.za * m_cameraDistance;
		dir.Negate();
		float hit = GetPVS().RayIntersectAll(pos, dir, false);
		if (hit > m_cameraRayHit && hit * m_cameraDistance > m_cameraClipDistance) {
			m_cameraClipDistance += elapsed * 16.0f;
			if (m_cameraClipDistance >= hit * m_cameraDistance) {
				m_cameraClipDistance = hit * m_cameraDistance;
				m_cameraRayHit = hit;
			}
		} else {
			m_cameraClipDistance = hit * m_cameraDistance;
			m_cameraRayHit = hit;
		}

		dir.Normalize();
		WVector eye;
		eye = pos + dir * m_cameraClipDistance;

		WVector ground = GetPVS().GetGroundPoint(eye, false, 0, false);
		float minY = ground.y + 3.5f;
		if (minY > eye.y) {
			eye.y = minY;
		}

		camera.pivot = eye;
	}

	g_camera = camera;
}

// avatar_main.cpp:1495
void ntAvatarChatMain::PrintText(float x, float y, const char *text, float spacing, int align, float scale, unsigned long color) {
	if (text == 0) {
		return;
	}

	int len = strlen(text);
	if (len == 0) {
		return;
	}

	char *charSet = m_pTitleFont->GetFontInfo()->pCharSet;

	float oldScale = m_pTitleFont->GetScale();
	m_pTitleFont->SetScale(scale);

	float width = 0.0f;
	const char *p = text;
	switch (align) {
	case 1: {
		while (*p) {
			int c = *p++;

			char *found = strchr(charSet, c);

			width += m_pTitleFont->GetCharWidthA(g_view, found - charSet) + spacing * scale;
		}
		width -= spacing;
		width *= 0.5f;
		break;
	}
	case 2: {
		while (*p) {
			int c = *p++;

			char *found = strchr(charSet, c);

			width += m_pTitleFont->GetCharWidthA(g_view, found - charSet) + spacing * scale;
		}
		width -= spacing;
		break;
	}
	}

	x -= width;

	p = text;
	while (*p) {
		int c = *p++;

		char *found = strchr(charSet, c);

		float charWidth = m_pTitleFont->PutChar(g_view, x, y, found - charSet, 0, color);
		charWidth += spacing;
		x += charWidth;
	}

	m_pTitleFont->SetScale(oldScale);
	m_pTitleFont->Flush(g_view);
}

// avatar_main.cpp:1572
void ntAvatarChatMain::Display(void) {
	if (ntAvatarManager::Instance()->m_pMyAvatar == 0) {
		return;
	}

	ntAvatarManager::Instance()->Render();

	if (m_showTargetDecal && m_uiCycle != 2) {
		// The decal under the followed avatar, or under the own avatar's target.
		WVector normal, tangent;
		normal = WVector(4.8f, 0.0f, 0.0f);
		tangent = WVector(0.0f, 0.0f, 4.8f);

		if (ntAvatarManager::Instance()->m_pMyAvatar->IsFollowing()) {
			ntAvatar *pTarget = ntAvatarManager::Instance()->FindAvatarByNickName(ntAvatarManager::Instance()->m_pMyAvatar->m_followName);
			if (pTarget == 0) {
				return;
			}
			GetPVS().DrawDecal(m_glowBallTexture, pTarget->m_position, normal, tangent, (cosf(m_decalTime * 10.0f) + 1.0f) * 0.05f + 1.0f, 1.0f, W_POLY_BLEND_ADD, false);
		} else {
			GetPVS().DrawDecal(m_glowBallTexture, ntAvatarManager::Instance()->m_pMyAvatar->GetTargetPosition(), normal, tangent, (cosf(m_decalTime * 10.0f) + 1.0f) * 0.05f + 1.0f, 1.0f, W_POLY_BLEND_ADD, false);
		}
	}
}

// avatar_main.cpp:1625
void ntAvatarChatMain::OnDisplay(void) {
	ntAvatarManager::Instance()->RenderOverlay();

	if (m_uiCycle != 2 && !CProjectG::Instance()->HidePI()) {
		ntBalloonTextManager::Instance()->Render();
	}

	if (m_uiCycle == 2 || CProjectG::Instance()->HideGUI() || CProjectG::Instance()->HidePI())
		return;

	FrGraphicInterface *pGDI = g_pFresh->GetManager()->GetGDI();

	const char *roomName = MakeStr("[%d] %s", CSharedDoc::Instance()->m_roomInfo.m_roomNumber, CSharedDoc::Instance()->m_roomInfo.m_title);

	pGDI->SetTextStyle(2);
	pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
	pGDI->PrintText(WPoint(20.0f, 48.0f), 0, roomName, 0);

	if (IsLocalContent((localContentType_t)10) && !CSharedDoc::Instance()->GetPangyaUserInfo().identityBits.administrator) {
		std::map<unsigned long, sBriefUserInfo>::iterator found;
		std::list<sSlotInfo>::iterator end = CSharedDoc::Instance()->m_roomPlayerList.end();
		for (std::list<sSlotInfo>::iterator it = CSharedDoc::Instance()->m_roomPlayerList.begin(); it != end; ++it) {
			sSlotInfo player = *it;

			if (player.m_identity & 4) {
				found = CSharedDoc::Instance()->m_userMap.find(player.m_uid);
			}
		}
	}

	PrintText(g_view->GetWidth() - 46.0f, 84.0f, MakeStr("%2d/%2d", CSharedDoc::Instance()->m_roomInfo.m_userCount, CSharedDoc::Instance()->m_roomInfo.m_userLimit), 1.0f, 2, 1.0f, 0xFFFFFFFF);
}

// avatar_main.cpp:1704
void ntAvatarChatMain::HandleMsg(const MsgObject &msg) {
	if ((unsigned long)msg.m_reserved > g_CurrentTime) {
		StoreDelayedMsg(msg);
		return;
	}

	switch (msg.m_msg) {
	case 0x4E:
		// avatar_main.cpp:1715: the jukebox button image (param1 == 1: playing).
		if (m_jukeBoxButton) {
			m_jukeBoxButton->SetButtonImg(msg.m_param1 == 1 ? "btn_jukebox_o" : "btn_jukebox_n", FrButton::NORMAL);
		}
		break;

	case 0xE4: {
		// A player entered the room (ntAvatarChatTask::OnPacket 0x46).
		const sSlotInfo *pSlotInfo = (const sSlotInfo *)msg.m_param1;
		const sCharacterInfo *pCharInfo = (const sCharacterInfo *)msg.m_param2;
		if (ntAvatarManager::Instance()->FindAvatarByNickName(pSlotInfo->m_nickname) == 0) {
			if (m_userListDlg) {
				m_userListDlg->UpdateUserList();
			}
			if (ntAvatarManager::Instance()->m_pMyAvatar) {
				if (IsLocalContent((localContentType_t)10)) {
					std::map<unsigned long, sBriefUserInfo>::const_iterator it = CSharedDoc::Instance()->m_userMap.find(pSlotInfo->m_uid);
					if (sBriefUserInfo(it->second).identityBits.administrator) {
						if (sBriefUserInfo(it->second).invisible) {
							std::string text(MakeStr("%s \xb4\xd4\xc0\xcc \xc0\xd4\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", pSlotInfo->m_nickname));
							CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false, false);
						}
					} else {
						std::string text(MakeStr("%s \xb4\xd4\xc0\xcc \xc0\xd4\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", pSlotInfo->m_nickname));
						CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false, false);
					}
				} else {
					std::string text(MakeStr("%s \xb4\xd4\xc0\xcc \xc0\xd4\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", pSlotInfo->m_nickname));
					CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false, false);
				}
			}

			ntAvatar *pAvatar = ntAvatarManager::Instance()->CreateAvatar(*pSlotInfo, *pCharInfo, this);
			if (pAvatar == 0) {
				break;
			}

			static_cast<ntAvatarChatTask *>(GetTask())->AddAvatarBurden(ntAvatarBurden(pSlotInfo->m_ruleUID, pSlotInfo->m_uid, *pCharInfo, 0));

			if (CSharedDoc::Instance()->GetPangyaUserInfo().guid == pSlotInfo->m_uid) {
				// The local player: place it at a random spot around the start point.
				ntAvatarManager::Instance()->m_pMyAvatar = pAvatar;
				CSharedDoc::Instance()->m_myCharacterId = pCharInfo->id;
				CSharedDoc::Instance()->SendTradeCloseShop();

				WPolySoup::sPoint *point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));
				if (point == 0) {
					point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xbd\xc3\xc0\xdb\xc1\xa1"));
				}

				WVector pos = point->m_pos;
				pos.x += Random(-16.0f, 16.0f, -1);
				pos.z += Random(-16.0f, 16.0f, -1);

				pos = m_pPVS->GetGroundPoint(pos, false, 0, false);

				bool showFx = true;
				if (IsLocalContent((localContentType_t)10)) {
					std::map<unsigned long, sBriefUserInfo>::const_iterator it = CSharedDoc::Instance()->m_userMap.find(pSlotInfo->m_uid);
					if (sBriefUserInfo(it->second).identityBits.administrator) {
						if (!sBriefUserInfo(it->second).invisible) {
							showFx = false;
						}
					}
				}
				if (showFx) {
					CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
					if (pSeq) {
						pSeq->Pos() = WVector(pos.x, pos.y + 5.0f, pos.z);
					}
				}

				ntAvatarManager::Instance()->m_pMyAvatar->SetPosition(pos);
				ntAvatarManager::Instance()->m_pMyAvatar->SetAngle(Random(0.0f, 6.2831855f, -1));
				ntAvatarManager::Instance()->m_pMyAvatar->Stop();

				WSendPacket packet(GCT_SYNC_ACTIVITY);
				packet.Encode1(4);
				packet.EncodeBuffer(&ntAvatarManager::Instance()->m_pMyAvatar->m_position.x, 4);
				packet.EncodeBuffer(&ntAvatarManager::Instance()->m_pMyAvatar->m_position.z, 4);
				packet.EncodeBuffer(&ntAvatarManager::Instance()->m_pMyAvatar->m_angle, 4);
				packet.Send(SEND_TO_DEFAULT);

				ChangePortraitImage(ntAvatarManager::Instance()->m_pMyAvatar->m_charTypeId & 0x3FFFFFF);
			} else {
				WVector pos = m_pPVS->GetGroundPoint(WVector(pSlotInfo->m_x, 0.0f, pSlotInfo->m_z), false, 0, false);
				pAvatar->SetPosition(pos);
				pAvatar->SetAngle(pSlotInfo->m_angle);
			}
		}

		WSendPacket packet(GCT_SYNC_AVATAR_SLOT);
		packet.Encode4(pSlotInfo->m_uid);
		packet.Send(SEND_TO_DEFAULT);
		break;
	}

	case 0xE5: {
		// A player left the room.
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(msg.m_param1);
		if (pAvatar == 0) {
			break;
		}

		if (m_userListDlg) {
			m_userListDlg->UpdateUserList();
		}

		if (ntAvatarManager::Instance()->m_pMyAvatar) {
			if (strcmp(pAvatar->GetIDName(), ntAvatarManager::Instance()->m_pMyAvatar->GetIDName()) == 0) {
				CSharedDoc::Instance()->SendTradeCloseShop();
				ntAvatarManager::Instance()->m_pMyAvatar->SubState(0x7E);
				ntAvatarManager::Instance()->m_pMyAvatar = 0;
			}

			if (IsLocalContent((localContentType_t)10)) {
				std::map<unsigned long, sBriefUserInfo>::const_iterator it = CSharedDoc::Instance()->m_userMap.find(pAvatar->m_guid);
				if (sBriefUserInfo(it->second).identityBits.administrator) {
					if (sBriefUserInfo(it->second).invisible) {
						std::string text(MakeStr("%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", pAvatar->GetNickName()));
						CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false, false);
					}
				} else {
					std::string text(MakeStr("%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", pAvatar->GetNickName()));
					CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false, false);
				}
			} else {
				std::string text(MakeStr("%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.", pAvatar->GetNickName()));
				CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false, false);
			}
		}

		bool showFx = true;
		if (IsLocalContent((localContentType_t)10)) {
			std::map<unsigned long, sBriefUserInfo>::const_iterator it = CSharedDoc::Instance()->m_userMap.find(pAvatar->m_guid);
			if (sBriefUserInfo(it->second).identityBits.administrator && !sBriefUserInfo(it->second).invisible) {
				showFx = false;
			}
		}
		if (showFx) {
			CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
			if (pSeq) {
				pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
			}
		}

		static_cast<ntAvatarChatTask *>(GetTask())->AddAvatarBurden(ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, pAvatar->m_characterInfo, (CPetFrame *)-1));
		static_cast<ntAvatarChatTask *>(GetTask())->RemoveAvatarBurden(ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, pAvatar->m_characterInfo, pAvatar->m_pPetFrame));
		ntAvatarManager::Instance()->DeleteAvatar(pAvatar->GetIDName());
		break;
	}

	case 0xE7:
		// avatar_main.cpp:1926: the local avatar's new nickname (param1).
		if (ntAvatarManager::Instance()->m_pMyAvatar) {
			strcpy(ntAvatarManager::Instance()->m_pMyAvatar->m_nickName, (const char *)msg.m_param1);
		}
		break;

	case 0xE6: {
		// avatar_main.cpp:1935: the original looks the avatar up and does nothing else.
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(msg.m_param1);
		break;
	}

	case 0xE8: {
		// avatar_main.cpp:1945: an avatar's state bits (param1: nickname, param2: kind).
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByNickName((const char *)msg.m_param1);
		if (pAvatar == 0) {
			break;
		}
		switch (msg.m_param2) {
		case 0:
		case 2:
			pAvatar->SubState(0x180);
			break;
		case 3:
			pAvatar->SubState(0x80);
			pAvatar->AddState(0x100);
			break;
		case 1:
			pAvatar->AddState(0x80);
			break;
		}
		break;
	}

	case 0xE9: {
		// avatar_main.cpp:1967: an avatar's trade shop title (param1: id name, param2: title).
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByIDName((const char *)msg.m_param1);
		if (pAvatar) {
			pAvatar->SetTradeTitle((const char *)msg.m_param2);
		}
		break;
	}

	case 0x49: {
		// avatar_main.cpp:1976: an avatar's character info changed (param1: guid, param2: the info).
		int guid = msg.m_param1;
		const sCharacterInfo *pCharInfo = (const sCharacterInfo *)msg.m_param2;
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar == 0) {
			break;
		}

		static_cast<ntAvatarChatTask *>(GetTask())->RemoveAvatarBurden(ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, pAvatar->m_characterInfo, pAvatar->m_pPetFrame));

		pAvatar->SetCharactorInfo(*pCharInfo);

		static_cast<ntAvatarChatTask *>(GetTask())->AddAvatarBurden(ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, *pCharInfo, 0));

		if (IsLocalContent((localContentType_t)10)) {
			std::map<unsigned long, sBriefUserInfo>::const_iterator it = CSharedDoc::Instance()->m_userMap.find(pAvatar->m_guid);
			if (sBriefUserInfo(it->second).identityBits.administrator) {
				if (sBriefUserInfo(it->second).invisible) {
					CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
					if (pSeq) {
						pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
					}
				}
			} else {
				CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
				if (pSeq) {
					pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
				}
			}
		} else {
			CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
			if (pSeq) {
				pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
			}
		}

		if (pAvatar == ntAvatarManager::Instance()->m_pMyAvatar) {
			CSharedDoc::Instance()->m_myCharacterId = pCharInfo->id;
			ChangePortraitImage(ntAvatarManager::Instance()->m_pMyAvatar->m_charTypeId & 0x3FFFFFF);
		}
		break;
	}

	case 0x4A: {
		// avatar_main.cpp:2027: the local avatar equipped a caddie (param2: its id).
		int guid = msg.m_param1;
		const unsigned long *pCaddieId = (const unsigned long *)msg.m_param2;
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar && pAvatar == ntAvatarManager::Instance()->m_pMyAvatar) {
			CSharedDoc::Instance()->m_myCaddieId = *pCaddieId;
		}
		break;
	}

	case 0x4B: {
		// avatar_main.cpp:2036: the local avatar equipped a club set (param2: its id).
		int guid = msg.m_param1;
		const unsigned long *pClubId = (const unsigned long *)msg.m_param2;
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar && pAvatar == ntAvatarManager::Instance()->m_pMyAvatar) {
			CSharedDoc::Instance()->m_myClubId = *pClubId;
		}
		break;
	}

	case 0x4C: {
		// avatar_main.cpp:2045: the local avatar equipped a ball (param2: its id).
		int guid = msg.m_param1;
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar && pAvatar == ntAvatarManager::Instance()->m_pMyAvatar) {
			CSharedDoc::Instance()->m_myBallId = msg.m_param2;
		}
		break;
	}

	case 0x69: {
		// avatar_main.cpp:2053: the local avatar equipped a mascot (param2: its id).
		int guid = msg.m_param1;
		const unsigned long *pMascotId = (const unsigned long *)msg.m_param2;
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar && pAvatar == ntAvatarManager::Instance()->m_pMyAvatar) {
			CSharedDoc::Instance()->m_myMascotId = *pMascotId;
		}
		break;
	}

	case 0x24E: {
		// avatar_main.cpp:2063: a special prize item effect from the server (0x49 kind 6).
		eSPAVILITYTYPE type = (eSPAVILITYTYPE)msg.m_param2;
		int guid = msg.m_param1;
		float value = (float)msg.m_param3;
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar == 0) {
			break;
		}

		if (type == 0) {
			if (ntAvatarManager::Instance()->m_pMyAvatar->m_guid == guid) {
				m_spItemChanged = true;
				g_SALoadingState = false;
			}
		}
		if (type != 3) {
			sCharacterInfo charInfo = pAvatar->m_characterInfo;
			static_cast<ntAvatarChatTask *>(GetTask())->RemoveAvatarBurden(ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, pAvatar->m_characterInfo, pAvatar->m_pPetFrame));
			pAvatar->SetCharactorInfo(charInfo);
			static_cast<ntAvatarChatTask *>(GetTask())->AddAvatarBurden(ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, pAvatar->m_characterInfo, 0));
		}

		pAvatar->GetMySpItem(type).changeflag();
		pAvatar->GetMySpItem(type).setvalue(value);

		if (IsLocalContent((localContentType_t)10)) {
			std::map<unsigned long, sBriefUserInfo>::const_iterator it = CSharedDoc::Instance()->m_userMap.find(pAvatar->m_guid);
			if (sBriefUserInfo(it->second).identityBits.administrator) {
				if (sBriefUserInfo(it->second).invisible) {
					StartSpEffect(pAvatar, type);
				}
			} else {
				StartSpEffect(pAvatar, type);
			}
		} else {
			StartSpEffect(pAvatar, type);
		}
		break;
	}

	case 0x0F: {
		// Find the room master.
		std::list<sSlotInfo> &players = CSharedDoc::Instance()->m_roomPlayerList;
		for (std::list<sSlotInfo>::iterator it = players.begin(); it != players.end(); ++it) {
			if (it->m_attribute & 8) {
				m_roomMasterUid = it->m_uid;
				break;
			}
		}
		break;
	}

	case 0x5F: {
		// avatar_main.cpp:2136: a new room master (param1: its uid).
		if (msg.m_param2 != 0xFFFF) {
			break;
		}
		std::list<sSlotInfo> &players = CSharedDoc::Instance()->m_roomPlayerList;
		for (std::list<sSlotInfo>::iterator it = players.begin(); it != players.end(); ++it) {
			if (it->m_uid == (unsigned long)msg.m_param1) {
				it->m_attribute |= 8;
				it->m_flags55.flag1 = 1;
			} else {
				it->m_attribute &= ~8;
			}
		}
		m_roomMasterUid = msg.m_param1;
		break;
	}

	case 0x5B: {
		// avatar_main.cpp:2160: another avatar's activity (param1: guid, param2: kind).
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(msg.m_param1);
		if (pAvatar == 0 || pAvatar == ntAvatarManager::Instance()->m_pMyAvatar) {
			break;
		}

		switch (msg.m_param2) {
		case 4: {
			// Warp: param3/param4 the x/z position, param5 the angle.
			WVector pos(*(const float *)msg.m_param3, 0.0f, *(const float *)msg.m_param4);
			pos = GetPVS().GetGroundPoint(pos, false, 0, false);
			pAvatar->SetPosition(pos);
			pAvatar->SetAngle(*(const float *)msg.m_param5);

			if (IsLocalContent((localContentType_t)10)) {
				std::map<unsigned long, sBriefUserInfo>::const_iterator it = CSharedDoc::Instance()->m_userMap.find(msg.m_param1);
				if (sBriefUserInfo(it->second).identityBits.administrator) {
					if (sBriefUserInfo(it->second).invisible) {
						CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
						if (pSeq) {
							pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
						}
					}
				}
			} else {
				CFxSequence *pSeq = CFx::Instance()->OpenSequence("Talk_Room.seq", true);
				if (pSeq) {
					pSeq->Pos() = WVector(pos.x, pos.y + 5.0f, pos.z);
				}
			}
			break;
		}
		case 5:
			pAvatar->SetAction(msg.m_param3);
			break;
		case 6: {
			// Move: param3/param4 the x/z offset, param5 the target angle.
			WVector target = pAvatar->GetTargetPosition();
			target.x += *(const float *)msg.m_param3;
			target.z += *(const float *)msg.m_param4;
			pAvatar->SetTargetPosition(target);
			pAvatar->SetPendingTargetAngle(*(const float *)msg.m_param5);
			break;
		}
		case 7:
			pAvatar->ChangeMotion(4, (const char *)msg.m_param3, 0.2f);
			break;
		case 8:
			pAvatar->SetState(msg.m_param3);
			break;
		case 9: {
			std::map<unsigned long, sBriefUserInfo>::iterator it = CSharedDoc::Instance()->m_userMap.find(msg.m_param1);
			if (it != CSharedDoc::Instance()->m_userMap.end()) {
				if (m_userListDlg) {
					m_userListDlg->UpdateUserList();
				}
			}
			break;
		}
		}
		break;
	}

	case 0x3D: {
		// avatar_main.cpp:2252: set or clear a room player's dive bit
		// (param2 == 1 sets it).
		std::list<sSlotInfo> &players = CSharedDoc::Instance()->m_roomPlayerList;
		std::list<sSlotInfo>::iterator it = std::find(players.begin(), players.end(), msg.m_param1);
		if (it == players.end()) {
			break;
		}
		it->m_attributeBits.dive = (msg.m_param2 == 1);
		break;
	}

	case 0x62:
		// avatar_main.cpp:2261: show or hide the chat controls (param1).
		if (m_chatFrame) {
			m_chatFrame->SetVisible(msg.m_param1 != 0);
		}
		if (m_chatTarget) {
			m_chatTarget->SetVisible(msg.m_param1 != 0);
		}
		if (m_chatInput) {
			if (msg.m_param1) {
				m_chatInput->SetVisible(true);
				m_chatInput->SetKeyFocus(true);
			} else {
				m_chatInput->SetVisible(false);
			}
			g_pFresh->GetManager()->SetExclusiveKey(msg.m_param1 != 0);
		}
		if (m_emoticonButton) {
			m_emoticonButton->SetVisible(msg.m_param1 != 0);
		}
		if (m_language) {
			m_language->SetVisible(msg.m_param1 != 0);
		}
		if (m_reportButton) {
			m_reportButton->SetVisible(msg.m_param1 != 0);
		}
		break;

	case 0x3F:
		// avatar_main.cpp:2294: return the chat input's text.
		*(const char **)msg.m_param1 = m_chatInput ? m_chatInput->GetLine(1, false) : 0;
		break;

	case 0x37:
		// avatar_main.cpp:2299: whisper to a player (param1: the nickname).
		if (CUserInfo::Instance()->IsVisible()) {
			CUserInfo::Instance()->Close();
		}
		if (CSharedDoc::Instance()->AddWhisperPartner(m_chatTarget, (const char *)msg.m_param1)) {
			m_chatTarget->SetLine(1, (const char *)msg.m_param1, 0, false, 0);
		}
		if (CChatMsg::Instance()->IsClosed()) {
			CChatMsg::Instance()->Open(true, true);
		}
		if (m_chatInput) {
			m_chatInput->SetKeyFocus(true);
		}
		break;

	case 0x65:
		// avatar_main.cpp:2315: select the last whisper partner.
		if (m_chatTarget) {
			if (CSharedDoc::Instance()->m_recentWhisperList.size() > 0) {
				m_chatTarget->SetLine(1, CSharedDoc::Instance()->m_recentWhisperList.back().c_str(), 0, false, 0);
			}
		}
		break;

	case 0x38:
		// avatar_main.cpp:2330
		CSharedDoc::Instance()->AddWhisperPartner(m_chatTarget, (const char *)msg.m_param1);
		break;

	case 0x47:
		// avatar_main.cpp:2335
		CSharedDoc::Instance()->InitRecentWhisperList(m_chatTarget);
		break;

	case 0x64:
		// avatar_main.cpp:2340: a chat line (param1: the text, param2: the color).
		if (m_chatView) {
			if (m_chatView->GetLineNum() > 50) {
				m_chatView->DeleteFirstLine();
				m_chatView->GetScrollBar()->DelItem();
			}
			m_chatView->AddLine((const char *)msg.m_param1, msg.m_param2, false);
			if (m_chatViewLines > 0) {
				m_chatView->DeleteFirstLine();
				m_chatView->GetScrollBar()->DelItem();
				m_chatViewLines--;
			}
		}
		if (m_chatLogDlg) {
			static_cast<FrAvatarChatLogDlg *>(m_chatLogDlg)->AddChatMsg((const char *)msg.m_param1, msg.m_param2);
		}
		break;

	case 0x63:
		// Clear the chat view to blank lines (posted by OnChatViewInit).
		if (m_chatView) {
			m_chatView->ClearLine();
			for (int i = 0; i < 8; i++) {
				m_chatView->AddLine("", 0, false);
			}
		}
		break;

	case 0x60: {
		// A chat line: the speaker's facial expression and chat motion.
		const char *nickName = (const char *)msg.m_param1;
		const char *text = (const char *)msg.m_param2;
		if (nickName == 0 && ntAvatarManager::Instance()->m_pMyAvatar) {
			nickName = ntAvatarManager::Instance()->m_pMyAvatar->GetNickName();
		}
		ntAvatar *pAvatar = ntAvatarManager::Instance()->FindAvatarByNickName(nickName);
		if (pAvatar == 0) {
			break;
		}
		if (SpCommand(text)) {
			break;
		}
		int face = CSharedDoc::Instance()->m_chatManager.GetFaceIndex(text);
		if (face != 5) {
			pAvatar->SetChatFacial(face);
		}
		const char *motionName = CSharedDoc::Instance()->m_chatManager.GetMotionName(text, pAvatar->m_charTypeId);
		if (motionName && !pAvatar->IsHoldState()) {
			pAvatar->ChangeMotion(4, motionName, 0.2f);
		}
		break;
	}

	case 0x00:
		// Open the chat room's layout (LobbyTask posts "AVATARCHAT_MAIN").
		if (g_pFresh) {
			g_pFresh->OpenLayout((const char *)msg.m_param1, this, false);
		}
		break;

	case 0x16:
		m_storedFlag = msg.m_param1 != 0;
		break;

	case 0x17:
		*(int *)msg.m_param1 = m_storedFlag;
		break;

	case 0x27:
		strcpy(m_storedText, (const char *)msg.m_param1);
		break;

	case 0x28:
		*(char **)msg.m_param1 = m_storedText;
		break;

	case 0x3B:
		// avatar_main.cpp:2466: a room entry error (param1's low byte: the reason).
		switch ((unsigned char)msg.m_param1) {
		case 2:
			this << MsgObject(0, 0x23, (intptr_t)"\xc1\xa4\xbf\xf8\xc0\xcc \xc3\xca\xb0\xfa \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0);
			break;
		case 3:
			this << MsgObject(0, 0x23, (intptr_t)"\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9", 0, 0, 0, 0);
			break;
		case 4:
			this << MsgObject(0, 0x23, (intptr_t)"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xc6\xb2\xb7\xc8\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0);
			break;
		case 7:
		case 9:
			this << MsgObject(0, 0x23, (intptr_t)"\xb9\xe6\xb8\xb8\xb5\xe9\xb1\xe2 \xbf\xe4\xc3\xbb\xc0\xcc \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9", 0, 0, 0, 0);
			break;
		case 8:
			this << MsgObject(0, 0x23, (intptr_t)"\xb0\xd4\xc0\xd3 \xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9", 0, 0, 0, 0);
			break;
		case 5:
			this << MsgObject(0, 0x23, (intptr_t)"\xc7\xf6\xc0\xe7 \xb7\xb9\xba\xa7\xbf\xa1\xbc\xad\xb4\xc2 \xb9\xe6\xc0\xbb \xb8\xb8\xb5\xe9\xb0\xc5\xb3\xaa \xb9\xe6\xbf\xa1 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
			break;
		case 0xD:
			this << MsgObject(0, 0x23, (intptr_t)"\xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xc7\xd8\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
			break;
		case 0xF:
			this << MsgObject(0, 0x23, (intptr_t)"\xc6\xce\xb9\xe8\xc6\xb2\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9", 0, 0, 0, 0);
			break;
		case 0x11:
			this << MsgObject(0, 0x23, (intptr_t)"\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1\xb0\xd4\xc0\xd3\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.", 0, 0, 0, 0);
			break;
		}
		break;

	case 0x1F9: {
		// avatar_main.cpp:2511: apply the GUI / PI hide state.
		if (CQuitWindow::Instance()->IsActive()) {
			break;
		}

		bool hideGUI = m_uiCycle == 2 || CProjectG::Instance()->HideGUI();
		if (g_pFresh) {
			g_pFresh->Toggle(!hideGUI);
		}

		bool hideBar = m_uiCycle == 1 || CProjectG::Instance()->HideGUI();
		ShowOverAndUnderBar(!hideBar);

		bool hideChat = (m_uiCycle == 2 || CProjectG::Instance()->HidePI()) && !CChatMsg::Instance()->IsClosed();
		CChatMsg::Instance()->Toggle(!hideChat);

		ntAvatarManager::Instance()->m_overlayDetail = m_uiCycle == 0 && !CProjectG::Instance()->HidePI();

		bool hideCursor = m_uiCycle == 2 || CProjectG::Instance()->HideGUI() || CProjectG::Instance()->HidePI();
		CMouseCursor::Instance()->Toggle(!hideCursor);

		bool hidePI = m_uiCycle == 2 || CProjectG::Instance()->HidePI();
		if (m_userListButton) {
			m_userListButton->SetVisible(!hidePI);
		}
		if (m_pangCookieWnd) {
			m_pangCookieWnd->SetVisible(!hidePI);
		}
		break;
	}

	case 0x03:
	case 0x31:
	case 0x34:
	case 0x3C:
	case 0x211:
		break;

	default:
		HandleMsgCommon(msg);
		break;
	}
}

// avatar_main.cpp:2583
void ntAvatarChatMain::RefreshAllUccClothes(void) {
	sUccRefreshItem item;

	for (int i = 0; i < 10; i++) {
		if (!UccManager()->GetRefreshItem(item)) {
			break;
		}

		int count = 0;
		for (int j = 0; j < 30; j++) {
			if (count >= ntAvatarManager::Instance()->m_avatarCount) {
				break;
			}
			ntAvatar *pAvatar = ntAvatarManager::Instance()->m_avatars[j];

			if (pAvatar == 0) {
				continue;
			}

			count++;
			unsigned char slot = (unsigned char)((item.m_typeId >> 13) & 0x1F);
			if (pAvatar->GetPartTidList().m_partTids[slot] != item.m_typeId) {
				continue;
			}
			unsigned long serial = pAvatar->GetPartTidList().m_partIds[slot];

			sUccClothes *pClothes = UccManager()->FindClothes(serial);

			if (pClothes != 0 && strcmpi(pClothes->m_uccKey, item.m_uccKey) == 0) {
				pAvatar->RefreshUccClothes(pClothes->m_typeId, pClothes->m_serial);
			}
		}
	}
}

// avatar_main.cpp:2625
CQuadTree &ntAvatarChatMain::GetPVS(void) {
	return *m_pPVS;
}

// avatar_main.cpp:2631
void ntAvatarChatMain::SetLocalPVS(CQuadTree &pvs) {
	m_pPVS = &pvs;
}

// avatar_main.cpp:2637
void ntAvatarChatMain::StartSpEffect(ntAvatar *pAvatar, eSPAVILITYTYPE type) {
	CFxSequence *pSeq = NULL;
	bool bOn = pAvatar->GetMySpItem(type).getflag();

	switch (type) {
	case 0:
		if (!bOn) {
			if (pAvatar->GetMySpItem((eSPAVILITYTYPE)1).getflag()) {
				pSeq = CFx::Instance()->OpenSequence("change_up-middle.seq", true);
			} else {
				pSeq = CFx::Instance()->OpenSequence("change_up-small.seq", true);
			}
		} else {
			pSeq = CFx::Instance()->OpenSequence("change_up-big.seq", true);
		}
		break;

	case 1:
		if (bOn) {
			if (pAvatar->GetMySpItem((eSPAVILITYTYPE)0).getflag()) {
				pSeq = CFx::Instance()->OpenSequence("change_up-big.seq", true);
			} else {
				pSeq = CFx::Instance()->OpenSequence("change_up-middle.seq", true);
			}
		} else {
			if (!pAvatar->GetMySpItem((eSPAVILITYTYPE)0).getflag()) {
				pSeq = CFx::Instance()->OpenSequence("change_up-small.seq", true);
			} else {
				pSeq = CFx::Instance()->OpenSequence("change_up-big.seq", true);
			}
		}
		break;

	case 2:
		if (!pAvatar->GetMySpItem((eSPAVILITYTYPE)0).getflag()) {
			if (pAvatar->GetMySpItem((eSPAVILITYTYPE)1).getflag()) {
				pSeq = CFx::Instance()->OpenSequence("change_up-middle.seq", true);
			} else {
				pSeq = CFx::Instance()->OpenSequence("change_up-small.seq", true);
			}
		} else {
			pSeq = CFx::Instance()->OpenSequence("change_up-big.seq", true);
		}
		break;

	case 3: {
		float height = pAvatar->GetMySpItem((eSPAVILITYTYPE)0).getvalue() * 10.0f;
		pSeq = CFx::Instance()->OpenSequence("BFX_B2.seq", true);
		if (pSeq) {
			pSeq->Pos() = WVector(pAvatar->m_position.x + 1.0f, pAvatar->m_position.y + height, pAvatar->m_position.z + 1.0f);
		}
		pSeq = CFx::Instance()->OpenSequence("BFX_B3.seq", true);
		if (pSeq) {
			pSeq->Pos() = WVector(pAvatar->m_position.x + 1.0f, pAvatar->m_position.y + height, pAvatar->m_position.z + 1.0f);
		}
		g_audio->PlaySfx("Ball_B", 0, 0, 0, 0, 0.5f, 200.0f);
		return;
	}

	default:
		break;
	}

	if (pSeq) {
		pSeq->Pos() = WVector(pAvatar->m_position.x, pAvatar->m_position.y + 5.0f, pAvatar->m_position.z);
	}
}

// avatar_main.cpp:2771
bool ntAvatarChatMain::SpCommand(const char *command) {
	bool handled = false;
	static unsigned long s_lastTime = timeGetTime() - 1000;
	int len = strlen(command + 1);

	if (len == 0 || command[0] != '/') {
		return false;
	}

	// The chat commands (giant, big head, light speed, sparkle), indexed by
	// eSPAVILITYTYPE.
	static const char s_commands[5][64] = {
		"\xb0\xc5\xc0\xce",
		"\xbf\xd5\xb8\xd3\xb8\xae",
		"\xb1\xa4\xbc\xd3",
		"\xb9\xdd\xc2\xa6\xc0\xcc",
		""
	};

	for (int i = 0; i < 5; i++) {
		if (strcmp(command + 1, s_commands[i]) == 0) {
			handled = true;

			if (i == 3) {
				if (timeGetTime() - s_lastTime >= 1000) {
					s_lastTime = timeGetTime();
				} else {
					CChatMsg::Instance()->AddChatMsg("\xc1\xf6\xb1\xdd\xc0\xba \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9", 0xFFFF7878, true, false);
					return true;
				}
			}

			if (CanUseSPItem((eSPAVILITYTYPE)i)) {
				ntAvatarManager::Instance()->m_pMyAvatar->UseSPItem((eSPAVILITYTYPE)i);
			} else {
				CChatMsg::Instance()->AddChatMsg("\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc2\xf8\xbf\xeb\xc7\xcf\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9", 0xFFFF7878, true, false);
			}
			break;
		}
	}

	return handled;
}

// avatar_main.cpp:2818
bool ntAvatarChatMain::CanUseSPItem(eSPAVILITYTYPE type) {
	const sCharacterInfo &info = ntAvatarManager::Instance()->m_pMyAvatar->GetCharacterInfo();
	for (int i = 0; i < 24; i++) {
		if (info.partTypeIds[i] != 0) {
			IFF_STRUCT::sSpecialPrizeItem *pItem = ItemManager()->FindSPItem(info.partTypeIds[i], type);
			if (pItem != 0) {
				return true;
			}
		}
	}

	for (int j = 0; j < 5; j++) {
		if (info.auxParts.tids[j] != 0) {
			IFF_STRUCT::sSpecialPrizeItem *pItem = ItemManager()->FindSPItem(info.auxParts.tids[j], type);
			if (pItem != 0) {
				return true;
			}
		}
	}

	return false;
}
