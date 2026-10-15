// Original: d:\build\custom\temp\code\client\projectg\realmyroommain.cpp
#define REALMYROOMMAIN_ANOTHERHAND_INLINE
#define REALMYROOMMAIN_TOOLTIP_INLINE
#define REALMYROOMMAIN_REALMYROOM_AVATAR_INLINE
#include "NetworkSystem.h"
#include "realmyroommain.h"
#include "ToolTip.h"
#include "AnotherHand.h"
#include "ObjectFactory.h"
#include "actor.h"
#include "TaskManager.h"
#include "SharedDoc.h"
#include "GolfDoc.h"
#include "ClientSetting.h"
#include "Shadow.h"
#include "fresh.h"
#include "client/Wangreal/include/wresrcmng.h"
#include "avatar_task.h"
#include "frwnd.h"
#include "wview.h"
#include "RealMyRoom_Avatar.h"
#include "RealMyRoomData.h"
#include "mousecursor.h"
#include "CCardBook.h"
#include "fx.h"
#include "shared/Localize.h"
#include "InputManager.h"
#include "woverlay.h"
#include "frwndmanager.h"
#include "TransferableItem.h"
#include "quadtree.h"
#include "UccManager.h"
#include "exhibition.h"
#include "partsinfo.h"
#include "sky.h"
#include "commonutil.h"
#include "camerawork.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "RealMyRoomTask.h"
#include "wind.h"
#include "s5/Utilities.h"
#include "frdesktop.h"
#include "messengerdlg.h"
#include "fredit.h"
#include "StandingInLine.h"	// s_standingNames initializer (sub_00997d40)

extern WMatrix g_camera;
extern bool g_bQuit;          // ProjectG.cpp
extern LightSet g_lightset;
float Random(float min, float max, int seed); // commonutil.cpp:1106
int WisZero(const float &value, float eps);

// CAnotherHand::GetThreadID (anotherhand.h:56), IsWorking, Enter and Leave
// (anotherhand.inl) are emitted here out of line by AnotherHand.h through
// REALMYROOMMAIN_ANOTHERHAND_INLINE.

// CIconToolTip::GetCurTipIndex (tooltip.h:98) is emitted here out of line by
// ToolTip.h through REALMYROOMMAIN_TOOLTIP_INLINE.

// realmyroommain.h:45
const WRTTI* CRealMyRoomMain::GetRTTI(void) const {
	return &m_RTTI;
}

// realmyroommain.h: no line record (empty constructor of a nested helper struct)
CRealMyRoomMain::miniBtn_t::miniBtn_t(void) {
}

// realmyroommain.cpp:129 (run-time type and factory registration of CRealMyRoomMain)
const WRTTI CRealMyRoomMain::m_RTTI("CRealMyRoomMain", &CTaskMain::m_RTTI);

IObject* CRealMyRoomMainMakeInstance(void) {
	return new CRealMyRoomMain;
}

struct __impCRealMyRoomMain {
	__impCRealMyRoomMain(void) {
		ObjectFactory().AddObjectFunctor(CRealMyRoomMainMakeInstance, "CRealMyRoomMain");
		ActorFactory().AddActorClass("CRealMyRoomMain");
	}
};
static __impCRealMyRoomMain s_impCRealMyRoomMain;

// realmyroommain.cpp:133
CRealMyRoomMain::CRealMyRoomMain(void) {
	m_priority = 4;

	m_decalPulse = 0.0f;
	m_avatarViewPos = WVector::ZERO;
	m_lookAtOffset = WVector(0.0f, 0.0f, 0.0f);
	m_fovDeg = 28.800001f;
	m_fovGoalDeg = 28.800001f;
	m_fovBaseDeg = 28.800001f;

	m_cameraIdleTime = 0.0f;
	m_cameraFlag = 0;
	m_cameraBlend = 1.0f; m_cameraHeight = 4.6f; m_cameraHeightGoal = 4.6f;

	m_cameraDropOrigin = WVector::ZERO;
	m_topViewMode = false;
	m_cameraFov = 256.0f;

	m_lookAtYMinDeg = -40.0f;
	m_lookAtYMaxDeg = 40.0f;
	m_lookAtXMinDeg = -25.0f;
	m_lookAtXMaxDeg = 25.0f;
	m_customLookAtLimits = 0;

	m_newYearMoneySerial = 0;

	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_decalShown = false;
	m_decalTexture = g_resrcmng->LoadTexture("~GlowBall.jpg", 0, 0, 0);

	tagWTITLEFONT font;
	char* fileNames[1] = { "[font_wind.jpg" };
	memset(&font, 0, sizeof(font));

	font.filename = fileNames; font.fonth = 16;
	font.fontw = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ym%-.?/:, ";
	m_pTitleFont->Create(&font);

	m_pLayoutWnd = NULL;
	m_pNamedCameras[1] = NULL;
	m_pCameras[0] = NULL;
	m_pCameras[1] = NULL;
	m_pNamedCameras[0] = NULL;
	m_pRoomPoint = NULL;

	m_pRoomFx = NULL;
	m_lookAtMoving = false;

	m_cameraDisplayType = 1;

	m_savedClipNear = g_view->GetClipNearValue();
	m_savedClipFar = g_view->GetClipFarValue();

	m_pRightMenuWnd = NULL;
	if (IsLocalContent((localContentType_t)0x51)) {
		m_cardTypeId = 0;
		LoadCardImages();
	}

	m_pCardBook = new CCardBook;

	m_pCardPackListBox = NULL;

	m_cutinImgMap.clear();

	m_pickIndex = -1;
}

// realmyroommain.cpp:217
CRealMyRoomMain::~CRealMyRoomMain(void) {
	Net_EquipDresserParts();
	Net_EquipCharacter();
	Net_EquipCaddie();
	Net_EquipItem();
	Net_EquipGear();
	Net_EquipSkin();

	Net_EquipMascot();

	if (IsLocalContent((localContentType_t)0x51)) {
		DeleteCardImages();
		CSharedDoc::Instance()->SendEquipItemSlot();
	} else {
		CSharedDoc::Instance()->SendEquipItemSlotByMascot();
	}

	HidePet(true);

	g_resrcmng->UnlockAllByThread(m_petLoader.GetThreadID());
	m_petLoader.BreakWorkingThread();
	this << MsgObject(NULL, 2, 0, 0, 0, 0, 0);

	if (m_pRoomFx) {
		CFx::Instance()->CloseSequence(m_pRoomFx, false);
	}

	if (g_resrcmng && m_decalTexture) { g_resrcmng->Release(m_decalTexture); m_decalTexture = 0; }
	if (g_resrcmng && m_pTitleFont) { g_resrcmng->Release(m_pTitleFont); m_pTitleFont = NULL; }

	CSceneManager::Instance()->ReleaseElement();

	if (m_pCardBook) {
		delete m_pCardBook;
		m_pCardBook = NULL;
	}

	DeleteCutinImages();
}

