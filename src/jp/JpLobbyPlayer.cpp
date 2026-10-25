// The JP 983 lobby's 3D character actor LobbyPlayer ("LobbyAvatar",
// JpLobbyPlayer.h), from the JP exe's bodies (node scripts/disasm.ts --jp
// 0x<va>). Not registered yet: KR CLobbyTask::Register adds no
// "LobbyAvatar" (the JP Register 0xabfcc0 does).
#include "wmath.h"

#include "JpLobbyPlayer.h"

#include "JpLobbyGlue.h"
#include "JpPetFrameGlue.h"

#include <stdlib.h>
#include <string.h>

#include "GolfDoc.h"
#include "ItemManager.h"
#include "parttidlist.h"
#include "UccManager.h"
#include "wbone.h"
#include "wresrcmng.h"
#include "polysoup.h"
#include "commonutil.h"
#include "partsinfo.h"
#include "wpuppet.h"
#include "RFLobbyDesktop.h"
#include "RefreshInputHandler.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "SharedDoc.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frwnd.h"
#include "wview.h"
#include "InputManager.h"
#include "JpTooltipWnd.h"
#include "RFManager.h"
#include "RFWnd.h"

// The Wangreal zero rect at JP 0xf145f8 (defined in JpItemSearchDlg.cpp).
extern const WRect data_f145f8;
#include "TaskManager.h"
#include "actor.h"

// 0xe751c0
const WRTTI LobbyPlayer::m_RTTI("LobbyPlayer", &IActor::m_RTTI);

// The CP949 names of the lobby scene's objects.
static const char kStartPointName[] = "\xBD\xC3\xC0\xDB\xC1\xA1";             // "시작점" (0xd0a510)
static const char kDefaultPoseName[] = "\xB1\xE2\xBA\xBB\xC0\xDA\xBC\xBC";    // "기본자세" (0xcfeca0)
// 0xe0c294 / 0xe0c298: the attach's character-change effect and sound.
static const char* const kCharChangeSequence = "avatar_d.seq";  // 0xd0a5c8
static const char* const kCharChangeSound = "char_change";      // 0xd0a5bc
// 0xcfec68: the suffix of a motion that has ended ("끝").
static const char kMotionEndSuffix[] = "\xB3\xA1";

// lib: 0x568d20 / 0x568960 / 0x568820 (std::map erase, subtree erase, node
// value destroy: JpLobbyPlayerTable::Clear), 0x5683d0 (shared_ptr assign),
// 0x567530 (the build request's implicit constructor: std::tr1::function's),
// 0x567540 (its implicit destructor: std::tr1::function::_Tidy), 0x568d20 (the
// two maps' erase in the destructor), 0x4d7320 / 0x4d7630 / 0x4d6bf0 /
// 0x4d7490 / 0x4d7840 / 0x4d6c30 (std::tr1::shared_ptr reset, assign, dtor)

// Sends a parameterless message to the current task's "Lobby" actor (the
// inlined CTask::GetActor + IActor::operator<< of the loaders).
static void SendToLobby(int msg) {
	IActor* lobby = AfxGetTask()->GetActor("Lobby");
	if (lobby != NULL) {
		lobby << MsgObject(NULL, msg, 0, 0, 0, 0, 0);
	}
}

// JP CSharedDoc +0x8cb: the KR MyUID() (glue, no JP body).
int Jp_GameState_AvatarNameKey() {
	return (int)MyUID();
}

// The JP object 0xe73e14's +0x19c table as 0x4cee80 fills it: per
// character type index, the bone a part change makes visible again.
static const char* const s_jpCharacterSubBoneNames[15] = {
	"M_WI_11_SUB", "F_WI_11_SUB", "A_WI_11_SUB", "C_WI_11_SUB", "D_WI_11_SUB",
	"E_WI_11_SUB", "G_WI_11_SUB", "H_WI_11_SUB", "I_WI_11_SUB", "J_WI_11_SUB",
	"K_WI_11_SUB", "MM_WI_11_SUB", "FF_WI_11_SUB", "A_WI_11_SUB", "CC_WI_11_SUB",
};

// 0x4cef20 (thiscall on the JP object at 0xe73e14, unplaced): its +0x19c
// entry for the character type index (IDummyPlayer::m_typeId).
const char* Jp_CharacterBoneName(uint32_t typeIndex) {
	// Ours: the JP reads past the table for larger indexes.
	if (typeIndex >= sizeof(s_jpCharacterSubBoneNames) / sizeof(s_jpCharacterSubBoneNames[0])) {
		return "";
	}
	return s_jpCharacterSubBoneNames[typeIndex];
}

// 0x4db4b0
float Jp_EaseInQuartic(float from, float to, float time, float duration) {
	float t = time / duration;
	float range = to - from;
	return from + range * t * t * t * t;
}

// 0x565620 (unplaced): the ray of the screen point (WView::GetScreen2World
// on g_view, 0xb4fdd0) enters the 5 x 9 x 5 box standing on the avatar's
// position (RayIntersectAABB, the out-of-line copy 0x565300).
bool Jp_LobbyPlayer_HitTest(const WVector& mouse, const WVector& position) {
	WVector from;
	WVector delta;
	g_view->GetScreen2World(mouse.x, mouse.y, from, delta);
	WVector boxMax(2.5f, 9.0f, 2.5f);    // 0xd08b8c, 0xd021c8
	WVector boxMin(-2.5f, 0.0f, -2.5f);  // 0xd0a5b8
	Waabb box(position + boxMin, position + boxMax);
	return RayIntersectAABB(box, from, delta, NULL, 0.0f) < 1.0f;
}

// 0x565730
WRect Jp_LobbyPlayerPointFromPosition(const WVector& position) {
	if (!WSingleton<RFSceneManager>::IsInstantiated()) {
		return data_f145f8;
	}
	RFManager* manager = WSingleton<RFSceneManager>::Instance()->GetManager();
	WRect rect = data_f145f8;
	float width;
	float height;
	if (manager == NULL) {
		width = 800.0f;   // 0xd0562c
		height = 600.0f;  // 0xd05628
	} else {
		RFSize size = manager->GetRootWnd()->GetSize();
		width = size.width;
		height = size.height;
	}
	// The middle row: the first column hit, then the first one missed.
	bool found = false;
	float row = (float)(height * 0.5);
	for (int x = 0; (float)x < width; x++) {
		if (found) {
			if (!Jp_LobbyPlayer_HitTest(WVector((float)x, row, 0.0f), position)) {
				rect.w = (float)x - rect.x;
				break;
			}
		} else if (Jp_LobbyPlayer_HitTest(WVector((float)x, row, 0.0f), position)) {
			found = true;
			rect.x = (float)x;
		}
	}
	if (rect.w == 0.0f) {
		return data_f145f8;
	}
	// The middle column of that span: the rows.
	found = false;
	float column = (float)(rect.w * 0.5 + rect.x);
	for (int y = 0; (float)y < height; y++) {
		if (found) {
			if (!Jp_LobbyPlayer_HitTest(WVector(column, (float)y, 0.0f), position)) {
				rect.h = (float)y - rect.y;
				break;
			}
		} else if (Jp_LobbyPlayer_HitTest(WVector(column, (float)y, 0.0f), position)) {
			found = true;
			rect.y = (float)y;
		}
	}
	if (rect.h == 0.0f) {
		return data_f145f8;
	}
	return rect;
}

// 0x565980 (unplaced, the name in ecx, the string in edx): the window name
// differs from `name` (std::string::compare(0, size, name, strlen(name))).
static bool IsWndNameOther(const char* name, const std::string& wndName) {
	return wndName.compare(0, wndName.size(), name, strlen(name)) != 0;
}

// Whether a Fresh window is the my-room or the shop form, which leave the
// avatar the mouse.
static bool IsAvatarForm(FrWnd* wnd) {
	if (!IsWndNameOther("realmyroom", wnd->GetWndName())) {
		return true;
	}
	return !IsWndNameOther("shopmain", wnd->GetWndName());
}

// 0x5659b0 (unplaced): the scene manager (0xe73ddc) and the input (0xf00510)
// are up; no Fresh modal form other than the my-room / shop ones, no Refresh
// modal, and the Fresh form under the mouse (FrWndManager 0xbe47e0) is none
// or one of those two.
bool Jp_LobbyPlayer_IsMouseFree(const WVector& mouse) {
	if (!WSingleton<RFSceneManager>::IsInstantiated() || g_input == NULL) {
		return false;
	}
	if (g_pFresh != NULL && g_pFresh->GetManager() != NULL) {
		FrWnd* modal = g_pFresh->GetManager()->GetModalForm();
		if (modal != NULL && !IsAvatarForm(modal)) {
			return false;
		}
	}
	RFManager* manager = WSingleton<RFSceneManager>::IsInstantiated() ? WSingleton<RFSceneManager>::Instance()->GetManager() : NULL;
	// Ours: the JP calls IsModalExist on a NULL manager.
	if (manager != NULL && manager->IsModalExist()) {
		return false;
	}
	// Ours: the JP reads g_pFresh's manager here without a test.
	if (g_pFresh == NULL || g_pFresh->GetManager() == NULL) {
		return true;
	}
	FrWnd* form = g_pFresh->GetManager()->GetFormFromPoint(mouse.x, mouse.y);
	if (form == NULL) {
		return true;
	}
	return IsAvatarForm(form);
}

// 0x565a90 (unplaced): the mouse is free and over the avatar.
static bool IsAvatarUnderMouse(const WVector& mouse, const WVector& position) {
	if (!Jp_LobbyPlayer_IsMouseFree(mouse)) {
		return false;
	}
	return Jp_LobbyPlayer_HitTest(mouse, position);
}

// 0x565b40
void LobbyPlayer::OnLoad(void) {
}

// 0x565b50
WRect LobbyPlayer::GetPoint() const {
	return Jp_LobbyPlayerPointFromPosition(m_position);
}

// 0x565f10
void LobbyPlayer::OnAnotherHand(int nWork, void* pData) {
	if (m_hands[0].IsWorking(0)) {
		m_requests[0].m_load();
		m_requests[0].m_built = true;
	}
	if (m_hands[1].IsWorking(0)) {
		m_requests[1].m_load();
		m_requests[1].m_built = true;
	}
}

// 0x566000
void LobbyPlayer::RequestBuild(int slot, int arg, int characterId) {
	if (slot == 2) {
		return;
	}
	if (m_hands[slot].IsWorking(0)) {
		return;
	}
	m_requests[slot].m_characterId = characterId;
	m_requests[slot].m_arg = arg;
	m_requests[slot].m_built = false;
	m_requests[slot].m_requested = true;
}

// 0x566330
void LobbyPlayer::OnDisplay(void) {
	IDummyPlayer* avatar = m_avatar.get();
	if (avatar != NULL && avatar->m_petFrame.get() != NULL) {
		avatar->Display();
	}
}

// 0x566350
void LobbyPlayer::SetPositionFromScenePoint(const char* name) {
	WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->FindPoint(name);
	if (point != NULL) {
		m_position = point->m_pos;
	}
}