// realmyroommain.cpp:272
void CRealMyRoomMain::OnPreLoadInit(void) {
	CTaskMain::OnPreLoadInit();

	// The shadow manager is kept across the avatar chat task.
	if (!((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false)) {
		new CShadowManager(g_resrcmng, COption::Instance()->vIsShadowEnabled());
	}

	CSharedDoc::Instance()->m_gameMode = 0;

	InitMyRoomControls();
	InitMyRoomDialogs();
	InitMyRoomVariables();

	GOLFDOC()->MakePlayer(1);
}

// realmyroommain.cpp:292
void CRealMyRoomMain::DoReLoad(void) {
	SendMsg(this, "Ambient", 500, 0, 0, 0, 0);

	OnLoad();

	CTaskManager *pTaskManager = CTaskManager::Instance();
	(!pTaskManager ? 0 : pTaskManager->GetCurrentTask())->Init(0);
}

// realmyroommain.cpp:301
void CRealMyRoomMain::OnInit(void) {
	CTaskMain::OnInit();

	SetBGMNone();

	CSceneManager::Instance()->UpdateFog(true);

	// "\xb8\xae\xbe\xf3\xb8\xb6\xc0\xcc\xb7\xeb": the real my room
	CSceneManager::Instance()->RegisterElement((eObjType)4, 0, this, "\xb8\xae\xbe\xf3\xb8\xb6\xc0\xcc\xb7\xeb", 0, 0);

	g_input->Reset();

	if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
		m_avatarViewPos = CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_position;
		m_avatarViewPos.y += 4.6f;
		m_avatarViewPos.z -= 46.0f;
	}

	m_characterMap = CSharedDoc::Instance()->m_characterMap;
	// realmyroommain.cpp:322 copies the 0x6C-byte equipment block of
	// CSharedDoc (0x628, m_myCaddieId...) into m_caddieId (0x974) as one
	// structure assignment; the structure is not declared yet.

	if (CMouseCursor::Instance() != NULL) {
		CMouseCursor::Instance()->SetActive(true);
		CMouseCursor::Instance()->m_mode = 2;
	}

	CCameraWork::Instance()->SelectCamera(200);
	CCameraWork::Instance()->SetManualType(0);

	g_camera.Rotate(-0.17453292f, 0);

	m_pCameras[0]->m_matrix = g_camera;
	m_pCameras[0]->m_matrix.pivot.y = g_camera.pivot.y - 5.0f;
	m_pCameras[0]->m_matrix.Rotate(0.34906584f, 1);
	m_pCameras[0]->m_matrix.pivot.x -= 5.0f;

	m_pCameras[0]->m_fov = g_view->GetFOV();
	m_savedCamera.m_matrix = g_camera;
	m_savedCamera.m_fov = g_view->GetFOV();
	m_savedCamera.m_matrix.pivot.y -= 5.0f;
	g_camera.pivot.y -= 7.0f;
	m_uiState = 0;
	g_view->SetFOV(0.78539819f);
}

// Nameless per-unit rdata constant: degrees to radians.
extern "C" {
static const float data_9fa708 = 0.017453292f;
}