// 0x566390
bool LobbyPlayer::AttachAvatar() {
	IDummyPlayer* avatar = m_avatar.get();
	if (avatar == NULL || avatar->m_petFrame.get() == NULL) {
		return false;
	}
	if (!avatar->SetMotionByName(kDefaultPoseName, 0)) {
		return false;
	}
	if (m_loadingSlot != 0 && m_characterId == 0) {
		m_unknown40d = false;
	} else {
		if (m_unknown40d) {
			m_avatar.get()->m_effectHost->PlaySequence(kCharChangeSequence, &m_position);
			g_audio->PlaySfx(kCharChangeSound, NULL, 0, NULL, NULL, 0.5f, 200.0f);
		}
		m_fadeTime = 0.0f;
	}
	m_avatar.get()->SetPosition(&m_position);
	m_avatar.get()->m_motionPlaying = true;
	m_avatar.get()->m_motionHold = true;
	m_avatar.get()->m_petFrame.get()->SetAlpha(m_unknown40d ? 0 : 0xff);
	m_fadeOut = !m_unknown40d;
	m_fadeIn = m_unknown40d;
	Jp_GameState_SetLobbyCameraEnabled(false);
	RFLobbyDesktop* desktop = RFLobbyDesktop::s_instance;
	if (desktop != NULL && desktop->GetLobbyState() == 1) {
		IActor* lobbyBg = AfxGetTask()->GetActor("LobbyBg");
		if (lobbyBg != NULL) {
			lobbyBg << MsgObject(NULL, 0x17f, reinterpret_cast<intptr_t>("DefaultCamera"), 0, 0, 0, 0);
		}
	}
	CSceneManager::Instance()->RegisterElement((eObjType)4, NULL, NULL, m_avatar.get()->m_name.c_str(), 0x18, NULL);
	CSceneManager::Instance()->SetPuppet(m_avatar.get()->m_name.c_str(), m_avatar.get()->m_petFrame.get()->GetBonePet());
	return true;
}

// 0x5665c0
bool LobbyPlayer::ProcessFade(float dt) {
	IDummyPlayer* avatar = m_avatar.get();
	if (avatar == NULL || avatar->m_petFrame.get() == NULL) {
		return false;
	}
	float from = m_fadeOut ? 255.0f : 0.0f;  // 0xcfec5c
	float to = m_fadeOut ? 0.0f : 255.0f;
	float alpha = 0.0f;
	if (m_loadingSlot == 0 || m_characterId != 0) {
		alpha = Jp_EaseInQuartic(from, to, m_fadeTime, 0.7f);  // 0xd0a5dc
		m_fadeTime += dt;
	}
	if (!m_fadeOut) {
		if (!(alpha < to)) {
			alpha = to;
			m_fadeOut = true;  // one word store (0x566686): the fade ends,
			m_fadeIn = false;  // m_fadeIn is cleared with it
		}
	} else if (alpha <= to) {
		alpha = to;
		m_fadeOut = false;  // one word store (0x566673)
		m_fadeIn = false;
	}
	m_avatar.get()->m_petFrame.get()->SetAlpha((unsigned char)(int)alpha);
	return m_fadeOut;
}

// 0x5666d0
unsigned long LobbyPlayer::SetAvatarPart(CPartTidList* parts, unsigned long tid, unsigned long id, bool keepTid) {
	if (m_avatar.get() == NULL || m_avatar.get()->m_petFrame.get() == NULL) {
		return 0;
	}
	if (ItemManager()->FindPart(tid) == NULL) {
		tid = 0;
	}
	SendToLobby(0x53);
	unsigned long result = Jp_PetFrame_SetPart(m_avatar.get()->m_petFrame.get(), parts, tid, id, keepTid);
	if (result != 0) {
		CPetFrame* frame = m_avatar.get()->m_petFrame.get();
		frame->GetBonePet()->xUpdateTnLBuffers(true);
		m_avatar.get()->m_petFrame.get()->CalcBVolume();
		m_avatar.get()->RefreshAfterPartChange(1, 0, 0, true);
		m_avatar.get()->m_petFrame.get()->Refresh(parts);
		// The JP SetHairClr(parts, parts->m_hairClr, keepTid ? tid : 0,
		// true) 0x8af6b0: the KR SetHairClr takes the first two only.
		m_avatar.get()->m_petFrame.get()->SetHairClr(parts, parts->m_hairClr);
		// JP WPuppet +0x6c: the root bone (the KR m_rootbone).
		WBone* root = m_avatar.get()->m_petFrame.get()->GetBonePet()->GetRootBone();
		if (root != NULL) {
			WBone* bone = root->FindBone(Jp_CharacterBoneName(m_avatar.get()->m_typeId), 0);
			if (bone != NULL) {
				bone->SetInvisible(false, true);
			}
		}
	}
	SendToLobby(0x54);
	if (RFLobbyDesktop::s_instance != NULL) {
		Jp_LobbyDesktop_SetAvatarPending(true);
	}
	return result;
}

// 0x5668a0
bool LobbyPlayer::SetAvatarAuxPart(unsigned char index, int tid) {
	bool result = false;
	if (m_avatar.get() == NULL || m_avatar.get()->m_petFrame.get() == NULL) {
		return result;
	}
	if (tid != 0 && ItemManager()->FindAuxPart((unsigned long)tid) == NULL) {
		return false;
	}
	SendToLobby(0x53);
	result = m_avatar.get()->m_petFrame.get()->SetAuxPart(index, (unsigned long)tid);
	if (result) {
		m_avatar.get()->m_petFrame.get()->GetBonePet()->xUpdateTnLBuffers(true);
		m_avatar.get()->m_petFrame.get()->CalcBVolume();
		m_avatar.get()->RefreshAfterPartChange(0, 0, 0, true);
	}
	SendToLobby(0x54);
	if (RFLobbyDesktop::s_instance != NULL) {
		Jp_LobbyDesktop_SetAvatarPending(true);
	}
	return result;
}

// 0x5669f0
bool LobbyPlayer::SetAvatarHairColor(CPartTidList* parts, unsigned char color, unsigned long hairStyleTid) {
	bool result = false;
	if (m_avatar.get() == NULL || m_avatar.get()->m_petFrame.get() == NULL) {
		return result;
	}
	if (hairStyleTid != 0 && ItemManager()->FindHairStyle(hairStyleTid) == NULL) {
		return false;
	}
	SendToLobby(0x53);
	// The JP SetHairClr(parts, color, 0, true) 0x8af6b0: its two extra
	// arguments are the defaults of the KR SetHairClr(parts, color).
	result = m_avatar.get()->m_petFrame.get()->SetHairClr(parts, color);
	if (result) {
		m_avatar.get()->RefreshAfterPartChange(0, 0, 0, true);
		m_avatar.get()->m_petFrame.get()->Refresh(parts);
	}
	SendToLobby(0x54);
	if (RFLobbyDesktop::s_instance != NULL) {
		Jp_LobbyDesktop_SetAvatarPending(true);
	}
	return result;
}