// realmyroommain.cpp:445
void CRealMyRoomMain::OnLoad(void) {
	sSlotInfo slotInfo;
	sCharacterInfo charInfo;

	CCameraWork::Instance()->Load();

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(WNetworkSystem::NET_UNIT_DEFAULT)) {
		SetCurMap(COption::Instance()->dGetMap());
		LoadBg(CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0, COption::Instance()->dGetStartHole());

		WPolySoup::sCamera* camera = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
		if (camera) {
			g_camera = camera->m_matrix;
			g_view->SetFOV(camera->m_fov * data_9fa708);
		}
		m_pCameras[0] = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
		m_pCameras[1] = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");

		m_fovGoalDeg = m_fovDeg = camera->m_fov;

		m_pNamedCameras[0] = GOLFDOC()->m_pPolySoup->FindCamera("\xc4\xab\xb8\xde\xb6\xf3\xb3\xa1\xc1\xa1");
		m_pNamedCameras[1] = GOLFDOC()->m_pPolySoup->FindCamera("\xc3\xca\xb1\xe2\xb0\xaa");
		m_pRoomPoint = GOLFDOC()->m_pPolySoup->FindPoint("\xbd\xc3\xc0\xdb\xc1\xa1");

		for (int i = 0; i < 4; i++) {
			char name[256];
			sprintf(name, "savecamera%d", i);

			m_pCameras[2 + i] = GOLFDOC()->m_pPolySoup->FindCamera(name);
		}
		m_pCameras[6] = GOLFDOC()->m_pPolySoup->FindCamera("uicamera0");

		WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb8\xb6\xc0\xcc\xb7\xeb\xbd\xc3\xc0\xdb\xc1\xa1"));
		if (point == 0)
			point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));

		WVector pos = point->m_pos;
		pos = GetPVS().GetGroundPoint(pos, false, 0, false);
		GetPVS().SetAvatarPosition(pos, 2.0f);
		m_cameraDropOrigin = pos;

		m_pRoomFx = CFx::Instance()->OpenSequence("room_point.seq", true);
		if (m_pRoomFx) {
			m_pRoomFx->m_pos = WVector(pos.x, pos.y, pos.z);
		}

		m_lookAt = pos;

		CSceneManager::Instance()->SetWeather(0, false);

		CRealMyRoom_AvatarManager::Instance()->FreeAllAvatars();
	} else {
		SetCurMap(COption::Instance()->dGetMap());
		LoadBg(CSharedDoc::Instance()->m_pCurCourse ? CSharedDoc::Instance()->m_pCurCourse->m_mapName : 0, COption::Instance()->dGetStartHole());

		WPolySoup::sCamera* camera = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
		if (camera) {
			g_camera = camera->m_matrix;
			g_view->SetFOV(camera->m_fov * data_9fa708);
		}
		m_pCameras[0] = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
		m_pCameras[1] = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");

		m_fovGoalDeg = m_fovDeg = camera->m_fov;

		m_pNamedCameras[0] = GOLFDOC()->m_pPolySoup->FindCamera("\xc4\xab\xb8\xde\xb6\xf3\xb3\xa1\xc1\xa1");
		m_pNamedCameras[1] = GOLFDOC()->m_pPolySoup->FindCamera("\xc3\xca\xb1\xe2\xb0\xaa");
		m_pRoomPoint = GOLFDOC()->m_pPolySoup->FindPoint("\xbd\xc3\xc0\xdb\xc1\xa1");

		for (int i = 0; i < 4; i++) {
			char name[256];
			sprintf(name, "savecamera%d", i);

			m_pCameras[2 + i] = GOLFDOC()->m_pPolySoup->FindCamera(name);
		}
		m_pCameras[6] = GOLFDOC()->m_pPolySoup->FindCamera("uicamera0");

		CSceneManager::Instance()->SetWeather(COption::Instance()->dGetWeather(), false);

		CRealMyRoom_AvatarManager::Instance()->FreeAllAvatars();

		WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb8\xb6\xc0\xcc\xb7\xeb\xbd\xc3\xc0\xdb\xc1\xa1"));
		if (point == 0)
			point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xbd\xc3\xc0\xdb\xc1\xa1"));

		WVector pos = point->m_pos;
		pos = GetPVS().GetGroundPoint(pos, false, 0, false);
		m_cameraDropOrigin = pos;

		GetPVS().SetAvatarPosition(pos, 2.0f);

		m_lookAt = pos;
		m_pRoomFx = CFx::Instance()->OpenSequence("room_point.seq", true);

		if (m_pRoomFx) {
			m_pRoomFx->m_pos = WVector(pos.x, pos.y, pos.z);
		}

		memset(&slotInfo, 0, sizeof(slotInfo));

		slotInfo.m_angle = 0.0f;
		slotInfo.m_attribute = (slotInfo.m_attribute & ~0x18) | 0x08;  // bits 3-4: 1 (the room master)
		slotInfo.m_uid = 0;
		slotInfo.m_titleTypeId = 0;
		slotInfo.m_level = 1;
		rand();
		slotInfo.m_x = pos.x;
		slotInfo.m_z = pos.z;
		slotInfo.m_attributeBits.userFlag = 0;
		strcpy(slotInfo.m_nickname, "bianca");
		slotInfo.m_characterTypeId = COption::Instance()->dGetDefaultCharacter() | 0x4000000;
		CSharedDoc::Instance()->m_roomPlayerList.push_back(slotInfo);

		memset(&charInfo, 0, sizeof(charInfo));
		charInfo.typeId = slotInfo.m_characterTypeId;

		_util::GenerateRandomTidParts(charInfo.typeId, charInfo.partTypeIds, charInfo.auxParts.tids);

		CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar = CRealMyRoom_AvatarManager::Instance()->CreateAvatar(slotInfo, charInfo, this);
		static_cast<CRealMyRoomTask*>(GetTask())->AddAvatarBurden(CRealMyRoom_AvatarBurden(0, charInfo, 0));

		for (int i = 1; i < COption::Instance()->dGetNumPlayers(); i++) {
			point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));
			if (point == 0)
				point = GOLFDOC()->m_pPolySoup->FindPoint(MakeStr("\xbd\xc3\xc0\xdb\xc1\xa1"));

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

			CRealMyRoom_AvatarManager::Instance()->CreateAvatar(slotInfo, charInfo, this);
			static_cast<CRealMyRoomTask*>(GetTask())->AddAvatarBurden(CRealMyRoom_AvatarBurden(i, charInfo, 0));
		}

		CRealMyRoomData::Instance()->OnLoad();
	}

	m_lightSet = g_lightset;

	GOLFDOC()->m_pSky->GenerateClouds(50, Wind().GetGlobalIntensity());

	Wind().Init(0xFF, 0xFF, 0, 1);

	Wind().CheckSpecialWind();
	Wind().SetWind(WVector(1.0f, 0.0f, 1.0f));
	GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
	CSceneManager::Instance()->UpdateWind();

	m_shadowTexture = g_resrcmng->LoadTexture("[shadow.jpg", 0, 0, 0);

	CSceneManager::Instance()->ToogleNoOverlapCamera();

	const Waabb& aabb = GetPVS().m_pBaseModel->m_aabb;
	WVector boxMin = aabb.min;
	WVector boxMax = aabb.max;

	float top = boxMax.y;
	WPolySoup::sModel* wall = GOLFDOC()->m_pPolySoup->FindModel("wall_a.pet");
	if (wall)
		top = wall->m_aabb.max.y;

	m_debugLineFrom = WVector(boxMax.x, top, boxMin.z);

	m_debugLineTo = WVector(boxMax.x, boxMin.y, boxMin.z);
	m_debugLineMid = WVector(boxMax.x, (boxMin.y + top) * 0.5f, boxMin.z);

	m_debugLine2From = WVector(boxMin.x, top, boxMin.z);
	m_debugLine2To = WVector(boxMin.x, boxMin.y, boxMin.z);
	m_debugLine2Mid = WVector(boxMin.x, (boxMin.y + top) * 0.5f, boxMin.z);

	m_debugLine3From = WVector((boxMin.x + boxMax.x) * 0.5f, top, boxMin.z);
	m_debugLine3To = WVector((boxMin.x + boxMax.x) * 0.5f, boxMin.y, boxMin.z);

	m_pGenderIcons[0] = g_resrcmng->GetOverlay("i_male.tga", 0);
	m_pGenderIcons[1] = g_resrcmng->GetOverlay("i_female.tga", 0);
	m_pGenderIcons[2] = g_resrcmng->GetOverlay("i_male_02.tga", 0);
	m_pGenderIcons[3] = g_resrcmng->GetOverlay("i_female_02.tga", 0);
	m_pGenderIcons[4] = g_resrcmng->GetOverlay("i_male_03.tga", 0);
	m_pGenderIcons[5] = g_resrcmng->GetOverlay("i_female_03.tga", 0);

	m_pMannerIcons[0] = g_resrcmng->GetOverlay("i_male_manner.tga", 0);
	m_pMannerIcons[1] = g_resrcmng->GetOverlay("i_female_manner.tga", 0);

	m_pAngelIcons[0] = g_resrcmng->GetOverlay("i_male_angel.tga", 0);
	m_pAngelIcons[1] = g_resrcmng->GetOverlay("i_female_angel.tga", 0);

	CTaskMain::OnLoad();
	LoadMyRoomImages();
}

// realmyroommain.cpp:681
void CRealMyRoomMain::OnDestroy(void) {
	CTaskMain::OnDestroy();

	// The shadow manager is kept across the avatar chat task.
	if (!((__rtti_obj = CTaskManager::Instance()->GetPreservedTask()) != 0 ? ((IObject*)__rtti_obj)->IsExactKindOf(&ntAvatarChatTask::m_RTTI) : false)) {
		if (CShadowManager::Instance())
			delete CShadowManager::Instance();
	}

	int i;
	for (i = 0; i < 6; i++) {
		if (g_resrcmng && m_pGenderIcons[i]) {
			g_resrcmng->Release(m_pGenderIcons[i]);
			m_pGenderIcons[i] = 0;
		}
	}

	for (i = 0; i < 2; i++) {
		if (g_resrcmng && m_pMannerIcons[i]) {
			g_resrcmng->Release(m_pMannerIcons[i]);
			m_pMannerIcons[i] = 0;
		}
	}

	for (i = 0; i < 2; i++) {
		if (g_resrcmng && m_pAngelIcons[i]) {
			g_resrcmng->Release(m_pAngelIcons[i]);
			m_pAngelIcons[i] = 0;
		}
	}

	if (g_resrcmng && m_shadowTexture) {
		g_resrcmng->Release(m_shadowTexture);
		m_shadowTexture = 0;
	}

	// The guids of the avatars, collected before the messages are sent.
	int guids[30];
	int count = 0;
	for (i = 0; i < 30 && count < CRealMyRoom_AvatarManager::Instance()->m_avatarCount; i++) {
		if (CRealMyRoom_AvatarManager::Instance()->m_pAvatars[i])
			guids[count++] = CRealMyRoom_AvatarManager::Instance()->m_pAvatars[i]->m_guid;
	}

	for (i = 0; i < count; i++) {
		this << MsgObject(this, 0xE5, guids[i], 0, 0, 0, 0);
	}

	g_pFresh->GetManager()->SetExclusiveKey(true);
}

// Nameless zero-initialised flags of OnProcess (no guard, so file statics):
// 0xAF5A9C set when a right drag moved the pointer, 0xAF5A9D tested by the
// furniture placement, 0xAF5A9E the own avatar walks to a target (shows the
// decal), 0xAF5A9F the own avatar walks by the keyboard.
extern "C" {
static bool data_af5a9c;
static bool data_af5a9d;
static bool data_af5a9e;
static bool data_af5a9f;
}

// realmyroommain.cpp:726
void CRealMyRoomMain::OnProcess(float elapsed) {
	static WVector s_keyMoveDir = WVector::UNIT_POS_Z;  // 0xAF5AAC, the keyboard walking direction
	static WVector s_cameraFront = WVector::UNIT_POS_X; // 0xAF5AA0, the camera's flat front (line 781)

	CTaskMain::OnProcess(elapsed);

	GetPVS().PreProcess(elapsed, false);

	if (!CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
		return;
	}

	if (g_pFresh->GetManager()->GetDesktop()->m_nFlags.GetFlag(0x8)) {
		int leftButton = g_input->GetButton(LEFT_BUTTON);
		int rightButton = g_input->GetButton(RIGHT_BUTTON);
		int middleButton = g_input->GetButton(MIDDLE_BUTTON);
		WVector mouseDelta = g_input->GetMouseDelta();

		WVector mouse = g_input->GetMousePoint();
		WVector rayFrom, rayDir;
		g_view->GetScreen2World(mouse.x, mouse.y, rayFrom, rayDir);

		if (m_pRightMenuWnd) {
			// Any click outside the right menu closes it.
			if (!CMouseCursor::Instance()->InArea(m_pRightMenuWnd->GetRect())) {
				if (leftButton == 1 || rightButton == 1) {
					m_pRightMenuWnd->SetVisible(false);
				}
			}
		}

		float topViewSpeed = m_cameraFov * 0.0010416667f;
		if (!(topViewSpeed > 0.1f)) {
			topViewSpeed = 0.1f;
		}

		if (!data_af5a9f) {
			s_cameraFront = g_view->camera.xa;
			s_cameraFront = s_cameraFront.Normalize();

			s_keyMoveDir = WCrossProduct(s_cameraFront, WVector::UNIT_POS_Y);
			s_keyMoveDir = s_keyMoveDir.Normalize();
		}

		if (m_pFurnitureListNumWnd && CMouseCursor::Instance()->GetArea() && m_pFurnitureListNumWnd->IsVisible()) {
			m_pFurnitureListNumWnd->SetKeyEvent(false);
		}

		if (CMouseCursor::Instance()->GetArea() == 0) {
			if (!m_topViewMode) {
				CRealMyRoom_AvatarManager::Instance()->m_pLitAvatar = CRealMyRoom_AvatarManager::Instance()->AvatarByRayIntersect(rayFrom, rayDir);
				CRealMyRoom_AvatarManager::Instance()->m_pHighlightAvatar = CRealMyRoom_AvatarManager::Instance()->CheckClick(mouse.x, mouse.y);
				CRealMyRoomData::Instance()->m_pPickedModel = CRealMyRoomData::Instance()->ModelByRayIntersectOBB(g_camera.pivot);

				if (CRealMyRoom_AvatarManager::Instance()->m_pLitAvatar || CRealMyRoom_AvatarManager::Instance()->m_pHighlightAvatar) {
					CMouseCursor::Instance()->SetCursor(1);
				} else {
					CMouseCursor::Instance()->SetCursor(0);
				}
			}

			bool inRightMenu = false;
			if (m_pRightMenuWnd && m_pRightMenuWnd->IsVisible()) {
				WRect menuRect(m_pRightMenuWnd->GetRect());

				if (menuRect.IsInRect(mouse.x, mouse.y)) {
					inRightMenu = true;
				}
			}

			if (!inRightMenu) {
				if (leftButton == 1) {
					if (!m_topViewMode && CRealMyRoom_AvatarManager::Instance()->CheckClick(mouse.x, mouse.y)) {
					} else {
						WVector target = GetPVS().GetRayIntersection(rayFrom, rayDir);
						int groundIndex;
						WVector ground = GetPVS().GetGroundPoint(target, true, &groundIndex, true);
						WVector wallHit = GetPVS().GetRayIntersectionWall(rayFrom, rayDir);

						WVector decalX(4.8f, 0.0f, 0.0f);
						WVector decalZ(0.0f, 0.0f, 4.8f);

						GetPVS().DrawDecal(m_decalTexture, ground, decalX, decalZ, 0.5f, 1.0f, (enumPolyBlend)0x800000, false);

						if (CRealMyRoomData::Instance()->m_pSelectModel && (CRealMyRoomData::Instance()->m_pSelectModel->m_pFurniture->m_typeId & 0x3FF800) != 0x7000) {
							CRealMyRoomData::Instance()->SetTargetPosition(wallHit, 1);
						}

						if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar && !(CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_state & 0x7E)) {
							GetPVS().GetGroundPoint(ground, true, &groundIndex, true);

							if (groundIndex >= 0 && !CRealMyRoomData::Instance()->m_pSelectModel) {
								data_af5a9e = true;
								if (ground.z > m_pRoomPoint->m_pos.z) {
									CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetTargetPosition(ground);
									CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetAction(0);
									CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->Follow(0);
								}
							}
						}

						if (GetFlags(8)) {
							if (CRealMyRoomData::Instance()->GetState() == 1 || CRealMyRoomData::Instance()->GetState() == 3) {
								if (!CRealMyRoomData::Instance()->m_pSelectModel || (CRealMyRoomData::Instance()->m_pSelectModel->m_pFurniture->m_typeId & 0x3FF800) != 0x7000) {
									CRealMyRoomData::Instance()->m_pSelectModel = CRealMyRoomData::Instance()->ModelByRayIntersect(rayFrom, rayDir);
								}
							}
						}
					}
				} else if (leftButton == 3) {
					// Dragging a picked furniture model moves it along the walls.
					if (CRealMyRoomData::Instance()->GetState() == 2) {
						WVector target;
						int groundIndex;
						WVector wallHit;

						data_af5a9d = true;
						target = GetPVS().GetRayIntersection(rayFrom, rayDir);

						target = GetPVS().GetGroundPoint(target, true, &groundIndex, true);
						wallHit = GetPVS().GetRayIntersectionWall(rayFrom, rayDir);

						Waabb aabb;
						GetPVS().GetAABB(aabb);

						if (CRealMyRoomData::Instance()->m_pSelectModel && GetFlags(8)) {
							CRealMyRoomData::Instance()->SetTargetPosition(wallHit, 3);
						}

						CMouseCursor::Instance()->m_mode = 0;
					}
				} else if (leftButton == 2) {
					if (CRealMyRoomData::Instance()->GetState() == 2) {
						if (data_af5a9d) {
							WVector target;
							int groundIndex;
							WVector wallHit;

							target = GetPVS().GetRayIntersection(rayFrom, rayDir);
							target = GetPVS().GetGroundPoint(target, true, &groundIndex, true);
							wallHit = GetPVS().GetRayIntersectionWall(rayFrom, rayDir);

							if (CRealMyRoomData::Instance()->m_pSelectModel) {
								CRealMyRoomData::Instance()->SetTargetPosition(wallHit, 2);
							}

							CRealMyRoomData::Instance()->m_pSelectModel = 0;

							CMouseCursor::Instance()->m_mode = 2;

							data_af5a9d = false;
							CRealMyRoomData::Instance()->SetState(0);
						}
					}
				} else {
					if (CRealMyRoomData::Instance()->GetState() == 2) {
						if (CRealMyRoomData::Instance()->m_pSelectModel && (CRealMyRoomData::Instance()->m_pSelectModel->m_pFurniture->m_typeId & 0x3FF800) == 0x7000) {
							WVector target;
							int groundIndex;
							WVector wallHit;

							data_af5a9d = true;
							target = GetPVS().GetRayIntersection(rayFrom, rayDir);

							target = GetPVS().GetGroundPoint(target, true, &groundIndex, true);
							wallHit = GetPVS().GetRayIntersectionWall(rayFrom, rayDir);

							Waabb aabb;
							GetPVS().GetAABB(aabb);

							if (CRealMyRoomData::Instance()->m_pSelectModel && GetFlags(8)) {
								CRealMyRoomData::Instance()->SetTargetPosition(wallHit, 3);
							}

							CMouseCursor::Instance()->m_mode = 0;
						}
					}
				}

				if (rightButton == 1) {
					if (!m_topViewMode) {
						if (GetFlags(8) || GetFlags(2) || g_pFresh->IsCurrentLayout("REALMYROOMVISITOR")) {
							CRealMyRoomData::Instance()->m_pSelectModel = CRealMyRoomData::Instance()->ModelByRayIntersect(rayFrom, rayDir);
						}
					}
					data_af5a9c = false;
				} else if (rightButton == 3) {
					// Dragging with the right button turns the camera.
					if (!m_topViewMode) {
						static WVector s_lastMouseDelta(0.0f, 0.0f, 0.0f); // 0xAF5A90

						float turnX = (mouseDelta.y + s_lastMouseDelta.y) * 0.5f;

						m_lookAtOffset.y -= turnX * 0.008f;
						s_lastMouseDelta = mouseDelta;

						if (!m_customLookAtLimits) {
							if (m_lookAtOffset.x < -1.5707964f) {
								m_lookAtOffset.x = -1.5707964f;
							}
							if (m_lookAtOffset.x > 0.78539819f) {
								m_lookAtOffset.x = 0.78539819f;
							}
						} else {
							if (m_lookAtOffset.x < m_lookAtXMinDeg * data_9fa708) {
								m_lookAtOffset.x = m_lookAtXMinDeg * data_9fa708;
							}
							if (m_lookAtOffset.x > m_lookAtXMaxDeg * data_9fa708) {
								m_lookAtOffset.x = m_lookAtXMaxDeg * data_9fa708;
							}
							if (m_lookAtOffset.y < m_lookAtYMinDeg * data_9fa708) {
								m_lookAtOffset.y = m_lookAtYMinDeg * data_9fa708;
							}
							if (m_lookAtOffset.y > m_lookAtYMaxDeg * data_9fa708) {
								m_lookAtOffset.y = m_lookAtYMaxDeg * data_9fa708;
							}
						}

						m_headingLookAtOffset = m_lookAtOffset;
						m_cameraIdleTime = 0.0f;
					}

					if (!WisZero(mouseDelta.x, 1e-5f) || !WisZero(mouseDelta.y, 1e-5f)) {
						data_af5a9c = true;
					}

					g_input->FixPointer(true);
					CMouseCursor::Instance()->Toggle(false);
				} else if (rightButton == 2) {
					if (CRealMyRoomData::Instance()->m_pSelectModel) {
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
						if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
							WVector target;
							target = CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_position;
							target += s_keyMoveDir * 4.1f;

							if (!WisEqual(target, CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_position, 1e-5f)
								&& !(CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_state & 0x7E)) {
								data_af5a9e = false;

								CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetTargetPosition(target);
								CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetAction(0);
								CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->Follow(0);
							}
						}
					}
				}

				if (middleButton == 1) {
					ToggleTopViewMode();
				}

				// The wheel rotates the edited furniture, else zooms, unless a scroll bar has it.
				if (!g_pFresh->GetManager()->GetWheelFocus()) {
					if (CRealMyRoomData::Instance()->GetState() == 3) {
						CRealMyRoomData::Instance()->SetRotate(mouseDelta.z);
					} else if (!m_topViewMode) {
						m_fovGoalDeg = Between(33.28f, m_fovGoalDeg - mouseDelta.z * g_input->GetSensitivity() * 0.021f, 60.8f);
					} else {
						m_cameraFov = Between(51.2f, m_cameraFov - mouseDelta.z * g_input->GetSensitivity() * topViewSpeed * 0.4f, 80.0f);
					}
				}
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

			WVector target;
			if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
				target = CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_position;
			}

			// The direction keys are named "up", "down", "left", "right" in CP949.
			if (!CChatMsg::Instance()->IsOpenning()) {
				if (!control) {
					// They walk.
					if (g_input->Get("\xbb\xf3", true)) {
						target.z += s_keyMoveDir.z * 4.1f;
					}
					if (g_input->Get("\xc7\xcf", true)) {
						target.z += s_keyMoveDir.z * -4.1f;
					}
					if (g_input->Get("\xc1\xc2", true)) {
						target.x += s_cameraFront.x * -4.1f;
					}
					if (g_input->Get("\xbf\xec", true)) {
						target.x += s_cameraFront.x * 4.1f;
					}

					if (target.z > -8.0f) {
						int groundIndex;
						target = GetPVS().GetGroundPoint(target, true, &groundIndex, true);
						if (groundIndex < 0) {
							if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
								target = CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_position;
							}
						}
					} else {
						target.z = -8.0f;
					}
				} else if (m_topViewMode) {
					// The top view: they pan the camera offset within [-3, 3].
					if (g_input->Get("\xbb\xf3", true)) {
						m_cameraOffset.z += s_keyMoveDir.z;
						if (m_cameraOffset.z > 3.0f) {
							m_cameraOffset.z = 3.0f;
						}
					}
					if (g_input->Get("\xc7\xcf", true)) {
						m_cameraOffset.z += -s_keyMoveDir.z;
						if (m_cameraOffset.z < -3.0f) {
							m_cameraOffset.z = -3.0f;
						}
					}
					if (g_input->Get("\xc1\xc2", true)) {
						m_cameraOffset.x += -s_cameraFront.x;
						if (m_cameraOffset.x < -3.0f) {
							m_cameraOffset.x = -3.0f;
						}
					}
					if (g_input->Get("\xbf\xec", true)) {
						m_cameraOffset.x += s_cameraFront.x;
						if (m_cameraOffset.x > 3.0f) {
							m_cameraOffset.x = 3.0f;
						}
					}
				} else {
					// With control held they turn the camera.
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
						m_lookAtOffset.y -= turnY;
						m_lookAtOffset.x -= turnX;

						if (m_lookAtOffset.x < -1.5707964f) {
							m_lookAtOffset.x = -1.5707964f;
						}
						if (m_lookAtOffset.x > 0.78539819f) {
							m_lookAtOffset.x = 0.78539819f;
						}

						m_headingLookAtOffset = m_lookAtOffset;
						m_cameraIdleTime = 0.0f;
					}
				}

				if (g_input->Get("HOME", true)) {
					m_fovGoalDeg = Between(33.28f, m_fovGoalDeg - elapsed * 16.8f, 60.8f);
				}

				if (g_input->Get("END", true)) {
					m_fovGoalDeg = Between(33.28f, m_fovGoalDeg + elapsed * 16.8f, 60.8f);
				}

				if (g_input->GetDown("PAGE_DOWN", true)) {
					if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
						if (!(CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_state & 0x7E)) {
							if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_action != 2) {
								if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_action == 1) {
									CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetAction(2);
								} else {
									CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->Stop();
									CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetAction(1);
								}
							}
						}
					}
				}

				if (g_input->GetDown("PAGE_UP", true)) {
					if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
						if (!(CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_state & 0x7E)) {
							if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_action == 1 || CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_action == 2) {
								if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_action == 1) {
									CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetAction(0);
								} else {
									CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetAction(1);
								}
							}
						}
					}
				}
			} else {
				// While the chat is open: home/end (or shift with up/down) zoom the top view.
				if (g_input->Get("HOME", true) || (shift && g_input->Get("\xbb\xf3", true))) {
					m_cameraFov = Between(25.6f, m_cameraFov - topViewSpeed * elapsed * 1200.0f, 960.0f);
				}
				if (g_input->Get("END", true) || (shift && g_input->Get("\xc7\xcf", true))) {
					m_cameraFov = Between(25.6f, m_cameraFov + topViewSpeed * elapsed * 1200.0f, 960.0f);
				}
			}

			if (g_input->GetDown("PAD_0", true) || g_input->GetDown("NUM_0", true)) {
				ToggleTopViewMode();
			}

			if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar
				&& !WisEqual(target, CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_position, 1e-5f)
				&& !(CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_state & 0x7E)) {
				data_af5a9e = false;
				data_af5a9f = true;

				if (target.z > m_pRoomPoint->m_pos.z) {
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetTargetPosition(target);
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->SetAction(0);
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->Follow(0);
				}
			} else {
				data_af5a9f = false;
			}
		}
	} else {
		if (CSharedDoc::Instance()->m_myUID == CSharedDoc::Instance()->m_myRoomOwnerUID) {
			if (m_pRightMenuWnd->IsVisible()) {
				m_pRightMenuWnd->SetVisible(false);
			}
		}

		g_input->FixPointer(false);
		CMouseCursor::Instance()->Toggle(true);
	}

	CRealMyRoom_AvatarManager::Instance()->Process(elapsed);
	ProcessCamera(elapsed);
	CRealMyRoomData::Instance()->Process(elapsed);

	CTransferableItem::Instance()->Process(elapsed);

	if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_motion == 1) {
			m_cameraIdleTime += elapsed;
		}
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_motion == 2 || CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_motion == 3) {
			m_cameraHeightGoal = 1.9f;
		} else {
			m_cameraHeightGoal = 4.6f;
		}

		if (IsEqual(CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_targetPosition.x, CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_position.x)
			&& IsEqual(CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_targetPosition.z, CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_position.z)) {
			data_af5a9e = false;
		}
	}

	m_decalShown = data_af5a9e;

	if (m_petLoader.IsWorking(0)) {
		Sleep(10);
	} else if (m_pPetExhibition) {
		m_petLoader.Enter();

		bool rotate;
		switch (m_mainTab) {
		case 1:
		case 4:
			rotate = false;
			break;
		default:
			rotate = true;
			break;
		}

		m_pPetExhibition->Process(elapsed, rotate);
		m_petLoader.Leave();
	}

	if (g_input->GetDown("ESCAPE", true) == 1 && !g_pFresh->GetManager()->HasEscKeyWindow()) {
		CMessengerInfo::Instance()->Close(false);
	}

	if (m_mainTab == 4) {
		for (int i = 0; i < 2; i++) {
			float speed = i == 0 ? 138.0f : 228.0f;
			switch (m_skinParam[i]) {
			case 0:
				m_skinSlot[i] = 0.0f;
				break;
			case 1:
				m_skinSlot[i] -= elapsed * speed * 0.25f;
				break;
			case 2:
				m_skinSlot[i] += elapsed * speed * 0.25f;
				break;
			}
		}
	}

	CIconToolTip* pToolTip = CIconToolTip::GetInstance();
	switch (pToolTip->GetCurTipIndex()) {
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1E:
	case 0x1F:
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x27:
	case 0x3E:
		OnBarHoverOff(0);
		break;
	}

	bool handled = false;
	SendMsg(this, "ItemBuff", 0x246, 2, (intptr_t)&handled, 0, 0);
	if (handled) {
		OnBarHoverOff(0);
	}

	if (IsLocalContent((localContentType_t)0x57)) {
		RefreshAllUccClothes();
	}

	if (CSharedDoc::Instance()->m_myUID == CSharedDoc::Instance()->m_myRoomOwnerUID) {
		if ((g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) && g_input->GetDown("HIDE", true) && !GetFlags(8)) {
			SelectMenu(0);
		}

		if ((g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true)) && g_input->GetDown("FURNITURE_UI", true)) {
			SelectMenu(2);
		}
	}

	g_input->GetDown("ENTER", true);
}