// 0x566b30
bool LobbyPlayer::SetAvatarUccClothes(unsigned long serial, int unused) {
	bool result = false;
	if (m_avatar.get() == NULL || m_avatar.get()->m_petFrame.get() == NULL) {
		return false;
	}
	SendToLobby(0x53);
	sUccClothes* clothes = UccManager()->FindClothes(serial);
	if (clothes != NULL) {
		Jp_PetFrame_SetUccClothes(m_avatar.get()->m_petFrame.get(), clothes);
		result = true;
	}
	SendToLobby(0x54);
	if (RFLobbyDesktop::s_instance != NULL) {
		Jp_LobbyDesktop_SetAvatarPending(true);
	}
	return result;
}

// 0x566c40
void LobbyPlayer::RotateAvatar(float angle) {
	IDummyPlayer* avatar = m_avatar.get();
	if (avatar == NULL) {
		return;
	}
	// Axis 1: y.
	WMatrix rotation = RotMat(angle, 1);
	m_avatar.get()->m_matrix = rotation * avatar->m_matrix;
}

// RFLobbyDesktop 0x55fe60 / 0x565af0 (desktop members, placed next to this class's
// code; written here on GetLobbyState until the desktop header has them):
// the lobby states 5-7, and the states 2 and 5.
static bool IsDesktopState567(RFLobbyDesktop* desktop) {
	int32_t state = desktop->GetLobbyState();
	if (state == 5 || state == 6 || state == 7) {
		return true;
	}
	return false;
}
static bool IsDesktopState25(RFLobbyDesktop* desktop) {
	int32_t state = desktop->GetLobbyState();
	if (state == 2 || state == 5) {
		return true;
	}
	return false;
}

// 0x566cd0
bool LobbyPlayer::ProcessMouse(float dt) {
	if (!g_jpTooltipFollowEnabled) {
		return false;
	}
	int buttons = Jp_RefreshScene_GetMouseButtons();
	bool released = (buttons & 2) != 0;
	if (released) {
		m_dragging = false;
	}
	if (m_paused || m_avatar.get() == NULL) {
		return false;
	}
	if (!WSingleton<RFSceneManager>::IsInstantiated() || !Jp_RefreshScene_HasInput()) {
		return false;
	}
	if (g_pFresh == NULL || g_pFresh->GetManager() == NULL) {
		return false;
	}
	RFLobbyDesktop* desktop = RFLobbyDesktop::s_instance;
	if (desktop != NULL && (desktop->IsShopState() || IsDesktopState567(desktop)) && !IsDesktopState25(desktop)) {
		return false;
	}
	WVector mouse = WSingleton<RFSceneManager>::Instance()->GetMousePosition();
	WVector delta = g_input->GetMouseDelta();
	if (!Jp_RefreshScene_IsOverWindow(mouse.x, mouse.y)) {
		bool hit = false;
		if (!m_dragging) {
			hit = IsAvatarUnderMouse(mouse, m_position);
		}
		if (m_dragging || hit) {
			if (buttons & 1) {
				m_dragging = true;
			}
			if (!m_dragging) {
				if (released) {
					m_dragging = false;
					return true;
				}
				if ((buttons & 4) != 0 && m_avatar.get()->SetMotionByName(kDefaultPoseName, 1)) {
					m_avatar.get()->SetPosition(&m_position);
					m_avatar.get()->m_motionHold = true;
					m_avatar.get()->m_motionPlaying = true;
				}
				Jp_RefreshScene_SetMouseCursor(1, true);
				return true;
			}
			Jp_RefreshScene_SetMouseCursor(0xe, true);
			if ((buttons & 0x100) != 0) {
				// 0xe751b0 (guard 0xe751bc): the previous frame's delta.
				static WVector s_previousDelta(0.0f, 0.0f, 0.0f);
				float average = (float)((s_previousDelta.x + delta.x) * 0.5);  // 0xd66ca0
				s_previousDelta = delta;
				RotateAvatar((float)(average * 0.0069813169));  // 0xd0a3d0: 0.4 degree in radians
			}
			return true;
		}
	}
	if (!Jp_LobbyDesktop_GetCaddieCameraZoom()) {
		Jp_RefreshScene_SetMouseCursor(0, false);
	}
	return false;
}

// Whether the motion's name ends with "끝" (the inlined strlen + _stricmp
// of 0x567050 and 0x567140).
static bool IsMotionEnded(const w_motion_data* motion) {
	const char* name = motion->name;
	return _stricmp(kMotionEndSuffix, name + strlen(name) - 2) == 0;
}

// 0x567050
bool LobbyPlayer::NotifyMotionEnd() {
	IDummyPlayer* avatar = m_avatar.get();
	if (avatar == NULL || m_avatarTid == 0) {
		return false;
	}
	int ended = 1;
	if (avatar->m_motionPlaying) {
		ended = IsMotionEnded(avatar->m_motionState) ? 1 : 0;
	}
	IActor* lobby = AfxGetTask()->GetActor("Lobby");
	if (lobby != NULL) {
		lobby << MsgObject(NULL, 0x195, ended, 0, 0, 0, 0);
	}
	return true;
}

// 0x567140
bool LobbyPlayer::PlayIdleMotion() {
	if (!m_motionFlag) {
		return false;
	}
	IDummyPlayer* avatar = m_avatar.get();
	if (avatar == NULL || avatar->m_petFrame.get() == NULL || m_avatarTid == 0) {
		return false;
	}
	if (IsMotionEnded(avatar->m_motionState)) {
		int charTypeId = (int)((uint32_t)m_avatarTid & 0x3ffffff);
		int count = JpMotionTable_GetCount(charTypeId);
		int index = rand() % count;
		const char* motion = JpMotionTable_GetMotion(charTypeId, index);
		unsigned int rule = 5;
		bool allowed = true;
		if (JpPartMotions_Contains(motion, rule)) {
			CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
			allowed = Jp_MotionPartRule_Check((uint32_t)m_avatarTid, &doc->GetMyPartTidList(), rule) != 0;
		}
		if (allowed && m_avatar.get()->m_petFrame.get()->FindMotionData(motion, true, NULL) != NULL) {
			m_avatar.get()->PlayMotion(motion);
		}
	}
	return true;
}