// realmyroommain.cpp:1814
void CRealMyRoomMain::Display(void) {
	if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar == NULL)
		return;

	CRealMyRoom_AvatarManager::Instance()->Render();
	CRealMyRoomData::Instance()->Render();

	CTransferableItem::Instance()->Render();

	// The pulsing decal under the avatar the player follows, or under his own target.
	if (m_decalShown) {
		WVector right, forward;
		right = WVector(4.8f, 0.0f, 0.0f);
		forward = WVector(0.0f, 0.0f, 4.8f);

		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_followNickName[0] != '\0') {
			CRealMyRoom_Avatar *pAvatar = CRealMyRoom_AvatarManager::Instance()->FindAvatarByNickName(CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_followNickName);
			if (pAvatar)
				GetPVS().DrawDecal(m_decalTexture, pAvatar->m_position, right, forward, (cos(m_decalPulse * 10.0f) + 1.0f) * 0.05f + 1.0f, 1.0f, W_POLY_BLEND_ADD, false);
		} else {
			GetPVS().DrawDecal(m_decalTexture, CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_targetPosition, right, forward, (cos(m_decalPulse * 10.0f) + 1.0f) * 0.05f + 1.0f, 1.0f, W_POLY_BLEND_ADD, false);
		}
	}

	DrawMouseCursor();
}