// 0x567280
bool LobbyPlayer::HasNewAvatar() {
	EnterCriticalSection(&m_lock);
	IDummyPlayer* avatar = m_newAvatar.get();
	LeaveCriticalSection(&m_lock);
	return avatar != NULL;
}

// 0x567a20
bool LobbyPlayer::BuildSelf(int characterId, uint32_t tid) {
	if (HasNewAvatar()) {
		return false;
	}
	IDummyPlayer* avatar = new IDummyPlayer(2);
	if (avatar == NULL) {
		return false;
	}
	avatar->m_petFrame = RF_SHARED_PTR<CPetFrame>(static_cast<CPetFrame*>(NULL));
	if (Jp_GameState_IsOfflinePlay()) {
		// The random dress-up: a random character (rand() % 15) in a locally
		// filled record.
		sCharacterInfo info;
		avatar->m_typeId = rand() % 15;
		avatar->RandomDressUp(&info);
		avatar->m_name = MakeStr("LOBBY_PLAYER_%d_%d", Jp_GameState_AvatarNameKey(), avatar->m_typeId);
		if (!avatar->BuildParts(info, false, true)) {
			delete avatar;
			return false;
		}
		EnterCriticalSection(&m_lock);
		m_builtCharacterUid = 0;
		m_builtCharacterTid = avatar->m_typeId | 0x4000000;
		LeaveCriticalSection(&m_lock);
	} else {
		sCharacterInfo* info = Jp_GameState_GetCharacterInfo(characterId == -1 ? 0 : characterId);
		if (info != NULL) {
			avatar->m_typeId = info->typeId & 0x3ffffff;
			avatar->m_name = MakeStr("LOBBY_PLAYER_%d_%d", Jp_GameState_AvatarNameKey(), avatar->m_typeId);
			if (!avatar->BuildParts(*info, true, true)) {
				delete avatar;
				return false;
			}
			EnterCriticalSection(&m_lock);
			m_builtCharacterUid = (int)info->id;
			m_builtCharacterTid = info->typeId;
			LeaveCriticalSection(&m_lock);
		} else {
			if (tid == 0) {
				delete avatar;
				return false;
			}
			avatar->m_typeId = tid & 0x3ffffff;
			avatar->m_name = MakeStr("LOBBY_PLAYER_%d_%d", Jp_GameState_AvatarNameKey(), avatar->m_typeId);
			CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
			doc->BuildMyPartTidListDefault(tid);
			if (!avatar->BuildFromPartTidList(doc->GetMyPartTidList(), doc->GetMyAuxPartTidList(), true)) {
				delete avatar;
				return false;
			}
			EnterCriticalSection(&m_lock);
			m_builtCharacterUid = 0;
			m_builtCharacterTid = tid;
			LeaveCriticalSection(&m_lock);
		}
	}
	EnterCriticalSection(&m_lock);
	m_matrix = avatar->m_matrix;
	m_newAvatar = RF_SHARED_PTR<IDummyPlayer>(avatar);
	m_avatarBuilt[0] = true;
	LeaveCriticalSection(&m_lock);
	return true;
}

// 0x567e10
bool LobbyPlayer::BuildOther(uint32_t tid) {
	if (tid == 0) {
		return false;
	}
	if (HasNewAvatar()) {
		return false;
	}
	IDummyPlayer* avatar = new IDummyPlayer(2);
	if (avatar == NULL) {
		return false;
	}
	avatar->m_petFrame = RF_SHARED_PTR<CPetFrame>(static_cast<CPetFrame*>(NULL));
	int uid = Jp_GameState_FindCharacterUid(tid);
	if (uid == -1) {
		uid = 0;
	}
	sCharacterInfo* info = Jp_GameState_GetCharacterInfo(uid);
	if (info != NULL) {
		avatar->m_typeId = info->typeId & 0x3ffffff;
		avatar->m_name = MakeStr("LOBBY_PLAYER_%d_%d", Jp_GameState_AvatarNameKey(), avatar->m_typeId);
		if (!avatar->BuildParts(*info, true, true)) {
			delete avatar;
			return false;
		}
		EnterCriticalSection(&m_lock);
		m_builtCharacterUid = (int)info->id;
		m_builtCharacterTid = info->typeId;
		LeaveCriticalSection(&m_lock);
	} else {
		avatar->m_typeId = tid & 0x3ffffff;
		avatar->m_name = MakeStr("LOBBY_PLAYER_%d_%d", Jp_GameState_AvatarNameKey(), avatar->m_typeId);
		CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
		doc->BuildMyPartTidListDefault(tid);
		if (!avatar->BuildFromPartTidList(doc->GetMyPartTidList(), doc->GetMyAuxPartTidList(), true)) {
			delete avatar;
			return false;
		}
		EnterCriticalSection(&m_lock);
		m_builtCharacterTid = tid;
		m_builtCharacterUid = uid;
		LeaveCriticalSection(&m_lock);
	}
	EnterCriticalSection(&m_lock);
	m_newAvatar = RF_SHARED_PTR<IDummyPlayer>(avatar);
	m_avatarBuilt[1] = true;
	LeaveCriticalSection(&m_lock);
	return true;
}

// 0x5680b0
void LobbyPlayer::ResetAvatar() {
	if (m_avatar.get() != NULL) {
		CSceneManager::Instance()->DeleteElement(m_avatar.get()->m_name.c_str());
		m_avatar.reset();
		m_avatar = RF_SHARED_PTR<IDummyPlayer>(static_cast<IDummyPlayer*>(NULL));
	}
	m_characterId = 0;
	m_avatarTid = 0;
	m_rebuild[0] = false;
	m_requests[0].m_built = true;
	if (m_hands[0].IsWorking(0)) {
		m_hands[0].WaitUntilWorkDone(0xffffffff);
		Jp_ReleaseThreadResources(m_hands[0].GetThreadID());
	}
}

// 0x568230
void LobbyPlayer::ClearPreview() {
	if (m_avatar.get() != NULL) {
		CSceneManager::Instance()->DeleteElement(m_avatar.get()->m_name.c_str());
		m_avatar.reset();
		m_avatar = RF_SHARED_PTR<IDummyPlayer>(static_cast<IDummyPlayer*>(NULL));
	}
	if (m_avatarTid != 0) {
		CSharedDoc* doc = WSingleton<CSharedDoc>::Instance();
		doc->BuildMyPartTidListDefault(m_avatarTid);
		// JP 0x63dfa0: the KR CSharedDoc::BuildMyPartTidList(void) (the
		// equipped character's record, else the default combo; same body).
		doc->BuildMyPartTidList();
	}
	m_characterId = 0;
	m_avatarTid = 0;
	m_rebuild[1] = false;
	m_requests[1].m_built = true;
	if (m_hands[1].IsWorking(0)) {
		m_hands[1].WaitUntilWorkDone(0xffffffff);
	}
	Jp_ReleaseThreadResources(m_hands[1].GetThreadID());
}

// 0x568480
bool LobbyPlayer::Build(int mode, int a, uint32_t b) {
	switch (mode) {
	case 0:
		return BuildSelf(a, b);
	case 1:
		return BuildOther(b);
	}
	return false;
}

// 0x5684b0
bool LobbyPlayer::TakeNewAvatar() {
	bool result = HasNewAvatar();
	if (result) {
		if (m_avatar.get() != NULL) {
			CSceneManager::Instance()->DeleteElement(m_avatar.get()->m_name.c_str());
		}
		EnterCriticalSection(&m_lock);
		m_avatar = m_newAvatar;
		m_newAvatar = RF_SHARED_PTR<IDummyPlayer>(static_cast<IDummyPlayer*>(NULL));
		m_avatarTid = (int)m_builtCharacterTid;
		m_builtCharacterTid = 0;
		m_characterId = m_builtCharacterUid;
		m_builtCharacterUid = 0;
		for (int i = 0; i < 2; i++) {
			m_rebuild[i] = m_avatarBuilt[i];
			m_avatarBuilt[i] = false;
		}
		LeaveCriticalSection(&m_lock);
	}
	return result;
}

// 0x568600
void LobbyPlayer::OnProcess(float dt) {
	for (int i = 0; i < 2; i++) {
		if (m_requests[i].m_requested) {
			m_requests[i].m_requested = false;
			if (!m_hands[i].IsWorking(0)) {
				m_hands[i].Work(this, 0, NULL, true, -1);
			}
		}
	}
	TakeNewAvatar();
	if (m_avatar.get() != NULL && m_avatar.get()->m_petFrame.get() != NULL) {
		if (m_rebuild[0]) {
			AttachAvatar();
			m_rebuild[0] = false;
		} else if (m_rebuild[1]) {
			AttachAvatar();
			m_rebuild[1] = false;
		}
	}
	if (m_avatar.get() != NULL) {
		m_avatar.get()->Process(dt);
		NotifyMotionEnd();
		PlayIdleMotion();
		bool zoom = ProcessMouse(dt);
		if (RFLobbyDesktop::s_instance != NULL) {
			Jp_LobbyDesktop_SetShopCameraZoom(zoom);
		}
	}
	if (m_fadeIn) {
		ProcessFade(dt);
	}
}

// 0x568730
void LobbyPlayer::LoadSlot(int slot) {
	SendToLobby(0x53);
	m_loadingSlot = slot;
	Build(slot, m_requests[slot].m_characterId, (uint32_t)m_requests[slot].m_arg);
	m_requests[slot].m_characterId = -1;
	m_requests[slot].m_arg = 0;
	SendToLobby(0x54);
}

// 0x568860
bool LobbyPlayer::LoadSelf() {
	if (m_requests[0].m_built) {
		return false;
	}
	LoadSlot(0);
	Sleep(15);
	SendToLobby(0x57);
	return true;
}

// 0x5688e0
bool LobbyPlayer::LoadOther() {
	if (m_requests[1].m_built) {
		return false;
	}
	LoadSlot(1);
	Sleep(15);
	SendToLobby(0x57);
	return true;
}

// 0x568c10
void LobbyPlayer::OnPreLoadInit(void) {
	m_requests[0].m_load = std::bind(&LobbyPlayer::LoadSelf, this);  // bind 0x566270, function assign 0x567930
	m_requests[0].m_built = false;
	m_requests[0].m_characterId = -1;
	m_requests[0].m_arg = 0;
	m_requests[1].m_load = std::bind(&LobbyPlayer::LoadOther, this);
	m_requests[1].m_built = false;
	m_requests[1].m_arg = 0;
	m_requests[1].m_characterId = -1;
}

// 0x568cc0
void JpLobbyPlayerTable::Clear() {
	m_map.clear();
}

// 0x568dc0
void LobbyPlayer::OnDestroy(void) {
	ResetAvatar();
	m_position = WVector::ZERO;
	m_table2c.Clear();
	m_table3c.Clear();
}

// 0x5692e0 (scalar deleting destructor; body 0x568ee0): the members (the
// two hands, the two requests, the two maps, the two avatar pointers) and
// IActor are destroyed implicitly.
LobbyPlayer::~LobbyPlayer() {
	DeleteCriticalSection(&m_lock);
}