// realmyroommain.cpp:1868
void CRealMyRoomMain::DrawMouseCursor(void) {
	int cursor = CMouseCursor::Instance()->GetCursor();
	static int s_oldCursor = cursor;

	if (m_pRightMenuWnd && m_pRightMenuWnd->IsVisible()) {
		CMouseCursor::Instance()->SetCursor(s_oldCursor);
		return;
	}

	if (CRealMyRoomData::Instance()->GetState() == 2 && CRealMyRoomData::Instance()->m_pSelectModel) {
		if (CRealMyRoomData::Instance()->m_bDropped)
			CMouseCursor::Instance()->SetCursor(9);
		else
			CMouseCursor::Instance()->SetCursor(8);
		return;
	}

	if (CRealMyRoomData::Instance()->GetState() == 3 && CRealMyRoomData::Instance()->m_pSelectModel) {
		if (CRealMyRoomData::Instance()->m_bDropped)
			CMouseCursor::Instance()->SetCursor(11);
		else
			CMouseCursor::Instance()->SetCursor(10);
		return;
	}

	if (cursor != s_oldCursor)
		CMouseCursor::Instance()->SetCursor(s_oldCursor);

	s_oldCursor = CMouseCursor::Instance()->GetCursor();
}

// realmyroommain.cpp:1899
void CRealMyRoomMain::OnDisplay(void) {
	// The avatars' name tags and balloons.
	CRealMyRoom_AvatarManager::Instance()->RenderOverlay();

	g_view->DrawLine(m_debugLineFrom, 0xFFFFFFFF, m_debugLineTo, 0xFFFFFFFF, 0);
	g_view->DrawLine(m_debugLine2From, 0xFFFFFFFF, m_debugLine2To, 0xFFFFFFFF, 0);
}