// 0x5690c0
LobbyPlayer::LobbyPlayer()
	: m_newAvatar(static_cast<IDummyPlayer*>(NULL)),
	  m_avatar(static_cast<IDummyPlayer*>(NULL)),
	  m_builtCharacterUid(0),
	  m_builtCharacterTid(0),
	  m_characterId(0),
	  m_avatarTid(0),
	  m_position(WVector::ZERO) {
	m_fadeTime = 0.0f;
	m_fadeOut = false;
	m_fadeIn = false;
	m_loadingSlot = 0;
	// 0xf04160: zeros in the file, filled at startup; WMatrix::IDENTITY is
	// assumed (the static next to WVector::ZERO 0xf04154), not proven.
	m_matrix = WMatrix::IDENTITY;
	m_unknown40c = true;
	m_unknown40d = true;
	m_motionParam = 0;
	m_motionFlag = false;
	m_paused = false;
	m_dragging = false;
	InitializeCriticalSectionAndSpinCount(&m_lock, 2000);
	for (int i = 0; i < 2; i++) {
		m_avatarBuilt[i] = false;
		m_rebuild[i] = false;
	}
}

// 0x569310
IObject* LobbyPlayerMakeInstance(void) {
	return new LobbyPlayer;
}

// 0x569380: the registration object's constructor (the KR
// IMPLEMENT pattern of LobbyBg.cpp: ObjectFactory 0xa4c0d0 /
// AddObjectFunctor 0xa4bcf0, ActorFactory 0xa471a0 / AddActorClass 0xa46aa0).
struct __impLobbyPlayer {
	__impLobbyPlayer(void) {
		ObjectFactory().AddObjectFunctor(LobbyPlayerMakeInstance, "LobbyPlayer");
		ActorFactory().AddActorClass("LobbyPlayer");
	}
};
static __impLobbyPlayer s_impLobbyPlayer;

// 0x5693c0
void LobbyPlayer::HandleMsg(const MsgObject& msg) {
	switch (msg.m_msg) {
	case 0x85: {
		bool noParam = msg.m_param1 == 0;
		ResetAvatar();
		static_cast<IActor*>(this) << MsgObject(NULL, 0x185, 0, 0, !noParam, 0, 0);
		break;
	}
	case 0x184:
		OnDestroy();
		break;
	// 0x185: build the local player's avatar. param1 (int*) the character
	// (-1 or none: the equipped one), param2 (int*) the build argument.
	case 0x185: {
		if (WisEqual(m_position, WVector::ZERO, 1e-5f)) {
			SetPositionFromScenePoint(kStartPointName);
		}
		int characterId = msg.m_param1 != 0 ? *reinterpret_cast<const int*>(msg.m_param1) : 0;
		int arg = msg.m_param2 != 0 ? *reinterpret_cast<const int*>(msg.m_param2) : 0;
		bool flag = msg.m_param3 == 0;
		if (characterId == -1) {
			characterId = 0;
		}
		if (characterId == 0 && arg == 0) {
			// JP CSharedDoc +0xa2a: the KR m_myCharacterId.
			characterId = (int)WSingleton<CSharedDoc>::Instance()->m_myCharacterId;
		}
		if (m_characterId == characterId) {
			break;
		}
		ClearPreview();
		m_unknown40d = flag;
		RequestBuild(0, arg, characterId);
		break;
	}
	// 0x186: stop the avatar's motion and drop it.
	case 0x186:
		if (m_avatar.get() != NULL) {
			m_avatar.get()->m_motionHold = false;
			m_avatar.get()->m_motionPlaying = false;
		}
		ResetAvatar();
		break;
	// 0x187: build the preview (slot 1). param1 (int*) the character.
	case 0x187: {
		int characterId = msg.m_param1 != 0 ? *reinterpret_cast<const int*>(msg.m_param1) : 0;
		bool flag = msg.m_param2 == 0;
		int previous = m_avatarTid;
		if (previous == characterId) {
			break;
		}
		if (characterId == 0) {
			characterId = previous;
		}
		if (previous != 0) {
			ClearPreview();
		}
		m_unknown40d = flag;
		RequestBuild(1, characterId, 0);
		break;
	}
	case 0x188:
		ClearPreview();
		break;
	case 0x189:
		ClearPreview();
		m_table3c.Clear();
		break;
	// 0x192: the lobby character carousel's change (param1 the character id,
	// RFLobbyCharacterSelectWnd).
	case 0x192: {
		int characterId = (int)msg.m_param1;
		static_cast<IActor*>(this) << MsgObject(NULL, 0x186, 0, 0, 0, 0, 0);
		// JP CSharedDoc +0xa2a: the KR m_myCharacterId.
		WSingleton<CSharedDoc>::Instance()->m_myCharacterId = characterId;
		static_cast<IActor*>(this) << MsgObject(NULL, 0x185, 0, 0, 0, 0, 0);
		Jp_LobbyViewReset(0);
		IActor* lobby = AfxGetTask()->GetActor("Lobby");
		if (lobby != NULL) {
			lobby << MsgObject(NULL, 0x1a0, 0, 0, 0, 0, 0);
		}
		break;
	}
	case 0x18a: {
		int arg2 = msg.m_param2 != 0 ? *reinterpret_cast<const int*>(msg.m_param2) : 0;
		int arg3 = msg.m_param3 != 0 ? *reinterpret_cast<const int*>(msg.m_param3) : 0;
		bool flag = msg.m_param5 != 0 ? *reinterpret_cast<const bool*>(msg.m_param5) : true;
		int* result = reinterpret_cast<int*>(msg.m_param4);
		unsigned long value = SetAvatarPart(reinterpret_cast<CPartTidList*>(msg.m_param1), (unsigned long)arg2, (unsigned long)arg3, flag);
		if (result != NULL) {
			*result = value;
		}
		break;
	}
	case 0x18b: {
		const uint32_t* tid = reinterpret_cast<const uint32_t*>(msg.m_param1);
		int* result = reinterpret_cast<int*>(msg.m_param3);
		bool value = SetAvatarAuxPart((unsigned char)((*tid >> 21) & 0x1f), msg.m_param2 != 0 ? (int)*tid : 0);
		if (result != NULL) {
			*result = value;
		}
		break;
	}
	case 0x18c: {
		int arg3 = msg.m_param3 != 0 ? *reinterpret_cast<const int*>(msg.m_param3) : 0;
		int* result = reinterpret_cast<int*>(msg.m_param4);
		bool value = SetAvatarHairColor(reinterpret_cast<CPartTidList*>(msg.m_param1), *reinterpret_cast<const unsigned char*>(msg.m_param2), (unsigned long)arg3);
		if (result != NULL) {
			*result = value;
		}
		break;
	}
	case 0x18d: {
		int* result = reinterpret_cast<int*>(msg.m_param3);
		bool value = SetAvatarUccClothes((unsigned long)msg.m_param1, *reinterpret_cast<const int*>(msg.m_param2));
		if (result != NULL) {
			*result = value;
		}
		break;
	}
	case 0x1ce:
	case 0x1cf: {
		IDummyPlayer* avatar = m_avatar.get();
		if (avatar != NULL && avatar->m_petFrame.get() != NULL) {
			avatar->m_petFrame.get()->SetAlpha(0xff);
		}
		break;
	}
	// 0x198: answers the avatar's CPetFrame in the message's param1.
	case 0x198: {
		IDummyPlayer* avatar = m_avatar.get();
		if (avatar != NULL && avatar->m_petFrame.get() != NULL) {
			const_cast<MsgObject&>(msg).m_param1 = reinterpret_cast<intptr_t>(avatar->m_petFrame.get());
		}
		break;
	}
	// 0x199: play the motion named by param1.
	case 0x199: {
		const char* name = reinterpret_cast<const char*>(msg.m_param1);
		if (name == NULL || m_avatar.get() == NULL) {
			break;
		}
		std::string motion(name);
		m_avatar.get()->PlayMotion(motion.c_str());
		m_avatar.get()->m_motionPlaying = true;
		m_motionParam = (int)msg.m_param2;
		m_motionFlag = msg.m_param3 != 0;
		break;
	}
	// 0x19a: back to the default pose unless already in it.
	case 0x19a: {
		if (m_avatar.get() == NULL) {
			break;
		}
		std::string motion(kDefaultPoseName);
		if (m_avatar.get()->m_motionState->name != motion) {
			m_avatar.get()->PlayMotion(motion.c_str());
			m_avatar.get()->m_motionPlaying = true;
		}
		break;
	}
	case 0x190: {
		int* result = reinterpret_cast<int*>(msg.m_param1);
		if (result != NULL) {
			*result = m_avatarTid;
		}
		break;
	}
	case 0x194: {
		WRect* result = reinterpret_cast<WRect*>(msg.m_param1);
		if (result != NULL) {
			*result = GetPoint();
		}
		break;
	}
	case 0x2ae:
		m_paused = true;
		break;
	case 0x2af:
		m_paused = false;
		break;
	}
}