// realmyroommain.cpp:1949
void CRealMyRoomMain::LoadBg(const char* name, int hole) {
	m_bgName = name;
	GOLFDOC()->m_currentHole = hole;

	if (CFx::Instance()) {
		CFx::Instance()->SetActive(false);
	}

	if (!GOLFDOC()->m_pPolySoup->Load("myroom_01_01.gbin", false)) {
		g_bQuit = true;
		return;
	}

	GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", name));
	GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", name));

	GOLFDOC()->m_pSky = new CSky;
	GOLFDOC()->m_pSky->LoadSky(name, 0.0f);

	GOLFDOC()->m_pPolySoup->MakeDataStructure();

	if (GOLFDOC()->m_pSky) {
		GOLFDOC()->m_pSky->GenerateStars(-1);
		GOLFDOC()->m_pSky->GenerateLensFlare();
		GOLFDOC()->m_pSky->GenerateMoons();
	}

	CShadowManager::Instance()->UpdateLight("sun", g_lightset);

	CSceneManager::Instance()->Load();

	GOLFDOC()->m_pPolySoup->EndLoad(false);

	CTransferableItem::Instance()->OnLoad();

	if (CFx::Instance()) {
		CFx::Instance()->SetActive(true);
	}
}

// realmyroommain.cpp:2000
void CRealMyRoomMain::CheckUIPosition(void) {
	static float s_barY = COption::Instance()->gGetBar_Y();

	if (COption::Instance()->gGetBar_Y() != s_barY) {
		CalcUIOffset(false);
		s_barY = COption::Instance()->gGetBar_Y();

		g_pFresh->OpenLayout("REALMYROOM", this, false);
	}
}

// Nameless file-static int of this unit: the room camera ProcessCamera's
// type 2 shows, visiting m_pCameras[3..5] one after the other.
extern "C" {
static int data_a888d0 = 1;
}

// realmyroommain.cpp:2024
void CRealMyRoomMain::ProcessCamera(float elapsed) {
	WMatrix mat;
	WVector target;
	WVector dir;
	WVector eye;
	float limit;
	float fov;

	limit = 8.0f;
	g_view->SetClip(1.0f, m_savedClipFar, false);

	switch (m_cameraDisplayType) {
	case 0:
		mat.Reset();

		mat = g_camera;
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
			target = CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_position;
		}

		dir = target - m_lookAt;

		target.y = g_camera.pivot.y;
		m_lookAt.y = target.y;
		dir *= elapsed * 4.0f;

		{
			float dist = (target - m_lookAt).Magnitude();

			float dz = fabsf(g_camera.pivot.z - target.z);
			if (dz > limit) {
				limit = dz;
			}

			if (dist == 0.0f) {
				m_lookAtMoving = false;
			} else if (dist > 3.0f / limit || m_lookAtMoving == true) {
				m_lookAtMoving = true;
				m_lookAt.x += dir.x;
				m_lookAt.z += dir.z;
			}
		}

		m_lookAt.y = g_camera.pivot.y;

		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar && CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_hide && target.z < m_cameraDropOrigin.z) {
			g_camera.pivot.y = m_savedCamera.m_matrix.pivot.y - fabsf(target.z - m_cameraDropOrigin.z);
			m_lookAt.y = 8.0f;
		}

		if (!WisEqual(m_fovDeg, m_fovGoalDeg, 0.00001f)) {
			float step = elapsed * 46.0f;

			if (m_fovGoalDeg > m_fovDeg) {
				m_fovDeg += step;
				if (m_fovDeg > m_fovGoalDeg) {
					m_fovDeg = m_fovGoalDeg;
				}
			} else {
				m_fovDeg -= step;
				if (m_fovDeg < m_fovGoalDeg) {
					m_fovDeg = m_fovGoalDeg;
				}
			}
		}

		g_view->SetFOV(m_fovDeg * data_9fa708);

		if (m_lookAtOffset.y < -25.0f) {
			m_lookAtOffset.y = -25.0f;
		} else if (m_lookAtOffset.y > 23.0f) {
			m_lookAtOffset.y = 23.0f;
		}

		m_lookAt.y += m_lookAtOffset.y;
		g_camera = MakeCameraMatrix(g_camera.pivot, m_lookAt);
		g_view->SetCamera(g_camera);
		break;

	case 1:
		g_view->SetCamera(g_camera);
		break;

	case 3:
		mat.Reset();

		g_view->SetClip(8.0f, 8192.0f, false);

		g_view->SetFOV(m_cameraFov * data_9fa708);

		mat = RotMat(WVector(-1.5707964f, 0.0f, 0.0f));

		target = m_cameraOffset;
		eye = m_savedCamera.m_matrix.pivot;

		eye.z += 5.0f;
		eye.y = 25.0f;
		mat = MakeCameraMatrix(eye, target);

		g_camera = mat;
		g_view->SetCamera(mat);
		break;

	case 4:
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
			target = CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_position;
		}

		target.y = m_eyePos.y;
		dir = m_eyePos - g_camera.pivot;
		if (dir.Magnitude() < 1.0f) {
			fov = g_view->GetFOV();
			if (m_eyeFov == fov) {
				SetCameraDisplayType(0);
				return;
			}

			CalculationCameraMat(mat, target, WVector::ZERO, g_camera.pivot, m_eyeFov, fov, elapsed * 5.0f);
			g_camera = mat;
			g_view->SetCamera(g_camera);
			break;
		} else {
			float curFov = g_view->GetFOV();
			CalculationCameraMat(mat, target, dir, g_camera.pivot, curFov, curFov, elapsed * 5.0f);
		}

		g_camera = mat;
		g_view->SetCamera(g_camera);
		break;

	case 2:
		target = WVector(0.0f, 0.0f, 5.0f);
		dir = m_eyePos - g_camera.pivot;
		if (dir.Magnitude() < 1.0f) {
			data_a888d0++;
			if (data_a888d0 == 4) {
				m_eyePos = m_pCameras[2]->m_matrix.pivot;
			} else if (data_a888d0 == 5) {
				SetCameraDisplayType(0);

				g_view->SetClip(m_savedClipNear, m_savedClipFar, false);
				data_a888d0 = 1;
			} else {
				m_eyePos = m_pCameras[data_a888d0 + 2]->m_matrix.pivot;
			}
			break;
		}

		CalculationCameraMat(mat, target, dir, g_camera.pivot, m_eyeFov, g_view->GetFOV(), elapsed * 8.0f);
		g_camera = mat;
		g_view->SetCamera(g_camera);
		break;

	case 5:
		g_view->SetFOV(m_cameraFov);

		target = WVector(0.0f, 0.0f, 5.0f);
		dir = m_eyePos - g_camera.pivot;
		if (dir.Magnitude() < 1.0f) {
			mat =MakeCameraMatrix(g_camera.pivot, m_cameraOffset);
		} else {
			float curFov = g_view->GetFOV();
			CalculationCameraMat(mat, target, dir, g_camera.pivot, curFov, curFov, elapsed * 8.0f);
		}

		g_camera = mat;
		g_view->SetCamera(g_camera);
		break;
	}

	g_view->UpdateCamera();
}

// realmyroommain.cpp:2289
void CRealMyRoomMain::CalculationCameraMat(WMatrix& mat, WVector at, WVector dir, WVector pos, float targetFov, float curFov, float speed) {
	mat = g_camera;

	float fov = curFov;

	dir.Normalize();
	dir *= speed;

	pos += dir;

	if (!(WAbsBySign(curFov - targetFov) < 0.00001f)) {
		float step = speed * 46.0f;

		if (targetFov > curFov) {
			fov = curFov + step;
			if (fov > targetFov)
				fov = targetFov;
		} else {
			fov = curFov - step;
			if (fov < targetFov)
				fov = targetFov;
		}
	}

	g_view->SetFOV(fov);

	mat = MakeCameraMatrix(pos, at);
}

// realmyroommain.cpp:2452
void CRealMyRoomMain::ToggleTopViewMode(void) {
	if (GetFlags(1))
		return;

	m_topViewMode = !m_topViewMode;

	if (m_topViewMode) {
		GOLFDOC()->GameMode() = 0x200;

		m_cameraFov = 70.0f;
		m_cameraOffset = WVector::ZERO;
		m_cameraDisplayType = 3;
	} else {
		GOLFDOC()->GameMode() = 0;

		SetCameraDisplayType(0);
	}
}

// Nameless file-static int of this unit: the camera type 5 shows next
// (m_pCameras[2..5]) in SetCameraDisplayType.
extern "C" {
static int data_af5a8c;
}

// realmyroommain.cpp:2508
void CRealMyRoomMain::SetCameraDisplayType(int type) {
	if (type != 0) {
		switch (type) {
		case 1:
			g_camera = m_pCameras[0]->m_matrix;
			g_view->SetFOV(m_pCameras[0]->m_fov);
			break;

		case 2:
			g_camera = m_pCameras[2]->m_matrix;
			g_view->SetFOV(0.7330383f);

			m_eyePos = m_pCameras[3]->m_matrix.pivot;
			m_eyeFov = 0.7330383f;
			break;

		case 3:
			m_cameraFov = 70.0f;
			m_cameraOffset = WVector::ZERO;
			break;

		case 4:
			if (m_pCameras[6]) {
				g_camera = m_pCameras[6]->m_matrix;
				g_view->SetFOV(m_pCameras[0]->m_fov);
			} else {
				g_camera = m_savedCamera.m_matrix;
				g_view->SetFOV(m_savedCamera.m_fov);
			}

			m_eyePos = m_savedCamera.m_matrix.pivot;
			m_eyeFov = m_savedCamera.m_fov;
			break;

		case 5:
			if (m_pCameras[data_af5a8c + 2])
				g_camera = m_pCameras[data_af5a8c + 2]->m_matrix;
			else
				g_camera = m_savedCamera.m_matrix;

			g_view->SetFOV(70.0f);

			data_af5a8c++;

			m_cameraFov = 70.0f;
			m_cameraOffset = WVector(0.0f, 0.0f, 5.0f);

			// Past the last camera: back to the saved view.
			if (data_af5a8c >= 4) {
				data_af5a8c = 0;
				SetCameraDisplayType(4);
				return;
			}

			if (m_pCameras[data_af5a8c + 2])
				m_eyePos = m_pCameras[data_af5a8c + 2]->m_matrix.pivot;
			else
				m_eyePos = m_savedCamera.m_matrix.pivot;
			break;
		}
	} else {
		if (m_cameraDisplayType != 4)
			g_camera = m_savedCamera.m_matrix;
		g_view->SetFOV(m_savedCamera.m_fov);
	}

	m_cameraDisplayType = type;
}

// realmyroommain.cpp:2601
void CRealMyRoomMain::RefreshAllUccClothes(void) {
	sUccRefreshItem item;

	for (int i = 0; i < 10; i++) {
		if (!UccManager()->GetRefreshItem(item))
			break;

		// The pet exhibition shows the player's own parts.
		if (m_pPetExhibition && m_pPetExhibition->GetPetFrame()) {
			CPartTidList& tidList = CSharedDoc::Instance()->GetMyPartTidList();

			unsigned char slot = (unsigned char)((item.m_typeId >> 13) & 0x1F);
			if (tidList.m_partTids[slot] == item.m_typeId) {
				unsigned long serial = tidList.m_partIds[slot];

				sUccClothes* pClothes = UccManager()->FindClothes(serial);

				if (pClothes && strcmpi(pClothes->m_uccKey, item.m_uccKey) == 0) {
					m_pPetExhibition->GetPetFrame()->SetPart(&CSharedDoc::Instance()->GetMyPartTidList(), pClothes->m_typeId, pClothes->m_serial);
					m_pPetExhibition->GetPetFrame()->SetPart(&CSharedDoc::Instance()->GetMyPartTidList(), pClothes->m_typeId, pClothes->m_serial);
				}
			}
		}

		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar) {
			CRealMyRoom_Avatar* pMyAvatar = CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar;

			unsigned char slot = (unsigned char)((item.m_typeId >> 13) & 0x1F);
			if (pMyAvatar->m_partTidList.m_partTids[slot] == item.m_typeId) {
				unsigned long serial = pMyAvatar->m_partTidList.m_partIds[slot];

				sUccClothes* pClothes = UccManager()->FindClothes(serial);

				if (pClothes && strcmpi(pClothes->m_uccKey, item.m_uccKey) == 0) {
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->RefreshUccClothes(pClothes->m_typeId, pClothes->m_serial);
				}
			}
		}

		for (int j = 0; j < CRealMyRoom_AvatarManager::Instance()->m_avatarCount; j++) {
			CRealMyRoom_Avatar* pAvatar = CRealMyRoom_AvatarManager::Instance()->m_pAvatars[j];

			if (pAvatar) {
				unsigned char slot = (unsigned char)((item.m_typeId >> 13) & 0x1F);
				if (pAvatar->m_partTidList.m_partTids[slot] == item.m_typeId) {
					unsigned long serial = pAvatar->m_partTidList.m_partIds[slot];

					sUccClothes* pClothes = UccManager()->FindClothes(serial);

					if (pClothes && strcmpi(pClothes->m_uccKey, item.m_uccKey) == 0) {
						pAvatar->RefreshUccClothes(pClothes->m_typeId, pClothes->m_serial);
					}
				}
			}
		}
	}
}