// 0x8b28b0 (JP CPetFrame, unplaced): the KR CPetFrame::SetPart (hide mask
// off, m_partList.SetPart, SetPartPets, hair and shirt colours, the bone
// work's end, hide mask on) with the JP four-argument SetHairClr(0,
// parts->m_hairClr, keepTid ? tid : 0, true) 0x8af6b0 in place of the KR
// SetHairClr(0, parts->m_hairClr): the KR CPetFrame has no hair colour
// with a part tid, so the port drops tid / keepTid there (as the caller
// 0x5666d0 does).
unsigned long Jp_PetFrame_SetPart(CPetFrame* frame, CPartTidList* parts, unsigned long tid, unsigned long id, bool keepTid) {
	(void)keepTid;
	return frame->SetPart(parts, tid, id);
}

// 0xb55a00
void Jp_ReleaseThreadResources(unsigned long threadId) {
	g_resrcmng->UnlockAllByThread(threadId);
}

// Ours: 0x566cd0 calls RefreshInputHandler::PollMouse (0x5f3b90) on the
// scene manager's handler; the JP calls it on a NULL handler without a
// scene manager.
int Jp_RefreshScene_GetMouseButtons() {
	RFSceneManager* scene = WSingleton<RFSceneManager>::IsInstantiated() ? WSingleton<RFSceneManager>::Instance() : NULL;
	if (scene == NULL || scene->GetInput() == NULL) {
		return 0;
	}
	return scene->GetInput()->PollMouse();
}

// Ours: the input handler test 0x566cd0 makes inline on the instance.
bool Jp_RefreshScene_HasInput() {
	return WSingleton<RFSceneManager>::Instance()->GetInput() != NULL;
}

// Ours: 0x566cd0 calls RFSceneManager::SetMouseCursor on the instance.
void Jp_RefreshScene_SetMouseCursor(int cursor, bool lock) {
	WSingleton<RFSceneManager>::Instance()->SetMouseCursor(cursor, lock);
}

// Ours: OnProcess (0x568600) stores the mouse handler's result in the
// desktop's +0x244 inside its s_instance != NULL test.
void Jp_LobbyDesktop_SetShopCameraZoom(bool value) {
	RFLobbyDesktop::s_instance->m_unknown244 = value;
}

// Ours: the desktop's +0x258; the callers test s_instance first.
void Jp_LobbyDesktop_SetAvatarPending(bool value) {
	RFLobbyDesktop::s_instance->m_avatarPending = value;
}

// Ours: 0x566dee reads the desktop's +0x245 without a test.
bool Jp_LobbyDesktop_GetCaddieCameraZoom() {
	return RFLobbyDesktop::s_instance != NULL && RFLobbyDesktop::s_instance->m_unknown245;
}

// Ours: the caddie's OnProcess (0x560d70) stores its mouse handler's result
// in the desktop's +0x245 inside its s_instance != NULL test.
void Jp_LobbyDesktop_SetCaddieCameraZoom(bool value) {
	RFLobbyDesktop::s_instance->m_unknown245 = value;
}

// Ours: the caddie mouse handler (0x5604b0) reads the desktop's +0x244
// before resetting the cursor.
bool Jp_LobbyDesktop_GetShopCameraZoom() {
	return RFLobbyDesktop::s_instance != NULL && RFLobbyDesktop::s_instance->m_unknown244;
}
